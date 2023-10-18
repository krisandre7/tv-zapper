#include <linux/dvb/dmx.h>
#include <sys/ioctl.h>

#define DEMUX_PATH "/dev/dvb/adapter0/demux0"
const unsigned BUFFER_SIZE = 4096;

int setup_demux(int demux) {
    struct dmx_sct_filter_params sctFilterParams;

    memset(&sctFilterParams.filter, 0, sizeof(sctFilterParams.filter));
    sctFilterParams.pid = 0;
    sctFilterParams.timeout = 2000;
    sctFilterParams.flags = DMX_IMMEDIATE_START | DMX_ONESHOT;

    if (ioctl(demux, DMX_SET_FILTER, &sctFilterParams) < 0) {
        perror("DEMUX DEVICE: ");
        return -1;
    }

    char *buffer = (char *)malloc(BUFFER_SIZE);

    int bytesRead = read(demux, buffer, BUFFER_SIZE);
    if (bytesRead <= 0) {
        printf("No section received\n");
        free(buffer);
        return -1;
    }
    printf("Section of size %d\n", bytesRead);
    parse_pat(buffer, bytesRead);

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

    free(buffer);
    return 0;
}