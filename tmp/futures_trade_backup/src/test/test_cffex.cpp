//
// File:        $ test_cffex.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/01/19 17:50:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2012 by Zhengzhang Futures Co. Ltd.

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE unit_test_cffex
#include <boost/test/unit_test.hpp>
#include "SimulatorCore.hpp"
#include "CFFEX.hpp"
#include "Strategy.hpp"

using simulator::Px;
using simulator::AbstractMessage;
using simulator::BuySellSide;
using simulator::AbstractMessage;
using simulator::LimitOrderMessage;
using simulator::MarketOrderMessage;
using simulator::CancelOrderMessage;
using simulator::AcknowledgedOrderMessage;
using simulator::FilledOrderMessage;
using simulator::CanceledOrderMessage;
using simulator::RejectedOrderMessage;
using simulator::BookUpdate;
using simulator::TradeUpdate;
using simulator::SimulatorCore;
using simulator::duration_nano;
using simulator::time_stamp;
using simulator::CFFEX;

// according to unit.test.to.be.written.cffex
class TestExch : public CFFEX
{
public:
  TestExch(string contractId, string fileName, SimulatorCore * simptr) :
    CFFEX(contractId, fileName, simptr)
  { }
  OrderBook & orderBook() {return orderBook_;}

};


class TestStrategy : public Strategy
{
private:
  int messageCount_;
  int orderCount_;
public:
  TestStrategy(SimulatorCore * simptr) :
    Strategy("test", simptr, NULL),
    messageCount_(0),
    orderCount_(0)
  { 
    addStrat("CFFEX", "IF01", 300.0);
  }
  // just submit limit buy order to match the level 2 tick; and market sell order
  void process() 
  {
    if(currTime_ == SimulatorCore::convertStringToTimePoint("2012-08-15T09:15:25.500") ) {
      // send market order
      MarketOrderMessage mom("CFFEX", "IF01", currTime_, getStratId(), messageCount_++, orderCount_++,
			     1, BuySellSide::sell);
      marketOrderMessages_.push_back(mom);
    }
    if(currTime_ == SimulatorCore::convertStringToTimePoint("2012-08-15T09:20:14.000") ) {
      // send a delayed market order
      MarketOrderMessage mom("CFFEX", "IF01", currTime_ + duration_nano(1000 * 1000000ULL), getStratId(), messageCount_++, orderCount_++,
			     1, BuySellSide::buy);
      marketOrderMessages_.push_back(mom);
    }

    if(currTime_ == SimulatorCore::convertStringToTimePoint("2012-08-15T09:50:43.000") ) {
      // send limit order
      // should be filled at 2012-08-15T09:50:45.500
      LimitOrderMessage lom("CFFEX", "IF01", currTime_, getStratId(), messageCount_++, orderCount_++,
			       1, BuySellSide::buy, Px(2348.0, 0.2));
      limitOrderMessages_.push_back(lom);
    }
    if(currTime_ == SimulatorCore::convertStringToTimePoint("2012-08-15T10:14:28.000") ) {
      // send limit order
      // will never be filled
      LimitOrderMessage lom("CFFEX", "IF01", currTime_, getStratId(), messageCount_++, orderCount_++,
			    1, BuySellSide::buy, Px(2148.0, 0.2));
      limitOrderMessages_.push_back(lom);
    }
    if(currTime_ == SimulatorCore::convertStringToTimePoint("2012-08-15T10:14:40.500") ) {
      // send cancel that has been filled
      CancelOrderMessage<LimitOrderMessage> clom("CFFEX", "IF01", currTime_, getStratId(), messageCount_++, orderCount_-2,
						 1, BuySellSide::buy, Px(2348.0, 0.2));
      cancelLimitOrderMessages_.push_back(clom);
    }
    if(currTime_ == SimulatorCore::convertStringToTimePoint("2012-08-15T10:14:50.000") ) {
      // send cancel with wrong time
      CancelOrderMessage<LimitOrderMessage> clom("CFFEX", "IF01", SimulatorCore::convertStringToTimePoint("2012-08-15T10:14:18.000"), getStratId(), messageCount_++, orderCount_-1,
						 1, BuySellSide::buy, Px(2148.0, 0.2));
      cancelLimitOrderMessages_.push_back(clom);
    }
    if(currTime_ == SimulatorCore::convertStringToTimePoint("2012-08-15T10:16:50.000") ) {
      // actually cancel the order
      CancelOrderMessage<LimitOrderMessage> clom("CFFEX", "IF01", currTime_, getStratId(), messageCount_++, orderCount_-1,
						 1, BuySellSide::buy, Px(2148.0, 0.2));
      cancelLimitOrderMessages_.push_back(clom);
    }
    
  }
  //accessors for inspection
  vector<OrderBook> & orderbooks() {return orderbooks_;}
  
  list<LimitOrderMessage> & limitOrderMessages() {return limitOrderMessages_;}
  list<MarketOrderMessage> & marketOrderMessages() {return marketOrderMessages_;}
  list<CancelOrderMessage<LimitOrderMessage> > & cancelLimitOrderMessages() { return cancelLimitOrderMessages_;}
  list<CancelOrderMessage<MarketOrderMessage> > & cancelMarketOrderMessages() { return cancelMarketOrderMessages_; }
  
  /* incoming order messages to from exchange */
  vector<list<AcknowledgedOrderMessage> > & acknowledgedOrderMessages() {return acknowledgedOrderMessages_; }
  vector<list<FilledOrderMessage> > & filledOrderMessages() { return filledOrderMessages_; }
  vector<list<CanceledOrderMessage<LimitOrderMessage> > > & canceledLimitOrderMessages() { return canceledLimitOrderMessages_; }
  vector<list<CanceledOrderMessage<MarketOrderMessage> > > & canceledMarketOrderMessages() { return canceledMarketOrderMessages_; }
  vector<list<RejectedOrderMessage<LimitOrderMessage> > > & rejectedLimitOrderMessages() { return rejectedLimitOrderMessages_; }
  vector<list<RejectedOrderMessage<MarketOrderMessage> > > & rejectedMarketOrderMessages() {return  rejectedMarketOrderMessages_; }
  
  /* incoming market data from exchange */
  vector<list<BookUpdate<true> > > & bookUpdates() { return bookUpdates_; }
  vector<list<TradeUpdate<true> > > & tradeUpdates() { return tradeUpdates_; }
  
  vector<list<LimitOrderMessage> > & pendingLimitOrderMessages() { return pendingLimitOrderMessages_; }
  vector<list<MarketOrderMessage> > & pendingMarketOrderMessages() { return pendingMarketOrderMessages_; }
  vector<list<CancelOrderMessage<LimitOrderMessage> > > & pendingCancelLimitOrderMessages() { return pendingCancelLimitOrderMessages_; }
  vector<list<CancelOrderMessage<MarketOrderMessage> > > & pendingCancelMarketOrderMessages() { return pendingCancelMarketOrderMessages_; }

};



// CFFEX is a child class of Matchy, which has 2 major test tasks:
BOOST_AUTO_TEST_CASE(simmatchy_latency)
{
    {
	SimulatorCore sc("2012-08-15T09:15:00.0", "2012-08-15T15:15:00.0", 500, "millisecond");
	TestExch exch("IF01", "./IF.test.csv", &sc);
	TestStrategy strat(&sc);
	
	sc.registerMatchy(&exch);
	sc.registerStrategy(&strat);
	while(sc.getCurrTime() < SimulatorCore::convertStringToTimePoint("2012-08-15T09:15:25.500") ) {
	    sc.next();
	}
	sc.next();
	BOOST_CHECK(strat.getOpenPosition() == -1);
    }

    {
	SimulatorCore sc("2012-08-15T09:15:00.0", "2012-08-15T15:15:00.0", 500, "millisecond");
	TestExch exch("IF01", "./IF.test.csv", &sc);
	TestStrategy strat(&sc);
	exch.setLatency(550);

	sc.registerMatchy(&exch);
	sc.registerStrategy(&strat);
	while(sc.getCurrTime() < SimulatorCore::convertStringToTimePoint("2012-08-15T09:15:25.500") ) {
	    sc.next();
	}
	sc.next();
	BOOST_CHECK(strat.getOpenPosition() == 0);
	sc.next();
	BOOST_CHECK(strat.getOpenPosition() == -1);
    }
  

}


// 1. Check if preLoadData and init methods work as advertised in the constructor, using test data IF.test.csv.
BOOST_AUTO_TEST_CASE(cffex_preloaddata)
{
  SimulatorCore sim("2012-08-15T09:15:00.0", "2012-08-15T15:15:00.0", 500, "millisecond");
  CFFEX mc("IF13", "./IF.test.csv", &sim);
  BOOST_CHECK(mc.init());
  BOOST_CHECK(mc.getNextBookUpdate().front().getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
	      mc.getNextBookUpdate().front().getBidPx()[0] == Px(2355.2, 0.2) &&
	      mc.getNextBookUpdate().front().getBidPx()[1] == Px(2355.0, 0.2) &&
	      mc.getNextBookUpdate().front().getBidPx()[2] == Px(2354.4, 0.2) &&
	      mc.getNextBookUpdate().front().getBidPx()[3] == Px(2354.2, 0.2) &&
	      mc.getNextBookUpdate().front().getBidPx()[4] == Px(2354.0, 0.2) &&	   
	      mc.getNextBookUpdate().front().getBidQty()[0] == 8 &&
	      mc.getNextBookUpdate().front().getBidQty()[1] == 11 &&
	      mc.getNextBookUpdate().front().getBidQty()[2] == 1 &&
	      mc.getNextBookUpdate().front().getBidQty()[3] == 1 &&
	      mc.getNextBookUpdate().front().getBidQty()[4] == 9 &&	   
	      mc.getNextBookUpdate().front().getAskPx()[0] == Px(2355.6, 0.2) &&
	      mc.getNextBookUpdate().front().getAskPx()[1] == Px(2356.2, 0.2) &&
	      mc.getNextBookUpdate().front().getAskPx()[2] == Px(2356.6, 0.2) &&
	      mc.getNextBookUpdate().front().getAskPx()[3] == Px(2357.0, 0.2) &&
	      mc.getNextBookUpdate().front().getAskPx()[4] == Px(2357.4, 0.2) &&	   
	      mc.getNextBookUpdate().front().getAskQty()[0] == 6 &&
	      mc.getNextBookUpdate().front().getAskQty()[1] == 1 &&
	      mc.getNextBookUpdate().front().getAskQty()[2] == 1 &&
	      mc.getNextBookUpdate().front().getAskQty()[3] == 2 &&
	      mc.getNextBookUpdate().front().getAskQty()[4] == 1 );
  BOOST_CHECK(mc.getNextTradeUpdate().front().getPx() == Px(2355.4, 0.2) &&
	      mc.getNextTradeUpdate().front().getQty() == 68);
}

// 2. In the process() method paired with Strategy class, check it follows the logic:
BOOST_AUTO_TEST_CASE(cffex_process)
{
  SimulatorCore sc("2012-08-15T09:15:00.0", "2012-08-15T15:15:00.0", 500, "millisecond");
  TestExch exch("IF01", "./IF.test.csv", &sc);
  TestStrategy strat(&sc);

  sc.registerMatchy(&exch);
  sc.registerStrategy(&strat);

//    (a) clear inbound messages (limitOrderMessages_, marketOrderMessages_, cancelLimitOrderMessages_, cancelMarketOrderMessages_), which have been processed during the last process
  
//    (b) clear outbound messages (acknowledgedOrderMessages_, filledOrderMessages_, canceledLimitOrderMessages_, canceledMarketOrderMessages_, rejectedLimitOrderMessages_, rejectedMarketOrderMessages_), which have been passed to simulator core and consumed by Strategy class in the last round

//    (c) clear outbound tradeUpdates_ and bookUpdates_, which have been passed to simuator core and consumed by Strategy class in the last round

  //(a-c) are unit-tested in test_simulatorcore.cpp
  // this unit test focuses on (d) and (e)

//    (d) while there is still data to consume and the data timestamp is earlier than or equal to the simulator core timestamp, match market orders and update filled messages, match bid limit orders and update filled messages, match ask limit orders and update filled messages, push market data to bookUpdates_ and tradeUpdates_ while loading the next available market data
  while(sc.getCurrTime() < SimulatorCore::convertStringToTimePoint("2012-08-15T09:15:25.500") ) {
    sc.next();
  }
  //strat send market order
  BOOST_CHECK(sc.getCurrTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:15:25.500"));
  BOOST_CHECK(sc.limitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.marketOrderMessages().size() == 1 );
  BOOST_CHECK(sc.cancelLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.cancelMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.acknowledgedOrderMessages().size() == 0 );
  BOOST_CHECK(sc.filledOrderMessages().size() == 0 );
  BOOST_CHECK(sc.canceledLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.canceledMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.rejectedLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.rejectedMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.getTradeUpdate().size() == 0 );
  BOOST_CHECK(sc.getBookUpdate().size() == 0);


  // check status of exch
  // std::cout << SimulatorCore::convertTimePointToString(exch.getCurrTime()) << std::endl;
  BOOST_CHECK(exch.getCurrTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:15:26.000"));
  BOOST_CHECK(exch.acknowledgedOrderMessages().size() == 0 );
  BOOST_CHECK(exch.filledOrderMessages().size() == 0 );
  BOOST_CHECK(exch.canceledLimitOrderMessages().size() == 0 );
  BOOST_CHECK(exch.canceledMarketOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedLimitOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedMarketOrderMessages().size() == 0 );
  BOOST_CHECK(exch.tradeUpdates().size() == 1 );
  BOOST_CHECK(exch.bookUpdates().size() == 1);

  // check status of strat
  BOOST_CHECK(strat.getCurrentTime() == sc.getCurrTime());
  BOOST_CHECK(strat.limitOrderMessages().size() == 0 );
  BOOST_CHECK(strat.marketOrderMessages().size() == 0 );
  BOOST_CHECK(strat.cancelLimitOrderMessages().size() == 0 );
  BOOST_CHECK(strat.cancelMarketOrderMessages().size() == 0 );
  BOOST_CHECK(strat.acknowledgedOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.filledOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.canceledLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.canceledMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.rejectedLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.rejectedMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.bookUpdates()[0].size() == 0 );
  BOOST_CHECK(strat.tradeUpdates()[0].size() == 0 );
  BOOST_CHECK(strat.pendingLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingMarketOrderMessages()[0].size() == 1 );
  BOOST_CHECK(strat.pendingCancelLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingCancelMarketOrderMessages()[0].size() == 0);
  BOOST_CHECK(strat.orderbooks()[0].countLimitOrders() == 0 );
  BOOST_CHECK(strat.orderbooks()[0].countLimitOrders(BuySellSide::buy) == 0 );
  
  // market order gets filled
  sc.next();
  BOOST_CHECK(sc.getCurrTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:15:26.000"));
  BOOST_CHECK(sc.limitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.marketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.cancelLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.cancelMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.acknowledgedOrderMessages().size() == 0 );
  BOOST_CHECK(sc.filledOrderMessages().size() == 0 );
  BOOST_CHECK(sc.canceledLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.canceledMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.rejectedLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.rejectedMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.getTradeUpdate().size() == 0 );
  BOOST_CHECK(sc.getBookUpdate().size() == 0);


  // check status of exch
  //std::cout << SimulatorCore::convertTimePointToString(exch.getCurrTime()) << std::endl;
  BOOST_CHECK(exch.getCurrTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:15:26.500"));
  BOOST_CHECK(exch.acknowledgedOrderMessages().size() == 1 );
  BOOST_CHECK(exch.filledOrderMessages().size() == 1 );
  BOOST_CHECK(exch.canceledLimitOrderMessages().size() == 0 );
  BOOST_CHECK(exch.canceledMarketOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedLimitOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedMarketOrderMessages().size() == 0 );
  BOOST_CHECK(exch.tradeUpdates().size() == 1 );
  BOOST_CHECK(exch.bookUpdates().size() == 1);

  // check status of strat
  BOOST_CHECK(strat.getCurrentTime() == sc.getCurrTime());
  BOOST_CHECK(strat.limitOrderMessages().size() == 0 );
  BOOST_CHECK(strat.marketOrderMessages().size() == 0 );
  BOOST_CHECK(strat.cancelLimitOrderMessages().size() == 0 );
  BOOST_CHECK(strat.cancelMarketOrderMessages().size() == 0 );
  BOOST_CHECK(strat.acknowledgedOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.filledOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.canceledLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.canceledMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.rejectedLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.rejectedMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.bookUpdates()[0].size() == 0 );
  BOOST_CHECK(strat.tradeUpdates()[0].size() == 0 );
  BOOST_CHECK(strat.pendingLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingCancelLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingCancelMarketOrderMessages()[0].size() == 0);
  BOOST_CHECK(strat.orderbooks()[0].countLimitOrders() == 0 );
  BOOST_CHECK(strat.orderbooks()[0].countLimitOrders(BuySellSide::buy) == 0 );
  BOOST_CHECK(strat.getVolume() == 1);
  BOOST_CHECK(strat.getOpenPosition(0) == -1);

  // delayed market order
  while(sc.getCurrTime() < SimulatorCore::convertStringToTimePoint("2012-08-15T09:20:14.000") ) {
    sc.next();
  }
  
  //strat send market order
  BOOST_CHECK(sc.getCurrTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:20:14.000"));
  BOOST_CHECK(sc.limitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.marketOrderMessages().size() == 1 );
  BOOST_CHECK(sc.cancelLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.cancelMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.acknowledgedOrderMessages().size() == 0 );
  BOOST_CHECK(sc.filledOrderMessages().size() == 0 );
  BOOST_CHECK(sc.canceledLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.canceledMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.rejectedLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.rejectedMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.getTradeUpdate().size() == 0 );
  BOOST_CHECK(sc.getBookUpdate().size() == 0);


  // check status of exch
  //std::cout << SimulatorCore::convertTimePointToString(exch.getCurrTime()) << std::endl;
  BOOST_CHECK(exch.getCurrTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:20:14.500"));
  BOOST_CHECK(exch.acknowledgedOrderMessages().size() == 0 );
  BOOST_CHECK(exch.filledOrderMessages().size() == 0 );
  BOOST_CHECK(exch.canceledLimitOrderMessages().size() == 0 );
  BOOST_CHECK(exch.canceledMarketOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedLimitOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedMarketOrderMessages().size() == 0 );
  BOOST_CHECK(exch.tradeUpdates().size() == 1 );
  BOOST_CHECK(exch.bookUpdates().size() == 1);

  // check status of strat
  BOOST_CHECK(strat.getCurrentTime() == sc.getCurrTime());
  BOOST_CHECK(strat.limitOrderMessages().size() == 0 );
  BOOST_CHECK(strat.marketOrderMessages().size() == 0 );
  BOOST_CHECK(strat.cancelLimitOrderMessages().size() == 0 );
  BOOST_CHECK(strat.cancelMarketOrderMessages().size() == 0 );
  BOOST_CHECK(strat.acknowledgedOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.filledOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.canceledLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.canceledMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.rejectedLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.rejectedMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.bookUpdates()[0].size() == 0 );
  BOOST_CHECK(strat.tradeUpdates()[0].size() == 0 );
  BOOST_CHECK(strat.pendingLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingMarketOrderMessages()[0].size() == 1 );
  BOOST_CHECK(strat.pendingCancelLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingCancelMarketOrderMessages()[0].size() == 0);
  BOOST_CHECK(strat.orderbooks()[0].countLimitOrders() == 0 );
  BOOST_CHECK(strat.orderbooks()[0].countLimitOrders(BuySellSide::buy) == 0 );
  BOOST_CHECK(strat.getVolume() == 1);
  BOOST_CHECK(strat.getOpenPosition(0) == -1);
  
  sc.next();
  //ack the market order 
  //TO BE DISCUSSED should we ack a delayed order?
  //strat send market order
  BOOST_CHECK(sc.getCurrTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:20:14.500"));
  BOOST_CHECK(sc.limitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.marketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.cancelLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.cancelMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.acknowledgedOrderMessages().size() == 0 );
  BOOST_CHECK(sc.filledOrderMessages().size() == 0 );
  BOOST_CHECK(sc.canceledLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.canceledMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.rejectedLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.rejectedMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.getTradeUpdate().size() == 0 );
  BOOST_CHECK(sc.getBookUpdate().size() == 0);


  // check status of exch
  //std::cout << SimulatorCore::convertTimePointToString(exch.getCurrTime()) << std::endl;
  BOOST_CHECK(exch.getCurrTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:20:16.000"));
  BOOST_CHECK(exch.acknowledgedOrderMessages().size() == 1 );
  BOOST_CHECK(exch.filledOrderMessages().size() == 0);
  BOOST_CHECK(exch.canceledLimitOrderMessages().size() == 0 );
  BOOST_CHECK(exch.canceledMarketOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedLimitOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedMarketOrderMessages().size() == 0 );
  BOOST_CHECK(exch.tradeUpdates().size() == 3 );
  BOOST_CHECK(exch.bookUpdates().size() == 3);

  // check status of strat
  BOOST_CHECK(strat.getCurrentTime() == sc.getCurrTime());
  BOOST_CHECK(strat.limitOrderMessages().size() == 0 );
  BOOST_CHECK(strat.marketOrderMessages().size() == 0 );
  BOOST_CHECK(strat.cancelLimitOrderMessages().size() == 0 );
  BOOST_CHECK(strat.cancelMarketOrderMessages().size() == 0 );
  BOOST_CHECK(strat.acknowledgedOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.filledOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.canceledLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.canceledMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.rejectedLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.rejectedMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.bookUpdates()[0].size() == 0 );
  BOOST_CHECK(strat.tradeUpdates()[0].size() == 0 );
  BOOST_CHECK(strat.pendingLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingMarketOrderMessages()[0].size() == 1 );
  BOOST_CHECK(strat.pendingCancelLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingCancelMarketOrderMessages()[0].size() == 0);
  BOOST_CHECK(strat.orderbooks()[0].countLimitOrders() == 0 );
  BOOST_CHECK(strat.orderbooks()[0].countLimitOrders(BuySellSide::buy) == 0 );
  BOOST_CHECK(strat.getVolume() == 1);
  BOOST_CHECK(strat.getOpenPosition(0) == -1);

    
  sc.next();
  //sc.next();
  //sc.next();
  //fill the market order 
  //strat send market order
  BOOST_CHECK(sc.getCurrTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:20:15.000"));
  BOOST_CHECK(sc.limitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.marketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.cancelLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.cancelMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.acknowledgedOrderMessages().size() == 0 );
  BOOST_CHECK(sc.filledOrderMessages().size() == 0 );
  BOOST_CHECK(sc.canceledLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.canceledMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.rejectedLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.rejectedMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.getTradeUpdate().size() == 0 );
  BOOST_CHECK(sc.getBookUpdate().size() == 0);


  // check status of exch
  //std::cout << SimulatorCore::convertTimePointToString(exch.getCurrTime()) << std::endl;
  BOOST_CHECK(exch.getCurrTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:20:16.000"));
  BOOST_CHECK(exch.acknowledgedOrderMessages().size() == 0 );
  BOOST_CHECK(exch.filledOrderMessages().size() == 1 );
  BOOST_CHECK(exch.canceledLimitOrderMessages().size() == 0 );
  BOOST_CHECK(exch.canceledMarketOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedLimitOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedMarketOrderMessages().size() == 0 );
  BOOST_CHECK(exch.tradeUpdates().size() == 0 );
  BOOST_CHECK(exch.bookUpdates().size() == 0);

  // check status of strat
  BOOST_CHECK(strat.getCurrentTime() == sc.getCurrTime());
  BOOST_CHECK(strat.limitOrderMessages().size() == 0 );
  BOOST_CHECK(strat.marketOrderMessages().size() == 0 );
  BOOST_CHECK(strat.cancelLimitOrderMessages().size() == 0 );
  BOOST_CHECK(strat.cancelMarketOrderMessages().size() == 0 );
  BOOST_CHECK(strat.acknowledgedOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.filledOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.canceledLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.canceledMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.rejectedLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.rejectedMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.bookUpdates()[0].size() == 0 );
  BOOST_CHECK(strat.tradeUpdates()[0].size() == 0 );
  BOOST_CHECK(strat.pendingLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingCancelLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingCancelMarketOrderMessages()[0].size() == 0);
  BOOST_CHECK(strat.orderbooks()[0].countLimitOrders() == 0 );
  BOOST_CHECK(strat.orderbooks()[0].countLimitOrders(BuySellSide::buy) == 0 );
  BOOST_CHECK(strat.getVolume() == 2);
  BOOST_CHECK(strat.getOpenPosition(0) == 0);
  // std::cout << strat.getPnL() << std::endl;

  while(sc.getCurrTime() < SimulatorCore::convertStringToTimePoint("2012-08-15T09:50:43.000")) {
    sc.next();
  }
  BOOST_CHECK(sc.getCurrTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:50:43.000"));
  BOOST_CHECK(sc.limitOrderMessages().size() == 1 );
  BOOST_CHECK(sc.marketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.cancelLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.cancelMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.acknowledgedOrderMessages().size() == 0 );
  BOOST_CHECK(sc.filledOrderMessages().size() == 0 );
  BOOST_CHECK(sc.canceledLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.canceledMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.rejectedLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.rejectedMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.getTradeUpdate().size() == 0 );
  BOOST_CHECK(sc.getBookUpdate().size() == 0);


  // check status of exch
  //std::cout << SimulatorCore::convertTimePointToString(exch.getCurrTime()) << std::endl;
  BOOST_CHECK(exch.getCurrTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:50:43.500"));
  BOOST_CHECK(exch.acknowledgedOrderMessages().size() == 0 );
  BOOST_CHECK(exch.filledOrderMessages().size() == 0 );
  BOOST_CHECK(exch.canceledLimitOrderMessages().size() == 0 );
  BOOST_CHECK(exch.canceledMarketOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedLimitOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedMarketOrderMessages().size() == 0 );
  BOOST_CHECK(exch.tradeUpdates().size() == 1 );
  BOOST_CHECK(exch.bookUpdates().size() == 1);

  // check status of strat
  BOOST_CHECK(strat.getCurrentTime() == sc.getCurrTime());
  BOOST_CHECK(strat.limitOrderMessages().size() == 0 );
  BOOST_CHECK(strat.marketOrderMessages().size() == 0 );
  BOOST_CHECK(strat.cancelLimitOrderMessages().size() == 0 );
  BOOST_CHECK(strat.cancelMarketOrderMessages().size() == 0 );
  BOOST_CHECK(strat.acknowledgedOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.filledOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.canceledLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.canceledMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.rejectedLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.rejectedMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.bookUpdates()[0].size() == 0 );
  BOOST_CHECK(strat.tradeUpdates()[0].size() == 0 );
  BOOST_CHECK(strat.pendingLimitOrderMessages()[0].size() == 1 );
  BOOST_CHECK(strat.pendingMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingCancelLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingCancelMarketOrderMessages()[0].size() == 0);
  BOOST_CHECK(strat.orderbooks()[0].countLimitOrders() == 1 );
  BOOST_CHECK(strat.orderbooks()[0].countLimitOrders(BuySellSide::buy) == 1 );
  BOOST_CHECK(strat.getVolume() == 2);
  BOOST_CHECK(strat.getOpenPosition(0) == 0);
  // std::cout << strat.getPnL() << std::endl;
  
  while(sc.getCurrTime() < SimulatorCore::convertStringToTimePoint("2012-08-15T09:50:45.500")) {
    sc.next();
  }
  // limitorder filled
  BOOST_CHECK(sc.getCurrTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:50:45.500"));
  BOOST_CHECK(sc.limitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.marketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.cancelLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.cancelMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.acknowledgedOrderMessages().size() == 0 );
  BOOST_CHECK(sc.filledOrderMessages().size() == 0 );
  BOOST_CHECK(sc.canceledLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.canceledMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.rejectedLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.rejectedMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.getTradeUpdate().size() == 0 );
  BOOST_CHECK(sc.getBookUpdate().size() == 0);


  // check status of exch
  //std::cout << SimulatorCore::convertTimePointToString(exch.getCurrTime()) << std::endl;
  BOOST_CHECK(exch.getCurrTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:50:46.250"));
  BOOST_CHECK(exch.acknowledgedOrderMessages().size() == 0 );
  BOOST_CHECK(exch.filledOrderMessages().size() == 1 );
  BOOST_CHECK(exch.canceledLimitOrderMessages().size() == 0 );
  BOOST_CHECK(exch.canceledMarketOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedLimitOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedMarketOrderMessages().size() == 0 );
  BOOST_CHECK(exch.tradeUpdates().size() == 2 );
  BOOST_CHECK(exch.bookUpdates().size() == 2);

  // check status of strat
  BOOST_CHECK(strat.getCurrentTime() == sc.getCurrTime());
  BOOST_CHECK(strat.limitOrderMessages().size() == 0 );
  BOOST_CHECK(strat.marketOrderMessages().size() == 0 );
  BOOST_CHECK(strat.cancelLimitOrderMessages().size() == 0 );
  BOOST_CHECK(strat.cancelMarketOrderMessages().size() == 0 );
  BOOST_CHECK(strat.acknowledgedOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.filledOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.canceledLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.canceledMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.rejectedLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.rejectedMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.bookUpdates()[0].size() == 0 );
  BOOST_CHECK(strat.tradeUpdates()[0].size() == 0 );
  BOOST_CHECK(strat.pendingLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingCancelLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingCancelMarketOrderMessages()[0].size() == 0);
  BOOST_CHECK(strat.orderbooks()[0].countLimitOrders() == 0 );
  BOOST_CHECK(strat.orderbooks()[0].countLimitOrders(BuySellSide::buy) == 0 );
  BOOST_CHECK(strat.getVolume() == 3);
  BOOST_CHECK(strat.getOpenPosition(0) == 1);
  

//    (e) move inbound messages to internal order book, reject cancel orders if cannot cancel due to order has been completely filled or cancel order timestamp ealier than order timestamp
  while(sc.getCurrTime() < SimulatorCore::convertStringToTimePoint("2012-08-15T10:14:40.500")) {
    //submitted another limit order
    sc.next();
  }
  // submitted a cancel to a filled order
  BOOST_CHECK(sc.getCurrTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T10:14:40.500"));
  BOOST_CHECK(sc.limitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.marketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.cancelLimitOrderMessages().size() == 1 );
  BOOST_CHECK(sc.cancelMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.acknowledgedOrderMessages().size() == 0 );
  BOOST_CHECK(sc.filledOrderMessages().size() == 0 );
  BOOST_CHECK(sc.canceledLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.canceledMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.rejectedLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.rejectedMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.getTradeUpdate().size() == 0 );
  BOOST_CHECK(sc.getBookUpdate().size() == 0);


  // check status of exch
  //std::cout << SimulatorCore::convertTimePointToString(exch.getCurrTime()) << std::endl;
  BOOST_CHECK(exch.getCurrTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T10:14:41.000"));
  BOOST_CHECK(exch.acknowledgedOrderMessages().size() == 0 );
  BOOST_CHECK(exch.filledOrderMessages().size() == 0 );
  BOOST_CHECK(exch.canceledLimitOrderMessages().size() == 0 );
  BOOST_CHECK(exch.canceledMarketOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedLimitOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedMarketOrderMessages().size() == 0 );
  BOOST_CHECK(exch.tradeUpdates().size() == 1 );
  BOOST_CHECK(exch.bookUpdates().size() == 1);

  // check status of strat
  BOOST_CHECK(strat.getCurrentTime() == sc.getCurrTime());
  BOOST_CHECK(strat.limitOrderMessages().size() == 0 );
  BOOST_CHECK(strat.marketOrderMessages().size() == 0 );
  BOOST_CHECK(strat.cancelLimitOrderMessages().size() == 0 );
  BOOST_CHECK(strat.cancelMarketOrderMessages().size() == 0 );
  BOOST_CHECK(strat.acknowledgedOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.filledOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.canceledLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.canceledMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.rejectedLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.rejectedMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.bookUpdates()[0].size() == 0 );
  BOOST_CHECK(strat.tradeUpdates()[0].size() == 0 );
  BOOST_CHECK(strat.pendingLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingCancelLimitOrderMessages()[0].size() == 1 );
  BOOST_CHECK(strat.pendingCancelMarketOrderMessages()[0].size() == 0);
  BOOST_CHECK(strat.orderbooks()[0].countLimitOrders() == 1 );
  BOOST_CHECK(strat.orderbooks()[0].countLimitOrders(BuySellSide::buy) == 1 );
  BOOST_CHECK(strat.getVolume() == 3);
  BOOST_CHECK(strat.getOpenPosition(0) == 1);

  sc.next();
  // get rejected
  BOOST_CHECK(sc.getCurrTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T10:14:41.000"));
  BOOST_CHECK(sc.limitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.marketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.cancelLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.cancelMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.acknowledgedOrderMessages().size() == 0 );
  BOOST_CHECK(sc.filledOrderMessages().size() == 0 );
  BOOST_CHECK(sc.canceledLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.canceledMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.rejectedLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.rejectedMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.getTradeUpdate().size() == 0 );
  BOOST_CHECK(sc.getBookUpdate().size() == 0);


  // check status of exch
  //std::cout << SimulatorCore::convertTimePointToString(exch.getCurrTime()) << std::endl;
  BOOST_CHECK(exch.getCurrTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T10:14:41.500"));
  BOOST_CHECK(exch.acknowledgedOrderMessages().size() == 1 );
  BOOST_CHECK(exch.filledOrderMessages().size() == 0 );
  BOOST_CHECK(exch.canceledLimitOrderMessages().size() == 0 );
  BOOST_CHECK(exch.canceledMarketOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedLimitOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedMarketOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedCancelLimitOrderMessages().size() == 1 );
  BOOST_CHECK(exch.rejectedCancelMarketOrderMessages().size() == 0 );
    
  BOOST_CHECK(exch.tradeUpdates().size() == 1 );
  BOOST_CHECK(exch.bookUpdates().size() == 1);

  // check status of strat
  BOOST_CHECK(strat.getCurrentTime() == sc.getCurrTime());
  BOOST_CHECK(strat.limitOrderMessages().size() == 0 );
  BOOST_CHECK(strat.marketOrderMessages().size() == 0 );
  BOOST_CHECK(strat.cancelLimitOrderMessages().size() == 0 );
  BOOST_CHECK(strat.cancelMarketOrderMessages().size() == 0 );
  BOOST_CHECK(strat.acknowledgedOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.filledOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.canceledLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.canceledMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.rejectedLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.rejectedMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.bookUpdates()[0].size() == 0 );
  BOOST_CHECK(strat.tradeUpdates()[0].size() == 0 );
  BOOST_CHECK(strat.pendingLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingCancelLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingCancelMarketOrderMessages()[0].size() == 0);
  BOOST_CHECK(strat.orderbooks()[0].countLimitOrders() == 1 );
  BOOST_CHECK(strat.orderbooks()[0].countLimitOrders(BuySellSide::buy) == 1 );
  BOOST_CHECK(strat.getVolume() == 3);
  BOOST_CHECK(strat.getOpenPosition(0) == 1);
  
  while(sc.getCurrTime() < SimulatorCore::convertStringToTimePoint("2012-08-15T10:14:50.000")) {
    sc.next();
  }
  // submitted a cancel with a wrong time
  BOOST_CHECK(sc.getCurrTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T10:14:50.000"));
  BOOST_CHECK(sc.limitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.marketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.cancelLimitOrderMessages().size() == 1 );
  BOOST_CHECK(sc.cancelMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.acknowledgedOrderMessages().size() == 0 );
  BOOST_CHECK(sc.filledOrderMessages().size() == 0 );
  BOOST_CHECK(sc.canceledLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.canceledMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.rejectedLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.rejectedMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.getTradeUpdate().size() == 0 );
  BOOST_CHECK(sc.getBookUpdate().size() == 0);


  // check status of exch
  //std::cout << SimulatorCore::convertTimePointToString(exch.getCurrTime()) << std::endl;
  BOOST_CHECK(exch.getCurrTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T10:14:50.500"));
  BOOST_CHECK(exch.acknowledgedOrderMessages().size() == 0 );
  BOOST_CHECK(exch.filledOrderMessages().size() == 0 );
  BOOST_CHECK(exch.canceledLimitOrderMessages().size() == 0 );
  BOOST_CHECK(exch.canceledMarketOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedLimitOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedMarketOrderMessages().size() == 0 );
  BOOST_CHECK(exch.tradeUpdates().size() == 1 );
  BOOST_CHECK(exch.bookUpdates().size() == 1);
  
  // check status of strat
  BOOST_CHECK(strat.getCurrentTime() == sc.getCurrTime());
  BOOST_CHECK(strat.limitOrderMessages().size() == 0 );
  BOOST_CHECK(strat.marketOrderMessages().size() == 0 );
  BOOST_CHECK(strat.cancelLimitOrderMessages().size() == 0 );
  BOOST_CHECK(strat.cancelMarketOrderMessages().size() == 0 );
  BOOST_CHECK(strat.acknowledgedOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.filledOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.canceledLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.canceledMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.rejectedLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.rejectedMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.bookUpdates()[0].size() == 0 );
  BOOST_CHECK(strat.tradeUpdates()[0].size() == 0 );
  BOOST_CHECK(strat.pendingLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingCancelLimitOrderMessages()[0].size() == 1 );
  BOOST_CHECK(strat.pendingCancelMarketOrderMessages()[0].size() == 0);
  BOOST_CHECK(strat.orderbooks()[0].countLimitOrders() == 1 );
  BOOST_CHECK(strat.orderbooks()[0].countLimitOrders(BuySellSide::buy) == 1 );
  BOOST_CHECK(strat.getVolume() == 3);
  BOOST_CHECK(strat.getOpenPosition(0) == 1);

  sc.next();
  // get rejected
  BOOST_CHECK(sc.getCurrTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T10:14:50.500"));
  BOOST_CHECK(sc.limitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.marketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.cancelLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.cancelMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.acknowledgedOrderMessages().size() == 0 );
  BOOST_CHECK(sc.filledOrderMessages().size() == 0 );
  BOOST_CHECK(sc.canceledLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.canceledMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.rejectedLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.rejectedMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.getTradeUpdate().size() == 0 );
  BOOST_CHECK(sc.getBookUpdate().size() == 0);


  // check status of exch
  //std::cout << SimulatorCore::convertTimePointToString(exch.getCurrTime()) << std::endl;
  BOOST_CHECK(exch.getCurrTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T10:14:51.000"));
  BOOST_CHECK(exch.acknowledgedOrderMessages().size() == 1 );
  BOOST_CHECK(exch.filledOrderMessages().size() == 0 );
  BOOST_CHECK(exch.canceledLimitOrderMessages().size() == 0 );
  BOOST_CHECK(exch.canceledMarketOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedLimitOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedMarketOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedCancelLimitOrderMessages().size() == 1 );
  BOOST_CHECK(exch.rejectedCancelMarketOrderMessages().size() == 0 );
  BOOST_CHECK(exch.tradeUpdates().size() == 1 );
  BOOST_CHECK(exch.bookUpdates().size() == 1);

  // check status of strat
  BOOST_CHECK(strat.getCurrentTime() == sc.getCurrTime());
  BOOST_CHECK(strat.limitOrderMessages().size() == 0 );
  BOOST_CHECK(strat.marketOrderMessages().size() == 0 );
  BOOST_CHECK(strat.cancelLimitOrderMessages().size() == 0 );
  BOOST_CHECK(strat.cancelMarketOrderMessages().size() == 0 );
  BOOST_CHECK(strat.acknowledgedOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.filledOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.canceledLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.canceledMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.rejectedLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.rejectedMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.bookUpdates()[0].size() == 0 );
  BOOST_CHECK(strat.tradeUpdates()[0].size() == 0 );
  BOOST_CHECK(strat.pendingLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingCancelLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingCancelMarketOrderMessages()[0].size() == 0);
  BOOST_CHECK(strat.orderbooks()[0].countLimitOrders() == 1 );
  BOOST_CHECK(strat.orderbooks()[0].countLimitOrders(BuySellSide::buy) == 1 );
  BOOST_CHECK(strat.getVolume() == 3);
  BOOST_CHECK(strat.getOpenPosition(0) == 1);

  while(sc.getCurrTime() < SimulatorCore::convertStringToTimePoint("2012-08-15T10:16:50.000")) {
    sc.next();
  }
  // actually cancel the orde
  BOOST_CHECK(sc.getCurrTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T10:16:50.000"));
  BOOST_CHECK(sc.limitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.marketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.cancelLimitOrderMessages().size() == 1 );
  BOOST_CHECK(sc.cancelMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.acknowledgedOrderMessages().size() == 0 );
  BOOST_CHECK(sc.filledOrderMessages().size() == 0 );
  BOOST_CHECK(sc.canceledLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.canceledMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.rejectedLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.rejectedMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.getTradeUpdate().size() == 0 );
  BOOST_CHECK(sc.getBookUpdate().size() == 0);


  // check status of exch
  //std::cout << SimulatorCore::convertTimePointToString(exch.getCurrTime()) << std::endl;
  BOOST_CHECK(exch.getCurrTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T10:16:50.500"));
  BOOST_CHECK(exch.acknowledgedOrderMessages().size() == 0 );
  BOOST_CHECK(exch.filledOrderMessages().size() == 0 );
  BOOST_CHECK(exch.canceledLimitOrderMessages().size() == 0 );
  BOOST_CHECK(exch.canceledMarketOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedLimitOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedMarketOrderMessages().size() == 0 );
  BOOST_CHECK(exch.tradeUpdates().size() == 1 );
  BOOST_CHECK(exch.bookUpdates().size() == 1);

  // check status of strat
  BOOST_CHECK(strat.getCurrentTime() == sc.getCurrTime());
  BOOST_CHECK(strat.limitOrderMessages().size() == 0 );
  BOOST_CHECK(strat.marketOrderMessages().size() == 0 );
  BOOST_CHECK(strat.cancelLimitOrderMessages().size() == 0 );
  BOOST_CHECK(strat.cancelMarketOrderMessages().size() == 0 );
  BOOST_CHECK(strat.acknowledgedOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.filledOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.canceledLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.canceledMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.rejectedLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.rejectedMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.bookUpdates()[0].size() == 0 );
  BOOST_CHECK(strat.tradeUpdates()[0].size() == 0 );
  BOOST_CHECK(strat.pendingLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingCancelLimitOrderMessages()[0].size() == 1 );
  BOOST_CHECK(strat.pendingCancelMarketOrderMessages()[0].size() == 0);
  BOOST_CHECK(strat.orderbooks()[0].countLimitOrders() == 1 );
  BOOST_CHECK(strat.orderbooks()[0].countLimitOrders(BuySellSide::buy) == 1 );
  BOOST_CHECK(strat.getVolume() == 3);
  BOOST_CHECK(strat.getOpenPosition(0) == 1);

  sc.next();
  // successfully canceled
  BOOST_CHECK(sc.getCurrTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T10:16:50.500"));
  BOOST_CHECK(sc.limitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.marketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.cancelLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.cancelMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.acknowledgedOrderMessages().size() == 0 );
  BOOST_CHECK(sc.filledOrderMessages().size() == 0 );
  BOOST_CHECK(sc.canceledLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.canceledMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.rejectedLimitOrderMessages().size() == 0 );
  BOOST_CHECK(sc.rejectedMarketOrderMessages().size() == 0 );
  BOOST_CHECK(sc.getTradeUpdate().size() == 0 );
  BOOST_CHECK(sc.getBookUpdate().size() == 0);


  // check status of exch
  //std::cout << SimulatorCore::convertTimePointToString(exch.getCurrTime()) << std::endl;
  BOOST_CHECK(exch.getCurrTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T10:16:51.000"));
  BOOST_CHECK(exch.acknowledgedOrderMessages().size() == 1 );
  BOOST_CHECK(exch.filledOrderMessages().size() == 0 );
  BOOST_CHECK(exch.canceledLimitOrderMessages().size() == 1 );
  BOOST_CHECK(exch.canceledMarketOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedLimitOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedMarketOrderMessages().size() == 0 );
  BOOST_CHECK(exch.tradeUpdates().size() == 1 );
  BOOST_CHECK(exch.bookUpdates().size() == 1);

  // check status of strat
  BOOST_CHECK(strat.getCurrentTime() == sc.getCurrTime());
  BOOST_CHECK(strat.limitOrderMessages().size() == 0 );
  BOOST_CHECK(strat.marketOrderMessages().size() == 0 );
  BOOST_CHECK(strat.cancelLimitOrderMessages().size() == 0 );
  BOOST_CHECK(strat.cancelMarketOrderMessages().size() == 0 );
  BOOST_CHECK(strat.acknowledgedOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.filledOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.canceledLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.canceledMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.rejectedLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.rejectedMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.bookUpdates()[0].size() == 0 );
  BOOST_CHECK(strat.tradeUpdates()[0].size() == 0 );
  BOOST_CHECK(strat.pendingLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingMarketOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingCancelLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingCancelMarketOrderMessages()[0].size() == 0);
  BOOST_CHECK(strat.orderbooks()[0].countLimitOrders() == 0 );
  BOOST_CHECK(strat.orderbooks()[0].countLimitOrders(BuySellSide::buy) == 0 );
  BOOST_CHECK(strat.getVolume() == 3);
  BOOST_CHECK(strat.getOpenPosition(0) == 1);
  // std::cout << strat.getPnL() << std::endl;

}
