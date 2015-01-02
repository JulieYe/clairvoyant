ROOTDIR = ../..
include $(ROOTDIR)/make.begin.mk
include $(ROOTDIR)/make.env.mk
##CXX=/usr/bin/g++

OBJDIR = ../../obj
LIBDIR = ../../lib
COREDIR = ../../core
UTILSDIR = ../../utils
APICTPDIR = ../CTP

LIBNAME = libSimulatorCoreZDMDApi

CPPFLAGS=$(CPPFLAGS_COMMON) -D$(SPFLAG) -D$(DFLAG) -D$(XFLAG) -D$(SAHFLAG) #-Wsign-conversion 
LDFLAGS=$(LDFLAGS_COMMON) -L$(LIBDIR) -lSimulatorCore -I$(COREDIR) -lSimulatorCoreUtils -I$(UTILSDIR) -I$(APICTPDIR)
#LDFLAGS=-lrt -Wl,-rpath=/opt/gcc/gcc-4.7.0/lib64
# LDFLAGS NOTE: -lrt needed for shm_open, etc. for boost interprocess.

LIBS_BASE = ZDMarketDataSpi.o

REBUILDABLES = $(LIBS) $(LIBDIR)/$(LIBNAME).a

LIBS := $(foreach s,$(LIBS_BASE),$(OBJDIR)/$s)

clean :
	rm -f $(REBUILDABLES) 
	echo Clean done

$(OBJDIR)/%.o : %.cpp
	$(CXX) $(CPPFLAGS) $(LDFLAGS) -o $@ -c $<

all : $(LIBS)
	ar crf $(LIBDIR)/$(LIBNAME).a $(LIBS)


ZDMarketDataSpi.o : ZDMarketDataSpi.hpp $(APICTPDIR)/CFFEX.hpp $(COREDIR)/Message.hpp Converter.hpp TraderSpi.hpp ThostFtdcMdApi.h $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreUtils.a
