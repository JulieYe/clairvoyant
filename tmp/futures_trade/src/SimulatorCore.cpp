//
// File:        $ SimulatorCore.cpp $
//
// Version:     $ Revision: 1.1 $
//
// Modified:    $ Date: 2014/01/26 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2014 by Zhengzhang Futures Co. Ltd.

#include "SimulatorCore.hpp"

namespace simulator
{

    void SimulatorCore::registerMatchy(Matchy *pMatchy)
    {
	exchanges_.push_back(pMatchy);
	numExchanges_ = exchanges_.size();
    }

    int SimulatorCore::getNumExchanges() const
    {
	return numExchanges_;
    }

    void SimulatorCore::registerStrategy(Strategy *pStrategy)
    {
#ifdef PRODUCTION
	stratUpdTrigger_.push_back(false);
#endif
        strategies_.push_back(pStrategy);
	pStrategy->setStratId(numStrategies_);
	numStrategies_ = strategies_.size();
    }

    int SimulatorCore::getNumStrategies() const
    {
	return numStrategies_;
    }

    // "2012-10-29T09:15:00.1234"
    time_stamp 
    SimulatorCore::convertStringToTimePoint(string datetime)
    {
	return simulator::convertStringToTimePoint(datetime);
    }

    // "2012-10-29T09:15:00.1234"
    string SimulatorCore::convertTimePointToString(time_stamp tp)
    {
	return simulator::convertTimePointToString(tp);
    }

    size_t SimulatorCore::unitIndex(string &s)
    {
	const char* stepUnits[] = {"nan", "mic", "mil", "sec", "min"};
	size_t stepUnitsLength = extent<decltype(stepUnits)>::value;
	s += "---";
	s.erase(3);
	transform(s.begin(), s.end(), s.begin(), function<int (int)> (tolower));
	size_t result = find(stepUnits, stepUnits + stepUnitsLength, s) - stepUnits;
	return (result < stepUnitsLength) ? (result + 1) : 0;
    }

    SimulatorCore::SimulatorCore(string startDateTime, string stopDateTime, int timeStep, string stepUnit)
	: startDateTime_(startDateTime), stopDateTime_(stopDateTime), 
	  isAllMarketDataReady_(false), isAllTraderReady_(false), 
	  numExchanges_(0), numStrategies_(0)
    {
	startTime_ = convertStringToTimePoint(startDateTime);
	stopTime_ = convertStringToTimePoint(stopDateTime);
	currTime_ = convertStringToTimePoint(startDateTime);

	unsigned long long numNanoseconds;
	switch (unitIndex(stepUnit))
	{
	case 1: 
	    numNanoseconds = 1;
	    break;
	case 2: 
	    numNanoseconds = 1000;
	    break;
	case 3: 
	    numNanoseconds = 1000000;
	    break;
	case 4: 
	    numNanoseconds = 1000000000;
	    break;
	case 5: 
	    numNanoseconds = 60000000000;
	    break;
	default: 
	    cout << "Cannot match time step unit, set to nanosecond" << endl;
	    numNanoseconds = 1;	    
	    break;
	}

	timeStep_ = duration_nano(timeStep * numNanoseconds);
    }

    time_stamp
    SimulatorCore::getStartTime()
    {
	return startTime_;
    }

    time_stamp
    SimulatorCore::getStopTime()
    {
	return stopTime_;
    }

    void SimulatorCore::increment()
    {
	currTime_ += timeStep_;
    }

    bool SimulatorCore::end() const
    {
	return currTime_ > stopTime_;
    }

#ifdef SIMULATION
    bool SimulatorCore::next()
    {
      increment();
      if (end())
	  return false;
      else {
	  for (auto & iter : exchanges_) {
	      (*iter).process(0);
	      getMessagesFromMatchy(iter);
	  }
	  // clearStrategyMessages();

	  for (auto & iter : strategies_) {
	       (*iter).update();
	       // getMessagesFromStrategy(iter);
	  }
	  clearMatchyMessages();

	  return true;
      }
    }

    time_stamp
    SimulatorCore::getCurrTime()
    {
	return currTime_;
    }
#endif

#ifdef PRODUCTION
    void SimulatorCore::resetStratUpdTrigger()
    {
	for (auto it = stratUpdTrigger_.begin();
	     it != stratUpdTrigger_.end();
	     ++it) {
	    *it = false;
	}
    }

    bool SimulatorCore::getStratUpdTrigger(int i)
    {
	return stratUpdTrigger_[i];
    }

    void SimulatorCore::setStratUpdTrigger(int i, bool b)
    {
	stratUpdTrigger_[i] = b;
    }

    void SimulatorCore::next()
    {
	isAllMarketDataReady_ = true;
	isAllTraderReady_ = true;
	// For each exchange, copy all inbound messages.
	for (auto & iter : exchanges_) {
	    getMessagesFromMatchy(iter);
	    isAllMarketDataReady_ &= iter->marketDataReadiness();
	    isAllTraderReady_ &= iter->traderReadiness();
	}

	// React on inbound messages and copy messages 
	// from strategies to *this.
	// only react on the triggered strategies
	int i = 0;
	for (auto & iter : strategies_) {
	    if (getStratUpdTrigger(i++)) {
		(*iter).update();
		// getMessagesFromStrategy(iter);
	    }
	}
	resetStratUpdTrigger();
	clearMatchyMessages();

	// For each exchange, copy all outbound messages from 
	// strategies and submit orders.
	// quick hack to synchronize global ascending order ids.
	for (auto & iter : exchanges_) { 
	    // Update global order id.
	    (*iter).setOrderId(orderId_);
	}

	// process market order cancels 
	while (!cancelMarketOrderMessages_.empty()) { 
	    for (auto & iter : exchanges_) { 
		if ((*iter).processCancelMarketOrder(cancelMarketOrderMessages_.front())) { 
		    break;
		}
	    }
	    cancelMarketOrderMessages_.pop_front();
	}

	// process limit order cancels
	while (!cancelLimitOrderMessages_.empty()) {
	    for (auto & iter : exchanges_) { 
		if ((*iter).processCancelLimitOrder(cancelLimitOrderMessages_.front())) { 
		    break;
		}
	    }
	    cancelLimitOrderMessages_.pop_front();
	}
		       
	// process limit orders and market orders by ascending order ids.
	while (!marketOrderMessages_.empty() || !limitOrderMessages_.empty()) { 
	    if (!marketOrderMessages_.empty() && !limitOrderMessages_.empty()) { 
		// process the order with the lowest orderId.
		if (marketOrderMessages_.front().getOrderId() < limitOrderMessages_.front().getOrderId()) { 
		    for (auto & iter : exchanges_ ) { 
			if ((*iter).processMarketOrder(marketOrderMessages_.front())) { 
			    break;
			}
		    }
		    // always remove the order from the stack whatsoever
		    marketOrderMessages_.pop_front();
		} else { 
		    for (auto & iter : exchanges_ ) { 
			if ((*iter).processLimitOrder(limitOrderMessages_.front())) { 
			    break;
			}
		    }
		    limitOrderMessages_.pop_front();
		}
	    } else if (!marketOrderMessages_.empty()) { 
		for (auto & iter : exchanges_ ) { 
		    if ((*iter).processMarketOrder(marketOrderMessages_.front())) { 
			break;
		    }
		}
		marketOrderMessages_.pop_front();
	    } else {
		for (auto & iter : exchanges_ ) { 
		    if ((*iter).processLimitOrder(limitOrderMessages_.front())) { 
			break;
		    }
		}
		limitOrderMessages_.pop_front();
	    }
	}
	// clearStrategyMessages();
    }

    time_stamp
    SimulatorCore::getCurrTime()
    {
	clock_gettime(CLOCK_REALTIME, &tp_);
	time_stamp tp(duration_nano(tp_.tv_sec * duration_nano::period::den / duration_nano::period::num));
	duration_nano numNanoseconds(tp_.tv_nsec);
	tp += numNanoseconds;

	return tp;
    }

    duration_nano & SimulatorCore::timeOffset()
    {
	return timeOffset_;
    }

    bool & SimulatorCore::timeOffsetQ()
    {
	return timeOffsetQ_;
    }
#endif

#ifdef SIMULATION
    void SimulatorCore::getMessagesFromMatchy(Matchy * ptr)
    {
	for_each((ptr->acknowledgedOrderMessages()).begin(), 
		 (ptr->acknowledgedOrderMessages()).end(), 
		 [&](AcknowledgedOrderMessage & m) { acknowledgedOrderMessages_.push_back(move(m)); });

	for_each((ptr->filledOrderMessages()).begin(), 
		 (ptr->filledOrderMessages()).end(), 
		 [&](FilledOrderMessage & m) { filledOrderMessages_.push_back(move(m)); });

	for_each((ptr->canceledLimitOrderMessages()).begin(), 
		 (ptr->canceledLimitOrderMessages()).end(), 
		 [&](CanceledOrderMessage<LimitOrderMessage> & m) { canceledLimitOrderMessages_.push_back(move(m)); });

	for_each((ptr->canceledMarketOrderMessages()).begin(), 
		 (ptr->canceledMarketOrderMessages()).end(), 
		 [&](CanceledOrderMessage<MarketOrderMessage> & m) { canceledMarketOrderMessages_.push_back(move(m)); });
	    
	for_each((ptr->rejectedLimitOrderMessages()).begin(), 
		 (ptr->rejectedLimitOrderMessages()).end(), 
		 [&](RejectedOrderMessage<LimitOrderMessage> & m) { rejectedLimitOrderMessages_.push_back(move(m)); });

	for_each((ptr->rejectedMarketOrderMessages()).begin(), 
		 (ptr->rejectedMarketOrderMessages()).end(), 
		 [&](RejectedOrderMessage<MarketOrderMessage> & m) { rejectedMarketOrderMessages_.push_back(move(m)); });

	for_each((ptr->rejectedCancelLimitOrderMessages()).begin(), 
		 (ptr->rejectedCancelLimitOrderMessages()).end(), 
		 [&](RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage> > & m) { rejectedCancelLimitOrderMessages_.push_back(move(m)); });

	for_each((ptr->rejectedCancelMarketOrderMessages()).begin(), 
		 (ptr->rejectedCancelMarketOrderMessages()).end(), 
		 [&](RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage> > & m) { rejectedCancelMarketOrderMessages_.push_back(move(m)); });

	for_each((ptr->bookUpdates()).begin(),
		 (ptr->bookUpdates()).end(),
		 [&](BookUpdate<true> & bookUpdate) { bookUpdates_.push_back(move(bookUpdate)); });

	for_each((ptr->tradeUpdates()).begin(),
		 (ptr->tradeUpdates()).end(),
		 [&](TradeUpdate<true> & tradeUpdate) { tradeUpdates_.push_back(move(tradeUpdate)); });
    }
#endif

#ifdef PRODUCTION
    void SimulatorCore::getMessagesFromMatchy(Matchy * ptr)
    {
	for_each((ptr->acknowledgedOrderMessages()).begin(), 
		 (ptr->acknowledgedOrderMessages()).end(), 
		 [&](AcknowledgedOrderMessage & m) { acknowledgedOrderMessages_.push_back(move(m)); });
	(ptr->acknowledgedOrderMessages()).clear();

	for_each((ptr->filledOrderMessages()).begin(), 
		 (ptr->filledOrderMessages()).end(), 
		 [&](FilledOrderMessage & m) { filledOrderMessages_.push_back(move(m)); });
	(ptr->filledOrderMessages()).clear();
	
	for_each((ptr->canceledLimitOrderMessages()).begin(), 
		 (ptr->canceledLimitOrderMessages()).end(), 
		 [&](CanceledOrderMessage<LimitOrderMessage> & m) { canceledLimitOrderMessages_.push_back(move(m)); });
	(ptr->canceledLimitOrderMessages()).clear();
	
	for_each((ptr->canceledMarketOrderMessages()).begin(), 
		 (ptr->canceledMarketOrderMessages()).end(), 
		 [&](CanceledOrderMessage<MarketOrderMessage> & m) { canceledMarketOrderMessages_.push_back(move(m)); });
	(ptr->canceledMarketOrderMessages()).clear();
	
	for_each((ptr->rejectedLimitOrderMessages()).begin(), 
		 (ptr->rejectedLimitOrderMessages()).end(), 
		 [&](RejectedOrderMessage<LimitOrderMessage> & m) { rejectedLimitOrderMessages_.push_back(move(m)); });
	(ptr->rejectedLimitOrderMessages()).clear();
	
	for_each((ptr->rejectedMarketOrderMessages()).begin(), 
		 (ptr->rejectedMarketOrderMessages()).end(), 
		 [&](RejectedOrderMessage<MarketOrderMessage> & m) { rejectedMarketOrderMessages_.push_back(move(m)); });
	(ptr->rejectedMarketOrderMessages()).clear();
	
	for_each((ptr->rejectedCancelLimitOrderMessages()).begin(), 
		 (ptr->rejectedCancelLimitOrderMessages()).end(), 
		 [&](RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage> > & m) { rejectedCancelLimitOrderMessages_.push_back(move(m)); });
	(ptr->rejectedCancelLimitOrderMessages()).clear();
	
	for_each((ptr->rejectedCancelMarketOrderMessages()).begin(), 
		 (ptr->rejectedCancelMarketOrderMessages()).end(), 
		 [&](RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage> > & m) { rejectedCancelMarketOrderMessages_.push_back(move(m)); });
	(ptr->rejectedCancelMarketOrderMessages()).clear();

	for_each((ptr->bookUpdates()).begin(),
		 (ptr->bookUpdates()).end(),
		 [&](BookUpdate<true> & bookUpdate) { bookUpdates_.push_back(move(bookUpdate)); });
	(ptr->bookUpdates()).clear();
	
	for_each((ptr->tradeUpdates()).begin(),
		 (ptr->tradeUpdates()).end(),
		 [&](TradeUpdate<true> & tradeUpdate) { tradeUpdates_.push_back(move(tradeUpdate)); });
	(ptr->tradeUpdates()).clear();
    }
#endif

    // duplicate implementation, same functionality is already implemented in Strategy::postProcess
    // commented out
    // void SimulatorCore::getMessagesFromStrategy(Strategy * ptr)
    // {
    // 	for_each((ptr->limitOrderMessages()).begin(), 
    // 		 (ptr->limitOrderMessages()).end(), 
    // 		 [&](LimitOrderMessage & m) { limitOrderMessages_.push_back(move(m)); });
    // 	for_each((ptr->marketOrderMessages()).begin(), 
    // 		 (ptr->marketOrderMessages()).end(), 
    // 		 [&](MarketOrderMessage & m) { marketOrderMessages_.push_back(move(m)); });
    // 	for_each((ptr->cancelLimitOrderMessages()).begin(), 
    // 		 (ptr->cancelLimitOrderMessages()).end(), 
    // 		 [&](CancelOrderMessage<LimitOrderMessage> & m) { cancelLimitOrderMessages_.push_back(move(m)); });
    // 	for_each((ptr->cancelMarketOrderMessages()).begin(), 
    // 		 (ptr->cancelMarketOrderMessages()).end(), 
    // 		 [&](CancelOrderMessage<MarketOrderMessage> & m) { cancelMarketOrderMessages_.push_back(move(m)); });
    // }

    void SimulatorCore::clearMatchyMessages()
    {
	tradeUpdates_.clear();
	bookUpdates_.clear();
	acknowledgedOrderMessages_.clear();
	filledOrderMessages_.clear();
	canceledLimitOrderMessages_.clear();
	canceledMarketOrderMessages_.clear();
	rejectedLimitOrderMessages_.clear();
	rejectedMarketOrderMessages_.clear();
	rejectedCancelLimitOrderMessages_.clear();
	rejectedCancelMarketOrderMessages_.clear();
    }

    // duplicate implementation, same functionality is already implemented in SimMatchy/CFFEX classes
    // commented out
    // void SimulatorCore::clearStrategyMessages()
    // {
    // 	limitOrderMessages_.clear();
    // 	marketOrderMessages_.clear();
    // 	cancelLimitOrderMessages_.clear();
    // 	cancelMarketOrderMessages_.clear();
    // }

    //adding accessors for strategy initiated messages (by mzhang)
    list<AcknowledgedOrderMessage> & SimulatorCore::acknowledgedOrderMessages()
    {
        return acknowledgedOrderMessages_;
    }

    list<FilledOrderMessage> & SimulatorCore::filledOrderMessages()
    {
        return filledOrderMessages_;
    }

    list<CanceledOrderMessage<LimitOrderMessage> > & SimulatorCore::canceledLimitOrderMessages()
    {
        return canceledLimitOrderMessages_;
    }

    list<CanceledOrderMessage<MarketOrderMessage> > & SimulatorCore::canceledMarketOrderMessages()
    {
        return canceledMarketOrderMessages_;
    }

    list<RejectedOrderMessage<LimitOrderMessage> > & SimulatorCore::rejectedLimitOrderMessages()
    {
        return rejectedLimitOrderMessages_;
    }

    list<RejectedOrderMessage<MarketOrderMessage> > & SimulatorCore::rejectedMarketOrderMessages()
    {
        return rejectedMarketOrderMessages_;
    }

    list<RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage> > > & SimulatorCore::rejectedCancelLimitOrderMessages()
    {
        return rejectedCancelLimitOrderMessages_;
    }

    list<RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage> > > & SimulatorCore::rejectedCancelMarketOrderMessages()
    {
        return rejectedCancelMarketOrderMessages_;
    }
    
    list<TradeUpdate<true> > SimulatorCore::getTradeUpdate() const 
    {
	return tradeUpdates_;
    }

    list<TradeUpdate<true> > & SimulatorCore::tradeUpdate()
    {
	return tradeUpdates_;
    }

    list<BookUpdate<true> > SimulatorCore::getBookUpdate() const
    { 
	return bookUpdates_;
    }

    list<BookUpdate<true> > & SimulatorCore::bookUpdate()
    { 
	return bookUpdates_;
    }
}
