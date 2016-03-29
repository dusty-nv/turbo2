#!/bin/bash

echo "starting TURBO2"


# start with sudo (under SSH, evdev needs root priveledge)
sudo ./turbo2 $(tput cols) $(tput lines)
