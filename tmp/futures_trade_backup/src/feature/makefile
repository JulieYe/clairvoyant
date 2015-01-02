ROOTDIR = ..
include $(ROOTDIR)/make.begin.mk
include $(ROOTDIR)/make.env.mk

#CXX=/usr/bin/g++

OBJDIR = ../obj
LIBDIR = ../lib
COREDIR = ../core

#CPPFLAGS=-g -Wall -pedantic -std=c++11 -I/usr/local/include -pthread 
CPPFLAGS=$(CPPFLAGS_COMMON) -L$(LIBDIR) -lSimulatorCore -I$(COREDIR) -D$(SPFLAG)
LDFLAGS=$(LDFLAGS_COMMON)
LDFLAGSPROD=$(LDFLAGSPROD_COMMON) -L. -Wl,-rpath,.

#LDFLAGS=-lrt -Wl,-rpath=/opt/gcc/gcc-4.7.0/lib64
# LDFLAGS NOTE: -lrt needed for shm_open, etc. for boost interprocess.
LIBNAME = libSimulatorCoreFeature

LIBS_BASE = FeatureBase.o Feature.o Signal.o QtEMA.o Technical.o 


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

FeatureBase.o: FeatureBase.hpp $(LIBDIR)/libSimulatorCore.a
Feature.o : FeatureBase.hpp Feature.hpp $(COREDIR)/Message.hpp QtEMA.hpp $(LIBDIR)/libSimulatorCore.a
Signal.o : Signal.hpp FeatureBase.hpp $(COREDIR)/Message.hpp $(LIBDIR)/libSimulatorCore.a
QtEMA.o : QtEMA.hpp $(LIBDIR)/libSimulatorCore.a
Technical.o : Technical.hpp $(LIBDIR)/libSimulatorCore.a
