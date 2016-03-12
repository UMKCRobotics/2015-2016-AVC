#! /bin/bash

sudo chmod +x $1
cp $1 /etc/init.d/
sudo update-rc.d $1 defaults
