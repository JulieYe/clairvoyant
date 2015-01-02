//
// File:        $ Signal.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/08/31 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef SIGNAL_HPP
#define SIGNAL_HPP

#include <algorithm>
#include <list>
#include <iterator>
#include <functional>
#include <numeric>

using std::for_each;
using std::all_of;
using std::list;
using std::back_inserter;
using std::multiplies;
using std::accumulate;

#include "Feature.hpp"
#include "Message.hpp"

using simulator::Feature;
using simulator::time_stamp;
using simulator::duration_nano;
using simulator::BuySellSide;

namespace simulator
{

    template<typename... T> struct typelist {};

    template<typename TL> struct Length;

    template<>
    struct Length<typelist<> >
    {
	enum { value = 0 };
    };

    template<typename T, typename... U>
    struct Length<typelist<T, U...> >
    {
	enum { value = 1 + Length<typelist<U...> >::value };
    };

    class Signal
    {
    protected:
	BuySellSide signal_ = BuySellSide::unknown;

    public:
	BuySellSide getSignal() const;
	BuySellSide & signal();

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
    };

    template<typename... T>
    class SignalOnPxPredict : public Signal
    {
    private:
	double thresholdLower_;
	double thresholdUpper_;
	static const int size_ = Length<typelist<T...> >::value;
	vector<double> betas_;
	vector<PFeature> featurePtrs_;

	double rawValue_;
    public:
        double getThresholdLower() const { return thresholdLower_; }
	double & thresholdLower() { return thresholdLower_; }

	double getThresholdUpper() const { return thresholdUpper_; }
	double & thresholdUpper() { return thresholdUpper_; }

	vector<PFeature> getFeatures() const { return featurePtrs_;}

	double getRawValue() const { return rawValue_; }

	void registerThresholds(double thresholdLower, double thresholdUpper)
	{
	    thresholdLower_ = thresholdLower;
	    thresholdUpper_ = thresholdUpper;
	}

	void registerBeta() {}

	template<typename... Args>
	void registerBeta(double value, Args... args)
	{
	    betas_.push_back(value);
	    registerBeta(args...);
	}

	void registerFeature() {}

	template<typename... Args>
	void registerFeature(PFeature ptr, Args... args)
	{
	    featurePtrs_.push_back(ptr);
	    registerFeature(args...);
	}
	
	template<typename U>
	void update(U u) 
	{
	    list<double> crossProduct;
	    for_each(featurePtrs_.begin(), featurePtrs_.end(), [&] (PFeature feature) { feature->update(u); });
	    if (all_of(featurePtrs_.begin(), featurePtrs_.end(), [&] (PFeature feature) { return feature->isGood(); })) {
		transform(betas_.begin(), betas_.end(), featurePtrs_.begin(), back_inserter(crossProduct), [&](double x, PFeature y) {return x * y->getValue(); });
		double result = accumulate(crossProduct.begin(), crossProduct.end(), 0.0);
		rawValue_ = result;
		signal_ =  (result > thresholdUpper_) ? BuySellSide::buy : ((result < thresholdLower_) ? BuySellSide::sell : BuySellSide::unknown);
	    } else {
		rawValue_ = 0.0;
		signal_ = BuySellSide::unknown;
	    }
	}

	template<typename U, typename V>
	void update(U u, V v) 
	{
	    list<double> crossProduct;
	    for_each(featurePtrs_.begin(), featurePtrs_.end(), [&] (PFeature feature) { feature->update(u, v); });
	    if (all_of(featurePtrs_.begin(), featurePtrs_.end(), [&] (PFeature feature) { return feature->isGood(); })) {
		transform(betas_.begin(), betas_.end(), featurePtrs_.begin(), back_inserter(crossProduct), [&](double x, PFeature y) {return x * y->getValue(); });
		double result = accumulate(crossProduct.begin(), crossProduct.end(), 0.0);
		rawValue_ = result;
		signal_ =  (result > thresholdUpper_) ? BuySellSide::buy : ((result < thresholdLower_) ? BuySellSide::sell : BuySellSide::unknown);
	    } else {
		rawValue_ = 0.0;
		signal_ = BuySellSide::unknown;
	    }
	}
    };

}

#endif  // SIGNAL_HPP
