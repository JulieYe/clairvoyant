//
// File:        $ FeatureSampler.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/08/29 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include "FeatureSampler.hpp"

namespace simulator
{

    FeatureSampler::FeatureSampler() :
	featureInfoVec_(),
	maxDelayNano_(0),
	data_(),
	sampleFrequency_(1),
	doIgnorePeriod_(false)
    {}
    
    FeatureSampler::FeatureSampler(long sampleFrequencyMs) :
	featureInfoVec_(),
	maxDelayNano_(0),
	data_(),
	sampleFrequency_(sampleFrequencyMs * 1000000ULL),
	doIgnorePeriod_(false)
    {}


    // figure out maxDelayNano_
    void FeatureSampler::init() 
    {
	for (auto it = featureInfoVec_.begin(); it!=featureInfoVec_.end(); ++it) {
	    maxDelayNano_ = std::max(maxDelayNano_, (*it).delayNano_);
	}
    }
    
    void FeatureSampler::updateData(time_stamp ts)
    {
	if(!doIgnorePeriod_ || ts < ignoreBeginTs_ || ts > ignoreEndTs_ ) {
	    if (data_.size() == 0) {
		DataPoint dp(ts, vector<double>(featureInfoVec_.size(), NAN)) ;
		data_.push_back(dp);
	    }
	    if (lastFeatureUpdateTs_ > data_.back().first) {
		if(ts >= data_.back().first + sampleFrequency_) {
		    DataPoint dp(ts, vector<double>(featureInfoVec_.size(), NAN)) ;
		    data_.push_back(dp);
		}
	    }
	}
	for (unsigned int ii = 0; ii < featureInfoVec_.size(); ++ii) {
	    for (auto dp = data_.begin(); dp != data_.end(); ++dp) {
		if ((*dp).first <= ts - featureInfoVec_[ii].delayNano_ && 
		    std::isnan((*dp).second[ii]) && 
		    (featureInfoVec_[ii].pFeature_)->isGood()) {
		    (*dp).second[ii] = (featureInfoVec_[ii].pFeature_)->getValue();
		}
	    }
	}
    }
    
    string FeatureSampler::flushCompletedVectors(time_stamp ts)
    {
	stringstream ss;
	auto it = data_.begin();
	time_stamp cursor_ts = (*it).first - duration_nano(1);
	while (it != data_.end()) {
	    if ((*it).first < ts - maxDelayNano_) {
		if (all_of((*it).second.begin(), (*it).second.end(), [&](double x) { return !std::isnan(x); })) {
		    ss << (*it).first.time_since_epoch().count();
		    ss << ',' << SimulatorCore::convertTimePointToString((*it).first);
		    for (auto iit = (*it).second.begin(); iit != (*it).second.end(); ++iit) {
			ss << ',' << (*iit);
		    }
		    ss << '\n';
		}
		++it;
		cursor_ts = (*it).first;
	    } 
	    else {
		break;
	    }
	}

	data_.erase(remove_if(data_.begin(), data_.end(), [&](DataPoint dp) { return dp.first < cursor_ts ;}), 
		    data_.end());
	return ss.str();
    }

    void FeatureSampler::update(time_stamp ts) 
    {
	for_each(featureInfoVec_.begin(), featureInfoVec_.end(), [&](FeatureInfo fi){(fi.pFeature_)->update(ts);});
	lastFeatureUpdateTs_ = ts;
    }

    void FeatureSampler::update(MarketOrderMessage order)   
    {
	for_each(featureInfoVec_.begin(), featureInfoVec_.end(), [&](FeatureInfo fi){(fi.pFeature_)->update(order);});
	lastFeatureUpdateTs_ = order.getTimestamp();
    }

    void FeatureSampler::update(LimitOrderMessage order)   
    {
	for_each(featureInfoVec_.begin(), featureInfoVec_.end(), [&](FeatureInfo fi){(fi.pFeature_)->update(order);});
	lastFeatureUpdateTs_ = order.getTimestamp();
    }

    void FeatureSampler::update(CanceledOrderMessage<LimitOrderMessage> cancel)   
    {
	for_each(featureInfoVec_.begin(), featureInfoVec_.end(), [&](FeatureInfo fi){(fi.pFeature_)->update(cancel);});
	lastFeatureUpdateTs_ = cancel.getTimestamp();
    }

    void FeatureSampler::update(CanceledOrderMessage<MarketOrderMessage> cancel)   
    {
	for_each(featureInfoVec_.begin(), featureInfoVec_.end(), [&](FeatureInfo fi){(fi.pFeature_)->update(cancel);});
	lastFeatureUpdateTs_ = cancel.getTimestamp();
    }

    void FeatureSampler::update(FilledOrderMessage fill)
    {
	for_each(featureInfoVec_.begin(), featureInfoVec_.end(), [&](FeatureInfo fi){(fi.pFeature_)->update(fill);});
	lastFeatureUpdateTs_ = fill.getTimestamp();
    }

    void FeatureSampler::update(AcknowledgedOrderMessage ack)
    {
	for_each(featureInfoVec_.begin(), featureInfoVec_.end(), [&](FeatureInfo fi){(fi.pFeature_)->update(ack);});
	lastFeatureUpdateTs_ = ack.getTimestamp();
    }

    void FeatureSampler::update(RejectedOrderMessage<LimitOrderMessage> reject)
    {
	for_each(featureInfoVec_.begin(), featureInfoVec_.end(), [&](FeatureInfo fi){(fi.pFeature_)->update(reject);});
	lastFeatureUpdateTs_ = reject.getTimestamp();
    }

    void FeatureSampler::update(RejectedOrderMessage<MarketOrderMessage> reject)
    {
	for_each(featureInfoVec_.begin(), featureInfoVec_.end(), [&](FeatureInfo fi){(fi.pFeature_)->update(reject);});
	lastFeatureUpdateTs_ = reject.getTimestamp();
    }

    void FeatureSampler::update(RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage> > reject)
    {
	for_each(featureInfoVec_.begin(), featureInfoVec_.end(), [&](FeatureInfo fi){(fi.pFeature_)->update(reject);});
	lastFeatureUpdateTs_ = reject.getTimestamp();
    }

    void FeatureSampler::update(RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage> > reject)   
    {
	for_each(featureInfoVec_.begin(), featureInfoVec_.end(), [&](FeatureInfo fi){(fi.pFeature_)->update(reject);});
	lastFeatureUpdateTs_ = reject.getTimestamp();
    }

    void FeatureSampler::update(BookUpdate<true> book)   
    {
	for_each(featureInfoVec_.begin(), featureInfoVec_.end(), [&](FeatureInfo fi){(fi.pFeature_)->update(book);});
	lastFeatureUpdateTs_ = book.getTimestamp();
    }

    void FeatureSampler::update(TradeUpdate<true> trade)   
    {
	for_each(featureInfoVec_.begin(), featureInfoVec_.end(), [&](FeatureInfo fi){(fi.pFeature_)->update(trade);});
	lastFeatureUpdateTs_ = trade.getTimestamp();
    }

    void FeatureSampler::update(BookUpdate<true> book, TradeUpdate<true> trade)   
    {
	for_each(featureInfoVec_.begin(), featureInfoVec_.end(), [&](FeatureInfo fi){(fi.pFeature_)->update(book, trade);});
	lastFeatureUpdateTs_ = std::max(book.getTimestamp(), trade.getTimestamp());
    }

    void FeatureSampler::update(BookUpdate<true> book, bool isFrontContractQ)   
    {
	for_each(featureInfoVec_.begin(), featureInfoVec_.end(), [&](FeatureInfo fi){(fi.pFeature_)->update(book, isFrontContractQ);});
	lastFeatureUpdateTs_ = book.getTimestamp();
    }

    void FeatureSampler::update(TradeUpdate<true> trade, bool isFrontContractQ)   
    {
	for_each(featureInfoVec_.begin(), featureInfoVec_.end(), [&](FeatureInfo fi){(fi.pFeature_)->update(trade, isFrontContractQ);});
	lastFeatureUpdateTs_ = trade.getTimestamp();
    }

    void FeatureSampler::update(BookUpdate<true> book, TradeUpdate<true> trade, bool isFrontContractQ)   
    {
	for_each(featureInfoVec_.begin(), featureInfoVec_.end(), [&](FeatureInfo fi){(fi.pFeature_)->update(book, trade, isFrontContractQ);});
	lastFeatureUpdateTs_ = std::max(book.getTimestamp(), trade.getTimestamp());
    }

}

// FEATURESAMPLER_CPP
