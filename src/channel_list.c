#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "channel_list.h"

void init_channel_list(struct ChannelList *list) {
    list->head = NULL;
}

bool is_channel_list_empty(struct ChannelList *list) {
    return list->head == NULL;
}

void insert_channel(struct ChannelList *list, struct Channel new_channel) {
    struct ChannelListNode *new_node = (struct ChannelListNode *)malloc(sizeof(struct ChannelListNode));
    if (new_node == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }

    new_node->data = new_channel;
    if (list->head == NULL) {
        new_node->next = new_node;
        new_node->prev = new_node;
        list->head = new_node;
    } else {
        struct ChannelListNode *tail = list->head->prev;
        new_node->next = list->head;
        new_node->prev = tail;
        list->head->prev = new_node;
        tail->next = new_node;
    }
}

void print_channel_list(struct ChannelList *list) {
    if (is_channel_list_empty(list)) {
        printf("List is empty.\n");
        return;
    }

    struct ChannelListNode *current = list->head;
    do {
        print_channel(&current->data);
        current = current->next;
    } while (current != list->head);
}

struct ChannelListNode *get_next_node(struct ChannelList *list, struct ChannelListNode *current) {
    return current->next;
}

struct ChannelListNode *get_prev_node(struct ChannelList *list, struct ChannelListNode *current) {
    return current->prev;
}

void free_channel_list(struct ChannelList *list) {
    if (is_channel_list_empty(list)) {
        return; // List is already empty
    }

    struct ChannelListNode *current = list->head;
    struct ChannelListNode *temp;

    do {
        temp = current;
        current = current->next;
        free(temp);
    } while (current != list->head);

    list->head = NULL;
}