//
// File:        $ Strategy.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2014/01/09 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2014 by Zhengzhang Futures Co. Ltd.

#ifndef STRATEGY_HPP
#define STRATEGY_HPP

#include <list>
#include <tuple>
#include <string>
#include <vector>
#include <chrono>
#include <algorithm>
#include <numeric>

using std::list;
using std::tuple;
using std::string;
using std::vector;
using std::chrono::time_point;
using std::chrono::system_clock;
using std::chrono::duration;
using std::nano;
using std::partition_copy;
using std::back_inserter;
using std::remove_if;
using std::for_each;
using std::accumulate;

#include "Message.hpp"
#include "OrderBook.hpp"
#include "SimulatorCore.hpp"
#include "Logger.hpp"

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
using simulator::Logger;
using simulator::time_stamp;

namespace simulator
{

    class Strategy
    {
    public:
	class FillList {
	public :
	    list<FilledOrderMessage> filledMessageList_; //cache to keep track of filled message (for FIFO blotter)
	    double multiplier_; // multiplier to convert price to RMB
	    double calcVWAP() const;

	    FillList(double multipler);
	    ~FillList() { }
	};
    
    private:
	int stratId_ = -1;
	const string strategyName_;
	unsigned int numProducts_ = 0;
	
	// pnl status header
	bool headerLogged_ = false;

#ifdef SIMULATION
	int nextOrderId_ = -1;
#endif
	int nextMessageId_ = -1;

	vector<double> midPrices_;
	vector<double> closedPnls_;
	vector<double> openPnls_;
	vector<int> tradedVolumes_;
	vector<int> openPositions_;
	vector<int> openBuyOrderCounts_;
	vector<int> openSellOrderCounts_;

	//TODO these counts are not updated in the implementation right now, needs to be updated.
	vector<long> limitOrderCounts_;
	vector<long> marketOrderCounts_;
	vector<long> fillCounts_;
	vector<long> cancelCounts_;

    protected:
	vector<string> tickers_;
	vector<string> exchanges_;
	vector<string> contracts_;

	vector<double> openPnlRefPxs_;
	vector<FillList *> filledMessageLists_;

	/* Added for spread trading */
	vector<double> lastTradeBuyPxs_;
	vector<double> lastTradeSellPxs_;
	int lastTradeProdId_ = -1;
	vector<BuySellSide> lastTradeSides_;

    private:
	void computeOpenPnls(unsigned prodId);
	//int generateStratId();
	
    protected:
	time_stamp currTime_;
	vector<OrderBook> orderbooks_;
    
	SimulatorCore * simulatorCorePtr_;
	Logger * loggerPtr_; // add logger pointer
	vector<bool> hasFills_;
	vector<time_stamp> lastFillTss_;

	/* outgoing messages from Strategy */
	list<LimitOrderMessage> limitOrderMessages_;
	list<MarketOrderMessage> marketOrderMessages_;
	list<CancelOrderMessage<LimitOrderMessage> > cancelLimitOrderMessages_;
	list<CancelOrderMessage<MarketOrderMessage> > cancelMarketOrderMessages_;
    
	/* incoming order messages to from exchange */
	vector<list<AcknowledgedOrderMessage> > acknowledgedOrderMessages_;
	vector<list<FilledOrderMessage> > filledOrderMessages_;
	vector<list<CanceledOrderMessage<LimitOrderMessage> > > canceledLimitOrderMessages_;
	vector<list<CanceledOrderMessage<MarketOrderMessage> > > canceledMarketOrderMessages_;
	vector<list<RejectedOrderMessage<LimitOrderMessage> > > rejectedLimitOrderMessages_;
	vector<list<RejectedOrderMessage<MarketOrderMessage> > > rejectedMarketOrderMessages_;
	vector<list<RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage> > > > rejectedCancelLimitOrderMessages_;
	vector<list<RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage> > > > rejectedCancelMarketOrderMessages_;

	/* incoming market data from exchange */
	vector<list<BookUpdate<true> > > bookUpdates_;
	vector<list<TradeUpdate<true> > > tradeUpdates_;

	/* pending messages waiting for acks */
	vector<list<LimitOrderMessage> > pendingLimitOrderMessages_;
	vector<list<MarketOrderMessage> > pendingMarketOrderMessages_;
	vector<list<CancelOrderMessage<LimitOrderMessage> > > pendingCancelLimitOrderMessages_;
	vector<list<CancelOrderMessage<MarketOrderMessage> > > pendingCancelMarketOrderMessages_;
    
    public:
	// constructor & destructor
	Strategy(const string strat_name, SimulatorCore * simcore_ptr, Logger * logger_ptr);
	virtual ~Strategy();

	// order id generator
	int generateNewOrderId(string ticker);
	int generateNewMessageId();

	// Outbound messages to SimuatorCore
	list<LimitOrderMessage> & limitOrderMessages() { return limitOrderMessages_; }
	list<MarketOrderMessage> & marketOrderMessages() { return marketOrderMessages_; }
	list<CancelOrderMessage<LimitOrderMessage> > & cancelLimitOrderMessages() { return cancelLimitOrderMessages_; }
	list<CancelOrderMessage<MarketOrderMessage> > & cancelMarketOrderMessages() { return cancelMarketOrderMessages_; }
      
	// extend strategy
	void addStrat(string ex, string con, double mult);
    
	// get status of the strategy
	const unsigned int getNumProducts() {return numProducts_;}
	time_stamp getCurrentTime() const {return currTime_;}
	double getClosedPnL() const;
	double getClosedPnL(unsigned int index) const { return closedPnls_[index]; }
	double getOpenPnL() const;
	double getOpenPnL(unsigned int index) const { return openPnls_[index]; }
	vector<double> getOpenPnlRefPxs() const { return openPnlRefPxs_; }
	double getOpenPnlRefPxs(unsigned int index) const { return openPnlRefPxs_[index]; }
	double getPnL() const;
	double getPnL(unsigned int index) const { return closedPnls_[index] + openPnls_[index]; }
	int getVolume() const ;
	int getVolume(unsigned int index) const { return tradedVolumes_[index]; }
	int getOpenPosition() const ;
	int getOpenPosition(unsigned int index) const {return openPositions_[index]; }
	int getOpenBuyOrderCount() const;
	int getOpenBuyOrderCount(unsigned int index) const { return openBuyOrderCounts_[index]; }
	int getOpenSellOrderCount() const;
	int getOpenSellOrderCount(unsigned int index) const { return openSellOrderCounts_[index]; }
	int getOpenOrderCount() const;
	int getOpenOrderCount(unsigned int index) const { return openBuyOrderCounts_[index] + openSellOrderCounts_[index]; }
	int getLimitOrderCount() const;
	int getLimitOrderCount(unsigned int index) const { return limitOrderCounts_[index]; }
	int getMarketOrderCount() const;
	int getMarketOrderCount(unsigned int index) const { return marketOrderCounts_[index]; }
	int getFillCount() const;
	int getFillCount(unsigned int index) const { return fillCounts_[index]; }
	int getCancelCount() const;
	int getCancelCount(unsigned int index) const { return cancelCounts_[index]; }

	void printStrategyStatus();
	void logStrategyStatus();
	void logStrategyStatus(std::string header, std::string content);// header will only be logged once
    
	// process information
	virtual void process();
    
	void updateStatus();
	void updateStatusOnMessage(AcknowledgedOrderMessage message, unsigned prodId);
	void updateStatusOnMessage(RejectedOrderMessage<LimitOrderMessage> message, unsigned prodId);
	void updateStatusOnMessage(RejectedOrderMessage<MarketOrderMessage> message, unsigned prodId);
	void updateStatusOnMessage(RejectedOrderMessage< CancelOrderMessage<LimitOrderMessage> > message, unsigned prodId);
	void updateStatusOnMessage(RejectedOrderMessage< CancelOrderMessage<MarketOrderMessage> > message, unsigned prodId);
	void updateStatusOnMessage(FilledOrderMessage message, unsigned prodId);
	void updateStatusOnMessage(CanceledOrderMessage<LimitOrderMessage> message, unsigned prodId);
	void updateStatusOnMessage(CanceledOrderMessage<MarketOrderMessage> message, unsigned prodId);
	void updateStatusOnMessage(BookUpdate<true> message, unsigned prodId);
	void updateStatusOnMessage(TradeUpdate<true> message, unsigned prodId);
	//void updateStatusOnMessage(BookUpdate<false> message, unsigned prodId);
	//void updateStatusOnMessage(TradeUpdate<false> message, unsigned prodId);

	void setStratId(int id) { stratId_ = id; }
	const int getStratId() { return stratId_; }
	const string getStratname() { return strategyName_; }

	inline vector<string> getTickers() { return tickers_; }
    
	void update();
	void preProcess();
	void postProcess();

	// helpsers
	void cancelAllLimitOrders(int prodid);
	void cancelAllLimitOrders(BuySellSide side, int prodid);
	void cancelMarketOrder(MarketOrderMessage mom);
	void cancelLimitOrder(LimitOrderMessage lom);
	void placeMarketOrder(BuySellSide side, int ordersize, int prodid);
	void placeLimitOrder(Px price, BuySellSide side, int ordersize, int prodid);
    };

}

#endif    // STRATEGY_HPP
