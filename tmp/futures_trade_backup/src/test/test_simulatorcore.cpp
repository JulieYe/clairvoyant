//
// File:        $ test_simulatorcore.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/01/19 21:15:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2012 by Zhengzhang Futures Co. Ltd.

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE unit_test_simulatorcore
#include <boost/test/unit_test.hpp>

#include <string>
#include <ctime>

using std::string;

#include "SimulatorCore.hpp"
#include "CFFEX.hpp"
#include "Strategy.hpp"
#include "Logger.hpp"

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
using simulator::Strategy;
using simulator::Logger;

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
    if(messageCount_ < 1 && bookUpdates_[0].size() > 0 && tradeUpdates_[0].size() > 0) {
      LimitOrderMessage blimit("CFFEX", "IF01", currTime_, getStratId(), messageCount_++, orderCount_++,
			       1, BuySellSide::buy, bookUpdates_[0].front().getBidPx()[1]);
      MarketOrderMessage smarket("CFFEX", "IF01", currTime_, getStratId(), messageCount_++, orderCount_++,
				 1, BuySellSide::sell);
      limitOrderMessages_.push_back(blimit);
      marketOrderMessages_.push_back(smarket);
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



// according to unit.test.to.be.written.simulatorcore
// 1. Check convertStringToTimePoint and convertTimePointToString methods.
BOOST_AUTO_TEST_CASE(simulatorcore_convert_between_timepoint_and_string)
{
  //time_stamp is timezone dependent (on local machine)
  time_stamp tp = SimulatorCore::convertStringToTimePoint("1970-01-01T00:00:00.0");
  //BOOST_CHECK_MESSAGE(tp.time_since_epoch().count() == 0ULL, "time_stamp is timezone dependent, probably not a big deal for now");
  BOOST_CHECK(tp.time_since_epoch().count() == 21600000000000ULL);
  BOOST_CHECK(SimulatorCore::convertTimePointToString(time_stamp(duration_nano(21600000000000ULL))) == "1970-01-01T00:00:00.000000000");
  string tstr = SimulatorCore::convertTimePointToString(SimulatorCore::convertStringToTimePoint("2012-09-07T09:15:00.1234"));
  BOOST_CHECK(tstr == "2012-09-07T09:15:00.123400000");
}

// 2. Check carefully the logic of next() method.
BOOST_AUTO_TEST_CASE(simulatorcore_next_method)
{ 
  SimulatorCore sc("2012-08-15T09:15:00.0", "2012-08-15T15:15:00.0", 500, "millisecond");
  TestExch exch("IF01", "../test/IF.test.csv", &sc);
  TestStrategy strat(&sc);

  sc.registerMatchy(&exch);
  sc.registerStrategy(&strat);

  sc.next();
  
  // check status of sc
  //std::cout << (*tzname) << std::endl;
  BOOST_CHECK(sc.getStartTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:15:00.0"));
  BOOST_CHECK(sc.getCurrTime() == sc.getStartTime() + duration_nano( 500 * 1000000ULL));
  BOOST_CHECK(sc.limitOrderMessages().size() == 1 );
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
  BOOST_CHECK(exch.getCurrTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:15:01.000"));
  BOOST_CHECK(exch.acknowledgedOrderMessages().size() == 0 );
  BOOST_CHECK(exch.filledOrderMessages().size() == 0 );
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
  BOOST_CHECK(strat.pendingLimitOrderMessages()[0].size() == 1 );
  BOOST_CHECK(strat.pendingMarketOrderMessages()[0].size() == 1 );
  BOOST_CHECK(strat.pendingCancelLimitOrderMessages()[0].size() == 0 );
  BOOST_CHECK(strat.pendingCancelMarketOrderMessages()[0].size() == 0);
  BOOST_CHECK(strat.orderbooks()[0].countLimitOrders() == 1 );
  BOOST_CHECK(strat.orderbooks()[0].countLimitOrders(BuySellSide::buy) == 1 );
  
  // next again
  sc.next();
  
  // check status of sc
  BOOST_CHECK(sc.getStartTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:15:00.0"));
  BOOST_CHECK(sc.getCurrTime() == sc.getStartTime() + duration_nano( 1000 * 1000000ULL));
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
  BOOST_CHECK(exch.getCurrTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:15:01.500"));
  BOOST_CHECK(exch.acknowledgedOrderMessages().size() == 2 );
  BOOST_CHECK(exch.filledOrderMessages().size() == 1 );
  BOOST_CHECK(exch.canceledLimitOrderMessages().size() == 0 );
  BOOST_CHECK(exch.canceledMarketOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedLimitOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedMarketOrderMessages().size() == 0 );
  BOOST_CHECK(exch.tradeUpdates().size() == 1 );
  BOOST_CHECK(exch.bookUpdates().size() == 1);
  BOOST_CHECK(exch.orderBook().countLimitOrders() == 1);
  BOOST_CHECK(exch.orderBook().countMarketOrders() == 0);

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
  BOOST_CHECK(strat.getVolume() == 1);
  BOOST_CHECK(strat.getOpenPosition(0) == -1);

  // next again
  sc.next();
  
  // check status of sc
  BOOST_CHECK(sc.getStartTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:15:00.0"));
  BOOST_CHECK(sc.getCurrTime() == sc.getStartTime() + duration_nano( 1500 * 1000000ULL));
  BOOST_CHECK(sc.limitOrderMessages().size() == 0 &&
	      sc.marketOrderMessages().size() == 0 &&
	      sc.cancelLimitOrderMessages().size() == 0 &&
	      sc.cancelMarketOrderMessages().size() == 0 &&
	      sc.acknowledgedOrderMessages().size() == 0 &&
	      sc.filledOrderMessages().size() == 0 &&
	      sc.canceledLimitOrderMessages().size() == 0 &&
	      sc.canceledMarketOrderMessages().size() == 0 &&
	      sc.rejectedLimitOrderMessages().size() == 0 &&
	      sc.rejectedMarketOrderMessages().size() == 0 &&
	      sc.getTradeUpdate().size() == 0 &&
	      sc.getBookUpdate().size() == 0);

  // check status of exch
  //std::cout << SimulatorCore::convertTimePointToString(exch.getCurrTime()) << std::endl;
  BOOST_CHECK(exch.getCurrTime() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:15:02.000"));
  BOOST_CHECK(exch.acknowledgedOrderMessages().size() == 0 );
  BOOST_CHECK(exch.filledOrderMessages().size() == 0 );
  BOOST_CHECK(exch.canceledLimitOrderMessages().size() == 0 );
  BOOST_CHECK(exch.canceledMarketOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedLimitOrderMessages().size() == 0 );
  BOOST_CHECK(exch.rejectedMarketOrderMessages().size() == 0 );
  BOOST_CHECK(exch.tradeUpdates().size() == 1 );
  BOOST_CHECK(exch.bookUpdates().size() == 1);
  BOOST_CHECK(exch.orderBook().countLimitOrders() == 1);
  BOOST_CHECK(exch.orderBook().countMarketOrders() == 0);

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
  BOOST_CHECK(strat.getVolume() == 1);
  BOOST_CHECK(strat.getOpenPosition(0) == -1);

}
