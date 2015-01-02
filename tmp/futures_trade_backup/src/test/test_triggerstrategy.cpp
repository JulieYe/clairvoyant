//
// File:        $ test_triggerstrategy.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/01/26 16:44:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2012 by Zhengzhang Futures Co. Ltd.

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE regression_test_triggerstrategy
#include <boost/test/unit_test.hpp>

#include <fstream>

using std::ifstream;
using std::getline;

#include "CFFEX.hpp"
#include "TriggerStrategy.hpp"

using simulator::Logger;
using simulator::Matchy;
using simulator::CFFEX;
using simulator::SimulatorCore;
using simulator::TriggerStrategy;

BOOST_AUTO_TEST_CASE(triggerstrategy_trading)
{
    {
	SimulatorCore sc("2013-02-04T09:15:00.0", "2013-02-04T15:16:00.0", 500, "millisecond");
	
	Logger logger("../tmp/test_trigger_strategy.csv");
	logger.start();
	CFFEX exch("IF1302", "../test/20130204.IF1302", &sc);
	TriggerStrategy ts("CFFEX", "IF1302", 300.0, &sc, &logger);
	
	sc.registerMatchy(&exch);
	sc.registerStrategy(&ts);
	
	while (sc.next()) {}
	logger.end();
    }
	
    ifstream fd, fg;
    fd.open("../tmp/test_trigger_strategy.csv", ifstream::in);
    fg.open(".gold_trigger_strategy.csv", ifstream::in);
    string lined, lineg;
    while(fd.is_open() && fd.good() && !fd.eof() &&
	  fg.is_open() && fg.good() && !fg.eof() ) {
	getline(fd, lined);
	getline(fg, lineg);
	BOOST_CHECK(lined == lineg);
    }
}
