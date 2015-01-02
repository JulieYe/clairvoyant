//
// File:        $ Matchy.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2014/01/09 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2014 by Zhengzhang Futures Co. Ltd.

#ifndef MATCHY_HPP
#define MATCHY_HPP

#include <string>
#include <list>
#include <algorithm>
#include <fstream>
#include <cstdio>
#include <mutex>
#include <exception>
#include <utility>
#include <cstdlib>
#include <atomic>

using std::exception;
using std::move;
using std::atoi;
using std::string;
using std::list;
using std::sort;
using std::partition_copy;
using std::back_inserter;
using std::remove_if;
using std::ifstream;
using std::sscanf;
using std::for_each;
using std::mutex;
using std::lock_guard;
using std::atomic_long;
using std::min;

#include "Px.hpp"
#include "Message.hpp"
#include "OrderBook.hpp"
#include "SimulatorCore.hpp"

/* necessary forward declaration */
namespace simulator
{
    class SimulatorCore;
}

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
using simulator::OrderBook;
using simulator::SimulatorCore;
using simulator::time_stamp;
using simulator::duration_nano;

namespace simulator
{

    class Matchy
    {
    protected:
	string exchangeId_;
	string contractId_;
	atomic_long messageId_;
	time_stamp currTime_;

	/* linking to simulator core */
	SimulatorCore * simulatorCorePtr_;

	/* internal full limit order book */
	OrderBook orderBook_;

	/* inbound messages sent from Strategies */
	list<LimitOrderMessage> limitOrderMessages_;
	list<MarketOrderMessage> marketOrderMessages_;
	list<CancelOrderMessage<LimitOrderMessage> > cancelLimitOrderMessages_;
	list<CancelOrderMessage<MarketOrderMessage> > cancelMarketOrderMessages_;

	/* outbound order messages to send to Strategies */
	list<AcknowledgedOrderMessage> acknowledgedOrderMessages_;
	list<FilledOrderMessage> filledOrderMessages_;
	list<CanceledOrderMessage<LimitOrderMessage> > canceledLimitOrderMessages_;
	list<CanceledOrderMessage<MarketOrderMessage> > canceledMarketOrderMessages_;
	list<RejectedOrderMessage<LimitOrderMessage> > rejectedLimitOrderMessages_;
	list<RejectedOrderMessage<MarketOrderMessage> > rejectedMarketOrderMessages_;
	list<RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage> > > rejectedCancelLimitOrderMessages_;
	list<RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage> > > rejectedCancelMarketOrderMessages_;

	/* BookUpdate and TradeUpdate */
	list<TradeUpdate<true> > tradeUpdates_;
	list<BookUpdate<true> > bookUpdates_;

	bool isMarketDataReady_;
	bool isTraderReady_;
	int actionFlag_; 

    public:
	Matchy() = delete;

	Matchy(Matchy const &) = delete;

	Matchy & operator=(Matchy const &) = delete;

	Matchy(string, string, SimulatorCore *);

	virtual ~Matchy();

	time_stamp getCurrTime() const;
	time_stamp & currTime();

	void clearAllMessages();

	void acknowledgeOrderMessage(LimitOrderMessage const);
	void acknowledgeOrderMessage(MarketOrderMessage const);
	void acknowledgeOrderMessage(CancelOrderMessage<LimitOrderMessage> const);
	void acknowledgeOrderMessage(CancelOrderMessage<MarketOrderMessage> const);
	void acknowledgeOrderMessages();

	list<AcknowledgedOrderMessage> & acknowledgedOrderMessages();
	list<FilledOrderMessage> & filledOrderMessages();
	list<CanceledOrderMessage<LimitOrderMessage> > & canceledLimitOrderMessages();
	list<CanceledOrderMessage<MarketOrderMessage> > & canceledMarketOrderMessages();
	list<RejectedOrderMessage<LimitOrderMessage> > & rejectedLimitOrderMessages();
	list<RejectedOrderMessage<MarketOrderMessage> > & rejectedMarketOrderMessages();
	list<RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage> > > & rejectedCancelLimitOrderMessages();
	list<RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage> > > & rejectedCancelMarketOrderMessages();

	list<TradeUpdate<true> > & tradeUpdates();
	list<BookUpdate<true> > & bookUpdates();

	list<LimitOrderMessage> & limitOrderMessages();
	list<MarketOrderMessage> & marketOrderMessages();
	list<CancelOrderMessage<LimitOrderMessage> > & cancelLimitOrderMessages();
	list<CancelOrderMessage<MarketOrderMessage> > & cancelMarketOrderMessages();

	bool & marketDataReadiness() {return isMarketDataReady_;}
	bool & traderReadiness() {return isTraderReady_;}
	int & actionFlag() {return actionFlag_;}

	virtual bool processLimitOrder(LimitOrderMessage &);
	virtual bool processMarketOrder(MarketOrderMessage &);
	virtual bool processCancelLimitOrder(CancelOrderMessage<LimitOrderMessage> &);
	virtual bool processCancelMarketOrder(CancelOrderMessage<MarketOrderMessage> &);
	virtual void setOrderId(int);
	virtual void process(int);

	inline SimulatorCore * getSimulatorCorePtr() { return simulatorCorePtr_; }
	
	inline string getContractId() const { return contractId_; }
	inline string getExchangeId() const { return exchangeId_; }
	inline string getTicker() const { return exchangeId_ + ":" + contractId_; }

	virtual int buyOpenQty() const { return 0; }
	virtual int sellOpenQty() const { return 0; }
	virtual int buyOpenOrderQty() const { return 0; }
	virtual int buyCloseOrderQty() const { return 0; }
	virtual int sellOpenOrderQty() const { return 0; }
	virtual int sellCloseOrderQty() const { return 0; }
    };
 
    class SimMatchy : public Matchy
    {
    protected:
	BookUpdate<true> currBookUpdate_;
	
	list<BookUpdate<true> > nextBookUpdate_;
	list<TradeUpdate<true> > nextTradeUpdate_;
	
	double tickSize_ = 0.2;
	double slippage_ = 0.0;

	/* Median book size (for simulation of large market orders purpose) */
	int medianBookSize_ = 0;

	ifstream data_file_;

    public:
	class InitException : exception
	{
	    virtual const char * what() const throw()
	    {
		return "Data initialization failed..";
	    }
	};
	
	SimMatchy() = delete;

	SimMatchy(SimMatchy const &) = delete;

	SimMatchy & operator=(SimMatchy const &) = delete;

	SimMatchy(string, string, double, string, SimulatorCore *);

	inline void setTickSize(double tickSize) { tickSize_ = tickSize; }
	inline void setSlippage(double slippage) { slippage_ = slippage; }
	inline void setMedianBookSize(int size) { medianBookSize_ = size; }

	virtual ~SimMatchy();

        virtual bool preLoadData(string);

	virtual bool init();

	virtual void parser(string, BookUpdate<true> *, TradeUpdate<true> *);

	virtual bool end() const;

	virtual void next();

	virtual void process(int);

	void matchMarketOrder(BookUpdate<true> &, vector<MarketOrderMessage> &, time_stamp &);
	void matchLimitOrder(BookUpdate<true> &, vector<LimitOrderMessage> &, vector<LimitOrderMessage> &, time_stamp &, BuySellSide);
    };   

}

#endif  // MATCHY_HPP
