#include <linux/dvb/frontend.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#include "tuner.h"

const unsigned CRC_SIZE = 4;
const int tune_timeout = 3000;

static double current_time_millis() {
  struct timeval tv;
  gettimeofday(&tv, (struct timezone *)NULL);
  return tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
}

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

int tune(int fd, unsigned int frequency) {
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