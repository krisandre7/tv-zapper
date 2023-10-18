#include "channel.h"
#include <stdbool.h>

struct ChannelListNode {
    struct Channel data;
    struct ChannelListNode *next;
    struct ChannelListNode *prev;
};

struct ChannelList {
    struct ChannelListNode *head;
};


void init_channel_list(struct ChannelList *list);

bool is_channel_list_empty(struct ChannelList *list);

void insert_channel(struct ChannelList *list, struct Channel new_channel);

void print_channel_list(struct ChannelList *list);

struct ChannelListNode *get_next_node(struct ChannelList *list, struct ChannelListNode *current);

struct ChannelListNode *get_prev_node(struct ChannelList *list, struct ChannelListNode *current);

void free_channel_list(struct ChannelList *list);