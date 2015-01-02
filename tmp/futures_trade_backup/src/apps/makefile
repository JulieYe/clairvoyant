#CXX=/usr/bin/g++
ROOTDIR = ..
include $(ROOTDIR)/make.begin.mk
include $(ROOTDIR)/make.env.mk

BINDIR = ../bin
OBJDIR = ../obj
TMPDIR = ../tmp
LIBDIR = ../lib
COREDIR = ../core
FEATUREDIR = ../feature
ALPHADIR = ../alpha
UTILSDIR = ../utils
STRATEGYDIR = ../strategy
APIDIR = ../api
APICTPDIR = ../api/CTP

CPPFLAGS=$(CPPFLAGS_COMMON) -D$(SPFLAG) -D$(DFLAG) -D$(XFLAG) -D$(SAHFLAG) #-Wsign-conversion 
LDFLAGS=$(LDFLAGS_COMMON) -L$(LIBDIR) -lSimulatorCoreStrategy -lSimulatorCore -lSimulatorCoreFeature -lSimulatorCoreAlpha -lSimulatorCoreApi -lSimulatorCoreCTPApi  -lSimulatorCoreUtils -I$(COREDIR) -I$(FEATUREDIR) -I$(ALPHADIR) -I$(UTILSDIR) -I$(APIDIR) -I$(APICTPDIR) -I$(STRATEGYDIR)
LDFLAGSPROD= -L$(LIBDIR) -lSimulatorCoreStrategy -lSimulatorCore -lSimulatorCoreFeature -lSimulatorCoreAlpha -lSimulatorCoreApi -lSimulatorCoreCTPApi  -lSimulatorCoreUtils -I$(COREDIR) -I$(FEATUREDIR) -I$(ALPHADIR) -I$(UTILSDIR) -I$(APIDIR) -I$(APICTPDIR) -I$(STRATEGYDIR) $(LDFLAGSPROD_COMMON) -L. -lthosttraderapi -lthostmduserapi -Wl,-rpath,.

#LDFLAGS=-lrt -Wl,-rpath=/opt/gcc/gcc-4.7.0/lib64
# LDFLAGS NOTE: -lrt needed for shm_open, etc. for boost interprocess.

LIBS_BASE = SimMain.o FeatureGenerator.o

APPLICATIONS_BASE = FeatureGenerator SimMain

LIBS := $(foreach s,$(LIBS_BASE),$(OBJDIR)/$s)
APPLICATIONS := $(foreach s,$(APPLICATIONS_BASE),$(BINDIR)/$s)
TMPFILES = $(ROOTDIR)/test/sim_2013-02-04_VVDSM_IF.log

REBUILDABLES = $(LIBS) $(APPLICATIONS) $(TMPFILES)

echo_libs :
	echo $(LIBS)
	echo $(APPLICATIONS)

clean :
	rm -f $(REBUILDABLES) 
	echo Clean done

$(OBJDIR)/%.o : %.cpp
	$(CXX) $(CPPFLAGS) $(LDFLAGS) -o $@ -c $<

FeatureGenerator : FeatureGenerator.cpp 
	$(CXX) -o $(BINDIR)/$@ $^ $(LDFLAGS) $(CPPFLAGS)

SimMain : SimMain.cpp 
	$(CXX) -o $(BINDIR)/$@ $^ $(LDFLAGS) $(CPPFLAGS)

CTPTrader : CTPTrader.cpp
	$(CXX) -o $(BINDIR)/$@ $^ $(LDFLAGSPROD) $(CPPFLAGS)

test_simmain : SimMain
	$(BINDIR)/SimMain cfg/simmain.cfg
	diff -q $(ROOTDIR)/test/sim_2013-02-04_VVDSM_IF.log $(ROOTDIR)/test/.gold_VVDSM_IF.log
	echo "SimMain test passed for VVDSM"


SimMain.o : SimulatorCore.hpp ObjectFactory.hpp StrategyRegistry.hpp MatchyRegistry.hpp INIReader.hpp Logger.hpp $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreFeature.a $(LIBDIR)/libSimulatorCoreAlpha.a $(LIBDIR)/libSimulatorCoreUtils.a $(LIBDIR)/libSimulatorCoreStrategy.a $(LIBDIR)/libSimulatorCoreApi.a $(LIBDir)/libSimulatorCoreCTPApi.a
FeatureGenerator.o : Feature.hpp CFFEX.hpp FeatureCollector.hpp ContractSpec.hpp ConfigSpec.hpp $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreFeature.a $(LIBDIR)/libSimulatorCoreAlpha.a $(LIBDIR)/libSimulatorCoreUtils.a $(LIBDIR)/libSimulatorCoreStrategy.a $(LIBDIR)/libSimulatorCoreApi.a $(LIBDir)/libSimulatorCoreCTPApi.a
