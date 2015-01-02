//
// File:        $ SMMS.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2014/01/07 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include "SMMS.hpp"

namespace simulator {

    SMMS::SMMS(string exchange, string contractFront, string contractBack, double multiple, double tickSize, string startTradingTimeString, string exitNoonTimeString, string startNoonTimeString, string exitTradingTimeString, SimulatorCore * simcore_ptr, Logger * logger_ptr) :
	Strategy("SMMS", simcore_ptr, logger_ptr),
	multiplier_(multiple),
	tickSize_(tickSize),
	startTradingTimeString_(startTradingTimeString),
	exitNoonTimeString_(exitNoonTimeString),
	startNoonTimeString_(startNoonTimeString),
	exitTradingTimeString_(exitTradingTimeString), 
	iSpreadPtr_(new ImpliedSpreadPrice<static_cast<int>(BuySellSide::mid)>()), 
	maPxPtr_(new ImpliedSpreadPriceSmooth<static_cast<int>(BuySellSide::mid), 4, 60>())
	//maPxPtr_(new ImpliedSpreadPrice<static_cast<int>(BuySellSide::mid)>())
    {
	addStrat(exchange, contractFront, multiple);
	addStrat(exchange, contractBack, multiple);
	string datestr = SimulatorCore::convertTimePointToString(simcore_ptr->getCurrTime()).substr(0, 10);
	stringstream ss1, ss2, ss3, ss4;
	ss1 << datestr << "T" << startTradingTimeString_ << ".000000000";
	startTradingTs_ = SimulatorCore::convertStringToTimePoint(ss1.str());
	cout << "Starting trade date and time is " << ss1.str() << endl;
	ss3 << datestr << "T" << exitNoonTimeString_ << ".000000000";
	exitNoonTs_ = SimulatorCore::convertStringToTimePoint(ss3.str());
	cout << "Exiting noon date and time is " << ss3.str() << endl;
	ss4 << datestr << "T" << startNoonTimeString_ << ".000000000";
	startNoonTs_ = SimulatorCore::convertStringToTimePoint(ss4.str());
	cout << "Starting noon date and time is " << ss4.str() << endl;
	ss2 << datestr << "T" << exitTradingTimeString_ << ".000000000";
	exitTradingTs_ = SimulatorCore::convertStringToTimePoint(ss2.str());
	cout << "Exiting trade date and time is " << ss2.str() << endl;

	lastQuoteTs_ = startTradingTs_;
    }

    string SMMS::getStatusString()
    {
	stringstream ss;
	ss << ","
	   << "(" << getOpenPosition(0) << "," 
	   << pendingLimitOrderMessages_[0].size() << "," 
	   << pendingCancelLimitOrderMessages_[0].size() << "," 
	   << (simulatorCorePtr_->getExchanges()).front()->buyOpenQty() << "," 
	   << (simulatorCorePtr_->getExchanges()).front()->sellOpenQty() << "," 
	   << (simulatorCorePtr_->getExchanges()).front()->buyOpenOrderQty() << "," 
	   << (simulatorCorePtr_->getExchanges()).front()->buyCloseOrderQty() << "," 
	   << (simulatorCorePtr_->getExchanges()).front()->sellOpenOrderQty() << "," 
	   << (simulatorCorePtr_->getExchanges()).front()->sellCloseOrderQty() << "),"
	   << "(" << getOpenPosition(1) << "," 
	   << pendingLimitOrderMessages_[1].size() << "," 
	   << pendingCancelLimitOrderMessages_[1].size() << "," 
	   << (simulatorCorePtr_->getExchanges()).back()->buyOpenQty() << "," 
	   << (simulatorCorePtr_->getExchanges()).back()->sellOpenQty() << "," 
	   << (simulatorCorePtr_->getExchanges()).back()->buyOpenOrderQty() << "," 
	   << (simulatorCorePtr_->getExchanges()).back()->buyCloseOrderQty() << "," 
	   << (simulatorCorePtr_->getExchanges()).back()->sellOpenOrderQty() << "," 
	   << (simulatorCorePtr_->getExchanges()).back()->sellCloseOrderQty() << ")," 
	   << "(" << iSpreadPtr_->getValue() << "," 
	   << maPxPtr_->getValue() << ")";
	return ss.str();
    }

    void SMMS::setEdge(double edge)
    {
	edge_ = edge;
	cout << "Edge is set to " << edge_ << endl;
    }

    void SMMS::setBuffer(double buffer)
    {
	buffer_ = buffer;
	cout << "Buffer is set to " << buffer_ << endl;
    }

    void SMMS::setOrderSize(int ordersize)
    {
	orderSize_ = ordersize;
	cout << "Order size is set to " << orderSize_ << endl;
    }

    void SMMS::setMaxSpreadPosition(int maxSpreadPosition) 
    {
	maxSpreadPosition_ = maxSpreadPosition;
	cout << "Max position of spread is set to " << maxSpreadPosition_ << endl;
    }

    void SMMS::setStopLoss(double stoploss, double stoplosses)
    {
	stopLossOrderLimit_ = stoploss;
	cout << "Stop loss is set to " << stopLossOrderLimit_ << endl;
	// stopLossOrdersLimit_ = stoplosses;
	// cout << "Stop losses is set to " << stopLossOrdersLimit_ << endl;
    }

    void SMMS::setTakeProfit(double takeprofit, double takeprofits)
    {
	takeProfitOrderLimit_ = takeprofit;
	cout << "Take profit is set to " << takeProfitOrderLimit_ << endl;
	// takeProfitOrdersLimit_ = takeprofits;
	// cout << "Take profits is set to " << takeProfitOrdersLimit_ << endl;
	takeProfitOrderPriceLimit_ = takeProfitOrderLimit_ / multiplier_;
	cout << "Point(s) to take profit is set to " << takeProfitOrderPriceLimit_ << endl;
	takeProfitOrdersPriceLimit_ = takeProfitOrdersLimit_ / multiplier_;
	cout << "Point(s) to take profits is set to " << takeProfitOrdersPriceLimit_ << endl;
    }

    void SMMS::setMaxCancelQty(int maxCancelQty) 
    {
	maxCancelQty_ = maxCancelQty;
	cout << "Maximum cancel quantity is set to " << maxCancelQty_ << endl;
    }

    bool SMMS::exceedMaxSpreadPositionQ() const 
    {
	if (abs(spreadPosition_) >= maxSpreadPosition_) {
	    return true;
	} else {
	    return false;
	}
    }

    double SMMS::getStaticOpenPnL(BuySellSide side, int prodId) const 
    {
	double ret = 0.0;
	if (side == BuySellSide::buy) {
	    ret = (openPnlRefPxs_[prodId] - lastTradeBuyPxs_[prodId]) * multiplier_;
	} else if (side == BuySellSide::sell) {
	    ret = (lastTradeSellPxs_[prodId] - openPnlRefPxs_[prodId]) * multiplier_;
	}
	return ret;
    }

    double SMMS::getStaticSpreadOpenPnL(BuySellSide side, int prodId) const 
    {
	double ret = 0.0;
	if (side == BuySellSide::buy && prodId == 0) { 
	    ret = (openPnlRefPxs_[1] - lastTradeBuyPxs_[0] - expSpreadPx_) * multiplier_;
	} else if (side == BuySellSide::buy && prodId == 1) { 
	    ret = -(lastTradeBuyPxs_[1] - openPnlRefPxs_[0] - expSpreadPx_) * multiplier_;
	} else if (side == BuySellSide::sell && prodId == 0) { 
	    ret = -(openPnlRefPxs_[1] - lastTradeSellPxs_[0] - expSpreadPx_) * multiplier_;
	} else if (side == BuySellSide::sell && prodId == 1) { 
	    ret = (lastTradeSellPxs_[1] - openPnlRefPxs_[0] - expSpreadPx_) * multiplier_;
	}
	return ret;
    }

    double SMMS::getStaticBestPnL(BuySellSide side, int prodId) const 
    {
	return max(getStaticOpenPnL(side, prodId), getStaticSpreadOpenPnL(side, prodId));
    }

    bool SMMS::pendingOrderMessagesQ(BuySellSide side, bool frontQ) const 
    {
	bool ret;
	if (frontQ) { 
	    if (any_of(pendingLimitOrderMessages_[0].begin(), 
		       pendingLimitOrderMessages_[0].end(), 
		       [&] (LimitOrderMessage v) { return v.getSide() == side; })) {  
		ret = true;
	    } else {
		ret = false;
	    }
	} else {
	    if (any_of(pendingLimitOrderMessages_[1].begin(), 
		       pendingLimitOrderMessages_[1].end(), 
		       [&] (LimitOrderMessage v) { return v.getSide() == side; })) {  
		ret = true;
	    } else {
		ret = false;
	    }
	}
	return ret;
    }

    bool SMMS::pendingOrderMessagesQ(BuySellSide side) const 
    {
	return pendingOrderMessagesQ(side, true) || pendingOrderMessagesQ(side, false);
    }

    bool SMMS::pendingOrderMessagesQ(bool frontQ) const 
    {
	return pendingOrderMessagesQ(BuySellSide::buy, frontQ) || pendingOrderMessagesQ(BuySellSide::sell, frontQ);
    }

    bool SMMS::pendingOrderMessagesQ() const 
    {
	return pendingOrderMessagesQ(true) || pendingOrderMessagesQ(false);
    }

    bool SMMS::pendingCancelMessagesQ(BuySellSide side, bool frontQ) const 
    {
	bool ret;
	if (frontQ) { 
	    if (any_of(pendingCancelLimitOrderMessages_[0].begin(), 
		       pendingCancelLimitOrderMessages_[0].end(), 
		       [&] (CancelOrderMessage<LimitOrderMessage> v) { return v.getSide() == side; })) {  
		ret = true;
	    } else {
		ret = false;
	    }
	} else {
	    if (any_of(pendingCancelLimitOrderMessages_[1].begin(), 
		       pendingCancelLimitOrderMessages_[1].end(), 
		       [&] (CancelOrderMessage<LimitOrderMessage> v) { return v.getSide() == side; })) {  
		ret = true;
	    } else {
		ret = false;
	    }
	}
	return ret;
    }

    bool SMMS::pendingCancelMessagesQ(BuySellSide side) const 
    {
	return pendingCancelMessagesQ(side, true) || pendingCancelMessagesQ(side, false);
    }

    bool SMMS::pendingCancelMessagesQ(bool frontQ) const 
    {
	return pendingCancelMessagesQ(BuySellSide::buy, frontQ) || pendingCancelMessagesQ(BuySellSide::sell, frontQ);
    }

    bool SMMS::pendingCancelMessagesQ() const 
    {
	return pendingCancelMessagesQ(true) || pendingCancelMessagesQ(false);
    }

    bool SMMS::pendingMessagesQ(BuySellSide side, bool frontQ) const 
    {
	return pendingOrderMessagesQ(side, frontQ) || pendingCancelMessagesQ(side, frontQ);
    }

    bool SMMS::pendingMessagesQ(BuySellSide side) const 
    {
	return pendingOrderMessagesQ(side) || pendingCancelMessagesQ(side);
    }

    bool SMMS::pendingMessagesQ(bool frontQ) const 
    {
	return pendingOrderMessagesQ(frontQ) || pendingCancelMessagesQ(frontQ);
    }

    bool SMMS::pendingMessagesQ() const 
    {
	return pendingOrderMessagesQ() || pendingCancelMessagesQ();
    }

    bool SMMS::quoteTimeout() 
    {
	if ((currTime_ - lastQuoteTs_).count() <= quoteTimeout_) { 
	    return true;
	} else {
	    lastQuoteTs_ = currTime_;
	    return false;
	}
	// return false;
    }

    void SMMS::quotePositions4way(bool qCancelOnly) 
    {
	passiveHedger(Px::round(bidPxs_[1].getPx() - maPxPtr_->getValue() - edge_ - buffer_, tickSize_, -1), orderSize_, BuySellSide::buy, 0, qCancelOnly);
	passiveHedger(Px::round(askPxs_[1].getPx() - maPxPtr_->getValue() + edge_ + buffer_, tickSize_, 1), orderSize_, BuySellSide::sell, 0, qCancelOnly);
	passiveHedger(Px::round(bidPxs_[0].getPx() + maPxPtr_->getValue() - edge_ - buffer_, tickSize_, -1), orderSize_, BuySellSide::buy, 1, qCancelOnly);
	passiveHedger(Px::round(askPxs_[0].getPx() + maPxPtr_->getValue() + edge_ + buffer_, tickSize_, 1), orderSize_, BuySellSide::sell, 1, qCancelOnly);
	spreadOnOff_ = true;
	expSpreadPx_ = maPxPtr_->getValue();
    }

    void SMMS::quotePositions2way(double targetPx, int targetQty, BuySellSide side, bool qCancelOnly) 
    {
	if (side == BuySellSide::buy) {
	    passiveHedger(Px::round(askPxs_[1].getPx() - targetPx + buffer_, tickSize_, 1), targetQty, BuySellSide::sell, 0, qCancelOnly);
	    passiveHedger(Px::round(bidPxs_[0].getPx() + targetPx - buffer_, tickSize_, -1), targetQty, BuySellSide::buy, 1, qCancelOnly);
	} else {
	    passiveHedger(Px::round(bidPxs_[1].getPx() - targetPx - buffer_, tickSize_, -1), targetQty, BuySellSide::buy, 0, qCancelOnly);
	    passiveHedger(Px::round(askPxs_[0].getPx() + targetPx + buffer_, tickSize_, 1), targetQty, BuySellSide::sell, 1, qCancelOnly);
	}
	spreadOnOff_ = false;
	expSpreadPx_ = targetPx;
    }

    void SMMS::activeHedger(Px targetPx, int hedgeQty, BuySellSide hedgeSide, int prodId, bool qCancelOnly) 
    {
	int remainQty = hedgeQty;
	auto limitOrderMessages = orderbooks_[prodId].getLimitOrderBook(hedgeSide);
	for (auto level1 : limitOrderMessages) { 
	    if (level1.front().getPx() != targetPx) { 
		for (auto level2 : level1) { 
		    cancelLimitOrder(level2);
		}
	    } else {
		for (auto level2 : level1) { 
		    if (level2.getQty() <= remainQty) { 
			remainQty -= level2.getQty();
		    } else { 
			cancelLimitOrder(level2);
		    }
		}
	    }
	}
	if (remainQty > 0 && !qCancelOnly) { 
	    placeLimitOrder(targetPx, hedgeSide, remainQty, prodId);
	}
    }	    

    void SMMS::passiveHedger(Px targetPx, int hedgeQty, BuySellSide hedgeSide, int prodId, bool qCancelOnly) 
    {
	int remainQty = hedgeQty;
	auto limitOrderMessages = orderbooks_[prodId].getLimitOrderBook(hedgeSide);
	for (auto level1 : limitOrderMessages) { 
	    Px px = level1.front().getPx();
	    if ((hedgeSide == BuySellSide::buy && (px > targetPx || (px < targetPx && px < bidPxs_[prodId]))) || 
		(hedgeSide == BuySellSide::sell && (px < targetPx || (px > targetPx && px > askPxs_[prodId])))) { 
		for (auto level2 : level1) { 
		    cancelLimitOrder(level2);
		}
	    } else {
		for (auto level2 : level1) { 
		    if (level2.getQty() <= remainQty) { 
			remainQty -= level2.getQty();
		    } else { 
			cancelLimitOrder(level2);
		    }
		}
	    }
	}
	if (remainQty > 0 && !qCancelOnly) { 
	    placeLimitOrder(targetPx, hedgeSide, remainQty, prodId);
	}
    }
    
    void SMMS::process()
    {
	// set enable trading and set close position
	if (currTime_ > startTradingTs_) {
	    enableTrading_ = true;
	}

	if (currTime_ > exitNoonTs_) {
	    stopQuoting_ = true;
	}
	if (currTime_ > startNoonTs_) {
	    stopQuoting_ = false;
	}

#ifdef ACCOUNT_ARBITRAGER
	bool cancelCountQ = false;
#else 
	bool cancelCountQ(getCancelCount() > maxCancelQty_);
#endif

	if ((currTime_ > exitTradingTs_) || 
	    cancelCountQ || 
	    getPnL() < -3000.0 * orderSize_) {
	    closePosition_ = true;
	}

	closeSpreadPosition_ = false;
	if (exceedMaxSpreadPositionQ() && (getOpenPosition(0) + getOpenPosition(1)) == 0) {
	    closeSpreadPosition_ = true;
	}

	// Update front leg.
	if (!bookUpdates_[0].empty() && 
	    !tradeUpdates_[0].empty()) {
	    auto ibu = bookUpdates_[0].begin();
	    auto itu = tradeUpdates_[0].begin();
	    while (ibu != bookUpdates_[0].end() && 
		   itu != tradeUpdates_[0].end()) {
		iSpreadPtr_->update(*ibu, true);
		maPxPtr_->update(*ibu, true);
		if (ibu->getBidQty()[0] > 0) {
		    bidPxs_[0] = ibu->getBidPx()[0];
		    bidQtys_[0] = ibu->getBidQty()[0];
		}
		if (ibu->getAskQty()[0] > 0) {
		    askPxs_[0] = ibu->getAskPx()[0];
		    askQtys_[0] = ibu->getAskQty()[0];
		}
		totalTradeQty_[0] = itu->getQty();
		++ibu;
		++itu;
	    }
	    hasMdUpdateFront_ = true;
	}
	// Update back leg.
	if (!bookUpdates_[1].empty() && 
	    !tradeUpdates_[1].empty()) {
	    auto ibu = bookUpdates_[1].begin();
	    auto itu = tradeUpdates_[1].begin();
	    while (ibu != bookUpdates_[1].end() && 
		   itu != tradeUpdates_[1].end()) {
		iSpreadPtr_->update(*ibu, false);
		maPxPtr_->update(*ibu, false);
		if (ibu->getBidQty()[0] > 0) {
		    bidPxs_[1] = ibu->getBidPx()[0];
		    bidQtys_[1] = ibu->getBidQty()[0];
		}
		if (ibu->getAskQty()[0] > 0) {
		    askPxs_[1] = ibu->getAskPx()[0];
		    askQtys_[1] = ibu->getAskQty()[0];
		}
		totalTradeQty_[1] = itu->getQty();
		++ibu;
		++itu;
	    }
	    hasMdUpdateBack_ = true;
	}

	hasMdUpdate_ = hasMdUpdateFront_ || hasMdUpdateBack_;
	
	logStrategyStatus(",OpenPositionFront,PendingOrderFront,PendingCancelFront,BuyOpenPositionFront,SellOpenPositionFront,BuyOpenOrderQtyFront,BuyCloseOrderQtyFront,SellOpenOrderQtyFront,SellCloseOrderQtyFront,OpenPositionBack,PendingOrderBack,PendingCancelBack,BuyOpenPositionBack,SellOpenPositionBack,BuyOpenOrderQtyBack,BuyCloseOrderQtyBack,SellOpenOrderQtyBack,SellCloseOrderQtyBack,MidPxSpread,MaPxSpread", getStatusString());

	if (!enableTrading_) {
	    return;
	}

	if (closePosition_) {
	    if (getOpenPosition(0) == 0 && getOpenPosition(1) == 0) { 
		logStrategyStatus("", ",ClosePosition: cancel all resting orders");
		if (!pendingMessagesQ(BuySellSide::buy, true)) { 
		    cancelAllLimitOrders(BuySellSide::buy, 0);
		}
		if (!pendingMessagesQ(BuySellSide::sell, true)) { 
		    cancelAllLimitOrders(BuySellSide::sell, 0);
		}
		if (!pendingMessagesQ(BuySellSide::buy, false)) { 
		    cancelAllLimitOrders(BuySellSide::buy, 1);
		}
		if (!pendingMessagesQ(BuySellSide::sell, false)) { 
		    cancelAllLimitOrders(BuySellSide::sell, 1);
		}
	    } else { 
		if (getOpenPosition(0) > 0) {
		    logStrategyStatus("", ",ClosePosition: sell front leg");
		    if (!pendingMessagesQ(BuySellSide::buy, true)) { 
			cancelAllLimitOrders(BuySellSide::buy, 0);
		    }
		    if (!pendingMessagesQ(BuySellSide::sell, true)) { 
			passiveHedger(askPxs_[0], abs(getOpenPosition(0)), BuySellSide::sell, 0, false);
		    }
		} else if (getOpenPosition(0) < 0) { 
		    logStrategyStatus("", ",ClosePosition: buy front leg");
		    if (!pendingMessagesQ(BuySellSide::sell, true)) {
			cancelAllLimitOrders(BuySellSide::sell, 0);
		    }
		    if (!pendingMessagesQ(BuySellSide::buy, true)) { 
			passiveHedger(bidPxs_[0], abs(getOpenPosition(0)), BuySellSide::buy, 0, false);
		    }
		} else { 
		    if (!pendingMessagesQ(BuySellSide::buy, true)) { 
			cancelAllLimitOrders(BuySellSide::buy, 0);
		    }
		    if (!pendingMessagesQ(BuySellSide::sell, true)) { 
			cancelAllLimitOrders(BuySellSide::sell, 0);
		    }
		}
		if (getOpenPosition(1) > 0) { 
		    logStrategyStatus("", ",ClosePosition: sell back leg");
		    if (!pendingMessagesQ(BuySellSide::buy, false)) { 
			cancelAllLimitOrders(BuySellSide::buy, 1);
		    }
		    if (!pendingMessagesQ(BuySellSide::sell, false)) { 
			passiveHedger(Px(askPxs_[1].getPx() - tickSize_, tickSize_), abs(getOpenPosition(1)), BuySellSide::sell, 1, false);
		    }
		} else if (getOpenPosition(1) < 0) {
		    logStrategyStatus("", ",ClosePosition: buy back leg");
		    if (!pendingMessagesQ(BuySellSide::sell, false)) { 
			cancelAllLimitOrders(BuySellSide::sell, 1);
		    }
		    if (!pendingMessagesQ(BuySellSide::buy, false)) {
			passiveHedger(Px(bidPxs_[1].getPx() + tickSize_, tickSize_), abs(getOpenPosition(1)), BuySellSide::buy, 1, false);
		    }
		} else { 
		    if (!pendingMessagesQ(BuySellSide::buy, false)) { 
			cancelAllLimitOrders(BuySellSide::buy, 1);
		    }
		    if (!pendingMessagesQ(BuySellSide::sell, false)) { 
			cancelAllLimitOrders(BuySellSide::sell, 1);
		    }
		}
	    }
	    return;
	}

	if (closeSpreadPosition_ && iSpreadPtr_->isGood()) {
	    if (getOpenPosition(1) < 0) { 
		logStrategyStatus("", ",CloseSpreadPosition: buy spread");
		if (!pendingMessagesQ(BuySellSide::buy, true)) { 
		    cancelAllLimitOrders(BuySellSide::buy, 0);
		}
		if (!pendingMessagesQ(BuySellSide::sell, false)) { 
		    cancelAllLimitOrders(BuySellSide::sell, 1);
		}
		if (!pendingMessagesQ(BuySellSide::sell, true) && !pendingMessagesQ(BuySellSide::buy, false)) { 
		    quotePositions2way(iSpreadPtr_->getValue(), abs(getOpenPosition(1)), BuySellSide::buy, quoteTimeout());
		}
	    } else {
		logStrategyStatus("", ",CloseSpreadPosition: sell spread");
		if (!pendingMessagesQ(BuySellSide::sell, true)) { 
		    cancelAllLimitOrders(BuySellSide::sell, 0);
		}
		if (!pendingMessagesQ(BuySellSide::buy, false)) { 
		    cancelAllLimitOrders(BuySellSide::buy, 1);
		}
		if (!pendingMessagesQ(BuySellSide::buy, true) && !pendingMessagesQ(BuySellSide::sell, false)) { 
		    quotePositions2way(iSpreadPtr_->getValue(), abs(getOpenPosition(1)), BuySellSide::sell, quoteTimeout());
		}
	    }
	    return;
	}

	// must NOT block here, needs to take liqiudity fast based on hasFills_ flag vector
	// if (pendingMessagesQ(true) || pendingMessagesQ(false)) {
	//     return;
	// }

	// imply spread position first
	if (getOpenPosition(0) * getOpenPosition(1) < 0) {
	    spreadPosition_ = getOpenPosition(1) > 0 ? min(-getOpenPosition(0), getOpenPosition(1)) : -min(getOpenPosition(0), -getOpenPosition(1));
	} else {
	    spreadPosition_ = 0;
	}
	positionFront = getOpenPosition(0) + spreadPosition_;
	positionBack = getOpenPosition(1) - spreadPosition_;
	positionDelta = abs(abs(positionFront) - abs(positionBack));
	// go down the status transition trees
	if (positionFront == 0) {
	    if (positionBack == 0) {
		if (!pendingMessagesQ() && hasMdUpdate_ && maPxPtr_->isGood() && !stopQuoting_ && spreadPosition_ == 0) {
		    logStrategyStatus("", ",Quote 4 ways");
		    quotePositions4way(quoteTimeout());
		    hasMdUpdateFront_ = false;
		    hasMdUpdateBack_ = false;
		// } else if (hasMdUpdate_ && maPxPtr_->isGood() && !stopQuoting_ && spreadPosition_ != 0) { 
		} else if (maPxPtr_->isGood() && !stopQuoting_ && spreadPosition_ != 0) { 
		    if (spreadPosition_ < 0) { 
			if (!pendingMessagesQ(BuySellSide::buy, true)) { 
			    cancelAllLimitOrders(BuySellSide::buy, 0);
			}
			if (!pendingMessagesQ(BuySellSide::sell, false)) { 
			    cancelAllLimitOrders(BuySellSide::sell, 1);
			}
			if (!pendingMessagesQ(BuySellSide::sell, true) && !pendingMessagesQ(BuySellSide::buy, false)) { 
			    logStrategyStatus("", ",Quote 2 ways, buy spread");
			    quotePositions2way(maPxPtr_->getValue() - takeProfitOrdersPriceLimit_, 
					       abs(spreadPosition_), 
					       BuySellSide::buy, 
					       quoteTimeout());
			    hasMdUpdateFront_ = false;
			    hasMdUpdateBack_ = false;
			}
		    } else {
			if (!pendingMessagesQ(BuySellSide::sell, true)) { 
			    cancelAllLimitOrders(BuySellSide::sell, 0);
			}
			if (!pendingMessagesQ(BuySellSide::buy, false)) { 
			    cancelAllLimitOrders(BuySellSide::buy, 1);
			}
			if (!pendingMessagesQ(BuySellSide::buy, true) && !pendingMessagesQ(BuySellSide::sell, false)) { 
			    logStrategyStatus("", ",Quote 2 ways, sell spread");
			    quotePositions2way(maPxPtr_->getValue() + takeProfitOrdersPriceLimit_, 
					       abs(spreadPosition_), 
					       BuySellSide::sell, 
					       quoteTimeout());
			    hasMdUpdateFront_ = false;
			    hasMdUpdateBack_ = false;
			}
		    }
		} else if (stopQuoting_) { 
		    logStrategyStatus("", ",Stop quoting: cancel all resting orders");
		    if (!pendingMessagesQ(BuySellSide::buy, true)) { 
			cancelAllLimitOrders(BuySellSide::buy, 0);
		    }
		    if (!pendingMessagesQ(BuySellSide::sell, true)) { 
			cancelAllLimitOrders(BuySellSide::sell, 0);
		    }
		    if (!pendingMessagesQ(BuySellSide::buy, false)) { 
			cancelAllLimitOrders(BuySellSide::buy, 1);
		    }
		    if (!pendingMessagesQ(BuySellSide::sell, false)) { 
			cancelAllLimitOrders(BuySellSide::sell, 1);
		    }
		}
	    } else {
		if (positionBack < 0) {
		    fillSide = BuySellSide::sell;
		    hedgeSide = BuySellSide::buy;
		    hedgeBuyQ = true;
		} else {
		    fillSide = BuySellSide::buy;
		    hedgeSide = BuySellSide::sell;
		    hedgeBuyQ = false;
		}
		if (!pendingMessagesQ(fillSide, true)) { 
		    cancelAllLimitOrders(fillSide, 0);
		}
		if (!pendingMessagesQ(fillSide, false)) {
		    cancelAllLimitOrders(fillSide, 1);
		}
		if (spreadOnOff_) {
		    stopLoss_ = getStaticBestPnL(fillSide, 1) < stopLossOrderLimit_;
		    // if back leg is stopped loss
		    if (hasMdUpdate_ && stopLoss_ && !pendingMessagesQ(hedgeSide)) { 
			logStrategyStatus("", ",SpreadOn: back leg stop loss, quote front leg to peg spread");
			targetActivePx = hedgeBuyQ ? bidPxs_[1] : askPxs_[1];
			activeHedger(targetActivePx, abs(positionBack), hedgeSide, 1, false);
			targetHedgePx = hedgeBuyQ ? bidPxs_[0] : askPxs_[0];
			activeHedger(targetHedgePx, abs(positionBack), hedgeSide, 0, false);
			hasMdUpdateBack_ = false;
			hasMdUpdateFront_ = false;
		    }
		    // if back leg is not stopped loss
		    else if (hasFills_[1] || !pendingMessagesQ(hedgeSide)) {
			logStrategyStatus("", ",SpreadOn: back leg take profit, hit/lift front leg");
			// take profit for back leg
			targetActivePx = hedgeBuyQ ? Px(lastTradeSellPxs_[1] - takeProfitOrderPriceLimit_, tickSize_) : Px(lastTradeBuyPxs_[1] + takeProfitOrderPriceLimit_, tickSize_);
			activeHedger(targetActivePx, abs(positionBack), hedgeSide, 1, false);
			// establish winning spread by taking appropriate position of front leg
			targetHedgePx = hedgeBuyQ ? Px::round(lastTradeSellPxs_[1] - expSpreadPx_ - edge_, tickSize_, -1) : Px::round(lastTradeBuyPxs_[1] - expSpreadPx_ + edge_, tickSize_, 1);
			activeHedger(targetHedgePx, abs(positionBack), hedgeSide, 0, false);
		    }
		} else {
		    stopLoss_ = getStaticBestPnL(fillSide, 0) < stopLossOrderLimit_;
		    if (hasMdUpdate_ && stopLoss_ && !pendingMessagesQ(hedgeSide)) {
			logStrategyStatus("", ",SpreadOff: front leg stop loss, quote back leg to peg spread");
			targetActivePx = hedgeBuyQ ? bidPxs_[0] : askPxs_[0];
			activeHedger(targetActivePx, abs(positionBack), hedgeSide, 0, false);
			targetHedgePx = hedgeBuyQ ? bidPxs_[1] : askPxs_[1];
			activeHedger(targetHedgePx, abs(positionBack), hedgeSide, 1, false);
			hasMdUpdateFront_ = false;
			hasMdUpdateBack_ = false;
		    }
		    else if (hasFills_[0] || !pendingMessagesQ(hedgeSide)) {
			logStrategyStatus("", ",SpreadOff: front leg take profit, hit/lift back leg");
			targetActivePx = hedgeBuyQ ? Px(lastTradeSellPxs_[0] - takeProfitOrderPriceLimit_, tickSize_) : Px(lastTradeBuyPxs_[0] + takeProfitOrderPriceLimit_, tickSize_);
			activeHedger(targetActivePx, abs(positionBack), hedgeSide, 0, false);
			targetHedgePx = hedgeBuyQ ? Px::round(lastTradeSellPxs_[0] + expSpreadPx_, tickSize_, -1) : Px::round(lastTradeBuyPxs_[0] + expSpreadPx_, tickSize_, 1);
			activeHedger(targetHedgePx, abs(positionBack), hedgeSide, 1, false);
		    }
		}
	    }
	} else {
	    if (positionBack == 0) {
		if (positionFront < 0) {
		    fillSide = BuySellSide::sell;
		    hedgeSide = BuySellSide::buy;
		    hedgeBuyQ = true;
		} else {
		    fillSide = BuySellSide::buy;
		    hedgeSide = BuySellSide::sell;
		    hedgeBuyQ = false;
		}
		if (!pendingMessagesQ(fillSide, true)) { 
		    cancelAllLimitOrders(fillSide, 0);
		}
		if (!pendingMessagesQ(fillSide, false)) { 
		    cancelAllLimitOrders(fillSide, 1);
		}
		if (spreadOnOff_) {
		    stopLoss_ = getStaticBestPnL(fillSide, 0) < stopLossOrderLimit_;
		    // if front leg is stopped loss
		    if (hasMdUpdate_ && stopLoss_ && !pendingMessagesQ(hedgeSide)) {
			logStrategyStatus("", ",SpreadOn: front leg stop loss, quote back leg to peg spread");
			targetActivePx = hedgeBuyQ ? bidPxs_[0] : askPxs_[0];
			activeHedger(targetActivePx, abs(positionFront), hedgeSide, 0, false);
			targetHedgePx = hedgeBuyQ ? bidPxs_[1] : askPxs_[1];
			activeHedger(targetHedgePx, abs(positionFront), hedgeSide, 1, false);
			hasMdUpdateFront_ = false;
			hasMdUpdateBack_ = false;
		    } 
		    // if front leg is not stopped loss
		    else if (hasFills_[0] || !pendingMessagesQ(hedgeSide)) { 
			logStrategyStatus("", ",SpreadOn: front leg take profit, hit/lift back leg");
			// take profit for front leg
			targetActivePx = hedgeBuyQ ? Px(lastTradeSellPxs_[0] - takeProfitOrderPriceLimit_, tickSize_) : Px(lastTradeBuyPxs_[0] + takeProfitOrderPriceLimit_, tickSize_);
			activeHedger(targetActivePx, abs(positionFront), hedgeSide, 0, false);
			// establish winning spread by taking appropriate position of back leg
			targetHedgePx = hedgeBuyQ ? Px::round(lastTradeSellPxs_[0] + expSpreadPx_ - edge_, tickSize_, -1) : Px::round(lastTradeBuyPxs_[0] + expSpreadPx_ + edge_, tickSize_, 1);
			activeHedger(targetHedgePx, abs(positionFront), hedgeSide, 1, false);
		    }
		} else {
		    stopLoss_ = getStaticBestPnL(fillSide, 1) < stopLossOrderLimit_;
		    if (hasMdUpdate_ && stopLoss_ && !pendingMessagesQ(hedgeSide)) {
			logStrategyStatus("", ",SpreadOff: back leg stop loss, quote front leg to peg spread");
			targetActivePx = hedgeBuyQ ? bidPxs_[1] : askPxs_[1];
			activeHedger(targetActivePx, abs(positionFront), hedgeSide, 1, false);
			targetHedgePx = hedgeBuyQ ? bidPxs_[0] : askPxs_[0];
			activeHedger(targetHedgePx, abs(positionFront), hedgeSide, 0, false);
			hasMdUpdateFront_ = false;
			hasMdUpdateBack_ = false;
		    } else if (hasFills_[1] || !pendingMessagesQ(hedgeSide)) {
			logStrategyStatus("", ",SpreadOff: back leg take profit, hit/lift front leg");
			targetActivePx = hedgeBuyQ ? Px(lastTradeSellPxs_[1] - takeProfitOrderPriceLimit_, tickSize_) : Px(lastTradeBuyPxs_[1] + takeProfitOrderPriceLimit_, tickSize_);
			activeHedger(targetActivePx, abs(positionFront), hedgeSide, 1, false);
			targetHedgePx = hedgeBuyQ ? Px::round(lastTradeSellPxs_[1] - expSpreadPx_, tickSize_, -1) : Px::round(lastTradeBuyPxs_[1] - expSpreadPx_, tickSize_, 1);
			activeHedger(targetHedgePx, abs(positionFront), hedgeSide, 0, false);
		    }
		}
	    } else {
		if ((positionFront * positionBack) > 0) {  // dangerous, both front and back legs are filled, get out ASAP
		    if (positionFront < 0) {
			fillSide = BuySellSide::sell;
			hedgeSide = BuySellSide::buy;
			hedgeBuyQ = true;
		    } else {
			fillSide = BuySellSide::buy;
			hedgeSide = BuySellSide::sell;
			hedgeBuyQ = false;
		    }
		    if (!pendingMessagesQ(fillSide, true)) { 
			cancelAllLimitOrders(fillSide, 0);
		    }
		    if (!pendingMessagesQ(fillSide, false)) { 
			cancelAllLimitOrders(fillSide, 1);
		    }
		    stopLoss_ = getStaticBestPnL(fillSide, 0) < stopLossOrderLimit_;
		    if (hasMdUpdateFront_ && stopLoss_ && !pendingMessagesQ(hedgeSide, true)) {
			logStrategyStatus("", ",SpreadOn: front leg stop loss, disregard what's going on with back leg");
			targetActivePx = hedgeBuyQ ? bidPxs_[0] : askPxs_[0];
			activeHedger(targetActivePx, abs(positionFront), hedgeSide, 0, false);
			hasMdUpdateFront_ = false;
		    } else if (hasFills_[0] || !pendingMessagesQ(hedgeSide, true)) {
			logStrategyStatus("", ",SpreadOn: front leg take profit, disregard what's going on with back leg");
			targetActivePx = hedgeBuyQ ? 
			    Px(lastTradeSellPxs_[0] - takeProfitOrderPriceLimit_, tickSize_) : 
			    Px(lastTradeBuyPxs_[0] + takeProfitOrderPriceLimit_, tickSize_);
			activeHedger(targetActivePx, abs(positionFront), hedgeSide, 0, false);
		    }
		    stopLoss_ = getStaticBestPnL(fillSide, 1) < stopLossOrderLimit_;
		    if (hasMdUpdateBack_ && stopLoss_ && !pendingMessagesQ(hedgeSide, false)) {
			logStrategyStatus("", ",SpreadOn: back leg stop loss, disregard what's going on with front leg");
			targetActivePx = hedgeBuyQ ? bidPxs_[1] : askPxs_[1];
			activeHedger(targetActivePx, abs(positionBack), hedgeSide, 1, false);
			hasMdUpdateBack_ = true;
		    } else if (hasFills_[1] || !pendingMessagesQ(hedgeSide, false)) {
			logStrategyStatus("", ",SpreadOn: back leg take profit, disregard what's going on with front leg");
			targetActivePx = hedgeBuyQ ? 
			    Px(lastTradeSellPxs_[1] - takeProfitOrderPriceLimit_, tickSize_) : 
			    Px(lastTradeBuyPxs_[1] + takeProfitOrderPriceLimit_, tickSize_);
			activeHedger(targetActivePx, abs(positionBack), hedgeSide, 1, false);
		    }
		} else if (abs(positionFront) > abs(positionBack)) {
		    if (positionFront < 0) {
			fillSide = BuySellSide::sell;
			hedgeSide = BuySellSide::buy;
			hedgeBuyQ = true;
		    } else {
			fillSide = BuySellSide::buy;
			hedgeSide = BuySellSide::sell;
			hedgeBuyQ = false;
		    }
		    if (!pendingMessagesQ(fillSide, true)) { 
			cancelAllLimitOrders(fillSide, 0);
		    }
		    if (!pendingMessagesQ(fillSide, false)) { 
			cancelAllLimitOrders(fillSide, 1);
		    }
		    if (spreadOnOff_) {
			stopLoss_ = getStaticBestPnL(fillSide, 0) < stopLossOrderLimit_;
			// if delta front leg is stopped loss
			if (hasMdUpdate_ && stopLoss_ && !pendingMessagesQ(hedgeSide)) {
			    logStrategyStatus("", ",SpreadOn: front leg stop loss, quote back leg to peg spread");
			    // stop loss for delta front leg, join in best bid/ask
			    targetActivePx = hedgeBuyQ ? bidPxs_[0] : askPxs_[0];
			    activeHedger(targetActivePx, positionDelta, hedgeSide, 0, false);
			    targetHedgePx = hedgeBuyQ ? bidPxs_[1] : askPxs_[1];
			    activeHedger(targetHedgePx, positionDelta, hedgeSide, 1, false);
			    hasMdUpdateFront_ = false;
			    hasMdUpdateBack_ = false;
			} 
			// if delta front leg is not stopped loss
			else if (hasFills_[0] || !pendingMessagesQ(hedgeSide)) {
			    logStrategyStatus("", ",SpreadOn: front leg take profit, hit/lift back leg");
			    // take profit for delta front leg
			    targetActivePx = hedgeBuyQ ? Px(lastTradeSellPxs_[0] - takeProfitOrderPriceLimit_, tickSize_) : Px(lastTradeBuyPxs_[0] + takeProfitOrderPriceLimit_, tickSize_);
			    activeHedger(targetActivePx, positionDelta, hedgeSide, 0, false);
			    // establish winning spread by taking appropriate position of delta back leg
			    targetHedgePx = hedgeBuyQ ? Px::round(lastTradeSellPxs_[0] + expSpreadPx_ - edge_, tickSize_, -1) : Px::round(lastTradeBuyPxs_[0] + expSpreadPx_ + edge_, tickSize_, 1);
			    activeHedger(targetHedgePx, positionDelta, hedgeSide, 1, false);
			}
		    } else {
			stopLoss_ = getStaticBestPnL(fillSide, 1) < stopLossOrderLimit_;
			if (hasMdUpdate_ && stopLoss_ && !pendingMessagesQ(hedgeSide)) {
			    logStrategyStatus("", ",SpreadOff: back leg stop loss, front leg stop loss");
			    targetActivePx = hedgeBuyQ ? bidPxs_[1] : askPxs_[1];
			    activeHedger(targetActivePx, positionDelta, hedgeSide, 1, false);
			    targetHedgePx = hedgeBuyQ ? bidPxs_[0] : askPxs_[0];
			    activeHedger(targetHedgePx, positionDelta, hedgeSide, 0, false);
			    hasMdUpdateFront_ = false;
			    hasMdUpdateBack_ = false;
			} else if (hasFills_[1] || !pendingMessagesQ(hedgeSide)) {
			    logStrategyStatus("", ",SpreadOff: back leg take profit, hit/lift front leg");
			    targetActivePx = hedgeBuyQ ? Px(lastTradeSellPxs_[1] - takeProfitOrderPriceLimit_, tickSize_) : Px(lastTradeBuyPxs_[1] + takeProfitOrderPriceLimit_, tickSize_);
			    activeHedger(targetActivePx, positionDelta, hedgeSide, 1, false);
			    targetHedgePx = hedgeBuyQ ? Px::round(lastTradeSellPxs_[1] - expSpreadPx_, tickSize_, -1) : Px::round(lastTradeBuyPxs_[1] - expSpreadPx_, tickSize_, 1);
			    activeHedger(targetHedgePx, positionDelta, hedgeSide, 0, false);
			}
		    }
		} else {
		    if (positionBack < 0) {
			fillSide = BuySellSide::sell;
			hedgeSide = BuySellSide::buy;
			hedgeBuyQ = true;
		    } else {
			fillSide = BuySellSide::buy;
			hedgeSide = BuySellSide::sell;
			hedgeBuyQ = false;
		    }
		    if (!pendingMessagesQ(fillSide, true)) { 
			cancelAllLimitOrders(fillSide, 0);
		    }
		    if (!pendingMessagesQ(fillSide, true)) { 
			cancelAllLimitOrders(fillSide, 1);
		    }
		    if (spreadOnOff_) {
			stopLoss_ = getStaticBestPnL(fillSide, 1) < stopLossOrderLimit_;
			// if delta back leg is stopped loss
			if (hasMdUpdate_ && stopLoss_ && !pendingMessagesQ(hedgeSide)) {
			    logStrategyStatus("", ",SpreadOn: back leg stop loss, quote front leg to peg spread");
			    // stop loss for delta back leg
			    targetActivePx = hedgeBuyQ ? bidPxs_[1] : askPxs_[1];
			    activeHedger(targetActivePx, positionDelta, hedgeSide, 1, false);
			    targetHedgePx = hedgeBuyQ ? bidPxs_[0] : askPxs_[0];
			    activeHedger(targetHedgePx, positionDelta, hedgeSide, 0, false);
			    hasMdUpdateFront_ = false;
			    hasMdUpdateBack_ = false;
			} 
			// if delta back leg is not stopped loss
			else if (hasFills_[1] || !pendingMessagesQ(hedgeSide)) {
			    logStrategyStatus("", ",SpreadOn: back leg take profit, hit/lift front leg");
			    // take profit for delta back leg
			    targetActivePx = hedgeBuyQ ? Px(lastTradeSellPxs_[1] - takeProfitOrderPriceLimit_, tickSize_) : Px(lastTradeBuyPxs_[1] + takeProfitOrderPriceLimit_, tickSize_);
			    activeHedger(targetActivePx, positionDelta, hedgeSide, 1, false);
			    // establish winning spread by taking appropriate position of delta front leg
			    targetHedgePx = hedgeBuyQ ? Px::round(lastTradeSellPxs_[1] - expSpreadPx_ - edge_, tickSize_, -1) : Px::round(lastTradeBuyPxs_[1] - expSpreadPx_ + edge_, tickSize_, 1);
			    activeHedger(targetHedgePx, positionDelta, hedgeSide, 0, false);
			}
		    } else {
			stopLoss_ = getStaticBestPnL(fillSide, 0) < stopLossOrderLimit_;
			if (hasMdUpdate_ && stopLoss_ && !pendingMessagesQ(hedgeSide)) {
			    logStrategyStatus("", ",SpreadOff: front leg stop loss, back leg stop loss");
			    targetActivePx = hedgeBuyQ ? bidPxs_[0] : askPxs_[0];
			    activeHedger(targetActivePx, positionDelta, hedgeSide, 0, false);
			    targetHedgePx = hedgeBuyQ ? bidPxs_[1] : askPxs_[1];
			    activeHedger(targetHedgePx, positionDelta, hedgeSide, 1, false);
			    hasMdUpdateFront_ = false;
			    hasMdUpdateBack_ = false;
			} else if (hasFills_[0] || !pendingMessagesQ(hedgeSide)) {
			    logStrategyStatus("", ",SpreadOff: front leg take profit, hit/lift back leg");
			    targetActivePx = hedgeBuyQ ? Px(lastTradeSellPxs_[0] - takeProfitOrderPriceLimit_, tickSize_) : Px(lastTradeBuyPxs_[0] + takeProfitOrderPriceLimit_, tickSize_);
			    activeHedger(targetActivePx, positionDelta, hedgeSide, 0, false);
			    targetHedgePx = hedgeBuyQ ? Px::round(lastTradeSellPxs_[0] + expSpreadPx_, tickSize_, -1) : Px::round(lastTradeBuyPxs_[0] + expSpreadPx_, tickSize_, 1);
			    activeHedger(targetHedgePx, positionDelta, hedgeSide, 1, false);
			}
		    }
		}
	    }
	}
    }

}

// SMMS_CPP
