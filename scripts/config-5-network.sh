#!/bin/bash


echo " " | sudo tee -a /etc/network/interfaces
echo "auto wlan0" | sudo tee -a /etc/network/interfaces
echo "iface wlan0 inet static" | sudo tee -a /etc/network/interfaces
echo "address 192.168.2.1" | sudo tee -a /etc/network/interfaces
echo "netmask 255.255.255.0" | sudo tee -a /etc/network/interfaces
