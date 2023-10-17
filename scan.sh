#!/bin/bash

# Escaneia canais e os escreve em dvb_channel.conf
channel_conf="dvb_channel.conf"

dvbv5-scan "/usr/share/dvb/isdb-t/br-am-Manaus" "--output=$channel_conf"