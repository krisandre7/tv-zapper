#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <channel_parser.h>

#define MAX_LINES 50 // Define a maximum number of lines to read, adjust as needed
#define MAX_LINE_LENGTH 256 // Define a maximum line length, adjust as needed
#define MAX_CHANNELS 100

int parseChannels(const char *filename, struct ChannelInfo *channels) {
    FILE* file = fopen(filename, "r");

    if (file == NULL) {
        perror("Failed to open file");
        return -1;
    }

    int channel_count = 0;
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '[') { // Start of a channel section
            // Remove leading and trailing whitespace
            char* channel_name = line + 1; // Skip the '['
            char* newline = strchr(channel_name, ']');
            if (newline != NULL) {
                *newline = '\0'; // Remove the ']' character
                strcpy(channels[channel_count].name, channel_name);
            }
        }
        else if (strstr(line, "SERVICE_ID") != NULL) {
            printf("%s", line);
            sscanf(line, "        SERVICE_ID = %d", &channels[channel_count].channel_id);
        }
        else if (strstr(line, "NETWORK_ID") != NULL) {
            sscanf(line, "        NETWORK_ID = %d", &channels[channel_count].network_id);
        }
        else if (strstr(line, "TRANSPORT_ID") != NULL) {
            sscanf(line, "        TRANSPORT_ID = %d", &channels[channel_count].transport_id);
        }
        else if (strstr(line, "VIDEO_PID") != NULL) {
            sscanf(line, "        VIDEO_PID = %d", &channels[channel_count].video_pid);
        }
        else if (strstr(line, "AUDIO_PID") != NULL) {
            sscanf(line, "        AUDIO_PID = %d", &channels[channel_count].audio_pid);
        }
        else if (strstr(line, "FREQUENCY") != NULL) {
            sscanf(line, "        FREQUENCY = %d", &channels[channel_count].frequency);
        }
        else if (strstr(line, "COUNTRY") != NULL) {
            char country[MAX_LINE_LENGTH];
            sscanf(line, "        COUNTRY = %s", country);
            strcpy(channels[channel_count].country, country);
        }
        else if (strstr(line, "DELIVERY_SYSTEM") != NULL) {
            char delivery_system[MAX_LINE_LENGTH];
            sscanf(line, "        DELIVERY_SYSTEM = %s", delivery_system);
            strcpy(channels[channel_count].delivery_system, delivery_system);
        }
        else if (line[0] == '\n') { // End of a channel section
            channel_count++;
        }

        if (channel_count == MAX_LINES) {
            printf("Max number of channels reached. Increase MAX_LINES if needed.\n");
            break;
        }

    }

    fclose(file);

    return channel_count;
}

int printChannels(struct ChannelInfo *channels, int channel_count) {
    // Print the extracted channel information
    for (int i = 0; i < channel_count; i++) {
        printf("Channel Name: %s\n", channels[i].name);
        printf("Frequency: %d\n", channels[i].frequency);
        printf("Video PID: %d\n", channels[i].video_pid);
        printf("Audio PID: %d\n", channels[i].audio_pid);
        printf("\n");
    }
}

