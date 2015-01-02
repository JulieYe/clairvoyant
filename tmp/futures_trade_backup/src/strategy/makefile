##CXX=/usr/bin/g++
ROOTDIR = ..
include $(ROOTDIR)/make.begin.mk
include $(ROOTDIR)/make.env.mk

OBJDIR = ../obj
LIBDIR = ../lib
COREDIR = ../core
FEATUREDIR = ../feature
UTILSDIR = ../utils
ALPHADIR = ../alpha

LIBNAME = libSimulatorCoreStrategy

#CPPFLAGS=-g -Wall -pedantic -std=c++11 -I/usr/local/include -pthread 
CPPFLAGS=$(CPPFLAGS_COMMON) -D$(SPFLAG) -D$(SAHFLAG)
LDFLAGS=$(LDFLAGS_COMMON) -L$(LIBDIR) -lSimulatorCore -lSimulatorCoreFeature -lSimulatorCoreUtils -lSimulatorCoreAlpha -I$(COREDIR) -I$(FEATUREDIR) -I$(UTILSDIR) -I$(ALPHADIR)


#LDFLAGS=-lrt -Wl,-rpath=/opt/gcc/gcc-4.7.0/lib64
# LDFLAGS NOTE: -lrt needed for shm_open, etc. for boost interprocess.

LIBS_BASE = TriggerStrategy.o VanillaDirectionalStrategy.o VVDSM.o LLS.o SpreadMM.o StrategyRegistry.o MarketDataCollector.o


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
	$(CXX) $(LDFLAGS) $(CPPFLAGS) -o $@ -c $<

TriggerStrategy.o : TriggerStrategy.hpp $(COREDIR)/Strategy.hpp $(COREDIR)/SimulatorCore.hpp $(COREDIR)/Logger.hpp $(FEATUREDIR)/Signal.hpp $(FEATUREDIR)/Feature.hpp $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreFeature.a $(LIBDIR)/libSimulatorCoreUtils.a $(LIBDIR)/libSimulatorCoreAlpha.a
VanillaDirectionalStrategy.o : VanillaDirectionalStrategy.hpp $(COREDIR)/Strategy.hpp $(COREDIR)/SimulatorCore.hpp $(COREDIR)/Logger.hpp $(FEATUREDIR)/Signal.hpp $(FEATUREDIR)/Feature.hpp $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreFeature.a $(LIBDIR)/libSimulatorCoreUtils.a $(LIBDIR)/libSimulatorCoreAlpha.a
VVDSM.o : VVDSM.hpp $(COREDIR)/Px.hpp $(COREDIR)/Strategy.hpp $(FEATUREDIR)/Signal.hpp $(FEATUREDIR)/Feature.hpp $(COREDIR)/SimulatorCore.hpp $(COREDIR)/Logger.hpp $(UTILSDIR)/INIReader.hpp $(COREDIR)/ObjectFactory.hpp $(ALPHADIR)/IF.hpp $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreFeature.a $(LIBDIR)/libSimulatorCoreUtils.a $(LIBDIR)/libSimulatorCoreAlpha.a
LLS.o : LLS.hpp $(COREDIR)/Strategy.hpp $(COREDIR)/Logger.hpp $(FEATUREDIR)/Feature.hpp $(UTILSDIR)/INIReader.hpp $(COREDIR)/ObjectFactory.hpp $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreFeature.a $(LIBDIR)/libSimulatorCoreUtils.a $(LIBDIR)/libSimulatorCoreAlpha.a
SpreadMM.o : SpreadMM.hpp $(COREDIR)/Px.hpp $(COREDIR)/Strategy.hpp $(FEATUREDIR)/Feature.hpp $(COREDIR)/SimulatorCore.hpp $(COREDIR)/Logger.hpp $(UTILSDIR)/INIReader.hpp $(COREDIR)/ObjectFactory.hpp $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreFeature.a $(LIBDIR)/libSimulatorCoreUtils.a $(LIBDIR)/libSimulatorCoreAlpha.a
StrategyRegistry.o : StrategyRegistry.hpp $(COREDIR)/ObjectFactory.hpp VVDSM.hpp SpreadMM.hpp $(ALPHADIR)/IF.hpp LLS.hpp $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreFeature.a $(LIBDIR)/libSimulatorCoreUtils.a $(LIBDIR)/libSimulatorCoreAlpha.a
MarketDataCollector.o : MarketDataCollector.hpp $(COREDIR)/SimulatorCore.hpp $(COREDIR)/Strategy.hpp $(COREDIR)/Message.hpp $(COREDIR)/Logger.hpp $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreFeature.a $(LIBDIR)/libSimulatorCoreUtils.a $(LIBDIR)/libSimulatorCoreAlpha.a
