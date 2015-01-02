ROOTDIR = ../..
include $(ROOTDIR)/make.begin.mk
include $(ROOTDIR)/make.env.mk
##CXX=/usr/bin/g++

OBJDIR = ../../obj
LIBDIR = ../../lib
COREDIR = ../../core
UTILSDIR = ../../utils

LIBNAME = libSimulatorCoreCTPApi

CPPFLAGS=$(CPPFLAGS_COMMON) -D$(SPFLAG) -D$(DFLAG) -D$(XFLAG) -D$(SAHFLAG) #-Wsign-conversion 
LDFLAGS=$(LDFLAGS_COMMON) -L$(LIBDIR) -lSimulatorCore -I$(COREDIR) -lSimulatorCoreUtils -I$(UTILSDIR)
#LDFLAGS=-lrt -Wl,-rpath=/opt/gcc/gcc-4.7.0/lib64
# LDFLAGS NOTE: -lrt needed for shm_open, etc. for boost interprocess.

LIBS_BASE = ThostStructToString.o OrderInputCache.o OrderInputActionCache.o OrderCache.o OrderActionCache.o Converter.o CFFEX.o TraderSpi.o MarketDataSpi.o SimCFFEXAlt.o

REBUILDABLES = $(LIBS) $(LIBDIR)/$(LIBNAME).a

LIBS := $(foreach s,$(LIBS_BASE),$(OBJDIR)/$s)

clean :
	rm -f $(REBUILDABLES) 
	echo Clean done

$(OBJDIR)/%.o : %.cpp
	$(CXX) $(CPPFLAGS) $(LDFLAGS) -o $@ -c $<

all : $(LIBS)
	ar crf $(LIBDIR)/$(LIBNAME).a $(LIBS)


ThostStructToString.o : THostStructToString.hpp ThostFtdcMdApi.h ThostFtdcTraderApi.h ThostFtdcUserApiDataType.h ThostFtdcUserApiStruct.h $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreUtils.a
OrderInputCache.o : OrderInputCache.hpp ThostFtdcUserApiDataType.h ThostFtdcUserApiStruct.h  $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreUtils.a
OrderInputActionCache.o : OrderInputActionCache.hpp ThostFtdcUserApiDataType.h ThostFtdcUserApiStruct.h $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreUtils.a
OrderCache.o : OrderCache.hpp ThostFtdcUserApiDataType.h ThostFtdcUserApiStruct.h $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreUtils.a
OrderActionCache.o : OrderActionCache.hpp ThostFtdcUserApiDataType.h ThostFtdcUserApiStruct.h $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreUtils.a
Converter.o : Converter.hpp $(COREDIR)/Message.hpp $(COREDIR)/SimulatorCore.hpp OrderCache.hpp ThostFtdcMdApi.h ThostFtdcTraderApi.h ThostFtdcUserApiDataType.h ThostFtdcUserApiStruct.h $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreUtils.a
CFFEX.o : CFFEX.hpp $(COREDIR)/Message.hpp $(COREDIR)/Matchy.hpp $(COREDIR)/SimulatorCore.hpp Converter.hpp OrderCache.hpp OrderActionCache.hpp OrderInputCache.hpp OrderInputActionCache.hpp ThostStructToString.hpp TraderSpi.hpp ThostFtdcMdApi.h ThostFtdcTraderApi.h ThostFtdcUserApiDataType.h ThostFtdcUserApiStruct.h $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreUtils.a
TraderSpi.o : TraderSpi.hpp CFFEX.hpp ThostStructToString.hpp ThostFtdcTraderApi.h $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreUtils.a
MarketDataSpi.o : CFFEX.hpp $(COREDIR)/Message.hpp Converter.hpp TraderSpi.hpp ThostFtdcMdApi.h $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreUtils.a
SimCFFEXAlt.o: CFFEX.hpp $(COREDIR)/Message.hpp $(COREDIR)/Matchy.hpp $(COREDIR)/SimulatorCore.hpp Converter.hpp OrderCache.hpp OrderActionCache.hpp OrderInputCache.hpp OrderInputActionCache.hpp ThostStructToString.hpp TraderSpi.hpp ThostFtdcMdApi.h ThostFtdcTraderApi.h ThostFtdcUserApiDataType.h ThostFtdcUserApiStruct.h $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreUtils.a
