//
// File:        $ MdCollector.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/02/25 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include <ctime>
#include <string>
#include <sstream>

#include "ThostFtdcMdApi.h"
#include "ThostFtdcTraderApi.h"
#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcUserApiStruct.h"

#include "MarketDataSpi.hpp"
#include "SimulatorCore.hpp"
#include "MarketDataCollector.hpp"

using namespace std;
using namespace simulator;

int main(int argc, char ** argv)
{
    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );

    char yyyymmdd[11];

    sprintf(yyyymmdd, "%d-%02d-%02d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);

    stringstream ss;
    ss << yyyymmdd;
    
    SimulatorCore sc(ss.str() + "T09:15:00.0", ss.str() + "T15:16:00.0", 10, "millisecond"); 
    
    CThostFtdcMdApi *pUserApi = CThostFtdcMdApi::CreateFtdcMdApi("/home/mingyuan/packages/traderapi_linux64/Md_con_test/");
    
    Logger logger(argv[1]);
    logger.start();
    
    string exchange, contract;
    exchange = "CFFEX";
    contract = argv[2];
    
    CFFEX<1, 5> exch(contract, &sc);
    
    MarketDataCollector mdc(1, &sc, &logger);
    mdc.addStrat(exchange, contract, 300.0);
    
    
    sc.registerMatchy(&exch);
    sc.registerStrategy(&mdc);
    
    CSimpleHandler sh(pUserApi, 5);
    sh.addMatchy(&exch);
    
    pUserApi->RegisterSpi(&sh);
    
#ifdef BROKER_ZD
    string registerFrontAddressPort("tcp://10.21.108.1:41213");
#elif defined BROKER_GD
    string registerFrontAddressPort("tcp://172.20.38.16:41213");
#endif
    pUserApi->RegisterFront(const_cast<char *>(registerFrontAddressPort.c_str()));

    std::cout << registerFrontAddressPort << std::endl;
    
    pUserApi->Init();
    
    std::cout << "Initiated" << std::endl;
    
    pUserApi->Join();
    
    pUserApi->Release();

    logger.end();

    return 0;
}

// MDCOLLECTOR_CPP
