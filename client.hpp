#ifndef CLIENT_H_
#define CLIENT_H_

#include <map>

int connectToServ(char* ip);
int client_send_world(struct world* w, int fd);
void* client_recv_world(void *fdd);
struct world* client_get_world();
std::map<int, struct conPlayer*>* client_get_players();

#endif /* CLIENT_H_ */
