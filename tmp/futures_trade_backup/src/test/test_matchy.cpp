//
// File:        $ test_matchy.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/01/19 16:18:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2012 by Zhengzhang Futures Co. Ltd.

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE unit_test_matchy
#include <boost/test/unit_test.hpp>

#include <string>

using std::string;

#include "Message.hpp"
#include "SimulatorCore.hpp"
#include "Matchy.hpp"

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
using simulator::Matchy;

// according to unit.test.to.be.written.matchy
// Mostly check whether acknowledgeOrderMessage(*) works for every type of message.

// to test the basic functionalities of the abstract class, create this subclass
class MatchyTest : public Matchy
{
public:
  MatchyTest(string exchangeId, string contractId, SimulatorCore *simptr) :
    Matchy(exchangeId, contractId, simptr)
  { }
  void process(int orderId) {}
  // add accessors to check stuff
  list<LimitOrderMessage> & limitOrderList() {return limitOrderMessages_;}
  list<MarketOrderMessage> & marketOrderList() { return marketOrderMessages_; }
  list<CancelOrderMessage<LimitOrderMessage> > & cancelLimitList() {return cancelLimitOrderMessages_; }
  list<CancelOrderMessage<MarketOrderMessage> > & cancelMarketList() {return cancelMarketOrderMessages_; }
  
};



// 1. LimitOrderMessage: check limitOrderMessages_ and acknowledgedOrderMessages_
BOOST_AUTO_TEST_CASE(matchy_acknowledge_limitordermessages)
{
  SimulatorCore sim("2012-08-15T09:15:00.0", "2012-08-15T15:15:00.0", 500, "millisecond");
  MatchyTest mc("CFFEX", "IF13", &sim);
  BOOST_CHECK(true);
  LimitOrderMessage blom("CFFEX", "IF13", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"),
			 1, 101, 15, 10, BuySellSide::buy, Px(100.1, 0.02));
  LimitOrderMessage blom_lower("CFFEX", "IF13", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:01.250"),
			       1, 102, 16, 30, BuySellSide::buy, Px(100.0, 0.02));
  LimitOrderMessage blom_higher("CFFEX", "IF13", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:03.250"),
				1, 105, 18, 8, BuySellSide::buy, Px(100.4, 0.02));
  mc.acknowledgeOrderMessage(blom);
  BOOST_CHECK(mc.limitOrderList().size() == 1 &&
	      mc.limitOrderList().back().getOrderId() == blom.getOrderId());
  BOOST_CHECK(mc.acknowledgedOrderMessages().size() == 1 &&
	      mc.acknowledgedOrderMessages().back().getExchangeId() == blom.getExchangeId() &&
	      mc.acknowledgedOrderMessages().back().getContractId() == blom.getContractId() &&
	      mc.acknowledgedOrderMessages().back().getStratId() == blom.getStratId() &&
	      mc.acknowledgedOrderMessages().back().getOrderId() == blom.getOrderId() &&
	      mc.acknowledgedOrderMessages().back().getMessageId() == 0);
  //std::cout << SimulatorCore::convertTimePointToString(mc.getCurrTime()) << std::endl;
  mc.acknowledgeOrderMessage(blom_higher);
  BOOST_CHECK(mc.limitOrderList().size() == 2 &&
	      mc.limitOrderList().back().getOrderId() == blom_higher.getOrderId());
  BOOST_CHECK(mc.acknowledgedOrderMessages().size() == 2 &&
	      mc.acknowledgedOrderMessages().back().getExchangeId() == blom_higher.getExchangeId() &&
	      mc.acknowledgedOrderMessages().back().getContractId() == blom_higher.getContractId() &&
	      mc.acknowledgedOrderMessages().back().getStratId() == blom_higher.getStratId() &&
	      mc.acknowledgedOrderMessages().back().getOrderId() == blom_higher.getOrderId() &&
	      mc.acknowledgedOrderMessages().back().getMessageId() == 1);
  mc.acknowledgeOrderMessage(blom_lower);
  BOOST_CHECK(mc.limitOrderList().size() == 3 &&
	      mc.limitOrderList().back().getOrderId() == blom_lower.getOrderId());
  BOOST_CHECK(mc.acknowledgedOrderMessages().size() == 3 &&
	      mc.acknowledgedOrderMessages().back().getExchangeId() == blom_lower.getExchangeId() &&
	      mc.acknowledgedOrderMessages().back().getContractId() == blom_lower.getContractId() &&
	      mc.acknowledgedOrderMessages().back().getStratId() == blom_lower.getStratId() &&
	      mc.acknowledgedOrderMessages().back().getOrderId() == blom_lower.getOrderId() &&
	      mc.acknowledgedOrderMessages().back().getMessageId() == 2);
  
}

// 2. MarketOrderMessage: check marketOrderMessages_ and acknowledgedOrderMessages_
BOOST_AUTO_TEST_CASE(matchy_acknowledge_marketordermessages)
{
  SimulatorCore sim("2012-08-15T09:15:00.0", "2012-08-15T15:15:00.0", 500, "millisecond");
  MatchyTest mc("CFFEX", "IF13", &sim);
  BOOST_CHECK(true);
  MarketOrderMessage mom("CFFEX", "IF13", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"),
			 1, 101, 15, 10, BuySellSide::buy);
  MarketOrderMessage mom_later("CFFEX", "IF13", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:01.250"),
			       1, 102, 16, 30, BuySellSide::sell);
  mc.acknowledgeOrderMessage(mom);
  BOOST_CHECK(mc.marketOrderList().size() == 1 &&
	      mc.marketOrderList().back().getOrderId() == mom.getOrderId());
  BOOST_CHECK(mc.acknowledgedOrderMessages().size() == 1 &&
	      mc.acknowledgedOrderMessages().back().getExchangeId() == mom.getExchangeId() &&
	      mc.acknowledgedOrderMessages().back().getContractId() == mom.getContractId() &&
	      mc.acknowledgedOrderMessages().back().getStratId() == mom.getStratId() &&
	      mc.acknowledgedOrderMessages().back().getOrderId() == mom.getOrderId() &&
	      mc.acknowledgedOrderMessages().back().getMessageId() == 0);
  //std::cout << SimulatorCore::convertTimePointToString(mc.getCurrTime()) << std::endl;
  mc.acknowledgeOrderMessage(mom_later);
  BOOST_CHECK(mc.marketOrderList().size() == 2 &&
	      mc.marketOrderList().back().getOrderId() == mom_later.getOrderId());
  BOOST_CHECK(mc.acknowledgedOrderMessages().size() == 2 &&
	      mc.acknowledgedOrderMessages().back().getExchangeId() == mom_later.getExchangeId() &&
	      mc.acknowledgedOrderMessages().back().getContractId() == mom_later.getContractId() &&
	      mc.acknowledgedOrderMessages().back().getStratId() == mom_later.getStratId() &&
	      mc.acknowledgedOrderMessages().back().getOrderId() == mom_later.getOrderId() &&
	      mc.acknowledgedOrderMessages().back().getMessageId() == 1);
}

// 3. CancelOrderMessage<LimitOrderMessage>: check cancelLimitOrderMessages_ and acknowledgedOrderMessages_
BOOST_AUTO_TEST_CASE(matchy_acknowledge_cancellimitordermessages)
{
  SimulatorCore sim("2012-08-15T09:15:00.0", "2012-08-15T15:15:00.0", 500, "millisecond");
  MatchyTest mc("CFFEX", "IF13", &sim);
  BOOST_CHECK(true);
  CancelOrderMessage<LimitOrderMessage> clom("CFFEX", "IF13", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"),
					     1, 101, 15, 10, BuySellSide::buy, Px(100.0, 0.01));
  CancelOrderMessage<LimitOrderMessage> clom2("CFFEX", "IF13", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:01.250"),
						   1, 102, 16, 30, BuySellSide::sell, Px(101.0, 0.01));
  mc.acknowledgeOrderMessage(clom);
  BOOST_CHECK(mc.cancelLimitList().size() == 1 &&
	      mc.cancelLimitList().back().getOrderId() == clom.getOrderId());
  BOOST_CHECK(mc.acknowledgedOrderMessages().size() == 1 &&
	      mc.acknowledgedOrderMessages().back().getExchangeId() == clom.getExchangeId() &&
	      mc.acknowledgedOrderMessages().back().getContractId() == clom.getContractId() &&
	      mc.acknowledgedOrderMessages().back().getStratId() == clom.getStratId() &&
	      mc.acknowledgedOrderMessages().back().getOrderId() == clom.getOrderId() &&
	      mc.acknowledgedOrderMessages().back().getMessageId() == 0);
  //std::cout << SimulatorCore::convertTimePointToString(mc.getCurrTime()) << std::endl;
  mc.acknowledgeOrderMessage(clom2);
  BOOST_CHECK(mc.cancelLimitList().size() == 2 &&
	      mc.cancelLimitList().back().getOrderId() == clom2.getOrderId());
  BOOST_CHECK(mc.acknowledgedOrderMessages().size() == 2 &&
	      mc.acknowledgedOrderMessages().back().getExchangeId() == clom2.getExchangeId() &&
	      mc.acknowledgedOrderMessages().back().getContractId() == clom2.getContractId() &&
	      mc.acknowledgedOrderMessages().back().getStratId() == clom2.getStratId() &&
	      mc.acknowledgedOrderMessages().back().getOrderId() == clom2.getOrderId() &&
	      mc.acknowledgedOrderMessages().back().getMessageId() == 1);
}

// 4. CancelOrderMessage<MarketOrderMessage>: check cancelMarketOrderMessages_ and acknowledgedOrderMessages_
BOOST_AUTO_TEST_CASE(matchy_acknowledge_cancelmarketordermessages)
{
  SimulatorCore sim("2012-08-15T09:15:00.0", "2012-08-15T15:15:00.0", 500, "millisecond");
  MatchyTest mc("CFFEX", "IF13", &sim);
  BOOST_CHECK(true);
  CancelOrderMessage<MarketOrderMessage> cmom("CFFEX", "IF13", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"),
					      1, 101, 15, 10, BuySellSide::buy);
  CancelOrderMessage<MarketOrderMessage> cmom2("CFFEX", "IF13", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:01.250"),
					       1, 102, 16, 30, BuySellSide::sell);
  mc.acknowledgeOrderMessage(cmom);
  BOOST_CHECK(mc.cancelMarketList().size() == 1 &&
	      mc.cancelMarketList().back().getOrderId() == cmom.getOrderId());
  BOOST_CHECK(mc.acknowledgedOrderMessages().size() == 1 &&
	      mc.acknowledgedOrderMessages().back().getExchangeId() == cmom.getExchangeId() &&
	      mc.acknowledgedOrderMessages().back().getContractId() == cmom.getContractId() &&
	      mc.acknowledgedOrderMessages().back().getStratId() == cmom.getStratId() &&
	      mc.acknowledgedOrderMessages().back().getOrderId() == cmom.getOrderId() &&
	      mc.acknowledgedOrderMessages().back().getMessageId() == 0);
  //std::cout << SimulatorCore::convertTimePointToString(mc.getCurrTime()) << std::endl;
  mc.acknowledgeOrderMessage(cmom2);
  BOOST_CHECK(mc.cancelMarketList().size() == 2 &&
	      mc.cancelMarketList().back().getOrderId() == cmom2.getOrderId());
  BOOST_CHECK(mc.acknowledgedOrderMessages().size() == 2 &&
	      mc.acknowledgedOrderMessages().back().getExchangeId() == cmom2.getExchangeId() &&
	      mc.acknowledgedOrderMessages().back().getContractId() == cmom2.getContractId() &&
	      mc.acknowledgedOrderMessages().back().getStratId() == cmom2.getStratId() &&
	      mc.acknowledgedOrderMessages().back().getOrderId() == cmom2.getOrderId() &&
	      mc.acknowledgedOrderMessages().back().getMessageId() == 1);
}

// 5. Register simulatorCorePtr_, check acknowledgeOrderMessages() method process correctly limit orders, market orders, cancel orders from simulatorCorePtr_'s outbound messages
// WARNING:
// this test code assumes that acknowledgeOrderMessages processes in the following sequence:
//     1. limitorder
//     2. marketorder
//     3. cancellimitorder
//     4. cancelmarketorder
// TODO:
// maybe make this not dependent on this assumption
BOOST_AUTO_TEST_CASE(matchy_acknowledgeordermessages)
{
  SimulatorCore sim("2012-08-15T09:15:00.0", "2012-08-15T15:15:00.0", 500, "millisecond");
  MatchyTest mc("CFFEX", "IF13", &sim);
  BOOST_CHECK(true);

  int nlimit = 10, nmarket = 5, nclimit = 10, ncmarket=5;
  int xnlimit= 3, xnmarket = 7, xnclimit=19, xncmarket=13;
  int nlimit_base = 1000, nmarket_base = 2000, nclimit_base=3000, ncmarket_base=4000;
  int xnlimit_base=5000, xnmarket_base = 6000, xnclimit_base=7000, xncmarket_base=8000;
  
  for(int ii = 0; ii < nlimit; ii++) {
    LimitOrderMessage lom("CFFEX", "IF13", 
			  SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") + duration_nano(1000000000ULL * ii),
			  1, nlimit_base+ii, nlimit_base+ii, 10+ii, BuySellSide::buy, Px(100.1, 0.02));
    sim.limitOrderMessages().push_back(lom);
  }
  for(int ii = 0; ii < nmarket; ii++) {
    MarketOrderMessage mom("CFFEX", "IF13", 
			   SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") + duration_nano(1000000000ULL * ii),
			   1, nmarket_base+ii, nmarket_base+ii, 10+ii, BuySellSide::sell);
    sim.marketOrderMessages().push_back(mom);
  }    
  for(int ii = 0; ii < nclimit; ii++) {
    CancelOrderMessage<LimitOrderMessage> clom("CFFEX", "IF13", 
					       SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") + duration_nano(1000000000ULL * ii),
					       1, nclimit_base+ii, nclimit_base+ii, 10+ii, BuySellSide::buy, Px(100.1, 0.02));
    sim.cancelLimitOrderMessages().push_back(clom);
  }
  for(int ii = 0; ii < ncmarket; ii++) {
    CancelOrderMessage<MarketOrderMessage> cmom("CFFEX", "IF13", 
						SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") + duration_nano(1000000000ULL * ii),
						1, ncmarket_base+ii, ncmarket_base+ii, 10+ii, BuySellSide::sell);
    sim.cancelMarketOrderMessages().push_back(cmom);
  }    
  for(int ii = 0; ii < xnlimit; ii++) {
    LimitOrderMessage lom("CFFEX", "IF12", 
			  SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") + duration_nano(1000000000ULL * ii),
			  1, xnlimit_base+ii, xnlimit_base+ii, 10+ii, BuySellSide::buy, Px(100.1, 0.02));
    sim.limitOrderMessages().push_back(lom);
  }
  for(int ii = 0; ii < xnmarket; ii++) {
    MarketOrderMessage mom("CFFEX3", "IF13", 
			   SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") + duration_nano(1000000000ULL * ii),
			   1, xnmarket_base+ii, xnmarket_base+ii, 10+ii, BuySellSide::sell);
    sim.marketOrderMessages().push_back(mom);
  }    
  for(int ii = 0; ii < xnclimit; ii++) {
    CancelOrderMessage<LimitOrderMessage> clom("CFFEX", "IF19", 
					       SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") + duration_nano(1000000000ULL * ii),
					       1, xnclimit_base+ii, xnclimit_base+ii, 10+ii, BuySellSide::buy, Px(100.1, 0.02));
    sim.cancelLimitOrderMessages().push_back(clom);
  }
  for(int ii = 0; ii < xncmarket; ii++) {
    CancelOrderMessage<MarketOrderMessage> cmom("CFFEX4", "IF13", 
						SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") + duration_nano(1000000000ULL * ii),
						1, xncmarket_base+ii, xncmarket_base+ii, 10+ii, BuySellSide::sell);
    sim.cancelMarketOrderMessages().push_back(cmom);
  }    
  
  mc.acknowledgeOrderMessages();

  //check copied messages
  BOOST_CHECK(mc.limitOrderList().size() == static_cast<unsigned>(nlimit) &&
	      mc.marketOrderList().size() == static_cast<unsigned>(nmarket) &&
	      mc.cancelLimitList().size() == static_cast<unsigned>(nclimit) &&
	      mc.cancelMarketList().size() == static_cast<unsigned>(ncmarket) &&
	      mc.acknowledgedOrderMessages().size() == static_cast<unsigned>(nlimit + nmarket + nclimit + ncmarket));
  int cnt = 0;
  for(int ii = 0; ii < nlimit; ii++) {
    LimitOrderMessage lom("CFFEX", "IF13", 
			  SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") + duration_nano(1000000000ULL * ii),
			  1, nlimit_base+ii, nlimit_base+ii, 10+ii, BuySellSide::buy, Px(100.1, 0.02));
    BOOST_CHECK(mc.limitOrderList().front().getMessageId() == lom.getMessageId() &&
		mc.limitOrderList().front().getOrderId() == lom.getOrderId());
    mc.limitOrderList().pop_front();
    BOOST_CHECK(mc.acknowledgedOrderMessages().front().getOrderId() == lom.getOrderId() &&
		mc.acknowledgedOrderMessages().front().getMessageId() == cnt);
    cnt++;
    mc.acknowledgedOrderMessages().pop_front();
  }
  for(int ii = 0; ii < nmarket; ii++) {
    MarketOrderMessage mom("CFFEX", "IF13", 
			   SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") + duration_nano(1000000000ULL * ii),
			   1, nmarket_base+ii, nmarket_base+ii, 10+ii, BuySellSide::sell);
    BOOST_CHECK(mc.marketOrderList().front().getMessageId() == mom.getMessageId() &&
		mc.marketOrderList().front().getOrderId() == mom.getOrderId());
    mc.marketOrderList().pop_front();
    BOOST_CHECK(mc.acknowledgedOrderMessages().front().getOrderId() == mom.getOrderId() &&
		mc.acknowledgedOrderMessages().front().getMessageId() == cnt);
    cnt++;
    mc.acknowledgedOrderMessages().pop_front();
  }    
  for(int ii = 0; ii < nclimit; ii++) {
    CancelOrderMessage<LimitOrderMessage> clom("CFFEX", "IF13", 
					       SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") + duration_nano(1000000000ULL * ii),
					       1, nclimit_base+ii, nclimit_base+ii, 10+ii, BuySellSide::buy, Px(100.1, 0.02));
    BOOST_CHECK(mc.cancelLimitList().front().getMessageId() == clom.getMessageId() &&
		mc.cancelLimitList().front().getOrderId() == clom.getOrderId());
    mc.cancelLimitList().pop_front();
    BOOST_CHECK(mc.acknowledgedOrderMessages().front().getOrderId() == clom.getOrderId() &&
		mc.acknowledgedOrderMessages().front().getMessageId() == cnt);
    cnt++;
    mc.acknowledgedOrderMessages().pop_front();
  }
  for(int ii = 0; ii < ncmarket; ii++) {
    CancelOrderMessage<MarketOrderMessage> cmom("CFFEX", "IF13", 
						SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") + duration_nano(1000000000ULL * ii),
						1, ncmarket_base+ii, ncmarket_base+ii, 10+ii, BuySellSide::sell);
    BOOST_CHECK(mc.cancelMarketList().front().getMessageId() == cmom.getMessageId() &&
		mc.cancelMarketList().front().getOrderId() == cmom.getOrderId());
    mc.cancelMarketList().pop_front();
    BOOST_CHECK(mc.acknowledgedOrderMessages().front().getOrderId() == cmom.getOrderId() &&
		mc.acknowledgedOrderMessages().front().getMessageId() == cnt);
    cnt++;
    mc.acknowledgedOrderMessages().pop_front();
  }    

  // check left over messages
  BOOST_CHECK(sim.limitOrderMessages().size() == static_cast<unsigned>(xnlimit) &&
	      sim.marketOrderMessages().size() == static_cast<unsigned>(xnmarket) &&
	      sim.cancelLimitOrderMessages().size() == static_cast<unsigned>(xnclimit) &&
	      sim.cancelMarketOrderMessages().size() == static_cast<unsigned>(xncmarket));

}
