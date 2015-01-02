#include "SimCFFEXAlt.hpp"


namespace simulator
{
#ifdef SIMULATION
    SimCFFEXAlt::SimCFFEXAlt(string contractId, string fileName, SimulatorCore * simulatorCorePtr)
	: CFFEX(contractId, fileName, simulatorCorePtr)
    {
    }


    SimCFFEXAlt::SimCFFEXAlt(string exchangeId, string contractId, string fileName, double tickSize, SimulatorCore * simulatorCorePtr)
	: CFFEX(exchangeId, contractId, fileName, tickSize, simulatorCorePtr)
    {
    }


    SimCFFEXAlt::SimCFFEXAlt(string exchangeId, double tickSize) :
	CFFEX(exchangeId, tickSize)
    { 
    }

    void SimCFFEXAlt::process(int orderId)
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
	stashIncomingMessages();

	/* Iterate over market data, match orders with earlier timestamps */
	/* while putting snapshots of market data into outbound update message list
	   and outbound order message list. */
	auto timestampSC = simulatorCorePtr_->getCurrTime();
	// changed by Mingyuan
	// more notes
	// worldtime = timestampSC (the time we process market data up to worldtime)
	// localtime = currTime_ (time right before the next book update)
	// recentbooktime = timestampFreeze (time of the most recent book update)
	
	//while (!end() && currTime_ <= timestampSC) { 
	while (!end() && currBookUpdate_.getTimestamp() <= timestampSC) {
	    /* Work on market orders. */
	    // market order will match with most recent book; and only orders >= recentbooktime and < localtime and <= worldtime get to match
	    processMarketOrder(timestampSC, currTime_);
	    // current timestamp of the book
	    auto timestampFreeze = currBookUpdate_.getTimestamp();
	    // for marketable limit order
	    // do nothing!!!!

	    // process limit order (passive matching)
	    // limit order book will be matched with most recent book; only orders < recentbooktime get to match
	    // this should not do anything when the loop started
	    processLimitOrderInOrderBook(timestampSC, currTime_, timestampFreeze);
	    // process cancel orders 
	    // cancel will be done only for cancels >= recentbooktime and < localtime and <= worldtime
	    orderBook_.processStashedCancelOrderMessages(currTime_, canceledMarketOrderMessages_, rejectedCancelMarketOrderMessages_, messageId_);
	    orderBook_.processStashedCancelOrderMessages(currTime_, canceledLimitOrderMessages_, rejectedCancelLimitOrderMessages_, messageId_);

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



#endif
}
