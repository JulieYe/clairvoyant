//
// File:        $ SpreadMM.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2014/01/22 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2014 by Zhengzhang Futures Co. Ltd.

#include "SpreadMM.hpp"

namespace simulator {

    SpreadMM::SpreadMM(string exchange, string contractFront, string contractBack, double multiple, double tickSize, string startTradingTimeString, string exitNoonTimeString, string startNoonTimeString, string exitTradingTimeString, SimulatorCore * simcore_ptr, Logger * logger_ptr) :
	Strategy("SpreadMM", simcore_ptr, logger_ptr),
	multiplier_(multiple),
	tickSize_(tickSize),
	startTradingTimeString_(startTradingTimeString),
	exitNoonTimeString_(exitNoonTimeString),
	startNoonTimeString_(startNoonTimeString),
	exitTradingTimeString_(exitTradingTimeString), 
	iSpreadPtr_(new ImpliedSpreadPrice<static_cast<int>(BuySellSide::mid)>()), 
	maPxPtr_(new ImpliedSpreadPriceSmooth<static_cast<int>(BuySellSide::mid), 4, 10>()),
	volPtr_(new HighFrequencyVolatility<60,2>())
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
    }

    string SpreadMM::getStatusString()
    {
	stringstream ss;
	ss << ",("
	   << getOpenPosition(0) << "," 
	   << pendingLimitOrderMessages_[0].size() << "," 
	   << pendingCancelLimitOrderMessages_[0].size() << "," 
	   << (simulatorCorePtr_->getExchanges()).front()->buyOpenQty() << "," 
	   << (simulatorCorePtr_->getExchanges()).front()->sellOpenQty() << "," 
	   << (simulatorCorePtr_->getExchanges()).front()->buyOpenOrderQty() << "," 
	   << (simulatorCorePtr_->getExchanges()).front()->buyCloseOrderQty() << "," 
	   << (simulatorCorePtr_->getExchanges()).front()->sellOpenOrderQty() << "," 
	   << (simulatorCorePtr_->getExchanges()).front()->sellCloseOrderQty() << "),("
	   << getOpenPosition(1) << "," 
	   << pendingLimitOrderMessages_[1].size() << "," 
	   << pendingCancelLimitOrderMessages_[1].size() << "," 
	   << (simulatorCorePtr_->getExchanges()).back()->buyOpenQty() << "," 
	   << (simulatorCorePtr_->getExchanges()).back()->sellOpenQty() << "," 
	   << (simulatorCorePtr_->getExchanges()).back()->buyOpenOrderQty() << "," 
	   << (simulatorCorePtr_->getExchanges()).back()->buyCloseOrderQty() << "," 
	   << (simulatorCorePtr_->getExchanges()).back()->sellOpenOrderQty() << "," 
	   << (simulatorCorePtr_->getExchanges()).back()->sellCloseOrderQty() << "),(" 
	   << iSpreadPtr_->getValue() << "," 
	   << maPxPtr_->getValue() << ","
	   << volPtr_->getValue() << "),("
	   << bidQtys_[0] << ","
	   << bidPxs_[0].getPx() << ","
	   << askPxs_[0].getPx() << ","
	   << askQtys_[0] << "),("
	   << bidQtys_[1] << ","
	   << bidPxs_[1].getPx() << ","
	   << askPxs_[1].getPx() << ","
	   << askQtys_[1] << ")";
	return ss.str();
    }

    void SpreadMM::setEdges(double enterEdge, double exitEdge)
    {
	enterEdge_ = enterEdge;
	cout << "Enter edge is set to " << enterEdge_ << endl;

	exitEdge_ = exitEdge;
	cout << "Exit edge is set to " << exitEdge_ << endl;
    }

    void SpreadMM::setBuffer(double buffer)
    {
	buffer_ = buffer;
	cout << "Buffer is set to " << buffer_ << endl;
    }

    void SpreadMM::setOrderSize(int ordersize)
    {
	orderSize_ = ordersize;
	cout << "Order size is set to " << orderSize_ << endl;
    }

    void SpreadMM::setMaxSpreadPosition(int maxSpreadPosition) 
    {
	maxSpreadPosition_ = maxSpreadPosition;
	cout << "Max position of spread is set to " << maxSpreadPosition_ << endl;
    }

    void SpreadMM::setStopLoss(double stoploss)
    {
	stopLossOrderLimit_ = stoploss;
	cout << "Stop loss is set to " << stopLossOrderLimit_ << endl;
    }

    void SpreadMM::setMaxCancelQty(int maxCancelQty) 
    {
	maxCancelQty_ = maxCancelQty;
	cout << "Maximum cancel quantity is set to " << maxCancelQty_ << endl;
    }

    bool SpreadMM::exceedMaxSpreadPositionQ() const 
    {
	if (abs(spreadPosition_) >= maxSpreadPosition_) {
	    return true;
	} else {
	    return false;
	}
    }

    double SpreadMM::getStaticOpenPnL(BuySellSide side, int prodId) const 
    {
	double ret = 0.0;
	if (side == BuySellSide::buy) {
	    ret = (openPnlRefPxs_[prodId] - lastTradeBuyPxs_[prodId]) * multiplier_;
	} else if (side == BuySellSide::sell) {
	    ret = (lastTradeSellPxs_[prodId] - openPnlRefPxs_[prodId]) * multiplier_;
	}
	return ret;
    }

    bool SpreadMM::pendingOrderMessagesQ(BuySellSide side, bool frontQ) const 
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

    bool SpreadMM::pendingOrderMessagesQ(BuySellSide side) const 
    {
	return pendingOrderMessagesQ(side, true) || pendingOrderMessagesQ(side, false);
    }

    bool SpreadMM::pendingOrderMessagesQ(bool frontQ) const 
    {
	return pendingOrderMessagesQ(BuySellSide::buy, frontQ) || pendingOrderMessagesQ(BuySellSide::sell, frontQ);
    }

    bool SpreadMM::pendingOrderMessagesQ() const 
    {
	return pendingOrderMessagesQ(true) || pendingOrderMessagesQ(false);
    }

    bool SpreadMM::pendingCancelMessagesQ(BuySellSide side, bool frontQ) const 
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

    bool SpreadMM::pendingCancelMessagesQ(BuySellSide side) const 
    {
	return pendingCancelMessagesQ(side, true) || pendingCancelMessagesQ(side, false);
    }

    bool SpreadMM::pendingCancelMessagesQ(bool frontQ) const 
    {
	return pendingCancelMessagesQ(BuySellSide::buy, frontQ) || pendingCancelMessagesQ(BuySellSide::sell, frontQ);
    }

    bool SpreadMM::pendingCancelMessagesQ() const 
    {
	return pendingCancelMessagesQ(true) || pendingCancelMessagesQ(false);
    }

    bool SpreadMM::pendingMessagesQ(BuySellSide side, bool frontQ) const 
    {
	return pendingOrderMessagesQ(side, frontQ) || pendingCancelMessagesQ(side, frontQ);
    }

    bool SpreadMM::pendingMessagesQ(BuySellSide side) const 
    {
	return pendingOrderMessagesQ(side) || pendingCancelMessagesQ(side);
    }

    bool SpreadMM::pendingMessagesQ(bool frontQ) const 
    {
	return pendingOrderMessagesQ(frontQ) || pendingCancelMessagesQ(frontQ);
    }

    bool SpreadMM::pendingMessagesQ() const 
    {
	return pendingOrderMessagesQ() || pendingCancelMessagesQ();
    }

    void SpreadMM::quotePositions4way(bool qCancelOnly) 
    {
	spreadOnOff_ = true;
	expSpreadPx_ = maPxPtr_->getValue();
	if (bidQtys_[1] >= bidQtyQualifier_) { 
	    activeHedger(Px::round(bidPxs_[1].getPx() - expSpreadPx_ - enterEdge_ - buffer_, tickSize_, -1), 
			  orderSize_, 
			  BuySellSide::buy, 
			  0, 
			  qCancelOnly);
	} else { 
	    cancelAllLimitOrders(BuySellSide::buy, 0);
	}
	if (askQtys_[1] >= askQtyQualifier_) { 
	    activeHedger(Px::round(askPxs_[1].getPx() - expSpreadPx_ + enterEdge_ + buffer_, tickSize_, 1), 
			  orderSize_, 
			  BuySellSide::sell, 
			  0, 
			  qCancelOnly);
	} else { 
	    cancelAllLimitOrders(BuySellSide::sell, 0);
	}
	activeHedger(Px::round(bidPxs_[0].getPx() + expSpreadPx_ - enterEdge_ - buffer_, tickSize_, -1), 
		     orderSize_, 
		     BuySellSide::buy, 
		     1, 
		     qCancelOnly);
	activeHedger(Px::round(askPxs_[0].getPx() + expSpreadPx_ + enterEdge_ + buffer_, tickSize_, 1), 
		     orderSize_, 
		     BuySellSide::sell, 
		     1, 
		     qCancelOnly);
    }

    void SpreadMM::quotePositions2way(int targetQty, BuySellSide side, bool qCancelOnly) 
    {
	spreadOnOff_ = false;
	expSpreadPx_ = maPxPtr_->getValue();
	if (side == BuySellSide::buy) {
	    if (askQtys_[1] >= askQtyQualifier_) { 
		activeHedger(Px::round(askPxs_[1].getPx() - expSpreadPx_ + exitEdge_ + buffer_, tickSize_, 1), 
			      targetQty, 
			      BuySellSide::sell, 
			      0, 
			      qCancelOnly);
	    } else { 
		cancelAllLimitOrders(BuySellSide::sell, 0);
	    }
	    activeHedger(Px::round(bidPxs_[0].getPx() + expSpreadPx_ - exitEdge_ - buffer_, tickSize_, -1), 
			 targetQty, 
			 BuySellSide::buy, 
			 1, 
			 qCancelOnly);
	} else {
	    if (bidQtys_[1] >= bidQtyQualifier_) { 
		activeHedger(Px::round(bidPxs_[1].getPx() - expSpreadPx_ - exitEdge_ - buffer_, tickSize_, -1), 
			      targetQty, 
			      BuySellSide::buy, 
			      0, 
			      qCancelOnly);
	    } else { 
		cancelAllLimitOrders(BuySellSide::buy, 0);
	    }
	    activeHedger(Px::round(askPxs_[0].getPx() + expSpreadPx_ + exitEdge_ + buffer_, tickSize_, 1), 
			 targetQty, 
			 BuySellSide::sell, 
			 1, 
			 qCancelOnly);
	}
    }

    void SpreadMM::activeHedger(Px targetPx, int hedgeQty, BuySellSide hedgeSide, int prodId, bool qCancelOnly) 
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

    void SpreadMM::take(int position) 
    {
	// Throttle stacking positions on the direction of filled order.
	if (!pendingMessagesQ(fillSide_, true)) { 
	    cancelAllLimitOrders(fillSide_, 0);
	}
	if (!pendingMessagesQ(fillSide_, false)) {
	    cancelAllLimitOrders(fillSide_, 1);
	}
	stopLoss_ = getStaticOpenPnL(fillSide_, fillIdx_) < stopLossOrderLimit_;
	stopLoss_ = stopLoss_ || ((currTime_ - lastFillTss_[fillIdx_]).count() >= quoteTimeout_);
	// If filled leg is stopped loss, first cancel all resting hedge leg orders.
	if (hasMdUpdates_[fillIdx_] && stopLoss_ && orderbooks_[1-fillIdx_].countLimitOrderQty(hedgeSide_) > 0) {
	    if (spreadOnOff_) { 
		logStrategyStatus("", ",Spread on: stop loss");
	    } else { 
		logStrategyStatus("", ",Spread off: stop loss");
	    }
	    if (!pendingMessagesQ(hedgeSide_, !fillFrontQ_)) {
		cancelAllLimitOrders(hedgeSide_, 1-fillIdx_);
	    }
	    return;
	}
	// If filled leg is stopped loss and no hedging orders rest, stop loss.
	if (hasMdUpdates_[fillIdx_] && stopLoss_ && 
	    !pendingMessagesQ(hedgeSide_, fillFrontQ_)) { 
	    if (spreadOnOff_) { 
		logStrategyStatus("", ",Spread on: stop loss");
	    } else { 
		logStrategyStatus("", ",Spread off: stop loss");
	    }
	    activeHedger(hedgeBuyQ_ ? 
			  Px(max(bidPxs_[fillIdx_].getPx(), lastTradeSellPxs_[fillIdx_]), tickSize_) : 
			  Px(min(askPxs_[fillIdx_].getPx(), lastTradeBuyPxs_[fillIdx_]), tickSize_), 
			 abs(position), 
			 hedgeSide_, 
			 fillIdx_, 
			 false);
	    hasMdUpdates_[fillIdx_] = false;
	}
	// If filled leg is not stopped loss, take profit by spreading the other leg.
	else if (!stopLoss_ && (hasFills_[fillIdx_] || !pendingMessagesQ(hedgeSide_, !fillFrontQ_))) {
	    if (spreadOnOff_) { 
		logStrategyStatus("", ",Spread on: take profit");
	    } else {
		logStrategyStatus("", ",Spread off: take profit");
	    }
	    // cancel stop loss order
	    if (!pendingMessagesQ(hedgeSide_, fillFrontQ_)) { 
		cancelAllLimitOrders(hedgeSide_, fillIdx_);
	    }
	    // establish winning spread by taking appropriate position of hedge leg
	    if (fillFrontQ_ && hedgeBuyQ_) { 
		targetHedgePx_ = Px::round(lastTradeSellPxs_[0] + expSpreadPx_ - (spreadOnOff_ ? enterEdge_ : exitEdge_), 
					   tickSize_, 
					   -1);
	    } else if (fillFrontQ_ && !hedgeBuyQ_) { 
		targetHedgePx_ = Px::round(lastTradeBuyPxs_[0] + expSpreadPx_ + (spreadOnOff_ ? enterEdge_ : exitEdge_), 
					   tickSize_, 
					   1);
	    } else if (!fillFrontQ_ && hedgeBuyQ_) { 
		targetHedgePx_ = Px::round(lastTradeSellPxs_[1] - expSpreadPx_ - (spreadOnOff_ ? enterEdge_ : exitEdge_), 
					   tickSize_, 
					   -1);
	    } else { 
		targetHedgePx_ = Px::round(lastTradeBuyPxs_[1] - expSpreadPx_ + (spreadOnOff_ ? enterEdge_ : exitEdge_), 
					   tickSize_, 
					   1);
	    }
	    activeHedger(targetHedgePx_, abs(position), hedgeSide_, 1-fillIdx_, false);
	}
    }

    void SpreadMM::reconcilePositions() 
    {
	// imply spread position first
	if (getOpenPosition(0) * getOpenPosition(1) < 0) {
	    spreadPosition_ = getOpenPosition(1) > 0 ? min(-getOpenPosition(0), getOpenPosition(1)) : -min(getOpenPosition(0), -getOpenPosition(1));
	} else {
	    spreadPosition_ = 0;
	}
	positionFront_ = getOpenPosition(0) + spreadPosition_;
	positionBack_ = getOpenPosition(1) - spreadPosition_;
    }

    void SpreadMM::reconcileSides(bool frontQ) 
    {
	if (frontQ) { 
	    if (positionFront_ < 0 && spreadOnOff_) {
		fillSide_ = BuySellSide::sell;
		fillIdx_ = 0;
	    } else if (positionFront_ < 0 && !spreadOnOff_) {
		fillSide_ = BuySellSide::sell;
		fillIdx_ = 1;
	    } else if (positionFront_ > 0 && spreadOnOff_) { 
		fillSide_ = BuySellSide::buy;
		fillIdx_ = 0;
	    } else {
		fillSide_ = BuySellSide::buy;
		fillIdx_ = 1;
	    }
	} else { 
	    if (positionBack_ < 0 && spreadOnOff_) {
		fillSide_ = BuySellSide::sell;
		fillIdx_ = 1;
	    } else if (positionBack_ < 0 && !spreadOnOff_) { 
		fillSide_ = BuySellSide::sell;
		fillIdx_ = 0;
	    } else if (positionBack_ > 0 && spreadOnOff_) { 
		fillSide_ = BuySellSide::buy;
		fillIdx_ = 1;
	    } else {
		fillSide_ = BuySellSide::buy;
		fillIdx_ = 0;
	    }
	}
	fillFrontQ_ = (fillIdx_ == 0) ? true : false;
	hedgeSide_ = (fillSide_ == BuySellSide::buy) ? BuySellSide::sell : BuySellSide::buy;
	hedgeBuyQ_ = (hedgeSide_ == BuySellSide::buy) ? true : false;
    }

    void SpreadMM::calcBuffer() 
    {
	if (volPtr_->isGood()) { 
	    vol_ = volPtr_->getValue();

	    if (vol_ <= 0.1) { 
		buffer_ = 0.2;
	    } else if (vol_ <= 0.25) { 
		buffer_ = 0.4;
	    } else {
		buffer_ = 0.6;
	    }
	}
    }
    
    void SpreadMM::process()
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
	    getPnL() < -1000.0 * orderSize_) {
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
		volPtr_->update(*ibu);
		if (ibu->getBidQty()[0] > 0) {
		    bidPxs_[0] = ibu->getBidPx()[0];
		    bidQtys_[0] = ibu->getBidQty()[0];
		}
		if (ibu->getAskQty()[0] > 0) {
		    askPxs_[0] = ibu->getAskPx()[0];
		    askQtys_[0] = ibu->getAskQty()[0];
		}
		++ibu;
		++itu;
	    }
	    hasMdUpdates_[0] = true;
	    calcBuffer();
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
		++ibu;
		++itu;
	    }
	    hasMdUpdates_[1] = true;
	}

	logStrategyStatus(",OpenPositionFront,PendingOrderFront,PendingCancelFront,BuyOpenPositionFront,SellOpenPositionFront,BuyOpenOrderQtyFront,BuyCloseOrderQtyFront,SellOpenOrderQtyFront,SellCloseOrderQtyFront,OpenPositionBack,PendingOrderBack,PendingCancelBack,BuyOpenPositionBack,SellOpenPositionBack,BuyOpenOrderQtyBack,BuyCloseOrderQtyBack,SellOpenOrderQtyBack,SellCloseOrderQtyBack,MidPxSpread,MaPxSpread,Vol,BidQtyFront,BidPxFront,AskPxFront,AskQtyFront,BidQtyBack,BidPxBack,AskPxBack,AskQtyBack", getStatusString());

	if (!enableTrading_) {
	    return;
	}

	if (closePosition_) {
	    if (getOpenPosition(0) == 0 && getOpenPosition(1) == 0) { 
		logStrategyStatus("", ",Close position: cancel all resting orders");
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
		    logStrategyStatus("", ",Close position: sell front leg");
		    if (!pendingMessagesQ(BuySellSide::buy, true)) { 
			cancelAllLimitOrders(BuySellSide::buy, 0);
		    }
		    if (!pendingMessagesQ(BuySellSide::sell, true)) { 
			activeHedger(askPxs_[0], abs(getOpenPosition(0)), BuySellSide::sell, 0, false);
		    }
		} else if (getOpenPosition(0) < 0) { 
		    logStrategyStatus("", ",Close position: buy front leg");
		    if (!pendingMessagesQ(BuySellSide::sell, true)) {
			cancelAllLimitOrders(BuySellSide::sell, 0);
		    }
		    if (!pendingMessagesQ(BuySellSide::buy, true)) { 
			activeHedger(bidPxs_[0], abs(getOpenPosition(0)), BuySellSide::buy, 0, false);
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
		    logStrategyStatus("", ",Close position: sell back leg");
		    if (!pendingMessagesQ(BuySellSide::buy, false)) { 
			cancelAllLimitOrders(BuySellSide::buy, 1);
		    }
		    if (!pendingMessagesQ(BuySellSide::sell, false)) { 
			activeHedger(Px(askPxs_[1].getPx() - tickSize_, tickSize_), abs(getOpenPosition(1)), BuySellSide::sell, 1, false);
		    }
		} else if (getOpenPosition(1) < 0) {
		    logStrategyStatus("", ",Close position: buy back leg");
		    if (!pendingMessagesQ(BuySellSide::sell, false)) { 
			cancelAllLimitOrders(BuySellSide::sell, 1);
		    }
		    if (!pendingMessagesQ(BuySellSide::buy, false)) {
			activeHedger(Px(bidPxs_[1].getPx() + tickSize_, tickSize_), abs(getOpenPosition(1)), BuySellSide::buy, 1, false);
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
		logStrategyStatus("", ",Close spread position: buy spread");
		if (!pendingMessagesQ(BuySellSide::buy, true)) { 
		    cancelAllLimitOrders(BuySellSide::buy, 0);
		}
		if (!pendingMessagesQ(BuySellSide::sell, false)) { 
		    cancelAllLimitOrders(BuySellSide::sell, 1);
		}
		if (!pendingMessagesQ(BuySellSide::sell, true) && !pendingMessagesQ(BuySellSide::buy, false)) { 
		    quotePositions2way(abs(getOpenPosition(1)), BuySellSide::buy, false);
		}
	    } else {
		logStrategyStatus("", ",Close spread position: sell spread");
		if (!pendingMessagesQ(BuySellSide::sell, true)) { 
		    cancelAllLimitOrders(BuySellSide::sell, 0);
		}
		if (!pendingMessagesQ(BuySellSide::buy, false)) { 
		    cancelAllLimitOrders(BuySellSide::buy, 1);
		}
		if (!pendingMessagesQ(BuySellSide::buy, true) && !pendingMessagesQ(BuySellSide::sell, false)) { 
		    quotePositions2way(abs(getOpenPosition(1)), BuySellSide::sell, false);
		}
	    }
	    return;
	}

	reconcilePositions();

	if (positionFront_ == 0) {
	    if (positionBack_ == 0) {
		if (stopQuoting_) { 
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
		} else if (spreadPosition_ == 0 && !pendingMessagesQ() && hasMdUpdate() && maPxPtr_->isGood()) {
		    logStrategyStatus("", ",Quote 4 ways");
		    quotePositions4way(false);
		    hasMdUpdates_[0] = false;
		    hasMdUpdates_[1] = false;
		} else if (spreadPosition_ < 0) { 
		    if (!pendingMessagesQ(BuySellSide::buy, true)) { 
			cancelAllLimitOrders(BuySellSide::buy, 0);
		    }
		    if (!pendingMessagesQ(BuySellSide::sell, false)) { 
			cancelAllLimitOrders(BuySellSide::sell, 1);
		    }
		    if (hasMdUpdate() && maPxPtr_->isGood() && 
			!pendingMessagesQ(BuySellSide::sell, true) && 
			!pendingMessagesQ(BuySellSide::buy, false)) { 
			logStrategyStatus("", ",Quote 2 ways: buy spread");
			quotePositions2way(abs(spreadPosition_), 
					   BuySellSide::buy, 
					   false);
			hasMdUpdates_[0] = false;
			hasMdUpdates_[1] = false;
		    }
		} else if (spreadPosition_ > 0) { 
		    if (!pendingMessagesQ(BuySellSide::sell, true)) { 
			cancelAllLimitOrders(BuySellSide::sell, 0);
		    }
		    if (!pendingMessagesQ(BuySellSide::buy, false)) { 
			cancelAllLimitOrders(BuySellSide::buy, 1);
		    }
		    if (hasMdUpdate() && maPxPtr_->isGood() && 
			!pendingMessagesQ(BuySellSide::buy, true) && 
			!pendingMessagesQ(BuySellSide::sell, false)) { 
			logStrategyStatus("", ",Quote 2 ways: sell spread");
			quotePositions2way(abs(spreadPosition_), 
					   BuySellSide::sell, 
					   false);
			hasMdUpdates_[0] = false;
			hasMdUpdates_[1] = false;
		    }
		}
	    } else {
		reconcileSides(false);
		take(positionBack_);
	    }
	} else {
	    if (positionBack_ == 0) {
		reconcileSides(true);
		take(positionFront_);
	    } else {
		if ((positionFront_ * positionBack_) > 0) {  // dangerous, both front and back legs are filled, get out ASAP
		    if (positionFront_ < 0) {
			fillSide_ = BuySellSide::sell;
			hedgeSide_ = BuySellSide::buy;
			hedgeBuyQ_ = true;
		    } else {
			fillSide_ = BuySellSide::buy;
			hedgeSide_ = BuySellSide::sell;
			hedgeBuyQ_ = false;
		    }
		    if (!pendingMessagesQ(fillSide_, true)) { 
			cancelAllLimitOrders(fillSide_, 0);
		    }
		    if (!pendingMessagesQ(fillSide_, false)) { 
			cancelAllLimitOrders(fillSide_, 1);
		    }
		    if (hasMdUpdates_[0] && !pendingMessagesQ(hedgeSide_, true)) {
			logStrategyStatus("", ",Spread on: scratch front leg position");
			activeHedger(hedgeBuyQ_ ? 
				     Px(max(bidPxs_[0].getPx(), lastTradeSellPxs_[0]), tickSize_) : 
				     Px(min(askPxs_[0].getPx(), lastTradeBuyPxs_[0]), tickSize_), 
				     abs(positionFront_), 
				     hedgeSide_, 
				     0, 
				     false);
			hasMdUpdates_[0] = false;
		    } else if (hasFills_[0] || !pendingMessagesQ(hedgeSide_, true)) {
			logStrategyStatus("", ",Spread on: scratch front leg position");
			activeHedger(hedgeBuyQ_ ? 
				     Px(max(bidPxs_[0].getPx(), lastTradeSellPxs_[0]), tickSize_) : 
				     Px(min(askPxs_[0].getPx(), lastTradeBuyPxs_[0]), tickSize_), 
				     abs(positionFront_), 
				     hedgeSide_, 
				     0, 
				     false);
		    }
		    if (hasMdUpdates_[1] && !pendingMessagesQ(hedgeSide_, false)) {
			logStrategyStatus("", ",Spread on: scratch back leg position");
			activeHedger(hedgeBuyQ_ ? 
				     Px(max(bidPxs_[1].getPx(), lastTradeSellPxs_[1]), tickSize_) : 
				     Px(min(askPxs_[1].getPx(), lastTradeBuyPxs_[1]), tickSize_), 
				     abs(positionBack_), 
				     hedgeSide_, 
				     1, 
				     false);
			hasMdUpdates_[1] = false;
		    } else if (hasFills_[1] || !pendingMessagesQ(hedgeSide_, false)) {
			logStrategyStatus("", ",Spread on: scratch back leg position");
			activeHedger(hedgeBuyQ_ ? 
				     Px(max(bidPxs_[1].getPx(), lastTradeSellPxs_[1]), tickSize_) : 
				     Px(min(askPxs_[1].getPx(), lastTradeBuyPxs_[1]), tickSize_), 
				     abs(positionBack_), 
				     hedgeSide_, 
				     1, 
				     false);
		    }
		} else if (positionFront_ != 0) {
		    reconcileSides(true);
		    take(positionFront_);
		} else {
		    reconcileSides(false);
		    take(positionBack_);
		}
	    }
	}
    }

}

// SpreadMM_CPP
