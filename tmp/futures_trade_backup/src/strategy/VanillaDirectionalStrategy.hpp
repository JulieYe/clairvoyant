//
// File:        $ VanillaDirectionalStrategy.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/02/13 00:00:00 $
//
// ID:          $ fye $
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
using simulator::BookSizeChg;

namespace simulator
{
    class VanillaDirectionalStrategy : public Strategy
    {
    private:
	//signal related
	SignalOnPxPredict<BookPressure, BookSizeChg<500, static_cast<int>(BuySellSide::buy)>, BookSizeChg<500, static_cast<int>(BuySellSide::sell)> > directionSignal_;
	
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
	VanillaDirectionalStrategy(string exchange, string contract, double multiple, SimulatorCore * simcore_ptr, Logger * logger_ptr);
	~VanillaDirectionalStrategy();
	
	void process();


    };




}






#endif
