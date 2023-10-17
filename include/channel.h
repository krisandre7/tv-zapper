#define VAR_SIZE 50 

// Structure to store information about each channel
struct Channel {
    char name[VAR_SIZE];
    int frequency;
    int channel_id;
    int network_id;
    int transport_id;
    int video_pid;
    int audio_pid;
    char country[VAR_SIZE];
    char delivery_system[VAR_SIZE];
};

void print_channel(const struct Channel *channel);