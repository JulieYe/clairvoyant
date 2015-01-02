#!/bin/bash

TZ='Asia/Shanghai' date --date "25 seconds" +%T | xargs ssh -p 58569 root@27.115.57.170 sudo date +%T -s

