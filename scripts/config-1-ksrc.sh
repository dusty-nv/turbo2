#!/bin/bash

if [ $(id -u) != 0 ]; then
   echo "This script requires root permissions"
   echo "$ sudo "$0""
   exit
fi


# Get the kernel source for LT4 21.4
cd /usr/src/
wget --no-check-certificate 'https://developer.nvidia.com/embedded/dlc/l4t-sources-24-2-1' -O sources_r24.2.1.tbz2

# Decompress
tar -xvf sources_r24.2.1.tbz2
cd sources

tar -xvf kernel_src.tbz2
cd kernel


