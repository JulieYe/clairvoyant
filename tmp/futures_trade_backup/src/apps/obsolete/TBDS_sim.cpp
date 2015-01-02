//
// File:        $ TBDS_sim.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/08/11 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

#include "CFFEX.hpp"
#include "TBDS.hpp"
#include "SimulatorCore.hpp"
#include "ContractSpec.hpp"
#include "ConfigSpec.hpp"

using namespace simulator;

int main(int argc, char ** argv)
{
    string contractSpec_filename(argv[1]);
    string configSpec_filename(argv[2]);
    string filename(argv[3]);
    string date(argv[4]);
    string exchange(argv[5]);
    string contract(argv[6]);
    string log_folder(argv[7]);
    double threshold(strtod(argv[8], NULL));
    double thresholdVol(strtod(argv[9], NULL));
    int order_size(atoi(argv[10]));
    int latency_ms(atoi(argv[11]));

    SimulatorCore sc(date+"T08:59:00.0", date+"T15:16:00.0", 500, "millisecond");
    sc.setLatency(latency_ms);
    
    Logger logger(log_folder + "/tbds_" + date + "_" + contract + ".log");
    logger.start();

    ContractSpec contractSpec(contractSpec_filename);
    string tickerHead = exchange + ":" + contract.substr(0, 2);
    double multiplier, tickSize;
    contractSpec.getMultiplier(tickerHead, multiplier);
    contractSpec.getTickSize(tickerHead, tickSize);

    CTPEXCH exch(exchange, contract, filename, tickSize, &sc);

    ConfigSpec configSpec(configSpec_filename);
    string startTradingTime, exitTradingTime;
    double slippage;
    int medianBookSize;
    configSpec.getStartTradingTime(tickerHead, startTradingTime);
    configSpec.getExitTradingTime(tickerHead, exitTradingTime);
    configSpec.getSlippage(tickerHead, slippage);
    configSpec.getMedianBookSize(tickerHead, medianBookSize);

    TBDS vvds(exchange, contract, multiplier, tickSize, startTradingTime, exitTradingTime, &sc, &logger);
    exch.setSlippage(slippage);
    exch.setMedianBookSize(medianBookSize);

    std::cout << exchange << ","
	      << contract << ","
	      << multiplier << ","
	      << tickSize << ","
	      << startTradingTime << ","
	      << exitTradingTime << std::endl;
		
    vvds.setThreshold(threshold);
    vvds.setThresholdVol(thresholdVol);
    vvds.setOrderSize(order_size);

    sc.registerMatchy(&exch);
    sc.registerStrategy(&vvds);

    while (sc.next()) {}
    logger.end();
}
