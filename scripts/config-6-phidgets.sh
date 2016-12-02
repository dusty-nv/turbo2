#!/bin/bash

sudo apt-get install -y libusb-1.0-0-dev 

wget http://www.phidgets.com/downloads/libraries/libphidget.tar.gz
tar -xvzf libphidget.tar.gz

cd libphidget-2.1.8.20151217
./configure
make
sudo make install
