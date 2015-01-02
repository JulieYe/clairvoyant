
#CXX=/usr/bin/g++
ROOTDIR = ..
include $(ROOTDIR)/make.begin.mk
include $(ROOTDIR)/make.env.sim.mk

BINDIR = ../bin
TESTDIR = ../test
OBJDIR = ../obj
TMPDIR = ../tmp
LIBDIR = ../lib

COREDIR = ../core
FEATUREDIR = ../feature
CTPAPIDIR = ../api/CTP
ALPHADIR = ../alpha
UTILSDIR = ../utils
STRATEGYDIR = ../strategy

UNIT_TESTS_BASE = test_px test_message test_logger test_strategy test_orderbook test_matchy test_cffex test_simulatorcore test_marketdatacollector test_featurecollector test_feature test_converter test_triggerstrategy test_vanilladirectionalstrategy test_contractspec test_configspec test_technical test_configreader

TEMP_TEST_BASE = test.log test_fc.csv test_md.csv test_feature.csv test_trigger_strategy.csv test_vanilla_directional_strategy.csv test_strategy.log test_configreader.test.config

UNIT_TESTS := $(foreach s,$(UNIT_TESTS_BASE),$(TESTDIR)/$s)
TEMP_TEST_FILES := $(foreach s,$(TEMP_TEST_BASE),$(TMPDIR)/$s)

CPPFLAGS=$(CPPFLAGS_COMMON) -D$(SPFLAG) -D$(DFLAG) -D$(XFLAG) -D$(SAHFLAG) #-Wsign-conversion 

LDFLAGS=$(LDFLAGS_COMMON) -L$(LIBDIR) -lSimulatorCore -lSimulatorCoreAlpha -lSimulatorCoreCTPApi  -lSimulatorCoreStrategy -lSimulatorCoreUtils -I$(COREDIR) -I$(FEATUREDIR) -I$(CTPAPIDIR) -I$(ALPHADIR) -I$(UTILSDIR) -I$(STRATEGYDIR) -lSimulatorCoreFeature

LDFLAGSTEST=-lboost_unit_test_framework

$(UNIT_TESTS) : $(TESTDIR)/test_% : $(TESTDIR)/test_%.cpp 
	$(CXX) -o $@ $^ $(LDFLAGS) $(LDFLAGSTEST) $(CPPFLAGS) 


clean :
	rm -f $(UNIT_TESTS) $(TEMP_TEST_FILES)

test : $(UNIT_TESTS)
	for TEST in $(UNIT_TESTS); do \
		echo running test $$TEST && \
		./$$TEST; \
	done
	echo All tests done
