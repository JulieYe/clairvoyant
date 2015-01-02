//
// File:        $ prof_trader.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/11/13 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include <ctime>
#include <string>
#include <sstream>
#include <cstdio>
#include <iostream>

#include "../SimulatorCore.hpp"
#include "../CFFEX.hpp"
#include "../Strategy.hpp"
#include "../Logger.hpp"
#include "../VVDSM.hpp"

using namespace std;
using namespace simulator;

int main(int argc, char ** argv)
{
    // initializing simulator core
    SimulatorCore sc("2013-08-07T09:10:00.0", "2013-08-07T15:20:00.0", 500, "millisecond"); 

    // initializing Logger
    Logger logger("./tmp/prof_trader.log");
    logger.start();

    // initializing Exchange
    string exchange = "CFFEX";
    string contract = "IF1308";

    // load contract specifications
    double multiplier = 300.0;
    double tickSize = 0.2;
    
    // load trading model config
    double stopLoss = -180.0;
    double takeProfit = 180.0;
    double threshold = 0.34;
    double thresholdVol = 0.2;
    string startTradingTime = "09:17:00";
    string exitTradingTime = "15:13:00";
    int orderSize = 1;

    CTPEXCH exch(exchange, contract, "./prof/20130807.IF1308", tickSize, &sc);

    // initializing strategy
    VVDSM vvdsm(exchange, contract, multiplier, tickSize, startTradingTime, exitTradingTime, &sc, &logger);
    vvdsm.setStopLoss(stopLoss);
    vvdsm.setTakeProfit(takeProfit);
    vvdsm.setThreshold(threshold);
    vvdsm.setThresholdVol(thresholdVol);
    vvdsm.setOrderSize(orderSize);

    // register exchange, strategies to simulator core
    sc.registerMatchy(&exch);
    //sc.registerStrategy(&mdc);
    sc.registerStrategy(&vvdsm);

    while (sc.next()) {}
    logger.end();
    
    return 0;
}
