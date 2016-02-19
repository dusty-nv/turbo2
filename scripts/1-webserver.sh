#!/bin/bash

echo "starting Torch display webserver"

cd /home/ubuntu/workspace/rovernet/build/torch/bin/
ls -ll
./th -ldisplay.start 80 0.0.0.0
cd ../../
./rovernet-console

