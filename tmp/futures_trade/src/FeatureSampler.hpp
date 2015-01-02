//
// File:        $ FeatureSampler.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/08/29 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef FEATURESAMPLER_HPP
#define FEATURESAMPLER_HPP

#include <vector>
#include <chrono>
#include <string>
#include <utility>
#include <list>
#include <algorithm>
#include <sstream>

using std::string;
using std::vector;
using std::chrono::time_point;
using std::chrono::system_clock;
using std::chrono::duration;
using std::nano;
using std::list;
using std::pair;
using std::for_each;
using std::stringstream;
using std::all_of;

#include "Feature.hpp"
#include "Message.hpp"
#include "SimulatorCore.hpp"

using simulator::Feature;
using simulator::AbstractMessage;
using simulator::BuySellSide;
using simulator::AbstractMessage;
using simulator::LimitOrderMessage;
using simulator::MarketOrderMessage;
using simulator::CancelOrderMessage;
using simulator::AcknowledgedOrderMessage;
using simulator::FilledOrderMessage;
using simulator::CanceledOrderMessage;
using simulator::RejectedOrderMessage;
using simulator::BookUpdate;
using simulator::TradeUpdate;
using simulator::time_stamp;
using simulator::duration_nano;
using simulator::PFeature;
using simulator::SimulatorCore;

namespace simulator
{

    struct FeatureInfo
    {
	PFeature pFeature_;
	duration_nano delayNano_;
    };
    
    class FeatureSampler
    {
    private:
	typedef pair<time_stamp, vector<double> > DataPoint;

	vector<FeatureInfo> featureInfoVec_;
	duration_nano maxDelayNano_;
	list<DataPoint> data_;
	time_stamp lastFeatureUpdateTs_;

    public:
	FeatureSampler();
    
	// initializer
	void init();
    
	// accessor to featureInfoVec_
	inline vector<FeatureInfo> & featureInfoVector()
	{
	    return featureInfoVec_;
	}
    
	inline time_stamp getLastFeatureUpdateTs() const
	{
	    return lastFeatureUpdateTs_;
	}
    
	// data updater
	void updateData(time_stamp ts);
    
	// get a string for completed vectors
	string flushCompletedVectors(time_stamp ts);

	// feature value updator
	void update(time_stamp ts);
	void update(MarketOrderMessage order);
	void update(LimitOrderMessage order);
	void update(CanceledOrderMessage<LimitOrderMessage> cancel);
	void update(CanceledOrderMessage<MarketOrderMessage> cancel);
	void update(FilledOrderMessage fill);
	void update(AcknowledgedOrderMessage ack);
	void update(RejectedOrderMessage<LimitOrderMessage> reject);
	void update(RejectedOrderMessage<MarketOrderMessage> reject);
	void update(RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage> > reject);
	void update(RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage> > reject);
	void update(BookUpdate<true> book);
	void update(TradeUpdate<true> trade);
	void update(BookUpdate<true> book, TradeUpdate<true> trade);
	void update(BookUpdate<true> book, bool isFrontContractQ);
	void update(TradeUpdate<true> trade, bool isFrontContractQ);
	void update(BookUpdate<true> book, TradeUpdate<true> trade, bool isFrontContractQ);
    };
    
}
  
#endif  // FEATURESAMPLER_HPP
