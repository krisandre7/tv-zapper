#include <linux/dvb/dmx.h>
#include <sys/ioctl.h>
#include <demux.h>

#define DEMUX_PATH "/dev/dvb/adapter0/demux0"
const unsigned BUFFER_SIZE = 4096;

int setup_demux(int demux) {
    struct dmx_pes_filter_params pesFilterParams;
    pesFilterParams.pid = 0x2000;
    pesFilterParams.pes_type = DMX_PES_VIDEO;
    pesFilterParams.input = DMX_IN_FRONTEND;
    pesFilterParams.output = DMX_OUT_TS_TAP;
    pesFilterParams.flags = DMX_IMMEDIATE_START | DMX_CHECK_CRC;  

    if (ioctl(demux, DMX_SET_PES_FILTER, &pesFilterParams) < 0) {
        perror("DEMUX DMX_SET_PES_FILTER: ");
        return -1;
    }

    return 0;
}