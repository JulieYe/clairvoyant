//
// File:        $ TriggerStrategy.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/02/04 20:59:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef TRIGGERSTRATEGY_HPP
#define TRIGGERSTRATEGY_HPP

#include "Strategy.hpp"
#include "Signal.hpp"
#include "Feature.hpp"
#include "SimulatorCore.hpp"
#include "Logger.hpp"


using simulator::Signal;
using simulator::SignalOnPxPredict;
using simulator::Strategy;
using simulator::VolumeTrend;
using simulator::Volatility;
using simulator::SignedVolume;
using simulator::BookPressure;
using simulator::SimulatorCore;
using simulator::Logger;

namespace simulator
{
    class TriggerStrategy : public Strategy
    {
    private:
	//signal related
	SignalOnPxPredict<VolumeTrend<2, 20> > volumeSignal_;
	SignalOnPxPredict<Volatility<60, 250> > volatilitySignal_;
	SignalOnPxPredict<BookPressure, ShortTermReturn<2000>, TradeDeviation, SignedVolume<5000> , BookSizeChg<1000, static_cast<int>(BuySellSide::buy)>, BookSizeChg<1000, static_cast<int>(BuySellSide::sell)> > directionSignal_;
	
	//trading related
	int positionLimit_;
	int orderSize_;
	int stopLossOrderLimit_; // in RMB space
	int takeProfitOrderLimit_; // in RMB space
	double takeProfitOrderPriceLimit_; // in deltaPrice space
	double tickSize_;
	bool enableTrading_;
	bool closePosition_;
	
	string startTradingTimeString_;
	string exitTradingTimeString_;
	time_stamp startTradingTs_;
	time_stamp exitTradingTs_;

    public:
	TriggerStrategy(string exchange, string contract, double multiple, SimulatorCore * simcore_ptr, Logger * logger_ptr);
	~TriggerStrategy();
	
	void process();


    };




}






#endif
