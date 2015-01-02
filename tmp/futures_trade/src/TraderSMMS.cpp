//
// File:        $ TraderSMMS.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/11/07 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

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
#include "MarketDataCollector.hpp"
#include "Strategy.hpp"
#include "Logger.hpp"
#include "SMMS.hpp"
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
    cout << "TraderSMMS starts on " << yyyymmdd << endl;
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
#ifdef BROKER_MEY1
    CThostFtdcMdApi *pMdApi = CThostFtdcMdApi::CreateFtdcMdApi("/home/mingyuan/packages/con/Md.CFFEX.MEY1.con/", true);
#elif defined BROKER_HT
    CThostFtdcMdApi *pMdApi = CThostFtdcMdApi::CreateFtdcMdApi("/home/mingyuan/packages/con/Md.CFFEX.HT.con/", true);
#elif defined BROKER_MEY3
    CThostFtdcMdApi *pMdApi = CThostFtdcMdApi::CreateFtdcMdApi("/home/mingyuan/packages/con/Md.CFFEX.MEY3.con/", true);
#else
    CThostFtdcMdApi *pMdApi = CThostFtdcMdApi::CreateFtdcMdApi("/home/mingyuan/packages/con/Md.CFFEX.con/", true);
#endif
#elif defined EXCHANGE_SHFE
#ifdef BROKER_MEY1
    CThostFtdcMdApi *pMdApi = CThostFtdcMdApi::CreateFtdcMdApi("/home/mingyuan/packages/con/Md.SHFE.MEY1.con/", true);
#elif defined BROKER_HT
    CThostFtdcMdApi *pMdApi = CThostFtdcMdApi::CreateFtdcMdApi("/home/mingyuan/packages/con/Md.SHFE.HT.con/", true);
#elif defined BROKER_MEY3
    CThostFtdcMdApi *pMdApi = CThostFtdcMdApi::CreateFtdcMdApi("/home/mingyuan/packages/con/Md.SHFE.MEY3.con/", true);
#else
    CThostFtdcMdApi *pMdApi = CThostFtdcMdApi::CreateFtdcMdApi("/home/mingyuan/packages/con/Md.SHFE.con/", true);
#endif
#endif

#ifdef EXCHANGE_CFFEX
#ifdef BROKER_MEY1
    CThostFtdcTraderApi *pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi("/home/mingyuan/packages/con/Trader.CFFEX.MEY1.con/");
#elif defined BROKER_HT
    CThostFtdcTraderApi *pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi("/home/mingyuan/packages/con/Trader.CFFEX.HT.con/");
#elif defined BROKER_MEY3
    CThostFtdcTraderApi *pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi("/home/mingyuan/packages/con/Trader.CFFEX.MEY3.con/");
#else
    CThostFtdcTraderApi *pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi("/home/mingyuan/packages/con/Trader.CFFEX.con/");
#endif
#elif defined EXCHANGE_SHFE
#ifdef BROKER_MEY1
    CThostFtdcTraderApi *pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi("/home/mingyuan/packages/con/Trader.SHFE.MEY1.con/");
#elif defined BROKER_HT
    CThostFtdcTraderApi *pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi("/home/mingyuan/packages/con/Trader.SHFE.HT.con/");
#elif defined BROKER_MEY3
    CThostFtdcTraderApi *pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi("/home/mingyuan/packages/con/Trader.SHFE.MEY3.con/");
#else
    CThostFtdcTraderApi *pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi("/home/mingyuan/packages/con/Trader.SHFE.con/");
#endif
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
    string configSpec_filename("/home/mingyuan/packages/config/SMMS." + exchange + "." + tickerHead + ".specs");
    ConfigReader configSpec(configSpec_filename);
    string startTradingTime = configSpec.getString("StartTradingTime");
    string exitNoonTime = configSpec.getString("ExitNoonTime");
    string startNoonTime = configSpec.getString("StartNoonTime");
    string exitTradingTime = configSpec.getString("ExitTradingTime");
    double edge = configSpec.getDouble("Edge");
    double buffer = configSpec.getDouble("Buffer");
    double takeProfit = configSpec.getDouble("TakeProfit");
    double takeProfits = configSpec.getDouble("TakeProfits");
    double stopLoss = configSpec.getDouble("StopLoss");
    double stopLosses = configSpec.getDouble("StopLosses");
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
    SMMS smms(exchange, contractSpreadFront, contractSpreadBack, multiplier, tickSize, startTradingTime, exitNoonTime, startNoonTime, exitTradingTime, &sc, &logger);
    smms.setEdge(edge);
    smms.setBuffer(buffer);
    smms.setTakeProfit(takeProfit, takeProfits);
    smms.setStopLoss(stopLoss, stopLosses);
    smms.setMaxSpreadPosition(maxSpreadPosition);
    smms.setMaxCancelQty(maxCancelQty);
    smms.setOrderSize(orderSize);

    // register exchange, strategies to simulator core
    sc.registerMatchy(&exchSpreadFront);
    sc.registerMatchy(&exchSpreadBack);

    sc.registerStrategy(&smms);

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

#ifdef BROKER_ZD
    string registerFrontAddressPortTrader("tcp://10.21.108.1:41205");
#elif defined BROKER_GD
    string registerFrontAddressPortTrader("tcp://172.20.38.16:41205");
#elif defined BROKER_MEY1
    string registerFrontAddressPortTrader("tcp://172.26.50.3:41205");
#elif defined BROKER_HT
    string registerFrontAddressPortTrader("tcp://192.168.20.1:41205");
#elif defined BROKER_MEY3
    string registerFrontAddressPortTrader("tcp://172.26.50.3:41205");
#endif
    pTraderApi->RegisterFront(const_cast<char *>(registerFrontAddressPortTrader.c_str()));

    pTraderApi->Init();

    sleep(30); // Wait for login.

    // initializing MarketData SPi
    CSimpleHandler sh(pMdApi, 10);
    sh.addMatchy(&exchSpreadFront);
    sh.addMatchy(&exchSpreadBack);

    pMdApi->RegisterSpi(&sh);
    
#ifdef BROKER_ZD
    string registerFrontAddressPort("tcp://10.21.108.1:41213");
#elif defined BROKER_GD
    string registerFrontAddressPort("tcp://172.20.38.16:41213");
#elif defined BROKER_MEY1
    string registerFrontAddressPort("tcp://172.26.41.1:18215");
#elif defined BROKER_HT
    string registerFrontAddressPort("tcp://192.168.20.1:41213");
#elif defined BROKER_MEY3
    string registerFrontAddressPort("tcp://172.26.41.1:18215");
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

    return 0;
}
