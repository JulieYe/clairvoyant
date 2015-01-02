//
// File:        $ VVDSM_sim.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/08/17 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

#include "CFFEX.hpp"
#include "VUDS.hpp"
#include "VVDSM.hpp"
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
    double thresholdVol(strtod(argv[13], NULL));

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

    VVDSM vvds(exchange, contract, multiplier, tickSize, startTradingTime, exitTradingTime, &sc, &logger);
    //VUDS vvds(exchange, contract, multiplier, tickSize, startTradingTime, exitTradingTime, &sc, &logger);
    exch.setSlippage(slippage);
    exch.setMedianBookSize(medianBookSize);

    std::cout << exchange << ","
	      << contract << ","
	      << multiplier << ","
	      << tickSize << ","
	      << startTradingTime << ","
	      << exitTradingTime << std::endl;
		
    vvds.setStopLoss(stop_loss);
    vvds.setTakeProfit(take_profit);
    vvds.setThreshold(threshold);
    vvds.setThresholdVol(thresholdVol);
    vvds.setOrderSize(order_size);

    sc.registerMatchy(&exch);
    sc.registerStrategy(&vvds);

    while (sc.next()) {}
    logger.end();
}
