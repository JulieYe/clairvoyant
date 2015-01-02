//
// File:        $ Trader2.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/05/26 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include <ctime>
#include <string>
#include <sstream>
#include <cstdio>
#include <iostream>

#include "ThostFtdcMdApi.h"
#include "ThostFtdcTraderApi.h"
#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcUserApiStruct.h"

#include "MarketDataSpi.hpp"
#include "TraderSpi.hpp"
#include "SimulatorCore.hpp"
#include "CFFEX.hpp"
#include "MarketDataCollector.hpp"
#include "Strategy.hpp"
#include "Logger.hpp"
#include "VVDSM.hpp"
#include "LLS.hpp"
#include "ContractSpec.hpp"
#include "ConfigSpec.hpp"

using namespace std;
using namespace simulator;

int main(int argc, char ** argv)
{
    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );

    char yyyymmdd[11];

    sprintf(yyyymmdd, "%d-%02d-%02d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);

    stringstream ss;

#ifdef DEBUG
    cout << "Trader starts on " << yyyymmdd << endl;
#endif

    ss << yyyymmdd;

    // initializing simulator core
    SimulatorCore sc(ss.str() + "T08:55:00.0", ss.str() + "T15:20:00.0", 10, "millisecond"); 

#ifdef DEBUG
    cout << "Current local system-side time is " 
	 << SimulatorCore::convertTimePointToString(sc.getCurrTime()) 
	 << endl;
#endif

    // initializing Market Data Api and Trader Api
#ifdef EXCHANGE_CFFEX
    CThostFtdcMdApi *pMdApi = CThostFtdcMdApi::CreateFtdcMdApi("/home/mingyuan/packages/con/Md.CFFEX.con/", true);
#elif defined EXCHANGE_SHFE
    CThostFtdcMdApi *pMdApi = CThostFtdcMdApi::CreateFtdcMdApi("/home/mingyuan/packages/con/Md.SHFE.con/", true);
#endif

#ifdef EXCHANGE_CFFEX
    CThostFtdcTraderApi *pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi("/home/mingyuan/packages/con/Trader.CFFEX.con/");
#elif defined EXCHANGE_SHFE
    CThostFtdcTraderApi *pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi("/home/mingyuan/packages/con/Trader.SHFE.con/");
#endif

    // initializing Logger
    Logger logger(argv[1]);
    logger.start();

    string contractSpec_filename("/home/mingyuan/packages/config/contract.specs");
    string configSpec_filename("/home/mingyuan/packages/config/config.specs");
    
    // initializing Exchange
    string exchange = argv[2];
    string contract = argv[3];

    // initializing Logger for LLS
    Logger logger_lls(argv[4]);
    logger_lls.start();
    // initializing exchange for LLS
    // it assumes that argv[3] is the leading instrument
    string contract_lls = argv[5];

    // load contract specifications
    ContractSpec contractSpec(contractSpec_filename);
    string tickerHead = exchange + ":" + contract.substr(0, 2);
    double multiplier;
    contractSpec.getMultiplier(tickerHead, multiplier);
    double tickSize;
    contractSpec.getTickSize(tickerHead, tickSize);
    
    // load trading model config
    ConfigSpec configSpec(configSpec_filename);
    double stopLoss, takeProfit, threshold, thresholdVol;
    configSpec.getStopLoss(tickerHead, stopLoss);
    configSpec.getTakeProfit(tickerHead, takeProfit);
    configSpec.getThreshold(tickerHead, threshold);
    configSpec.getVolThreshold(tickerHead, thresholdVol);
    string startTradingTime, exitTradingTime;
    configSpec.getStartTradingTime(tickerHead, startTradingTime);
    configSpec.getExitTradingTime(tickerHead, exitTradingTime);
    int buyOpenPosition, sellOpenPosition, orderSize;
    configSpec.getBuyOpenPosition(tickerHead, buyOpenPosition);
    configSpec.getSellOpenPosition(tickerHead, sellOpenPosition);
    configSpec.getOrderSize(tickerHead, orderSize);

    CTPEXCH<1, 5> exch(exchange, contract, buyOpenPosition, sellOpenPosition, &sc);
    CTPEXCH<1, 5> exch2(exchange, contract_lls, 0, 0, &sc);

    // initializing strategy
    VVDSM ts(exchange, contract, multiplier, tickSize, startTradingTime, exitTradingTime, &sc, &logger);
    ts.setStopLoss(stopLoss);
    ts.setTakeProfit(takeProfit);
    ts.setThreshold(threshold);
    ts.setThresholdVol(thresholdVol);
    ts.setOrderSize(orderSize);

    // initializing LLS strategy
    LLS lls(exchange, contract_lls, multiplier, tickSize,
	    exchange, contract, tickSize, startTradingTime, exitTradingTime,
	    &sc, &logger_lls);
    lls.setStopLoss(-420.0);
    lls.setTakeProfit(420.0);
    lls.setThreshold(7, 7);
    lls.setOrderSize(1);
    lls.setAggressivenessInTicks(4);
    lls.setNSnapshots(1);

    // register exchange, strategies to simulator core
    sc.registerMatchy(&exch);
    sc.registerMatchy(&exch2);
    //sc.registerStrategy(&mdc);
    sc.registerStrategy(&ts);
    sc.registerStrategy(&lls);

    // register trader api to Exchange
    exch.traderApi() = pTraderApi;
    exch2.traderApi() = pTraderApi;

    // initializing Trader Spi
    CSimpleTrader trader(pTraderApi);
    trader.addMatchy(&exch);
    trader.addMatchy(&exch2);

    // register trader spi to Exchange
    exch.traderSpi() = &trader;
    exch2.traderSpi() = &trader;

    pTraderApi->RegisterSpi(&trader);

    pTraderApi->SubscribePublicTopic(THOST_TERT_RESUME);
    pTraderApi->SubscribePrivateTopic(THOST_TERT_RESUME);

#ifdef BROKER_ZD
    string registerFrontAddressPortTrader("tcp://10.21.108.1:41205");
#elif defined BROKER_GD
    string registerFrontAddressPortTrader("tcp://172.20.38.16:41205");
#endif
    pTraderApi->RegisterFront(const_cast<char *>(registerFrontAddressPortTrader.c_str()));

    pTraderApi->Init();

    sleep(30); // Wait for login.

    // initializing MarketData SPi
    CSimpleHandler sh(pMdApi, 10);
    sh.addMatchy(&exch);
    sh.addMatchy(&exch2);

    pMdApi->RegisterSpi(&sh);
    
#ifdef BROKER_ZD
    string registerFrontAddressPort("tcp://10.21.108.1:41213");
#elif defined BROKER_GD
    string registerFrontAddressPort("tcp://172.20.38.16:41213");
#endif
    pMdApi->RegisterFront(const_cast<char *>(registerFrontAddressPort.c_str()));
    
    pMdApi->Init();
    
    pMdApi->Join();
    
    pTraderApi->Join();

    if (pMdApi) {
	pMdApi->RegisterSpi(NULL);
	pMdApi->Release();
	pMdApi = NULL;
    }

    if (pTraderApi) {
	pTraderApi->RegisterSpi(NULL);
	pTraderApi->Release();
	pTraderApi = NULL;
    }

    logger.end();
    logger_lls.end();
    
    return 0;
}
