#!/bin/bash

g++ -std=c++11 -Wall -O2 -DDEBUG MdRecorder_ZD.cpp -o MdRecorder_ZD -lrt -L. -lthosttraderapi -lthostmduserapi -Wl,-rpath,. 