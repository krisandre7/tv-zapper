#include "channel.h"
#include <stdio.h>

void print_channel(const struct Channel *channel) {
    printf("Channel Name: %s\n", channel->name);
    printf("Frequency: %d\n", channel->frequency);
    printf("Channel ID: %d\n", channel->channel_id);
    printf("Network ID: %d\n", channel->network_id);
    printf("Transport ID: %d\n", channel->transport_id);
    printf("Video PID: %d\n", channel->video_pid);
    printf("Audio PID: %d\n", channel->audio_pid);
    printf("Country: %s\n", channel->country);
    printf("Delivery System: %s\n", channel->delivery_system);
    printf("\n");
}
