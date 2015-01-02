//
// File:        $ VVDSM.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/08/17 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef VVDSM_HPP
#define VVDSM_HPP

#include <sstream>
#include <limits>
#include <cstdlib>

using std::string;
using std::stringstream;
using std::numeric_limits;
using std::abs;

#include "Px.hpp"
#include "Strategy.hpp"
#include "Signal.hpp"
#include "Feature.hpp"
#include "SimulatorCore.hpp"
#include "Logger.hpp"
#include "IncludeHelper.hpp"

using simulator::Px;
using simulator::Signal;
using simulator::SignalOnPxPredict;
using simulator::Strategy;
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
    class VVDSM : public Strategy
    {
    private:
	// signal related
	TradingModel tradingModel_;

	// contract specification related
        const double multiplier_;
        const double tickSize_;
    
        // trading specification related
        int orderSize_ = 0;
        double stopLossOrderLimit_ = numeric_limits<double>::min();   // in RMB space
        double takeProfitOrderLimit_ = numeric_limits<double>::max();        // in RMB space
        double takeProfitOrderPriceLimit_;                                   // in deltaPrice space
        bool enableTrading_ = false;
        bool closePosition_ = false;
        string startTradingTimeString_;
        string exitTradingTimeString_;
        time_stamp startTradingTs_;
        time_stamp exitTradingTs_;
        // double spread_ = numeric_limits<double>::max();

	PFeature volPtr_;
	double thresholdVol_ = numeric_limits<double>::max();

	// market and strategy status for logging
	int bidSize_ = 0;
	double bid_ = 0;
	double ask_ = 0;
	int askSize_ = 0;
	int marketVolume_ = 0;
	double tradePrice_ = 0.0;
	double signal_ = 0.0;
	double volatility_ = 0.0;

	std::string getStatusString();

	void placeAggressiveLimitOrder(BuySellSide side, int size, int prodid, int aggressiveness);
	void placeJoiningLimitOrder(BuySellSide side, int size, int prodid);

    public:
	VVDSM(string exchange, string contract, double multiple, double tickSize, string startTradingTimeString, string exitTradingTimeString, SimulatorCore * simcore_ptr, Logger * logger_ptr);
	~VVDSM();

	void setStopLoss(double stoploss);
	void setTakeProfit(double takeprofit);
	void setThreshold(double threshold);
	void setOrderSize(int ordersize);
	void setThresholdVol(double thresholdVol);

        // inline bool tightQ();
	
	void process();
    };

}

#endif  // VVDSM_HPP
