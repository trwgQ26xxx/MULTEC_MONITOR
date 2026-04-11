#! /bin/bash

clear

avrdude -c arduino -p m328p -P /dev/ttyUSB0 -b 57600 -U flash:w:"out/emulator.hex":a
