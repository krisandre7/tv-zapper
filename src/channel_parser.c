#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <channel_parser.h>

#define MAX_LINES 2000 // Define a maximum number of lines to read, adjust as needed
#define MAX_LINE_LENGTH 256 // Define a maximum line length, adjust as needed
#define MAX_CHANNELS 100

struct ChannelList parse_channels(const char *filename) {
    struct ChannelList channel_list;
    FILE* file = fopen(filename, "r");

    if (file == NULL) {
        perror("Failed to open file");
        return channel_list;
    }

    int line_count = 0;
    char line[MAX_LINE_LENGTH];

    init_channel_list(&channel_list);

    while (fgets(line, sizeof(line), file)) {
        struct Channel channel;

        if (line[0] == '[') { // Start of a channel section
            // Remove leading and trailing whitespace
            char* channel_name = line + 1; // Skip the '['
            char* newline = strchr(channel_name, ']');
            if (newline != NULL) {
                *newline = '\0'; // Remove the ']' character
                strcpy(channel.name, channel_name);
            }
        }
        else if (strstr(line, "SERVICE_ID") != NULL) {
            sscanf(line, "        SERVICE_ID = %d", &channel.channel_id);
        }
        else if (strstr(line, "NETWORK_ID") != NULL) {
            sscanf(line, "        NETWORK_ID = %d", &channel.network_id);
        }
        else if (strstr(line, "TRANSPORT_ID") != NULL) {
            sscanf(line, "        TRANSPORT_ID = %d", &channel.transport_id);
        }
        else if (strstr(line, "VIDEO_PID") != NULL) {
            sscanf(line, "        VIDEO_PID = %d", &channel.video_pid);
        }
        else if (strstr(line, "AUDIO_PID") != NULL) {
            sscanf(line, "        AUDIO_PID = %d", &channel.audio_pid);
        }
        else if (strstr(line, "FREQUENCY") != NULL) {
            sscanf(line, "        FREQUENCY = %d", &channel.frequency);
        }
        else if (strstr(line, "COUNTRY") != NULL) {
            char country[MAX_LINE_LENGTH];
            sscanf(line, "        COUNTRY = %s", country);
            strcpy(channel.country, country);
        }
        else if (strstr(line, "DELIVERY_SYSTEM") != NULL) {
            char delivery_system[MAX_LINE_LENGTH];
            sscanf(line, "        DELIVERY_SYSTEM = %s", delivery_system);
            strcpy(channel.delivery_system, delivery_system);
        }
        else if (line[0] == '\n') { // End of a channel section
            insert_channel(&channel_list, channel);
            line_count++;
        }

        if (line_count == MAX_LINES) {
            printf("Max number of channels reached. Increase MAX_LINES if needed.\n");
            break;
        }

    }

    fclose(file);

    return channel_list;
}

