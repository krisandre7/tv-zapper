#include <fcntl.h>
#include <linux/dvb/frontend.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <unistd.h>

#include "player.h"
#include "channel_parser.h"
#include "tuner.h"
#include "demux.h"

#define FRONTEND_PATH "/dev/dvb/adapter0/frontend0"
#define DEMUX_PATH "/dev/dvb/adapter0/demux0"
#define DVR_PATH "/dev/dvb/adapter0/dvr0"

#define DATA_BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
  char *channel_file;
  // int frequency = 0;

  if (argc > 2) {
    printf("Usage: main channel_file\n");
    return -1;
  } else if (argc == 2) {
    strcpy(channel_file, argv[1]);
  } else {
    channel_file = "dvb_channel.conf";
  }

  struct ChannelList channel_list = parse_channels(channel_file);

  struct ChannelListNode *current_node = channel_list.head;

  printf("Current Channel: %s\n", current_node->data.name);

  // Acessa frontend
  int fd;  // Mova a declaração do descritor de arquivo do frontend para fora do loop

  if ((fd = open(FRONTEND_PATH, O_RDWR)) < 0) {
      perror("FRONTEND DEVICE: ");
      return -1;
  }

  // Inicia player
  player_t player;
  if (PlayerInit(&player) < 0) {
    printf("Player Init Failed\n");
    return -1;
  }

  // Acessa demux
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

  bool playing = false;
  char data_buffer[DATA_BUFFER_SIZE];
  while (1) {
    if (playing == false) {
      PlayerStart(&player);
      
      if (tune(fd, current_node->data.frequency) < 0) {
        perror("Não foi possível sintonizar o canal.");
        return -1;
      }

      playing = true;
    }

    // printf("Enter 'q' to quit, 'u' to tune up, 'd' to tune down: ");
    // char input = getchar();

    // switch (input)
    // {
    //   case 'q':
    //     goto exit_loop;
    //     break;
    //   case 'u':
    //     current_node = current_node->next;
    //     printf("%s\n", current_node->data.name); 
    //     break;
    //   case 'd':
    //     current_node = current_node->prev;
    //     printf("%s\n", current_node->data.name); 
    //     break;
    // } 

    bytes_read = read(dvr, data_buffer, DATA_BUFFER_SIZE);
    
    if (bytes_read > 0) {
      InjectData(&player, data_buffer, bytes_read);
    }

  } exit_loop: ;

  PlayerStop(&player);
  free_channel_list(&channel_list);

  close(fd);
  close(demux);
  return 0;
}
