CXX=/usr/bin/g++
CPPFLAGS_COMMON=-O2 -Wall -pedantic -std=c++11 -I/usr/local/include -pthread
##CPPFLAGS_COMMON=-g -Wall -pedantic -std=c++11 -I/usr/local/include -pthread
LDFLAGS_COMMON=-lrt -lpthread -L/usr/local/lib
LDFLAGSPROD_COMMON=-lrt -lpthread -L/usr/local/lib 

