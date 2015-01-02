//
// File:        $ LLS_sim.cpp $
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

using namespace std;

#include "CFFEX.hpp"
#include "LLS.hpp"
#include "SimulatorCore.hpp"
#include "ContractSpec.hpp"
#include "ConfigSpec.hpp"

using namespace simulator;

int main(int argc, char ** argv)
{
    string contractSpec_filename(argv[1]);
    string configSpec_filename(argv[2]);
    string filename_trade(argv[3]);
    string exchange_trade(argv[4]);
    string contract_trade(argv[5]);
    string filename_lead(argv[6]);
    string exchange_lead(argv[7]);
    string contract_lead(argv[8]);

    string date(argv[9]);
    string log_folder(argv[10]);
    
    double stop_loss(strtod(argv[11], NULL));
    double take_profit(strtod(argv[12], NULL));
    int threshold_lead(atoi(argv[13]));
    int threshold_trade(atoi(argv[14]));
    int order_size(atoi(argv[15]));
    int num_snapshots(atoi(argv[16]));

    SimulatorCore sc(date+"T08:59:00.0", date+"T15:16:00.0", 500, "millisecond");
    
    Logger logger(log_folder + "/lls_" + date + "_" + contract_trade + ".log");
    logger.start();

    ContractSpec contractSpec(contractSpec_filename);
    string tickerHead = exchange_trade + ":" + contract_trade.substr(0, 2);
    double multiplier, tickSize;
    contractSpec.getMultiplier(tickerHead, multiplier);
    contractSpec.getTickSize(tickerHead, tickSize);

    CTPEXCH exch_trade(exchange_trade, contract_trade, filename_trade, tickSize, &sc);

    string tickerHead_lead = exchange_lead + ":" + contract_lead.substr(0, 2);
    double multiplier_lead, tickSize_lead;
    contractSpec.getMultiplier(tickerHead_lead, multiplier_lead);
    contractSpec.getTickSize(tickerHead_lead, tickSize_lead);

    CTPEXCH exch_lead(exchange_lead, contract_lead, filename_lead, tickSize_lead, &sc);


    ConfigSpec configSpec(configSpec_filename);
    string startTradingTime, exitTradingTime;
    double slippage;
    int medianBookSize;
    configSpec.getStartTradingTime(tickerHead, startTradingTime);
    configSpec.getExitTradingTime(tickerHead, exitTradingTime);
    configSpec.getSlippage(tickerHead, slippage);
    configSpec.getMedianBookSize(tickerHead, medianBookSize);

    LLS lls(exchange_trade, contract_trade, multiplier, tickSize,
	    exchange_lead, contract_lead, tickSize_lead,
	    startTradingTime, exitTradingTime,
	    &sc, &logger);
    lls.setStopLoss(stop_loss);
    lls.setTakeProfit(take_profit);
    lls.setThreshold(threshold_lead, threshold_trade);
    lls.setOrderSize(order_size);
    lls.setNSnapshots(num_snapshots);

    exch_trade.setSlippage(slippage);
    exch_trade.setMedianBookSize(medianBookSize);

    std::cout << exchange_trade << ","
	      << contract_trade << ","
	      << multiplier << ","
	      << tickSize << ","
	      << startTradingTime << ","
	      << exitTradingTime << std::endl;
		
    sc.registerMatchy(&exch_trade);
    sc.registerMatchy(&exch_lead);
    sc.registerStrategy(&lls);
		
    while (sc.next()) {}
    logger.end();

}
