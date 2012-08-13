#ifndef CONFIG_H
#define CONFIG_H

#define GAME_NAME "SIDE-SCROLLING SPACE SHOOTER"

#define MAX_FPS 33

#define STAR_BANNER_SPEED 2
#define STAR_BANNER_PARALAX 3
#define STAR_BANNER_X_STARS 40
#define STAR_BANNER_Y_STARS 20

/* max time between hits to increase the point multiplier, in ms */
#define POINTS_MULTI_TIME 1000

#define POINTS_PER_HIT 23
#define DAMAGE 40
#define MAX_WEAPON_HEAT 100

#define LIVE_COST 10000

#define ENEMY_BULLET_SPEED 3

/* maximum number of objects */
#define EXPLOSION_MAX 50
#define ENEMY_MAX 10
#define MY_BULLET_MAX 50
#define ENEMY_BULLET_MAX (MY_BULLET_MAX * ENEMY_MAX)


#define CLIENT_WORLD_SIZE (sizeof(struct player))+sizeof(char)
//+(sizeof(struct bullet)*MY_BULLET_MAX)+(sizeof(struct bullet)*MY_BULLET_MAX))
#define WORLD_SIZE (sizeof(struct player))+sizeof(char)
//+(sizeof(struct enemy)*ENEMY_MAX)+(sizeof(struct bullet)*MY_BULLET_MAX)+(sizeof(struct bullet)*MY_BULLET_MAX))

#define CLIENT_UPDATE 100
#define NEW_PLAYER 1
#define UPDATE_PLAYERS 2
#define UPDATE_WORLD 3

enum mode {SERVER, CLIENT, SP};

#endif /* CONFIG_H */

