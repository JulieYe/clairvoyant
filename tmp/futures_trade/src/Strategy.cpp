//
// File:        $ Strategy.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2014/01/09 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2014 by Zhengzhang Futures Co. Ltd.

#include "Strategy.hpp"

namespace simulator
{

    Strategy::FillList::FillList(double multiplier) :
	multiplier_(multiplier)
    {}

    double Strategy::FillList::calcVWAP() const
    {
	int qty = accumulate(filledMessageList_.begin(),
			     filledMessageList_.end(),
			     0,
			     [&] (int qtyCumsum, FilledOrderMessage fom) { return qtyCumsum + fom.getQty(); });
	double cap = accumulate(filledMessageList_.begin(),
				filledMessageList_.end(),
				0.0,
				[&] (double capCumsum, FilledOrderMessage fom) { return capCumsum + fom.getPx().getPx() * fom.getQty(); });
	return (qty > 0) ? (cap/qty) : 0.0;
    }

    //constructor & destructor
    Strategy::Strategy(const string strat_name, SimulatorCore * simcore_ptr, Logger * logger_ptr) :
	strategyName_(strat_name),
	simulatorCorePtr_(simcore_ptr),
	loggerPtr_(logger_ptr)
    {}

#ifdef SIMULATION
    int Strategy::generateNewOrderId(string ticker)
    {
	int currOrderId;
	if (nextOrderId_ == -1) {
	    nextOrderId_ = stratId_;
	}
	currOrderId = nextOrderId_;
	nextOrderId_ += simulatorCorePtr_->getNumStrategies();
	return currOrderId;
    }
#endif

#ifdef PRODUCTION
    int Strategy::generateNewOrderId(string ticker)
    {
	int orderId = simulatorCorePtr_->orderId() + stratId_;
	simulatorCorePtr_->orderId() += simulatorCorePtr_->getNumStrategies();
	return orderId;
    }
#endif

    int Strategy::generateNewMessageId()
    {
	int currMessageId;
	if (nextMessageId_ == -1) {
	    nextMessageId_ = stratId_;
	}
	currMessageId = nextMessageId_;
	nextMessageId_ += simulatorCorePtr_->getNumStrategies();
	return currMessageId;
    }

    Strategy::~Strategy()
    {
	for(unsigned ii=0; ii<numProducts_; ii++) {
	    delete filledMessageLists_[ii];
	}
    }

    // extend strategy
    void Strategy::addStrat(string ex, string con, double mult)
    {
	++numProducts_;
	orderbooks_.push_back(OrderBook());
	bookUpdates_.push_back(list<BookUpdate<true> >());
	tradeUpdates_.push_back(list<TradeUpdate<true> >());

	//new tracking
	midPrices_.push_back(0.0);
	closedPnls_.push_back(0.0);
	openPnls_.push_back(0.0);
	openPnlRefPxs_.push_back(0.0);
	tradedVolumes_.push_back(0);
	openPositions_.push_back(0);
	openBuyOrderCounts_.push_back(0);
	openSellOrderCounts_.push_back(0);
	limitOrderCounts_.push_back(0);
	marketOrderCounts_.push_back(0);
	fillCounts_.push_back(0);
	cancelCounts_.push_back(0);

	//new indexing
	tickers_.push_back(ex + ":" + con);
	exchanges_.push_back(ex);
	contracts_.push_back(con);

	//new caching
	filledMessageLists_.push_back(new FillList(mult));
	lastTradeBuyPxs_.push_back(0.0);
	lastTradeSellPxs_.push_back(0.0);
	lastTradeSides_.push_back(BuySellSide::unknown);
	hasFills_.push_back(false);
	lastFillTss_.push_back(simulatorCorePtr_->getCurrTime());
	
	acknowledgedOrderMessages_.push_back(list<AcknowledgedOrderMessage>());
	filledOrderMessages_.push_back(list<FilledOrderMessage>());
	canceledLimitOrderMessages_.push_back(list<CanceledOrderMessage<LimitOrderMessage> >());
	canceledMarketOrderMessages_.push_back(list<CanceledOrderMessage<MarketOrderMessage> >());
	rejectedLimitOrderMessages_.push_back(list<RejectedOrderMessage<LimitOrderMessage> >());
	rejectedMarketOrderMessages_.push_back(list<RejectedOrderMessage<MarketOrderMessage> >());
	rejectedCancelLimitOrderMessages_.push_back(list<RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage> > >());
	rejectedCancelMarketOrderMessages_.push_back(list<RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage> > >());
    
	pendingLimitOrderMessages_.push_back(list<LimitOrderMessage>());
	pendingMarketOrderMessages_.push_back(list<MarketOrderMessage>());
	pendingCancelLimitOrderMessages_.push_back(list<CancelOrderMessage<LimitOrderMessage> >());
	pendingCancelMarketOrderMessages_.push_back(list<CancelOrderMessage<MarketOrderMessage> >());
    }
  
    // get status of the strategy
    double Strategy::getClosedPnL() const
    {
	return accumulate(closedPnls_.begin(), closedPnls_.end(), 0.0);
    }

    double Strategy::getOpenPnL() const
    {
	return accumulate(openPnls_.begin(), openPnls_.end(), 0.0);
    }

    double Strategy::getPnL() const 
    {
	return getClosedPnL() + getOpenPnL();
    }

    int Strategy::getVolume() const 
    {
	return accumulate(tradedVolumes_.begin(), tradedVolumes_.end(), 0);
    }

    int Strategy::getOpenPosition() const 
    {
	return accumulate(openPositions_.begin(), openPositions_.end(), 0);
    }

    int Strategy::getOpenBuyOrderCount() const
    {
	return accumulate(openBuyOrderCounts_.begin(), openBuyOrderCounts_.end(), 0);
    }

    int Strategy::getOpenSellOrderCount() const
    {
	return accumulate(openSellOrderCounts_.begin(), openSellOrderCounts_.end(), 0);
    }

    int Strategy::getOpenOrderCount() const
    {
	return getOpenBuyOrderCount() + getOpenSellOrderCount();
    }
  
    int Strategy::getLimitOrderCount() const
    {
	return accumulate(limitOrderCounts_.begin(), limitOrderCounts_.end(), 0);
    }

    int Strategy::getMarketOrderCount() const
    {
	return accumulate(marketOrderCounts_.begin(), marketOrderCounts_.end(), 0);
    }

    int Strategy::getFillCount() const
    {
	return accumulate(fillCounts_.begin(), fillCounts_.end(), 0);
    }

    int Strategy::getCancelCount() const
    {
	return accumulate(cancelCounts_.begin(), cancelCounts_.end(), 0);
    }

    void Strategy::printStrategyStatus()
    { 
	std::cout << SimulatorCore::convertTimePointToString(currTime_) << "\n"
		  << "Ts = " << currTime_.time_since_epoch().count() << "\n"
		  << "PNL = " << getPnL() << "\n"
		  << "ClosedPNL = " << getClosedPnL() << "\n"
		  << "OpenPNL = " << getOpenPnL() << "\n"
		  << "Volume = " << getVolume() << "\n"
		  << "OpenPosition = " << getOpenPosition() << "\n"
		  << "OpenBuyLimitOrder = " << getOpenBuyOrderCount() << "\n"
		  << "OpenSellLimitOrder = " << getOpenSellOrderCount() << "\n"
		  << "FillCount = " << getFillCount() << "\n"
		  << "LimitOrderCount = " << getLimitOrderCount() << "\n"
		  << "MarketOrderCount = " << getMarketOrderCount() << "\n"
		  << "CancelCount = " << getCancelCount() << std::endl;
    }
  
    void Strategy::logStrategyStatus(std::string header, std::string content)
    {
	//push status stuff to logger
	if(!headerLogged_) {
	    stringstream ss;
	    ss << "Name" << ","
	       << "Time" << ","
	       << "Ts" << ","
	       << "PNL" << ","
	       << "ClosedPNL" << ","
	       << "OpenPNL" << ","
	       << "Volume"  << ","
	       << "OpenPosition" << ","
	       << "OpenBuyLimitOrder" << ","
	       << "OpenSellLimitOrder" << ","
	       << "FillCount" << ","
	       << "LimitOrderCount" << ","
	       << "MarketOrderCount" << ","
	       << "CancelCount";
	    ss << header;
	    loggerPtr_->push(ss.str());
	    headerLogged_ = true;
	}
	    
	stringstream ss;
	ss << "StrategyStatus" << ","
	   << SimulatorCore::convertTimePointToString(currTime_) << ","
	   << currTime_.time_since_epoch().count() << ","
	   << getPnL() << ","
	   << getClosedPnL() << ","
	   << getOpenPnL() << ","
	   << getVolume() << ","
	   << getOpenPosition() << ","
	   << getOpenBuyOrderCount() << ","
	   << getOpenSellOrderCount() << ","
	   << getFillCount() << ","
	   << getLimitOrderCount() << ","
	   << getMarketOrderCount() << ","
	   << getCancelCount() ;
	ss << content;
	loggerPtr_->push(ss.str());
    }
  
    void Strategy::logStrategyStatus()
    {
	logStrategyStatus("", "");
    }

    // process information
    void Strategy::process()
    {
	//right now does nothing here
	//supposed to process all incoming messages, accessing book/trade updates (not limited to local ones)
	//send messages to outgoing messages
	//push logging stuff to logger
    }
  
    //update tracking fields and caches
    void Strategy::updateStatus()
    {
	// update status on acks
	for(unsigned ii=0; ii<numProducts_; ii++) {
	    for_each(acknowledgedOrderMessages_[ii].begin(), acknowledgedOrderMessages_[ii].end(),
		     [&](AcknowledgedOrderMessage msg) {updateStatusOnMessage(msg, ii);});
	}
	// update status on fills
	for(unsigned ii=0; ii<numProducts_; ii++) {
	    for_each(filledOrderMessages_[ii].begin(), filledOrderMessages_[ii].end(),
		     [&](FilledOrderMessage msg) {updateStatusOnMessage(msg, ii);});
	    fillCounts_[ii] += filledOrderMessages_[ii].size();
	}
	// update status on cancel limits
	for(unsigned ii=0; ii<numProducts_; ii++) {
	    for_each(canceledLimitOrderMessages_[ii].begin(), canceledLimitOrderMessages_[ii].end(),
		     [&](CanceledOrderMessage<LimitOrderMessage> msg) {updateStatusOnMessage(msg, ii);});
	}
	// update status on cancel markets
	for(unsigned ii=0; ii<numProducts_; ii++) {
	    for_each(canceledMarketOrderMessages_[ii].begin(), canceledMarketOrderMessages_[ii].end(),
		     [&](CanceledOrderMessage<MarketOrderMessage> msg) {updateStatusOnMessage(msg, ii);});
	}
	// update status on rejected limits
	for(unsigned ii=0; ii<numProducts_; ii++) {
	    for_each(rejectedLimitOrderMessages_[ii].begin(), rejectedLimitOrderMessages_[ii].end(),
		     [&](RejectedOrderMessage<LimitOrderMessage> msg) {updateStatusOnMessage(msg, ii);});
	}
	// update status on reject markets
	for(unsigned ii=0; ii<numProducts_; ii++) {
	    for_each(rejectedMarketOrderMessages_[ii].begin(), rejectedMarketOrderMessages_[ii].end(),
		     [&](RejectedOrderMessage<MarketOrderMessage> msg) {updateStatusOnMessage(msg, ii);});
	}
	// update status on rejected cancel limits
	for(unsigned ii=0; ii<numProducts_; ii++) {
	    for_each(rejectedCancelLimitOrderMessages_[ii].begin(), rejectedCancelLimitOrderMessages_[ii].end(),
		     [&](RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage> > msg) {updateStatusOnMessage(msg, ii);});
	}
	// update status on reject cancel markets
	for(unsigned ii=0; ii<numProducts_; ii++) {
	    for_each(rejectedCancelMarketOrderMessages_[ii].begin(), rejectedCancelMarketOrderMessages_[ii].end(),
		     [&](RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage> >  msg) {updateStatusOnMessage(msg, ii);});
	}

	for(unsigned ii=0; ii<numProducts_; ii++) {
	    for_each(bookUpdates_[ii].begin(), bookUpdates_[ii].end(),
		     [&](BookUpdate<true> bookupdate) {updateStatusOnMessage(bookupdate, ii);});
	}

	for(unsigned ii=0; ii<numProducts_; ii++) {
	    for_each(tradeUpdates_[ii].begin(), tradeUpdates_[ii].end(),
		     [&](TradeUpdate<true> tradeupdate) {updateStatusOnMessage(tradeupdate, ii);});
	}
    }
    
    void Strategy::updateStatusOnMessage(AcknowledgedOrderMessage message, unsigned prodId)
    {
	/* TODO */
	//only process acks for limit order ignores market order and cancel acks
	//basically removes the limit order from pending messages on ack

	if (loggerPtr_) {
	    loggerPtr_->push(message.logMessage());
	}

	if (!message.getAcknowledgeCancelQ()) {
	    pendingLimitOrderMessages_[prodId].erase(remove_if(pendingLimitOrderMessages_[prodId].begin(),
							       pendingLimitOrderMessages_[prodId].end(),
							       [&] (LimitOrderMessage msg) 
							       { 
								   return msg.getOrderId() == message.getOrderId();
							       }),
						     pendingLimitOrderMessages_[prodId].end());
	} 
	// else {
	//     pendingCancelLimitOrderMessages_[prodId].erase(remove_if(pendingCancelLimitOrderMessages_[prodId].begin(),
	// 							     pendingCancelLimitOrderMessages_[prodId].end(),
	// 							     [&] (CancelOrderMessage<LimitOrderMessage> msg) 
	// 							     { 
	// 								 return msg.getOrderId() == message.getOrderId();
	// 							     }),
	// 						   pendingCancelLimitOrderMessages_[prodId].end());
	//     pendingCancelMarketOrderMessages_[prodId].erase(remove_if(pendingCancelMarketOrderMessages_[prodId].begin(),
	// 							      pendingCancelMarketOrderMessages_[prodId].end(),
	// 							      [&] (CancelOrderMessage<MarketOrderMessage> msg) 
	// 							      { 
	// 								  return msg.getOrderId() == message.getOrderId();
	// 							      }),
	// 						    pendingCancelMarketOrderMessages_[prodId].end());
	// }
    }
    
    void Strategy::updateStatusOnMessage(RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage> > message, unsigned prodId)
    {
	if (loggerPtr_) {
	    loggerPtr_->push(message.logMessage());
	}
	//removes cancel from pending cancel messages
	pendingCancelLimitOrderMessages_[prodId].erase(remove_if(pendingCancelLimitOrderMessages_[prodId].begin(),
								 pendingCancelLimitOrderMessages_[prodId].end(),
								 [&] (CancelOrderMessage<LimitOrderMessage> msg) 
								 { 
								     return msg.getOrderId() == message.getOrderId(); 
								 }),
						       pendingCancelLimitOrderMessages_[prodId].end());
    }

    void Strategy::updateStatusOnMessage(RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage> > message, unsigned prodId)
    {
	if (loggerPtr_) {
	    loggerPtr_->push(message.logMessage());
	}

	//removes cancel from pending cancel messages
	pendingCancelMarketOrderMessages_[prodId].erase(remove_if(pendingCancelMarketOrderMessages_[prodId].begin(),
								  pendingCancelMarketOrderMessages_[prodId].end(),
								  [&] (CancelOrderMessage<MarketOrderMessage> msg) 
								  { 
								      return msg.getOrderId() == message.getOrderId(); 
								  }),
							pendingCancelMarketOrderMessages_[prodId].end());
    }

    void Strategy::updateStatusOnMessage(RejectedOrderMessage<LimitOrderMessage> message, unsigned prodId)
    {
	if (loggerPtr_) {
	    loggerPtr_->push(message.logMessage());
	}

	//removes limit order from pending messages
	pendingLimitOrderMessages_[prodId].erase(remove_if(pendingLimitOrderMessages_[prodId].begin(),
							   pendingLimitOrderMessages_[prodId].end(),
							   [&] (LimitOrderMessage msg) 
							   { 
							       return msg.getOrderId() == message.getOrderId(); 
							   }),
						 pendingLimitOrderMessages_[prodId].end());

	//removes limit order from orderbook
	//alternatively this can be done with a reject helper method in OrderBook class (save a construction operation)
	CancelOrderMessage<LimitOrderMessage> cxl(message.getExchangeId(),
						  message.getContractId(),
						  message.getTimestamp(),
						  message.getStratId(),
						  message.getMessageId(),
						  message.getOrderId(),
						  message.getQty(),
						  message.getSide(),
						  message.getPx());
	orderbooks_[prodId].cancelOrderHelper(&cxl);
    }

    void Strategy::updateStatusOnMessage(RejectedOrderMessage<MarketOrderMessage> message, unsigned prodId)
    {
	if (loggerPtr_) {
	    loggerPtr_->push(message.logMessage());
	}

	//removes market order from pending messages
	pendingMarketOrderMessages_[prodId].erase(remove_if(pendingMarketOrderMessages_[prodId].begin(),
							    pendingMarketOrderMessages_[prodId].end(),
							    [&] (MarketOrderMessage msg) 
							    { 
								return msg.getOrderId() == message.getOrderId(); 
							    }),
						  pendingMarketOrderMessages_[prodId].end());
    }     

    void Strategy::updateStatusOnMessage(FilledOrderMessage message, unsigned prodId)
    {
	if (loggerPtr_) {
	    loggerPtr_->push(message.logMessage());
	}

	// update openPnlRefPxs_[prodId] here.
	if (message.getTicker() == tickers_[prodId]) {
	    openPnlRefPxs_[prodId] = message.getPx().getPx();
	    hasFills_[prodId] = true;
	    lastFillTss_[prodId] = message.getTimestamp();
	}

	//removes limit order from pending messages if needed
	pendingLimitOrderMessages_[prodId].erase(remove_if(pendingLimitOrderMessages_[prodId].begin(),
							   pendingLimitOrderMessages_[prodId].end(),
							   [&] (LimitOrderMessage msg) 
							   { 
							       return msg.getOrderId() == message.getOrderId(); 
							   }),
						 pendingLimitOrderMessages_[prodId].end());

	//removes market order from pending messages if needed
        pendingMarketOrderMessages_[prodId].erase(remove_if(pendingMarketOrderMessages_[prodId].begin(),
                                                            pendingMarketOrderMessages_[prodId].end(),
                                                            [&] (MarketOrderMessage msg) 
							    { 
								return msg.getOrderId() == message.getOrderId() && 
								    msg.getQty() <= message.getQty(); 
							    }),
                                                  pendingMarketOrderMessages_[prodId].end());

        //reduce market order size
        for (auto imo = pendingMarketOrderMessages_[prodId].begin();
	     imo != pendingMarketOrderMessages_[prodId].end(); 
	     ++imo) {
            if (imo->getOrderId() == message.getOrderId()) {
		imo->qty() -= message.getQty();
            }
	}
	
	// removes cancel limit message from pending messages if needed
	// commented out for now
	// we trust exchange to send same number of response of cancel limit order message as the number of cancel limit orders
	// pendingCancelLimitOrderMessages_[prodId].erase(remove_if(pendingCancelLimitOrderMessages_[prodId].begin(),
	// 							 pendingCancelLimitOrderMessages_[prodId].end(),
	// 							 [&] (CancelOrderMessage<LimitOrderMessage> msg) 
	// 							 { 
	// 							     return msg.getOrderId() == message.getOrderId(); 
	// 							 }),
	// 					       pendingCancelLimitOrderMessages_[prodId].end());

	//removes cancel message from pending messages if needed
	// commented out for now
	// we trust exchange to send same number of response of cancel limit order message as the number of cancel limit orders
	// pendingCancelMarketOrderMessages_[prodId].erase(remove_if(pendingCancelMarketOrderMessages_[prodId].begin(),
	// 							  pendingCancelMarketOrderMessages_[prodId].end(),
	// 							  [&] (CancelOrderMessage<MarketOrderMessage> msg) 
	// 							  { 
	// 							      return msg.getOrderId() == message.getOrderId(); 
	// 							  }),
	// 						pendingCancelMarketOrderMessages_[prodId].end());
    
	//updates order book
	orderbooks_[prodId].filledOrderHelper(&message);
	//updates filllist, position, pnl, order counts etc.
	if((message.getSide() == BuySellSide::buy && openPositions_[prodId] < 0) ||
	   (message.getSide() == BuySellSide::sell&& openPositions_[prodId] > 0) ) {
	    //do some complicated stuff
	    int res_qty = message.getQty();
	    for(auto fil=filledMessageLists_[prodId]->filledMessageList_.begin();
		fil != filledMessageLists_[prodId]->filledMessageList_.end() && res_qty > 0; fil++) {
		int exe_qty = std::min(fil->getQty() , res_qty);
		closedPnls_[prodId] += (fil->getSide() == BuySellSide::buy ? (message.getPx().getPx() - fil->getPx().getPx()) * exe_qty * filledMessageLists_[prodId]->multiplier_ :
					(message.getPx().getPx() - fil->getPx().getPx()) * exe_qty * filledMessageLists_[prodId]->multiplier_ * (-1.0));
		openPositions_[prodId] += message.getSide() == BuySellSide::buy ? exe_qty : (-1) * exe_qty;
		tradedVolumes_[prodId] += exe_qty;
		fil->qty() = fil->getQty() - exe_qty;
		res_qty -= exe_qty;
	    }    
	    message.qty() = res_qty;
	    if(res_qty > 0) {
		filledMessageLists_[prodId]->filledMessageList_.push_back(message);
	    } 
	    filledMessageLists_[prodId]->filledMessageList_.erase(remove_if(filledMessageLists_[prodId]->filledMessageList_.begin(),
									    filledMessageLists_[prodId]->filledMessageList_.end(),
									    [&](FilledOrderMessage msg) { return msg.getQty() == 0; }),
								  filledMessageLists_[prodId]->filledMessageList_.end());
	} else {
	    filledMessageLists_[prodId]->filledMessageList_.push_back(message);
	}
	openPositions_[prodId] += message.getSide() == BuySellSide::buy ? message.getQty() : (-1) * message.getQty();
	tradedVolumes_[prodId] += message.getQty();
	computeOpenPnls(prodId);

	lastTradeProdId_ = prodId;
	if (message.getSide() == BuySellSide::buy) {
	    lastTradeBuyPxs_[prodId] = message.getPx().getPx();
	    lastTradeSides_[prodId] = BuySellSide::buy;
	} else {
	    lastTradeSellPxs_[prodId] = message.getPx().getPx();
	    lastTradeSides_[prodId] = BuySellSide::sell;
	}
    }

    void Strategy::updateStatusOnMessage(CanceledOrderMessage<LimitOrderMessage> message, unsigned prodId)
    {
	if (loggerPtr_) {
	    loggerPtr_->push(message.logMessage());
	}

	//removes pending limit order if needed
	pendingLimitOrderMessages_[prodId].erase(remove_if(pendingLimitOrderMessages_[prodId].begin(),
							   pendingLimitOrderMessages_[prodId].end(),
							   [&] (LimitOrderMessage msg) 
							   { 
							       return msg.getOrderId() == message.getOrderId(); 
							   }),
						 pendingLimitOrderMessages_[prodId].end());

	//removes pending cancal msg if needed
	pendingCancelLimitOrderMessages_[prodId].erase(remove_if(pendingCancelLimitOrderMessages_[prodId].begin(),
								 pendingCancelLimitOrderMessages_[prodId].end(),
								 [&] (CancelOrderMessage<LimitOrderMessage> msg) 
								 { 
								     return msg.getOrderId() == message.getOrderId(); 
								 }),
						       pendingCancelLimitOrderMessages_[prodId].end());

	//update order book
	CancelOrderMessage<LimitOrderMessage> cxl(message.getExchangeId(),
						  message.getContractId(),
						  message.getTimestamp(),
						  message.getStratId(),
						  message.getMessageId(),
						  message.getOrderId(),
						  message.getQty(),
						  message.getSide(),
						  message.getPx());

	orderbooks_[prodId].cancelOrderHelper(&cxl);
    }

    void Strategy::updateStatusOnMessage(CanceledOrderMessage<MarketOrderMessage> message, unsigned prodId)
    {
	if (loggerPtr_) {
	    loggerPtr_->push(message.logMessage());
	}

	//removes pending market order
	pendingMarketOrderMessages_[prodId].erase(remove_if(pendingMarketOrderMessages_[prodId].begin(),
							    pendingMarketOrderMessages_[prodId].end(),
							    [&] (MarketOrderMessage msg) 
							    { 
								return msg.getOrderId() == message.getOrderId(); 
							    }),
						  pendingMarketOrderMessages_[prodId].end());

	//removes pending cancel msg
	pendingCancelMarketOrderMessages_[prodId].erase(remove_if(pendingCancelMarketOrderMessages_[prodId].begin(),
								  pendingCancelMarketOrderMessages_[prodId].end(),
								  [&] (CancelOrderMessage<MarketOrderMessage> msg) 
								  { 
								      return msg.getOrderId() == message.getOrderId(); 
								  }),
							pendingCancelMarketOrderMessages_[prodId].end());
    }

    void Strategy::computeOpenPnls(unsigned prodId)
    {
	double open_pnl = 0.0, tmp_pnl = 0.0;
	filledMessageLists_[prodId]->filledMessageList_.erase(remove_if(filledMessageLists_[prodId]->filledMessageList_.begin(),
									filledMessageLists_[prodId]->filledMessageList_.end(),
									[&](FilledOrderMessage msg) { return msg.getQty() == 0; } ),
							      filledMessageLists_[prodId]->filledMessageList_.end());

	for(auto fil = filledMessageLists_[prodId]->filledMessageList_.begin();
	    fil != filledMessageLists_[prodId]->filledMessageList_.end();
	    fil++) {
	    tmp_pnl = (openPnlRefPxs_[prodId] - (*fil).getPx().getPx()) * (*fil).getQty();
	    tmp_pnl = (*fil).getSide() == BuySellSide::buy ? tmp_pnl : (-1.0) * tmp_pnl;
	    open_pnl += tmp_pnl;
	}
	openPnls_[prodId] = open_pnl * filledMessageLists_[prodId]->multiplier_;
    }

    void Strategy::updateStatusOnMessage(BookUpdate<true> message, unsigned prodId)
    {
	//update mid point
	message.getMidPx(&midPrices_[prodId]);
	//update open pnl reference price
	openPnlRefPxs_[prodId] = midPrices_[prodId];
	//update open pnl
	computeOpenPnls(prodId);
    }

    void Strategy::updateStatusOnMessage(TradeUpdate<true> message, unsigned prodId)
    {
	//NOT IMPLEMENTED FOR NOW
    }

    //  void Strategy::updateStatusOnMessage(BookUpdate<false> message, unsigned prodId) 
    //{  // NOT IMPLEMENTED FOR NOW
    //}
    //void Strategy::updateStatusOnMessage(TradeUpdate<false> message, unsigned prodId)
    //{  // NOT IMPLEMENTED FOR NOW
    //}
  
    void Strategy::update()
    {
	currTime_ = simulatorCorePtr_->getCurrTime();
	preProcess(); // copy inbound messages
	updateStatus(); // update strategy status
	process(); // strategies core function, logging, alpha computation, order/cancel generation all happens here
	postProcess(); // post process outbound messages, copy out outbound msg, clear inbound/outbound cache
    }
  
    //copy relevant messages to the correct place
    void Strategy::preProcess()
    {
	//copy relevant AcknowledgedOrderMessage
	for(unsigned ii=0; ii < numProducts_; ii++) {
	    auto ack = simulatorCorePtr_->acknowledgedOrderMessages();
	    list<AcknowledgedOrderMessage> ack_in;
	    list<AcknowledgedOrderMessage> ack_out;
	    partition_copy(ack.begin(),
			   ack.end(),
			   back_inserter(ack_in),
			   back_inserter(ack_out),
			   [&](AcknowledgedOrderMessage m) { return (m.getTicker() == tickers_[ii]) && (m.getStratId() == stratId_); });
	    simulatorCorePtr_->acknowledgedOrderMessages() = ack_out;
	    acknowledgedOrderMessages_[ii] = ack_in;
	}

	//copy relevant FilledOrderMessage
	for(unsigned ii=0; ii < numProducts_; ii++) {
	    auto fil = simulatorCorePtr_->filledOrderMessages();
	    list<FilledOrderMessage> fil_in;
	    list<FilledOrderMessage> fil_out;
	    partition_copy(fil.begin(),
			   fil.end(),
			   back_inserter(fil_in),
			   back_inserter(fil_out),
			   [&](FilledOrderMessage m) { return (m.getTicker() == tickers_[ii]) && (m.getStratId() == stratId_); });
	    simulatorCorePtr_->filledOrderMessages() = fil_out;
	    filledOrderMessages_[ii] = fil_in;
	}

	//copy relevant CanceledOrderMessage<LimitOrderMessage>
	for(unsigned ii=0; ii < numProducts_; ii++) {
	    auto cal = simulatorCorePtr_->canceledLimitOrderMessages();
	    list<CanceledOrderMessage<LimitOrderMessage> > cal_in;
	    list<CanceledOrderMessage<LimitOrderMessage> > cal_out;
	    partition_copy(cal.begin(),
			   cal.end(),
			   back_inserter(cal_in),
			   back_inserter(cal_out),
			   [&](CanceledOrderMessage<LimitOrderMessage> m) { return (m.getTicker() == tickers_[ii]) && (m.getStratId() == stratId_); });
	    simulatorCorePtr_->canceledLimitOrderMessages() = cal_out;
	    canceledLimitOrderMessages_[ii] = cal_in;
	}

	//copy relevant CanceledOrderMessage<MarketOrderMessage>
	for(unsigned ii=0; ii < numProducts_; ii++) {
	    auto cam = simulatorCorePtr_->canceledMarketOrderMessages();
	    list<CanceledOrderMessage<MarketOrderMessage> > cam_in;
	    list<CanceledOrderMessage<MarketOrderMessage> > cam_out;
	    partition_copy(cam.begin(),
			   cam.end(),
			   back_inserter(cam_in),
			   back_inserter(cam_out),
			   [&](CanceledOrderMessage<MarketOrderMessage> m) { return (m.getTicker() == tickers_[ii]) && (m.getStratId() == stratId_); });
	    simulatorCorePtr_->canceledMarketOrderMessages() = cam_out;
	    canceledMarketOrderMessages_[ii] = cam_in;
	}

	//copy relevant RejectedOrderMessage<LimitOrderMessage>
	for(unsigned ii=0; ii < numProducts_; ii++) {
	    auto rel = simulatorCorePtr_->rejectedLimitOrderMessages();
	    list<RejectedOrderMessage<LimitOrderMessage> > rel_in;
	    list<RejectedOrderMessage<LimitOrderMessage> > rel_out;
	    partition_copy(rel.begin(),
			   rel.end(),
			   back_inserter(rel_in),
			   back_inserter(rel_out),
			   [&](RejectedOrderMessage<LimitOrderMessage> m) { return (m.getTicker() == tickers_[ii]) && (m.getStratId() == stratId_); });
	    simulatorCorePtr_->rejectedLimitOrderMessages() = rel_out;
	    rejectedLimitOrderMessages_[ii] = rel_in;
	}

	//copy relevant RejectedOrderMessage<MarketOrderMessage>
	for(unsigned ii=0; ii < numProducts_; ii++) {
	    auto rem = simulatorCorePtr_->rejectedMarketOrderMessages();
	    list<RejectedOrderMessage<MarketOrderMessage> > rem_in;
	    list<RejectedOrderMessage<MarketOrderMessage> > rem_out;
	    partition_copy(rem.begin(),
			   rem.end(),
			   back_inserter(rem_in),
			   back_inserter(rem_out),
			   [&](RejectedOrderMessage<MarketOrderMessage> m) { return (m.getTicker() == tickers_[ii]) && (m.getStratId() == stratId_); });
	    simulatorCorePtr_->rejectedMarketOrderMessages() = rem_out;
	    rejectedMarketOrderMessages_[ii] = rem_in;
	}

	//copy relevant RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage>>
	for(unsigned ii=0; ii < numProducts_; ii++) {
	    auto rel = simulatorCorePtr_->rejectedCancelLimitOrderMessages();
	    list<RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage> > > rel_in;
	    list<RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage> > > rel_out;
	    partition_copy(rel.begin(),
			   rel.end(),
			   back_inserter(rel_in),
			   back_inserter(rel_out),
			   [&](RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage> > m) { return (m.getTicker() == tickers_[ii]) && (m.getStratId() == stratId_); });
	    simulatorCorePtr_->rejectedCancelLimitOrderMessages() = rel_out;
	    rejectedCancelLimitOrderMessages_[ii] = rel_in;
	}

	//copy relevant RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage>>
	for(unsigned ii=0; ii < numProducts_; ii++) {
	    auto rem = simulatorCorePtr_->rejectedCancelMarketOrderMessages();
	    list<RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage> > > rem_in;
	    list<RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage> > > rem_out;
	    partition_copy(rem.begin(),
			   rem.end(),
			   back_inserter(rem_in),
			   back_inserter(rem_out),
			   [&](RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage> > m) { return (m.getTicker() == tickers_[ii]) && (m.getStratId() == stratId_); });
	    simulatorCorePtr_->rejectedCancelMarketOrderMessages() = rem_out;
	    rejectedCancelMarketOrderMessages_[ii] = rem_in;
	}

	//copy relevant book updates for strategy updates
	//note strategy process can access all bookupdate and tradeupdate from simulatorCorePtr_
	for(unsigned ii=0; ii < numProducts_; ii++) {
	    auto bu = simulatorCorePtr_->getBookUpdate();
	    for(auto abu = bu.begin(); abu !=bu.end(); abu++) {
		if(abu->getTicker() == tickers_[ii]) {
		    bookUpdates_[ii].push_back((*abu));
		}
	    }
	}

	for(unsigned ii=0; ii < numProducts_; ii++) {
	    auto tu = simulatorCorePtr_->getTradeUpdate();
	    for(auto atu = tu.begin(); atu != tu.end(); atu++) {
		if(atu->getTicker() == tickers_[ii]) {
		    tradeUpdates_[ii].push_back((*atu));
		}
	    }
	}
    }

    //clear out useless stuff
    void Strategy::postProcess()
    {
	// update OrderBooks, update pending messages
	//copy and clear outgoing messages to SimulatorCore
	//set strategy id so that process don't need to worry about it.
	unsigned prodId;
	while (!limitOrderMessages_.empty()) {
	    limitOrderMessages_.front().stratId() = stratId_;
	    simulatorCorePtr_->limitOrderMessages().push_back(limitOrderMessages_.front());
	    for (prodId = 0; prodId < numProducts_; ++prodId) {
		if (tickers_[prodId] == limitOrderMessages_.front().getTicker()) {
		    break;
		}
	    }
	    orderbooks_[prodId].limitOrderHelper(&(limitOrderMessages_.front()));
	    ++limitOrderCounts_[prodId];
	    pendingLimitOrderMessages_[prodId].push_back(limitOrderMessages_.front());
	    limitOrderMessages_.pop_front();
	}

	while (!marketOrderMessages_.empty()) {
	    marketOrderMessages_.front().stratId() = stratId_;
	    simulatorCorePtr_->marketOrderMessages().push_back(marketOrderMessages_.front());
	    for (prodId = 0; prodId < numProducts_; ++prodId) {
		if (tickers_[prodId] == marketOrderMessages_.front().getTicker()) {
		    break;
		}
	    }
	    orderbooks_[prodId].marketOrderHelper(&(marketOrderMessages_.front()));
	    ++marketOrderCounts_[prodId];
	    pendingMarketOrderMessages_[prodId].push_back(marketOrderMessages_.front());
	    marketOrderMessages_.pop_front();
	}

	while (!cancelLimitOrderMessages_.empty()) {
	    cancelLimitOrderMessages_.front().stratId() = stratId_;
	    simulatorCorePtr_->cancelLimitOrderMessages().push_back(cancelLimitOrderMessages_.front());
	    for (prodId = 0; prodId < numProducts_; ++prodId) {
		if (tickers_[prodId] == cancelLimitOrderMessages_.front().getTicker()) {
		    break;
		}
	    }
	    ++cancelCounts_[prodId];
	    pendingCancelLimitOrderMessages_[prodId].push_back(cancelLimitOrderMessages_.front());
	    cancelLimitOrderMessages_.pop_front();
	}

	while (!cancelMarketOrderMessages_.empty()) {
	    cancelMarketOrderMessages_.front().stratId() = stratId_;
	    simulatorCorePtr_->cancelMarketOrderMessages().push_back(cancelMarketOrderMessages_.front());
	    for (prodId = 0; prodId < numProducts_; ++prodId) {
		if (tickers_[prodId] == cancelMarketOrderMessages_.front().getTicker()) {
		    break;
		}
	    }
	    ++cancelCounts_[prodId];
	    pendingCancelMarketOrderMessages_[prodId].push_back(cancelMarketOrderMessages_.front());
	    cancelMarketOrderMessages_.pop_front();
	}

	//clearing out incoming order messages 
	for_each(acknowledgedOrderMessages_.begin(),    acknowledgedOrderMessages_.end(),   [&](list<AcknowledgedOrderMessage>                  & ll) { ll.clear(); } );
	for_each(filledOrderMessages_.begin(),          filledOrderMessages_.end(),         [&](list<FilledOrderMessage>                        & ll) { ll.clear(); } );
	for_each(canceledLimitOrderMessages_.begin(),   canceledLimitOrderMessages_.end(),  [&](list<CanceledOrderMessage<LimitOrderMessage> >  & ll) { ll.clear(); } );
	for_each(canceledMarketOrderMessages_.begin(),  canceledMarketOrderMessages_.end(), [&](list<CanceledOrderMessage<MarketOrderMessage> > & ll) { ll.clear(); } );
	for_each(rejectedLimitOrderMessages_.begin(),   rejectedLimitOrderMessages_.end(),  [&](list<RejectedOrderMessage<LimitOrderMessage> >  & ll) { ll.clear(); } );
	for_each(rejectedMarketOrderMessages_.begin(),  rejectedMarketOrderMessages_.end(), [&](list<RejectedOrderMessage<MarketOrderMessage> > & ll) { ll.clear(); } );
	for_each(rejectedCancelLimitOrderMessages_.begin(),   rejectedCancelLimitOrderMessages_.end(),  [&](list<RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage> > >  & ll) { ll.clear(); } );
	for_each(rejectedCancelMarketOrderMessages_.begin(),  rejectedCancelMarketOrderMessages_.end(), [&](list<RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage> > > & ll) { ll.clear(); } );
	for_each(bookUpdates_.begin(),                  bookUpdates_.end(),                 [&](list<BookUpdate<true> >                         & ll) { ll.clear(); } );
	for_each(tradeUpdates_.begin(),                 tradeUpdates_.end(),                [&](list<TradeUpdate<true> >                        & ll) { ll.clear(); } );
	
	// update openBuyOrderCounts_ and openSellOrderCounts_
	for (unsigned int ii = 0; ii < numProducts_; ++ii) {
	    openBuyOrderCounts_[ii] = orderbooks_[ii].countLimitOrders(BuySellSide::buy);
	    openSellOrderCounts_[ii] = orderbooks_[ii].countLimitOrders(BuySellSide::sell);
	    hasFills_[ii] = false;
	}
    }
    
    void Strategy::cancelAllLimitOrders(int prodid)
    {
	if (orderbooks_[prodid].countLimitOrders(BuySellSide::buy) > 0) {
	    cancelAllLimitOrders(BuySellSide::buy, prodid);
	}
	if (orderbooks_[prodid].countLimitOrders(BuySellSide::sell) > 0) {
	    cancelAllLimitOrders(BuySellSide::sell, prodid);
	}
    }
    
    void Strategy::cancelAllLimitOrders(BuySellSide side, int prodid)
    {
	if (side == BuySellSide::buy) {
	    for (unsigned int ii = 0; ii < orderbooks_[prodid].getBidLimitOrderBook().size(); ++ii) {
		for (unsigned int jj=0; jj < orderbooks_[prodid].getBidLimitOrderBook()[ii].size(); ++jj) {
		    cancelLimitOrder(orderbooks_[prodid].getBidLimitOrderBook()[ii][jj]);
		}
		
	    }
	}
	
	if (side == BuySellSide::sell) {
	    for (unsigned int ii = 0; ii < orderbooks_[prodid].getAskLimitOrderBook().size(); ++ii) {
		for (unsigned int jj=0; jj < orderbooks_[prodid].getAskLimitOrderBook()[ii].size(); ++jj) {
		    cancelLimitOrder(orderbooks_[prodid].getAskLimitOrderBook()[ii][jj]);
		}
		
	    }
	}
    }
    
    void Strategy::cancelMarketOrder(MarketOrderMessage mom)
    {
	CancelOrderMessage<MarketOrderMessage> com(mom.getExchangeId(),
						   mom.getContractId(),
						   currTime_,
						   mom.getStratId(),
						   generateNewMessageId(),
						   mom.getOrderId(),
						   mom.getQty(),
						   mom.getSide());
	cancelMarketOrderMessages_.push_back(com);
	if(loggerPtr_) 
	    loggerPtr_->push(com.logMessage());
    }
    
    void Strategy::cancelLimitOrder(LimitOrderMessage lom)
    {
	
	CancelOrderMessage<LimitOrderMessage> col(lom.getExchangeId(),
						  lom.getContractId(),
						  currTime_,
						  lom.getStratId(),
						  generateNewMessageId(),
						  lom.getOrderId(),
						  lom.getQty(),
						  lom.getSide(),
						  lom.getPx());
	cancelLimitOrderMessages_.push_back(col);
	if(loggerPtr_) 
	    loggerPtr_->push(col.logMessage());
	
    }
    
    void Strategy::placeMarketOrder(BuySellSide side, int ordersize, int prodid)
    {
	MarketOrderMessage mom(exchanges_[prodid],
			       contracts_[prodid],
			       currTime_,
			       getStratId(),
			       generateNewMessageId(),
			       generateNewOrderId(tickers_[prodid]),
			       ordersize,
			       side);
	marketOrderMessages_.push_back(mom);
	if(loggerPtr_)
	    loggerPtr_->push(mom.logMessage());
    }

    void Strategy::placeLimitOrder(Px price, BuySellSide side, int ordersize, int prodid)
    {
	LimitOrderMessage lom(exchanges_[prodid],
			      contracts_[prodid],
			      currTime_,
			      getStratId(),
			      generateNewMessageId(),
			      generateNewOrderId(tickers_[prodid]),
			      ordersize,
			      side,
			      price);
	limitOrderMessages_.push_back(lom);
	if(loggerPtr_)
	    loggerPtr_->push(lom.logMessage());
    }
    
}  // STRATEGY_CPP
