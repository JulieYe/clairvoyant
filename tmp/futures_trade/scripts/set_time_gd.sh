#!/bin/bash

TZ='Asia/Shanghai' date --date "5 seconds" +%T | xargs ssh -p 48484 mingyuan@140.206.129.147 sudo date +%T -s

