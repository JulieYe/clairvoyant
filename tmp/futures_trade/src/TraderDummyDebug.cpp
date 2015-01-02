//
// File:        $ Trader.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/02/16 19:50:00 $
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


using namespace std;
using namespace simulator;

class DummyStrategy : public Strategy
{
private:
    bool placedOrder_;
    bool setTimer_;
    time_stamp timeToPlace_; // 2min after first BookUpdate
    time_stamp timeToCancel_; // 1min after timeToPlace

    time_stamp startTime_;

public:
    DummyStrategy(string exchange, string contract, double multi, SimulatorCore * simptr, Logger * loggerptr) :
	Strategy("dummy", simptr, loggerptr), placedOrder_(false), setTimer_(false)
    {
	addStrat(exchange, contract, multi);
	
	string datestr = SimulatorCore::convertTimePointToString(simptr->getCurrTime()).substr(0, 10);
	stringstream ss1;
	ss1 << datestr << "T" << "09:20:00" << ".000000000";
	startTime_ = convertStringToTimePoint(ss1.str());
    }

    void process()
    {
    }

};

int main(int argc, char ** argv)
{
    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );

    char yyyymmdd[11];

    sprintf(yyyymmdd, "%d-%02d-%02d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);

    stringstream ss;
    // ss << (now->tm_year + 1900) << '-' 
    //    << (now->tm_mon + 1) << '-'
    //    <<  now->tm_mday; 

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
    

    // initializing Logger
    Logger logger(argv[1]);
    logger.start();
    
    // initializing Exchange
    string exchange, contract;
    exchange = "CFFEX";
    contract = argv[2];
    
    CFFEX<1, 5> exch(contract, &sc);
    
    // initializing Market data Collector
    MarketDataCollector mdc(1, &sc, &logger);
    mdc.addStrat(exchange, contract, 300.0);
    
    // initializing strategy
    DummyStrategy ts("CFFEX", contract, 300.0, &sc, &logger);

    // register exchange, strategies to simulator core
    sc.registerMatchy(&exch);
    //sc.registerStrategy(&mdc);
    sc.registerStrategy(&ts);

    CThostFtdcOrderField order1, order2, order3, order4;
    CThostFtdcTradeField trade;

    memset(&order1, 0, sizeof(CThostFtdcOrderField));
    memset(&order2, 0, sizeof(CThostFtdcOrderField));
    memset(&order3, 0, sizeof(CThostFtdcOrderField));
    memset(&order4, 0, sizeof(CThostFtdcOrderField));
    memset(&trade, 0, sizeof(CThostFtdcTradeField));

    // order1
    strcpy(order1.BrokerID, "8090");
    strcpy(order1.InvestorID, "57128098");
    strcpy(order1.InstrumentID, "IF1303");
    strcpy(order1.OrderRef, "3");
    strcpy(order1.UserID, "57128098");
    order1.OrderPriceType='2';
    order1.Direction='0';
    order1.CombOffsetFlag[0]='0';
    order1.CombHedgeFlag[0]='1';
    order1.LimitPrice=2602.8;
    order1.VolumeTotalOriginal=1;
    order1.TimeCondition='3';
    order1.VolumeCondition='1';
    order1.MinVolume=1;
    order1.ContingentCondition='1';
    order1.StopPrice=0;
    order1.ForceCloseReason='0';
    order1.IsAutoSuspend=0;
    strcpy(order1.BusinessUnit, "015913");
    order1.RequestID=1;
    strcpy(order1.OrderLocalID,         "1108");
    strcpy(order1.ExchangeID, "CFFEX");
    strcpy(order1.ParticipantID, "0159");
    strcpy(order1.ClientID, "01129319");
    strcpy(order1.ExchangeInstID, "IF1303");
    strcpy(order1.TraderID, "015913");
    order1.InstallID=1;
    order1.OrderSubmitStatus='0';
    order1.NotifySequence=0;
    strcpy(order1.TradingDay, "20130304");
    order1.SettlementID=1;
    order1.OrderSource='0';
    order1.OrderStatus='a';
    order1.OrderType='0';
    order1.VolumeTraded=0;
    order1.VolumeTotal=1;
    strcpy(order1.InsertDate, "20130304");
    strcpy(order1.InsertTime, "10:15:54");
    order1.SequenceNo=0;
    order1.FrontID=1;
    order1.SessionID=29032636;
    order1.UserForceClose='0';
    order1.BrokerOrderSeq=5300;
    order1.ZCETotalTradedVolume=0;
    order1.IsSwapOrder=0;

    // order2
    strcpy(order2.BrokerID, "8090");
    strcpy(order2.InvestorID, "57128098");
    strcpy(order2.InstrumentID, "IF1303");
    strcpy(order2.OrderRef, "3");
    strcpy(order2.UserID, "57128098");
    order2.OrderPriceType='2';
    order2.Direction='0';
    order2.CombOffsetFlag[0]='0';
    order2.CombHedgeFlag[0]='1';
    order2.LimitPrice=2602.8;
    order2.VolumeTotalOriginal=1;
    order2.TimeCondition='3';
    order2.VolumeCondition='1';
    order2.MinVolume=1;
    order2.ContingentCondition='1';
    order2.StopPrice=0;
    order2.ForceCloseReason='0';
    order2.IsAutoSuspend=0;
    strcpy(order2.BusinessUnit, "015913");
    order2.RequestID=1;
    strcpy(order2.OrderLocalID, "1108");
    strcpy(order2.ExchangeID, "CFFEX");
    strcpy(order2.ParticipantID, "0159");
    strcpy(order2.ClientID, "01129319");
    strcpy(order2.ExchangeInstID, "IF1303");
    strcpy(order2.TraderID, "015913");
    order2.InstallID=1;
    order2.OrderSubmitStatus='3';
    order2.NotifySequence=0;
    strcpy(order2.TradingDay, "20130304");
    order2.SettlementID=1;
    order2.OrderSource='0';
    order2.OrderStatus='3';
    order2.OrderType='0';
    order2.VolumeTraded=0;
    order2.VolumeTotal=1;
    strcpy(order2.InsertDate, "20130304");
    strcpy(order2.InsertTime, "10:15:54");
    order2.SequenceNo=11353;
    order2.FrontID=1;
    order2.SessionID=29032636;
    order2.UserForceClose='0';
    order2.BrokerOrderSeq=5300;
    order2.ZCETotalTradedVolume=0;
    order2.IsSwapOrder=0;

    // order3
    strcpy(order3.BrokerID, "8090");
    strcpy(order3.InvestorID, "57128098");
    strcpy(order3.InstrumentID, "IF1303");
    strcpy(order3.OrderRef, "3");
    strcpy(order3.UserID, "57128098");
    order3.OrderPriceType='2';
    order3.Direction='0';
    order3.CombOffsetFlag[0]='0';
    order3.CombHedgeFlag[0]='1';
    order3.LimitPrice=2602.8;
    order3.VolumeTotalOriginal=1;
    order3.TimeCondition='3';
    order3.VolumeCondition='1';
    order3.MinVolume=1;
    order3.ContingentCondition='1';
    order3.StopPrice=0;
    order3.ForceCloseReason='0';
    order3.IsAutoSuspend=0;
    strcpy(order3.BusinessUnit, "015913");
    order3.RequestID=1;
    strcpy(order3.OrderLocalID, "1108");
    strcpy(order3.ExchangeID, "CFFEX");
    strcpy(order3.ParticipantID, "0159");
    strcpy(order3.ClientID, "01129319");
    strcpy(order3.ExchangeInstID, "IF1303");
    strcpy(order3.TraderID, "015913");
    order3.InstallID=1;
    order3.OrderSubmitStatus='3';
    order3.NotifySequence=0;
    strcpy(order3.TradingDay, "20130304");
    order3.SettlementID=1;
    order3.OrderSource='0';
    order3.OrderStatus='3';
    order3.OrderType='0';
    order3.VolumeTraded=0;
    order3.VolumeTotal=1;
    strcpy(order3.InsertDate, "20130304");
    strcpy(order3.InsertTime, "10:15:54");
    order3.SequenceNo=11357;
    order3.FrontID=1;
    order3.SessionID=29032636;
    order3.UserForceClose='0';
    order3.BrokerOrderSeq=5300;
    order3.ZCETotalTradedVolume=0;
    order3.IsSwapOrder=0;

    // order4
    strcpy(order4.BrokerID, "8090");
    strcpy(order4.InvestorID, "57128098");
    strcpy(order4.InstrumentID, "IF1303");
    strcpy(order4.OrderRef, "3");
    strcpy(order4.UserID, "57128098");
    order4.OrderPriceType='2';
    order4.Direction='0';
    order4.CombOffsetFlag[0]='0';
    order4.CombHedgeFlag[0]='1';
    order4.LimitPrice=2602.8;
    order4.VolumeTotalOriginal=1;
    order4.TimeCondition='3';
    order4.VolumeCondition='1';
    order4.MinVolume=1;
    order4.ContingentCondition='1';
    order4.StopPrice=0;
    order4.ForceCloseReason='0';
    order4.IsAutoSuspend=0;
    strcpy(order4.BusinessUnit, "015913");
    order4.RequestID=1;
    strcpy(order4.OrderLocalID, "1108");
    strcpy(order4.ExchangeID, "CFFEX");
    strcpy(order4.ParticipantID, "0159");
    strcpy(order4.ClientID, "01129319");
    strcpy(order4.ExchangeInstID, "IF1303");
    strcpy(order4.TraderID, "015913");
    order4.InstallID=1;
    order4.OrderSubmitStatus='3';
    order4.NotifySequence=0;
    strcpy(order4.TradingDay, "20130304");
    order4.SettlementID=1;
    order4.OrderSource='0';
    order4.OrderStatus='0';
    order4.OrderType='0';
    order4.VolumeTraded=1;
    order4.VolumeTotal=0;
    strcpy(order4.InsertDate, "20130304");
    strcpy(order4.InsertTime, "10:15:54");
    order4.SequenceNo=11357;
    order4.FrontID=1;
    order4.SessionID=29032636;
    order4.UserForceClose='0';
    strcpy(order4.ActiveTraderID, "015913");
    order4.BrokerOrderSeq=5300;
    order4.ZCETotalTradedVolume=0;
    order4.IsSwapOrder=0;

    //trade
    strcpy(trade.BrokerID, "8090");
    strcpy(trade.InvestorID, "57128098");
    strcpy(trade.InstrumentID, "IF1303");
    strcpy(trade.OrderRef, "3");
    strcpy(trade.UserID, "57128098");
    strcpy(trade.ExchangeID, "CFFEX");
    strcpy(trade.TradeID,       "170710");
    trade.Direction='0';
    strcpy(trade.OrderSysID,    "585236");
    strcpy(trade.ParticipantID, "0159");
    strcpy(trade.ClientID, "01129319");
    strcpy(trade.ExchangeInstID, "IF1303");
    trade.OffsetFlag='0';
    trade.HedgeFlag='1';
    trade.Price=2602.8;
    trade.Volume=1;
    strcpy(trade.TradeDate, "20130304");
    strcpy(trade.TradeTime, "10:15:57");
    strcpy(trade.TraderID, "015913");
    strcpy(trade.OrderLocalID,         "1108");
    strcpy(trade.ClearingPartID, "0159");
    trade.SequenceNo=11358;
    strcpy(trade.TradingDay, "20130304");
    trade.SettlementID=1;
    trade.BrokerOrderSeq=5300;
    trade.TradeSource='0';

    
    exch.push(&order1);
    std::cout << "pushed order1" << std::endl;
    exch.push(&order2);
    std::cout << "pushed order2" << std::endl;
    exch.push(&order3);
    std::cout << "pushed order3" << std::endl;
    exch.push(&order4);
    std::cout << "pushed order4" << std::endl;
    exch.push(&trade);
    std::cout << "pushed trade" << std::endl;



    logger.end();
    
    return 0;
}



