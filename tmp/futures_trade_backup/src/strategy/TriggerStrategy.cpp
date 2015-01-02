//
// File:        $ TriggerStrategy.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/02/09 14:39:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include "TriggerStrategy.hpp"
#include <sstream>

using std::string;
using std::stringstream;


namespace simulator
{

    TriggerStrategy::TriggerStrategy(string exchange, string contract, double multiple, SimulatorCore * simcore_ptr, Logger * logger_ptr) :
	Strategy("TriggerStrategy", simcore_ptr, logger_ptr),
	positionLimit_(1),
	orderSize_(1),
	stopLossOrderLimit_(-120.0),
	takeProfitOrderLimit_(120.0),
	tickSize_(0.2),
	enableTrading_(false),
	closePosition_(false),
	startTradingTimeString_("09:30:00"),
	exitTradingTimeString_("14:30:00")
    {
	// add strat
	addStrat(exchange, contract, multiple);

	takeProfitOrderPriceLimit_ = takeProfitOrderLimit_ / multiple;

	string datestr = SimulatorCore::convertTimePointToString(simcore_ptr->getCurrTime()).substr(0, 10);
	stringstream ss1, ss2;
	ss1 << datestr << "T" << startTradingTimeString_ << ".000000000";
	startTradingTs_ = SimulatorCore::convertStringToTimePoint(ss1.str());
	ss2 << datestr << "T" << exitTradingTimeString_ << ".000000000";
	exitTradingTs_ = SimulatorCore::convertStringToTimePoint(ss2.str());
	    

	// initialize signals
	volumeSignal_.registerFeature(PFeature(new VolumeTrend<2, 20>()));
	volumeSignal_.registerBeta(1.0);
	volumeSignal_.registerThresholds(0.0, 0.0);

	volatilitySignal_.registerFeature(PFeature(new Volatility<60, 250000>()));
	volatilitySignal_.registerBeta(1.0);
	volatilitySignal_.registerThresholds(0.0, 0.0);
	
	directionSignal_.registerFeature(PFeature(new BookPressure()),
					 PFeature(new ShortTermReturn<2000>()),
					 PFeature(new TradeDeviation()),
					 PFeature(new SignedVolume<5000>()),
					 PFeature(new BookSizeChg<1000, static_cast<int>(BuySellSide::buy)>()),
	    PFeature(new BookSizeChg<1000, static_cast<int>(BuySellSide::sell)>()));

	directionSignal_.registerBeta(8.60283523e-05,
				      1.82546012e-01,
				      1.24432697e-01,
				      -5.40213197e-08,
				      2.21941327e-07,
				      -2.48407700e-07);
	directionSignal_.registerThresholds(-1e-04, 1e-04);

    }

    TriggerStrategy::~TriggerStrategy()
    {

    }
    
    void TriggerStrategy::process()
    {
	logStrategyStatus();


	// set enable trading and set close position
	if(currTime_ > startTradingTs_) {
	    enableTrading_ = true;
	}
	if(currTime_ > exitTradingTs_) {
	    closePosition_ = true;
	}
	
	if(bookUpdates_[0].size() > 0 && tradeUpdates_[0].size() > 0) {
	    auto ibu = bookUpdates_[0].begin();
	    auto itu = tradeUpdates_[0].begin();
	    while(ibu != bookUpdates_[0].end() && itu != tradeUpdates_[0].end()) {
		volumeSignal_.update((*ibu));
		volumeSignal_.update((*itu));
		volumeSignal_.update((*ibu), (*itu));
		volatilitySignal_.update((*ibu));
		volatilitySignal_.update((*itu));
		volatilitySignal_.update((*ibu), (*itu));
		directionSignal_.update((*ibu));
		directionSignal_.update((*itu));
		directionSignal_.update((*ibu), (*itu));
		ibu++;
		itu++;
	    }
	}
	
	if(!enableTrading_) {
	    return;
	}

	if(closePosition_) {
	    cancelAllLimitOrders(0);
	    if(getOpenPosition() > 0) {
		if(pendingMarketOrderMessages_[0].size() > 0) {
		    if(pendingMarketOrderMessages_[0].front().getSide() != BuySellSide::sell) {
			cancelMarketOrder(pendingMarketOrderMessages_[0].front());
			placeMarketOrder(BuySellSide::sell, orderSize_, 0);
		    }
		} else {
		    placeMarketOrder(BuySellSide::sell, orderSize_, 0);
		}
	    } else if (getOpenPosition() < 0) {
		if(pendingMarketOrderMessages_[0].size() > 0) {
		    if(pendingMarketOrderMessages_[0].front().getSide() != BuySellSide::buy) {
			cancelMarketOrder(pendingMarketOrderMessages_[0].front());
			placeMarketOrder(BuySellSide::buy, orderSize_, 0);
		    }
		} else {
		    placeMarketOrder(BuySellSide::buy, orderSize_, 0);
		}
	    } 
	    return;
	}

	    
	BuySellSide trade_signal = BuySellSide::unknown;
	if(volumeSignal_.getSignal() == BuySellSide::buy &&
	   volatilitySignal_.getSignal() == BuySellSide::buy) {
	    trade_signal = directionSignal_.getSignal() ;
	}
	
	// if zero position
	if(getOpenPosition() == 0) {
	    // if there are limit orders, cancel limit order
	    if(orderbooks_[0].countLimitOrders() > 0) {
		cancelAllLimitOrders(0);
	    }
	    // if pending; check signal and wait or cancel and resend
	    if(pendingMarketOrderMessages_[0].size() > 0) {
		if(pendingMarketOrderMessages_[0].front().getSide() != trade_signal) {
		    // cancel Order
		    cancelMarketOrder(pendingMarketOrderMessages_[0].front());
		    if(trade_signal == BuySellSide::buy || trade_signal == BuySellSide::sell) {
			// send market Order
			placeMarketOrder(trade_signal, orderSize_, 0);
		    }
		}
	    } else {
		// send orders
		if(trade_signal == BuySellSide::buy || trade_signal == BuySellSide::sell) {
		    // send market Order
		    placeMarketOrder(trade_signal, orderSize_, 0);
		}
	    }
	    return;
	}
	
	if(getOpenPosition() > 0) {
	    if(orderbooks_[0].countLimitOrders(BuySellSide::buy) > 0) {
		cancelAllLimitOrders(BuySellSide::buy, 0);
	    } 
	    if(orderbooks_[0].countLimitOrders(BuySellSide::sell) == 0) {
		placeLimitOrder(Px(filledMessageLists_[0]->filledMessageList_.back().getPx().getPx() + takeProfitOrderPriceLimit_, tickSize_), BuySellSide::sell, orderSize_, 0);
	    }
	    if(pendingMarketOrderMessages_[0].size() > 0) {
		if(pendingMarketOrderMessages_[0].front().getSide() == BuySellSide::buy) {
		    // cancel Order
		    cancelMarketOrder(pendingMarketOrderMessages_[0].front());
		} 
		if (trade_signal == BuySellSide::sell) {
		    // send market Order
		    placeMarketOrder(trade_signal, orderSize_, 0);
		}
	    } else {
		if(getOpenPnL() < stopLossOrderLimit_) {
		    placeMarketOrder(BuySellSide::sell, orderSize_,  0);
		}
	    }
	    return;
	}
	
	if(getOpenPosition() < 0) {
	    if(orderbooks_[0].countLimitOrders(BuySellSide::sell) > 0) {
		cancelAllLimitOrders(BuySellSide::sell, 0);
	    } 
	    if(orderbooks_[0].countLimitOrders(BuySellSide::buy) == 0) {
		placeLimitOrder(Px(filledMessageLists_[0]->filledMessageList_.back().getPx().getPx() - takeProfitOrderPriceLimit_, tickSize_), BuySellSide::buy, orderSize_, 0);
	    }
	    if(pendingMarketOrderMessages_[0].size() > 0) {
		if(pendingMarketOrderMessages_[0].front().getSide() == BuySellSide::sell) {
		    // cancel Order
		    cancelMarketOrder(pendingMarketOrderMessages_[0].front());
		} 
		if (trade_signal == BuySellSide::buy) {
		    // send market Order
		    placeMarketOrder(trade_signal, orderSize_, 0);
		}
	    } else {
		if(getOpenPnL() < stopLossOrderLimit_) {
		    placeMarketOrder(BuySellSide::buy, orderSize_, 0);
		}
	    }
	    return;

	}

    }



    
}
