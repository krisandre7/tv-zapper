#include <fcntl.h>
#include <linux/dvb/dmx.h>
#include <linux/dvb/frontend.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <unistd.h>

#include "player.h"
#include "channel_parser.h"

#define FRONTEND_PATH "/dev/dvb/adapter0/frontend0"
#define DEMUX_PATH "/dev/dvb/adapter0/demux0"
#define DVR_PATH "/dev/dvb/adapter0/dvr0"

const int tune_timeout = 3000;
const unsigned BUFFER_SIZE = 4096;
const unsigned CRC_SIZE = 4;

typedef struct u16_node {
  unsigned short value;
  struct u16_node *next;
} u16_node_t;

void u16_node_print(u16_node_t *head) {
  u16_node_t *current = head;
  
  while (current != NULL) {
    printf("0x%X\n", current->value);
    current = current->next;
  }
}

void u16_node_push(u16_node_t **head, unsigned short value) {
  u16_node_t *last = *head;

  u16_node_t *new_node = (u16_node_t *)malloc(sizeof(u16_node_t));
  new_node->value = value;
  new_node->next = NULL;

  if (*head == NULL) {
    *head = new_node;
    return;
  }

  while (last->next != NULL) {
    last = last->next;
  }

  last->next = new_node;
}

static double current_time_millis() {
  struct timeval tv;
  gettimeofday(&tv, (struct timezone *)NULL);
  return tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
}

int is_fe_locked(int fd) {
  fe_status_t status = 0;
  static int state = 0;
  if ((ioctl(fd, FE_READ_STATUS, &status) < 0)) {
    perror("FE_READ_STATUS");
    state = 0;
    return 0;
  }
  switch (state) {
  case 0:
    if (status & FE_HAS_SIGNAL) {
      printf("FE_HAS_SIGNAL\n");
      state = 1;
    }
    break;
  case 1:
    if (status & FE_HAS_SYNC) {
      printf("FE_HAS_SYNC\n");
      state = 2;
    }
    break;
  case 2:
    if (status & FE_HAS_LOCK) {
      printf("FE_HAS_LOCK\n");
      state = 0;
      return 1;
    }
    break;
  default:
    break;
  }
  return 0;
}

int parse_pat(char *data, unsigned size) {
  unsigned tableId = *data;
  data++;
  unsigned short sectionLength = ((data[0] << 8) | (data[1])) & 0x0FFF;
  data += 2;

  /* Ignore transport_stream_id, current_next_indicator and
   * last_section_number*/
  data += 5;

  unsigned int N = sectionLength - 5;
  unsigned int i = 0;

  u16_node_t *pmtList = NULL;

  while (i < N - CRC_SIZE) {
    unsigned short programNumber = (data[i] << 8) | (data[i + 1]);
    unsigned short pmtPID = ((data[i + 2] << 8) | (data[i + 3])) & 0x1FFF;
    if (programNumber != 0) {
      u16_node_push(&pmtList, pmtPID);
    }
    i += 4;
  }

  printf("Table ID:%d\n", tableId);
  printf("sectionLength:%d\n", sectionLength);
  printf("PMT PIDs:\n");

  u16_node_print(pmtList);

  return 0;
}

int tune(unsigned int frequency) {
  int fd;
  /* Open Frontend */
  if ((fd = open(FRONTEND_PATH, O_RDWR)) < 0) {
    perror("FRONTEND DEVICE: ");
    return -1;
  }

  /* Configure dtv_property with tuning fe params */
  struct dtv_property tuning_props_array[] = {
      {.cmd = DTV_DELIVERY_SYSTEM, .u.data = SYS_ISDBT},
      {.cmd = DTV_FREQUENCY, .u.data = frequency},
      {.cmd = DTV_TUNE}};

  struct dtv_properties tuning_props = {.num = 3, .props = tuning_props_array};

  /* System call to proceed tuning */
  printf("Tuning to %dkHz...\n", frequency / 1000);
  if (ioctl(fd, FE_SET_PROPERTY, &tuning_props) < 0) {
    perror("FE_SET_PROPERTY: ");
    return -1;
  }

  double tune_clock = current_time_millis();
  while ((current_time_millis() - tune_clock) < tune_timeout) {
    if (is_fe_locked(fd)) {
      printf("Tuner Locked\n");
      return 0;
    }
  }
  return -1;
}

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

  struct ChannelList channel_list;
  channel_list = parse_channels(channel_file);

  print_channel_list(&channel_list);

  // printQueue(&channelQueue);

  // if (channel_count == -1)
  //   return -1;

  // if (tune(frequency) < 0) {
  //   printf("Timeout\n");
  //   return -1;
  // }

  // player_t player;

  // if (PlayerInit(&player) < 0) {
  //   printf("Player Init Failed\n");
  //   return -1;
  // }
  // PlayerStart(&player);

  // int demux;
  // if ((demux = open(DEMUX_PATH, O_RDWR)) < 0) {
  //   perror("DEMUX DEVICE: ");
  //   return -1;
  // }

  // struct dmx_sct_filter_params sctFilterParams;
  // memset(&sctFilterParams.filter, 0, sizeof(sctFilterParams.filter));
  // sctFilterParams.pid = 0;
  // sctFilterParams.timeout = 2000;
  // sctFilterParams.flags = DMX_IMMEDIATE_START | DMX_ONESHOT;

  // if (ioctl(demux, DMX_SET_FILTER, &sctFilterParams) < 0) {
  //   perror("DEMUX DEVICE: ");
  //   return -1;
  // }

  // char *buffer = (char *)malloc(BUFFER_SIZE);

  // int bytesRead = read(demux, buffer, BUFFER_SIZE);
  // if (bytesRead <= 0) {
  //   printf("No section received\n");
  //   free(buffer);
  //   return -1;
  // }
  // printf("Section of size %d\n", bytesRead);
  // parse_pat(buffer, bytesRead);

  // struct dmx_pes_filter_params pesFilterParams;
  // pesFilterParams.pid = 0x2000;
  // pesFilterParams.pes_type = DMX_PES_VIDEO;
  // pesFilterParams.input = DMX_IN_FRONTEND;
  // pesFilterParams.output = DMX_OUT_TS_TAP;
  // pesFilterParams.flags = DMX_IMMEDIATE_START | DMX_CHECK_CRC;  

  // if (ioctl(demux, DMX_SET_PES_FILTER, &pesFilterParams) < 0) {
  //   perror("DEMUX DMX_SET_PES_FILTER: ");
  //   return -1;
  // }

  // int dvr;
  // if ((dvr = open(DVR_PATH, O_RDONLY)) < 0) {
  //   perror("DVR DEVICE: ");
  //   return -1;
  // }

  // char data_buffer[4096];

  // while(1) {
  //   int bytes_read = read(dvr, data_buffer,4096);
    
  //   // if (bytes_read > 0) {
  //   //   InjectData(&player, data_buffer, bytes_read);
  //   // }
  // }

  // free_channel_list(&channel_list);

  // free(buffer);
  // close(demux);
  return 0;
}
