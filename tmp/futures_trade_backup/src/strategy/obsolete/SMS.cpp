//
// File:        $ SMS.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/05/27 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include "SMS.hpp"

namespace simulator
{
    void SMS::getParamsFromConfig(string configfile)
    {
	ConfigReader cr(configfile);
	
	multiplier_ = cr.getDouble("Multiplier");
	tickSize_ = cr.getDouble("TickSize");
	orderSize_ = cr.getInteger("OrderSize");
	startTradingTimeString_ = cr.getString("StartTradingTime");
	exitTradingTimeString_ = cr.getString("ExitTradingTime");
	signalThreshold_ = cr.getDouble("SignalThreshold");
	thresholdVol_ = cr.getDouble("VolThreshold");
	addStrat(cr.getString("Exchange"), cr.getString("Contract"), multiplier_);


    }


    SMS::SMS(string configfile, SimulatorCore * simcore_ptr, Logger * logger_ptr) :
	Strategy("SMS", simcore_ptr, logger_ptr),
	volPtr_(new HighFrequencyVolatility<60,2>())
    {

	getParamsFromConfig(configfile);
	// add strat

	string datestr = SimulatorCore::convertTimePointToString(simcore_ptr->getCurrTime()).substr(0, 10);
	stringstream ss1, ss2;
	ss1 << datestr << "T" << startTradingTimeString_ << ".000000000";
	startTradingTs_ = SimulatorCore::convertStringToTimePoint(ss1.str());
	ss2 << datestr << "T" << exitTradingTimeString_ << ".000000000";
	exitTradingTs_ = SimulatorCore::convertStringToTimePoint(ss2.str());
	
	tradingModel_.directionSignal_.registerThresholds(-1.0 * signalThreshold_, signalThreshold_);
    }

    void SMS::setStopLoss(double stoploss)
    {
	stopLossOrderLimit_ = stoploss;
	cout << "Stop loss is set to " << stopLossOrderLimit_ << endl;
    }

    void SMS::setTakeProfit(double takeprofit)
    {
	takeProfitOrderLimit_ = takeprofit,
	cout << "Take profit is set to " << takeProfitOrderLimit_ << endl;
	takeProfitOrderPriceLimit_ = takeProfitOrderLimit_ / multiplier_;
	cout << "Points to take profit is set to " << takeProfitOrderPriceLimit_ << endl;
    }

    void SMS::setThreshold(double threshold)
    {
	cout << "Threshold is set to " << threshold << endl;
	tradingModel_.directionSignal_.registerThresholds(-1.0 * threshold, threshold);
    }

    void SMS::setThresholdVol(double thresholdVol)
    {
	thresholdVol_ = thresholdVol;
	cout << "Volatility threshold is set to " << thresholdVol_ << endl;
    }

    void SMS::setOrderSize(int ordersize)
    {
	orderSize_ = ordersize;
	cout << "Order size is set to " << orderSize_ << endl;
    }

    bool SMS::tightQ()
    {
	return spread_ <= 1.1 * tickSize_;
    }

    void SMS::checkAndPlaceJoiningLimitOrder(BuySellSide side, int size)
    {
	if(side == BuySellSide::buy) {
	    Px px(bid_, tickSize_);
	    int workingQty = size;
	    for (unsigned int ii = 0; ii < orderbooks_[0].getBidLimitOrderBook().size(); ++ii) {
		for (unsigned int jj = 0; jj < orderbooks_[0].getBidLimitOrderBook()[ii].size(); ++jj) {
		    if (orderbooks_[0].getBidLimitOrderBook()[ii][jj].getPx() == px && workingQty > 0) {
			if (orderbooks_[0].getBidLimitOrderBook()[ii][jj].getQty() <= workingQty) {
			    workingQty -= orderbooks_[0].getBidLimitOrderBook()[ii][jj].getQty();
			} else {
			    cancelLimitOrder(orderbooks_[0].getBidLimitOrderBook()[ii][jj]);
			}
		    } else {
			cancelLimitOrder(orderbooks_[0].getBidLimitOrderBook()[ii][jj]);
		    }
		}
	    }
	    if (workingQty > 0) {
		placeLimitOrder(px, BuySellSide::buy, workingQty, 0);
	    }

	} else {
	    Px px(ask_, tickSize_);
	    int workingQty = size;
	    for (unsigned int ii = 0; ii < orderbooks_[0].getAskLimitOrderBook().size(); ++ii) {
		for (unsigned int jj = 0; jj < orderbooks_[0].getAskLimitOrderBook()[ii].size(); ++jj) {
		    if (orderbooks_[0].getAskLimitOrderBook()[ii][jj].getPx() == px && workingQty > 0) {
			if (orderbooks_[0].getAskLimitOrderBook()[ii][jj].getQty() <= workingQty) {
			    workingQty -= orderbooks_[0].getAskLimitOrderBook()[ii][jj].getQty();
			} else {
			    cancelLimitOrder(orderbooks_[0].getAskLimitOrderBook()[ii][jj]);
			}
		    } else {
			cancelLimitOrder(orderbooks_[0].getAskLimitOrderBook()[ii][jj]);
		    }
		}
	    }
	    if (workingQty > 0) {
		placeLimitOrder(px, BuySellSide::sell, workingQty, 0);
	    }
	}
    }

    void SMS::closePosition()
    {
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

    }

    SMS::~SMS()
    {}
    
    std::string SMS::getStatusString()
    {
	stringstream ss;
	ss << ",";
	ss << bidSize_ << ","
	   << bid_ << ","
	   << ask_ << ","
	   << askSize_ << ","
	   << marketVolume_ << ","
	   << tradePrice_ << ","
	   << signal_ << ","
	   << volatility_ ;
	return ss.str();
    }

    void SMS::process()
    {
	logStrategyStatus(",bidSize,bid,ask,askSize,marketVolume,tradePrice,signal,volatility",
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
		spread_ = ibu->getAskPx()[0].getPx() - ibu->getBidPx()[0].getPx();
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

	if(getPnL() < -12000.0 * orderSize_) {
	    closePosition_ = true;
	}

	if(!enableTrading_) {
	    return;
	}

	if (closePosition_) {
	    closePosition();
	    return;
	}

	// saftey checks, waiting for proper acks!
	if (!pendingMarketOrderMessages_[0].empty() ||
	    !pendingCancelLimitOrderMessages_[0].empty() ||
	    !pendingLimitOrderMessages_[0].empty()) {
	    return;
	}

	BuySellSide trade_signal = tradingModel_.directionSignal_.getSignal();
	double vol_signal = volPtr_->getValue();
	int open_position = getOpenPosition(0);
	signal_ = tradingModel_.directionSignal_.getRawValue();
	volatility_ = vol_signal;

	// flat position
	if (open_position == 0) {
	    if ((trade_signal == BuySellSide::buy || trade_signal == BuySellSide::sell) && hasMDupdate) {
		checkAndPlaceJoiningLimitOrder(trade_signal, orderSize_);
	    }
	}
	// positive position
	else if (open_position > 0) {
	    if (orderbooks_[0].countLimitOrderQty(BuySellSide::buy) > 0 ) {
		cancelAllLimitOrders(BuySellSide::buy, 0);
	    } 
	    if (trade_signal == BuySellSide::sell && hasMDupdate) {
		checkAndPlaceJoiningLimitOrder(trade_signal, orderSize_ + abs(open_position));
	    }
	}
	// negative position
	else {
	    if (orderbooks_[0].countLimitOrderQty(BuySellSide::sell) > 0) {
		cancelAllLimitOrders(BuySellSide::sell, 0);
	    } 
	    if (trade_signal == BuySellSide::buy && hasMDupdate) {
		checkAndPlaceJoiningLimitOrder(trade_signal, orderSize_ + abs(open_position));
	    }
	}
    }

}

// SMS_CPP
