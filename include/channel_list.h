#include "channel.h"
#include <stdbool.h>

struct Node {
    struct Channel data;
    struct Node *next;
    struct Node *prev;
};

struct ChannelList {
    struct Node *head;
};


void init_channel_list(struct ChannelList *list);

bool is_channel_list_empty(struct ChannelList *list);

void insert_channel(struct ChannelList *list, struct Channel new_channel);

void print_channel_list(struct ChannelList *list);

struct Node *get_next_node(struct ChannelList *list, struct Node *current);

struct Node *get_prev_node(struct ChannelList *list, struct Node *current);

void free_channel_list(struct ChannelList *list);