//
// File:        $ VLDS.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/04/07 00:00:00 $
//
// ID:          $ mzhang (modifed from fei's VDS) $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef VLDS_HPP
#define VLDS_HPP

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
#include "IF.hpp"
#include "RB.hpp"
#include "RU.hpp"
//#include "IncludeHelper.hpp"

using simulator::Signal;
using simulator::SignalOnPxPredict;
using simulator::Strategy;
using simulator::BookSizeChg;
using simulator::BookSizeChg2;
using simulator::VwapMidReturn;
using simulator::TradeDeviationMA;
using simulator::VolumeTrend;
using simulator::Volatility;
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
    template<class TRADINGMODEL>
    class VLDS : public Strategy
    {
    private:
	//signal related
	TRADINGMODEL tradingModel_;

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
	int aggressivenessInTicks_ = 4;

	Px lastBid_;
	Px lastAsk_;
	int lastBidSize_;
	int lastAskSize_;

	WindowedBreakthrough<300> wbt_;

    public:
	VLDS(string exchange, string contract, double multiple, double tickSize, string startTradingTimeString, string exitTradingTimeString, SimulatorCore * simcore_ptr, Logger * logger_ptr);
	~VLDS();

	void setStopLoss(double stoploss);
	void setTakeProfit(double takeprofit);
	void setThreshold(double threshold);
	void setOrderSize(int ordersize);
	void setAggressivenessInTicks(int ticks);

        inline bool tightQ();
	
	void process();
    private:
	bool isBookGood(BookUpdate<true> &bu);
	bool isBookGood();
	void placeAggressiveLimitOrder(BuySellSide side, int size, int prodid);
	void placeJoiningLimitOrder(BuySellSide side, int size, int prodid);
	void logStrategyData();
    };

    // template implementations
    template< class TRADINGMODEL>
    VLDS<TRADINGMODEL>::VLDS(string exchange, string contract, double multiplier, double tickSize, string startTradingTimeString, string exitTradingTimeString, SimulatorCore * simcore_ptr, Logger * logger_ptr) :
	Strategy("VLDS", simcore_ptr, logger_ptr),
	multiplier_(multiplier),
	tickSize_(tickSize),
	startTradingTimeString_(startTradingTimeString),
	exitTradingTimeString_(exitTradingTimeString),
	wbt_()
    {
	// add strat
	addStrat(exchange, contract, multiplier);

	string datestr = SimulatorCore::convertTimePointToString(simcore_ptr->getCurrTime()).substr(0, 10);
	stringstream ss1, ss2;
	ss1 << datestr << "T" << startTradingTimeString_ << ".000000000";
	startTradingTs_ = SimulatorCore::convertStringToTimePoint(ss1.str());
	ss2 << datestr << "T" << exitTradingTimeString_ << ".000000000";
	exitTradingTs_ = SimulatorCore::convertStringToTimePoint(ss2.str());
	
	tradingModel_.directionSignal_.registerThresholds(-0.55, 0.55);

    }
    
    template< class TRADINGMODEL > 
    void VLDS<TRADINGMODEL>::setStopLoss(double stoploss)
    {
	stopLossOrderLimit_ = stoploss;
	cout << "Stop loss is set to " << stopLossOrderLimit_ << endl;
    }

    template< class TRADINGMODEL>
    void VLDS<TRADINGMODEL>::setTakeProfit(double takeprofit)
    {
	takeProfitOrderLimit_ = takeprofit,
	cout << "Take profit is set to " << takeProfitOrderLimit_ << endl;
	takeProfitOrderPriceLimit_ = takeProfitOrderLimit_ / multiplier_;
	cout << "Points to take profit is set to " << takeProfitOrderPriceLimit_ << endl;
    }

    template< class TRADINGMODEL>
    void VLDS<TRADINGMODEL>::setThreshold(double threshold)
    {
	cout << "Threshold is set to " << threshold << endl;
	tradingModel_.directionSignal_.registerThresholds(-1.0 * threshold, threshold);
    }

    template< class TRADINGMODEL>
    void VLDS<TRADINGMODEL>::setOrderSize(int ordersize)
    {
	orderSize_ = ordersize;
    }

    template< class TRADINGMODEL>
    void VLDS<TRADINGMODEL>::setAggressivenessInTicks(int ticks)
    {
	aggressivenessInTicks_ = ticks;
    }

    template< class TRADINGMODEL>
    bool VLDS<TRADINGMODEL>::tightQ()
    {
	//return spread_ <= 1.1 * tickSize_;
	return true;
    }

    template< class TRADINGMODEL>
    bool VLDS<TRADINGMODEL>::isBookGood(BookUpdate<true> &bu)
    {
	return ((bu.getAskPx())[0].getPx() > 0
		&& (bu.getBidPx())[0].getPx() > 0);
    }

    template< class TRADINGMODEL>
    bool VLDS<TRADINGMODEL>::isBookGood()
    {
	return (lastAsk_.getPx() > 0 && lastBid_.getPx() > 0 && lastAsk_ > lastBid_);
    }

    template< class TRADINGMODEL>
    void VLDS<TRADINGMODEL>::placeAggressiveLimitOrder(BuySellSide side, int size, int prodid)
    {
	double price_place = (side == BuySellSide::buy ? 
			      lastAsk_.getPx() + aggressivenessInTicks_ * tickSize_ :
			      lastBid_.getPx() - aggressivenessInTicks_ * tickSize_);
	placeLimitOrder(Px(price_place, tickSize_), side, size, prodid);
    }

    template< class TRADINGMODEL>
    void VLDS<TRADINGMODEL>::placeJoiningLimitOrder(BuySellSide side, int size, int prodid)
    {
	double price_place = (side == BuySellSide::buy ? 
			      lastBid_.getPx() : lastAsk_.getPx());
	placeLimitOrder(Px(price_place, tickSize_), side, size, prodid);
    }


    template< class TRADINGMODEL>
    VLDS<TRADINGMODEL>::~VLDS()
    {}
    
    template< class TRADINGMODEL>
    void VLDS<TRADINGMODEL>::process()
    {
	logStrategyStatus();

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
		//wbt_.update(*itu);
		if(isBookGood(*ibu)) {
		    lastBid_ = (*ibu).getBidPx()[0];
		    lastAsk_ = (*ibu).getAskPx()[0];
		    lastBidSize_ = (*ibu).getBidQty()[0];
		    lastAskSize_ = (*ibu).getAskQty()[0];
		}
		ibu++;
		itu++;
	    }
	    hasMDupdate = true;
	}

	if(getPnL() < -12000.0) {
	    closePosition_ = true;
	}

	if(!enableTrading_) {
	    return;
	}

	if (closePosition_) {
	    cancelAllLimitOrders(0);
	    if (getOpenPosition(0) > 0 && hasMDupdate && isBookGood()) {
		placeAggressiveLimitOrder(BuySellSide::sell, std::min(orderSize_, getOpenPosition(0)), 0);
	    } else if (getOpenPosition(0) < 0) {
		placeAggressiveLimitOrder(BuySellSide::buy, std::min(orderSize_, std::abs(getOpenPosition(0))), 0);
	    } 
	    return;
	}

	// we should have never sent market order. 
	// we should never have more than 2 limit orders out there
	if (pendingMarketOrderMessages_[0].size() > 0 || orderbooks_[0].countLimitOrders() > 2) {
	    return;
	}
	    
	BuySellSide trade_signal = tradingModel_.directionSignal_.getSignal();
	//trade_signal = std::abs(wbt_.getValue()) > 0 ? trade_signal : BuySellSide::unknown;
	
	logStrategyData();
	
	// if zero position
	if (getOpenPosition(0) == 0) {
	    // if flat and no market data update. do nothing
	    if (!hasMDupdate) {
		return;
	    }
	    // if there are limit orders, cancel limit order
	    // if (orderbooks_[0].countLimitOrders() > 0) {
	    // 	cancelAllLimitOrders(0);
	    // }
	    if (trade_signal == BuySellSide::buy) {
		cancelAllLimitOrders(BuySellSide::sell, 0);
	    }
	    if (trade_signal == BuySellSide::sell) {
		cancelAllLimitOrders(BuySellSide::buy, 0);
	    }
	    // check signal and send limit order to open position
	    if ((trade_signal == BuySellSide::buy || trade_signal == BuySellSide::sell) 
		&& tightQ() 
		&& hasMDupdate
		&& (bookUpdates_[0].back().getAskPx())[0].getPx() > 0
		&& (bookUpdates_[0].back().getBidPx())[0].getPx() > 0) {
		//placeAggressiveLimitOrder(trade_signal, orderSize_, 0);
		cancelAllLimitOrders(0);
		placeJoiningLimitOrder(trade_signal, orderSize_, 0);
	    }
	    return;
	}
	
	if (getOpenPosition(0) > 0) {
	    if (orderbooks_[0].countLimitOrders(BuySellSide::buy) > 0) {
		cancelAllLimitOrders(BuySellSide::buy, 0);
	    } 
	    if (orderbooks_[0].countLimitOrders(BuySellSide::sell) == 0) {
		placeLimitOrder(Px(filledMessageLists_[0]->filledMessageList_.back().getPx().getPx() + takeProfitOrderPriceLimit_, tickSize_), BuySellSide::sell, orderSize_, 0);
	    } else {
		if ((getOpenPnL(0) < stopLossOrderLimit_) && tightQ()) {
		    placeAggressiveLimitOrder(BuySellSide::sell, orderSize_, 0);
		}
		else if ((trade_signal == BuySellSide::sell) && tightQ() && hasMDupdate) {
		    //placeAggressiveLimitOrder(trade_signal, orderSize_, 0);
		    cancelAllLimitOrders(0);
		    placeJoiningLimitOrder(trade_signal, orderSize_, 0);
		} else if((trade_signal == BuySellSide::buy && hasMDupdate)) {
		    cancelAllLimitOrders(BuySellSide::sell, 0);
		    placeLimitOrder(Px(lastAsk_.getPx() + takeProfitOrderPriceLimit_, tickSize_), BuySellSide::sell, orderSize_, 0);
		}
	    }
	    return;
	}
	
	if (getOpenPosition(0) < 0) {
	    if (orderbooks_[0].countLimitOrders(BuySellSide::sell) > 0) {
		cancelAllLimitOrders(BuySellSide::sell, 0);
	    } 
	    if (orderbooks_[0].countLimitOrders(BuySellSide::buy) == 0) {
		placeLimitOrder(Px(filledMessageLists_[0]->filledMessageList_.back().getPx().getPx() - takeProfitOrderPriceLimit_, tickSize_), BuySellSide::buy, orderSize_, 0);
	    } else {
		if ((getOpenPnL(0) < stopLossOrderLimit_) && tightQ()) {
		    placeAggressiveLimitOrder(BuySellSide::buy, orderSize_, 0);
		}
		else if ((trade_signal == BuySellSide::buy) && tightQ() && hasMDupdate) {
		    //placeAggressiveLimitOrder(trade_signal, orderSize_, 0);
		    cancelAllLimitOrders(0);
		    placeJoiningLimitOrder(trade_signal, orderSize_, 0);
		} else if((trade_signal == BuySellSide::sell) && hasMDupdate) {
		    cancelAllLimitOrders(BuySellSide::buy, 0);
		    placeLimitOrder(Px(lastBid_.getPx() - takeProfitOrderPriceLimit_, tickSize_), BuySellSide::buy, orderSize_, 0);
		}
	    }
	    return;

	}

    }

    template<class TRADINGMODEL>
    void VLDS<TRADINGMODEL>::logStrategyData()
    {
	stringstream ss;
	ss << "StrategyData" << ","
	   << SimulatorCore::convertTimePointToString(currTime_) << ","
	   << currTime_.time_since_epoch().count() << ","
	   << lastBidSize_ << ","
	   << lastBid_ << ","
	   << lastAsk_ << ","
	   << lastAskSize_ << ","
	   << tradingModel_.directionSignal_.getRawValue();
	loggerPtr_->push(ss.str());

    }
    


}

#endif  // VLDS_HPP
