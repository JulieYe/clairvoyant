//
// File:        $ CTPTrader.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/05/07 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2014 by Zhengzhang Futures Co. Ltd.

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
#include "SpreadMM.hpp"
#include "ContractSpec.hpp"
#include "ConfigSpec.hpp"

#include "ObjectFactory.hpp"
#include "StrategyRegistry.hpp"
#include "MatchyRegistry.hpp"
#include "INIReader.hpp"

using namespace std;
using namespace simulator;

int main(int argc, char ** argv)
{
    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );

    char yyyymmdd[11];

    sprintf(yyyymmdd, "%d-%02d-%02d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);

    stringstream ssd;

#ifdef DEBUG
    cout << "Trader starts on " << yyyymmdd << endl;
#endif

    ssd << yyyymmdd;


    string config(argv[1]);
    INIReader iniconfig(config);

    registerAllMatchies();
    registerAllStrategies();
    
    string dateStr(ssd.str());
    // reading main section
    string startTimeStr = iniconfig.Get("Main", "StartTime", "08:55");
    string endTimeStr = iniconfig.Get("Main", "EndTime", "15:20");
    int interval = iniconfig.GetInteger("Main", "Interval", 10);
    string timeUnit = iniconfig.Get("Main", "TimeUnit", "millisecond");
    string matchiesStr = iniconfig.Get("Main", "Matchy", "");
    string strategiesStr = iniconfig.Get("Main", "Strategy", "");

    SimulatorCore sc(dateStr + "T" + startTimeStr + ":00.0", dateStr + "T" + endTimeStr + ":00.0",
		     interval, timeUnit);
    

    // reading CTP api section
    // market data api and trader api
    CThostFtdcMdApi *pMdApi = CThostFtdcMdApi::CreateFtdcMdApi(iniconfig.Get("CTPApi", "MdConnectionFolder", "/home/mingyuan/packages/con/Md.CFFEX.con/").c_str(), true);
    CThostFtdcTraderApi *pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi(iniconfig.Get("CTPApi", "TraderConnectionFolder", "/home/mingyuan/packages/con/Trader.CFFEX.con/").c_str());
    CSimpleTrader trader(pTraderApi);
    CSimpleHandler sh(pMdApi, 10);

    // reading Matchy sections
    stringstream ss(matchiesStr);
    string matchyName;
    while(ss.good()) {
	getline(ss, matchyName, ',');
	std::cout << "creating Matchy " << matchyName << std::endl;
	CTPMatchyApi * pMatchy = static_cast<CTPMatchyApi *>(MatchyFactory::instance()->getByName(matchyName));
	pMatchy->getParamFromConfig(iniconfig, matchyName);
	pMatchy->setSimulatorCore(&sc);
	sc.registerMatchy(pMatchy);

	pMatchy->traderApi() = pTraderApi;
	pMatchy->traderSpi() = &trader;
	trader.addMatchy(pMatchy);
	sh.addMatchy(pMatchy);
    }
    
    // reading strategy sections
    stringstream sss(strategiesStr);
    string strategyName;
    vector<Logger *> loggerVec;
    while(sss.good()) {
	getline(sss, strategyName, ',');
	std::cout << "creating Strategy " << strategyName << std::endl;
	Strategy * pStrategy = StrategyFactory::instance()->getByName(strategyName);
	pStrategy->setSimulatorCore(&sc);
	pStrategy->getParamFromConfig(iniconfig, strategyName);
	sc.registerStrategy(pStrategy);
	string loggerFolder = iniconfig.Get(strategyName, "LoggerOutputFolder", "");
	if(loggerFolder != "") {
	    string loggerFile = loggerFolder + "/sim_" + dateStr + "_" + strategyName + ".log";
	    Logger * pLogger = new Logger(loggerFile);
	    pLogger->start();
	    pStrategy->setLogger(pLogger);
	    loggerVec.push_back(pLogger);
	}
    }

    // ctp part
    pTraderApi->RegisterSpi(&trader);
    pTraderApi->SubscribePublicTopic(THOST_TERT_RESUME);
    pTraderApi->SubscribePrivateTopic(THOST_TERT_RESUME);

    string registerFrontAddressPortTrader(iniconfig.Get("CTPApi", "FrontAddressTrader", "tcp://10.21.108.1:41205").c_str());

    pTraderApi->RegisterFront(const_cast<char *>(registerFrontAddressPortTrader.c_str()));

    pTraderApi->Init();

#ifdef DEBUG
    cout << "Current local system-side time is " 
	 << SimulatorCore::convertTimePointToString(sc.getCurrTime()) 
	 << endl;
#endif

    sleep(30); // Wait for login.

    pMdApi->RegisterSpi(&sh);

    string registerFrontAddressPort(iniconfig.Get("CTPApi", "FrontAddress", "tcp://10.21.108.1:41213"));
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

    // release logger
    for(vector<Logger *>::iterator it = loggerVec.begin();
	it != loggerVec.end(); it++) {
	(*it)->end();
	delete (*it);
    }

    
    return 0;
}
