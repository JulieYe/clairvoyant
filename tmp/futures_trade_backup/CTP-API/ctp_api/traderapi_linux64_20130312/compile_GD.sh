#!/bin/bash

g++ -std=c++11 -Wall -O2 -DDEBUG MdRecorder_GD.cpp -o MdRecorder_GD -lrt -L. -lthosttraderapi -lthostmduserapi -Wl,-rpath,. 