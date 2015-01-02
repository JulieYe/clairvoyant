//
// File:        $ LLS.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/05/16 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef LLS_HPP
#define LLS_HPP

#include <string>
#include <sstream>
#include <limits>
#include <deque>
#include <cstdlib>

using std::string;
using std::stringstream;
using std::numeric_limits;
using std::deque;
using std::abs;

#include "Strategy.hpp"
#include "Feature.hpp"
#include "Logger.hpp"
#include "INIReader.hpp"

namespace simulator
{
    class LLS : public Strategy
    {
    private:
	// contract specification related
        const double multiplier_;
        const double tickSize_;
	const double leadTickSize_;

	string exchange_;
	string exchangeLead_;

        // trading specification related
        double stopLossOrderLimit_ = -1.0 * numeric_limits<double>::max();   // in RMB space
        double takeProfitOrderLimit_ = numeric_limits<double>::max();        // in RMB space
        double takeProfitOrderPriceLimit_;                                   // in deltaPrice space
        bool enableTrading_ = false;
        bool closePosition_ = false;
        string startTradingTimeString_;
        string exitTradingTimeString_;
        time_stamp startTradingTs_;
        time_stamp exitTradingTs_;
        // double spread_ = numeric_limits<double>::max();
	int aggressivenessInTicks_ = 4;
	unsigned int nSnapshots_ = 1;

	int orderSize_ = 0;

	double leadThresholdInPrice_ = numeric_limits<int>::max();
	double tradeThresholdInPrice_ = 0.0;

	Px lastBid_;
	Px lastAsk_;
	int lastBidSize_;
	int lastAskSize_;

	deque<double> leadMidQueue_;
	deque<double> tradeMidQueue_;
	
	double previousLeadMid_;
	double previousTradeMid_;
	double currentLeadMid_;
	double currentTradeMid_;

    public:
	LLS(string exchange, string contract, double multiple, double tickSize,
	    string exchange_lead, string contract_lead, double tickSize_lead,
	    string startTradingTimeString, string exitTradingTimeString, 
	    SimulatorCore * simcore_ptr, Logger * logger_ptr);

	LLS(string exchange, double multiple, double tickSize,
	    string exchange_lead, double tickSize_lead);

	void getParamFromConfig(INIReader &iniconfig, string sectionName);

	void setContracts(string tradeContract, string leadContract);

	void setTradingHours(string startTradingTimeString, string exitTradingTimeString);

	void setStopLoss(double stoploss);
	void setTakeProfit(double takeprofit);
	void setThreshold(int lead_threshold_in_ticks, int trade_threshold_in_ticks);
	void setOrderSize(int ordersize);
	void setAggressivenessInTicks(int ticks);
	void setNSnapshots(int nn);

        // inline bool tightQ();
	
	void process();

    private:
	bool isBookGood(BookUpdate<true> &bu);
	bool isBookGood();
	void placeAggressiveLimitOrder(BuySellSide side, int size, int prodid);
	void placeJoiningLimitOrder(BuySellSide side, int size, int prodid);
	void logStrategyData();
	bool isReady();
    };	

}

#endif  // LLS_HPP
