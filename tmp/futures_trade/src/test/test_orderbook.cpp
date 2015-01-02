//
// File:        $ test_orderbook.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/01/13 13:31:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2012 by Zhengzhang Futures Co. Ltd.

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE unit_test_orderbook
#include <boost/test/unit_test.hpp>

#include <cstdlib>
#include <time.h>
#include <algorithm>
#include <vector>

#include "../OrderBook.hpp"
#include "../Message.hpp"
#include "../Px.hpp"
#include "../SimulatorCore.hpp"

using simulator::OrderBook;
using simulator::BuySellSide;
using simulator::LimitOrderMessage;
using simulator::MarketOrderMessage;
using simulator::CancelOrderMessage;
using simulator::FilledOrderMessage;
using simulator::Px;
using simulator::SimulatorCore;
using simulator::time_stamp;
using simulator::duration_nano;

// according to file unit.test.to.be.written.orderbook
// Mostly validate the *Helper methods and count* methods.

// 1. Check copy constructor and assignment operator.
BOOST_AUTO_TEST_CASE(orderbook_copy_constructor_assignment_operator)
{
  OrderBook ob;
  OrderBook ob1(ob);
  OrderBook ob3 = ob;
}

BOOST_AUTO_TEST_CASE(orderbook_operations)
{
  // 2. Create OrderBook class with default constructor,
  OrderBook ob;
  //   (a) add a buy limit order, check bidLimitOrderBook_
  LimitOrderMessage blom("CFFEX", "IF13", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"),
			1, 101, 15, 10, BuySellSide::buy, Px(100.1, 0.02));
  ob.limitOrderHelper(&blom);
  BOOST_CHECK(ob.getBidLimitOrderBook().size() == 1);
  BOOST_CHECK(ob.getBidLimitOrderBook()[0].size() == 1);
  BOOST_CHECK(ob.getBidLimitOrderBook()[0][0].getPx() == blom.getPx() &&
	      ob.getBidLimitOrderBook()[0][0].getQty() == blom.getQty() &&
	      ob.getBidLimitOrderBook()[0][0].getExchangeId() == blom.getExchangeId() &&
	      ob.getBidLimitOrderBook()[0][0].getContractId() == blom.getContractId() &&
	      ob.getBidLimitOrderBook()[0][0].getTimestamp() == blom.getTimestamp() &&
	      ob.getBidLimitOrderBook()[0][0].getStratId() == blom.getStratId() &&
	      ob.getBidLimitOrderBook()[0][0].getMessageId() == blom.getMessageId() &&
	      ob.getBidLimitOrderBook()[0][0].getOrderId() == blom.getOrderId());
    
  //   (b) add a buy limit order with lower price than (a), check bidLimitOrderBook_
  LimitOrderMessage blom_lower("CFFEX", "IF13", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:01.250"),
			      1, 102, 16, 30, BuySellSide::buy, Px(100.0, 0.02));
  ob.limitOrderHelper(&blom_lower);
  BOOST_CHECK(ob.getBidLimitOrderBook().size() == 2);
  BOOST_CHECK(ob.getBidLimitOrderBook()[0].size() == 1);
  BOOST_CHECK(ob.getBidLimitOrderBook()[0][0].getPx() == blom.getPx() &&
	      ob.getBidLimitOrderBook()[0][0].getQty() == blom.getQty() &&
	      ob.getBidLimitOrderBook()[0][0].getExchangeId() == blom.getExchangeId() &&
	      ob.getBidLimitOrderBook()[0][0].getContractId() == blom.getContractId() &&
	      ob.getBidLimitOrderBook()[0][0].getTimestamp() == blom.getTimestamp() &&
	      ob.getBidLimitOrderBook()[0][0].getStratId() == blom.getStratId() &&
	      ob.getBidLimitOrderBook()[0][0].getMessageId() == blom.getMessageId() &&
	      ob.getBidLimitOrderBook()[0][0].getOrderId() == blom.getOrderId());
  BOOST_CHECK(ob.getBidLimitOrderBook()[1].size() == 1);
  BOOST_CHECK(ob.getBidLimitOrderBook()[1][0].getPx() == blom_lower.getPx() &&
	      ob.getBidLimitOrderBook()[1][0].getQty() == blom_lower.getQty() &&
	      ob.getBidLimitOrderBook()[1][0].getExchangeId() == blom_lower.getExchangeId() &&
	      ob.getBidLimitOrderBook()[1][0].getContractId() == blom_lower.getContractId() &&
	      ob.getBidLimitOrderBook()[1][0].getTimestamp() == blom_lower.getTimestamp() &&
	      ob.getBidLimitOrderBook()[1][0].getStratId() == blom_lower.getStratId() &&
	      ob.getBidLimitOrderBook()[1][0].getMessageId() == blom_lower.getMessageId() &&
	      ob.getBidLimitOrderBook()[1][0].getOrderId() == blom_lower.getOrderId());

  //   (c) add a buy limit order with high price than (a), check bidLimitOrderBook_    
  LimitOrderMessage blom_higher("CFFEX", "IF13", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:03.250"),
			       1, 105, 18, 8, BuySellSide::buy, Px(100.4, 0.02));
  ob.limitOrderHelper(&blom_higher);
  BOOST_CHECK(ob.getBidLimitOrderBook().size() == 3);
  BOOST_CHECK(ob.getBidLimitOrderBook()[0].size() == 1);
  BOOST_CHECK(ob.getBidLimitOrderBook()[0][0].getPx() == blom_higher.getPx() &&
	      ob.getBidLimitOrderBook()[0][0].getQty() == blom_higher.getQty() &&
	      ob.getBidLimitOrderBook()[0][0].getExchangeId() == blom_higher.getExchangeId() &&
	      ob.getBidLimitOrderBook()[0][0].getContractId() == blom_higher.getContractId() &&
	      ob.getBidLimitOrderBook()[0][0].getTimestamp() == blom_higher.getTimestamp() &&
	      ob.getBidLimitOrderBook()[0][0].getStratId() == blom_higher.getStratId() &&
	      ob.getBidLimitOrderBook()[0][0].getMessageId() == blom_higher.getMessageId() &&
	      ob.getBidLimitOrderBook()[0][0].getOrderId() == blom_higher.getOrderId());
  BOOST_CHECK(ob.getBidLimitOrderBook()[1].size() == 1);
  BOOST_CHECK(ob.getBidLimitOrderBook()[1][0].getPx() == blom.getPx() &&
	      ob.getBidLimitOrderBook()[1][0].getQty() == blom.getQty() &&
	      ob.getBidLimitOrderBook()[1][0].getExchangeId() == blom.getExchangeId() &&
	      ob.getBidLimitOrderBook()[1][0].getContractId() == blom.getContractId() &&
	      ob.getBidLimitOrderBook()[1][0].getTimestamp() == blom.getTimestamp() &&
	      ob.getBidLimitOrderBook()[1][0].getStratId() == blom.getStratId() &&
	      ob.getBidLimitOrderBook()[1][0].getMessageId() == blom.getMessageId() &&
	      ob.getBidLimitOrderBook()[1][0].getOrderId() == blom.getOrderId());
  BOOST_CHECK(ob.getBidLimitOrderBook()[2].size() == 1);
  BOOST_CHECK(ob.getBidLimitOrderBook()[2][0].getPx() == blom_lower.getPx() &&
	      ob.getBidLimitOrderBook()[2][0].getQty() == blom_lower.getQty() &&
	      ob.getBidLimitOrderBook()[2][0].getExchangeId() == blom_lower.getExchangeId() &&
	      ob.getBidLimitOrderBook()[2][0].getContractId() == blom_lower.getContractId() &&
	      ob.getBidLimitOrderBook()[2][0].getTimestamp() == blom_lower.getTimestamp() &&
	      ob.getBidLimitOrderBook()[2][0].getStratId() == blom_lower.getStratId() &&
	      ob.getBidLimitOrderBook()[2][0].getMessageId() == blom_lower.getMessageId() &&
	      ob.getBidLimitOrderBook()[2][0].getOrderId() == blom_lower.getOrderId());
  //   (d) add a buy limit order with the same price as (a) but different timestamp, check bidLimitOrderBook_
  LimitOrderMessage blom_later("CFFEX", "IF13", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:30.250"),
			      1, 110, 19, 35, BuySellSide::buy, Px(100.1, 0.02));
  ob.limitOrderHelper(&blom_later);
  BOOST_CHECK(ob.getBidLimitOrderBook().size() == 3);
  BOOST_CHECK(ob.getBidLimitOrderBook()[0].size() == 1);
  BOOST_CHECK(ob.getBidLimitOrderBook()[0][0].getPx() == blom_higher.getPx() &&
	      ob.getBidLimitOrderBook()[0][0].getQty() == blom_higher.getQty() &&
	      ob.getBidLimitOrderBook()[0][0].getExchangeId() == blom_higher.getExchangeId() &&
	      ob.getBidLimitOrderBook()[0][0].getContractId() == blom_higher.getContractId() &&
	      ob.getBidLimitOrderBook()[0][0].getTimestamp() == blom_higher.getTimestamp() &&
	      ob.getBidLimitOrderBook()[0][0].getStratId() == blom_higher.getStratId() &&
	      ob.getBidLimitOrderBook()[0][0].getMessageId() == blom_higher.getMessageId() &&
	      ob.getBidLimitOrderBook()[0][0].getOrderId() == blom_higher.getOrderId());
  BOOST_CHECK(ob.getBidLimitOrderBook()[1].size() == 2);

  BOOST_CHECK(ob.getBidLimitOrderBook()[1][0].getPx() == blom.getPx() &&
	      ob.getBidLimitOrderBook()[1][0].getQty() == blom.getQty() &&
	      ob.getBidLimitOrderBook()[1][0].getExchangeId() == blom.getExchangeId() &&
	      ob.getBidLimitOrderBook()[1][0].getContractId() == blom.getContractId() &&
	      ob.getBidLimitOrderBook()[1][0].getTimestamp() == blom.getTimestamp() &&
	      ob.getBidLimitOrderBook()[1][0].getStratId() == blom.getStratId() &&
	      ob.getBidLimitOrderBook()[1][0].getMessageId() == blom.getMessageId() &&
	      ob.getBidLimitOrderBook()[1][0].getOrderId() == blom.getOrderId());
  BOOST_CHECK(ob.getBidLimitOrderBook()[1][1].getPx() == blom_later.getPx() &&
	      ob.getBidLimitOrderBook()[1][1].getQty() == blom_later.getQty() &&
	      ob.getBidLimitOrderBook()[1][1].getExchangeId() == blom_later.getExchangeId() &&
	      ob.getBidLimitOrderBook()[1][1].getContractId() == blom_later.getContractId() &&
	      ob.getBidLimitOrderBook()[1][1].getTimestamp() == blom_later.getTimestamp() &&
	      ob.getBidLimitOrderBook()[1][1].getStratId() == blom_later.getStratId() &&
	      ob.getBidLimitOrderBook()[1][1].getMessageId() == blom_later.getMessageId() &&
	      ob.getBidLimitOrderBook()[1][1].getOrderId() == blom_later.getOrderId());
  BOOST_CHECK(ob.getBidLimitOrderBook()[2].size() == 1);
  BOOST_CHECK(ob.getBidLimitOrderBook()[2][0].getPx() == blom_lower.getPx() &&
	      ob.getBidLimitOrderBook()[2][0].getQty() == blom_lower.getQty() &&
	      ob.getBidLimitOrderBook()[2][0].getExchangeId() == blom_lower.getExchangeId() &&
	      ob.getBidLimitOrderBook()[2][0].getContractId() == blom_lower.getContractId() &&
	      ob.getBidLimitOrderBook()[2][0].getTimestamp() == blom_lower.getTimestamp() &&
	      ob.getBidLimitOrderBook()[2][0].getStratId() == blom_lower.getStratId() &&
	      ob.getBidLimitOrderBook()[2][0].getMessageId() == blom_lower.getMessageId() &&
	      ob.getBidLimitOrderBook()[2][0].getOrderId() == blom_lower.getOrderId());
  // 3. Check count method:
  //   (a) at stage 2.(d), count buy limit orders, count total qty of buy limit orders
  BOOST_CHECK(ob.countLimitOrders() == 4 &&
	      ob.countLimitOrders(BuySellSide::buy) == 4 &&
	      ob.countLimitOrders(BuySellSide::sell) == 0 &&
	      ob.countLimitOrders(BuySellSide::buy, blom.getPx()) == 2 &&
	      ob.countLimitOrders(BuySellSide::buy, blom_lower.getPx()) == 1 &&
	      ob.countLimitOrders(BuySellSide::buy, blom_higher.getPx()) == 1 &&
	      ob.countLimitOrders(blom.getPx()) == 2 &&
	      ob.countLimitOrders(blom_lower.getPx()) == 1 &&
	      ob.countLimitOrders(blom_higher.getPx()) == 1);
  BOOST_CHECK(ob.countLimitOrderQty() == blom.getQty() + blom_lower.getQty() + blom_higher.getQty() + blom_later.getQty() &&
	      ob.countLimitOrderQty(BuySellSide::buy) == blom.getQty() + blom_lower.getQty() + blom_higher.getQty() + blom_later.getQty() &&
	      ob.countLimitOrderQty(BuySellSide::sell) == 0 &&
	      ob.countLimitOrderQty(BuySellSide::buy, blom.getPx()) == blom.getQty() + blom_later.getQty() &&
	      ob.countLimitOrderQty(BuySellSide::buy, blom_lower.getPx()) == blom_lower.getQty() &&
	      ob.countLimitOrderQty(BuySellSide::buy, blom_higher.getPx()) == blom_higher.getQty() &&
	      ob.countLimitOrderQty(blom.getPx()) == blom.getQty() + blom_later.getQty() &&
	      ob.countLimitOrderQty(blom_lower.getPx()) == blom_lower.getQty() &&
	      ob.countLimitOrderQty(blom_higher.getPx()) == blom_higher.getQty());
    
  
  //   (e) repeat (a)-(d) and check askLimitOrderBook_
  //   (a) add a sell limit order, check askLimitOrderBook_
  LimitOrderMessage slom("CFFEX", "IF13", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:31.250"),
			 1, 121, 20, 6, BuySellSide::sell, Px(100.7, 0.02));
  ob.limitOrderHelper(&slom);
  BOOST_CHECK(ob.getAskLimitOrderBook().size() == 1);
  BOOST_CHECK(ob.getAskLimitOrderBook()[0].size() == 1);
  BOOST_CHECK(ob.getAskLimitOrderBook()[0][0].getPx() == slom.getPx() &&
	      ob.getAskLimitOrderBook()[0][0].getQty() == slom.getQty() &&
	      ob.getAskLimitOrderBook()[0][0].getExchangeId() == slom.getExchangeId() &&
	      ob.getAskLimitOrderBook()[0][0].getContractId() == slom.getContractId() &&
	      ob.getAskLimitOrderBook()[0][0].getTimestamp() == slom.getTimestamp() &&
	      ob.getAskLimitOrderBook()[0][0].getStratId() == slom.getStratId() &&
	      ob.getAskLimitOrderBook()[0][0].getMessageId() == slom.getMessageId() &&
	      ob.getAskLimitOrderBook()[0][0].getOrderId() == slom.getOrderId());
    
  //   (b) add a sell limit order with higher price than (a), check askLimitOrderBook_
  LimitOrderMessage slom_higher("CFFEX", "IF13", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:41.250"),
				1, 122, 21, 33, BuySellSide::sell, Px(100.8, 0.02));
  ob.limitOrderHelper(&slom_higher);
  BOOST_CHECK(ob.getAskLimitOrderBook().size() == 2);
  BOOST_CHECK(ob.getAskLimitOrderBook()[0].size() == 1);
  BOOST_CHECK(ob.getAskLimitOrderBook()[0][0].getPx() == slom.getPx() &&
	      ob.getAskLimitOrderBook()[0][0].getQty() == slom.getQty() &&
	      ob.getAskLimitOrderBook()[0][0].getExchangeId() == slom.getExchangeId() &&
	      ob.getAskLimitOrderBook()[0][0].getContractId() == slom.getContractId() &&
	      ob.getAskLimitOrderBook()[0][0].getTimestamp() == slom.getTimestamp() &&
	      ob.getAskLimitOrderBook()[0][0].getStratId() == slom.getStratId() &&
	      ob.getAskLimitOrderBook()[0][0].getMessageId() == slom.getMessageId() &&
	      ob.getAskLimitOrderBook()[0][0].getOrderId() == slom.getOrderId());
  BOOST_CHECK(ob.getAskLimitOrderBook()[1].size() == 1);
  BOOST_CHECK(ob.getAskLimitOrderBook()[1][0].getPx() == slom_higher.getPx() &&
	      ob.getAskLimitOrderBook()[1][0].getQty() == slom_higher.getQty() &&
	      ob.getAskLimitOrderBook()[1][0].getExchangeId() == slom_higher.getExchangeId() &&
	      ob.getAskLimitOrderBook()[1][0].getContractId() == slom_higher.getContractId() &&
	      ob.getAskLimitOrderBook()[1][0].getTimestamp() == slom_higher.getTimestamp() &&
	      ob.getAskLimitOrderBook()[1][0].getStratId() == slom_higher.getStratId() &&
	      ob.getAskLimitOrderBook()[1][0].getMessageId() == slom_higher.getMessageId() &&
	      ob.getAskLimitOrderBook()[1][0].getOrderId() == slom_higher.getOrderId());

  //   (c) add a sell limit order with lower price than (a), check askLimitOrderBook_    
  LimitOrderMessage slom_lower("CFFEX", "IF13", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:43.250"),
			       1, 125, 28, 9, BuySellSide::sell, Px(100.62, 0.02));
  ob.limitOrderHelper(&slom_lower);
  BOOST_CHECK(ob.getAskLimitOrderBook().size() == 3);
  BOOST_CHECK(ob.getAskLimitOrderBook()[0].size() == 1);
  BOOST_CHECK(ob.getAskLimitOrderBook()[0][0].getPx() == slom_lower.getPx() &&
	      ob.getAskLimitOrderBook()[0][0].getQty() == slom_lower.getQty() &&
	      ob.getAskLimitOrderBook()[0][0].getExchangeId() == slom_lower.getExchangeId() &&
	      ob.getAskLimitOrderBook()[0][0].getContractId() == slom_lower.getContractId() &&
	      ob.getAskLimitOrderBook()[0][0].getTimestamp() == slom_lower.getTimestamp() &&
	      ob.getAskLimitOrderBook()[0][0].getStratId() == slom_lower.getStratId() &&
	      ob.getAskLimitOrderBook()[0][0].getMessageId() == slom_lower.getMessageId() &&
	      ob.getAskLimitOrderBook()[0][0].getOrderId() == slom_lower.getOrderId());
  BOOST_CHECK(ob.getAskLimitOrderBook()[1].size() == 1);
  BOOST_CHECK(ob.getAskLimitOrderBook()[1][0].getPx() == slom.getPx() &&
	      ob.getAskLimitOrderBook()[1][0].getQty() == slom.getQty() &&
	      ob.getAskLimitOrderBook()[1][0].getExchangeId() == slom.getExchangeId() &&
	      ob.getAskLimitOrderBook()[1][0].getContractId() == slom.getContractId() &&
	      ob.getAskLimitOrderBook()[1][0].getTimestamp() == slom.getTimestamp() &&
	      ob.getAskLimitOrderBook()[1][0].getStratId() == slom.getStratId() &&
	      ob.getAskLimitOrderBook()[1][0].getMessageId() == slom.getMessageId() &&
	      ob.getAskLimitOrderBook()[1][0].getOrderId() == slom.getOrderId());
  BOOST_CHECK(ob.getAskLimitOrderBook()[2].size() == 1);
  BOOST_CHECK(ob.getAskLimitOrderBook()[2][0].getPx() == slom_higher.getPx() &&
	      ob.getAskLimitOrderBook()[2][0].getQty() == slom_higher.getQty() &&
	      ob.getAskLimitOrderBook()[2][0].getExchangeId() == slom_higher.getExchangeId() &&
	      ob.getAskLimitOrderBook()[2][0].getContractId() == slom_higher.getContractId() &&
	      ob.getAskLimitOrderBook()[2][0].getTimestamp() == slom_higher.getTimestamp() &&
	      ob.getAskLimitOrderBook()[2][0].getStratId() == slom_higher.getStratId() &&
	      ob.getAskLimitOrderBook()[2][0].getMessageId() == slom_higher.getMessageId() &&
	      ob.getAskLimitOrderBook()[2][0].getOrderId() == slom_higher.getOrderId());

  //   (d) add a sell limit order with the same price as (a) but different timestamp, check askLimitOrderBook_
  LimitOrderMessage slom_later("CFFEX", "IF13", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:50.250"),
			       1, 130, 17, 20, BuySellSide::sell, Px(100.7, 0.02));
  ob.limitOrderHelper(&slom_later);
  BOOST_CHECK(ob.getAskLimitOrderBook().size() == 3);
  BOOST_CHECK(ob.getAskLimitOrderBook()[0].size() == 1);
  BOOST_CHECK(ob.getAskLimitOrderBook()[0][0].getPx() == slom_lower.getPx() &&
	      ob.getAskLimitOrderBook()[0][0].getQty() == slom_lower.getQty() &&
	      ob.getAskLimitOrderBook()[0][0].getExchangeId() == slom_lower.getExchangeId() &&
	      ob.getAskLimitOrderBook()[0][0].getContractId() == slom_lower.getContractId() &&
	      ob.getAskLimitOrderBook()[0][0].getTimestamp() == slom_lower.getTimestamp() &&
	      ob.getAskLimitOrderBook()[0][0].getStratId() == slom_lower.getStratId() &&
	      ob.getAskLimitOrderBook()[0][0].getMessageId() == slom_lower.getMessageId() &&
	      ob.getAskLimitOrderBook()[0][0].getOrderId() == slom_lower.getOrderId());
  BOOST_CHECK(ob.getAskLimitOrderBook()[1].size() == 2);

  BOOST_CHECK(ob.getAskLimitOrderBook()[1][0].getPx() == slom.getPx() &&
	      ob.getAskLimitOrderBook()[1][0].getQty() == slom.getQty() &&
	      ob.getAskLimitOrderBook()[1][0].getExchangeId() == slom.getExchangeId() &&
	      ob.getAskLimitOrderBook()[1][0].getContractId() == slom.getContractId() &&
	      ob.getAskLimitOrderBook()[1][0].getTimestamp() == slom.getTimestamp() &&
	      ob.getAskLimitOrderBook()[1][0].getStratId() == slom.getStratId() &&
	      ob.getAskLimitOrderBook()[1][0].getMessageId() == slom.getMessageId() &&
	      ob.getAskLimitOrderBook()[1][0].getOrderId() == slom.getOrderId());
  BOOST_CHECK(ob.getAskLimitOrderBook()[1][1].getPx() == slom_later.getPx() &&
	      ob.getAskLimitOrderBook()[1][1].getQty() == slom_later.getQty() &&
	      ob.getAskLimitOrderBook()[1][1].getExchangeId() == slom_later.getExchangeId() &&
	      ob.getAskLimitOrderBook()[1][1].getContractId() == slom_later.getContractId() &&
	      ob.getAskLimitOrderBook()[1][1].getTimestamp() == slom_later.getTimestamp() &&
	      ob.getAskLimitOrderBook()[1][1].getStratId() == slom_later.getStratId() &&
	      ob.getAskLimitOrderBook()[1][1].getMessageId() == slom_later.getMessageId() &&
	      ob.getAskLimitOrderBook()[1][1].getOrderId() == slom_later.getOrderId());
  BOOST_CHECK(ob.getAskLimitOrderBook()[2].size() == 1);
  BOOST_CHECK(ob.getAskLimitOrderBook()[2][0].getPx() == slom_higher.getPx() &&
	      ob.getAskLimitOrderBook()[2][0].getQty() == slom_higher.getQty() &&
	      ob.getAskLimitOrderBook()[2][0].getExchangeId() == slom_higher.getExchangeId() &&
	      ob.getAskLimitOrderBook()[2][0].getContractId() == slom_higher.getContractId() &&
	      ob.getAskLimitOrderBook()[2][0].getTimestamp() == slom_higher.getTimestamp() &&
	      ob.getAskLimitOrderBook()[2][0].getStratId() == slom_higher.getStratId() &&
	      ob.getAskLimitOrderBook()[2][0].getMessageId() == slom_higher.getMessageId() &&
	      ob.getAskLimitOrderBook()[2][0].getOrderId() == slom_higher.getOrderId());
  // 3. Check count method:
  //   (b) at stage 2.(e), count sell limit orders, count total qty of sell limit orders
  BOOST_CHECK(ob.countLimitOrders() == 8 &&
	      ob.countLimitOrders(BuySellSide::buy) == 4 &&
	      ob.countLimitOrders(BuySellSide::sell) == 4 &&
	      ob.countLimitOrders(BuySellSide::sell, slom.getPx()) == 2 &&
	      ob.countLimitOrders(BuySellSide::sell, slom_lower.getPx()) == 1 &&
	      ob.countLimitOrders(BuySellSide::sell, slom_higher.getPx()) == 1 &&
	      ob.countLimitOrders(slom.getPx()) == 2 &&
	      ob.countLimitOrders(slom_lower.getPx()) == 1 &&
	      ob.countLimitOrders(slom_higher.getPx()) == 1);
  BOOST_CHECK(ob.countLimitOrderQty() == blom.getQty() + blom_lower.getQty() + blom_higher.getQty() + blom_later.getQty() + 
	      slom.getQty() + slom_lower.getQty() + slom_higher.getQty() + slom_later.getQty() &&
	      ob.countLimitOrderQty(BuySellSide::buy) == blom.getQty() + blom_lower.getQty() + blom_higher.getQty() + blom_later.getQty() &&
	      ob.countLimitOrderQty(BuySellSide::sell) == slom.getQty() + slom_lower.getQty() + slom_higher.getQty() + slom_later.getQty() &&
	      ob.countLimitOrderQty(BuySellSide::sell, slom.getPx()) == slom.getQty() + slom_later.getQty() &&
	      ob.countLimitOrderQty(BuySellSide::sell, slom_lower.getPx()) == slom_lower.getQty() &&
	      ob.countLimitOrderQty(BuySellSide::sell, slom_higher.getPx()) == slom_higher.getQty() &&
	      ob.countLimitOrderQty(slom.getPx()) == slom.getQty() + slom_later.getQty() &&
	      ob.countLimitOrderQty(slom_lower.getPx()) == slom_lower.getQty() &&
	      ob.countLimitOrderQty(slom_higher.getPx()) == slom_higher.getQty());

  
  //   (f) add a market order, check marketOrderBook_
  MarketOrderMessage mom("CFFEX", "IF13", SimulatorCore::convertStringToTimePoint("2012-08-15T09:15:00.250"),
			 1, 201, 65, 77, BuySellSide::buy);
  ob.marketOrderHelper(&mom);
  BOOST_CHECK(ob.getMarketOrderBook().size() == 1);
  BOOST_CHECK(ob.getMarketOrderBook()[0].getQty() == mom.getQty() &&
	      ob.getMarketOrderBook()[0].getExchangeId() == mom.getExchangeId() &&
	      ob.getMarketOrderBook()[0].getContractId() == mom.getContractId() &&
	      ob.getMarketOrderBook()[0].getTimestamp() == mom.getTimestamp() &&
	      ob.getMarketOrderBook()[0].getStratId() == mom.getStratId() &&
	      ob.getMarketOrderBook()[0].getMessageId() == mom.getMessageId() &&
	      ob.getMarketOrderBook()[0].getOrderId() == mom.getOrderId());
  
  //   (g) add a market order with different timestamp, check marketOrderBook_
  MarketOrderMessage mom_earlier("CFFEX", "IF13", SimulatorCore::convertStringToTimePoint("2012-08-15T09:15:00.200"),
			       1, 200, 64, 11, BuySellSide::buy);
  ob.marketOrderHelper(&mom_earlier);
  BOOST_CHECK(ob.getMarketOrderBook().size() == 2);
  BOOST_CHECK(ob.getMarketOrderBook()[0].getQty() == mom_earlier.getQty() &&
	      ob.getMarketOrderBook()[0].getExchangeId() == mom_earlier.getExchangeId() &&
	      ob.getMarketOrderBook()[0].getContractId() == mom_earlier.getContractId() &&
	      ob.getMarketOrderBook()[0].getTimestamp() == mom_earlier.getTimestamp() &&
	      ob.getMarketOrderBook()[0].getStratId() == mom_earlier.getStratId() &&
	      ob.getMarketOrderBook()[0].getMessageId() == mom_earlier.getMessageId() &&
	      ob.getMarketOrderBook()[0].getOrderId() == mom_earlier.getOrderId());
  BOOST_CHECK(ob.getMarketOrderBook()[1].getQty() == mom.getQty() &&
	      ob.getMarketOrderBook()[1].getExchangeId() == mom.getExchangeId() &&
	      ob.getMarketOrderBook()[1].getContractId() == mom.getContractId() &&
	      ob.getMarketOrderBook()[1].getTimestamp() == mom.getTimestamp() &&
	      ob.getMarketOrderBook()[1].getStratId() == mom.getStratId() &&
	      ob.getMarketOrderBook()[1].getMessageId() == mom.getMessageId() &&
	      ob.getMarketOrderBook()[1].getOrderId() == mom.getOrderId());

  //   (c) at stage 2.(g), count market orders, count total qty of market orders
  BOOST_CHECK(ob.countMarketOrders() == 2);
  BOOST_CHECK(ob.countMarketOrders(BuySellSide::buy) == 2 &&
	      ob.countMarketOrders(BuySellSide::sell) == 0);
  BOOST_CHECK(ob.countMarketOrderQty() == mom.getQty() + mom_earlier.getQty() &&
	      ob.countMarketOrderQty(BuySellSide::buy) == mom.getQty() + mom_earlier.getQty() &&
	      ob.countMarketOrderQty(BuySellSide::sell)== 0);

  //   (h) cancel a market order with different exchangeId_ or contractId_ or orderId_, check marketOrderBook_
  CancelOrderMessage<MarketOrderMessage> com_wrong1("WRONG", "IF13", 
						    SimulatorCore::convertStringToTimePoint("2012-08-15T09:15:00.200"),
						    1, 200, 64, 11, BuySellSide::buy);
  CancelOrderMessage<MarketOrderMessage> com_wrong2("CFFEX", "IF15", 
						    SimulatorCore::convertStringToTimePoint("2012-08-15T09:15:00.200"),
						    1, 200, 64, 11, BuySellSide::buy);
  CancelOrderMessage<MarketOrderMessage> com_wrong3("CFFEX", "IF13", 
						    SimulatorCore::convertStringToTimePoint("2012-08-15T09:15:00.200"),
						    1, 200, 60, 11, BuySellSide::buy);
  BOOST_CHECK(!ob.cancelOrderHelper(&com_wrong1));
  BOOST_CHECK(!ob.cancelOrderHelper(&com_wrong2));
  BOOST_CHECK(!ob.cancelOrderHelper(&com_wrong2));
  BOOST_CHECK(ob.getMarketOrderBook().size() == 2);
  BOOST_CHECK(ob.getMarketOrderBook()[0].getQty() == mom_earlier.getQty() &&
	      ob.getMarketOrderBook()[0].getExchangeId() == mom_earlier.getExchangeId() &&
	      ob.getMarketOrderBook()[0].getContractId() == mom_earlier.getContractId() &&
	      ob.getMarketOrderBook()[0].getTimestamp() == mom_earlier.getTimestamp() &&
	      ob.getMarketOrderBook()[0].getStratId() == mom_earlier.getStratId() &&
	      ob.getMarketOrderBook()[0].getMessageId() == mom_earlier.getMessageId() &&
	      ob.getMarketOrderBook()[0].getOrderId() == mom_earlier.getOrderId());
  BOOST_CHECK(ob.getMarketOrderBook()[1].getQty() == mom.getQty() &&
	      ob.getMarketOrderBook()[1].getExchangeId() == mom.getExchangeId() &&
	      ob.getMarketOrderBook()[1].getContractId() == mom.getContractId() &&
	      ob.getMarketOrderBook()[1].getTimestamp() == mom.getTimestamp() &&
	      ob.getMarketOrderBook()[1].getStratId() == mom.getStratId() &&
	      ob.getMarketOrderBook()[1].getMessageId() == mom.getMessageId() &&
	      ob.getMarketOrderBook()[1].getOrderId() == mom.getOrderId());

  //   (i) cancel a market order with same exchangeId_, contractId_, orderId_, different timestamps, check marketOrderBook_
  CancelOrderMessage<MarketOrderMessage> com_right("CFFEX", "IF13", SimulatorCore::convertStringToTimePoint("2012-08-15T09:16:00.250"),
						   1, 251, 65, 77, BuySellSide::buy);
  BOOST_CHECK(ob.cancelOrderHelper(&com_right));
  BOOST_CHECK(ob.getMarketOrderBook().size() == 1);
  BOOST_CHECK(ob.getMarketOrderBook()[0].getQty() == mom_earlier.getQty() &&
	      ob.getMarketOrderBook()[0].getExchangeId() == mom_earlier.getExchangeId() &&
	      ob.getMarketOrderBook()[0].getContractId() == mom_earlier.getContractId() &&
	      ob.getMarketOrderBook()[0].getTimestamp() == mom_earlier.getTimestamp() &&
	      ob.getMarketOrderBook()[0].getStratId() == mom_earlier.getStratId() &&
	      ob.getMarketOrderBook()[0].getMessageId() == mom_earlier.getMessageId() &&
	      ob.getMarketOrderBook()[0].getOrderId() == mom_earlier.getOrderId());
  
    
  //   (j) cancel buy limit order for (a)-(d), check bidLimitOrderBook_
  CancelOrderMessage<LimitOrderMessage> col_blom(blom.getExchangeId(), blom.getContractId(),
						SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:00.250"),
						blom.getStratId(), 261, blom.getOrderId(), blom.getQty(), blom.getSide(),
						blom.getPx());
  BOOST_CHECK(ob.cancelOrderHelper(&col_blom));
  BOOST_CHECK(ob.getBidLimitOrderBook().size() == 3);
  BOOST_CHECK(ob.getBidLimitOrderBook()[0].size() == 1);
  BOOST_CHECK(ob.getBidLimitOrderBook()[0][0].getPx() == blom_higher.getPx() &&
	      ob.getBidLimitOrderBook()[0][0].getQty() == blom_higher.getQty() &&
	      ob.getBidLimitOrderBook()[0][0].getExchangeId() == blom_higher.getExchangeId() &&
	      ob.getBidLimitOrderBook()[0][0].getContractId() == blom_higher.getContractId() &&
	      ob.getBidLimitOrderBook()[0][0].getTimestamp() == blom_higher.getTimestamp() &&
	      ob.getBidLimitOrderBook()[0][0].getStratId() == blom_higher.getStratId() &&
	      ob.getBidLimitOrderBook()[0][0].getMessageId() == blom_higher.getMessageId() &&
	      ob.getBidLimitOrderBook()[0][0].getOrderId() == blom_higher.getOrderId());
  BOOST_CHECK(ob.getBidLimitOrderBook()[1].size() == 1);

  BOOST_CHECK(ob.getBidLimitOrderBook()[1][0].getPx() == blom_later.getPx() &&
	      ob.getBidLimitOrderBook()[1][0].getQty() == blom_later.getQty() &&
	      ob.getBidLimitOrderBook()[1][0].getExchangeId() == blom_later.getExchangeId() &&
	      ob.getBidLimitOrderBook()[1][0].getContractId() == blom_later.getContractId() &&
	      ob.getBidLimitOrderBook()[1][0].getTimestamp() == blom_later.getTimestamp() &&
	      ob.getBidLimitOrderBook()[1][0].getStratId() == blom_later.getStratId() &&
	      ob.getBidLimitOrderBook()[1][0].getMessageId() == blom_later.getMessageId() &&
	      ob.getBidLimitOrderBook()[1][0].getOrderId() == blom_later.getOrderId());
  BOOST_CHECK(ob.getBidLimitOrderBook()[2].size() == 1);
  BOOST_CHECK(ob.getBidLimitOrderBook()[2][0].getPx() == blom_lower.getPx() &&
	      ob.getBidLimitOrderBook()[2][0].getQty() == blom_lower.getQty() &&
	      ob.getBidLimitOrderBook()[2][0].getExchangeId() == blom_lower.getExchangeId() &&
	      ob.getBidLimitOrderBook()[2][0].getContractId() == blom_lower.getContractId() &&
	      ob.getBidLimitOrderBook()[2][0].getTimestamp() == blom_lower.getTimestamp() &&
	      ob.getBidLimitOrderBook()[2][0].getStratId() == blom_lower.getStratId() &&
	      ob.getBidLimitOrderBook()[2][0].getMessageId() == blom_lower.getMessageId() &&
	      ob.getBidLimitOrderBook()[2][0].getOrderId() == blom_lower.getOrderId());
  
  //   (k) cancel sell limit order for (e), check askLimitOrderBook_
  CancelOrderMessage<LimitOrderMessage> col_slom_higher(slom_higher.getExchangeId(), slom_higher.getContractId(),
							SimulatorCore::convertStringToTimePoint("2012-08-15T09:18:00.250"),
							slom_higher.getStratId(), 262, slom_higher.getOrderId(), slom_higher.getQty(), slom_higher.getSide(),
							slom_higher.getPx());
  CancelOrderMessage<LimitOrderMessage> col_slom_later(slom_later.getExchangeId(), slom_later.getContractId(),
						       SimulatorCore::convertStringToTimePoint("2012-08-15T09:19:00.250"),
						       slom_later.getStratId(), 263, slom_later.getOrderId(), slom_later.getQty(), slom_later.getSide(),
						       slom_later.getPx());
  BOOST_CHECK(ob.cancelOrderHelper(&col_slom_higher));
  BOOST_CHECK(ob.cancelOrderHelper(&col_slom_later));
  BOOST_CHECK(ob.getAskLimitOrderBook().size() == 2);
  BOOST_CHECK(ob.getAskLimitOrderBook()[0].size() == 1);
  BOOST_CHECK(ob.getAskLimitOrderBook()[0][0].getPx() == slom_lower.getPx() &&
	      ob.getAskLimitOrderBook()[0][0].getQty() == slom_lower.getQty() &&
	      ob.getAskLimitOrderBook()[0][0].getExchangeId() == slom_lower.getExchangeId() &&
	      ob.getAskLimitOrderBook()[0][0].getContractId() == slom_lower.getContractId() &&
	      ob.getAskLimitOrderBook()[0][0].getTimestamp() == slom_lower.getTimestamp() &&
	      ob.getAskLimitOrderBook()[0][0].getStratId() == slom_lower.getStratId() &&
	      ob.getAskLimitOrderBook()[0][0].getMessageId() == slom_lower.getMessageId() &&
	      ob.getAskLimitOrderBook()[0][0].getOrderId() == slom_lower.getOrderId());
  BOOST_CHECK(ob.getAskLimitOrderBook()[1].size() == 1);

  BOOST_CHECK(ob.getAskLimitOrderBook()[1][0].getPx() == slom.getPx() &&
	      ob.getAskLimitOrderBook()[1][0].getQty() == slom.getQty() &&
	      ob.getAskLimitOrderBook()[1][0].getExchangeId() == slom.getExchangeId() &&
	      ob.getAskLimitOrderBook()[1][0].getContractId() == slom.getContractId() &&
	      ob.getAskLimitOrderBook()[1][0].getTimestamp() == slom.getTimestamp() &&
	      ob.getAskLimitOrderBook()[1][0].getStratId() == slom.getStratId() &&
	      ob.getAskLimitOrderBook()[1][0].getMessageId() == slom.getMessageId() &&
	      ob.getAskLimitOrderBook()[1][0].getOrderId() == slom.getOrderId());

  //   (l) update on FilledOrderMessage for MarketOrderMessage (partial fill & complete fill), check marketOrderBook_
  FilledOrderMessage fom_partial("CFFEX", "IF13", SimulatorCore::convertStringToTimePoint("2012-08-15T09:35:00.200"),
			       1, 400, 64, 3, BuySellSide::buy, Px(103.1, 0.02));
  ob.filledOrderHelper(&fom_partial);
  BOOST_CHECK(ob.getMarketOrderBook().size() == 1);
  BOOST_CHECK(ob.getMarketOrderBook()[0].getQty() == mom_earlier.getQty() - fom_partial.getQty() &&
	      ob.getMarketOrderBook()[0].getExchangeId() == mom_earlier.getExchangeId() &&
	      ob.getMarketOrderBook()[0].getContractId() == mom_earlier.getContractId() &&
	      ob.getMarketOrderBook()[0].getTimestamp() == mom_earlier.getTimestamp() &&
	      ob.getMarketOrderBook()[0].getStratId() == mom_earlier.getStratId() &&
	      ob.getMarketOrderBook()[0].getMessageId() == mom_earlier.getMessageId() &&
	      ob.getMarketOrderBook()[0].getOrderId() == mom_earlier.getOrderId());

  FilledOrderMessage fom_full("CFFEX", "IF13", SimulatorCore::convertStringToTimePoint("2012-08-15T09:36:00.200"),
			    1, 401, 64, mom_earlier.getQty() - fom_partial.getQty(), BuySellSide::buy, Px(103.2, 0.02));
  ob.filledOrderHelper(&fom_full);
  BOOST_CHECK(ob.getMarketOrderBook().size() == 0);
  
  //   (m) update on FilledOrderMessage for buy LimitOrderMessage (partial fill & complete fill), check bidLimitOrderBook_
  FilledOrderMessage fblom_partial(blom_higher.getExchangeId(), blom_higher.getContractId(),
				   SimulatorCore::convertStringToTimePoint("2012-08-15T09:37:00.200"),
				   blom_higher.getStratId(), 410, blom_higher.getOrderId(),
				   1, blom_higher.getSide(), blom_higher.getPx());
  ob.filledOrderHelper(&fblom_partial);
  BOOST_CHECK(ob.getBidLimitOrderBook().size() == 3);
  BOOST_CHECK(ob.getBidLimitOrderBook()[0].size() == 1);
  BOOST_CHECK(ob.getBidLimitOrderBook()[0][0].getPx() == blom_higher.getPx() &&
	      ob.getBidLimitOrderBook()[0][0].getQty() == blom_higher.getQty() - fblom_partial.getQty() &&
	      ob.getBidLimitOrderBook()[0][0].getExchangeId() == blom_higher.getExchangeId() &&
	      ob.getBidLimitOrderBook()[0][0].getContractId() == blom_higher.getContractId() &&
	      ob.getBidLimitOrderBook()[0][0].getTimestamp() == blom_higher.getTimestamp() &&
	      ob.getBidLimitOrderBook()[0][0].getStratId() == blom_higher.getStratId() &&
	      ob.getBidLimitOrderBook()[0][0].getMessageId() == blom_higher.getMessageId() &&
	      ob.getBidLimitOrderBook()[0][0].getOrderId() == blom_higher.getOrderId());
  BOOST_CHECK(ob.getBidLimitOrderBook()[1].size() == 1);

  BOOST_CHECK(ob.getBidLimitOrderBook()[1][0].getPx() == blom_later.getPx() &&
	      ob.getBidLimitOrderBook()[1][0].getQty() == blom_later.getQty() &&
	      ob.getBidLimitOrderBook()[1][0].getExchangeId() == blom_later.getExchangeId() &&
	      ob.getBidLimitOrderBook()[1][0].getContractId() == blom_later.getContractId() &&
	      ob.getBidLimitOrderBook()[1][0].getTimestamp() == blom_later.getTimestamp() &&
	      ob.getBidLimitOrderBook()[1][0].getStratId() == blom_later.getStratId() &&
	      ob.getBidLimitOrderBook()[1][0].getMessageId() == blom_later.getMessageId() &&
	      ob.getBidLimitOrderBook()[1][0].getOrderId() == blom_later.getOrderId());
  BOOST_CHECK(ob.getBidLimitOrderBook()[2].size() == 1);
  BOOST_CHECK(ob.getBidLimitOrderBook()[2][0].getPx() == blom_lower.getPx() &&
	      ob.getBidLimitOrderBook()[2][0].getQty() == blom_lower.getQty() &&
	      ob.getBidLimitOrderBook()[2][0].getExchangeId() == blom_lower.getExchangeId() &&
	      ob.getBidLimitOrderBook()[2][0].getContractId() == blom_lower.getContractId() &&
	      ob.getBidLimitOrderBook()[2][0].getTimestamp() == blom_lower.getTimestamp() &&
	      ob.getBidLimitOrderBook()[2][0].getStratId() == blom_lower.getStratId() &&
	      ob.getBidLimitOrderBook()[2][0].getMessageId() == blom_lower.getMessageId() &&
	      ob.getBidLimitOrderBook()[2][0].getOrderId() == blom_lower.getOrderId());

  FilledOrderMessage fblom_full(blom_later.getExchangeId(), blom_later.getContractId(),
				SimulatorCore::convertStringToTimePoint("2012-08-15T09:38:00.200"),
				blom_later.getStratId(), 411, blom_later.getOrderId(),
				blom_later.getQty(), blom_later.getSide(), blom_later.getPx());
  ob.filledOrderHelper(&fblom_full);
  //std::cout << ob.getBidLimitOrderBook().size() << std::endl;
  BOOST_CHECK(ob.getBidLimitOrderBook().size() == 2);
  BOOST_CHECK(ob.getBidLimitOrderBook()[0].size() == 1);
  BOOST_CHECK(ob.getBidLimitOrderBook()[0][0].getPx() == blom_higher.getPx() &&
	      ob.getBidLimitOrderBook()[0][0].getQty() == blom_higher.getQty() - fblom_partial.getQty() &&
	      ob.getBidLimitOrderBook()[0][0].getExchangeId() == blom_higher.getExchangeId() &&
	      ob.getBidLimitOrderBook()[0][0].getContractId() == blom_higher.getContractId() &&
	      ob.getBidLimitOrderBook()[0][0].getTimestamp() == blom_higher.getTimestamp() &&
	      ob.getBidLimitOrderBook()[0][0].getStratId() == blom_higher.getStratId() &&
	      ob.getBidLimitOrderBook()[0][0].getMessageId() == blom_higher.getMessageId() &&
	      ob.getBidLimitOrderBook()[0][0].getOrderId() == blom_higher.getOrderId());
  //std::cout << ob.getBidLimitOrderBook()[1].size() << std::endl;
  BOOST_CHECK(ob.getBidLimitOrderBook()[1].size() == 1);

  BOOST_CHECK(ob.getBidLimitOrderBook()[1][0].getPx() == blom_lower.getPx() &&
	      ob.getBidLimitOrderBook()[1][0].getQty() == blom_lower.getQty() &&
	      ob.getBidLimitOrderBook()[1][0].getExchangeId() == blom_lower.getExchangeId() &&
	      ob.getBidLimitOrderBook()[1][0].getContractId() == blom_lower.getContractId() &&
	      ob.getBidLimitOrderBook()[1][0].getTimestamp() == blom_lower.getTimestamp() &&
	      ob.getBidLimitOrderBook()[1][0].getStratId() == blom_lower.getStratId() &&
	      ob.getBidLimitOrderBook()[1][0].getMessageId() == blom_lower.getMessageId() &&
	      ob.getBidLimitOrderBook()[1][0].getOrderId() == blom_lower.getOrderId());

  //   (n) update on FilledOrderMessage for sell LimitOrderMessage (partial fill & complete fill), check askLimitOrderBook_
  FilledOrderMessage fslom_partial(slom_lower.getExchangeId(), slom_lower.getContractId(),
				   SimulatorCore::convertStringToTimePoint("2012-08-15T09:48:00.250"),
				   slom_lower.getStratId(), 450, slom_lower.getOrderId(), 
				   2, slom_lower.getSide(),
				   slom_lower.getPx());
  ob.filledOrderHelper(&fslom_partial);
  BOOST_CHECK(ob.getAskLimitOrderBook().size() == 2);
  BOOST_CHECK(ob.getAskLimitOrderBook()[0].size() == 1);
  BOOST_CHECK(ob.getAskLimitOrderBook()[0][0].getPx() == slom_lower.getPx() &&
	      ob.getAskLimitOrderBook()[0][0].getQty() == slom_lower.getQty()  - fslom_partial.getQty() &&
	      ob.getAskLimitOrderBook()[0][0].getExchangeId() == slom_lower.getExchangeId() &&
	      ob.getAskLimitOrderBook()[0][0].getContractId() == slom_lower.getContractId() &&
	      ob.getAskLimitOrderBook()[0][0].getTimestamp() == slom_lower.getTimestamp() &&
	      ob.getAskLimitOrderBook()[0][0].getStratId() == slom_lower.getStratId() &&
	      ob.getAskLimitOrderBook()[0][0].getMessageId() == slom_lower.getMessageId() &&
	      ob.getAskLimitOrderBook()[0][0].getOrderId() == slom_lower.getOrderId());
  BOOST_CHECK(ob.getAskLimitOrderBook()[1].size() == 1);

  BOOST_CHECK(ob.getAskLimitOrderBook()[1][0].getPx() == slom.getPx() &&
	      ob.getAskLimitOrderBook()[1][0].getQty() == slom.getQty() &&
	      ob.getAskLimitOrderBook()[1][0].getExchangeId() == slom.getExchangeId() &&
	      ob.getAskLimitOrderBook()[1][0].getContractId() == slom.getContractId() &&
	      ob.getAskLimitOrderBook()[1][0].getTimestamp() == slom.getTimestamp() &&
	      ob.getAskLimitOrderBook()[1][0].getStratId() == slom.getStratId() &&
	      ob.getAskLimitOrderBook()[1][0].getMessageId() == slom.getMessageId() &&
	      ob.getAskLimitOrderBook()[1][0].getOrderId() == slom.getOrderId());

  FilledOrderMessage fslom_full(slom_lower.getExchangeId(), slom_lower.getContractId(),
				SimulatorCore::convertStringToTimePoint("2012-08-15T09:49:00.250"),
				slom_lower.getStratId(), 451, slom_lower.getOrderId(), 
				slom_lower.getQty() - fslom_partial.getQty(), slom_lower.getSide(),
				slom_lower.getPx());
  ob.filledOrderHelper(&fslom_full);
  BOOST_CHECK(ob.getAskLimitOrderBook().size() == 1);
  BOOST_CHECK(ob.getAskLimitOrderBook()[0].size() == 1);

  BOOST_CHECK(ob.getAskLimitOrderBook()[0][0].getPx() == slom.getPx() &&
	      ob.getAskLimitOrderBook()[0][0].getQty() == slom.getQty() &&
	      ob.getAskLimitOrderBook()[0][0].getExchangeId() == slom.getExchangeId() &&
	      ob.getAskLimitOrderBook()[0][0].getContractId() == slom.getContractId() &&
	      ob.getAskLimitOrderBook()[0][0].getTimestamp() == slom.getTimestamp() &&
	      ob.getAskLimitOrderBook()[0][0].getStratId() == slom.getStratId() &&
	      ob.getAskLimitOrderBook()[0][0].getMessageId() == slom.getMessageId() &&
	      ob.getAskLimitOrderBook()[0][0].getOrderId() == slom.getOrderId());

  // 3. Check count method:
  //   (d) at stage 2.(n), count everything and make sure they are consistent with the cancel and update
  BOOST_CHECK(ob.countLimitOrders() == 3 &&
	      ob.countLimitOrders(BuySellSide::buy) == 2 &&
	      ob.countLimitOrders(BuySellSide::sell) == 1 &&
	      ob.countLimitOrders(BuySellSide::buy, blom_higher.getPx()) == 1 &&
	      ob.countLimitOrders(BuySellSide::buy, blom_lower.getPx()) == 1 &&
	      ob.countLimitOrders(BuySellSide::sell, slom.getPx()) == 1 &&
	      ob.countLimitOrders(blom_higher.getPx()) == 1 &&
	      ob.countLimitOrders(blom_lower.getPx()) == 1 &&
	      ob.countLimitOrders(slom.getPx()) == 1);
  BOOST_CHECK(ob.countLimitOrderQty() == blom_lower.getQty() + blom_higher.getQty() + slom.getQty() - fblom_partial.getQty()&&
	      ob.countLimitOrderQty(BuySellSide::buy) == blom_lower.getQty() + blom_higher.getQty() - fblom_partial.getQty() &&
	      ob.countLimitOrderQty(BuySellSide::sell) == slom.getQty() &&
	      ob.countLimitOrderQty(BuySellSide::sell, slom.getPx()) == slom.getQty() &&
	      ob.countLimitOrderQty(BuySellSide::buy, blom_lower.getPx()) == blom_lower.getQty() &&
	      ob.countLimitOrderQty(BuySellSide::buy, blom_higher.getPx()) == blom_higher.getQty() - fblom_partial.getQty()&&
	      ob.countLimitOrderQty(slom.getPx()) == slom.getQty() &&
	      ob.countLimitOrderQty(blom_lower.getPx()) == blom_lower.getQty() &&
	      ob.countLimitOrderQty(blom_higher.getPx()) == blom_higher.getQty() - fblom_partial.getQty());
  BOOST_CHECK(ob.countMarketOrders() == 0);
  BOOST_CHECK(ob.countMarketOrders(BuySellSide::buy) == 0 &&
	      ob.countMarketOrders(BuySellSide::sell) == 0);
  BOOST_CHECK(ob.countMarketOrderQty() == 0 &&
	      ob.countMarketOrderQty(BuySellSide::buy) == 0 &&
	      ob.countMarketOrderQty(BuySellSide::sell)== 0);


}
