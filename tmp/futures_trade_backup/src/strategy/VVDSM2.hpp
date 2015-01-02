//
// File:        $ VVDSM2.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2014/01/18 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef VVDSM2_HPP
#define VVDSM2_HPP

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
#include "INIReader.hpp"

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
//using INIReader;

namespace simulator
{
    template<class TRADINGMODEL, class TRADINGMODELSHORT>
    class VVDSM2 : public Strategy
    {
    private:
	// signal related
	TRADINGMODEL tradingModel_;
	TRADINGMODELSHORT tradingModelShort_;

	// contract specification related
        const double multiplier_;
        const double tickSize_;
	
	string exchange_;
    
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
    
	double entryEdge_; // in deltaPrice space
	double exitEdge_;  // in deltaPrice space

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
	double signalShort_ = 0.0;
	double volatility_ = 0.0;

	time_stamp entryTime_;
	duration_nano timerLength_ = duration_nano(0ULL);

	std::string getStatusString();

	void placeAggressiveLimitOrder(BuySellSide side, int size, int prodid, int aggressiveness);
	void placeJoiningLimitOrder(BuySellSide side, int size, int prodid);

	Px getOrderPlacementPrice(BuySellSide side, bool isEntry);

    public:
	VVDSM2(string exchange, string contract, double multiple, double tickSize, string startTradingTimeString, string exitTradingTimeString, SimulatorCore * simcore_ptr, Logger * logger_ptr);
	VVDSM2(string exchange, double multiple, double tickSize);
	~VVDSM2();

	void getParamFromConfig(INIReader & iniconfig, string sectionName);

	void setTradingHours(string startTradingTimeString, string exitTradingTimeString);
	void setContract(string contract);

	void setStopLoss(double stoploss);
	void setTakeProfit(double takeprofit);
	void setThreshold(double threshold);
	void setThresholdLong(double thresholdLong);
	void setOrderSize(int ordersize);
	void setThresholdVol(double thresholdVol);
	
	void setEntryEdge(double entryEdge);
	void setExitEdge(double exitEdge);

        // inline bool tightQ();
	
	void process();
    };

    // template implementation
    template<class TRADINGMODEL, class TRADINGMODELSHORT>
    VVDSM2<TRADINGMODEL, TRADINGMODELSHORT>::VVDSM2(string exchange, string contract, double multiplier, double tickSize, string startTradingTimeString, string exitTradingTimeString, SimulatorCore * simcore_ptr, Logger * logger_ptr) :
	Strategy("VVDSM2", simcore_ptr, logger_ptr),
	multiplier_(multiplier),
	tickSize_(tickSize),
	startTradingTimeString_(startTradingTimeString),
	exitTradingTimeString_(exitTradingTimeString),
	/* TODO */
	volPtr_(new HighFrequencyVolatility<60,2>())
    {
	// add strat
	addStrat(exchange, contract, multiplier);

	setTradingHours(startTradingTimeString, exitTradingTimeString);

	tradingModel_.directionSignal_.registerThresholds(-1.0 * numeric_limits<double>::max(), numeric_limits<double>::max());
    }

    template<class TRADINGMODEL, class TRADINGMODELSHORT>
    VVDSM2<TRADINGMODEL, TRADINGMODELSHORT>::VVDSM2(string exchange, double multiplier, double ticksize) :
	Strategy("VVDSM2"),
	multiplier_(multiplier),
	tickSize_(ticksize),
	exchange_(exchange),
	volPtr_(new HighFrequencyVolatility<60,2>())
    { 
	tradingModel_.directionSignal_.registerThresholds(-1.0 * numeric_limits<double>::max(), numeric_limits<double>::max());
    }	

    template<class TRADINGMODEL, class TRADINGMODELSHORT>
    void VVDSM2<TRADINGMODEL, TRADINGMODELSHORT>::getParamFromConfig(INIReader & iniconfig, string sectionName)
    {
	string contract = iniconfig.Get(sectionName, "Contract", "");
	if(contract == "") {
	    std::cerr << "not valid contract" << std::endl;
	    throw;
	}
	setContract(contract);
	string startTime = iniconfig.Get(sectionName, "StartTime", "09:20:00");
	string exitTime = iniconfig.Get(sectionName, "ExitTime", "15:10:00");
	double stoploss = iniconfig.GetReal(sectionName, "StopLoss", -180);
	double takeprofit = iniconfig.GetReal(sectionName, "TakeProfit", 180);
	double threshold = iniconfig.GetReal(sectionName, "Threshold", 0.5);
	double thresholdLong = iniconfig.GetReal(sectionName, "ThresholdLong", 0.5);
	int ordersize = iniconfig.GetInteger(sectionName, "OrderSize", 1);
	double volthresh = iniconfig.GetReal(sectionName, "ThresholdVol", 0.25);
	double entryEdge = iniconfig.GetReal(sectionName, "EntryEdge", 0.1);
	double exitEdge = iniconfig.GetReal(sectionName, "ExitEdge", 0.0);
	std::cout << "trading parameters are" << "\n"
		  << "Contract = " << contract << "\n"
		  << "StartTime = " << startTime << "\n"
		  << "ExitTime = " << exitTime << "\n"
		  << "StopLoss = " << stoploss << "\n"
		  << "TakeProfit = " << takeprofit << "\n"
		  << "Threshold = " << threshold << "\n"
		  << "ThresholdLong = " << thresholdLong << "\n"
		  << "OrderSize = " << ordersize << "\n"
		  << "VolThresh = " << volthresh << "\n"
		  << "EntryEdge = " << entryEdge << "\n"
		  << "ExitEdge = " << exitEdge << std::endl;
	setTradingHours(startTime, exitTime);
	setStopLoss(stoploss);
	setTakeProfit(takeprofit);
	setThreshold(threshold);
	setThresholdLong(thresholdLong);
	setOrderSize(ordersize);
	setThresholdVol(volthresh);
	setEntryEdge(entryEdge);
	setExitEdge(exitEdge);
    }
    

    template<class TRADINGMODEL, class TRADINGMODELSHORT>
    void VVDSM2<TRADINGMODEL, TRADINGMODELSHORT>::setContract(string contract)
    {
	addStrat(exchange_, contract, multiplier_);
    }

    template<class TRADINGMODEL, class TRADINGMODELSHORT>
    void VVDSM2<TRADINGMODEL, TRADINGMODELSHORT>::setTradingHours(string startTradingTimeString, string exitTradingTimeString)
    {
	startTradingTimeString_ = startTradingTimeString;
	exitTradingTimeString_ = exitTradingTimeString;

	string datestr = SimulatorCore::convertTimePointToString(simulatorCorePtr_->getCurrTime()).substr(0, 10);
	stringstream ss1, ss2;
	ss1 << datestr << "T" << startTradingTimeString_ << ".000000000";
	startTradingTs_ = SimulatorCore::convertStringToTimePoint(ss1.str());
	ss2 << datestr << "T" << exitTradingTimeString_ << ".000000000";
	exitTradingTs_ = SimulatorCore::convertStringToTimePoint(ss2.str());
    }
    

    template<class TRADINGMODEL, class TRADINGMODELSHORT>
    void VVDSM2<TRADINGMODEL, TRADINGMODELSHORT>::setStopLoss(double stoploss)
    {
	stopLossOrderLimit_ = stoploss;
	cout << "Stop loss is set to " << stopLossOrderLimit_ << endl;
    }

    template<class TRADINGMODEL, class TRADINGMODELSHORT>
    void VVDSM2<TRADINGMODEL, TRADINGMODELSHORT>::setTakeProfit(double takeprofit)
    {
	takeProfitOrderLimit_ = takeprofit,
	cout << "Take profit is set to " << takeProfitOrderLimit_ << endl;
	takeProfitOrderPriceLimit_ = takeProfitOrderLimit_ / multiplier_;
	cout << "Points to take profit is set to " << takeProfitOrderPriceLimit_ << endl;
    }

    template<class TRADINGMODEL, class TRADINGMODELSHORT>
    void VVDSM2<TRADINGMODEL, TRADINGMODELSHORT>::setThreshold(double threshold)
    {
	cout << "Threshold is set to " << threshold << endl;
	tradingModelShort_.directionSignal_.registerThresholds(-1.0 * threshold, threshold);
    }


    template<class TRADINGMODEL, class TRADINGMODELSHORT>
    void VVDSM2<TRADINGMODEL, TRADINGMODELSHORT>::setThresholdLong(double threshold)
    {
	cout << "ThresholdLong is set to " << threshold << endl;
	tradingModel_.directionSignal_.registerThresholds(-1.0 * threshold, threshold);
    }

    template<class TRADINGMODEL, class TRADINGMODELSHORT>
    void VVDSM2<TRADINGMODEL, TRADINGMODELSHORT>::setThresholdVol(double thresholdVol)
    {
	thresholdVol_ = thresholdVol;
	cout << "Volatility threshold is set to " << thresholdVol_ << endl;
    }

    template<class TRADINGMODEL, class TRADINGMODELSHORT>
    void VVDSM2<TRADINGMODEL, TRADINGMODELSHORT>::setOrderSize(int ordersize)
    {
	orderSize_ = ordersize;
	cout << "Order size is set to " << orderSize_ << endl;
    }

    template<class TRADINGMODEL, class TRADINGMODELSHORT>
    void VVDSM2<TRADINGMODEL, TRADINGMODELSHORT>::setEntryEdge(double entryEdge)
    {
	entryEdge_ = entryEdge;
	cout << "Entry Edge is set to " << entryEdge_ << endl;
    }
    template<class TRADINGMODEL, class TRADINGMODELSHORT>
    void VVDSM2<TRADINGMODEL, TRADINGMODELSHORT>::setExitEdge(double exitEdge)
    {
	exitEdge_ = exitEdge;
	cout << "Exit Edge is set to " << exitEdge_ << endl;
    }

    template<class TRADINGMODEL, class TRADINGMODELSHORT>
    VVDSM2<TRADINGMODEL, TRADINGMODELSHORT>::~VVDSM2()
    {}
    
    template<class TRADINGMODEL, class TRADINGMODELSHORT>
    std::string VVDSM2<TRADINGMODEL, TRADINGMODELSHORT>::getStatusString()
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
	   << signalShort_ << ","
	   << volatility_ ;
	return ss.str();
    }

    template<class TRADINGMODEL, class TRADINGMODELSHORT>
    void VVDSM2<TRADINGMODEL, TRADINGMODELSHORT>::placeAggressiveLimitOrder(BuySellSide side, int size, int prodid, int aggressiveness)
    {
	double price_place = (side == BuySellSide::buy ? 
			      ask_ + aggressiveness * tickSize_ :
			      bid_ - aggressiveness * tickSize_);
	placeLimitOrder(Px(price_place, tickSize_), side, size, prodid);
    }
 
    template<class TRADINGMODEL, class TRADINGMODELSHORT>
    void VVDSM2<TRADINGMODEL, TRADINGMODELSHORT>::placeJoiningLimitOrder(BuySellSide side, int size, int prodid)
    {
	placeLimitOrder(side == BuySellSide::buy ? Px(bid_, tickSize_) : Px(ask_, tickSize_), side, size, prodid);
    }

    template<class TRADINGMODEL, class TRADINGMODELSHORT>
    Px VVDSM2<TRADINGMODEL, TRADINGMODELSHORT>::getOrderPlacementPrice(BuySellSide side, bool isEntry)
    {
	double edge = isEntry ? entryEdge_ : exitEdge_;
	//double price = 0.5 * (bid_ + ask_) + signalShort_ + (side == BuySellSide::buy ? -1.0 * edge : 1.0 * edge);
	double price = 0.5 * (bid_ + ask_) + (side == BuySellSide::buy ? -1.0 * edge : 1.0 * edge);
	// round to conservative tick
	if(side == BuySellSide::buy) {
	    return Px::round(price, tickSize_, -1);
	} else {
	    return Px::round(price, tickSize_, 1);
	}
    }

    template<class TRADINGMODEL, class TRADINGMODELSHORT>
    void VVDSM2<TRADINGMODEL, TRADINGMODELSHORT>::process()
    {
	logStrategyStatus(",BidQty,BidPx,AskPx,AskQty,MarketVolume,TradePx,Signal,Volatility",
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
		// spread_ = ibu->getAskPx()[0].getPx() - ibu->getBidPx()[0].getPx();
		tradingModel_.directionSignal_.update((*ibu));
		tradingModel_.directionSignal_.update((*itu));
		tradingModel_.directionSignal_.update((*ibu), (*itu));
		tradingModelShort_.directionSignal_.update((*ibu));
		tradingModelShort_.directionSignal_.update((*itu));
		tradingModelShort_.directionSignal_.update((*ibu), (*itu));
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

	BuySellSide trade_signal_long = tradingModel_.directionSignal_.getSignal();
	BuySellSide trade_signal = tradingModelShort_.directionSignal_.getSignal();
	signal_ = tradingModel_.directionSignal_.getRawValue();
	signalShort_ = tradingModelShort_.directionSignal_.getRawValue();
	//trade_signal = trade_signal_long != BuySellSide::unknown ? trade_signal : BuySellSide::unknown;
	trade_signal = trade_signal_long != BuySellSide::buy ? trade_signal : BuySellSide::unknown;
	double vol_signal = volPtr_->getValue();
	volatility_ = vol_signal;
	int open_position = getOpenPosition(0);
	BuySellSide side = (open_position > 0) ? BuySellSide::buy : (open_position < 0 ? BuySellSide::sell : BuySellSide::unknown);
	BuySellSide side_hedge = (open_position > 0) ? BuySellSide::sell : (open_position < 0 ? BuySellSide::buy : BuySellSide::unknown);

	// flat position
	if (open_position == 0) {
	    // if there are limit orders, cancel limit order
	    if (orderbooks_[0].countLimitOrders() > 0 && hasMDupdate && currTime_ >= entryTime_ + timerLength_) {
		cancelAllLimitOrders(0);
	    }
	    if ((vol_signal >= thresholdVol_) && (trade_signal == BuySellSide::buy || trade_signal == BuySellSide::sell) && hasMDupdate
		&& ask_ - bid_ <= 3 * tickSize_ + 1e-9) {
		placeLimitOrder(getOrderPlacementPrice(trade_signal, true), 
				trade_signal, orderSize_, 0);
		entryTime_ = currTime_;
	    }
	}
	// positive position
	else if (open_position > 0) {
	    cancelAllLimitOrders(side, 0);
	    // stop loss or reverse signal
	    if (hasMDupdate && 
		(trade_signal == side_hedge ||
		 getOpenPnL(0) < stopLossOrderLimit_ * abs(open_position))) {
		int hedge_qty = orderbooks_[0].countLimitOrderQty(side_hedge);
		Px target_px(getOrderPlacementPrice(side_hedge, false));
		if (hedge_qty > 0) {
		    if (orderbooks_[0].getAskLimitOrderBook()[0][0].getPx() > target_px) {
			cancelAllLimitOrders(side_hedge, 0);
			placeLimitOrder(target_px, side_hedge, abs(open_position), 0);
		    } else if (hedge_qty < abs(open_position)) {
			placeLimitOrder(target_px, side_hedge, abs(open_position) - hedge_qty, 0);
		    }
		} else {
		    placeLimitOrder(target_px, side_hedge, abs(open_position), 0);
		}
	    } else {
		int hedge_qty = orderbooks_[0].countLimitOrderQty(side_hedge);
		Px refPx(Px::round(filledMessageLists_[0]->calcVWAP(), tickSize_, (open_position > 0) ? -1 : 1));
		Px takeProfitPx(refPx.getPx() + takeProfitOrderPriceLimit_, tickSize_);
		if (hedge_qty > 0) {
		    if (hasMDupdate) {
			if (orderbooks_[0].getAskLimitOrderBook()[0][0].getPx() != takeProfitPx) {
			    cancelAllLimitOrders(side_hedge, 0);
			    placeLimitOrder(takeProfitPx, side_hedge, abs(open_position), 0);
			} else if (hedge_qty < abs(open_position)) {
			    placeLimitOrder(takeProfitPx, side_hedge, abs(open_position) - hedge_qty, 0);
			}
		    }
		} else {
		    placeLimitOrder(takeProfitPx, side_hedge, abs(open_position), 0);
		}
	    }
	}
	// negative position
	else {
	    cancelAllLimitOrders(side, 0);
	    // stop loss or reverse signal
	    if (hasMDupdate && 
		(trade_signal == side_hedge ||
		 getOpenPnL(0) < stopLossOrderLimit_ * abs(open_position))) {
		int hedge_qty = orderbooks_[0].countLimitOrderQty(side_hedge);
		Px target_px(getOrderPlacementPrice(side_hedge, false));
		if (hedge_qty > 0) {
		    if (orderbooks_[0].getBidLimitOrderBook()[0][0].getPx() < target_px) {
			cancelAllLimitOrders(side_hedge, 0);
			placeLimitOrder(target_px, side_hedge, abs(open_position), 0);
		    } else if (hedge_qty < abs(open_position)) {
			placeLimitOrder(target_px, side_hedge, abs(open_position) - hedge_qty, 0);
		    }
		} else {
		    placeLimitOrder(target_px, side_hedge, abs(open_position), 0);
		}
	    } else {
		int hedge_qty = orderbooks_[0].countLimitOrderQty(side_hedge);
		Px refPx(Px::round(filledMessageLists_[0]->calcVWAP(), tickSize_, (open_position > 0) ? -1 : 1));
		Px takeProfitPx(refPx.getPx() - takeProfitOrderPriceLimit_, tickSize_);
		if (hedge_qty > 0) {
		    if (hasMDupdate) {
			if (orderbooks_[0].getBidLimitOrderBook()[0][0].getPx() != takeProfitPx) {
			    cancelAllLimitOrders(side_hedge, 0);
			    placeLimitOrder(takeProfitPx, side_hedge, abs(open_position), 0);
			} else if (hedge_qty < abs(open_position)) {
			    placeLimitOrder(takeProfitPx, side_hedge, abs(open_position) - hedge_qty, 0);
			}
		    }
		} else {
		    placeLimitOrder(takeProfitPx, side_hedge, abs(open_position), 0);
		}
	    }
	}
    }

}

#endif  // VVDSM2_HPP
