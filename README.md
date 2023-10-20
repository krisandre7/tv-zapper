# TV Zapper
Uses the linux dvb api and gstreamer to parse a list of TV channels, tune the frontend to a specific frequency and play the incoming stream to the user. Using the "u" and "d" keys for up and down, the user can tune to all the channels in the list.

# Usage
`./main <channel_file> (default value: dvb_channel.conf)`

# Prerequisites
## Packages
- gstreamer
- dvb-apps
## Channel configuration file
In order to generate a list of channel frequencies to tune to, we need to scan our local area for channels. This can be done using the provided script `scan.sh`:
```
sudo chmod +x ./scan.sh
./scan.sh
```

# Build
Run `make`.