#!/bin/bash

# Escaneia canais e os escreve em dvb_channel.conf
channel_conf="dvb_channel.conf"

dvbv5-scan "/usr/share/dvb/isdb-t/br-am-Manaus" "--output=$channel_conf"


# Check if the input file exists
if [ ! -f "$channel_conf" ]; then
  echo "$channel_conf file not found."
  exit 1
fi

# Count the number of left brackets and store it in a variable
left_bracket_count=$(grep -o "\[" "$channel_conf" | wc -l)

# Concatenate the count with the file's contents and save it to the input file
temp_file=$(mktemp)
echo "Number of channels: $left_bracket_count" | cat - "$channel_conf" > "$temp_file"
mv "$temp_file" "$channel_conf"

echo "Number of left brackets: $left_bracket_count"
echo "Modified content saved to: $channel_conf"