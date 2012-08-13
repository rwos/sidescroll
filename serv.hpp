#ifndef SERV_H_
#define SERV_H_

#include <map>

void* createServ(void*);
int send_world(struct world* w, int fd);
void* recv_world(void *fdd);
struct world* get_world();
std::map<int, struct conPlayer*>* get_players();
void acceptNewPlayer(int fd);
void update_players();

#endif /* SERV_H_ */
