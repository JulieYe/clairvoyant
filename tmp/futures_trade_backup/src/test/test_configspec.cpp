//
// File:        $ test_configspec.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/03/29 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE unit_test_px
#include <boost/test/unit_test.hpp>
#include <cmath>
#include <iostream>
#include <string>

using std::fabs;
using std::string;

#include "ConfigSpec.hpp"

using simulator::ConfigSpec;

static const double tolerance = 1e-6;

BOOST_AUTO_TEST_CASE(test_configspec)
{
    ConfigSpec configSpec("test.config.specs");

    double stopLoss;
    double takeProfit;
    double threshold;
    double volThreshold;
    string startTradingTime;
    string exitTradingTime;
    double slippage;
    int bookSize;
    int buyOpenPosition;
    int sellOpenPosition;
    int orderSize;

    BOOST_CHECK(configSpec.getStopLoss("CFFEX:IF", stopLoss) && fabs(stopLoss+180) < tolerance);
    BOOST_CHECK(configSpec.getTakeProfit("CFFEX:IF", takeProfit) && fabs(takeProfit-180) < tolerance);
    BOOST_CHECK(configSpec.getThreshold("CFFEX:IF", threshold) && fabs(threshold-0.6) < tolerance);
    BOOST_CHECK(configSpec.getVolThreshold("CFFEX:IF", threshold) && fabs(threshold-0.3) < tolerance);
    BOOST_CHECK(configSpec.getStartTradingTime("CFFEX:IF", startTradingTime) && startTradingTime == "09:30:00");
    BOOST_CHECK(configSpec.getExitTradingTime("CFFEX:IF", exitTradingTime) && exitTradingTime == "15:00:00");
    BOOST_CHECK(configSpec.getSlippage("CFFEX:IF", slippage) && fabs(slippage-0.2) < tolerance);
    BOOST_CHECK(configSpec.getMedianBookSize("CFFEX:IF", bookSize) && bookSize == 15);
    BOOST_CHECK(configSpec.getBuyOpenPosition("CFFEX:IF", buyOpenPosition) && buyOpenPosition == 1);
    BOOST_CHECK(configSpec.getSellOpenPosition("CFFEX:IF", sellOpenPosition) && sellOpenPosition == 1);
    BOOST_CHECK(configSpec.getOrderSize("CFFEX:IF", orderSize) && orderSize == 5);
    BOOST_CHECK(!configSpec.getThreshold("SHFE:zz", threshold));
}
