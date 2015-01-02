//
// File:        $ Matchy.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2014/01/09 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2014 by Zhengzhang Futures Co. Ltd.

#include "Matchy.hpp"

namespace simulator
{

    Matchy::Matchy(string exchangeId, string contractId, SimulatorCore * simulatorCorePtr)
	: exchangeId_(exchangeId), contractId_(contractId), messageId_(0), simulatorCorePtr_(simulatorCorePtr),
	  isMarketDataReady_(false), isTraderReady_(false), actionFlag_(2)
    {}

    Matchy::~Matchy()
    {}

    time_stamp Matchy::getCurrTime() const
    {
	return currTime_;
    }

    time_stamp & Matchy::currTime()
    {
	return currTime_;
    }

    list<AcknowledgedOrderMessage> & Matchy::acknowledgedOrderMessages() 
    {
	return acknowledgedOrderMessages_;
    }

    list<FilledOrderMessage> & Matchy::filledOrderMessages() 
    {
	return filledOrderMessages_;
    }

    list<CanceledOrderMessage<LimitOrderMessage> > & Matchy::canceledLimitOrderMessages()
    {
	return canceledLimitOrderMessages_;
    }
 
    list<CanceledOrderMessage<MarketOrderMessage> > & Matchy::canceledMarketOrderMessages()
    {
	return canceledMarketOrderMessages_;
    }
    
    list<RejectedOrderMessage<LimitOrderMessage> > & Matchy::rejectedLimitOrderMessages()
    {
	return rejectedLimitOrderMessages_;
    }

    list<RejectedOrderMessage<MarketOrderMessage> > & Matchy::rejectedMarketOrderMessages()
    {
	return rejectedMarketOrderMessages_;
    }

    list<RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage> > > & Matchy::rejectedCancelLimitOrderMessages()
    {
	return rejectedCancelLimitOrderMessages_;
    }

    list<RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage> > > & Matchy::rejectedCancelMarketOrderMessages()
    {
	return rejectedCancelMarketOrderMessages_;
    }

    list<TradeUpdate<true> > & Matchy::tradeUpdates()
    {
	return tradeUpdates_;
    }

    list<BookUpdate<true> > & Matchy::bookUpdates()
    {
	return bookUpdates_;
    }

    list<LimitOrderMessage> & Matchy::limitOrderMessages()
    {
	return limitOrderMessages_;
    }

    list<MarketOrderMessage> & Matchy::marketOrderMessages()
    {
	return marketOrderMessages_;
    }

    list<CancelOrderMessage<LimitOrderMessage> > & Matchy::cancelLimitOrderMessages()
    {
	return cancelLimitOrderMessages_;
    }

    list<CancelOrderMessage<MarketOrderMessage> > & Matchy::cancelMarketOrderMessages()
    {
	return cancelMarketOrderMessages_;
    }
    
    void Matchy::clearAllMessages()
    {
	limitOrderMessages_.clear();
	marketOrderMessages_.clear();
	cancelLimitOrderMessages_.clear();
	cancelMarketOrderMessages_.clear();

	acknowledgedOrderMessages_.clear();
	filledOrderMessages_.clear();
	canceledLimitOrderMessages_.clear();
	canceledMarketOrderMessages_.clear();
	rejectedLimitOrderMessages_.clear();
	rejectedMarketOrderMessages_.clear();
	rejectedCancelLimitOrderMessages_.clear();
	rejectedCancelMarketOrderMessages_.clear();
    }

    void Matchy::acknowledgeOrderMessage(LimitOrderMessage const m)
    {
	limitOrderMessages_.push_back(m);
	acknowledgedOrderMessages_.emplace_back(AcknowledgedOrderMessage(m.getExchangeId(), m.getContractId(), currTime_, m.getStratId(), messageId_++, m.getOrderId(), false));
    }

    void Matchy::acknowledgeOrderMessage(MarketOrderMessage const m)
    {
	marketOrderMessages_.push_back(m);
	acknowledgedOrderMessages_.emplace_back(AcknowledgedOrderMessage(m.getExchangeId(), m.getContractId(), currTime_, m.getStratId(), messageId_++, m.getOrderId(), false));
    }

    void Matchy::acknowledgeOrderMessage(CancelOrderMessage<LimitOrderMessage> const m)
    {
	cancelLimitOrderMessages_.push_back(m);
	acknowledgedOrderMessages_.emplace_back(AcknowledgedOrderMessage(m.getExchangeId(), m.getContractId(), currTime_, m.getStratId(), messageId_++, m.getOrderId(), true));
    }

    void Matchy::acknowledgeOrderMessage(CancelOrderMessage<MarketOrderMessage> const m)
    {
	cancelMarketOrderMessages_.push_back(m);
	acknowledgedOrderMessages_.emplace_back(AcknowledgedOrderMessage(m.getExchangeId(), m.getContractId(), currTime_, m.getStratId(), messageId_++, m.getOrderId(), true));
    }

    void Matchy::acknowledgeOrderMessages()
    {
	/* Process limit order messages. */
	auto lom = simulatorCorePtr_->limitOrderMessages();
        list<LimitOrderMessage> lom_in;
	list<LimitOrderMessage> lom_out;
	partition_copy(lom.begin(),
		       lom.end(),
		       back_inserter(lom_in),
		       back_inserter(lom_out),
		       [&](LimitOrderMessage m) { return (m.getExchangeId() == exchangeId_) && (m.getContractId() == contractId_); });

	simulatorCorePtr_->limitOrderMessages() = lom_out;
	for_each(lom_in.begin(), lom_in.end(), [&](LimitOrderMessage const m) { acknowledgeOrderMessage(m); });

	/* Process market order messages. */
	auto mom = simulatorCorePtr_->marketOrderMessages();
        list<MarketOrderMessage> mom_in;
	list<MarketOrderMessage> mom_out;
	partition_copy(mom.begin(),
		       mom.end(),
		       back_inserter(mom_in),
		       back_inserter(mom_out),
		       [&](MarketOrderMessage m) { return (m.getExchangeId() == exchangeId_) && (m.getContractId() == contractId_); });

	simulatorCorePtr_->marketOrderMessages() = mom_out;
	for_each(mom_in.begin(), mom_in.end(), [&](MarketOrderMessage const m) { acknowledgeOrderMessage(m); });

	/* Process cancel limit order messages. */
	auto clom = simulatorCorePtr_->cancelLimitOrderMessages();
        list<CancelOrderMessage<LimitOrderMessage> > clom_in;
	list<CancelOrderMessage<LimitOrderMessage> > clom_out;
	partition_copy(clom.begin(),
		       clom.end(),
		       back_inserter(clom_in),
		       back_inserter(clom_out),
		       [&](CancelOrderMessage<LimitOrderMessage> m) { return (m.getExchangeId() == exchangeId_) && (m.getContractId() == contractId_); });

	simulatorCorePtr_->cancelLimitOrderMessages() = clom_out;
	for_each(clom_in.begin(), clom_in.end(), [&](CancelOrderMessage<LimitOrderMessage> const m) { acknowledgeOrderMessage(m); });

	/* Process cancel market order messages. */
	auto cmom = simulatorCorePtr_->cancelMarketOrderMessages();
        list<CancelOrderMessage<MarketOrderMessage> > cmom_in;
	list<CancelOrderMessage<MarketOrderMessage> > cmom_out;
	partition_copy(cmom.begin(),
		       cmom.end(),
		       back_inserter(cmom_in),
		       back_inserter(cmom_out),
		       [&](CancelOrderMessage<MarketOrderMessage> m) { return (m.getExchangeId() == exchangeId_) && (m.getContractId() == contractId_); });

	simulatorCorePtr_->cancelMarketOrderMessages() = cmom_out;
	for_each(cmom_in.begin(), cmom_in.end(), [&](CancelOrderMessage<MarketOrderMessage> const m) { acknowledgeOrderMessage(m); });
    }

    bool Matchy::processLimitOrder(LimitOrderMessage & m) 
    {}

    bool Matchy::processMarketOrder(MarketOrderMessage & m) 
    {}

    bool Matchy::processCancelLimitOrder(CancelOrderMessage<LimitOrderMessage> & m) 
    {}

    bool Matchy::processCancelMarketOrder(CancelOrderMessage<MarketOrderMessage> & m) 
    {}

    void Matchy::setOrderId(int n) 
    {}

    void Matchy::process(int n) 
    {}

    /* Load data for simulation matchy. */
    bool SimMatchy::preLoadData(string fileName)
    {
	data_file_.open(fileName.c_str(), ifstream::in);
	
	if (data_file_.is_open() && data_file_.good())
	    return true;
	else
	    return false;
    }

    /* Initialize nextBookUpdate_, nextTradeUpdate_. */
    bool SimMatchy::init()
    {
	if (data_file_.good() && !data_file_.eof()) {
	    string line;
	    getline(data_file_, line, '\n');  // Discard header.
	    getline(data_file_, line, '\n');
	    
	    BookUpdate<true> bookupdate(exchangeId_, contractId_, currTime_, messageId_++);
	    TradeUpdate<true> tradeupdate(exchangeId_, contractId_, currTime_, messageId_++);
	    this->parser(line, &bookupdate, &tradeupdate);
	    nextBookUpdate_.push_back(bookupdate);
	    nextTradeUpdate_.push_back(tradeupdate);
	    currBookUpdate_ = bookupdate;

	    isMarketDataReady_ = true;
	    isTraderReady_ = true;

	    return true;
	}
	else
	    return false;
    }

    /* Parse string of data line and store. */
    void SimMatchy::parser(string data, BookUpdate<true> *pBookUpdate, TradeUpdate<true> *pTradeUpdate)
    {
	char datetime[30];
	double lastPx;
	int totalVolume;
	double bid1px, bid2px, bid3px, bid4px, bid5px;
	int bid1qty, bid2qty, bid3qty, bid4qty, bid5qty;
	double ask1px, ask2px, ask3px, ask4px, ask5px;
	int ask1qty, ask2qty, ask3qty, ask4qty, ask5qty;

	sscanf(data.c_str(), "%[^','],%lf,%d,%lf,%d,%lf,%d,%lf,%d,%lf,%d,%lf,%d,%lf,%d,%lf,%d,%lf,%d,%lf,%d,%lf,%d",
	       datetime,
	       &lastPx,
	       &totalVolume,
	       &bid1px, &bid1qty, &bid2px, &bid2qty, &bid3px, &bid3qty, &bid4px, &bid4qty, &bid5px, &bid5qty,
	       &ask1px, &ask1qty, &ask2px, &ask2qty, &ask3px, &ask3qty, &ask4px, &ask4qty, &ask5px, &ask5qty);

	datetime[29] = '\0';

	time_stamp tp = simulatorCorePtr_->convertStringToTimePoint(datetime);
	pBookUpdate->timestamp() = tp;
	pTradeUpdate->timestamp() = tp;
	currTime_ = tp;    // Update Matchy timestamp.

	vector<Px> bidPx, askPx;
	bidPx.push_back(move(Px(bid1px, tickSize_)));
	bidPx.push_back(move(Px(bid2px, tickSize_)));
	bidPx.push_back(move(Px(bid3px, tickSize_)));
	bidPx.push_back(move(Px(bid4px, tickSize_)));
	bidPx.push_back(move(Px(bid5px, tickSize_)));
	askPx.push_back(move(Px(ask1px, tickSize_)));
	askPx.push_back(move(Px(ask2px, tickSize_)));
	askPx.push_back(move(Px(ask3px, tickSize_)));
	askPx.push_back(move(Px(ask4px, tickSize_)));
	askPx.push_back(move(Px(ask5px, tickSize_)));
	pBookUpdate->bidPx() = bidPx;
	pBookUpdate->askPx() = askPx;

	vector<int> bidQty = {bid1qty, bid2qty, bid3qty, bid4qty, bid5qty};
	vector<int> askQty = {ask1qty, ask2qty, ask3qty, ask4qty, ask5qty};
	pBookUpdate->bidQty() = bidQty;
	pBookUpdate->askQty() = askQty;

	Px px(lastPx, tickSize_);
	pTradeUpdate->px() = px;
	pTradeUpdate->qty() = totalVolume;
	pTradeUpdate->side() = BuySellSide::unknown;
    }

    /* Iterate through market data and update Matchy's currTime_. */
    void SimMatchy::next()
    {
	/* Snap current book update to last book update. */
	currBookUpdate_ = nextBookUpdate_.back();

	/* Clear the book and trade updates. */
	nextBookUpdate_.clear();
	nextTradeUpdate_.clear();

	/* Fill in the book and trade updates if data are available. */
	if (data_file_.good() && !data_file_.eof() && !data_file_.fail()) {
	    string line;
	    getline(data_file_, line, '\n');
		
	    if(line.size() < 1) 
		return;

	    BookUpdate<true> bookupdate(exchangeId_, contractId_, currTime_, messageId_++);
	    TradeUpdate<true> tradeupdate(exchangeId_, contractId_, currTime_, messageId_++);
	    this->parser(line, &bookupdate, &tradeupdate);
	    nextBookUpdate_.push_back(bookupdate);
	    nextTradeUpdate_.push_back(tradeupdate);
	}
    }

    SimMatchy::SimMatchy(string exchange, string contractId, double tickSize, string fileName, SimulatorCore * simulatorCorePtr)
	: Matchy(exchange, contractId, simulatorCorePtr),
	  tickSize_(tickSize)
    {
	if (!preLoadData(fileName)) {
	    InitException myexception;
	    throw myexception;
	}
    }

    SimMatchy::~SimMatchy()
    {
	if (data_file_.is_open())
	    data_file_.close();
    }

    bool SimMatchy::end() const
    {
	return nextBookUpdate_.empty();
    }

    void SimMatchy::process(int orderId)
    {
	/* Clear all inbound and outbound messages. */
	clearAllMessages();

	/* Clear outbound updates. */
	tradeUpdates_.clear();
	bookUpdates_.clear();

	/* If end-of-day, any inbound order is not acknowledged. */
	if (end())
	    return;

	/* Put order messages to pending message list and acknowledge receipts of messages. */
	acknowledgeOrderMessages();

	/* Move inbound messages to orderBook_. */
	// moved to before market data updates to avoid mechanical latency
	for (auto & m : marketOrderMessages_)
	    orderBook_.marketOrderHelper(&m);
	for (auto & m : limitOrderMessages_)
	    orderBook_.limitOrderHelper(&m);
	for (auto & m : cancelMarketOrderMessages_) {
	    bool canceledQ = orderBook_.cancelOrderHelper(&m);
	    if (canceledQ)
		canceledMarketOrderMessages_.push_back(CanceledOrderMessage<MarketOrderMessage>(m.getExchangeId(), m.getContractId(), currTime_, m.getStratId(), messageId_++, m.getOrderId(), m.getQty(), m.getSide()));
	    else
		rejectedCancelMarketOrderMessages_.push_back(RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage> >(m.getExchangeId(), m.getContractId(), currTime_, m.getStratId(), messageId_++, m.getOrderId(), m.getQty(), m.getSide()));
	}
	for (auto & m : cancelLimitOrderMessages_) {
	    bool canceledQ = orderBook_.cancelOrderHelper(&m);
	    if (canceledQ)
		canceledLimitOrderMessages_.push_back(CanceledOrderMessage<LimitOrderMessage>(m.getExchangeId(), m.getContractId(), currTime_, m.getStratId(), messageId_++, m.getOrderId(), m.getQty(), m.getSide(), m.getPx()));
	    else
		rejectedCancelLimitOrderMessages_.push_back(RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage> >(m.getExchangeId(), m.getContractId(), currTime_, m.getStratId(), messageId_++, m.getOrderId(), m.getQty(), m.getSide(), m.getPx()));
	}

	/* Iterate over market data, match orders with earlier timestamps */
	/* while putting snapshots of market data into outbound update message list
	   and outbound order message list. */
	auto timestampSC = simulatorCorePtr_->getCurrTime();
	// changed by Mingyuan
	//while (!end() && currTime_ <= timestampSC) { 
	while (!end() && currBookUpdate_.getTimestamp() <= timestampSC) {
	
	    /* Work on market orders. */
	    vector<MarketOrderMessage> & marketOrderBook = orderBook_.marketOrderBook();
	    vector<MarketOrderMessage> mob_in;
	    vector<MarketOrderMessage> mob_out;
	    partition_copy(marketOrderBook.begin(),
			   marketOrderBook.end(),
			   back_inserter(mob_in),
			   back_inserter(mob_out),
					   [&](MarketOrderMessage m) { return m.getTimestamp() <= timestampSC && m.getTimestamp() < currTime_; });
	    /* Sort market orders by time. */
	    sort(mob_in.begin(),
		 mob_in.end(),
		 [] (MarketOrderMessage l, MarketOrderMessage r) { return l.getTimestamp() < r.getTimestamp(); });
	    /* Fill order and append to filledOrderMessages_. */
	    /* Assume each level of the limit order book has the same liquidity (qty) as the top book. :P */
	    matchMarketOrder(currBookUpdate_, mob_in, timestampSC);
	    /* Update leftover market orders in orderBook_. */
	    orderBook_.clearMarketOrderBook();
	    for (auto & m : mob_out)
		orderBook_.marketOrderHelper(&m);
	    
	    // current timestamp of the book
	    auto timestampFreeze = currBookUpdate_.getTimestamp();
	    // for marketable limit order
	    {
		/* Work on marketable bid limit orders. */
		vector<vector<LimitOrderMessage> > & bidLimitOrderBook = orderBook_.bidLimitOrderBook();
		vector<LimitOrderMessage> lob_in;
		vector<LimitOrderMessage> lob_out;
		for (auto & bidLimitOrder : bidLimitOrderBook) {
		    partition_copy(bidLimitOrder.begin(),
				   bidLimitOrder.end(),
				   back_inserter(lob_in),
				   back_inserter(lob_out),
				   [&](LimitOrderMessage m) { return m.getTimestamp() >= timestampFreeze && m.getTimestamp() < currTime_; });
		}
		/* Sort limit orders by time. */
		sort(lob_in.begin(),
		     lob_in.end(),
		     [] (LimitOrderMessage l, LimitOrderMessage r) { return l.getTimestamp() < r.getTimestamp(); });
		/* Fill order and append to filledOrderMessages_. */
		matchLimitOrder(currBookUpdate_, lob_in, lob_out, timestampSC, BuySellSide::buy);
		/* Update leftover bid limit orders in orderBook_. */
		orderBook_.clearBidLimitOrderBook();
		for (auto & m : lob_out)
		    orderBook_.limitOrderHelper(&m);
		
		/* Work on marketable ask limit orders */
		vector<vector<LimitOrderMessage> > & askLimitOrderBook = orderBook_.askLimitOrderBook();
		lob_in.clear();
		lob_out.clear();
		for (auto & askLimitOrder : askLimitOrderBook) {
		    partition_copy(askLimitOrder.begin(),
				   askLimitOrder.end(),
				   back_inserter(lob_in),
				   back_inserter(lob_out),
				   [&](LimitOrderMessage m) { return m.getTimestamp() >= timestampFreeze && m.getTimestamp() < currTime_; });
		}
		/* Sort limit orders by time. */
		sort(lob_in.begin(),
		     lob_in.end(),
		     [] (LimitOrderMessage l, LimitOrderMessage r) { return l.getTimestamp() < r.getTimestamp(); });
		/* Fill order and append to filledOrderMessages_. */
		matchLimitOrder(currBookUpdate_, lob_in, lob_out, timestampSC, BuySellSide::sell);
		/* Update leftover ask limit orders in orderBook_. */
		orderBook_.clearAskLimitOrderBook();
		for (auto & m : lob_out)
		    orderBook_.limitOrderHelper(&m);
	    }
	    
	    /* Work on bid limit orders. */
	    vector<vector<LimitOrderMessage> > & bidLimitOrderBook = orderBook_.bidLimitOrderBook();
	    vector<LimitOrderMessage> lob_in;
	    vector<LimitOrderMessage> lob_out;
	    for (auto & bidLimitOrder : bidLimitOrderBook) {
		partition_copy(bidLimitOrder.begin(),
			       bidLimitOrder.end(),
			       back_inserter(lob_in),
			       back_inserter(lob_out),
			       [&](LimitOrderMessage m) { return m.getTimestamp() <= timestampSC && m.getTimestamp() < currTime_; });
	    }
	    /* Fill order and append to filledOrderMessages_. */
	    for (auto & m : lob_in) {
		if (m.getTimestamp() < timestampFreeze && m.getPx() >= currBookUpdate_.getAskPx().front()) {
		    FilledOrderMessage fom(m.getExchangeId(), m.getContractId(), timestampSC, m.getStratId(), messageId_++, 
					   m.getOrderId(), m.getQty(), BuySellSide::buy, m.getPx());
		    filledOrderMessages_.push_back(fom);
		}
		else {
		    lob_out.push_back(m);
		}
	    }
	    /* Update leftover bid limit orders in orderBook_. */
	    orderBook_.clearBidLimitOrderBook();
	    for (auto & m : lob_out)
		orderBook_.limitOrderHelper(&m);

	    /* Work on ask limit orders. */
	    vector<vector<LimitOrderMessage> > & askLimitOrderBook = orderBook_.askLimitOrderBook();
	    lob_in.clear();
	    lob_out.clear();
	    for (auto & askLimitOrder : askLimitOrderBook) {
		partition_copy(askLimitOrder.begin(),
			       askLimitOrder.end(),
			       back_inserter(lob_in),
			       back_inserter(lob_out),
			       [&](LimitOrderMessage m) { return m.getTimestamp() <= timestampSC && m.getTimestamp() < currTime_; });
	    }
	    /* Fill order and append to filledOrderMessages_. */
	    for (auto & m : lob_in) {
		if (m.getTimestamp() < timestampFreeze && m.getPx() <= currBookUpdate_.getBidPx().front()) {
		    FilledOrderMessage fom(m.getExchangeId(), m.getContractId(), timestampSC, m.getStratId(), messageId_++, 
					   m.getOrderId(), m.getQty(), BuySellSide::sell, m.getPx());
		    filledOrderMessages_.push_back(fom);
		}
		else {
		    lob_out.push_back(m);
		}
	    }
	    /* Update leftover ask limit orders in orderBook_. */
	    orderBook_.clearAskLimitOrderBook();
	    for (auto & m : lob_out)
		orderBook_.limitOrderHelper(&m);

	    /* Push updates to tradeUpdates_ and bookUpdates_. */
	    if (currTime_ <= timestampSC) {
		bookUpdates_.push_back(nextBookUpdate_.front());
		tradeUpdates_.push_back(nextTradeUpdate_.front());
		next();
	    } else {
		break;
	    } 
	}
    }

    void SimMatchy::matchMarketOrder(BookUpdate<true> & bookUpdate, vector<MarketOrderMessage> & mob, time_stamp & ts)
    {
	int bidLevel = 0;
	double bidPxFix = bookUpdate.getBidPx().front().getPx();
	int bidQtyFix = bookUpdate.getBidQty().front();
	int askLevel = 0;
	double askPxFix = bookUpdate.getAskPx().front().getPx();
	int askQtyFix = bookUpdate.getAskQty().front();
	int bidQtyCurr = bidQtyFix;
	int askQtyCurr = askQtyFix;
	for (auto & m : mob) {
	    while (m.getQty() > 0) {
		int execQty = min(m.getQty(), (m.getSide() == BuySellSide::buy) ? askQtyCurr : bidQtyCurr);
		FilledOrderMessage fom(m.getExchangeId(), m.getContractId(), ts, m.getStratId(), messageId_++, m.getOrderId(), 
				       execQty, m.getSide(), (m.getSide() == BuySellSide::buy) ? Px(askPxFix + askLevel * tickSize_ + slippage_, tickSize_) : Px(bidPxFix - bidLevel * tickSize_ - slippage_, tickSize_));
		filledOrderMessages_.push_back(fom);
		m.qty() -= execQty;
		if (m.getSide() == BuySellSide::buy) {
		    askQtyCurr -= execQty;
		    if (askQtyCurr == 0) {
			++askLevel;
			askQtyCurr = medianBookSize_;
		    }
		} else {
		    bidQtyCurr -= execQty;
		    if (bidQtyCurr == 0) {
			++bidLevel;
			bidQtyCurr = medianBookSize_;
		    }
		}
	    }
	}
    }

    void SimMatchy::matchLimitOrder(BookUpdate<true> & bookUpdate, vector<LimitOrderMessage> & lob_in, vector<LimitOrderMessage> & lob_out, time_stamp & ts, BuySellSide side)
    {
	if (side == BuySellSide::buy) {
	    int askLevel = 0;
	    double askPxFix = bookUpdate.getAskPx().front().getPx();
	    int askQtyFix = bookUpdate.getAskQty().front();
	    int askQtyCurr = askQtyFix;
	    for (auto & m : lob_in) {
		while (m.getQty() > 0 && m.getPx() >= Px(askPxFix + askLevel * tickSize_, tickSize_)) {
		    int execQty = min(m.getQty(), askQtyCurr);
		    FilledOrderMessage fom(m.getExchangeId(), m.getContractId(), ts, m.getStratId(), messageId_++, m.getOrderId(), 
					   execQty, m.getSide(), Px(askPxFix + askLevel * tickSize_ + slippage_, tickSize_));
		    filledOrderMessages_.push_back(fom);
		    m.qty() -= execQty;
		    askQtyCurr -= execQty;
		    if (askQtyCurr == 0) {
			++askLevel;
			askQtyCurr = medianBookSize_;
		    }
		}
		if (m.getQty() > 0) {
		    lob_out.push_back(m);
		}
	    } 
	} else {
	    int bidLevel = 0;
	    double bidPxFix = bookUpdate.getBidPx().front().getPx();
	    int bidQtyFix = bookUpdate.getBidQty().front();
	    int bidQtyCurr = bidQtyFix;
	    for (auto & m : lob_in) {
		while (m.getQty() > 0 && m.getPx() <= Px(bidPxFix - bidLevel * tickSize_, tickSize_)) {
		    int execQty = min(m.getQty(), bidQtyCurr);
		    FilledOrderMessage fom(m.getExchangeId(), m.getContractId(), ts, m.getStratId(), messageId_++, m.getOrderId(), 
					   execQty, m.getSide(), Px(bidPxFix - bidLevel * tickSize_ - slippage_, tickSize_));
		    filledOrderMessages_.push_back(fom);
		    m.qty() -= execQty;
		    bidQtyCurr -= execQty;
		    if (bidQtyCurr == 0) {
			++bidLevel;
			bidQtyCurr = medianBookSize_;
		    }
		}
		if (m.getQty() > 0) {
		    lob_out.push_back(m);
		}
	    } 
	}
    }

}

// MATCHY_CPP
