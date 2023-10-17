#include <stdio.h>
#include <stdlib.h>
#include "string.h"

#define MAX_LINES 50 // Define a maximum number of lines to read, adjust as needed
#define MAX_LINE_LENGTH 256 // Define a maximum line length, adjust as needed
#define MAX_CHANNELS 100

// Structure to store information about each channel
struct ChannelInfo {
    char name[MAX_LINE_LENGTH];
    int frequency;
    int channel_id;
    int network_id;
    int transport_id;
    int video_pid;
    int audio_pid;
    char country[MAX_LINE_LENGTH];
    char delivery_system[MAX_LINE_LENGTH];
};

int parseChannels(const char *filename, struct ChannelInfo *channels);

int printChannels(struct ChannelInfo *channels, int channel_count);
