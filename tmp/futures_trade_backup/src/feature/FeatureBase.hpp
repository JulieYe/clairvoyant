//
// File:        $ FeatureBase.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2014/01/25 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2014 by Zhengzhang Futures Co. Ltd.

#ifndef FEATUREBASE_HPP
#define FEATUREBASE_HPP

#include <chrono>
#include <string>
#include <tuple>
#include <deque>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <memory>
#include <limits>

const static double TOL = 1e-6;
const static double NTOL = -1e-6;

using std::shared_ptr;
using std::string;
using std::chrono::time_point;
using std::chrono::system_clock;
using std::nano;
using std::tuple;
using std::get;
using std::make_tuple;
using std::stringstream;
using std::accumulate;
using std::remove_if;
using std::sqrt;
using std::pow;
using std::deque;
using std::pow;
using std::max;
using std::min;
using std::log;
using std::abs;
using std::round;
using std::numeric_limits;

#include "Message.hpp"
#include "QtEMA.hpp"
#include "Technical.hpp"

using simulator::AbstractMessage;
using simulator::BuySellSide;
using simulator::FrontBackSide;
using simulator::AbstractMessage;
using simulator::LimitOrderMessage;
using simulator::MarketOrderMessage;
using simulator::CancelOrderMessage;
using simulator::AcknowledgedOrderMessage;
using simulator::FilledOrderMessage;
using simulator::CanceledOrderMessage;
using simulator::RejectedOrderMessage;
using simulator::BookUpdate;
using simulator::ImpliedSpreadTopBookUpdate;
using simulator::TradeUpdate;
using simulator::time_stamp;
using simulator::duration_nano;
using simulator::QtEMA;
using simulator::QtMVar;
using simulator::QtMA;

namespace simulator
{

    class Feature
    {
    protected:
	string featureName_;
	double value_;
	double badValue_;
	time_stamp currTime_;
	bool isGood_;
	
	bool useBound_ = false;
	double upperBound_ = numeric_limits<double>::max();
	double lowerBound_ = numeric_limits<double>::min();

    public:
	Feature();
	Feature(string featureName);
	Feature(string featureName, double initValue);
	Feature(string featureName, double initValue, double badValue);

	~Feature();

	virtual void update(time_stamp ts) { }
	virtual void update(MarketOrderMessage order) { }
	virtual void update(LimitOrderMessage order) { }
	virtual void update(CanceledOrderMessage<LimitOrderMessage> cancel) { }
	virtual void update(CanceledOrderMessage<MarketOrderMessage> cancel) { }
	virtual void update(FilledOrderMessage fill) { }
	virtual void update(AcknowledgedOrderMessage ack) { }
	virtual void update(RejectedOrderMessage<LimitOrderMessage> reject) { }
	virtual void update(RejectedOrderMessage<MarketOrderMessage> reject) { }
	virtual void update(RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage> > reject) { }
	virtual void update(RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage> > reject) { }
	virtual void update(BookUpdate<true> book) { }
	virtual void update(TradeUpdate<true> trade) { }
	virtual void update(BookUpdate<true> book, TradeUpdate<true> trade) { }
	virtual void update(BookUpdate<true> book, bool isFrontContractQ) { }
	virtual void update(TradeUpdate<true> trade, bool isFrontContractQ) { }
	virtual void update(BookUpdate<true> book, TradeUpdate<true> trade, bool isFrontContractQ) { }
	// virtual void update(ImpliedSpreadTopBookUpdate<true> ibook) { }
    
	void setBound(bool useBound, double bound);

	inline double getValue() const
	{
	    return isGood_ ? (useBound_? max(lowerBound_, min(value_, upperBound_)) : value_) : badValue_;
	}

	inline bool isGood() const
	{
	    return isGood_;
	}

	inline string getName() const
	{
	    return featureName_;
	}
    };

    typedef shared_ptr<Feature> PFeature;

}

#endif  // FEATUREBASE_HPP
