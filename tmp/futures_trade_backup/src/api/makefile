ROOTDIR = ..
include $(ROOTDIR)/make.begin.mk
include $(ROOTDIR)/make.env.mk

##CXX=/usr/bin/g++

OBJDIR = ../obj
LIBDIR = ../lib
COREDIR = ../core
UTILSDIR = ../utils
APICTPDIR = ./CTP

LIBNAME = libSimulatorCoreApi

CPPFLAGS=$(CPPFLAGS_COMMON) -D$(SPFLAG) -D$(DFLAG) -D$(XFLAG) -D$(SAHFLAG) #-Wsign-conversion 
LDFLAGS=$(LDFLAGS_COMMON) -L$(LIBDIR) -lSimulatorCore -I$(COREDIR) -lSimulatorCoreCTPApi -I$(APICTPDIR) -lSimulatorCoreUtils -I$(UTILSDIR)
#LDFLAGS=-lrt -Wl,-rpath=/opt/gcc/gcc-4.7.0/lib64
# LDFLAGS NOTE: -lrt needed for shm_open, etc. for boost interprocess.

LIBS_BASE = MatchyRegistry.o

REBUILDABLES = $(LIBS) $(LIBDIR)/$(LIBNAME).a

LIBS := $(foreach s,$(LIBS_BASE),$(OBJDIR)/$s)

clean :
	rm -f $(REBUILDABLES) 
	echo Clean done

$(OBJDIR)/%.o : %.cpp
	$(CXX) $(CPPFLAGS) $(LDFLAGS) -o $@ -c $<

all : $(LIBS)
	ar crf $(LIBDIR)/$(LIBNAME).a $(LIBS)

MatchyRegistry.o : MatchyRegistry.hpp $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreCTPApi.a $(LIBDIR)/libSimulatorCoreUtils.a
