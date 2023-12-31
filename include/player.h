#include "gst/gst.h"

typedef struct player
{
    GstElement* pipeline;
    GstElement *appsrc;
} player_t;

int PlayerInit(player_t *player);
int PlayerStart(player_t *player) ;
int PlayerReady(player_t *player);
int InjectData(player_t *player, const unsigned char *data, unsigned int size);

int PlayerStop(player_t *player);

int PlayerRestart(player_t *player);

void PlayerFree(player_t *player);