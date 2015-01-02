CC = /usr/local/bin/g++-4.8
CCFLAGS = -O3 -Wall -Wno-unused -pedantic -pthread -std=c++11 -I/usr/local/include
#CXXFLAGS = -g -Wall -Wno-unused -pedantic -pthread -std=c++11 -I/usr/local/include
LDFLAGS = -lrt -lpthread -L/usr/local/lib
.PHONY: all test clean ready
