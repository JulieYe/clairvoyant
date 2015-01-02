#CXX=/usr/bin/g++
ROOTDIR = ..
include $(ROOTDIR)/make.begin.mk
include $(ROOTDIR)/make.env.mk

OBJDIR = ../obj
LIBDIR = ../lib
COREDIR = ../core
FEATUREDIR = ../feature

#CPPFLAGS=-g -Wall -pedantic -std=c++11 -I/usr/local/include -pthread 
CPPFLAGS=$(CPPFLAGS_COMMON) -L$(LIBDIR) -lSimulatorCore -L$(FEATUREDIR) -lSimulatorCoreFeature -I$(COREDIR) -I$(FEATUREDIR) -D$(SPFLAG)

LDFLAGS=$(LDFLAGS_COMMON)
LDFLAGSPROD=$(LDFLAGSPROD_COMMON) -L. -Wl,-rpath,.

#LDFLAGS=-lrt -Wl,-rpath=/opt/gcc/gcc-4.7.0/lib64
# LDFLAGS NOTE: -lrt needed for shm_open, etc. for boost interprocess.
LIBNAME = libSimulatorCoreAlpha

LIBS_BASE = AG.o IF.o RB.o RU.o IF30SEC.o IFDIR.o


LIBS := $(foreach s,$(LIBS_BASE),$(OBJDIR)/$s)

REBUILDABLES = $(LIBS) $(LIBDIR)/$(LIBNAME).a

echo_libs :
	echo $(LIBS)

clean :
	rm -f $(REBUILDABLES) $(LIBDIR)/$(LIBNAME).a
	echo Clean done

all : $(LIBS)
	ar crf $(LIBDIR)/$(LIBNAME).a $(LIBS)

$(OBJDIR)/%.o : %.cpp
	$(CXX) $(CPPFLAGS) -o $@ -c $<

AG.o : AG.hpp $(FEATUREDIR)/Signal.hpp $(FEATUREDIR)/Feature.hpp $(COREDIR)/Px.hpp $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreFeature.a
IF.o : AG.hpp $(FEATUREDIR)/Signal.hpp $(FEATUREDIR)/Feature.hpp $(COREDIR)/Px.hpp $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreFeature.a
RB.o : AG.hpp $(FEATUREDIR)/Signal.hpp $(FEATUREDIR)/Feature.hpp $(COREDIR)/Px.hpp $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreFeature.a
RU.o : AG.hpp $(FEATUREDIR)/Signal.hpp $(FEATUREDIR)/Feature.hpp $(COREDIR)/Px.hpp $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreFeature.a
IF30SEC.o : IF30SEC.hpp $(FEATUREDIR)/Signal.hpp $(FEATUREDIR)/Feature.hpp $(COREDIR)/Px.hpp $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreFeature.a
IFDIR.o : IFDIR.hpp $(FEATUREDIR)/Signal.hpp $(FEATUREDIR)/Feature.hpp $(COREDIR)/Px.hpp $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreFeature.a

