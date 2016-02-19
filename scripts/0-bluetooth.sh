#!/bin/bash

echo "starting Dualshock bluetooth controller"

sudo rfkill unblock bluetooth
sudo service bluetooth stop
sixad --start

