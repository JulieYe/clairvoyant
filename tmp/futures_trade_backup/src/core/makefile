ROOTDIR = ..
include $(ROOTDIR)/make.begin.mk
include $(ROOTDIR)/make.env.mk

#CXX=/usr/bin/g++
#CPPFLAGS=-g -Wall -pedantic -std=c++11 -I/usr/local/include -pthread -D$(SPFLAG) 
CPPFLAGS=$(CPPFLAGS_COMMON) -D$(SPFLAG) 
LDFLAGS=$(LDFLAGS_COMMON)
LDFLAGSPROD=$(LDFLAGSPROD_COMMON) -L. -Wl,-rpath,.

#LDFLAGS=-lrt -Wl,-rpath=/opt/gcc/gcc-4.7.0/lib64
# LDFLAGS NOTE: -lrt needed for shm_open, etc. for boost interprocess.
OBJDIR = ../obj
LIBDIR = ../lib
LIBNAME = libSimulatorCore


LIBS_BASE = Px.o Message.o SimulatorCore.o Matchy.o OrderBook.o Strategy.o Logger.o PrintingStack.o ObjectFactory.o

LIBS := $(foreach s,$(LIBS_BASE),$(OBJDIR)/$s)

REBUILDABLES = $(LIBS) $(LIBDIR)/$(LIBNAME).a

echo_libs :
	echo $(LIBS)

clean :
	rm -f $(REBUILDABLES) 
	echo Clean done

all : $(LIBS)
	ar crf $(LIBDIR)/$(LIBNAME).a $(LIBS)

$(OBJDIR)/%.o : %.cpp
	$(CXX) $(CPPFLAGS) -o $@ -c $<

Message.o : Message.hpp
Px.o : Px.hpp
SimulatorCore.o : SimulatorCore.hpp Message.hpp Matchy.hpp Strategy.hpp
Matchy.o : Matchy.hpp Px.hpp Message.hpp OrderBook.hpp SimulatorCore.hpp
OrderBook.o : OrderBook.hpp Px.hpp Message.hpp
Strategy.o : Strategy.hpp SimulatorCore.hpp Logger.hpp
Logger.o : Logger.hpp PrintingStack.hpp
PrintingStack.o : PrintingStack.hpp
ObjectFactory.o : ObjectFactory.hpp Matchy.hpp Strategy.hpp