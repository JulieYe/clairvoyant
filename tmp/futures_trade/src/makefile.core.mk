CXX=/usr/bin/g++
#CPPFLAGS=-O2 -pg -g -Wall -pedantic -std=c++11 -I/usr/local/include -pthread -D$(SPFLAG) -D$(DFLAG) -D$(BFLAG) -D$(THFLAG) -D$(XFLAG) -D$(SAHFLAG)
CPPFLAGS=-O2 -Wall -pedantic -std=c++11 -I/usr/local/include -pthread -D$(SPFLAG) -D$(DFLAG) -D$(BFLAG) -D$(THFLAG) -D$(XFLAG) -D$(SAHFLAG)
LDFLAGS=-lrt -lpthread -L/usr/local/lib
LDFLAGSTEST=-lboost_unit_test_framework
LDFLAGSPROD=-lrt -lpthread -L/usr/local/lib -L. -lthosttraderapi -lthostmduserapi -Wl,-rpath,.
LDFLAGSPROF=-pg

#LDFLAGS=-lrt -Wl,-rpath=/opt/gcc/gcc-4.7.0/lib64
# LDFLAGS NOTE: -lrt needed for shm_open, etc. for boost interprocess.
BINDIR = ./bin
TESTDIR = ./test
OBJDIR = ./obj
TMPDIR = ./tmp
PROFDIR = ./prof

#LIBS_BASE = Px.o Message.o SimulatorCore.o Matchy.o CFFEX.o OrderBook.o Strategy.o Logger.o Feature.o FeatureSampler.o Signal.o Converter.o MarketDataCollector.o FeatureCollector.o MarketDataSpi.o OrderCache.o OrderActionCache.o OrderInputCache.o OrderInputActionCache.o TriggerStrategy.o VanillaDirectionalStrategy.o ThostStructToString.o TraderSpi.o VUDS.o VVDSM.o MMS.o SMMS.o SpreadMM.o TBDS.o MMSpreader.o IF.o AG.o TF.o ContractSpec.o ConfigSpec.o IncludeHelper.o QtEMA.o LLS.o Technical.o ConfigReader.o PrintingStack.o SMS.o

LIBS_BASE = Px.o Message.o SimulatorCore.o Matchy.o CFFEX.o OrderBook.o Strategy.o Logger.o Feature.o Converter.o MarketDataSpi.o OrderCache.o OrderActionCache.o OrderInputCache.o OrderInputActionCache.o ThostStructToString.o TraderSpi.o SpreadMM.o ContractSpec.o QtEMA.o ConfigReader.o PrintingStack.o Technical.o

UNIT_TESTS_BASE = test_px test_message test_logger test_strategy test_orderbook test_matchy test_cffex test_simulatorcore test_marketdatacollector test_featurecollector test_feature test_converter test_triggerstrategy test_vanilladirectionalstrategy test_contractspec test_configspec test_technical test_configreader

PROFS_BASE = prof_trader

APPLICATIONS_BASE = FeatureGenerator SpreadFeatureGenerator MdCollector Trader TraderDummy TraderDummyDebug VDS_sim VVDSM_sim VLDS_sim MMS_sim SMMS_sim LLS_sim TBDS_sim Trader2 Trader3 TraderMMS TraderSMMS TraderSpreadMM TraderTBDS SMS_sim

TEMP_TEST_BASE = test.log test_fc.csv test_md.csv test_feature.csv test_trigger_strategy.csv test_vanilla_directional_strategy.csv test_strategy.log

TEMP_PROF_BASE = prof_trader.log gprof.csv

REBUILDABLES = $(LIBS) $(TEMP_TEST_FILES) $(TEMP_PROF_FILES)

LIBS := $(foreach s,$(LIBS_BASE),$(OBJDIR)/$s)
UNIT_TESTS := $(foreach s,$(UNIT_TESTS_BASE),$(TESTDIR)/$s)
PROFS := $(foreach s,$(PROFS_BASE),$(PROFDIR)/$s)
APPLICATIONS := $(foreach s,$(APPLICATIONS_BASE),$(BINDIR)/$s)
TEMP_TEST_FILES := $(foreach s,$(TEMP_TEST_BASE),$(TMPDIR)/$s)
TEMP_PROF_FILES := $(foreach s,$(TEMP_PROF_BASE),$(TMPDIR)/$s)
GMON.OUT := ./gmon.out

echo_libs :
	echo $(LIBS)
	echo $(UNIT_TESTS)
	echo $(APPLICATIONS)
	echo $(TEMP_TEST_FILES)
	echo $(TEMP_PROF_FILES)

clean :
	rm -f $(REBUILDABLES) $(UNIT_TESTS) $(PROFS) $(GMON.OUT) $(APPLICATIONS)
	echo Clean done

test : clean $(UNIT_TESTS)
	for TEST in $(UNIT_TESTS); do \
		echo running test $$TEST && \
		./$$TEST; \
	done
	echo All tests done

prof : clean $(PROFS)
	for PROF in $(PROFS); do \
		echo running prof $$PROF && \
		./$$PROF && \
	        gprof ./$$PROF >> $(TMPDIR)/gprof.csv; \
	done
	echo All profiles done

$(PROFS) : $(PROFDIR)/prof_% : $(LIBS) $(PROFDIR)/prof_%.cpp
	$(CXX) -o $@ $^ $(LDFLAGS) $(LDFLAGSPROF) $(CPPFLAGS) 

$(UNIT_TESTS) : $(TESTDIR)/test_% : $(LIBS) $(TESTDIR)/test_%.cpp 
	$(CXX) -o $@ $^ $(LDFLAGS) $(LDFLAGSTEST) $(CPPFLAGS) 

$(OBJDIR)/%.o : %.cpp
	$(CXX) $(CPPFLAGS) -o $@ -c $<

FeatureGenerator : FeatureGenerator.cpp $(LIBS)
	$(CXX) -o $(BINDIR)/$@ $^ $(LDFLAGS) $(CPPFLAGS)

SpreadFeatureGenerator : SpreadFeatureGenerator.cpp $(LIBS)
	$(CXX) -o $(BINDIR)/$@ $^ $(LDFLAGS) $(CPPFLAGS)

MdCollector : MdCollector.cpp $(LIBS)
	$(CXX) -o $(BINDIR)/$@ $^ $(LDFLAGSPROD) $(CPPFLAGS)

Trader : Trader.cpp $(LIBS)
	$(CXX) -o $(BINDIR)/$@ $^ $(LDFLAGSPROD) $(CPPFLAGS)

Trader2 : Trader2.cpp $(LIBS)
	$(CXX) -o $(BINDIR)/$@ $^ $(LDFLAGSPROD) $(CPPFLAGS)

Trader3 : Trader3.cpp $(LIBS)
	$(CXX) -o $(BINDIR)/$@ $^ $(LDFLAGSPROD) $(CPPFLAGS)

TraderMMS : TraderMMS.cpp $(LIBS)
	$(CXX) -o $(BINDIR)/$@ $^ $(LDFLAGSPROD) $(CPPFLAGS)

TraderSMMS : TraderSMMS.cpp $(LIBS)
	$(CXX) -o $(BINDIR)/$@ $^ $(LDFLAGSPROD) $(CPPFLAGS)

TraderSpreadMM : TraderSpreadMM.cpp $(LIBS)
	$(CXX) -o $(BINDIR)/$@ $^ $(LDFLAGSPROD) $(CPPFLAGS)

TraderTBDS : TraderTBDS.cpp $(LIBS)
	$(CXX) -o $(BINDIR)/$@ $^ $(LDFLAGSPROD) $(CPPFLAGS)

TraderDummy : TraderDummy.cpp $(LIBS)
	$(CXX) -o $(BINDIR)/$@ $^ $(LDFLAGSPROD) $(CPPFLAGS)

TraderDummyDebug : TraderDummyDebug.cpp $(LIBS)
	$(CXX) -o $(BINDIR)/$@ $^ $(LDFLAGSPROD) $(CPPFLAGS)

VDS_sim : VDS_sim.cpp $(LIBS)
	$(CXX) -o $(BINDIR)/$@ $^ $(LDFLAGS) $(CPPFLAGS)

VVDSM_sim : VVDSM_sim.cpp $(LIBS)
	$(CXX) -o $(BINDIR)/$@ $^ $(LDFLAGS) $(CPPFLAGS)

VLDS_sim : VLDS_sim.cpp VLDS.hpp $(LIBS)
	$(CXX) -o $(BINDIR)/$@ $^ $(LDFLAGS) $(CPPFLAGS)

LLS_sim : LLS_sim.cpp $(LIBS)
	$(CXX) -o $(BINDIR)/$@ $^ $(LDFLAGS) $(CPPFLAGS)

PSSBO_sim : PSSBO_sim.cpp $(LIBS)
	$(CXX) -o $(BINDIR)/$@ $^ $(LDFLAGS) $(CPPFLAGS)

MMS_sim : MMS_sim.cpp $(LIBS)
	$(CXX) -o $(BINDIR)/$@ $^ $(LDFLAGS) $(CPPFLAGS)

SMMS_sim : SMMS_sim.cpp $(LIBS)
	$(CXX) -o $(BINDIR)/$@ $^ $(LDFLAGS) $(CPPFLAGS)

SMS_sim : SMS_sim.cpp $(LIBS)
	$(CXX) -o $(BINDIR)/$@ $^ $(LDFLAGS) $(CPPFLAGS)

TBDS_sim : TBDS_sim.cpp $(LIBS)
	$(CXX) -o $(BINDIR)/$@ $^ $(LDFLAGS) $(CPPFLAGS)

Message.o : Message.hpp
Px.o : Px.hpp
SimulatorCore.o : SimulatorCore.hpp Message.hpp Matchy.hpp Strategy.hpp
Matchy.o : Matchy.hpp Px.hpp Message.hpp OrderBook.hpp SimulatorCore.hpp
OrderBook.o : OrderBook.hpp Px.hpp Message.hpp
CFFEX.o : CFFEX.hpp Matchy.hpp Converter.hpp OrderCache.hpp OrderActionCache.hpp OrderInputCache.hpp OrderInputActionCache.hpp
Strategy.o : Strategy.hpp SimulatorCore.hpp Logger.hpp
Logger.o : Logger.hpp PrintingStack.hpp
Feature.o : Feature.hpp Message.hpp QtEMA.hpp
FeatureSampler.o : FeatureSampler.hpp Feature.hpp Message.hpp
Signal.o : Signal.hpp Feature.hpp Message.hpp
MarketDataCollector.o : MarketDataCollector.hpp SimulatorCore.hpp Strategy.hpp Message.hpp Logger.hpp
FeatureCollector.o: FeatureCollector.hpp SimulatorCore.hpp Strategy.hpp Message.hpp Logger.hpp
MarketDataSpi.o : MarketDataSpi.hpp CFFEX.hpp
OrderCache.o : OrderCache.hpp ThostFtdcUserApiDataType.h ThostFtdcUserApiStruct.h
OrderActionCache.o : OrderActionCache.hpp ThostFtdcUserApiDataType.h ThostFtdcUserApiStruct.h
OrderInputCache.o : OrderInputCache.hpp ThostFtdcUserApiDataType.h ThostFtdcUserApiStruct.h
OrderInputActionCache.o : OrderInputActionCache.hpp ThostFtdcUserApiDataType.h ThostFtdcUserApiStruct.h
Converter.o : Converter.hpp OrderCache.hpp Message.hpp SimulatorCore.hpp
TriggerStrategy.o : TriggerStrategy.hpp Strategy.hpp Signal.hpp Feature.hpp SimulatorCore.hpp Logger.hpp
VanillaDirectionalStrategy.o : VanillaDirectionalStrategy.hpp Strategy.hpp Signal.hpp Feature.hpp SimulatorCore.hpp Logger.hpp
ThostStructToString.o : ThostStructToString.hpp
TraderSpi.o : TraderSpi.hpp ThostFtdcTraderApi.h ThostStructToString.hpp CFFEX.hpp
IF.o : IF.hpp Signal.hpp Feature.hpp Px.hpp
AG.o : AG.hpp Signal.hpp Feature.hpp Px.hpp
TF.o : TF.hpp Signal.hpp Feature.hpp Px.hpp
VUDS.o : VUDS.hpp Strategy.hpp Signal.hpp Feature.hpp SimulatorCore.hpp Logger.hpp
VVDSM.o : VVDSM.hpp Strategy.hpp Signal.hpp Feature.hpp SimulatorCore.hpp Logger.hpp
MMS.o : MMS.hpp Strategy.hpp Signal.hpp Feature.hpp SimulatorCore.hpp Logger.hpp
SMMS.o : SMMS.hpp Strategy.hpp Signal.hpp Feature.hpp SimulatorCore.hpp Logger.hpp
SpreadMM.o : SpreadMM.hpp Strategy.hpp Signal.hpp Feature.hpp SimulatorCore.hpp Logger.hpp
SMS.o : SMS.hpp Strategy.hpp Signal.hpp Feature.hpp SimulatorCore.hpp Logger.hpp
MMSpreader.o : MMSpreader.hpp Strategy.hpp SimulatorCore.hpp Logger.hpp
LLS.o : LLS.hpp Strategy.hpp SimulatorCore.hpp Logger.hpp
TBDS.o : TBDS.hpp Strategy.hpp Signal.hpp Feature.hpp SimulatorCore.hpp Logger.hpp
ContractSpec.o : ContractSpec.hpp
ConfigSpec.o : ConfigSpec.hpp
IncludeHelper.o : IncludeHelper.hpp
QtEMA.o : QtEMA.hpp
PrintingStack.o : PrintingStack.hpp
