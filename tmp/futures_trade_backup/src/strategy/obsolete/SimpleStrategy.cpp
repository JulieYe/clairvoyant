#include "Strategy.hpp"
#include "SimpleStrategy.cpp"


namespace simulator
{
    SimpleStrategy::SimpleStrategy(const string strat_name, SimulatorCore * simcore_ptr, Logger * logger_ptr)
	:Strategy(strat_name, simcore_ptr, logger_ptr)
    {
    }

    SimpleStrategy::~SimpleStrategy()
    {
    }

    void SimpleStrategy::addContract(string ex, string con, double mult)
    {
	addStrat(ex, con, mult);
    }

    void SimpleStrategy::logStatus()
    {
	logStrategyStatus();
    }

    void SimpleStrategy::process()
    {
    }

    bool SimpleStrategy::getCurrentBookUpdate(int prodid, BookMessage &bm)
    {
	if(bookUpdates_[prodid].size() > 0) {
	    auto bu = bookUpdates_[prodid].back();
	    bm.bid = bu.getBidPx()[0].getPx();
	    bm.ask = bu.getAskPx()[0].getPx();
	    bm.bidSize = bu.getBidQty()[0];
	    bm.askSize = bu.getAskQty()[0];
	    return true;
	} else {
	    return false;
	}
    }

    bool SimpleStrategy::getCurrentTradeUpdate(int prodid, TradeMessage &tm)
    {
	if(tradeUpdates_[prodid].size() > 0) {
	    auto tu = tradeUpdates_[prodid].back();
	    tm.lastPrice = tu.getPx().getPx();
	    tm.cumVolume = tu.getQty();
	    return true;
	} else {
	    return false;
	}
    }

    void cancelALimitOrder(int orderId, int prodid)
    {
	for(unsigned ii=0; ii < orderbooks_[prodid].getBidLimitOrderBook().size(); ii++) {
	    for(auto ilom = orderbooks_[prodid].getBidLimitOrderBook()[ii].begin();
		ilom != orderbooks_[prodid].getBidLimitOrderBook()[ii].end();
		ilom++) {
		if(ilom->getOrderId() == orderId) {
		    cancelLimitOrder((*ilom));
		}
	    }
	}
	for(unsigned ii=0; ii < orderbooks_[prodid].getAskLimitOrderBook().size(); ii++) {
	    for(auto ilom = orderbooks_[prodid].getAskLimitOrderBook()[ii].begin();
		ilom != orderbooks_[prodid].getAskLimitOrderBook()[ii].end();
		ilom++) {
		if(ilom->getOrderId() == orderId) {
		    cancelLimitOrder((*ilom));
		}
	    }
	}
    }


    void SimpleStrategy::placeAMarketOrder(bool isBuy, int ordersize, int prodid)
    {
	BuySellSide side = isBuy ? BuySellSide::buy : BuySellSide::sell;
	placeMarketOrder(side, ordersize, prodid);
    }


    void SimpleStrategy::placeALimitOrder(double price, double tickSize, bool isBuy, int ordersize, int prodid)
    {
	Px px(price, tickSize);
	BuySellSide side = isBuy ? BuySellSide::buy : BuySellSide::sell;
	placeLimitOrder(px, side, ordersize, prodid);
    }

}
