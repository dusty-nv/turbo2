#!/bin/bash

if [ $(id -u) != 0 ]; then
   echo "This script requires root permissions"
   echo "$ sudo "$0""
   exit
fi


cd /usr/src/sources/kernel

# Get the kernel configuration file
zcat /proc/config.gz > .config


#sudo sed -i 's/# CONFIG_USB_SERIAL_FTDI_SIO is not set/CONFIG_USB_SERIAL_FTDI_SIO=m/' .config
sudo sed -i 's/# CONFIG_USB_SERIAL_CP210X is not set/CONFIG_USB_SERIAL_CP210X=m/' .config
sudo sed -i 's/CONFIG_LOCALVERSION=""/CONFIG_LOCALVERSION="-tegra"/' .config

