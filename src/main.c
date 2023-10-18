#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <ctype.h> // Include ctype.h for character case conversion

#include "player.h"
#include "channel_parser.h"
#include "tuner.h"
#include "demux.h"

#define FRONTEND_PATH "/dev/dvb/adapter0/frontend0"
#define DEMUX_PATH "/dev/dvb/adapter0/demux0"
#define DVR_PATH "/dev/dvb/adapter0/dvr0"

#define DATA_BUFFER_SIZE 4096
const int TUNE_TIMEOUT = 3000;

int main(int argc, char *argv[]) {
  char *channel_file;

  if (argc > 2) {
    printf("Usage: main channel_file\n");
    return -1;
  } else if (argc == 2) {
    channel_file = argv[1];
  } else {
    channel_file = "dvb_channel.conf";
  }

  struct ChannelList channel_list = parse_channels(channel_file);
  struct ChannelListNode *current_node = channel_list.head;

  printf("Current Channel: %s\n", current_node->data.name);

  int fd;
  if ((fd = open(FRONTEND_PATH, O_RDWR)) < 0) {
    perror("FRONTEND DEVICE: ");
    return -1;
  }

  player_t player;
  if (PlayerInit(&player) < 0) {
    printf("Player Init Failed\n");
    return -1;
  }

  int demux;
  if ((demux = open(DEMUX_PATH, O_RDWR)) < 0) {
    perror("DEMUX DEVICE: ");
    return -1;
  }

  int dvr;
  if ((dvr = open(DVR_PATH, O_RDONLY)) < 0) {
    perror("DVR DEVICE: ");
    return -1;
  }

  int buffer = setup_demux(demux);
  unsigned int bytes_read = 0;
  if (buffer < 0) {
    perror("Não foi possível configurar demux");
  }

  PlayerStart(&player);

  if (tune(fd, current_node->data.frequency) < 0) {
    perror("Não foi possível sintonizar o canal.");
    return -1;
  }

  const unsigned char data_buffer[DATA_BUFFER_SIZE];

  struct termios termios_original;
  tcgetattr(STDIN_FILENO, &termios_original);
  struct termios termios_modified = termios_original;
  termios_modified.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &termios_modified);

  fd_set fds;
  struct timeval tv;
  int stdin_fd = STDIN_FILENO;

  while (1) {
    FD_ZERO(&fds);
    FD_SET(stdin_fd, &fds);

    tv.tv_sec = 0;
    tv.tv_usec = 0;

    int ready = select(stdin_fd + 1, &fds, NULL, NULL, &tv);

    if (ready == -1) {
      perror("select");
      break;
    } else if (ready > 0) {
      if (FD_ISSET(stdin_fd, &fds)) {
        char input;
        ssize_t bytes = read(stdin_fd, &input, 1);
        if (bytes == 1) {
          // Handle user input
          if (input == 'q') {
            printf("Exiting...\n");
            break;
          } else if (input == 'u') {
            // Move to the next channel node
            current_node = current_node->next;
            printf("Current Channel: %s\n", current_node->data.name);

            // PlayerStop(&player);
            // PlayerRestart(&player);

            if (tune(fd, current_node->data.frequency) < 0) {
              perror("Não foi possível sintonizar o canal.");
            }

            // PlayerStart(&player);
          } else if (input == 'd') {
            // Move to the previous channel node
            current_node = current_node->prev;
            printf("Current Channel: %s\n", current_node->data.name);

            // PlayerStop(&player);
            // PlayerRestart(&player);

            if (tune(fd, current_node->data.frequency) < 0) {
              perror("Não foi possível sintonizar o canal.");
            }

            // PlayerStart(&player);
          }
        }
      }
    }

    bytes_read = read(dvr, data_buffer, DATA_BUFFER_SIZE);

    if (bytes_read > 0) {
      InjectData(&player, data_buffer, bytes_read);
    }
  }

  tcsetattr(STDIN_FILENO, TCSANOW, &termios_original);

  PlayerStop(&player);
  free_channel_list(&channel_list);

  close(fd);
  close(demux);
  return 0;
}
