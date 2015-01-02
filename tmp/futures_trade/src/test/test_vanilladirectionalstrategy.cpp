//
// File:        $ test_vanilladirectionalstrategy.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/02/13 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2012 by Zhengzhang Futures Co. Ltd.

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE regression_test_vanilladirectionalstrategy
#include <boost/test/unit_test.hpp>

#include <fstream>

using std::ifstream;
using std::getline;

#include "../CFFEX.hpp"
#include "../VanillaDirectionalStrategy.hpp"

using simulator::Logger;
using simulator::Matchy;
using simulator::CFFEX;
using simulator::SimulatorCore;
using simulator::VanillaDirectionalStrategy;

BOOST_AUTO_TEST_CASE(vanilladirectionalstrategy_trading)
{
    SimulatorCore sc("2013-02-05T09:14:00.0", "2013-02-05T15:16:00.0", 500, "millisecond");
		
    Logger logger("./tmp/test_vanilla_directional_strategy.csv");
    logger.start();
    CFFEX exch("IF1302", "./test/20130205.IF1302", &sc);
    VanillaDirectionalStrategy ts("CFFEX", "IF1302", 300.0, &sc, &logger);
		
    sc.registerMatchy(&exch);
    sc.registerStrategy(&ts);
		
    while (sc.next()) {}
    logger.end();
}
