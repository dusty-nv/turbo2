#!/bin/bash

# install hostapd (access point) package
sudo apt-get install hostapd

# configure hostapd daemon
zcat /usr/share/doc/hostapd/examples/hostapd.conf.gz | sudo tee -a /etc/hostapd/hostapd.conf
sudo sed -i 's/ssid=test/ssid=ROVERNET/' /etc/hostapd/hostapd.conf
sudo sed -i 's/# driver=hostap/driver=nl80211/' /etc/hostapd/hostapd.conf


# enable hostapd on starup
sudo sed -i 's/#DAEMON_CONF=""/DAEMON_CONF="\/etc\/hostapd\/hostapd.conf"/' /etc/default/hostapd

# enable access point mode in Jetson wifi driver
echo "options bcmdhd op_mode=2" | sudo tee -a /etc/modprobe.d/bcmdhd.conf

# (reboot here)
# check that the brcm change has taken hold
cat /sys/module/bcmdhd/parameters/op-mode
