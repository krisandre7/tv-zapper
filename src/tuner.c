#include <linux/dvb/frontend.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <fcntl.h>

#include "tuner.h"

#define FRONTEND_PATH "/dev/dvb/adapter0/frontend0"
const unsigned CRC_SIZE = 4;
const int tune_timeout = 3000;

int tune(int frontend, unsigned int frequency) {
  /* Configure dtv_property with tuning fe params */
  struct dtv_property tuning_props_array[] = {
      {.cmd = DTV_DELIVERY_SYSTEM, .u.data = SYS_ISDBT},
      {.cmd = DTV_FREQUENCY, .u.data = frequency},
      {.cmd = DTV_TUNE}};

  struct dtv_properties tuning_props = {.num = 3, .props = tuning_props_array};

  if (ioctl(frontend, FE_SET_PROPERTY, &tuning_props) == -1) {
    perror("ioctl: FE_SET_PROPERTY");
    return -1;
  }

  return 0;
}