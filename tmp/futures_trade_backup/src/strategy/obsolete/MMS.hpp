//
// File:        $ MMS.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/10/09 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef MMS_HPP
#define MMS_HPP

#include <sstream>
#include <limits>
#include <cstdlib>
#include <cmath>

using std::string;
using std::stringstream;
using std::numeric_limits;
using std::abs;
using std::fabs;

#include "Px.hpp"
#include "Strategy.hpp"
#include "SimulatorCore.hpp"
#include "Logger.hpp"
#include "IncludeHelper.hpp"
#include "Feature.hpp"

using simulator::Px;
using simulator::Strategy;
using simulator::SimulatorCore;
using simulator::Logger;
using simulator::HighFrequencyVolatility;
// using simulator::SweepStrong;

namespace simulator
{
    class MMS : public Strategy
    {
    private:
	// signal related
	TradingModel tradingModel_;

	// contract specification related
        const double multiplier_;
        const double tickSize_;
    
        // trading specification related
        int orderSize_ = 0;
        double stopLossOrderLimit_ = -1.0 * numeric_limits<double>::max();   // in RMB space
        double takeProfitOrderLimit_ = numeric_limits<double>::max();        // in RMB space
        double takeProfitOrderPriceLimit_;                                   // in deltaPrice space
        bool enableTrading_ = false;
        bool closePosition_ = false;
        string startTradingTimeString_;
        string exitTradingTimeString_;
        time_stamp startTradingTs_;
        time_stamp exitTradingTs_;
        double spread_ = numeric_limits<double>::max();
	double topBidPx_ = 0.0;
	int topBidQty_ = 0;
	double topAskPx_ = 0.0;
	int topAskQty_ = 0;
	int marketVolume_ = 0;
	double tradePx_ = 0.0;
	double signal_ = 0.0;
	double vol_ = 0.0;

        double bandwidth_ = numeric_limits<double>::max();        // in deltaPrice space

	PFeature volPtr_;
	double thresholdVolW_ = numeric_limits<double>::max();
	double thresholdVolS_ = numeric_limits<double>::max();
	double thresholdVolE_ = numeric_limits<double>::max();

        // PFeature sweepBidPtr_, sweepAskPtr_;

	bool winner_ = false;

	double thresholdLevelW_ = numeric_limits<double>::max();
	double thresholdLevelS_ = numeric_limits<double>::max();
	double thresholdLevelE_ = numeric_limits<double>::max();

	// void makeBidMarketPerLevel(Px px, int workingQty);
	// void makeAskMarketPerLevel(Px px, int workingQty);

	string getStatusString();

    public:
	MMS(string exchange, string contract, double multiple, double tickSize, string startTradingTimeString, string exitTradingTimeString, SimulatorCore * simcore_ptr, Logger * logger_ptr);
	~MMS();

	void setStopLoss(double stoploss);
	void setTakeProfit(double takeprofit);
	void setOrderSize(int ordersize);
	void setBandwidth(double bandwidth);
	void setThreshold();
	void setThresholdVol();

	// Count the resting limit orders in the order book of the same price and side, 
	// cancel all orders of the same side due to different price levels 
	// or qty of resting orders exceeds the working qty.
	// void makeMarketPerLevel(Px px, int workingQty, BuySellSide side);

        // inline bool tightQ();

	bool isBookGood(BookUpdate<true> &);
	
	void process();
    };

}

#endif  // MMS_HPP
