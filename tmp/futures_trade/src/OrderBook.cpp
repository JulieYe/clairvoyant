//
// File:        $ OrderBook.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/10/15 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include "OrderBook.hpp"

namespace simulator
{

    vector<vector<LimitOrderMessage> > & OrderBook::bidLimitOrderBook()
    {
	return bidLimitOrderBook_;
    }

    vector<vector<LimitOrderMessage> > OrderBook::getBidLimitOrderBook() const
    {
	return bidLimitOrderBook_;
    }

    void OrderBook::clearBidLimitOrderBook()
    {
	bidLimitOrderBook_.clear();
    }
    
    vector<vector<LimitOrderMessage> > & OrderBook::askLimitOrderBook()
    {
	return askLimitOrderBook_;
    }

    vector<vector<LimitOrderMessage> > OrderBook::getAskLimitOrderBook() const
    {
	return askLimitOrderBook_;
    }

    void OrderBook::clearAskLimitOrderBook()
    {
	askLimitOrderBook_.clear();
    }

    vector<vector<LimitOrderMessage> > OrderBook::getLimitOrderBook(BuySellSide side) const
    {
	if (side == BuySellSide::buy) { 
	    return bidLimitOrderBook_;
	} else if (side == BuySellSide::sell) { 
	    return askLimitOrderBook_;
	}
    }
    
    vector<MarketOrderMessage> & OrderBook::marketOrderBook()
    {
	return marketOrderBook_;
    }

    vector<MarketOrderMessage> OrderBook::getMarketOrderBook() const
    {
	return marketOrderBook_;
    }

    void OrderBook::clearMarketOrderBook()
    {
	marketOrderBook_.clear();
    }
    
  OrderBook::OrderBook()
    {
    }

    OrderBook::OrderBook(OrderBook const & other)
	: bidLimitOrderBook_(other.getBidLimitOrderBook()),
	  askLimitOrderBook_(other.getAskLimitOrderBook()),
	  marketOrderBook_(other.getMarketOrderBook())
    {}

    OrderBook & OrderBook::operator=(OrderBook const & other)
    {
	bidLimitOrderBook_ = other.getBidLimitOrderBook();
	askLimitOrderBook_ = other.getAskLimitOrderBook();
	marketOrderBook_ = other.getMarketOrderBook();
	return *this;
    }

    OrderBook::~OrderBook()
    {}

    void OrderBook::limitOrderHelper(LimitOrderMessage const * ptr)
    {
	bool buySideQ = (ptr->getSide()) == (BuySellSide::buy);
	vector<vector<LimitOrderMessage> > * limitOrderBookPtr = buySideQ ? &bidLimitOrderBook_ : &askLimitOrderBook_;
	
	auto it = find_if((*limitOrderBookPtr).begin(), 
			  (*limitOrderBookPtr).end(), 
			  [&](vector<LimitOrderMessage> v) { return ptr->getPx() == v[0].getPx(); });
	
	if (it != (*limitOrderBookPtr).end()) {
	    it->push_back(*ptr);
	    // sort the inside vector based on ascending timestamp
	    sort((*it).begin(), 
		 (*it).end(), 
		 [](LimitOrderMessage l, LimitOrderMessage r) { return l.getTimestamp() < r.getTimestamp(); });
	}
	else {
	    vector<LimitOrderMessage> v = {*ptr};
	    (*limitOrderBookPtr).push_back(move(v));
	    // sort the outside vector based on ascending prices for ask, descending prices for bid
	    sort((*limitOrderBookPtr).begin(), 
		 (*limitOrderBookPtr).end(), 
		 buySideQ ? ([](vector<LimitOrderMessage> l, vector<LimitOrderMessage> r) { return l[0].getPx() > r[0].getPx(); }) :
		 ([](vector<LimitOrderMessage> l, vector<LimitOrderMessage> r) { return l[0].getPx() < r[0].getPx(); }));
	}
    }

    void OrderBook::marketOrderHelper(MarketOrderMessage const * ptr)
    {
	marketOrderBook_.push_back(*ptr);
	sort(marketOrderBook_.begin(),
	     marketOrderBook_.end(),
	     [](MarketOrderMessage l, MarketOrderMessage r) { return l.getTimestamp() < r.getTimestamp(); });
    }

    bool OrderBook::cancelOrderHelper(CancelOrderMessage<MarketOrderMessage> const * ptr)
    {
	size_t marketOrderNumber = marketOrderBook_.size();
	marketOrderBook_.erase(remove_if(marketOrderBook_.begin(), 
					 marketOrderBook_.end(),
					     [&](MarketOrderMessage m) { return (m.getExchangeId() == ptr->getExchangeId()) &&
						 (m.getContractId() == ptr->getContractId()) &&
						 (m.getOrderId() == ptr->getOrderId()) &&
						 (m.getTimestamp() <= ptr->getTimestamp()); }),
			       marketOrderBook_.end());
	
	if (marketOrderBook_.size() != marketOrderNumber)
	    return true;
	else
	    return false;
    }

    bool OrderBook::cancelOrderHelper(CancelOrderMessage<LimitOrderMessage> const * ptr)
    {
	size_t bidLimitOrderNumber = accumulate(bidLimitOrderBook_.begin(),
						bidLimitOrderBook_.end(),
						0,
						[&](size_t size, vector<LimitOrderMessage> v) { return size + v.size(); });
	for (auto & bidLimitOrderBookPerPxLevel : bidLimitOrderBook_) {
	    bidLimitOrderBookPerPxLevel.erase(remove_if(bidLimitOrderBookPerPxLevel.begin(),
							bidLimitOrderBookPerPxLevel.end(),
							[&](LimitOrderMessage m) { return (m.getExchangeId() == ptr->getExchangeId()) &&
								(m.getContractId() == ptr->getContractId()) &&
								(m.getOrderId() == ptr->getOrderId()) &&
								(m.getTimestamp() <= ptr->getTimestamp()); }),
					      bidLimitOrderBookPerPxLevel.end());
	}
	bidLimitOrderBook_.erase(remove_if(bidLimitOrderBook_.begin(),
					   bidLimitOrderBook_.end(),
					   [&](vector<LimitOrderMessage> v_lom) { return v_lom.empty(); }),
				 bidLimitOrderBook_.end());
	size_t numBidLimitOrderMatched = bidLimitOrderNumber - accumulate(bidLimitOrderBook_.begin(),
									  bidLimitOrderBook_.end(),
									  0,
									  [&](size_t size, vector<LimitOrderMessage> v) { return size + v.size(); });
	    
	size_t askLimitOrderNumber = accumulate(askLimitOrderBook_.begin(),
						askLimitOrderBook_.end(),
						0,
						[&](size_t size, vector<LimitOrderMessage> v) { return size + v.size(); });
	for (auto & askLimitOrderBookPerPxLevel : askLimitOrderBook_) {
	    askLimitOrderBookPerPxLevel.erase(remove_if(askLimitOrderBookPerPxLevel.begin(),
							askLimitOrderBookPerPxLevel.end(),
							[&](LimitOrderMessage m) { return (m.getExchangeId() == ptr->getExchangeId()) &&
								(m.getContractId() == ptr->getContractId()) &&
								(m.getOrderId() == ptr->getOrderId()) &&
								(m.getTimestamp() <= ptr->getTimestamp()); }),
					      askLimitOrderBookPerPxLevel.end());
	}
	askLimitOrderBook_.erase(remove_if(askLimitOrderBook_.begin(),
					   askLimitOrderBook_.end(),
					   [&](vector<LimitOrderMessage> v_lom) { return v_lom.empty(); }),
				 askLimitOrderBook_.end());
	size_t numAskLimitOrderMatched = askLimitOrderNumber - accumulate(askLimitOrderBook_.begin(),
									  askLimitOrderBook_.end(),
									  0,
									  [&](size_t size, vector<LimitOrderMessage> v) { return size + v.size(); });
	    
	if (numBidLimitOrderMatched != 0 || numAskLimitOrderMatched != 0)
	    return true;
	else
	    return false;
    }

    bool OrderBook::filledOrderHelper(FilledOrderMessage const * ptr)
    {
	/* Fill for marketOrderBook_. */
	auto itm = find_if(marketOrderBook_.begin(), 
			   marketOrderBook_.end(), 
			   [&](MarketOrderMessage m) { return (m.getExchangeId() == ptr->getExchangeId()) &&
						       (m.getContractId() == ptr->getContractId()) &&
						       (m.getOrderId() == ptr->getOrderId()) &&
						       (m.getTimestamp() <= ptr->getTimestamp()); });
	/* If found in marketOrderBook_. */
	if (itm != marketOrderBook_.end()) {
	    (*itm).qty() -= ptr->getQty();
	    /* for complete fill */
	    if ((*itm).getQty() == 0)
		marketOrderBook_.erase(itm);
	    return true;
	}
	
	/* Fill for bidLimitOrderBook_. */
	for (auto & bidLimitOrderBookPerPxLevel : bidLimitOrderBook_) {
	    auto itl = find_if(bidLimitOrderBookPerPxLevel.begin(), 
			       bidLimitOrderBookPerPxLevel.end(), 
			       [&](LimitOrderMessage m) { return (m.getExchangeId() == ptr->getExchangeId()) &&
							  (m.getContractId() == ptr->getContractId()) &&
							  (m.getOrderId() == ptr->getOrderId()) &&
							  (m.getTimestamp() <= ptr->getTimestamp()); });
	    if (itl != bidLimitOrderBookPerPxLevel.end()) {
		(*itl).qty() -= ptr->getQty();
		/* for complete fill */
		if ((*itl).getQty() == 0) {
		    bidLimitOrderBookPerPxLevel.erase(itl);
		    bidLimitOrderBook_.erase(remove_if(bidLimitOrderBook_.begin(),
						       bidLimitOrderBook_.end(),
						       [&](vector<LimitOrderMessage> v_lom) { return v_lom.empty(); }),
					     bidLimitOrderBook_.end());
		}
		return true;
	    }
	}

	/* Fill for askLimitOrderBook_. */
	for (auto & askLimitOrderBookPerPxLevel : askLimitOrderBook_) {
	    auto itl = find_if(askLimitOrderBookPerPxLevel.begin(), 
			  askLimitOrderBookPerPxLevel.end(), 
			  [&](LimitOrderMessage m) { return (m.getExchangeId() == ptr->getExchangeId()) &&
						     (m.getContractId() == ptr->getContractId()) &&
						     (m.getOrderId() == ptr->getOrderId()) &&
						     (m.getTimestamp() <= ptr->getTimestamp()); });
	    if (itl != askLimitOrderBookPerPxLevel.end()) {
		(*itl).qty() -= ptr->getQty();
		/* for complete fill */
		if ((*itl).getQty() == 0) {
		    askLimitOrderBookPerPxLevel.erase(itl);
		    askLimitOrderBook_.erase(remove_if(askLimitOrderBook_.begin(),
						       askLimitOrderBook_.end(),
						       [&](vector<LimitOrderMessage> v_lom) { return v_lom.empty(); }),
					     askLimitOrderBook_.end());
		}
		return true;
	    }
	}

	return false;
    }

    bool OrderBook::rejectedOrderHelper(RejectedOrderMessage<LimitOrderMessage> const * ptr)
    {
	size_t bidLimitOrderNumber = accumulate(bidLimitOrderBook_.begin(),
						bidLimitOrderBook_.end(),
						0,
						[&](size_t size, vector<LimitOrderMessage> v) { return size + v.size(); });
	for (auto & bidLimitOrderBookPerPxLevel : bidLimitOrderBook_) {
	    bidLimitOrderBookPerPxLevel.erase(remove_if(bidLimitOrderBookPerPxLevel.begin(),
							bidLimitOrderBookPerPxLevel.end(),
							[&](LimitOrderMessage m) { return (m.getExchangeId() == ptr->getExchangeId()) &&
								(m.getContractId() == ptr->getContractId()) &&
								(m.getOrderId() == ptr->getOrderId()) &&
								(m.getTimestamp() <= ptr->getTimestamp()); }),
					      bidLimitOrderBookPerPxLevel.end());
	}
	bidLimitOrderBook_.erase(remove_if(bidLimitOrderBook_.begin(),
					   bidLimitOrderBook_.end(),
					   [&](vector<LimitOrderMessage> v_lom) { return v_lom.empty(); }),
				 bidLimitOrderBook_.end());
	size_t numBidLimitOrderMatched = bidLimitOrderNumber - accumulate(bidLimitOrderBook_.begin(),
									  bidLimitOrderBook_.end(),
									  0,
									  [&](size_t size, vector<LimitOrderMessage> v) { return size + v.size(); });
	    
	size_t askLimitOrderNumber = accumulate(askLimitOrderBook_.begin(),
						askLimitOrderBook_.end(),
						0,
						[&](size_t size, vector<LimitOrderMessage> v) { return size + v.size(); });
	for (auto & askLimitOrderBookPerPxLevel : askLimitOrderBook_) {
	    askLimitOrderBookPerPxLevel.erase(remove_if(askLimitOrderBookPerPxLevel.begin(),
							askLimitOrderBookPerPxLevel.end(),
							[&](LimitOrderMessage m) { return (m.getExchangeId() == ptr->getExchangeId()) &&
								(m.getContractId() == ptr->getContractId()) &&
								(m.getOrderId() == ptr->getOrderId()) &&
								(m.getTimestamp() <= ptr->getTimestamp()); }),
					      askLimitOrderBookPerPxLevel.end());
	}
	askLimitOrderBook_.erase(remove_if(askLimitOrderBook_.begin(),
					   askLimitOrderBook_.end(),
					   [&](vector<LimitOrderMessage> v_lom) { return v_lom.empty(); }),
				 askLimitOrderBook_.end());
	size_t numAskLimitOrderMatched = askLimitOrderNumber - accumulate(askLimitOrderBook_.begin(),
									  askLimitOrderBook_.end(),
									  0,
									  [&](size_t size, vector<LimitOrderMessage> v) { return size + v.size(); });
	    
	if (numBidLimitOrderMatched != 0 || numAskLimitOrderMatched != 0)
	    return true;
	else
	    return false;
    }

    bool OrderBook::rejectedOrderHelper(RejectedOrderMessage<MarketOrderMessage> const * ptr)
    {
	size_t marketOrderNumber = marketOrderBook_.size();
	marketOrderBook_.erase(remove_if(marketOrderBook_.begin(), 
					 marketOrderBook_.end(),
					     [&](MarketOrderMessage m) { return (m.getExchangeId() == ptr->getExchangeId()) &&
						 (m.getContractId() == ptr->getContractId()) &&
						 (m.getOrderId() == ptr->getOrderId()) &&
						 (m.getTimestamp() <= ptr->getTimestamp()); }),
			       marketOrderBook_.end());
	
	if (marketOrderBook_.size() != marketOrderNumber)
	    return true;
	else
	    return false;
    }

    bool OrderBook::rejectedOrderHelper(RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage> > const * ptr)
    {
	size_t bidLimitOrderNumber = accumulate(bidLimitOrderBook_.begin(),
						bidLimitOrderBook_.end(),
						0,
						[&](size_t size, vector<LimitOrderMessage> v) { return size + v.size(); });
	for (auto & bidLimitOrderBookPerPxLevel : bidLimitOrderBook_) {
	    bidLimitOrderBookPerPxLevel.erase(remove_if(bidLimitOrderBookPerPxLevel.begin(),
							bidLimitOrderBookPerPxLevel.end(),
							[&](LimitOrderMessage m) { return (m.getExchangeId() == ptr->getExchangeId()) &&
								(m.getContractId() == ptr->getContractId()) &&
								(m.getOrderId() == ptr->getOrderId()) &&
								(m.getTimestamp() <= ptr->getTimestamp()); }),
					      bidLimitOrderBookPerPxLevel.end());
	}
	bidLimitOrderBook_.erase(remove_if(bidLimitOrderBook_.begin(),
					   bidLimitOrderBook_.end(),
					   [&](vector<LimitOrderMessage> v_lom) { return v_lom.empty(); }),
				 bidLimitOrderBook_.end());
	size_t numBidLimitOrderMatched = bidLimitOrderNumber - accumulate(bidLimitOrderBook_.begin(),
									  bidLimitOrderBook_.end(),
									  0,
									  [&](size_t size, vector<LimitOrderMessage> v) { return size + v.size(); });
	    
	size_t askLimitOrderNumber = accumulate(askLimitOrderBook_.begin(),
						askLimitOrderBook_.end(),
						0,
						[&](size_t size, vector<LimitOrderMessage> v) { return size + v.size(); });
	for (auto & askLimitOrderBookPerPxLevel : askLimitOrderBook_) {
	    askLimitOrderBookPerPxLevel.erase(remove_if(askLimitOrderBookPerPxLevel.begin(),
							askLimitOrderBookPerPxLevel.end(),
							[&](LimitOrderMessage m) { return (m.getExchangeId() == ptr->getExchangeId()) &&
								(m.getContractId() == ptr->getContractId()) &&
								(m.getOrderId() == ptr->getOrderId()) &&
								(m.getTimestamp() <= ptr->getTimestamp()); }),
					      askLimitOrderBookPerPxLevel.end());
	}
	askLimitOrderBook_.erase(remove_if(askLimitOrderBook_.begin(),
					   askLimitOrderBook_.end(),
					   [&](vector<LimitOrderMessage> v_lom) { return v_lom.empty(); }),
				 askLimitOrderBook_.end());
	size_t numAskLimitOrderMatched = askLimitOrderNumber - accumulate(askLimitOrderBook_.begin(),
									  askLimitOrderBook_.end(),
									  0,
									  [&](size_t size, vector<LimitOrderMessage> v) { return size + v.size(); });

	LimitOrderMessage lom(ptr->getExchangeId(), ptr->getContractId(), ptr->getTimestamp(), ptr->getStratId(), ptr->getMessageId(), ptr->getOrderId(), ptr->getQty(), ptr->getSide(), ptr->getPx());
	limitOrderHelper(&lom);
	    
	if (numBidLimitOrderMatched != 0 || numAskLimitOrderMatched != 0)
	    return false;
	else
	    return true;
    }

    bool OrderBook::rejectedOrderHelper(RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage> > const * ptr)
    {
	size_t marketOrderNumber = marketOrderBook_.size();
	marketOrderBook_.erase(remove_if(marketOrderBook_.begin(), 
					 marketOrderBook_.end(),
					     [&](MarketOrderMessage m) { return (m.getExchangeId() == ptr->getExchangeId()) &&
						 (m.getContractId() == ptr->getContractId()) &&
						 (m.getOrderId() == ptr->getOrderId()) &&
						 (m.getTimestamp() <= ptr->getTimestamp()); }),
			       marketOrderBook_.end());
	size_t numMarketOrderMatched = marketOrderNumber - marketOrderBook_.size();

	MarketOrderMessage mom(ptr->getExchangeId(), ptr->getContractId(), ptr->getTimestamp(), ptr->getStratId(), ptr->getMessageId(), ptr->getOrderId(), ptr->getQty(), ptr->getSide());
	marketOrderHelper(&mom);
	
	if ( numMarketOrderMatched != 0)
	    return false;
	else
	    return true;
    }

    int OrderBook::countMarketOrderQty(BuySellSide side)
    {
	int qty = accumulate(marketOrderBook_.begin(),
			     marketOrderBook_.end(),
			     0,
			     [&](int size, MarketOrderMessage m) { return (m.getSide() == side) ? (size + m.getQty()) : size; });
	return qty;
    }    

    int OrderBook::countMarketOrderQty()
    {
	return countMarketOrderQty(BuySellSide::buy) + countMarketOrderQty(BuySellSide::sell);
    }

    int OrderBook::countMarketOrders(BuySellSide side)
    {
	int num_order = accumulate(marketOrderBook_.begin(),
				   marketOrderBook_.end(),
				   0,
				   [&](int size, MarketOrderMessage m) { return (m.getSide() == side) ? (size + 1) : size; });
	return num_order;
    }    

    int OrderBook::countMarketOrders()
    {
	return countMarketOrders(BuySellSide::buy) + countMarketOrders(BuySellSide::sell);
    }

    int OrderBook::countLimitOrders(BuySellSide side)
    {
	int num_order = 0;
	if (side == BuySellSide::buy) {
	    num_order = accumulate(bidLimitOrderBook_.begin(),
				   bidLimitOrderBook_.end(),
				   0,
				   [&](int size, vector<LimitOrderMessage> v_lom) { return size + v_lom.size(); });
	}
	else if (side == BuySellSide::sell) {
	    num_order = accumulate(askLimitOrderBook_.begin(),
				   askLimitOrderBook_.end(),
				   0,
				   [&](int size, vector<LimitOrderMessage> v_lom) { return size + v_lom.size(); });
	}

	return num_order;
    }
    
    int OrderBook::countLimitOrders()
    {
	return countLimitOrders(BuySellSide::buy) + countLimitOrders(BuySellSide::sell);
    }
    
    int OrderBook::countLimitOrders(BuySellSide side, Px px)
    {
	int num_order = 0;
	if (side == BuySellSide::buy) {
	    num_order = accumulate(bidLimitOrderBook_.begin(),
				   bidLimitOrderBook_.end(),
				   0,
				   [&](int size, vector<LimitOrderMessage> v_lom) 
				   { return size + accumulate(v_lom.begin(), 
							      v_lom.end(), 
							      0, 
							      [&](int qty, LimitOrderMessage m) { return (m.getPx() == px) ? (qty + 1) : qty; }); 
				   });
	}
	else if (side == BuySellSide::sell) {
	    num_order = accumulate(askLimitOrderBook_.begin(),
				   askLimitOrderBook_.end(),
				   0,
				   [&](int size, vector<LimitOrderMessage> v_lom) 
				   { return size + accumulate(v_lom.begin(), 
							      v_lom.end(), 
							      0, 
							      [&](int qty, LimitOrderMessage m) { return (m.getPx() == px) ? (qty + 1) : qty; }); 
				   });
	}

	return num_order;
    }
    
    int OrderBook::countLimitOrders(Px px)
    {
	return countLimitOrders(BuySellSide::buy, px) + countLimitOrders(BuySellSide::sell, px);
    }    

    int OrderBook::countLimitOrderQty(BuySellSide side)
    {
	int order_qty = 0;
	if (side == BuySellSide::buy) {
	    order_qty = accumulate(bidLimitOrderBook_.begin(),
				   bidLimitOrderBook_.end(),
				   0,
				   [&](int size, vector<LimitOrderMessage> v_lom) 
				   { return size + accumulate(v_lom.begin(), 
							      v_lom.end(), 
							      0, 
							      [&](int qty, LimitOrderMessage m) { return qty + m.getQty(); }); 
				   });
	}
	else if (side == BuySellSide::sell) {
	    order_qty = accumulate(askLimitOrderBook_.begin(),
				   askLimitOrderBook_.end(),
				   0,
				   [&](int size, vector<LimitOrderMessage> v_lom) 
				   { return size + accumulate(v_lom.begin(), 
							      v_lom.end(), 
							      0, 
							      [&](int qty, LimitOrderMessage m) { return qty + m.getQty(); }); 
				   });
	}

	return order_qty;
    }
    
    int OrderBook::countLimitOrderQty()
    {
	return countLimitOrderQty(BuySellSide::buy) + countLimitOrderQty(BuySellSide::sell);
    }
    
    int OrderBook::countLimitOrderQty(BuySellSide side, Px px)
    {
	int order_qty = 0;
	if (side == BuySellSide::buy) {
	    order_qty = accumulate(bidLimitOrderBook_.begin(),
				   bidLimitOrderBook_.end(),
				   0,
				   [&](int size, vector<LimitOrderMessage> v_lom) 
				   { return size + accumulate(v_lom.begin(), 
							      v_lom.end(), 
							      0, 
							      [&](int qty, LimitOrderMessage m) { return (m.getPx() == px) ? (qty + m.getQty()) : qty; }); 
				   });
	}
	else if (side == BuySellSide::sell) {
	    order_qty = accumulate(askLimitOrderBook_.begin(),
				   askLimitOrderBook_.end(),
				   0,
				   [&](int size, vector<LimitOrderMessage> v_lom) 
				   { return size + accumulate(v_lom.begin(), 
							      v_lom.end(), 
							      0, 
							      [&](int qty, LimitOrderMessage m) { return (m.getPx() == px) ? (qty + m.getQty()) : qty; }); 
				   });
	}

	return order_qty;
    }
    
    int OrderBook::countLimitOrderQty(Px px)
    {
	return countLimitOrderQty(BuySellSide::buy, px) + countLimitOrderQty(BuySellSide::sell, px);
    }

}
