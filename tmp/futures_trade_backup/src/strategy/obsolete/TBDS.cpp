//
// File:        $ TBDS.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/08/25 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include "TBDS.hpp"

namespace simulator
{

    TBDS::TBDS(string exchange, string contract, double multiplier, double tickSize, string startTradingTimeString, string exitTradingTimeString, SimulatorCore * simcore_ptr, Logger * logger_ptr) :
	Strategy("TBDS", simcore_ptr, logger_ptr),
	multiplier_(multiplier),
	tickSize_(tickSize),
	startTradingTimeString_(startTradingTimeString),
	exitTradingTimeString_(exitTradingTimeString),
	/* TODO */
	volPtr_(new HighFrequencyVolatility<60,2>())
    {
	// add strat
	addStrat(exchange, contract, multiplier);

	string datestr = SimulatorCore::convertTimePointToString(simcore_ptr->getCurrTime()).substr(0, 10);
	stringstream ss1, ss2;
	ss1 << datestr << "T" << startTradingTimeString_ << ".000000000";
	startTradingTs_ = SimulatorCore::convertStringToTimePoint(ss1.str());
	ss2 << datestr << "T" << exitTradingTimeString_ << ".000000000";
	exitTradingTs_ = SimulatorCore::convertStringToTimePoint(ss2.str());
	
	tradingModel_.directionSignal_.registerThresholds(-1.0 * numeric_limits<double>::max(), numeric_limits<double>::max());
    }

    void TBDS::setThreshold(double threshold)
    {
	cout << "Threshold is set to " << threshold << endl;
	tradingModel_.directionSignal_.registerThresholds(-1.0 * threshold, threshold);
    }

    void TBDS::setThresholdVol(double thresholdVol)
    {
	thresholdVol_ = thresholdVol;
	cout << "Volatility threshold is set to " << thresholdVol_ << endl;
    }

    void TBDS::setOrderSize(int ordersize)
    {
	orderSize_ = ordersize;
	cout << "Order size is set to " << orderSize_ << endl;
    }

    std::string TBDS::getStatusString()
    {
	stringstream ss;
	ss << ","
	   << bidSize_ << ","
	   << bid_ << ","
	   << ask_ << ","
	   << askSize_ << ","
	   << marketVolume_ << ","
	   << tradePrice_ << ","
	   << signal_ << ","
	   << volatility_ ;
	return ss.str();
    }

    void TBDS::placeAggressiveLimitOrder(BuySellSide side, int size, int prodid, int aggressiveness)
    {
	double price_place = (side == BuySellSide::buy ? 
			      ask_ + aggressiveness * tickSize_ :
			      bid_ - aggressiveness * tickSize_);
	placeLimitOrder(Px(price_place, tickSize_), side, size, prodid);
    }
 
    void TBDS::placeJoiningLimitOrder(BuySellSide side, int size, int prodid)
    {
	placeLimitOrder(side == BuySellSide::buy ? Px(bid_, tickSize_) : Px(ask_, tickSize_), side, size, prodid);
    }

    TBDS::~TBDS()
    {}
    
    void TBDS::process()
    {
	logStrategyStatus(",BidQty,BidPx,AskPx,AskQty,MarketVolume,LastTradePx,Signal,Volatility",
			  getStatusString());

	bool hasMDupdate = false; 

	// set enable trading and set close position
	if (currTime_ > startTradingTs_) {
	    enableTrading_ = true;
	}
	if (currTime_ > exitTradingTs_) {
	    closePosition_ = true;
	}
	
	if (!bookUpdates_[0].empty() && !tradeUpdates_[0].empty()) {
	    auto ibu = bookUpdates_[0].begin();
	    auto itu = tradeUpdates_[0].begin();
	    while (ibu != bookUpdates_[0].end() && itu != tradeUpdates_[0].end()) {
		tradingModel_.directionSignal_.update((*ibu));
		tradingModel_.directionSignal_.update((*itu));
		tradingModel_.directionSignal_.update((*ibu), (*itu));
		volPtr_->update(*ibu);
		bidSize_ = ibu->getBidQty()[0];
		askSize_ = ibu->getAskQty()[0];
		bid_ = ibu->getBidPx()[0].getPx();
		ask_ = ibu->getAskPx()[0].getPx();
		marketVolume_ = itu->getQty();
		tradePrice_ = itu->getPx().getPx();
		ibu++;
		itu++;
	    }
	    hasMDupdate = true;
	}

	if(getPnL() < -10000.0 * orderSize_) {
	    closePosition_ = true;
	}

	if(!enableTrading_) {
	    return;
	}

	if (closePosition_) {
	    if (pendingCancelLimitOrderMessages_[0].empty()) {
		cancelAllLimitOrders(0);
	    }
	    if (getOpenPosition(0) > 0) {
		if (!pendingMarketOrderMessages_[0].empty()) {
		    if (pendingMarketOrderMessages_[0].front().getSide() != BuySellSide::sell) { 
			if (pendingCancelMarketOrderMessages_[0].empty()) {
			    cancelMarketOrder(pendingMarketOrderMessages_[0].front());
			}
			placeMarketOrder(BuySellSide::sell, getOpenPosition(0), 0);
		    }
		} else {
		    placeMarketOrder(BuySellSide::sell, getOpenPosition(0), 0);
		}
	    } else if (getOpenPosition(0) < 0) {
		if (!pendingMarketOrderMessages_[0].empty()) {
		    if (pendingMarketOrderMessages_[0].front().getSide() != BuySellSide::buy) {
			if (pendingCancelMarketOrderMessages_[0].empty()) {
			    cancelMarketOrder(pendingMarketOrderMessages_[0].front());
			}
			placeMarketOrder(BuySellSide::buy, -getOpenPosition(0), 0);
		    }
		} else {
		    placeMarketOrder(BuySellSide::buy, -getOpenPosition(0), 0);
		}
	    } 
	    return;
	}

	if (!pendingCancelLimitOrderMessages_[0].empty() || 
	    !pendingLimitOrderMessages_[0].empty()) {
	    return;
	}

	BuySellSide trade_signal = tradingModel_.directionSignal_.getSignal();
	signal_ = tradingModel_.directionSignal_.getRawValue();
	double vol_signal = volPtr_->getValue();
	volatility_ = vol_signal;
	int open_position = getOpenPosition(0);
	BuySellSide side = (open_position > 0) ? BuySellSide::buy : (open_position < 0 ? BuySellSide::sell : BuySellSide::unknown);
	BuySellSide side_hedge = (open_position > 0) ? BuySellSide::sell : (open_position < 0 ? BuySellSide::buy : BuySellSide::unknown);

	// flat position
	if (open_position == 0) {
	    // if there are limit orders, cancel limit order
	    if (orderbooks_[0].countLimitOrders() > 0) {
		cancelAllLimitOrders(0);
	    }
	    if ((vol_signal >= thresholdVol_) && (trade_signal == BuySellSide::buy || trade_signal == BuySellSide::sell) && hasMDupdate) {
		placeAggressiveLimitOrder(trade_signal, orderSize_, 0, 1);
	    }
	}
	// positive position
	else if (open_position > 0) {
	    cancelAllLimitOrders(side, 0);
	    if (hasMDupdate && 
		(currTime_.time_since_epoch().count() - filledMessageLists_[0]->filledMessageList_.front().getTimestamp().time_since_epoch().count()) >= 2500000000ULL) {
		int qty = orderbooks_[0].countLimitOrderQty(side_hedge);
		if (qty > 0) {
		    if (orderbooks_[0].getAskLimitOrderBook()[0][0].getPx() > Px(ask_, tickSize_)) {
			cancelAllLimitOrders(side_hedge, 0);
			placeJoiningLimitOrder(side_hedge, abs(open_position), 0);
		    } else if (abs(open_position) > qty) {
			placeJoiningLimitOrder(side_hedge, abs(open_position) - qty, 0);
		    }
		} else {
			placeJoiningLimitOrder(side_hedge, abs(open_position), 0);
		}
	    }
	}
	// negative position
	else {
	    cancelAllLimitOrders(side, 0);
	    if (hasMDupdate && 
		(currTime_.time_since_epoch().count() - filledMessageLists_[0]->filledMessageList_.front().getTimestamp().time_since_epoch().count()) >= 2500000000ULL) {
		int qty = orderbooks_[0].countLimitOrderQty(side_hedge);
		if (qty > 0) {
		    if (orderbooks_[0].getBidLimitOrderBook()[0][0].getPx() < Px(bid_, tickSize_)) {
			cancelAllLimitOrders(side_hedge, 0);
			placeJoiningLimitOrder(side_hedge, abs(open_position), 0);
		    } else if (abs(open_position) > qty) {
			placeJoiningLimitOrder(side_hedge, abs(open_position) - qty, 0);
		    }
		} else {
			placeJoiningLimitOrder(side_hedge, abs(open_position), 0);
		}
	    }
	}
    }

}

// TBDS_CPP
