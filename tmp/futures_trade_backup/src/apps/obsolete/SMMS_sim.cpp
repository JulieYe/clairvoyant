//
// File:        $ SMMS_sim.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/10/17 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

#include "CFFEX.hpp"
#include "SMMS.hpp"
#include "SimulatorCore.hpp"
#include "ContractSpec.hpp"
#include "ConfigReader.hpp"

using namespace simulator;

int main(int argc, char ** argv)
{
    string pathSpec(argv[1]);
    string contractSpec_filename(argv[2]);
    string filename_front(argv[3]);
    string exchange_front(argv[4]);
    string contract_front(argv[5]);
    string filename_back(argv[6]);
    string exchange_back(argv[7]);
    string contract_back(argv[8]);

    string date(argv[9]);
    string log_folder(argv[10]);
    
    double edge(strtod(argv[11], NULL));
    double buffer(strtod(argv[12], NULL));
    double take_profit(strtod(argv[13], NULL));
    double take_profit_spread(strtod(argv[14], NULL));
    double stop_loss(strtod(argv[15], NULL));
    double stop_loss_spread(strtod(argv[16], NULL));
    int max_spread_position(atoi(argv[17]));
    int max_cancel_qty(atoi(argv[18]));
    int order_size(atoi(argv[19]));

    SimulatorCore sc(date+"T08:59:00.0", date+"T15:16:00.0", 500, "millisecond");
    
    Logger logger(log_folder + "/smms_" + date + "_" + contract_back + "-" + contract_front + ".log");
    logger.start();

    ContractSpec contractSpec(contractSpec_filename);
    string tickerHead = exchange_front + ":" + contract_front.substr(0, 2);
    double multiplier, tickSize;
    contractSpec.getMultiplier(tickerHead, multiplier);
    contractSpec.getTickSize(tickerHead, tickSize);
    CTPEXCH exch_front(exchange_front, contract_front, filename_front, tickSize, &sc);
    CTPEXCH exch_back(exchange_back, contract_back, filename_back, tickSize, &sc);

    ConfigReader cfr(pathSpec + "/SMMS." + exchange_front + "." + contract_front.substr(0, 2) + ".specs");
    string startTradingTime = cfr.getString("StartTradingTime");
    string exitNoonTime = cfr.getString("ExitNoonTime");
    string startNoonTime = cfr.getString("StartNoonTime");
    string exitTradingTime = cfr.getString("ExitTradingTime");
    // double edge = cfr.getDouble("Edge");
    // double take_profit = cfr.getDouble("TakeProfit");
    // double stop_loss = cfr.getDouble("StopLoss");
    // int max_spread_position = cfr.getInt("MaxSpreadPosition");
    // int max_cancel_qty = cfr.getInt("MaxCancelQty");
    // int order_size = cfr.getInt("OrderSize");

    SMMS smms(exchange_front, contract_front, contract_back, multiplier, tickSize, startTradingTime, exitNoonTime, startNoonTime, exitTradingTime, &sc, &logger);
    smms.setEdge(edge);
    smms.setBuffer(buffer);
    smms.setTakeProfit(take_profit, take_profit_spread);
    smms.setStopLoss(stop_loss, stop_loss_spread);
    smms.setMaxSpreadPosition(max_spread_position);
    smms.setMaxCancelQty(max_cancel_qty);
    smms.setOrderSize(order_size);

    sc.registerMatchy(&exch_front);
    sc.registerMatchy(&exch_back);
    sc.registerStrategy(&smms);
		
    while (sc.next()) {}
    logger.end();
}
