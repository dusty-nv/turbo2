#!/usr/bin/env bash
# this script is automatically run from CMakeLists.txt

sudo apt-get update
sudo apt-get install -y screen libusb-dev libusb-1.0-0-dev libbluetooth-dev libgstreamer0.10-dev libgstreamer-plugins-base0.10-dev
sudo apt-get update


git clone http://github.com/dusty-nv/jetson-inference
cd jetson-inference
mkdir build
cd build
cmake ../
make


cd ../../
git clone http://github.com/dusty-nv/rovernet
cd rovernet
mkdir build
cd build
cmake ../
make

