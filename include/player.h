#include "gst/gst.h"

typedef struct player
{
    GstElement* pipeline;
    GstElement *appsrc;
    GstElement *dec;
} player_t;

int PlayerInit(player_t *player);
int PlayerStart(player_t *player) ;
int InjectData(player_t *player, const unsigned char *data, unsigned int size);