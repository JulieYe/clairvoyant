//
// File:        $ SpreadMM.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2014/01/22 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2014 by Zhengzhang Futures Co. Ltd.

#ifndef SpreadMM_HPP
#define SpreadMM_HPP

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
using simulator::HighFrequencyVolatility;

namespace simulator
{

    class SpreadMM : public Strategy
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
	bool hedgeBuyQ_;
	bool fillFrontQ_;
	bool hasMdUpdates_ [2] = {false, false};

	int orderSize_ = 0;
	int maxSpreadPosition_ = 0;
	int maxCancelQty_ = 499;
	int spreadPosition_ = 0;
	int fillIdx_;
	int positionFront_;
	int positionBack_;
	int bidQtyQualifier_ = 2;
	int askQtyQualifier_ = 2;
	int bidQtys_ [2];
	int askQtys_ [2];

	unsigned long long quoteTimeout_ = 1000000000ULL;

        string startTradingTimeString_;
	string exitNoonTimeString_;
	string startNoonTimeString_;
        string exitTradingTimeString_;

        time_stamp startTradingTs_;
	time_stamp exitNoonTs_;
	time_stamp startNoonTs_;
        time_stamp exitTradingTs_;

	double enterEdge_                  = numeric_limits<double>::max();  // in deltaPrice space
	double exitEdge_                   = numeric_limits<double>::max();  // in deltaPrice space
	double edge_                       = numeric_limits<double>::max();  // in deltaPrice space
	double buffer_                     = numeric_limits<double>::max();  // in deltaPrice space
        double stopLossOrderLimit_         = numeric_limits<double>::min();  // in RMB space
	double expSpreadPx_, vol_;
	
	// internal stuff
	PFeature iSpreadPtr_;
	PFeature maPxPtr_;
	PFeature volPtr_;

	Px targetHedgePx_;
	Px bidPxs_ [2];
	Px askPxs_ [2];

	BuySellSide fillSide_;
	BuySellSide hedgeSide_;

    public:
	SpreadMM(string exchange, string contractFront, string contractBack, double multiple, double tickSize, string startTradingTimeString, string exitNoonTimeString, string startNoonTimeString, string exitTradingTimeString, SimulatorCore * simcore_ptr, Logger * logger_ptr);

	void setEdges(double enterEdge, double exitEdge);
	void setBuffer(double buffer);
	void setStopLoss(double stoploss);
	void setMaxSpreadPosition(int maxSpreadPosition);
	void setMaxCancelQty(int maxCancelQty);
	void setOrderSize(int ordersize);

	void process();
	void take(int position);
	void reconcilePositions();
	void reconcileSides(bool frontQ);
	void calcBuffer();

	string getStatusString();

	bool exceedMaxSpreadPositionQ() const;

	double getStaticOpenPnL(BuySellSide side, int prodId) const;

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
	void quotePositions4way(bool qCancelOnly);
	void quotePositions2way(int targetQty, BuySellSide side, bool qCancelOnly);

	bool hasMdUpdate() const { return hasMdUpdates_[0] || hasMdUpdates_[1]; }
    };	

}

#endif  // SpreadMM_HPP
