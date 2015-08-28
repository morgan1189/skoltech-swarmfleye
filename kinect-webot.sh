#!/bin/bash
lxterminal -e ffmpeg -y -f v4l2 -r 30 -s 640x480 -i /dev/video4 http://192.168.1.41:8090/feed1.ffm
lxterminal -e /home/linaro/kinect-webot-build/kinect-webot
