#!/bin/sh
rm sudo-uno-server
find . -type f -name '*.o' -delete
make
clear
./sudo-uno-server