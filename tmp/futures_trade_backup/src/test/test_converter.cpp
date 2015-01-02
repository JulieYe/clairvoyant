//
// File:        $ test_converter.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/05/14 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE unit_test_converter
#include <boost/test/unit_test.hpp>

#include "Converter.hpp"
#include "SimulatorCore.hpp"
#include "OrderCache.hpp"
#include "CFFEX.hpp"

using simulator::combineToTimestampString;
using simulator::ConverterMessage;
using simulator::ConverterOrder;
using simulator::SimulatorCore;
using simulator::OrderCache;
using simulator::FilledOrderMessage;
using simulator::CTPMatchyApi;

// hack
class DummyCTPMatchyApi : public CTPMatchyApi
{
public:
    DummyCTPMatchyApi(string exchange, string contract, SimulatorCore * ptr) : 
	CTPMatchyApi(exchange, contract, 0, 0, ptr)
    {}

    virtual void push(CThostFtdcDepthMarketDataField *) {}
    virtual void push(CThostFtdcInputOrderField * pInput) {}
    virtual void push(CThostFtdcInputOrderActionField * pAction) {}
    virtual void push(CThostFtdcOrderActionField * pAction) {}
    virtual void push(CThostFtdcOrderField * pOrder) {}
    virtual void push(CThostFtdcTradeField * pTrade) {}
    virtual void process(int orderId) {}
};

BOOST_AUTO_TEST_CASE(test_combinetotimestampstring)
{
    TThostFtdcDateType dt="20130109";
    TThostFtdcTimeType tm="09:30:24";
    TThostFtdcMillisecType ms=100;
    
    string tp = combineToTimestampString(dt, tm, ms);
    BOOST_CHECK(tp == "2013-01-09T09:30:24.100000000");
}


BOOST_AUTO_TEST_CASE(test_convertmessage)
{
    ConverterMessage<CThostFtdcDepthMarketDataField *, true, true, 1, 1, 5> converter1;
    //ConverterMessage<CThostFtdcDepthMarketDataField *, true, true, 5, 1, 5> converter5;
    
    OrderCache oc;
    
    // test 1 level book converter
    BookUpdate<true> bu;
    TradeUpdate<true> tu;
    duration_nano timeoffset(1000000ULL);
    
    CThostFtdcDepthMarketDataField data;
    strcpy(data.TradingDay, "20130109");
    strcpy(data.InstrumentID, "IF1209");
    strcpy(data.ExchangeID, "CFFEX");
    strcpy(data.ExchangeInstID, "CFFEX"); // test what this is?
    
    data.LastPrice = 2034.2;
    data.Volume = 4000;
    strcpy(data.UpdateTime, "09:30:30");
    data.UpdateMillisec = 500;
    data.BidPrice1 = 2030;
    data.BidVolume1 = 1;
    data.AskPrice1 = 2030.4;
    data.AskVolume1 = 4;
    
    converter1.convert(&data, &bu, &tu, timeoffset);
    BOOST_CHECK(bu.getBidPx()[0] == Px(2030, 0.2));
    BOOST_CHECK(bu.getAskPx()[0] == Px(2030.4, 0.2));
    BOOST_CHECK(bu.getBidQty()[0] == 1);
    BOOST_CHECK(bu.getAskQty()[0] == 4);
    BOOST_CHECK(tu.getPx() == Px(2034.2, 0.2));
    BOOST_CHECK(tu.getQty() == 4000);
    BOOST_CHECK(bu.getTimestamp() == SimulatorCore::convertStringToTimePoint("2013-01-09T09:30:30.501"));
    BOOST_CHECK(tu.getTimestamp() == SimulatorCore::convertStringToTimePoint("2013-01-09T09:30:30.501"));
    
    //test convert filled order
    CThostFtdcTradeField trade;
    FilledOrderMessage filled;
    // date time?
    strcpy(trade.OrderRef, "1234");
    trade.Volume = 12;
    trade.Direction = THOST_FTDC_D_Buy;
    trade.Price = 120.0;
    converter1.convert(&trade, &filled, &oc);
    
    BOOST_CHECK(filled.getOrderId() == atoi(trade.OrderRef));
    BOOST_CHECK(filled.getQty() == trade.Volume);
    BOOST_CHECK(filled.getSide() == ((trade.Direction == THOST_FTDC_D_Buy) ? BuySellSide::buy : BuySellSide::sell));
    BOOST_CHECK(filled.getPx() == Px(trade.Price, 0.2));
    
    CThostFtdcOrderField order;
    CanceledOrderMessage<LimitOrderMessage> col;
    strcpy(order.OrderRef, "3456");
    order.VolumeTotal = 32;
    order.Direction = THOST_FTDC_D_Sell;
    order.LimitPrice = 134;
    
    converter1.convert(&order, &col);
    BOOST_CHECK(col.getOrderId() == atoi(order.OrderRef));
    BOOST_CHECK(col.getQty() == order.VolumeTotal);
    BOOST_CHECK(col.getSide() == ((order.Direction == THOST_FTDC_D_Buy) ? BuySellSide::buy : BuySellSide::sell));
    BOOST_CHECK(col.getPx() == Px(order.LimitPrice, 0.2));
    
    CThostFtdcOrderField morder;
    CanceledOrderMessage<MarketOrderMessage> com;
    strcpy(morder.OrderRef, "3352");
    morder.VolumeTotal = 33;
    morder.Direction = THOST_FTDC_D_Sell;
    
    converter1.convert(&morder, &com);
    BOOST_CHECK(com.getOrderId() == atoi(morder.OrderRef));
    BOOST_CHECK(com.getQty() == morder.VolumeTotal);
    BOOST_CHECK(com.getSide() == ((morder.Direction == THOST_FTDC_D_Buy) ? BuySellSide::buy : BuySellSide::sell));
    
    CThostFtdcOrderField rorder;
    RejectedOrderMessage<LimitOrderMessage> rol;
    strcpy(rorder.OrderRef, "3456");
    rorder.VolumeTotal = 32;
    rorder.Direction = THOST_FTDC_D_Sell;
    rorder.LimitPrice = 134;
    
    converter1.convert(&rorder, &rol);
    BOOST_CHECK(rol.getOrderId() == atoi(rorder.OrderRef));
    BOOST_CHECK(rol.getQty() == rorder.VolumeTotal);
    BOOST_CHECK(rol.getSide() == ((rorder.Direction == THOST_FTDC_D_Buy) ? BuySellSide::buy : BuySellSide::sell));
    BOOST_CHECK(rol.getPx() == Px(rorder.LimitPrice, 0.2));

    CThostFtdcOrderField rmorder;
    CanceledOrderMessage<MarketOrderMessage> rom;
    strcpy(rmorder.OrderRef, "3352");
    rmorder.VolumeTotal = 33;
    rmorder.Direction = THOST_FTDC_D_Sell;
	
    converter1.convert(&rmorder, &rom);
    BOOST_CHECK(rom.getOrderId() == atoi(rmorder.OrderRef));
    BOOST_CHECK(rom.getQty() == rmorder.VolumeTotal);
    BOOST_CHECK(rom.getSide() == ((rmorder.Direction == THOST_FTDC_D_Buy) ? BuySellSide::buy : BuySellSide::sell));
	
    // for reject cancel order
	
    CThostFtdcOrderField rclorder;
    RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage> > rocl;
    strcpy(rclorder.OrderRef, "3456");
    rclorder.VolumeTotal = 32;
    rclorder.Direction = THOST_FTDC_D_Sell;
    rclorder.LimitPrice = 134;
	
    converter1.convert(&rclorder, &rocl);
    BOOST_CHECK(rocl.getOrderId() == atoi(rclorder.OrderRef));
    BOOST_CHECK(rocl.getQty() == rclorder.VolumeTotal);
    BOOST_CHECK(rocl.getSide() == ((rclorder.Direction == THOST_FTDC_D_Buy) ? BuySellSide::buy : BuySellSide::sell));
    BOOST_CHECK(rocl.getPx() == Px(rclorder.LimitPrice, 0.2));

    CThostFtdcOrderField rcmorder;
    RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage> > rocm;
    strcpy(rcmorder.OrderRef, "3352");
    rcmorder.VolumeTotal = 33;
    rcmorder.Direction = THOST_FTDC_D_Sell;
	
    converter1.convert(&rcmorder, &rocm);
    BOOST_CHECK(rocm.getOrderId() == atoi(rcmorder.OrderRef));
    BOOST_CHECK(rocm.getQty() == rcmorder.VolumeTotal);
    BOOST_CHECK(rocm.getSide() == ((rcmorder.Direction == THOST_FTDC_D_Buy) ? BuySellSide::buy : BuySellSide::sell));

    CThostFtdcOrderField aorder;
    AcknowledgedOrderMessage aom;
    strcpy(aorder.OrderRef, "1235");

    converter1.convert(&aorder, &aom);
    BOOST_CHECK(aom.getOrderId() == atoi(aorder.OrderRef));

}


BOOST_AUTO_TEST_CASE(test_convertorder)
{
    ConverterOrder<CThostFtdcInputOrderField * > converterOrder;
    SimulatorCore sc("2012-01-03T09:15:00.000", "2012-01-03T15:15:00.000", 500, "mil");
    DummyCTPMatchyApi cffex("testEX", "textCon", &sc);

    string broker("mybroker");
    string investor("myinvestor");
    strcpy(cffex.brokerId(), broker.c_str());
    strcpy(cffex.investorId(), investor.c_str());
    strcpy(cffex.userId(), investor.c_str());
    cffex.frontId() = 100;
    cffex.sessionId() = 101;
	
    OrderCache oc;

    ConverterOrder<CThostFtdcInputOrderActionField * > converterCancel;

    MarketOrderMessage mom("testEX", "textCon", convertStringToTimePoint("20120103T09:30:00.0001000"),
			   0, 11, 20, 3, BuySellSide::buy);
    TThostFtdcRequestIDType requestId = 0;
    CThostFtdcInputOrderField order;
    cffex.inputOrderFieldInit(&order);
    converterOrder.convertMarketOrder(&mom, requestId, &order);
    BOOST_CHECK(strcmp(order.BrokerID, "mybroker") == 0);
    BOOST_CHECK(mom.getOrderId() == atoi(order.OrderRef));
    BOOST_CHECK(strcmp(order.InvestorID, "myinvestor") == 0);
    BOOST_CHECK(strcmp(order.InstrumentID, (mom.getContractId()).c_str()) == 0);
    BOOST_CHECK(strcmp(order.UserID, "myinvestor") == 0);
    BOOST_CHECK(order.OrderPriceType == THOST_FTDC_OPT_AnyPrice);
    BOOST_CHECK(order.Direction == ((mom.getSide() == BuySellSide::buy) ? THOST_FTDC_D_Buy : THOST_FTDC_D_Sell));
    BOOST_CHECK(order.CombHedgeFlag[0] == THOST_FTDC_HF_Speculation);
    BOOST_CHECK(order.VolumeTotalOriginal == mom.getQty());
    BOOST_CHECK(order.TimeCondition == THOST_FTDC_TC_IOC);
    BOOST_CHECK(order.VolumeCondition == THOST_FTDC_CC_Immediately);
    BOOST_CHECK(order.MinVolume == 1);
    BOOST_CHECK(order.ContingentCondition == THOST_FTDC_CC_Immediately);
    BOOST_CHECK(order.ForceCloseReason == THOST_FTDC_FCC_NotForceClose);
    BOOST_CHECK(order.IsAutoSuspend == 0);
    BOOST_CHECK(order.RequestID == requestId);
    BOOST_CHECK(order.UserForceClose == 0);


    LimitOrderMessage lom("testEX", "textCon", convertStringToTimePoint("20120103T09:30:00.0001000"),
			  0, 11, 20, 3, BuySellSide::buy, Px(200.2, 0.2));
    TThostFtdcRequestIDType lrequestId = 1;
    CThostFtdcInputOrderField lorder;
    cffex.inputOrderFieldInit(&lorder);
    converterOrder.convertLimitOrder(&lom, lrequestId, &lorder);
    BOOST_CHECK(strcmp(lorder.BrokerID, "mybroker") == 0);
    BOOST_CHECK(lom.getOrderId() == atoi(lorder.OrderRef));
    BOOST_CHECK(strcmp(lorder.InvestorID, "myinvestor") == 0);
    BOOST_CHECK(strcmp(lorder.InstrumentID, (lom.getContractId()).c_str()) == 0);
    BOOST_CHECK(strcmp(lorder.UserID, "myinvestor") == 0);
    BOOST_CHECK(lorder.OrderPriceType == THOST_FTDC_OPT_LimitPrice);
    BOOST_CHECK(lorder.Direction == ((lom.getSide() == BuySellSide::buy) ? THOST_FTDC_D_Buy : THOST_FTDC_D_Sell));
    BOOST_CHECK(lorder.CombHedgeFlag[0] == THOST_FTDC_HF_Speculation);
    BOOST_CHECK(lorder.LimitPrice == lom.getPx().getPx());
    BOOST_CHECK(lorder.TimeCondition == THOST_FTDC_TC_GFD);
    BOOST_CHECK(lorder.VolumeCondition == THOST_FTDC_VC_AV);
    BOOST_CHECK(lorder.MinVolume == 1);
    BOOST_CHECK(lorder.ContingentCondition== THOST_FTDC_CC_Immediately);
    BOOST_CHECK(lorder.ForceCloseReason == THOST_FTDC_FCC_NotForceClose);
    BOOST_CHECK(lorder.IsAutoSuspend == 0);
    BOOST_CHECK(lorder.RequestID == lrequestId);
    BOOST_CHECK(lorder.UserForceClose == 0);

    CThostFtdcOrderField mf, lf;
    strcpy(mf.BrokerID, "mybroker");
    strcpy(mf.InvestorID, "myinvestor");
    strcpy(mf.OrderRef, "123");
    mf.RequestID = 1;
    mf.FrontID = 100;
    mf.SessionID=101;
    strcpy(mf.ExchangeID, "testEX");
    strcpy(mf.OrderSysID, "sysID");
    strcpy(mf.UserID, "myuser");
    strcpy(mf.InstrumentID, "textCon");
    oc.insert(&mf);
    
    strcpy(lf.BrokerID, "mybroker");
    strcpy(lf.InvestorID, "myinvestor");
    strcpy(lf.OrderRef, "125");
    lf.RequestID = 1;
    lf.FrontID = 100;
    lf.SessionID=101;
    strcpy(lf.ExchangeID, "testEX");
    strcpy(lf.OrderSysID, "sysID");
    strcpy(lf.UserID, "myuser");
    strcpy(lf.InstrumentID, "textCon");
    lf.LimitPrice = 1234.0;
    oc.insert(&lf);


    CancelOrderMessage<MarketOrderMessage> com("testEX", "textCon", convertStringToTimePoint("20120103T09:30:00.0001000"),
					       0, 11, 123, 1, BuySellSide::buy);
    TThostFtdcRequestIDType crequestId = 2;
    CThostFtdcInputOrderActionField corder;
    BOOST_CHECK(converterCancel.convertCancelOrder(&com, crequestId, &corder, &oc));
    BOOST_CHECK(strcmp(corder.BrokerID, mf.BrokerID) == 0);
    BOOST_CHECK(strcmp(corder.InvestorID, mf.InvestorID)==0);
    BOOST_CHECK(atoi(corder.OrderRef) == com.getOrderId());
    BOOST_CHECK(corder.RequestID == crequestId);
    BOOST_CHECK(corder.FrontID == mf.FrontID);
    BOOST_CHECK(corder.SessionID == mf.SessionID);
    BOOST_CHECK(strcmp(corder.ExchangeID, mf.ExchangeID) == 0);
    BOOST_CHECK(strcmp(corder.OrderSysID, mf.OrderSysID) == 0);
    BOOST_CHECK(corder.ActionFlag == THOST_FTDC_AF_Delete);
    BOOST_CHECK(strcmp(corder.UserID, mf.UserID) == 0);
    BOOST_CHECK(strcmp(corder.InstrumentID, mf.InstrumentID) == 0);
    
    CancelOrderMessage<LimitOrderMessage> col_false("testEX", "textCon", convertStringToTimePoint("20120103T09:30:00.0001000"),
						    0, 11, 129, 1, BuySellSide::buy, Px(123, 0.2));
    CancelOrderMessage<LimitOrderMessage> col("testEX", "textCon", convertStringToTimePoint("20120103T09:30:00.0001000"),
					      0, 11, 125, 1, BuySellSide::buy, Px(lf.LimitPrice, 0.2));
    TThostFtdcRequestIDType clrequestId = 3;
    CThostFtdcInputOrderActionField clorder;
    BOOST_CHECK(!converterCancel.convertCancelOrder(&col_false, clrequestId, &clorder, &oc));

    BOOST_CHECK(converterCancel.convertCancelOrder(&col, clrequestId, &clorder, &oc));
    BOOST_CHECK(strcmp(clorder.BrokerID, lf.BrokerID) == 0);
    BOOST_CHECK(strcmp(clorder.InvestorID, lf.InvestorID)==0);
    BOOST_CHECK(atoi(clorder.OrderRef) == col.getOrderId());
    BOOST_CHECK(clorder.RequestID == clrequestId);
    BOOST_CHECK(clorder.FrontID == lf.FrontID);
    BOOST_CHECK(clorder.SessionID == lf.SessionID);
    BOOST_CHECK(strcmp(clorder.ExchangeID, lf.ExchangeID) == 0);
    BOOST_CHECK(strcmp(clorder.OrderSysID, lf.OrderSysID) == 0);
    BOOST_CHECK(clorder.ActionFlag == THOST_FTDC_AF_Delete);
    BOOST_CHECK(strcmp(clorder.UserID, lf.UserID) == 0);
    BOOST_CHECK(strcmp(clorder.InstrumentID, lf.InstrumentID) == 0);
    BOOST_CHECK(clorder.LimitPrice == lf.LimitPrice);
}
