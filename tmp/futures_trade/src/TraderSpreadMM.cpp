//
// File:        $ TraderSpreadMM.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2014/01/12 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2014 by Zhengzhang Futures Co. Ltd.

#include <ctime>
#include <string>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "ThostFtdcMdApi.h"
#include "ThostFtdcTraderApi.h"
#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcUserApiStruct.h"

#include "MarketDataSpi.hpp"
#include "TraderSpi.hpp"
#include "SimulatorCore.hpp"
#include "CFFEX.hpp"
#include "Strategy.hpp"
#include "Logger.hpp"
#include "SpreadMM.hpp"
#include "ContractSpec.hpp"
#include "ConfigReader.hpp"

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
    cout << "Trader SpreadMM starts on " << yyyymmdd << endl;
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
    CThostFtdcTraderApi *pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi("/home/mingyuan/packages/con/Trader.CFFEX.con/");
#elif defined EXCHANGE_SHFE
    CThostFtdcMdApi *pMdApi = CThostFtdcMdApi::CreateFtdcMdApi("/home/mingyuan/packages/con/Md.SHFE.con/", true);
    CThostFtdcTraderApi *pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi("/home/mingyuan/packages/con/Trader.SHFE.con/");
#endif

    // initializing Logger
    Logger logger(argv[1]);
    logger.start();
    string exchange = argv[2];
    string tickerHead = argv[3];
    string contractSpreadFront = argv[4];
    string contractSpreadBack = argv[5];
    
    // load contracts specifications
    string contractSpec_filename("/home/mingyuan/packages/config/contract.specs");
    ContractSpec contractSpec(contractSpec_filename);
    double multiplier, tickSize;
    contractSpec.getMultiplier(exchange + ":" + tickerHead, multiplier);
    contractSpec.getTickSize(exchange + ":" + tickerHead, tickSize);
    
    // load trading model config
    string configSpec_filename("/home/mingyuan/packages/config/SpreadMM." + exchange + "." + tickerHead + ".specs");
    ConfigReader configSpec(configSpec_filename);
    string startTradingTime = configSpec.getString("StartTradingTime");
    string exitNoonTime = configSpec.getString("ExitNoonTime");
    string startNoonTime = configSpec.getString("StartNoonTime");
    string exitTradingTime = configSpec.getString("ExitTradingTime");
    double enterEdge = configSpec.getDouble("EnterEdge");
    double exitEdge = configSpec.getDouble("ExitEdge");
    double buffer = configSpec.getDouble("Buffer");
    double stopLoss = configSpec.getDouble("StopLoss");
    int maxSpreadPosition = configSpec.getInteger("MaxSpreadPosition");
    int maxCancelQty = configSpec.getInteger("MaxCancelQty");
    int orderSize = configSpec.getInteger("OrderSize");

    // initializing front contract exchange
    string frontSpec("/home/mingyuan/packages/config/" + exchange + "." + contractSpreadFront + ".specs");
    ConfigReader frontContractSpec(frontSpec);
    CTPEXCH<1, 5> exchSpreadFront(exchange, contractSpreadFront, frontContractSpec.getInteger("BuyOpenPosition"), frontContractSpec.getInteger("SellOpenPosition"), &sc);

    // initializing front contract exchange
    string backSpec("/home/mingyuan/packages/config/" + exchange + "." + contractSpreadBack + ".specs");
    ConfigReader backContractSpec(backSpec);
    CTPEXCH<1, 5> exchSpreadBack(exchange, contractSpreadBack, backContractSpec.getInteger("BuyOpenPosition"), backContractSpec.getInteger("SellOpenPosition"), &sc);

    // initializing strategy
    SpreadMM spreadmm(exchange, contractSpreadFront, contractSpreadBack, multiplier, tickSize, startTradingTime, exitNoonTime, startNoonTime, exitTradingTime, &sc, &logger);
    spreadmm.setEdges(enterEdge, exitEdge);
    spreadmm.setBuffer(buffer);
    spreadmm.setStopLoss(stopLoss);
    spreadmm.setMaxSpreadPosition(maxSpreadPosition);
    spreadmm.setMaxCancelQty(maxCancelQty);
    spreadmm.setOrderSize(orderSize);

    // register exchange, strategies to simulator core
    sc.registerMatchy(&exchSpreadFront);
    sc.registerMatchy(&exchSpreadBack);

    sc.registerStrategy(&spreadmm);

    // register trader api to exchange
    exchSpreadFront.traderApi() = pTraderApi;
    exchSpreadBack.traderApi() = pTraderApi;

    // initializing Trader Spi
    CSimpleTrader trader(pTraderApi);
    trader.addMatchy(&exchSpreadFront);
    trader.addMatchy(&exchSpreadBack);

    // register trader spi to Exchange
    exchSpreadFront.traderSpi() = &trader;
    exchSpreadBack.traderSpi() = &trader;

    pTraderApi->RegisterSpi(&trader);

    pTraderApi->SubscribePublicTopic(THOST_TERT_RESUME);
    pTraderApi->SubscribePrivateTopic(THOST_TERT_RESUME);

    string registerFrontAddressPortTrader("tcp://172.20.38.16:41205");
    pTraderApi->RegisterFront(const_cast<char *>(registerFrontAddressPortTrader.c_str()));

    pTraderApi->Init();

    sleep(30); // Wait for login.

    // initializing MarketData SPi
    CSimpleHandler sh(pMdApi, 10);
    sh.addMatchy(&exchSpreadFront);
    sh.addMatchy(&exchSpreadBack);

    pMdApi->RegisterSpi(&sh);
    
    string registerFrontAddressPort("tcp://172.20.38.16:41213");
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

    return 0;
}
