//
// File:        $ SMMS.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/12/27 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef SMMS_HPP
#define SMMS_HPP

#include <string>
#include <sstream>
#include <limits>
#include <cstdlib>
#include <algorithm>

using std::string;
using std::stringstream;
using std::numeric_limits;
using std::abs;
using std::min;
using std::max;
using std::any_of;

#include "Strategy.hpp"
#include "Feature.hpp"
#include "Logger.hpp"

using simulator::ImpliedSpreadPrice;
using simulator::ImpliedSpreadPriceSmooth;
// using simulator::ImpliedSpreadBidAskSpread;
// using simulator::HighFrequencyVolatilitySpread;
using simulator::PFeature;

namespace simulator
{

    class SMMS : public Strategy
    {
    private:
	// contract specification related
        const double multiplier_;
        const double tickSize_;

        // trading specification related
        bool enableTrading_ = false;
        bool closePosition_ = false;
        bool closeSpreadPosition_ = false;
	bool spreadOnOff_ = true;
	bool stopLoss_ = false;
	bool stopQuoting_ = false;
	bool hasMdUpdate_ = false;
	bool hasMdUpdateFront_ = false;
	bool hasMdUpdateBack_ = false;
	bool hedgeBuyQ;

	int orderSize_ = 0;
	int maxSpreadPosition_ = 0;
	int maxCancelQty_ = 480;
	int bidQtys_ [2];
	int askQtys_ [2];
	int totalTradeQty_[2];
	int spreadPosition_ = 0;
	int positionFront, positionBack, positionDelta;

        string startTradingTimeString_;
	string exitNoonTimeString_;
	string startNoonTimeString_;
        string exitTradingTimeString_;

	unsigned long long quoteTimeout_ = 1000000000ULL;

        time_stamp startTradingTs_;
	time_stamp exitNoonTs_;
	time_stamp startNoonTs_;
        time_stamp exitTradingTs_;
	time_stamp lastQuoteTs_;

	double edge_                       = numeric_limits<double>::max();  // in deltaPrice space
	double buffer_                     = numeric_limits<double>::max();  // in deltaPrice space
        double stopLossOrderLimit_         = numeric_limits<double>::min();  // in RMB space
	double stopLossOrdersLimit_        = numeric_limits<double>::min();  // in RMB space
        double takeProfitOrderLimit_       = numeric_limits<double>::max();  // in RMB space
        double takeProfitOrderPriceLimit_  = numeric_limits<double>::max();  // in deltaPrice space
	double takeProfitOrdersLimit_      = numeric_limits<double>::max();  // in RMB space
	double takeProfitOrdersPriceLimit_ = numeric_limits<double>::max();  // in deltaPrice space
	double expSpreadPx_;
	
	// internal stuff
	PFeature iSpreadPtr_;
	PFeature maPxPtr_;
	Px bidPxs_ [2];
	Px askPxs_ [2];
	Px targetActivePx, targetHedgePx;
	BuySellSide fillSide;
	BuySellSide hedgeSide;

    public:
	SMMS(string exchange, string contractFront, string contractBack, double multiple, double tickSize, string startTradingTimeString, string exitNoonTimeString, string startNoonTimeString, string exitTradingTimeString, SimulatorCore * simcore_ptr, Logger * logger_ptr);

	void setEdge(double edge);
	void setBuffer(double buffer);
	void setTakeProfit(double takeprofit, double takeprofits);
	void setStopLoss(double stoploss, double stoplosses);
	void setMaxSpreadPosition(int maxSpreadPosition);
	void setMaxCancelQty(int maxCancelQty);
	void setOrderSize(int ordersize);

	void process();

	string getStatusString();

	bool exceedMaxSpreadPositionQ() const;

	double getStaticOpenPnL(BuySellSide side, int prodId) const;
	double getStaticSpreadOpenPnL(BuySellSide side, int prodId) const;
	double getStaticBestPnL(BuySellSide side, int prodId) const;

	bool pendingCancelMessagesQ(BuySellSide side, bool frontQ) const;
	bool pendingCancelMessagesQ(BuySellSide side) const;
	bool pendingCancelMessagesQ(bool frontQ) const;
	bool pendingCancelMessagesQ() const;

	bool pendingOrderMessagesQ(BuySellSide side, bool frontQ) const;
	bool pendingOrderMessagesQ(BuySellSide side) const;
	bool pendingOrderMessagesQ(bool frontQ) const;
	bool pendingOrderMessagesQ() const;

	bool pendingMessagesQ(BuySellSide side, bool frontQ) const;
	bool pendingMessagesQ(BuySellSide side) const;
	bool pendingMessagesQ(bool frontQ) const;
	bool pendingMessagesQ() const;

	void activeHedger(Px targetPx, int hedgeQty, BuySellSide hedgeSide, int prodId, bool qCancelOnly);
	void passiveHedger(Px targetPx, int hedgeQty, BuySellSide hedgeSide, int prodId, bool qCancelOnly);
	void quotePositions4way(bool qCancelOnly);
	void quotePositions2way(double targetPx, int targetQty, BuySellSide side, bool qCancelOnly);

	bool quoteTimeout();
    };	

}

#endif  // SMMS_HPP
