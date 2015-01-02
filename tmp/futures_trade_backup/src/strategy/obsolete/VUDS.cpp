//
// File:        $ VUDS.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/05/21 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include "VUDS.hpp"

namespace simulator
{

    VUDS::VUDS(string exchange, string contract, double multiplier, double tickSize, string startTradingTimeString, string exitTradingTimeString, SimulatorCore * simcore_ptr, Logger * logger_ptr) :
	Strategy("VUDS", simcore_ptr, logger_ptr),
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
	stringstream ssnoon1, ssnoon2;
	ssnoon1 << datestr << "T11:28:00.000000000";
	exitTradingNoonTs_ = SimulatorCore::convertStringToTimePoint(ssnoon1.str());
	ssnoon2 << datestr << "T13:02:00.000000000";
	startTradingNoonTs_ = SimulatorCore::convertStringToTimePoint(ssnoon2.str());
	
	tradingModel_.directionSignal_.registerThresholds(-1.0 * numeric_limits<double>::max(), numeric_limits<double>::max());
    }

    void VUDS::setStopLoss(double stoploss)
    {
	stopLossOrderLimit_ = stoploss;
	cout << "Stop loss is set to " << stopLossOrderLimit_ << endl;
    }

    void VUDS::setTakeProfit(double takeprofit)
    {
	takeProfitOrderLimit_ = takeprofit,
	cout << "Take profit is set to " << takeProfitOrderLimit_ << endl;
	takeProfitOrderPriceLimit_ = takeProfitOrderLimit_ / multiplier_;
	cout << "Points to take profit is set to " << takeProfitOrderPriceLimit_ << endl;
    }

    void VUDS::setThreshold(double threshold)
    {
	cout << "Threshold is set to " << threshold << endl;
	tradingModel_.directionSignal_.registerThresholds(-1.0 * threshold, threshold);
    }

    void VUDS::setThresholdVol(double thresholdVol)
    {
	thresholdVol_ = thresholdVol;
	cout << "Volatility threshold is set to " << thresholdVol_ << endl;
    }

    void VUDS::setOrderSize(int ordersize)
    {
	orderSize_ = ordersize;
    }

    void VUDS::makeMarketPerLevel(Px px, int workingQty, BuySellSide side)
    {
	if (side == BuySellSide::buy) {
	    makeBidMarketPerLevel(px, workingQty);
	} else if (side == BuySellSide::sell) {
	    makeAskMarketPerLevel(px, workingQty);
	}
    }

    void VUDS::makeBidMarketPerLevel(Px px, int workingQty)
    {
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
    }

    void VUDS::makeAskMarketPerLevel(Px px, int workingQty)
    {
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

    bool VUDS::tightQ()
    {
	return spread_ <= 1.1 * tickSize_;
    }

    VUDS::~VUDS()
    {}
    
    void VUDS::process()
    {
	logStrategyStatus();

	bool hasMDupdate = false; 

	// set enable trading and set close position
	if (currTime_ > startTradingTs_) {
	    enableTrading_ = true;
	}
	if (currTime_ > exitTradingTs_) {
	    closePosition_ = true;
	} else if (currTime_ > exitTradingNoonTs_ && currTime_ < startTradingNoonTs_) {
	    closePosition_ = true;
	} else {
	    closePosition_ = false;
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
		ibu++;
		itu++;
	    }
	    hasMDupdate = true;
	}

	if((getPnL() - 17.0 * getVolume()) < -2500.0 || getCancelCount() >= 400) {
	    closePosition_ = true;
	}

	if(!enableTrading_) {
	    return;
	}

	if (closePosition_) {
	    if (getOpenPosition(0) > 0) {
		if (!pendingMarketOrderMessages_[0].empty()) {
		    if (pendingMarketOrderMessages_[0].front().getSide() != BuySellSide::sell) {
			cancelMarketOrder(pendingMarketOrderMessages_[0].front());
			placeMarketOrder(BuySellSide::sell, orderSize_, 0);
		    }
		} else {
		    placeMarketOrder(BuySellSide::sell, orderSize_, 0);
		}
	    } else if (getOpenPosition(0) < 0) {
		if (!pendingMarketOrderMessages_[0].empty()) {
		    if (pendingMarketOrderMessages_[0].front().getSide() != BuySellSide::buy) {
			cancelMarketOrder(pendingMarketOrderMessages_[0].front());
			placeMarketOrder(BuySellSide::buy, orderSize_, 0);
		    }
		} else {
		    placeMarketOrder(BuySellSide::buy, orderSize_, 0);
		}
	    } 
	    return;
	}

	if (!pendingMarketOrderMessages_[0].empty()) {
	    return;
	}

	BuySellSide trade_signal = tradingModel_.directionSignal_.getSignal();
	double vol_signal = volPtr_->getValue();
	
	// if zero position
	if (getOpenPosition(0) == 0) {
	    // send market Order
	    if((vol_signal >= thresholdVol_) && (trade_signal == BuySellSide::buy || trade_signal == BuySellSide::sell) && tightQ() && hasMDupdate) {
		placeMarketOrder(trade_signal, orderSize_, 0);
	    }
	}
	// long position
	else if (getOpenPosition(0) > 0) {
	    // send market Order
	    if ((trade_signal == BuySellSide::sell) && tightQ() && hasMDupdate) {
		// cancelAllLimitOrders(0);
		placeMarketOrder(trade_signal, orderSize_, 0);
		//makeMarketPerLevel(bookUpdates_[0].back().getBidPx()[0], abs(getOpenPosition(0)), trade_signal);
	    }
	    // else if (vol_signal < thresholdVol_ / 2) {
	    // 	if (orderbooks_[0].countLimitOrders(BuySellSide::sell) == 0) {
	    // 	    placeLimitOrder(Px(filledMessageLists_[0]->filledMessageList_.back().getPx().getPx() + 2 * tickSize_, tickSize_), BuySellSide::sell, abs(getOpenPosition(0)), 0);
	    // 	}
	    // }
	}
	// short position
	else {
	    // send market Order
	    if ((trade_signal == BuySellSide::buy) && tightQ() && hasMDupdate) {
		// cancelAllLimitOrders(0);
		placeMarketOrder(trade_signal, orderSize_, 0);
		// makeMarketPerLevel(bookUpdates_[0].back().getAskPx()[0], abs(getOpenPosition(0)), trade_signal);
	    }
	    // else if (vol_signal < thresholdVol_ / 2) {
	    // 	if (orderbooks_[0].countLimitOrders(BuySellSide::buy) == 0) {
	    // 	    placeLimitOrder(Px(filledMessageLists_[0]->filledMessageList_.back().getPx().getPx() - 2 * tickSize_, tickSize_), BuySellSide::buy, abs(getOpenPosition(0)), 0);
	    // 	}
	    // }
	}

    }

}

// VUDS_CPP
