//
// File:        $ VUDS.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/05/21 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef VUDS_HPP
#define VUDS_HPP

#include <sstream>
#include <limits>

using std::string;
using std::stringstream;
using std::numeric_limits;

#include "Strategy.hpp"
#include "Signal.hpp"
#include "Feature.hpp"
#include "SimulatorCore.hpp"
#include "Logger.hpp"
#include "IncludeHelper.hpp"

using simulator::Signal;
using simulator::SignalOnPxPredict;
using simulator::Strategy;
using simulator::PFeature;
using simulator::BookSizeChg;
using simulator::BookSizeChg2;
using simulator::VwapMidReturn;
using simulator::TradeDeviationMA;
using simulator::VolumeTrend;
using simulator::Volatility;
using simulator::HighFrequencyVolatility;
using simulator::SignedVolume;
using simulator::SignedVolumeTradeVsMid;
using simulator::SignedVolumeWithBookSizeChg;
using simulator::BookPressure;
using simulator::BookPressureNL;
using simulator::BookPressureLog;
using simulator::ShortTermReturn;
using simulator::SimulatorCore;
using simulator::Logger;

namespace simulator
{
    class VUDS : public Strategy
    {
    private:
	//signal related
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
	time_stamp startTradingNoonTs_;
	time_stamp exitTradingNoonTs_;
        double spread_ = numeric_limits<double>::max();

	PFeature volPtr_;
	double thresholdVol_ = numeric_limits<double>::max();

	void makeBidMarketPerLevel(Px px, int workingQty);
	void makeAskMarketPerLevel(Px px, int workingQty);

    public:
	VUDS(string exchange, string contract, double multiple, double tickSize, string startTradingTimeString, string exitTradingTimeString, SimulatorCore * simcore_ptr, Logger * logger_ptr);
	~VUDS();

	void setStopLoss(double stoploss);
	void setTakeProfit(double takeprofit);
	void setThreshold(double threshold);
	void setOrderSize(int ordersize);
	void setThresholdVol(double thresholdVol);

	// Count the resting limit orders in the order book of the same price and side, 
	// cancel all orders of the same side due to different price levels 
	// or qty of resting orders exceeds the working qty.
	void makeMarketPerLevel(Px px, int workingQty, BuySellSide side);

        inline bool tightQ();
	
	void process();
    };

}

#endif  // VUDS_HPP
