#!/bin/bash

if [ $(id -u) != 0 ]; then
   echo "This script requires root permissions"
   echo "$ sudo "$0""
   exit
fi


cd /usr/src/sources/kernel


# Prepare the module for compilation
# note - if it asks for confirmation in step below, enter 'm' for build as module
make prepare
make modules_prepare

# Compile the module
make M=drivers/usb/serial/

cp drivers/usb/serial/cp210x.ko /lib/modules/$(uname -r)/kernel/drivers/usb/serial
depmod -a
/bin/echo -e "\e[1;32mCP210x Driver Module Installed.\e[0m"

