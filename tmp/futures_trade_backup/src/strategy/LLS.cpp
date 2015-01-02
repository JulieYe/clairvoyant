//
// File:        $ LLS.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/06/12 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include "LLS.hpp"

#define TOL 1e-6

namespace simulator {

    LLS::LLS(string exchange, string contract, double multiple, double tickSize,
	     string exchange_lead, string contract_lead, double tickSize_lead,
	     string startTradingTimeString, string exitTradingTimeString, 
	     SimulatorCore * simcore_ptr, Logger * logger_ptr) :
	Strategy("LLS", simcore_ptr, logger_ptr),
	multiplier_(multiple),
	tickSize_(tickSize),
	leadTickSize_(tickSize_lead),
	exchange_(exchange),
	exchangeLead_(exchange_lead),
	startTradingTimeString_(startTradingTimeString),
	exitTradingTimeString_(exitTradingTimeString),
	lastBid_(Px(0.0, tickSize)),
	lastAsk_(Px(0.0, tickSize)),
	lastBidSize_(0),
	lastAskSize_(0),
	previousLeadMid_(0.0),
	previousTradeMid_(0.0),
	currentLeadMid_(0.0),
	currentTradeMid_(0.0)
    {
	// trading strat
	addStrat(exchange, contract, multiple);
	// leading contract for signal 
	addStrat(exchange_lead, contract_lead, 1.0);
	
	setTradingHours(startTradingTimeString, exitTradingTimeString);	
    }
    
    LLS::LLS(string exchange, double multiple, double tickSize,
	string exchange_lead, double tickSize_lead) :
	Strategy("LLS"),
	multiplier_(multiple),
	tickSize_(tickSize),
	leadTickSize_(tickSize_lead),
	exchange_(exchange),
	exchangeLead_(exchange_lead),
	lastBid_(Px(0.0, tickSize)),
	lastAsk_(Px(0.0, tickSize)),
	lastBidSize_(0),
	lastAskSize_(0),
	previousLeadMid_(0.0),
	previousTradeMid_(0.0),
	currentLeadMid_(0.0),
	currentTradeMid_(0.0)
    { }	
    
    void LLS::setContracts(string tradeContract, string leadContract)
    {
	// trading strat
	addStrat(exchange_, tradeContract, multiplier_);
	// leading contract for signal 
	addStrat(exchangeLead_, leadContract, 1.0);
    }
    
    void LLS::setTradingHours(string startTradingTimeString, string exitTradingTimeString)
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

    void LLS::getParamFromConfig(INIReader &iniconfig, string sectionName)
    {
	string tradeContract = iniconfig.Get(sectionName, "TradeContract", "");
	string leadContract = iniconfig.Get(sectionName, "LeadContract", "");
	if(tradeContract == "" || leadContract == "") {
	    std::cerr << "invalide contracts" << std::endl;
	    throw;
	}
	setContracts(tradeContract, leadContract);
	string startTime = iniconfig.Get(sectionName, "StartTime", "09:20:00");
	string exitTime = iniconfig.Get(sectionName, "ExitTime", "15:10:00");
	double stoploss = iniconfig.GetReal(sectionName, "StopLoss", -180);
	double takeprofit = iniconfig.GetReal(sectionName, "TakeProfit", 180);
	int leadThresh = iniconfig.GetInteger(sectionName, "LeadThresh", 5);
	int tradeThresh = iniconfig.GetInteger(sectionName, "TradeThresh", 5);
	int ordersize = iniconfig.GetInteger(sectionName, "OrderSize", 1);
	int ticks = iniconfig.GetInteger(sectionName, "Aggressive", 3);
	int nsnapshots= iniconfig.GetInteger(sectionName, "NSnapshots", 1);
	std::cout << "trading parameters are" << "\n"
		  << "TradeContract = " << tradeContract << "\n"
		  << "LeadContrade = " << leadContract << "\n"
		  << "StartTime = " << startTime << "\n"
		  << "ExitTime = " << exitTime << "\n"
		  << "StopLoss = " << stoploss << "\n"
		  << "TakeProfit = " << takeprofit << "\n"
		  << "LeadThresh = " << leadThresh << "\n"
		  << "TradeThresh = " << tradeThresh << "\n"
		  << "OrderSize = " << ordersize << "\n"
		  << "Aggressive = " << ticks << "\n"
		  << "NSnapshots = " << nsnapshots << std::endl;
	setTradingHours(startTime, exitTime);
	setStopLoss(stoploss);
	setTakeProfit(takeprofit);
	setThreshold(leadThresh, tradeThresh);
	setOrderSize(ordersize);
	setAggressivenessInTicks(ticks);
	setNSnapshots(nsnapshots);
    }
    

    bool LLS::isBookGood(BookUpdate<true> &bu)
    {
	return ((bu.getAskPx())[0].getPx() > 0
		&& (bu.getBidPx())[0].getPx() > 0);
    }

    bool LLS::isBookGood()
    {
	return (lastAsk_.getPx() > 0 && lastBid_.getPx() > 0 && lastAsk_ > lastBid_);
    }
    
    void LLS::placeAggressiveLimitOrder(BuySellSide side, int size, int prodid)
    {
	double price_place = (side == BuySellSide::buy ? 
			      lastAsk_.getPx() + aggressivenessInTicks_ * tickSize_ :
			      lastBid_.getPx() - aggressivenessInTicks_ * tickSize_);
	placeLimitOrder(Px(price_place, tickSize_), side, size, prodid);
    }

    void LLS::placeJoiningLimitOrder(BuySellSide side, int size, int prodid)
    {
	placeLimitOrder(side == BuySellSide::buy ? lastBid_ : lastAsk_, side, size, prodid);
    }

    void LLS::logStrategyData()
    {
	stringstream ss;
	ss << "StrategyData" << ","
	   << SimulatorCore::convertTimePointToString(currTime_) << ","
	   << currTime_.time_since_epoch().count() << ","
	   << lastBidSize_ << ","
	   << lastBid_ << ","
	   << lastAsk_ << ","
	   << lastAskSize_ << ","
	   << currentLeadMid_ - previousLeadMid_ << ","
	   << currentTradeMid_ - previousTradeMid_ ;
	loggerPtr_->push(ss.str());
    }

    // bool LLS::tightQ()
    // {
    // 	return spread_ <= 1.1 * tickSize_;
    // }
    
    void LLS::setStopLoss(double stoploss)
    {
	stopLossOrderLimit_ = stoploss;
	cout << "Stop loss is set to " << stopLossOrderLimit_ << endl;
    }

    void LLS::setTakeProfit(double takeprofit)
    {
	takeProfitOrderLimit_ = takeprofit,
	cout << "Take profit is set to " << takeProfitOrderLimit_ << endl;
	takeProfitOrderPriceLimit_ = takeProfitOrderLimit_ / multiplier_;
	cout << "Points to take profit is set to " << takeProfitOrderPriceLimit_ << endl;
    }

    void LLS::setOrderSize(int ordersize)
    {
	orderSize_ = ordersize;
    }
    
    void LLS::setAggressivenessInTicks(int ticks)
    {
	aggressivenessInTicks_ = ticks;
    }

    void LLS::setThreshold(int lead_threshold_in_ticks, int trade_threshold_in_ticks)
    {
	leadThresholdInPrice_ = lead_threshold_in_ticks * leadTickSize_;
	tradeThresholdInPrice_ = trade_threshold_in_ticks * tickSize_;
    }

    void LLS::setNSnapshots(int nn)
    {
	nSnapshots_ = nn;
    }

    bool LLS::isReady()
    {
	return previousLeadMid_ > 0 && 
	    previousTradeMid_ > 0 &&
	    currentLeadMid_ > 0 &&
	    currentTradeMid_ > 0;
    }

    void LLS::process()
    {
	logStrategyStatus();

	bool hasLeadMDupdate = false; 
	bool hasTradeMDupdate = false;

	// set enable trading and set close position
	if (currTime_ > startTradingTs_) {
	    enableTrading_ = true;
	}

#ifdef ACCOUNT_ARBITRAGER
	bool cancelCountQ = false;
#else 
	bool cancelCountQ(getCancelCount(0) > 300);
#endif
	if ((currTime_ > exitTradingTs_) || cancelCountQ) {
	    closePosition_ = true;
	}

	if (!bookUpdates_[0].empty()) {
	    auto ibu = bookUpdates_[0].begin();
	    while (ibu != bookUpdates_[0].end() ) {
		// spread_ = ibu->getAskPx()[0].getPx() - ibu->getBidPx()[0].getPx();
		if (isBookGood(*ibu)) {
		    lastBid_ = (*ibu).getBidPx()[0];
		    lastAsk_ = (*ibu).getAskPx()[0];
		    lastBidSize_ = (*ibu).getBidQty()[0];
		    lastAskSize_ = (*ibu).getAskQty()[0];
		    ibu->getMidPx(&currentTradeMid_);
		    tradeMidQueue_.push_back(currentTradeMid_);
		}
		ibu++;
	    }
	    hasTradeMDupdate = true;
	}

	if (!bookUpdates_[1].empty()) {
	    auto ibu = bookUpdates_[1].begin();
	    while (ibu != bookUpdates_[1].end()) {
		if (isBookGood(*ibu)) {
		    ibu->getMidPx(&currentLeadMid_);
		    leadMidQueue_.push_back(currentLeadMid_);
		}
		ibu++;
	    }
	    hasLeadMDupdate = true;
	}

	while (leadMidQueue_.size() > nSnapshots_ + 1) {
	    leadMidQueue_.pop_front();
	}
	while (tradeMidQueue_.size() > nSnapshots_ + 1) {
	    tradeMidQueue_.pop_front();
	}
	previousTradeMid_ = tradeMidQueue_.front();
	previousLeadMid_ = leadMidQueue_.front();
	
	if (getPnL() < -12000.0 * orderSize_) {
	    closePosition_ = true;
	}
	
	if (!enableTrading_) {
	    return;
	}
	
	if (closePosition_) {
	    int open_position = getOpenPosition(0);
	    if (open_position > 0 && hasTradeMDupdate && isBookGood()) {
		if (orderbooks_[0].countLimitOrderQty(BuySellSide::buy) > 0 && pendingCancelLimitOrderMessages_[0].empty()) {
		    cancelAllLimitOrders(BuySellSide::buy, 0);
		}
		if (orderbooks_[0].countLimitOrderQty(BuySellSide::sell) < abs(open_position)) {
		    placeAggressiveLimitOrder(BuySellSide::sell, abs(open_position) - orderbooks_[0].countLimitOrderQty(BuySellSide::sell), 0);
		}
	    } else if (open_position < 0 && hasTradeMDupdate && isBookGood()) {
		if (orderbooks_[0].countLimitOrderQty(BuySellSide::sell) > 0 && pendingCancelLimitOrderMessages_[0].empty()) {
		    cancelAllLimitOrders(BuySellSide::sell, 0);
		}
		if (orderbooks_[0].countLimitOrderQty(BuySellSide::buy) < abs(open_position)) {
		    placeAggressiveLimitOrder(BuySellSide::buy, abs(open_position) - orderbooks_[0].countLimitOrderQty(BuySellSide::buy), 0);
		}
	    }
	    return;
	}
	
	// signals can only be non zero on LeadMDupdate
	BuySellSide trade_signal = BuySellSide::unknown;
	if (isReady() &&
	    currentLeadMid_ - previousLeadMid_ >= leadThresholdInPrice_ - TOL &&
	    currentTradeMid_ - previousTradeMid_ <= tradeThresholdInPrice_ + TOL &&
	    hasLeadMDupdate ) {
	    trade_signal = BuySellSide::buy;
	} else if (isReady() &&
		   previousLeadMid_ - currentLeadMid_ >= leadThresholdInPrice_ - TOL &&
		   previousTradeMid_ - currentTradeMid_ <= tradeThresholdInPrice_ + TOL &&
		   hasLeadMDupdate ) {
	    trade_signal = BuySellSide::sell;
	}

	logStrategyData();

	int open_position = getOpenPosition(0);
	// flat position
	if (open_position == 0) {
	    // if flat and no lead market data update. do nothing
	    if (!hasLeadMDupdate) {
		return;
	    }
	    // if there are limit orders, cancel limit order
	    if (orderbooks_[0].countLimitOrders() > 0 && pendingCancelLimitOrderMessages_[0].empty()) {
	     	cancelAllLimitOrders(0);
	    }
	    // check signal and send limit order to open position
	    if ((trade_signal == BuySellSide::buy || trade_signal == BuySellSide::sell) 
		&& isBookGood() ) {
		placeAggressiveLimitOrder(trade_signal, orderSize_, 0);
	    }
	} 
	// positive position
	else if (open_position > 0) {
	    if (orderbooks_[0].countLimitOrders(BuySellSide::buy) > 0 && pendingCancelLimitOrderMessages_[0].empty()) {
		cancelAllLimitOrders(BuySellSide::buy, 0);
	    }
	    // send limit order to take profit
	    if (orderbooks_[0].countLimitOrderQty(BuySellSide::sell) < abs(open_position)) {
		Px takeProfitPx(Px::round(filledMessageLists_[0]->calcVWAP(), tickSize_, (open_position > 0) ? -1 : 1));
		placeLimitOrder(Px(takeProfitPx.getPx() + takeProfitOrderPriceLimit_, tickSize_), BuySellSide::sell, abs(open_position) - orderbooks_[0].countLimitOrderQty(BuySellSide::sell), 0);
	    } else {
		// stop loss
		if (((getOpenPnL(0) < stopLossOrderLimit_ * abs(open_position)) && hasTradeMDupdate) || 
		    // or reverse signal
		    ((trade_signal == BuySellSide::sell) && hasLeadMDupdate)) {
		    if (orderbooks_[0].countLimitOrders(BuySellSide::sell) > 0 && pendingCancelLimitOrderMessages_[0].empty()) {		    
			cancelAllLimitOrders(BuySellSide::sell, 0);
		    }
		    placeAggressiveLimitOrder(BuySellSide::sell, abs(open_position) - orderbooks_[0].countLimitOrderQty(BuySellSide::sell), 0);
		}
		// update take-profit price level
		else if ((trade_signal == BuySellSide::buy && hasLeadMDupdate)) {
		    if (orderbooks_[0].countLimitOrders(BuySellSide::sell) > 0 && pendingCancelLimitOrderMessages_[0].empty()) {		    
			cancelAllLimitOrders(BuySellSide::sell, 0);
		    }
		    placeLimitOrder(Px(lastAsk_.getPx() + takeProfitOrderPriceLimit_, tickSize_), BuySellSide::sell, abs(open_position) - orderbooks_[0].countLimitOrderQty(BuySellSide::sell), 0);
		}
	    }
	}
	// negative position
	else {
	    if (orderbooks_[0].countLimitOrders(BuySellSide::sell) > 0 && pendingCancelLimitOrderMessages_[0].empty()) {
		cancelAllLimitOrders(BuySellSide::sell, 0);
	    }
	    // send limit order to take profit
	    if (orderbooks_[0].countLimitOrderQty(BuySellSide::buy) < abs(open_position)) {
		Px takeProfitPx(Px::round(filledMessageLists_[0]->calcVWAP(), tickSize_, (open_position > 0) ? -1 : 1));
		placeLimitOrder(Px(takeProfitPx.getPx() - takeProfitOrderPriceLimit_, tickSize_), BuySellSide::buy, abs(open_position) - orderbooks_[0].countLimitOrderQty(BuySellSide::buy), 0);
	    } else {
		// stop loss
		if (((getOpenPnL(0) < stopLossOrderLimit_ * abs(open_position)) && hasTradeMDupdate) || 
		    // or reverse signal
		    ((trade_signal == BuySellSide::buy) && hasLeadMDupdate)) {
		    if (orderbooks_[0].countLimitOrders(BuySellSide::buy) > 0 && pendingCancelLimitOrderMessages_[0].empty()) {		    
			cancelAllLimitOrders(BuySellSide::buy, 0);
		    }
		    placeAggressiveLimitOrder(BuySellSide::buy, abs(open_position) - orderbooks_[0].countLimitOrderQty(BuySellSide::buy), 0);
		}
		// update take-profit price level
		else if ((trade_signal == BuySellSide::sell) && hasLeadMDupdate) {
		    if (orderbooks_[0].countLimitOrders(BuySellSide::buy) > 0 && pendingCancelLimitOrderMessages_[0].empty()) {		    
			cancelAllLimitOrders(BuySellSide::buy, 0);
		    }
		    placeLimitOrder(Px(lastBid_.getPx() - takeProfitOrderPriceLimit_, tickSize_), BuySellSide::buy, abs(open_position) - orderbooks_[0].countLimitOrderQty(BuySellSide::buy), 0);
		}
	    }
	}
    }




}

// LLS_CPP
