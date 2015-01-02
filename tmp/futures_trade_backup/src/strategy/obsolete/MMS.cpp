//
// File:        $ MMS.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2014/01/01 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include "MMS.hpp"

namespace simulator
{

    MMS::MMS(string exchange, string contract, double multiplier, double tickSize, string startTradingTimeString, string exitTradingTimeString, SimulatorCore * simcore_ptr, Logger * logger_ptr) :
	Strategy("MMS", simcore_ptr, logger_ptr),
	multiplier_(multiplier),
	tickSize_(tickSize),
	startTradingTimeString_(startTradingTimeString),
	exitTradingTimeString_(exitTradingTimeString),
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

    void MMS::setStopLoss(double stoploss)
    {
	stopLossOrderLimit_ = stoploss;
	cout << "Stop loss is set to " << stopLossOrderLimit_ << endl;
    }

    void MMS::setTakeProfit(double takeprofit)
    {
	takeProfitOrderLimit_ = takeprofit,
	cout << "Take profit is set to " << takeProfitOrderLimit_ << endl;
	takeProfitOrderPriceLimit_ = takeProfitOrderLimit_ / multiplier_;
	cout << "Points to take profit is set to " << takeProfitOrderPriceLimit_ << endl;
    }

    void MMS::setThreshold()
    {
	thresholdLevelW_ = 0.25;
	thresholdLevelS_ = 0.29;
	thresholdLevelE_ = 0.33;
	cout << "Threshold (weak) is set to " << thresholdLevelW_ << endl;
	tradingModel_.directionSignal_.registerThresholds(-1.0 * thresholdLevelW_, thresholdLevelW_);
	cout << "Threshold (strong) is set to " << thresholdLevelS_ << endl;
	cout << "Threshold (exit) is set to " << thresholdLevelE_ << endl;
    }

    void MMS::setThresholdVol()
    {
	thresholdVolW_ = 0.09;
	thresholdVolS_ = 0.115;
	thresholdVolE_ = 0.14;
	cout << "Volatility threshold (weak) is set to " << thresholdVolW_ << endl;
	cout << "Volatility threshold (strong) is set to " << thresholdVolS_ << endl;
	cout << "Volatility threshold (exit) is set to " << thresholdVolE_ << endl;
    }

    void MMS::setOrderSize(int ordersize)
    {
	orderSize_ = ordersize;
	cout << "Order size is set to " << orderSize_ << endl;
    }

    void MMS::setBandwidth(double bandwidth)
    {
	bandwidth_ = bandwidth / multiplier_;
	cout << "Posting bandwidth is set to " << bandwidth_ << endl;
    }

    // bool MMS::tightQ()
    // {
    // 	return spread_ <= 1.1 * tickSize_;
    // }

    bool MMS::isBookGood(BookUpdate<true> &bu)
    {
	return ((bu.getAskPx())[0].getPx() > 1e-6
		&& (bu.getBidPx())[0].getPx() > 1e-6);
    }

    string MMS::getStatusString()
    {
	stringstream ss;
	ss << ","
	   << topBidQty_ << ","
	   << topBidPx_ << ","
	   << topAskPx_ << ","
	   << topAskQty_ << ","
	   << marketVolume_ << ","
	   << tradePx_ << ","
	   << signal_ << "," 
	   << vol_;
	// << sweepBidPtr_->getValue() << "," 
	// << sweepAskPtr_->getValue();
	return ss.str();
    }

    MMS::~MMS()
    {}
    
    void MMS::process()
    {
	/*
	logStrategyStatus(",BidQty,BidPx,AskPx,AskQty,MarketVolume,LastTradePx,Signal,Vol,SweepBid,SweepAsk", 
			  getStatusString());
	*/
	logStrategyStatus(",BidQty,BidPx,AskPx,AskQty,MarketVolume,LastTradePx,Signal,Vol", 
			  getStatusString());

	bool hasMDupdate = false;

	// set enable trading and set close position
	if (currTime_ > startTradingTs_) {
	    enableTrading_ = true;
	}
	if (currTime_ > exitTradingTs_) {
	    closePosition_ = true;
	}

#ifdef ACCOUNT_ARBITRAGER
	bool cancelCountQ = false;
#else 
	bool cancelCountQ(getCancelCount(0) > 500);
#endif
	if (cancelCountQ || getPnL() < -3000.0 * orderSize_) {
	    closePosition_ = true;
	}

	if (!bookUpdates_[0].empty() && !tradeUpdates_[0].empty()) {
	    auto ibu = bookUpdates_[0].begin();
	    auto itu = tradeUpdates_[0].begin();
	    while (ibu != bookUpdates_[0].end() && 
		   itu != tradeUpdates_[0].end()) {
		if (isBookGood((*ibu))) {
		    tradingModel_.directionSignal_.update((*ibu));
		    tradingModel_.directionSignal_.update((*itu));
		    tradingModel_.directionSignal_.update((*ibu), (*itu));
		    volPtr_->update(*ibu);
		    topBidPx_ = ibu->getBidPx()[0].getPx();
		    topAskPx_ = ibu->getAskPx()[0].getPx();
		    topBidQty_ = ibu->getBidQty()[0];
		    topAskQty_ = ibu->getAskQty()[0];
		    spread_ = topAskPx_ - topBidPx_;
		    // volPtr_->update(bookUpdates_[0].back());
		    marketVolume_ = itu->getQty();
		    tradePx_ = itu->getPx().getPx();
		    // sweepBidPtr_->update((*ibu));
		    // sweepAskPtr_->update((*ibu));
		    hasMDupdate = true;
		}
		ibu++;
		itu++;
	    }
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

	// BuySellSide trade_signal = tradingModel_.directionSignal_.getSignal();
	signal_ = tradingModel_.directionSignal_.getRawValue();
	vol_ = volPtr_->getValue();
	int open_position = getOpenPosition(0);
	// double vol_signal = volPtr_->getValue();
	// double sweep_bid_signal = sweepBidPtr_->getValue();
	// double sweep_ask_signal = sweepAskPtr_->getValue();
	// if zero position
	if (open_position == 0) {
	    // make market around best bid/ask price levels using given bandwidth
	    /*
	    if (hasMDupdate && (vol_signal <= thresholdVol_) && pendingCancelLimitOrderMessages_[0].empty()) {
		int roundUpDown = (topBidQty_ > topAskQty_) ? 1 : (-1);
		makeMarketPerLevel(Px::round(midPx_ - bandwidth_ / 2.0, tickSize_, roundUpDown), orderSize_, BuySellSide::buy);
		makeMarketPerLevel(Px::round(midPx_ + bandwidth_ / 2.0, tickSize_, roundUpDown), orderSize_, BuySellSide::sell);
	    } else if (pendingCancelLimitOrderMessages_[0].empty()) {
		cancelAllLimitOrders(0);
	    }
	    */
	    if (hasMDupdate || winner_) {
		winner_ = false;
		if (signal_ <= -thresholdLevelS_ && vol_ >= thresholdVolS_) {
		    if (orderbooks_[0].countLimitOrders(BuySellSide::buy) > 0) {
			cancelAllLimitOrders(BuySellSide::buy, 0);
		    }
		    Px targetPx(topBidPx_, tickSize_);  // most aggressive
		    if (orderbooks_[0].countLimitOrders(BuySellSide::sell) > 0) {
			if (orderbooks_[0].getAskLimitOrderBook()[0][0].getPx() > targetPx) {
			    cancelAllLimitOrders(BuySellSide::sell, 0);
			    placeLimitOrder(targetPx, BuySellSide::sell, orderSize_, 0);
			}
		    } else {			    
			placeLimitOrder(targetPx, BuySellSide::sell, orderSize_, 0);
		    }
		} else if (signal_ >= thresholdLevelS_ && vol_ >= thresholdVolS_) {
		    if (orderbooks_[0].countLimitOrders(BuySellSide::sell) > 0) {
			cancelAllLimitOrders(BuySellSide::sell, 0);
		    }
		    Px targetPx(topAskPx_, tickSize_);  // most aggressive
		    if (orderbooks_[0].countLimitOrders(BuySellSide::buy) > 0) {
			if (orderbooks_[0].getBidLimitOrderBook()[0][0].getPx() < targetPx) {
			    cancelAllLimitOrders(BuySellSide::buy, 0);
			    placeLimitOrder(targetPx, BuySellSide::buy, orderSize_, 0);
			}
		    } else {			    
			placeLimitOrder(targetPx, BuySellSide::buy, orderSize_, 0);
		    }
		// hard-coded number 2
		//} else if ((signal_ <= -thresholdLevel1_ && vol_ >= thresholdVol_) || (sweep_bid_signal >=2 && trade_signal == BuySellSide::sell)) {
		} else if (signal_ <= -thresholdLevelW_ && vol_ >= thresholdVolW_) {
		    // makeMarketPerLevel(Px(topBidPx_ - bandwidth_, tickSize_), orderSize_, BuySellSide::buy);
		    // Test 1 lot trade.
		    if (orderbooks_[0].countLimitOrders(BuySellSide::buy) > 0) {
			cancelAllLimitOrders(BuySellSide::buy, 0);
		    }
		    // Px targetPx(topAskPx_ - bandwidth_, tickSize_);  // more conservative
		    Px targetPx(topBidPx_ + bandwidth_, tickSize_);  // more aggressive
		    if (orderbooks_[0].countLimitOrders(BuySellSide::sell) > 0) {
			if (orderbooks_[0].getAskLimitOrderBook()[0][0].getPx() > targetPx) {
			    cancelAllLimitOrders(BuySellSide::sell, 0);
			    placeLimitOrder(targetPx, BuySellSide::sell, orderSize_, 0);
			}
		    } else {			    
			placeLimitOrder(targetPx, BuySellSide::sell, orderSize_, 0);
		    }
		// hard-coded number 2
		//} else if ((signal_ >= thresholdLevel1_ && vol_ >= thresholdVol_) || (sweep_ask_signal >= 2 && trade_signal == BuySellSide::buy)) {
		} else if (signal_ >= thresholdLevelW_ && vol_ >= thresholdVolW_) {
		    // makeMarketPerLevel(Px(topAskPx_ + bandwidth_, tickSize_), orderSize_, BuySellSide::sell);
		    if (orderbooks_[0].countLimitOrders(BuySellSide::sell) > 0) {
			cancelAllLimitOrders(BuySellSide::sell, 0);
		    }
		    // Px targetPx(topBidPx_ + bandwidth_, tickSize_);  // more conservative
		    Px targetPx(topAskPx_ - bandwidth_, tickSize_);  // more aggressive
		    if (orderbooks_[0].countLimitOrders(BuySellSide::buy) > 0) {
			if (orderbooks_[0].getBidLimitOrderBook()[0][0].getPx() < targetPx) {
			    cancelAllLimitOrders(BuySellSide::buy, 0);
			    placeLimitOrder(targetPx, BuySellSide::buy, orderSize_, 0);
			}
		    } else {			    
			placeLimitOrder(targetPx, BuySellSide::buy, orderSize_, 0);
		    }
		} else {
		    cancelAllLimitOrders(0);
		}
	    }
	}
	else if (open_position > 0) {
	    // if stop-lossed
	    if (getOpenPnL(0) < stopLossOrderLimit_ * abs(open_position) ||
		(signal_ < -thresholdLevelE_ && vol_ > thresholdVolE_)) {
		if (hasMDupdate) {
		    Px targetPx(topAskPx_, tickSize_);
		    if (orderbooks_[0].countLimitOrders(BuySellSide::sell) == 0 || orderbooks_[0].getAskLimitOrderBook()[0][0].getPx() > targetPx) {
			cancelAllLimitOrders(0);
			placeLimitOrder(targetPx, BuySellSide::sell, abs(open_position), 0);
			winner_ = false;
		    }
		}
	    }
	    // compute the appropriate price level for hedging the existing position
	    else if (orderbooks_[0].countLimitOrders(BuySellSide::sell) == 0) {
		// makeMarketPerLevel(Px::round(filledMessageLists_[0]->calcVWAP() + takeProfitOrderPriceLimit_, tickSize_, (open_position > 0) ? -1 : 1), abs(open_position), BuySellSide::sell);
		placeLimitOrder(Px::round(filledMessageLists_[0]->calcVWAP() + takeProfitOrderPriceLimit_, tickSize_, 1), BuySellSide::sell, abs(open_position), 0);
		winner_ = true;
	    }
	}
	else {
	    // if stop-lossed
	    if (getOpenPnL(0) < stopLossOrderLimit_ * abs(open_position) || 
		(signal_ > thresholdLevelE_ && vol_ > thresholdLevelE_)) {
		if (hasMDupdate) {
		    Px targetPx(topBidPx_, tickSize_);
		    if (orderbooks_[0].countLimitOrders(BuySellSide::buy) == 0 || orderbooks_[0].getBidLimitOrderBook()[0][0].getPx() < targetPx) {
			cancelAllLimitOrders(0);
			placeLimitOrder(targetPx, BuySellSide::buy, abs(open_position), 0);
			winner_ = false;
		    }
		}
	    }
	    // compute the appropriate price level for hedging the existing position
	    else if (orderbooks_[0].countLimitOrders(BuySellSide::buy) == 0) {
		// makeMarketPerLevel(Px::round(filledMessageLists_[0]->calcVWAP() - takeProfitOrderPriceLimit_, tickSize_, (open_position > 0) ? -1 : 1), abs(open_position), BuySellSide::buy);
		placeLimitOrder(Px::round(filledMessageLists_[0]->calcVWAP() - takeProfitOrderPriceLimit_, tickSize_, -1), BuySellSide::buy, abs(open_position), 0);
		winner_ = true;
	    }
	}
    }

    /*
    void MMS::setThresholdVol(double thresholdVol)
    {
	thresholdVol_ = thresholdVol;
	cout << "Volatility threshold is set to " << thresholdVol_ << endl;
    }

    void MMS::makeMarketPerLevel(Px px, int workingQty, BuySellSide side)
    {
	if (side == BuySellSide::buy) {
	    makeBidMarketPerLevel(px, workingQty);
	} else if (side == BuySellSide::sell) {
	    makeAskMarketPerLevel(px, workingQty);
	}
     }

    void MMS::makeBidMarketPerLevel(Px px, int workingQty)
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

    void MMS::makeAskMarketPerLevel(Px px, int workingQty)
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
    */

}

// MMS_CPP
