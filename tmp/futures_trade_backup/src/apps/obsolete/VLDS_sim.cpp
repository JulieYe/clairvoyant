//
// File:        $ VLDS_sim.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/04/07 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

#include "CFFEX.hpp"
#include "VLDS.hpp"
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
    double stop_loss(strtod(argv[8], NULL));
    double take_profit(strtod(argv[9], NULL));
    double threshold(strtod(argv[10], NULL));
    int order_size(atoi(argv[11]));
    int latency_ms(atoi(argv[12]));

    SimulatorCore sc(date+"T08:59:00.0", date+"T15:16:00.0", 500, "millisecond");
    sc.setLatency(latency_ms);
    
    Logger logger(log_folder + "/vds_" + date + "_" + contract + ".log");
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

    Strategy * vlds=NULL;
    if(contract.substr(0,2) == "IF") {
	vlds = new VLDS<IF>(exchange, contract, multiplier, tickSize, startTradingTime, exitTradingTime, &sc, &logger);
	dynamic_cast<VLDS<IF> * >(vlds)->setStopLoss(stop_loss);
	dynamic_cast<VLDS<IF> * >(vlds)->setTakeProfit(take_profit);
	dynamic_cast<VLDS<IF> * >(vlds)->setThreshold(threshold);
	dynamic_cast<VLDS<IF> * >(vlds)->setOrderSize(order_size);

    } else if (contract.substr(0,2) == "ru") {
	vlds = new VLDS<RU>(exchange, contract, multiplier, tickSize, startTradingTime, exitTradingTime, &sc, &logger);	
	dynamic_cast<VLDS<RU> * >(vlds)->setStopLoss(stop_loss);
	dynamic_cast<VLDS<RU> * >(vlds)->setTakeProfit(take_profit);
	dynamic_cast<VLDS<RU> * >(vlds)->setThreshold(threshold);
	dynamic_cast<VLDS<RU> * >(vlds)->setOrderSize(order_size);
    } else if (contract.substr(0,2) == "rb") {
	vlds = new VLDS<RB>(exchange, contract, multiplier, tickSize, startTradingTime, exitTradingTime, &sc, &logger);
	dynamic_cast<VLDS<RB> * >(vlds)->setStopLoss(stop_loss);
	dynamic_cast<VLDS<RB> * >(vlds)->setTakeProfit(take_profit);
	dynamic_cast<VLDS<RB> * >(vlds)->setThreshold(threshold);
	dynamic_cast<VLDS<RB> * >(vlds)->setOrderSize(order_size);
    } else {
	std::cerr << "Unknown contract head: " << contract << std::endl;
	exit(1);
    }

    exch.setSlippage(slippage);
    exch.setMedianBookSize(medianBookSize);

    std::cout << exchange << ","
	      << contract << ","
	      << multiplier << ","
	      << tickSize << ","
	      << startTradingTime << ","
	      << exitTradingTime << std::endl;
		

    sc.registerMatchy(&exch);
    sc.registerStrategy(vlds);
		
    while (sc.next()) {}
    logger.end();
    delete vlds;
}
