//
// File:        $ test_marketdatacollector.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/01/26 16:44:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2012 by Zhengzhang Futures Co. Ltd.

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE regression_test_marketdatacollector
#include <boost/test/unit_test.hpp>

#include <cstdlib>
#include <time.h>
#include <algorithm>
#include <vector>

#include "MarketDataCollector.hpp"
#include "SimulatorCore.hpp"
#include "CFFEX.hpp"
#include "Matchy.hpp"

using simulator::Logger;
using simulator::Matchy;
using simulator::CFFEX;
using simulator::MarketDataCollector;
using simulator::SimulatorCore;

class CFFEX2 : public CFFEX
{
public:
	CFFEX2(string contract, string filename, SimulatorCore * simptr):
	    CFFEX(contract, "../test/IF.test.csv", simptr)
		// a hack; we should make parser to be virtual to implement other parsers?
	{ 
	    data_file_.close();
	    bool loadSuccessQ = preLoadData(filename);
	    
	    if (!loadSuccessQ) {
		InitException myexception;
		throw myexception;
	    }
	    
	}

	void parser(string data, BookUpdate<true> *pBookUpdate, TradeUpdate<true> *pTradeUpdate)
	{
	    unsigned pos = data.find("2012-08-15");
	    CFFEX::parser(data.substr(pos), pBookUpdate, pTradeUpdate);
	}
};

BOOST_AUTO_TEST_CASE(marketdatacollector_read_historical_file)
{
    {
	SimulatorCore sc("2012-08-15T09:15:00.0", "2012-08-15T15:16:00.0", 500, "millisecond");
	
	
	Logger logger("../tmp/test_md.csv");
	logger.start();
	CFFEX exch("IF1209", "../test/IF.test.csv", &sc);
	MarketDataCollector mdc(5, &sc, &logger);
	mdc.addStrat("CFFEX", "IF1209", 300.0);
	sc.registerMatchy(&exch);
	sc.registerStrategy(&mdc);
	
	while (sc.next()) {
	}
	logger.end();
    }

    SimulatorCore sc0("2012-08-15T09:15:00.0", "2012-08-15T15:16:00.0", 500, "millisecond");
    CFFEX exch1("IF1209-1", "../test/IF.test.csv", &sc0);
    CFFEX2 exch2("IF1209-2", "../tmp/test_md.csv", &sc0);
    
    exch2.init();
    sc0.registerMatchy(&exch1);
    sc0.registerMatchy(&exch2);
    while (sc0.next() && !exch1.end() && !exch2.end()) {
	for(int ii = 0; ii < 5 ; ii++) {
	    BOOST_CHECK(exch1.getNextBookUpdate().front().getBidPx()[ii] == exch2.getNextBookUpdate().front().getBidPx()[ii]);
	    BOOST_CHECK(exch1.getNextBookUpdate().front().getBidQty()[ii] == exch2.getNextBookUpdate().front().getBidQty()[ii]);
	    BOOST_CHECK(exch1.getNextBookUpdate().front().getAskPx()[ii] == exch2.getNextBookUpdate().front().getAskPx()[ii]);
	    BOOST_CHECK(exch1.getNextBookUpdate().front().getAskQty()[ii] == exch2.getNextBookUpdate().front().getAskQty()[ii]);	
	}
	BOOST_CHECK(exch1.getNextTradeUpdate().front().getPx() == exch2.getNextTradeUpdate().front().getPx());
	BOOST_CHECK(exch1.getNextTradeUpdate().front().getQty() == exch2.getNextTradeUpdate().front().getQty());
    }
}
