#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <map>

#include "objects.h"
#include "world.h"
#include "client.hpp"

#define PORT "8080" // the port client will be connecting to

#define MAXDATASIZE 100 // max number of bytes we can get at once

using namespace std;

struct conPlayer {
    struct player    me;
    struct bullet    my_bullets[MY_BULLET_MAX];
    unsigned int progress;
    /* indices */
    int my_bullet_i;
};

int me;
map<int, struct conPlayer*> players;
map<int, struct conPlayer*>::iterator playersIt;

struct world client_coopW;

int connectToServ(char* ip)
{
	int sockfd, numbytes;
	char buf[WORLD_SIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(ip, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(1);
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}

		break;
	}
	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		exit(2);
	}

	freeaddrinfo(servinfo); // all done with this structure

	pthread_t t;

	pthread_create(&t, NULL, client_recv_world, (void*)&sockfd);

	sleep(1);
	return sockfd;
}

int client_send_world(struct world* w, int fd)
{
	size_t size = CLIENT_WORLD_SIZE+sizeof(int);
	char what = CLIENT_UPDATE;
	int actSize = size-sizeof(int);
	int ret, i, a;
	char *data = (char*)malloc(size);
	char *sE;
	memcpy(data, &actSize, sizeof(int));
	sE = &(data[sizeof(int)]);
	memcpy(sE, &what, sizeof(char));
	sE = &(sE[sizeof(char)]);
	memcpy(sE, &(w->me), sizeof(struct player));
//	char *sE = &(data[sizeof(struct player)]);
//	for (i = 0, a = 0; i < MY_BULLET_MAX; i++) {
//		memcpy((void*)(&sE[sizeof(struct bullet)*i]), (void*)&(w->my_bullets[i]), sizeof(struct bullet));
//		if (w->my_bullets[i].shot == 1) {
//			a++;
//			w->my_bullets[i].shot = 0;
//		}
//	}
//	io_printf(10, 10, "%d schuesse uebertragen", a);
	ret = send(fd, data, size, 0);
	free(data);
	return ret;
}

void* client_recv_world(void *fdd)
{
	int fd = *((int*)fdd);
	size_t size = 1024;
	char what;
	char anzPlayer;
	int who;
	int len;
	int i;
	char *data;
	struct timeval tv;
	tv.tv_sec = 60;
	tv.tv_usec = 0;
	ssize_t anz = 0;
	char *sE;
	map<int, struct conPlayer*>::iterator tmpIt;
	sleep(5);
	while (1) {
		anz = recv(fd, &len, sizeof(int), 0);
		if (anz > 0) {
			data = new char[len];
			anz = recv(fd, data, len, 0);
			memcpy(&what, data, sizeof(char));
			sE = &(data[sizeof(char)]);
			switch (what) {
			case NEW_PLAYER:
				memcpy(&me, sE, sizeof(int));
				printf("ICH BIN ID %d\n", me);
				break;
			case UPDATE_PLAYERS:
				memcpy(&anzPlayer, sE, sizeof(char));
				printf("anzPlaya %d\n", anzPlayer);
				sE = &(sE[sizeof(char)]);
				for (i = 0; i < anzPlayer; i++) {
					memcpy(&who, sE, sizeof(int));
					sE = &(sE[sizeof(int)]);
					printf("Bekomme id %d\n", who);
					if (who == me) {
//						memcpy(&(client_coopW.me), sE, sizeof(struct player));
					} else {
						tmpIt = players.find(who);
						if (tmpIt != players.end()) {
							memcpy(&(tmpIt->second->me), sE, sizeof(struct player));
						} else {
							players[who] = new struct conPlayer;
							memcpy(&(players[who]->me), sE, sizeof(struct player));
						}
						printf("mate pos x:%d, y:%d\n", players[who]->me.x, players[who]->me.y);
					}
					sE = &sE[sizeof(struct player)];
				}
				break;
			}
			delete data;
//			sE = &(buf[sizeof(struct player)]);
//			memcpy(&(client_coopW.enemies), sE, sizeof(struct enemy)*ENEMY_MAX);
//			sE = &(sE[sizeof(struct enemy)*ENEMY_MAX]);
//			memcpy(&(client_coopW.mate_bullets), sE, sizeof(struct bullet)*MY_BULLET_MAX);
//			sE = &(sE[sizeof(struct bullet)*MY_BULLET_MAX]);
//			memcpy(&(client_coopW.my_bullets), sE, sizeof(struct bullet)*MY_BULLET_MAX);
		} else {
			printf("error recv client\n");
			exit(0);
		}

	}
	return NULL;
}

struct world* client_get_world()
{
	return &client_coopW;
}

map<int, struct conPlayer*>* client_get_players()
{
	return &players;
}
