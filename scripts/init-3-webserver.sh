#!/bin/bash

echo "starting Torch display webserver"

cd /home/ubuntu/workspace/rovernet/build/torch/bin/
sudo ./th -ldisplay.start 80 0.0.0.0
#sleep 3
#cd ../../
#./rovernet-console

