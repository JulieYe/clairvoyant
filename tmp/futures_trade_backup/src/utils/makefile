##CXX=/usr/bin/g++
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
LIBNAME = libSimulatorCoreUtils

LIBS_BASE = ConfigReader.o ConfigSpec.o ContractSpec.o FeatureCollector.o FeatureSampler.o ini.o INIReader.o


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

ConfigReader.o: ConfigReader.hpp $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreFeature.a
ini.o: ini.hpp
INIReader.o : ini.hpp INIReader.hpp
ConfigSpec.o : ConfigSpec.hpp $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreFeature.a
ContractSpec.o : ContractSpec.hpp $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreFeature.a
FeatureSampler.o: FeatureSampler.hpp $(FEATUREDIR)/FeatureBase.hpp $(COREDIR)/Message.hpp $(COREDIR)/SimulatorCore.hpp $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreFeature.a
FeatureCollector.o: FeatureCollector.hpp $(FEATUREDIR)/FeatureBase.hpp FeatureSampler.hpp $(COREDIR)/Message.hpp $(COREDIR)/SimulatorCore.hpp $(COREDIR)/Strategy.hpp $(LIBDIR)/libSimulatorCore.a $(LIBDIR)/libSimulatorCoreFeature.a
