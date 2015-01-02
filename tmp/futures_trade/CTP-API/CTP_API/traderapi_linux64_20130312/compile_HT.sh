#!/bin/bash

g++ -std=c++11 -Wall -O2 -DDEBUG MdRecorder_HT.cpp -o MdRecorder_HT -lrt -L. -lthosttraderapi -lthostmduserapi -Wl,-rpath,. 