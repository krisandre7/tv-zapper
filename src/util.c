#include "util.h"
#include <sys/time.h>
#include <stdlib.h>

double current_time_millis() {
  struct timeval tv;
  gettimeofday(&tv, (struct timezone *)NULL);
  return tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
}

void wait_milliseconds(unsigned int milisseconds) {
    // esperar 5 segundos garante tempo o suficiente para o demuxer
    // ser criado
    double tuneClock = current_time_millis();
    while ((current_time_millis() - tuneClock) < milisseconds) {
    }
}