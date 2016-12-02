#!/bin/bash

echo "starting Dualshock bluetooth controller"

sudo rfkill unblock bluetooth
sleep 3
hcitool dev
sleep 2
#sudo service bluetooth stop
sleep 1
sixad --start

