#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>

#include "objects.h"
#include "world.h"
#include "serv.hpp"

using namespace std;

#define PORT "8080"  // the port users will be connecting to
#define BACKLOG 10     // how many pending connections queue will hold
#define ANZCONNECTIONS 10

struct conPlayer {
    struct player    me;
    struct bullet    my_bullets[MY_BULLET_MAX];
    unsigned int progress;
    /* indices */
    int my_bullet_i;
};

map<int, struct conPlayer*> players;
map<int, struct conPlayer*>::iterator playersIt;

struct world coopW;


void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void* createServ(void*)
{
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	struct timeval tv;
	memset(&tv, 0, sizeof(struct timeval));
	tv.tv_sec = 5;
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
//    hints.ai_flags = AI_PASSIVE; // use my IP
	if ((rv = getaddrinfo("127.0.0.1", PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return NULL;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		return NULL;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");


	while(1) {  // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		} else {
			printf("Connection accepted\n");
		}

		players[new_fd] = new conPlayer;

		sleep(2);
		acceptNewPlayer(new_fd);
		sleep(2);
		update_players();

		if (players.size() < 2) {
			printf("Erstelle thread\n");
			pthread_t t;

			pthread_create(&t, NULL, recv_world, (void*)&new_fd);
		}
	}
	sleep(1);
	return NULL;
}

int send_world(struct world* w, int fd)
{
	size_t size = WORLD_SIZE;
	int ret;
	char *data = (char*)malloc(size);
	memcpy(data, &(w->me), sizeof(struct player));
//	char *sE = &(data[sizeof(struct player)]);
//	int i = 0;
//	for (i = 0; i < ENEMY_MAX; i++) {
//		memcpy((void*)(&sE[sizeof(struct enemy)*i]), (void*)&(w->enemies[i]), sizeof(struct enemy));
//
//	}
//	sE = &(sE[(sizeof(struct enemy)*ENEMY_MAX)]);
//	for (i = 0; i < MY_BULLET_MAX; i++) {
//		memcpy((void*)(&sE[sizeof(struct bullet)*i]), (void*)&(w->my_bullets[i]), sizeof(struct bullet));
//
//	}
//	sE = &(sE[(sizeof(struct bullet)*MY_BULLET_MAX)]);
//	for (i = 0; i < MY_BULLET_MAX; i++) {
//		memcpy((void*)(&sE[sizeof(struct bullet)*i]), (void*)&(w->mate_bullets[i]), sizeof(struct bullet));
//
//	}
	ret = send(fd, data, size, 0);
	free(data);
	return ret;
}

void* recv_world(void *fdd)
{
	int fd = *((int*)fdd);
	setsockopt(fd, SOL_SOCKET, SOCK_NONBLOCK, 0, 0);
	fd_set fds;
	FD_ZERO(&fds);
	size_t size = CLIENT_WORLD_SIZE;
	char *buf;
	struct timeval tv;
	ssize_t anz = 0;
	char *sE;
	int what, len, seli = 0;
	while (1) {
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		for (playersIt = players.begin(); playersIt != players.end(); playersIt++) {
			FD_SET(playersIt->first, &fds);
		}
		seli = select(10, &fds, NULL, NULL, &tv);
		for (playersIt = players.begin(); playersIt != players.end(); playersIt++) {
			if (FD_ISSET(playersIt->first, &fds)) {
				anz = recv(playersIt->first, &len, sizeof(int), 0);
				if (anz > 0) {
					buf = new char[len];
					anz = recv(playersIt->first, buf, len, 0);
					memcpy(&what, buf, sizeof(char));
					sE = &(buf[sizeof(char)]);
					switch (what) {
					case CLIENT_UPDATE:
						memcpy(&playersIt->second->me, sE, sizeof(struct player));
//						printf("Spieler %d an pos x:%d, y:%d\n", playersIt->first, playersIt->second->me.x, playersIt->second->me.y);
						break;
					}
//					memcpy(&(playersIt->second->me), sE, sizeof(struct player));
//					sE = &(buf[sizeof(struct player)]);
//					memcpy(&(playersIt->second->my_bullets), sE, sizeof(struct bullet)*MY_BULLET_MAX);
					delete buf;
				} else {
					delete playersIt->second;
					players.erase(playersIt);
				}
			}
		}
		FD_ZERO(&fds);
	}
}

struct world* get_world()
{
	return &coopW;
}

map<int, struct conPlayer*>* get_players()
{
	return &players;
}

void acceptNewPlayer(int fd)
{
	int size = sizeof(int)+sizeof(char)+sizeof(int);
	int actSize = size-sizeof(int);
	char what = NEW_PLAYER;
	char *data = (char*)malloc(size);
	char *sE;
	memcpy(data, &actSize, sizeof(int));
	sE = &data[sizeof(int)];
	memcpy(sE, &what, sizeof(char));
	sE = &sE[sizeof(char)];
	memcpy(sE, &fd, sizeof(int));
	send(fd, data, size, 0);
}

void update_players()
{
	int size = sizeof(int)+sizeof(char)+sizeof(char)+(sizeof(int)*players.size())+(sizeof(struct player)*players.size());
	int actSize = size-sizeof(int);
	char what = UPDATE_PLAYERS;
	char *data = (char*)malloc(size);
	char *sE;
	char anzPlayer = players.size();
	int i = 0;
	memcpy(data, &actSize, sizeof(int));
	sE = &data[sizeof(int)];
	memcpy(sE, &what, sizeof(char));
	sE = &sE[sizeof(char)];
	memcpy(sE, &anzPlayer, sizeof(char));
	sE = &sE[sizeof(char)];
	for (playersIt = players.begin(); playersIt != players.end(); playersIt++) {
		memcpy(sE, &playersIt->first, sizeof(int));
		sE = &(sE[sizeof(int)]);
		memcpy(sE, &playersIt->second->me, sizeof(struct player));
		sE = &(sE[sizeof(struct player)]);
		i++;
	}
	for (playersIt = players.begin(); playersIt != players.end(); playersIt++) {
		send(playersIt->first, data, size, 0);
	}
}
