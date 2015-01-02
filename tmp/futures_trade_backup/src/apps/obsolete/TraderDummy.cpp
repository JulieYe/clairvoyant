//
// File:        $ TraderDummy.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/03/11 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include <ctime>
#include <string>
#include <sstream>
#include <cstdio>

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
#include "ContractSpec.hpp"

using namespace std;
using namespace simulator;

class DummyStrategy : public Strategy
{
private:
    bool placedOrder_ = false;
    bool placedMarketOrder_ = false;
    bool setTimer_ = false;
    time_stamp timeToPlace_; // 2min after first BookUpdate
    time_stamp timeToCancel_; // 1min after timeToPlace
    time_stamp startTime_;

public:
    DummyStrategy(string exchange, string contract, double multi, SimulatorCore * simptr, Logger * loggerptr) :
	Strategy("dummy", simptr, loggerptr)
    {
	addStrat(exchange, contract, multi);
	
	string datestr = SimulatorCore::convertTimePointToString(simptr->getCurrTime()).substr(0, 10);
	stringstream ss1;
	ss1 << datestr << "T" << "09:20:00" << ".000000000";
	startTime_ = convertStringToTimePoint(ss1.str());
    }

    void process()
    {
	if(!placedMarketOrder_ && getOpenPosition(0) < 0) {
	    placeMarketOrder(BuySellSide::buy, 1, 0);
	    std::cout << "Placed market order at " << simulatorCorePtr_->getCurrTime()
		      << " buy with market order" << std::endl;
	    placedMarketOrder_ = true;
	}

	// place and cancel
	auto ibu = bookUpdates_[0].begin();
	while (ibu != bookUpdates_[0].end() && simulatorCorePtr_->timeOffsetQ()) {
#ifdef DEBUG
	    // std::cout << "Current local time is " << convertTimePointToString((*ibu).getTimestamp() + simulatorCorePtr_->timeOffset()) << std::endl;
#endif
	    if((*ibu).getTimestamp() < startTime_) {
		return;
	    }
	    if(!simulatorCorePtr_->isAllMarketDataReady() ||
	       !simulatorCorePtr_->isAllTraderReady()) {
		return;
	    }
	    if(!setTimer_) {
		timeToPlace_ = (*ibu).getTimestamp() + simulatorCorePtr_->timeOffset() + duration_nano(60 * 1000000000ULL);
		timeToCancel_ = timeToPlace_ + duration_nano(30 * 1000000000ULL);
		setTimer_ = true;
		std::cout << "Set Time To Place " << convertTimePointToString(timeToPlace_) << std::endl;
		std::cout << "Set Time To Cancel " << convertTimePointToString(timeToCancel_) << std::endl;
		return;
	    }
	    if(((*ibu).getTimestamp() + simulatorCorePtr_->timeOffset()) >= timeToPlace_ && !placedOrder_) {
		double pxToSubmit = dynamic_cast<CTPMatchyApi *>((simulatorCorePtr_->getExchanges()).front())->getLowerLimitPrice();
#ifdef TICKERHEAD_IF	      
		double tickSize = dynamic_cast<CFFEX<1,5> *>((simulatorCorePtr_->getExchanges()).front())->getTickSize();
#elif TICKERHEAD_RU
		double tickSize = dynamic_cast<CFFEX<5,1> *>((simulatorCorePtr_->getExchanges()).front())->getTickSize();
#endif
		double pxBidminus2Tick = (*ibu).getAskPx()[0].getPx();
		pxToSubmit = pxBidminus2Tick;
		pxToSubmit += tickSize;
		placeLimitOrder(Px(pxToSubmit, tickSize), BuySellSide::sell, 1, 0);
		placedOrder_ = true;
		std::cout << "Placed sell order at " << convertTimePointToString((*ibu).getTimestamp() + simulatorCorePtr_->timeOffset()) 
			  << " with price = " << pxToSubmit
			  << std::endl;
		return;
	    }
	    if(((*ibu).getTimestamp() + simulatorCorePtr_->timeOffset()) >= timeToCancel_ && placedOrder_) {
		cancelAllLimitOrders(0);
#ifdef DEBUG
		// std::cout << "Cancel all order at " << convertTimePointToString((*ibu).getTimestamp() + simulatorCorePtr_->timeOffset()) << std::endl;
#endif
		return;
	    }
	    ibu++;
	}

    }

};

int main(int argc, char ** argv)
{
    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );

    char yyyymmdd[11];

    sprintf(yyyymmdd, "%d-%02d-%02d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);

    stringstream ss;

#ifdef DEBUG
    cout << "TraderDummy starts on " << yyyymmdd << endl;
#endif

    ss << yyyymmdd;

    // initializing simulator core
    SimulatorCore sc(ss.str() + "T09:15:00.0", ss.str() + "T15:16:00.0", 10, "millisecond"); 

#ifdef DEBUG
    cout << "Current local system-side time is " 
	 << SimulatorCore::convertTimePointToString(sc.getCurrTime()) 
	 << endl;
#endif
    
    // initializing Market Data Api and Trader Api
    CThostFtdcMdApi *pMdApi = CThostFtdcMdApi::CreateFtdcMdApi("/home/mingyuan/packages/traderapi_linux64/Md_con_test/");
    CThostFtdcTraderApi *pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi("/home/mingyuan/packages/traderapi_linux64/Trader_con_test/");

    // initializing Logger
    Logger logger(argv[1]);
    logger.start();
    
    // initializing Exchange
    string exchange, contract;
#ifdef EXCHANGE_CFFEX
    exchange = "CFFEX";
#elif defined EXCHANGE_SHFE
    exchange = "SHFE";
#endif
    contract = argv[2];

#ifdef TICKERHEAD_IF
    CTPEXCH<1, 5> exch(exchange, contract, 0, 0, &sc);
#elif defined TICKERHEAD_RU
    CTPEXCH<5, 1> exch(exchange, contract, 0, 0, &sc);
#endif

    // load contract specifications
    string contractSpec_filename("/home/mingyuan/packages/config/contract.specs");
    ContractSpec contractSpec(contractSpec_filename);
    string tickerHead = exchange + ":" + contract.substr(0, 2);
    double multiplier;
    contractSpec.getMultiplier(tickerHead, multiplier);
    
    // initializing Market data Collector
    MarketDataCollector mdc(1, &sc, &logger);
    mdc.addStrat(exchange, contract, multiplier);
   
    // initializing strategy
    DummyStrategy ts(exchange, contract, multiplier, &sc, &logger);

    // register exchange, strategies to simulator core
    sc.registerMatchy(&exch);
    //sc.registerStrategy(&mdc);
    sc.registerStrategy(&ts);

    // register trader api to Exchange
    exch.traderApi() = pTraderApi;

    // initializing Trader Spi
    CSimpleTrader trader(pTraderApi);
    trader.addMatchy(&exch);

    // register trader spi to Exchange
    exch.traderSpi() = &trader;

    pTraderApi->RegisterSpi(&trader);

    pTraderApi->SubscribePublicTopic(THOST_TERT_RESUME);
    pTraderApi->SubscribePrivateTopic(THOST_TERT_RESUME);

#ifdef BROKER_ZD
    string registerFrontAddressPortTrader("tcp://10.21.108.1:41205");
#elif defined BROKER_GJ
    string registerFrontAddressPortTrader("tcp://172.22.91.101:41205");
#endif
    pTraderApi->RegisterFront(const_cast<char *>(registerFrontAddressPortTrader.c_str()));

    pTraderApi->Init();

    sleep(30); // Wait for login.

    // initializing MarketData SPi
    CSimpleHandler sh(pMdApi, 5);
    sh.addMatchy(&exch);

    pMdApi->RegisterSpi(&sh);
    
#ifdef BROKER_ZD
    string registerFrontAddressPort("tcp://10.21.108.1:41213");
#elif defined BROKER_GJ
    string registerFrontAddressPort("tcp://172.22.91.101:41213");
#endif
    pMdApi->RegisterFront(const_cast<char *>(registerFrontAddressPort.c_str()));
    
    pMdApi->Init();
    
    sleep(20);

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
