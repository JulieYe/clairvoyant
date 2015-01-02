//
// File:        $ PSS_sim.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/04/21 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <map>
#include <tuple>
#include <vector>
#include <tr1/memory>

using namespace std;

#include "CFFEX.hpp"
#include "PSS.hpp"
#include "SimulatorCore.hpp"
#include "ContractSpec.hpp"
#include "ConfigSpec.hpp"
#include "BreakOut.hpp"

using namespace simulator;

int main(int argc, char ** argv)
{
    string contractSpec_filename(argv[1]);
    string configSpec_filename(argv[2]);
    string filename_trade(argv[3]);
    string exchange_trade(argv[4]);
    string contract_trade(argv[5]);

    string date(argv[6]);
    string log_folder(argv[7]);
    
    double stop_loss(strtod(argv[8], NULL));
    int threshold_trade(atoi(argv[9]));
    int order_size(atoi(argv[10]));
    int numSec(atoi(argv[11]));
    int ntick(atoi(argv[12]));
    int minCount(atoi(argv[13]));

    SimulatorCore sc(date+"T08:59:00.0", date+"T15:16:00.0", 500, "millisecond");
    
    Logger logger(log_folder + "/pssbo_" + date + "_" + contract_trade + ".log");
    logger.start();

    ContractSpec contractSpec(contractSpec_filename);
    string tickerHead = exchange_trade + ":" + contract_trade.substr(0, 2);
    double multiplier, tickSize;
    contractSpec.getMultiplier(tickerHead, multiplier);
    contractSpec.getTickSize(tickerHead, tickSize);

    CTPEXCH exch_trade(exchange_trade, contract_trade, filename_trade, tickSize, &sc);


    ConfigSpec configSpec(configSpec_filename);
    string startTradingTime, exitTradingTime;
    double slippage;
    int medianBookSize;
    configSpec.getStartTradingTime(tickerHead, startTradingTime);
    configSpec.getExitTradingTime(tickerHead, exitTradingTime);
    configSpec.getSlippage(tickerHead, slippage);
    configSpec.getMedianBookSize(tickerHead, medianBookSize);

    PSS<BreakOut> pss(exchange_trade, contract_trade, multiplier, tickSize, startTradingTime, exitTradingTime, &sc, &logger);

    pss.getTradingModel().setParam(numSec, ntick, minCount);
    
    pss.setStopLoss(stop_loss);
    pss.setThreshold(threshold_trade);
    pss.setOrderSize(order_size);

    exch_trade.setSlippage(slippage);
    exch_trade.setMedianBookSize(medianBookSize);

    std::cout << exchange_trade << ","
	      << contract_trade << ","
	      << multiplier << ","
	      << tickSize << ","
	      << startTradingTime << ","
	      << exitTradingTime << std::endl;
		
    sc.registerMatchy(&exch_trade);
    sc.registerStrategy(&pss);
		
    while (sc.next()) {}
    logger.end();

}
