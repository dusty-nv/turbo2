#!/bin/bash

cd ../sixpair
gcc -o sixpair sixpair.c -lusb

sudo rfkill unblock bluetooth
sleep 3
hcitool dev
sleep 2

sudo ./sixpair

cd ../sixad
make clean
make
sudo make install


