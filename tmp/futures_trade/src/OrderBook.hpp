//
// File:        $ OrderBook.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/10/15 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef ORDERBOOK_HPP
#define ORDERBOOK_HPP

#include <vector>
#include <algorithm>
#include <numeric>

using std::vector;
using std::find_if;
using std::sort;
using std::remove_if;
using std::accumulate;
using std::move;

#include "Message.hpp"
#include "Px.hpp"

using simulator::BuySellSide;
using simulator::LimitOrderMessage;
using simulator::MarketOrderMessage;
using simulator::CancelOrderMessage;
using simulator::FilledOrderMessage;
using simulator::RejectedOrderMessage;
using simulator::Px;

namespace simulator
{

    class OrderBook
    {
    private:
	vector<vector<LimitOrderMessage> > bidLimitOrderBook_;    // 1st level ordered by descending prices, 2nd level ordered by ascending timestamps
	vector<vector<LimitOrderMessage> > askLimitOrderBook_;    // 1st level ordered by ascending prices, 2nd level ordered by ascending timestamps
	vector<MarketOrderMessage> marketOrderBook_;              // ordered by ascending timestamps

    public:
	/* default constructor */
	OrderBook();

	/* copy constructor */
	OrderBook(OrderBook const &);

	/* assignment operator overloading */
	OrderBook & operator=(OrderBook const &);

	/* default destructor */
	~OrderBook();

	vector<vector<LimitOrderMessage> > & bidLimitOrderBook();
	vector<vector<LimitOrderMessage> > getBidLimitOrderBook() const;
	void clearBidLimitOrderBook();

	vector<vector<LimitOrderMessage> > & askLimitOrderBook();
	vector<vector<LimitOrderMessage> > getAskLimitOrderBook() const;
	void clearAskLimitOrderBook();

	vector<vector<LimitOrderMessage> > getLimitOrderBook(BuySellSide) const;

	vector<MarketOrderMessage> & marketOrderBook();
	vector<MarketOrderMessage> getMarketOrderBook() const;
	void clearMarketOrderBook();

	/* update limit order method */
	void limitOrderHelper(LimitOrderMessage const *);

	/* update market order method */
	void marketOrderHelper(MarketOrderMessage const *);

	/* update cancel order method for market order */
	bool cancelOrderHelper(CancelOrderMessage<MarketOrderMessage> const *);

	/* update cancel order method for limit order */
	bool cancelOrderHelper(CancelOrderMessage<LimitOrderMessage> const *);

        /* update filled order method */
        bool filledOrderHelper(FilledOrderMessage const *);

        /* update rejected order method */
        bool rejectedOrderHelper(RejectedOrderMessage<LimitOrderMessage> const *);
        bool rejectedOrderHelper(RejectedOrderMessage<MarketOrderMessage> const *);
        bool rejectedOrderHelper(RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage> > const *);
        bool rejectedOrderHelper(RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage> > const *);

	/* count number of market orders for buy or sell */
	int countMarketOrders(BuySellSide);

	/* count total number of market orders */
	int countMarketOrders();

	/* count number of limit orders for buy or sell */
	int countLimitOrders(BuySellSide);

	/* count total number of limit orders */
	int countLimitOrders();

	/* count number of limit orders for buy or sell conditioned on px level */
	int countLimitOrders(BuySellSide, Px);

	/* count total number of limit orders conditioned on px level*/
	int countLimitOrders(Px);

	/* count market orders' qty for buy or sell */
	int countMarketOrderQty(BuySellSide);

	/* count total market orders's qty */
	int countMarketOrderQty();

	/* count limit orders' qty for buy or sell */
	int countLimitOrderQty(BuySellSide);

	/* count total limit orders' qty */
	int countLimitOrderQty();

	/* count limit orders' qty for buy or sell conditioned on px level */
	int countLimitOrderQty(BuySellSide, Px);

	/* count total limit orders' qty conditioned on px level*/
	int countLimitOrderQty(Px);
    };

}

#endif    // ORDERBOOK_HPP
