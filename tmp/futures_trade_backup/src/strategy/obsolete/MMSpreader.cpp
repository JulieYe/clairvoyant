//
// File:        $ MMSpreader.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/08/05 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include "MMSpreader.hpp"

namespace simulator
{

    MMSpreader::MMSpreader(string exchange, string contract0, string contract1, double multiplier, double tickSize, string startTradingTimeString, string exitTradingTimeString, SimulatorCore * simcore_ptr, Logger * logger_ptr) :
	Strategy("MMSpreader", simcore_ptr, logger_ptr),
	multiplier_(multiplier),
	tickSize_(tickSize),
	startTradingTimeString_(startTradingTimeString),
	exitTradingTimeString_(exitTradingTimeString)
    {
	// add strat
	addStrat(exchange, contract0, multiplier);
	addStrat(exchange, contract1, multiplier);

	string datestr = SimulatorCore::convertTimePointToString(simcore_ptr->getCurrTime()).substr(0, 10);
	stringstream ss1, ss2;
	ss1 << datestr << "T" << startTradingTimeString_ << ".000000000";
	startTradingTs_ = SimulatorCore::convertStringToTimePoint(ss1.str());
	ss2 << datestr << "T" << exitTradingTimeString_ << ".000000000";
	exitTradingTs_ = SimulatorCore::convertStringToTimePoint(ss2.str());
    }

    bool MMSpreader::tightQ(int productId)
    {
    	return spreads_[productId] <= 1.1 * tickSize_;
    }

    bool MMSpreader::isBookGood(BookUpdate<true> &bu)
    {
	return ((bu.getAskPx())[0].getPx() > 1e-6
		&& (bu.getBidPx())[0].getPx() > 1e-6);
    }

    MMSpreader::~MMSpreader()
    {}
    
    void MMSpreader::process()
    {
	// set enable trading and set close position
	if (currTime_ > startTradingTs_) {
	    enableTrading_ = true;
	}

	if (currTime_ > exitTradingTs_) {
	    closePosition_ = true;
	}

	for (int productId = 0; productId < 2; ++productId) {
	    if (!bookUpdates_[productId].empty() && !tradeUpdates_[productId].empty()) {
		if (isBookGood(bookUpdates_[productId].back())) {
		    spreads_[productId] = bookUpdates_[productId].back().getAskPx()[0].getPx() - bookUpdates_[productId].back().getBidPx()[0].getPx();
		}
	    }
	}
	
	if (closePosition_ && (getOpenPosition(0) != 0 || getOpenPosition(1) != 0)) {
	    if (tightQ(0) && tightQ(1) && pendingMarketOrderMessages_[0].empty() && pendingMarketOrderMessages_[1].empty()) {
		placeMarketOrder(getOpenPosition(0) > 0 ? BuySellSide::sell : BuySellSide::buy, abs(getOpenPosition(0)), 0);
		placeMarketOrder(getOpenPosition(1) > 0 ? BuySellSide::sell : BuySellSide::buy, abs(getOpenPosition(1)), 1);
	    }
	    return;
	}

	if (enableTrading_ && (getOpenPosition(0) != spreadPositions_[0] || getOpenPosition(1) != spreadPositions_[1])) {
	    if (tightQ(0) && tightQ(1) && pendingMarketOrderMessages_[0].empty() && pendingMarketOrderMessages_[1].empty()) {
		placeMarketOrder(spreadPositions_[0] > 0 ? BuySellSide::buy : BuySellSide::sell, 1, 0);
		placeMarketOrder(spreadPositions_[1] > 0 ? BuySellSide::buy : BuySellSide::sell, 1, 1);
	    }
	}
    }

}

// MMSPREADER_CPP
