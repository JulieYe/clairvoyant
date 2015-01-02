//
// File:        $ test_contractspec.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/03/20 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE unit_test_px
#include <boost/test/unit_test.hpp>
#include <cmath>
#include <iostream>

using std::fabs;

#include "ContractSpec.hpp"

using simulator::ContractSpec;

static const double tolerance = 1e-6;

BOOST_AUTO_TEST_CASE(test_contractspec)
{
    ContractSpec contractSpec("../test/test.contract.specs");

    double multiplier;
    double tickSize;
    string startTradingTime;
    string stopTradingTime;

    BOOST_CHECK(contractSpec.getMultiplier("CFFEX:IF", multiplier) && fabs(multiplier-300) < tolerance);
    BOOST_CHECK(contractSpec.getTickSize("CFFEX:IF", tickSize) && fabs(tickSize-0.2) < tolerance);
    BOOST_CHECK(contractSpec.getDefaultStartTradingTime("CFFEX:IF", startTradingTime) && startTradingTime == "09:15:00");
    BOOST_CHECK(contractSpec.getDefaultExitTradingTime("CFFEX:IF", stopTradingTime) && stopTradingTime == "15:15:00");
    BOOST_CHECK(contractSpec.getMultiplier("SHFE:rb", multiplier) && fabs(multiplier-10) < tolerance && 
		contractSpec.getTickSize("SHFE:rb", tickSize) && fabs(tickSize-1) < tolerance &&
		contractSpec.getDefaultStartTradingTime("SHFE:rb", startTradingTime) && startTradingTime == "09:00:00" &&
		contractSpec.getDefaultExitTradingTime("SHFE:rb", stopTradingTime) && stopTradingTime == "15:00:00");
    BOOST_CHECK(!contractSpec.getMultiplier("SHFE:zz", multiplier));
}
