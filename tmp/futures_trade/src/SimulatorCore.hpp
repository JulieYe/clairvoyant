//
// File:        $ SimulatorCore.hpp $
//
// Version:     $ Revision: 1.1 $
//
// Modified:    $ Date: 2014/01/25 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2014 by Zhengzhang Futures Co. Ltd.

#ifndef SIMULATORCORE_HPP
#define SIMULATORCORE_HPP

#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cstring>
#include <cmath>
#include <cctype>

#include <string>
#include <list>
#include <vector>
#include <chrono>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <iostream>
#include <utility>
#include <mutex>

using std::string;
using std::chrono::time_point;
using std::chrono::system_clock;
using std::chrono::duration;
using std::nano;
using std::transform;
using std::find;
using std::function;
using std::extent;
using std::list;
using std::vector;
using std::cout;
using std::endl;
using std::for_each;
using std::move;
using std::mutex;
using std::lock_guard;

#include "Message.hpp"
#include "Matchy.hpp"
#include "Strategy.hpp"

/* necessary forward declaration */
namespace simulator
{
    class Strategy;
    class Matchy;
}

using simulator::Matchy;
using simulator::Strategy;
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
using simulator::Strategy;
using simulator::time_stamp;
using simulator::duration_nano;

namespace simulator
{

    class SimulatorCore
    {
    private:
	string startDateTime_;      // e.g., "2012-10-29T09:15:00.123456789"
	string stopDateTime_;       // e.g., "2012-10-29T15:15:00.123456789"
	duration_nano timeStep_;

#ifdef PRODUCTION
	duration_nano timeOffset_;  // Time offset to Matchy
	bool timeOffsetQ_ = false;
	timespec tp_;               // System-wide real-time clock
#endif

	time_stamp startTime_;
	time_stamp stopTime_;
	time_stamp currTime_;

	size_t unitIndex(string &);

	bool isAllMarketDataReady_;
	bool isAllTraderReady_;

	/* Exchange sides */
	list<Matchy *> exchanges_;

	int numExchanges_;
	list<TradeUpdate<true> > tradeUpdates_;
	list<BookUpdate<true> > bookUpdates_;
	list<AcknowledgedOrderMessage> acknowledgedOrderMessages_;
	list<FilledOrderMessage> filledOrderMessages_;
	list<CanceledOrderMessage<LimitOrderMessage> > canceledLimitOrderMessages_;
	list<CanceledOrderMessage<MarketOrderMessage> > canceledMarketOrderMessages_;
	list<RejectedOrderMessage<LimitOrderMessage> > rejectedLimitOrderMessages_;
	list<RejectedOrderMessage<MarketOrderMessage> > rejectedMarketOrderMessages_;
	list<RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage> > > rejectedCancelLimitOrderMessages_;
	list<RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage> > > rejectedCancelMarketOrderMessages_;
	void clearMatchyMessages();

	/* Strategy sides */
	list<Strategy *> strategies_;
	int numStrategies_;
#ifdef PRODUCTION
	vector<bool> stratUpdTrigger_;
#endif
	list<LimitOrderMessage> limitOrderMessages_;
        list<MarketOrderMessage> marketOrderMessages_;
	list<CancelOrderMessage<LimitOrderMessage> > cancelLimitOrderMessages_;
	list<CancelOrderMessage<MarketOrderMessage> > cancelMarketOrderMessages_;
	void clearStrategyMessages();

	// Append outbound messages from exchange sides to this.
	void getMessagesFromMatchy(Matchy *);

	// Append outbound messages from strategy sides to this.
	void getMessagesFromStrategy(Strategy *);

	int orderId_ = 0;

    public:
	SimulatorCore(string, string, int, string);

	void registerMatchy(Matchy * pmatchy);

	inline list<Matchy *> getExchanges() { return exchanges_; }

	int getNumExchanges() const;

        void registerStrategy(Strategy *strategy);
	int getNumStrategies() const;

	inline list<Strategy *> getStrategies() { return strategies_; }

	time_stamp getCurrTime();
	time_stamp getStartTime();
	time_stamp getStopTime();

	static time_stamp convertStringToTimePoint(string);
	static string convertTimePointToString(time_stamp);

	void increment();
	bool end() const;

	// If end(), do nothing and return false,
	// otherwise, process exchange sides, update messsages from exchange sides and clear messages from previous strategy sides
	// then process strategy sides, update messages from strategy sides and clear messages from previous exchange sides.
#ifdef SIMULATION
	bool next();
#endif

	// production next method
#ifdef PRODUCTION
	mutex lock_;
	void resetStratUpdTrigger();
	bool getStratUpdTrigger(int);
	void setStratUpdTrigger(int, bool);
	void next();
	duration_nano & timeOffset();
	bool & timeOffsetQ();
#endif

	inline list<LimitOrderMessage> & limitOrderMessages() { return limitOrderMessages_; }
	inline list<MarketOrderMessage> & marketOrderMessages() { return marketOrderMessages_; }
	inline list<CancelOrderMessage<LimitOrderMessage> > & cancelLimitOrderMessages() { return cancelLimitOrderMessages_; }
	inline list<CancelOrderMessage<MarketOrderMessage> > & cancelMarketOrderMessages() { return cancelMarketOrderMessages_; }

	// adding accessors for exchange initiated market data
	inline list<TradeUpdate<true> > & tradeUpdates() { return tradeUpdates_; }
	inline list<BookUpdate<true> > & bookUpdates() { return bookUpdates_; }

	// adding accessors for strategy initiated messages
	list<AcknowledgedOrderMessage> & acknowledgedOrderMessages();

        list<FilledOrderMessage> & filledOrderMessages();

	list<CanceledOrderMessage<LimitOrderMessage> > & canceledLimitOrderMessages();

	list<CanceledOrderMessage<MarketOrderMessage> > & canceledMarketOrderMessages();

	list<RejectedOrderMessage<LimitOrderMessage> > & rejectedLimitOrderMessages();

	list<RejectedOrderMessage<MarketOrderMessage> > & rejectedMarketOrderMessages();

	list<RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage> > > & rejectedCancelLimitOrderMessages();

	list<RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage> > > & rejectedCancelMarketOrderMessages();

        list<TradeUpdate<true> > getTradeUpdate() const;

	list<TradeUpdate<true> > & tradeUpdate();

        list<BookUpdate<true> > getBookUpdate() const;

	list<BookUpdate<true> > & bookUpdate();
	
	inline bool isAllMarketDataReady() const { return isAllMarketDataReady_; }
	inline bool isAllTraderReady() const { return isAllTraderReady_; }

	inline int & orderId() { return orderId_; }
    };

}

#endif    // SIMULATORCORE_HPP
