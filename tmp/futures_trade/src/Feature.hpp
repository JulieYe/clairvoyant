//
// File:        $ Feature.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/10/17 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef FEATURE_HPP
#define FEATURE_HPP

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

    // mid point
    class MidPoint : public Feature
    {
    public:
	MidPoint();
	void update(BookUpdate<true> book);
    };

    // bid-ask spread
    class BidAskSpread : public Feature 
    {
    public:
	BidAskSpread();
	void update(BookUpdate<true> book);
    };

    /* Implied Price of Spread Contract */
    template<int BUYSELLSIDE>
    class ImpliedSpreadPrice;

    template<>
    class ImpliedSpreadPrice<static_cast<int>(BuySellSide::mid)> : public Feature
    {
    private:
	ImpliedSpreadTopBookUpdate<true> iSpread_;

    public:
    	ImpliedSpreadPrice() 
	    : Feature("ImpliedSpreadPrice<Mid>"), 
	      iSpread_() 
    	{}

    	void update(BookUpdate<true> book, bool frontQ)
    	{
	    iSpread_.update(book, frontQ);
	    if (iSpread_.syncQ()) {
		iSpread_.imply();
		if (iSpread_.getMidPx(&value_)) {
		    isGood_ = true;
		} else {
		    isGood_ = false;
		}
	    } else {
		isGood_ = false;
	    }
    	}
    };

    template<>
    class ImpliedSpreadPrice<static_cast<int>(BuySellSide::buy)> : public Feature
    {
    private:
	ImpliedSpreadTopBookUpdate<true> iSpread_;

    public:
    	ImpliedSpreadPrice() 
	    : Feature("ImpliedSpreadPrice<Bid>"), 
	      iSpread_() 
    	{}

    	void update(BookUpdate<true> book, bool frontQ)
    	{
	    iSpread_.update(book, frontQ);
	    if (iSpread_.syncQ()) {
		iSpread_.imply();
		if (iSpread_.getBidQty() > 0) {
		    isGood_ = true;
		    value_ = iSpread_.getBidPx().getPx();
		} else {
		    isGood_ = false;
		}
	    } else {
		isGood_ = false;
	    }
    	}
    };

    template<>
    class ImpliedSpreadPrice<static_cast<int>(BuySellSide::sell)> : public Feature
    {
    private:
	ImpliedSpreadTopBookUpdate<true> iSpread_;

    public:
    	ImpliedSpreadPrice() 
	    : Feature("ImpliedSpreadPrice<Ask>"), 
	      iSpread_() 
    	{}

    	void update(BookUpdate<true> book, bool frontQ)
    	{
	    iSpread_.update(book, frontQ);
	    if (iSpread_.syncQ()) {
		iSpread_.imply();
		if (iSpread_.getAskQty() > 0) {
		    isGood_ = true;
		    value_ = iSpread_.getAskPx().getPx();
		} else {
		    isGood_ = false;
		}
	    } else {
		isGood_ = false;
	    }
    	}
    };

    /* Smooth Implied Price of Spread Contract */
    template<int BUYSELLSIDE, int n, int numSec>
    class ImpliedSpreadPriceSmooth;
    
    template<int n, int numSec>
    class ImpliedSpreadPriceSmooth<static_cast<int>(BuySellSide::mid), n, numSec> : public Feature
    {
    private:
    	string createFeatureName()
    	{
    	    stringstream ss;
    	    ss << "ImpliedSpreadPriceSmooth<Mid:" << numSec << " Second>";
    	    return ss.str();
    	}
	
	ImpliedSpreadPrice<static_cast<int>(BuySellSide::mid)> iSpreadPx_;
	QtMA maPx_;
	
    public:
    	ImpliedSpreadPriceSmooth() : Feature(createFeatureName()), iSpreadPx_(), maPx_(QtMA(n, numSec, 0, 0, QtEMA::IntrpType::Linear))
	    {}
		
    	void update(BookUpdate<true> book, bool frontQ)
    	{
	    iSpreadPx_.update(book, frontQ);
	    if (iSpreadPx_.isGood()) {
		isGood_ = true;
		maPx_.update(static_cast<double>(book.getTimestamp().time_since_epoch().count())/1e9, iSpreadPx_.getValue());
		value_ = maPx_();
	    } else {
		isGood_ = false;
	    }
    	}
    };

    template<int n, int numSec>
    class ImpliedSpreadPriceSmooth<static_cast<int>(BuySellSide::buy), n, numSec> : public Feature
    {
    private:
    	string createFeatureName()
    	{
    	    stringstream ss;
    	    ss << "ImpliedSpreadPriceSmooth<Bid:" << numSec << " Second>";
    	    return ss.str();
    	}

	ImpliedSpreadPrice<static_cast<int>(BuySellSide::buy)> iSpreadPx_;
	QtMA maPx_;

    public:
    	ImpliedSpreadPriceSmooth() : Feature(createFeatureName()), iSpreadPx_(), maPx_(QtMA(n, numSec, 0, 0, QtEMA::IntrpType::Linear))
	    {}

    	void update(BookUpdate<true> book, bool frontQ)
    	{
	    iSpreadPx_.update(book, frontQ);
	    if (iSpreadPx_.isGood()) {
		isGood_ = true;
		maPx_.update(static_cast<double>(book.getTimestamp().time_since_epoch().count())/1e9, iSpreadPx_.getValue());
		value_ = maPx_();
	    } else {
		isGood_ = false;
	    }
    	}
    };

    template<int n, int numSec>
    class ImpliedSpreadPriceSmooth<static_cast<int>(BuySellSide::sell), n, numSec> : public Feature
    {
    private:
    	string createFeatureName()
    	{
    	    stringstream ss;
    	    ss << "ImpliedSpreadPriceSmooth<Ask:" << numSec << " Second>";
    	    return ss.str();
    	}

	ImpliedSpreadPrice<static_cast<int>(BuySellSide::sell)> iSpreadPx_;
	QtMA maPx_;

    public:
    	ImpliedSpreadPriceSmooth() : Feature(createFeatureName()), iSpreadPx_(), maPx_(QtMA(n, numSec, 0, 0, QtEMA::IntrpType::Linear))
	    {}
	
    	void update(BookUpdate<true> book, bool frontQ)
    	{
	    iSpreadPx_.update(book, frontQ);
	    if (iSpreadPx_.isGood()) {
		isGood_ = true;
		maPx_.update(static_cast<double>(book.getTimestamp().time_since_epoch().count())/1e9, iSpreadPx_.getValue());
		value_ = maPx_();
	    } else {
		isGood_ = false;
	    }
    	}
    };

    /* Bid-Ask Spread of Spread Contract */
    template<int FRONTBACKSIDE>
    class ImpliedSpreadBidAskSpread;

    template<>
    class ImpliedSpreadBidAskSpread<static_cast<int>(FrontBackSide::mid)> : public Feature
    {
    private:
	ImpliedSpreadTopBookUpdate<true> iSpread_;

    public:
    	ImpliedSpreadBidAskSpread() 
	    : Feature("ImpliedSpreadBidAskSpread<Mid>"), 
	      iSpread_() 
    	{}

    	void update(BookUpdate<true> book, bool frontQ)
    	{
	    iSpread_.update(book, frontQ);
	    if (iSpread_.syncQ()) {
		iSpread_.imply();
		if (iSpread_.getBidAskSpread(&value_)) {
		    isGood_ = true;
		} else {
		    isGood_ = false;
		}
	    } else {
		isGood_ = false;
	    }
    	}
    };

    template<>
    class ImpliedSpreadBidAskSpread<static_cast<int>(FrontBackSide::front)> : public Feature
    {
    private:
	ImpliedSpreadTopBookUpdate<true> iSpread_;

    public:
    	ImpliedSpreadBidAskSpread() 
	    : Feature("ImpliedSpreadBidAskSpread<Front>"), 
	      iSpread_() 
    	{}

    	void update(BookUpdate<true> book, bool frontQ)
    	{
	    iSpread_.update(book, frontQ);
	    if (iSpread_.syncQ()) {
		iSpread_.imply();
		if (iSpread_.getBidAskSpread(&value_, true) > 0) {
		    isGood_ = true;
		} else {
		    isGood_ = false;
		}
	    } else {
		isGood_ = false;
	    }
    	}
    };

    template<>
    class ImpliedSpreadBidAskSpread<static_cast<int>(FrontBackSide::back)> : public Feature
    {
    private:
	ImpliedSpreadTopBookUpdate<true> iSpread_;

    public:
    	ImpliedSpreadBidAskSpread() 
	    : Feature("ImpliedSpreadBidAskSpread<Back>"), 
	      iSpread_() 
    	{}

    	void update(BookUpdate<true> book, bool frontQ)
    	{
	    iSpread_.update(book, frontQ);
	    if (iSpread_.syncQ()) {
		iSpread_.imply();
		if (iSpread_.getBidAskSpread(&value_, false) > 0) {
		    isGood_ = true;
		} else {
		    isGood_ = false;
		}
	    } else {
		isGood_ = false;
	    }
    	}
    };

    // spread of last trade price
    class LastTradeSpread : public Feature
    {
    private:
	double frontContract_ = 0.0;
	double backContract_ = 0.0;

    public:
	LastTradeSpread() : Feature("LastTradeSpread")
	{}

	void update(TradeUpdate<true> trade, bool isFrontContractQ)
	{
	    if (isFrontContractQ) {
		frontContract_ = trade.getPx().getPx();
	    } else {
		backContract_ = trade.getPx().getPx();
	    }
	    if (frontContract_ > TOL && backContract_ > TOL) {
		isGood_ = true;
		value_ = backContract_ - frontContract_;
	    } else {
		isGood_ = false;
	    }
	}
    };

    // book pressure
    class BookPressure : public Feature
    {
    public:
	BookPressure();
	void update(BookUpdate<true> book);
    };

    // nonlinear book pressure
    template<int NONLINEARITY_NUM, int NONLINEARITY_DENOM>
    class BookPressureNL : public Feature
    {
    private:
	string createFeatureName()
	{
	    stringstream ss;
	    ss << "BookPressureNL<" << NONLINEARITY_NUM
	       << "/" << NONLINEARITY_DENOM << ">";
	    return ss.str();
	}
	static constexpr double power_ = NONLINEARITY_NUM * 1.0 / NONLINEARITY_DENOM;

    public:
	BookPressureNL() : Feature(createFeatureName(), 0.0, 0.0)
	{}

	void update(BookUpdate<true> book)
	{
	    if (book.getBidQty()[0] > 0 && book.getAskQty()[0] > 0) {
		isGood_ = true;
		value_ = static_cast<double>(pow(book.getBidQty()[0], power_)) / static_cast<double>(pow(book.getBidQty()[0], power_) + pow(book.getAskQty()[0], power_)) - 0.5;
	    } else {
		isGood_ = false;
	    }
	}
    };

    // log book pressure
    class BookPressureLog : public Feature
    {
    public:
	BookPressureLog();
	void update(BookUpdate<true> book);
    };
    
    // short term trend
    template<int numMillisec>
    class ShortTermReturn : public Feature
    {
    private:
	string createFeatureName()
	{
	    stringstream ss;
	    ss << "ShortTermReturn<" << numMillisec << " Millisec>";
	    return ss.str();
	}
	deque<double> queueMid_;
	deque<time_stamp> queueTs_;

    public:
	ShortTermReturn() :
	    Feature(createFeatureName()),
	    queueMid_(deque<double>()),
	    queueTs_(deque<time_stamp>())
	{}

	void update(BookUpdate<true> book)
	{
	    double midPx;
	    time_stamp ts, ts_old;
	    if (book.getMidPx(&midPx)) {
		ts = book.getTimestamp();
		ts_old = ts - duration_nano(numMillisec * 1000000ULL);
		queueMid_.push_back(midPx);
		queueTs_.push_back(ts);
		while(!queueMid_.empty() && queueTs_.front() < ts_old) {
		    queueMid_.pop_front();
		    queueTs_.pop_front();
		}
	    } 
	    if (!queueMid_.empty() && queueMid_.front() > TOL) {
		isGood_ = true;
		value_ = (queueMid_.back()  - queueMid_.front()) / queueMid_.front();
	    } else {
		isGood_ = false;
	    }
	}
	
    };
    
    // Trade Deviation
    class TradeDeviation : public Feature
    {
    public:
	TradeDeviation();
	void update(BookUpdate<true> book, TradeUpdate<true> trade);
    };

    /* Trade Deviation Moving Average */
    template<int n>
    class TradeDeviationMA : public Feature
    {
    private:
	string createFeatureName()
	{
	    stringstream ss;
	    ss << "TradeDeviationMA<" << n << ">";
	    return ss.str();
	}

	TradeDeviation lastTradeDeviation_;
	double weight_;

    public:
	TradeDeviationMA() : 
	    Feature(createFeatureName()),
	    lastTradeDeviation_(),
	    weight_(1.0 / n)
	{}

	virtual void update(BookUpdate<true> book, TradeUpdate<true> trade)
	{
	    lastTradeDeviation_.update(book, trade);
	    if (lastTradeDeviation_.isGood()) {
		isGood_ = true;
		value_ += (lastTradeDeviation_.getValue() - value_) * weight_;
	    }
	}

    };
    
    /* VolumeTrend */
    template<int shortN, int longN>
    class VolumeTrend : public Feature
    {
    private:
	double shortVolume_;
	double longVolume_;
	string createFeatureName()
	{
	    stringstream ss;
	    ss << "VolumeTrend<" << shortN << "-" << longN << ">";
	    return ss.str();
	}

    public:
	VolumeTrend() :
	    Feature(createFeatureName()),
	    shortVolume_(0.0),
	    longVolume_(0.0)
	{ }

	void update(TradeUpdate<true> trade)
	{
	    isGood_ = true;
	    shortVolume_ += (trade.getQty() - shortVolume_) * 1.0 / shortN;
	    longVolume_ += (trade.getQty() - longVolume_) * 1.0 / longN;
	    value_ = shortVolume_ - longVolume_;
	}
	
    };
    
    /* SignedVolume */
    // If the current mid px is higher than the previous mid px, +volume. 
    // If lower, -volume. 
    // Otherwise 0.0 (badValue_), including time mismatch.
    template<int numMillisec>
    class SignedVolume : public Feature
    {
    private:
	string createFeatureName()
	{
	    stringstream ss;
	    ss << "SignedVolume<" << numMillisec << " Millisec>";
	    return ss.str();
	}

	deque<time_stamp> queueTs_;
	deque<int> queueVolume_;
	double prevMidPx_;
	int prevVolume_;

	duration_nano dur_ = duration_nano(numMillisec * 1000000ULL);
      
    public:
	SignedVolume() : 
	    Feature(createFeatureName()),
	    prevMidPx_(0.0),
	    prevVolume_(0)
	{}

	virtual void update(BookUpdate<true> book, TradeUpdate<true> trade)
	{
	    // if cumulative traded volume so far on a certain day is zero, do nothing other than set flag isGood_
	    if (trade.getQty() == 0) {
		isGood_ = false;
	    } else {
		isGood_ = true;
		// push new
		double midPx;
		if (book.getMidPx(&midPx)) {
		    if (prevMidPx_ > TOL) {
			if (midPx > prevMidPx_ + TOL) {
			    queueTs_.push_back(trade.getTimestamp());
			    queueVolume_.push_back(trade.getQty() - prevVolume_);
			    value_ += (trade.getQty() - prevVolume_);
			}
			else if (midPx < prevMidPx_ - TOL) {
			    queueTs_.push_back(trade.getTimestamp());
			    queueVolume_.push_back(prevVolume_ - trade.getQty());
			    value_ += (prevVolume_ - trade.getQty());
			}
		    }
		    prevMidPx_ = midPx;		
		}
		prevVolume_ = trade.getQty();
		
		// remove old
		time_stamp ts_old = trade.getTimestamp() - dur_;
		while (!queueTs_.empty() && queueTs_.front() < ts_old) {
		    value_ -= queueVolume_.front();
		    queueTs_.pop_front();
		    queueVolume_.pop_front();
		}
		
		// check validatity
		if (queueTs_.empty()) {
		    value_ = 0;
		}
	    }
	}
    };

    /* SignedVolumeTradeVsMid */
    // If last trade px is higher than mid px, +volume. 
    // If lower, -volume. 
    // Otherwise 0.0 (badValue_), including time mismatch.
    template<int numMillisec>
    class SignedVolumeTradeVsMid : public Feature
    {
    private:
	string createFeatureName()
	{
	    stringstream ss;
	    ss << "SignedVolumeTradeVsMid<" << numMillisec << " Millisec>";
	    return ss.str();
	}

	deque<time_stamp> queueTs_;
	deque<int> queueVolume_;
	int prevVolume_;

	duration_nano dur_ = duration_nano(numMillisec * 1000000ULL);
      
    public:
	SignedVolumeTradeVsMid() : 
	    Feature(createFeatureName()),
	    prevVolume_(0)
	{}

	virtual void update(BookUpdate<true> book, TradeUpdate<true> trade)
	{
	    // if cumulative traded volume so far on a certain day is zero, do nothing other than set flag isGood_
	    if (trade.getQty() == 0) {
		isGood_ = false;
	    } else {
		isGood_ = true;
		// push new
		double midPx;
		if (book.getMidPx(&midPx)) {
		    if (trade.getPx().getPx() > midPx + TOL) {
			queueTs_.push_back(trade.getTimestamp());
			queueVolume_.push_back(trade.getQty() - prevVolume_);
			value_ += (trade.getQty() - prevVolume_);
		    }
		    else if (trade.getPx().getPx() < midPx - TOL) {
			queueTs_.push_back(trade.getTimestamp());
			queueVolume_.push_back(prevVolume_ - trade.getQty());
			value_ += (prevVolume_ - trade.getQty());
		    }
		}
		prevVolume_ = trade.getQty();
		
		// remove old
		time_stamp ts_old = trade.getTimestamp() - dur_;
		while (!queueTs_.empty() && queueTs_.front() < ts_old) {
		    value_ -= queueVolume_.front();
		    queueTs_.pop_front();
		    queueVolume_.pop_front();
		}
		
		// check validatity
		if (queueTs_.empty()) {
		    value_ = 0;
		}
	    }
	}
    };

    /* VWAP */
    // Assume volume all traded at lastPx.
    template<int numMillisec>
    class Vwap : public Feature
    {
    private:
	string createFeatureName()
	{
	    stringstream ss;
	    ss << "Vwap<" << numMillisec << " Millisec>";
	    return ss.str();
	}

	deque<time_stamp> queueTs_;
	deque<int> queueVolume_;
	deque<double> queuePx_;
	int prevVolume_;
	int vwapQty_;

	duration_nano dur_ = duration_nano(numMillisec * 1000000ULL);

    public:
	Vwap() : 
	    Feature(createFeatureName()),
	    prevVolume_(0),
	    vwapQty_(0)
	{}

	virtual void update(TradeUpdate<true> trade)
	{
	    // if cumulative traded volume so far on a certain day is zero, do nothing other than set flag isGood_
	    if (trade.getQty() == 0) {
		isGood_ = false;
	    } 
	    else {
	    	// push new trade
	    	if (trade.getQty() > prevVolume_) {
	    	    queueVolume_.push_back(trade.getQty() - prevVolume_);
	    	    queuePx_.push_back(trade.getPx().getPx());
		    queueTs_.push_back(trade.getTimestamp());
	    	    value_ = (value_ * vwapQty_ + queuePx_.back() * queueVolume_.back()) / (vwapQty_ + queueVolume_.back());
	    	    vwapQty_ += queueVolume_.back();
	    	}
	     	prevVolume_ = trade.getQty();
	    	// remove old
	    	time_stamp ts_old = trade.getTimestamp() - dur_;
	    	while (!queueTs_.empty() && queueTs_.front() < ts_old) {
	    	    value_ = (value_ * vwapQty_ - queuePx_.front() * queueVolume_.front()) / (vwapQty_ - queueVolume_.front());
	    	    vwapQty_ -= queueVolume_.front();
	    	    queueTs_.pop_front();
	    	    queueVolume_.pop_front();
	    	    queuePx_.pop_front();
	    	}
	    	// check validatity
	    	if (queueTs_.empty()) {
	    	    isGood_ = false;
	    	    value_ = 0.0;
	    	} else {
	    	    isGood_ = true;
		}
	    }
	}
    };
    
    /* VwapMidReturn */
    // Assume volume all traded at lastPx.
    template<int numMillisec>
    class VwapMidReturn : public Feature
    {
    private:
	string createFeatureName()
	{
	    stringstream ss;
	    ss << "VwapMidReturn<" << numMillisec << " Millisec>";
	    return ss.str();
	}

	MidPoint midPoint_;
	Vwap<numMillisec> vwap_;
	double prevMidPx_;
	
	duration_nano dur_ = duration_nano(numMillisec * 1000000ULL);

    public:
	VwapMidReturn() : 
	    Feature(createFeatureName()),
	    midPoint_(),
	    vwap_(),
	    prevMidPx_(0.0)
	{}

	virtual void update(BookUpdate<true> book, TradeUpdate<true> trade)
	{
	    midPoint_.update(book);
	    vwap_.update(trade);

	    // if both vwap and midpx are both nonzero
	    if (midPoint_.isGood() && vwap_.isGood() && vwap_.getValue() > TOL) {
		isGood_ = true;
		value_ = (midPoint_.getValue() - vwap_.getValue()) / vwap_.getValue();
	    }
	    // else use stale mid price
	    else if (vwap_.isGood() && vwap_.getValue() > TOL && prevMidPx_ > TOL) {
		isGood_ = false;
		badValue_ = (prevMidPx_ - vwap_.getValue()) / vwap_.getValue();
	    }
	    // data no good, use badValue_
	    else {
		isGood_ = false;
		badValue_ = 0.0;
	    }
	    // update prevMidPx_
	    if (midPoint_.getValue() > TOL) {
		prevMidPx_ = midPoint_.getValue();
	    }
	}
    };
    
    /* BookSizeChg */
    // compute top book bid/ask size change over certain time horizon
    template<int NUMMILLISEC, int BUYSELLSIDE>
    class BookSizeChg;
    
    template<int numMillisec>
    class BookSizeChg<numMillisec, static_cast<int>(BuySellSide::buy)> : public Feature
    {
    private:
	string createFeatureName()
	{
	    stringstream ss;
	    ss << "BookSizeChg<" << numMillisec << " Millisec:Bid>";
	    return ss.str();
	}
	
	typedef tuple<time_stamp, int> time_qty_tuple;

	vector<time_qty_tuple> time_qty_vector_;
	unsigned long long dur_ = numMillisec * 1000000ULL;

    public:
	BookSizeChg() : 
	    Feature(createFeatureName())
	{}

	virtual void update(BookUpdate<true> book)
	{
	    // push new
	    time_qty_vector_.emplace_back(make_tuple(book.getTimestamp(), (book.getBidQty())[0]));
	    
	    // remove old
	    time_stamp lastTimestamp(book.getTimestamp());
	    time_qty_vector_.erase(remove_if(time_qty_vector_.begin(),
					     time_qty_vector_.end(),
					     [&](time_qty_tuple &time_qty) { return (lastTimestamp - get<0>(time_qty)).count() > dur_; }),
				   time_qty_vector_.end());
	    
	    // compute bid/ask size change
	    if (!time_qty_vector_.empty()) {
		isGood_ = true;
		value_ = get<1>(time_qty_vector_.back()) - get<1>(time_qty_vector_.front());
	    } else {
		isGood_ = false;
	    }
	}
    };
    
    template<int numMillisec>
    class BookSizeChg<numMillisec, static_cast<int>(BuySellSide::sell)> : public Feature
    {
    private:
	string createFeatureName()
	{
	    stringstream ss;
	    ss << "BookSizeChg<" << numMillisec << " Millisec:Ask>";
	    return ss.str();
	}
	
	typedef tuple<time_stamp, int> time_qty_tuple;
	
	vector<time_qty_tuple> time_qty_vector_;
	unsigned long long dur_ = numMillisec * 1000000ULL;
	
    public:
	BookSizeChg() : 
	    Feature(createFeatureName())
	    {}
	
	virtual void update(BookUpdate<true> book)
	{
	    // push new
	    time_qty_vector_.emplace_back(make_tuple(book.getTimestamp(), (book.getAskQty())[0]));
	    
	    // remove old
	    time_stamp lastTimestamp(book.getTimestamp());
	    time_qty_vector_.erase(remove_if(time_qty_vector_.begin(),
					     time_qty_vector_.end(),
					     [&](time_qty_tuple &time_qty) { return (lastTimestamp - get<0>(time_qty)).count() > dur_; }),
				   time_qty_vector_.end());
	    
	    // compute bid/ask size change
	    if (!time_qty_vector_.empty()) {
		isGood_ = true;
		value_ = get<1>(time_qty_vector_.back()) - get<1>(time_qty_vector_.front());
	    } else {
		isGood_ = false;
	    }
        }
    };

    /* BookSizeChg2 */
    // compute top book bid/ask size change over certain time horizon
    template<int NUMMILLISEC, int BUYSELLSIDE>
    class BookSizeChg2;
    
    template<int numMillisec>
    class BookSizeChg2<numMillisec, static_cast<int>(BuySellSide::buy)> : public Feature
    {
    private:
	string createFeatureName()
	{
	    stringstream ss;
	    ss << "BookSizeChg2<" << numMillisec << " Millisec:Bid>";
	    return ss.str();
	}
	
	typedef tuple<time_stamp, int, Px> time_qty_px_tuple;

	vector<time_qty_px_tuple> time_qty_px_vector_;
	unsigned long long dur_ = numMillisec * 1000000ULL;

    public:
	BookSizeChg2() : 
	    Feature(createFeatureName())
	{}

	virtual void update(BookUpdate<true> book)
	{
	    time_qty_px_tuple new_tuple = make_tuple(book.getTimestamp(), (book.getBidQty())[0], (book.getBidPx()[0]));

	    if (time_qty_px_vector_.size() == 0) {
		isGood_ = false;
		time_qty_px_vector_.emplace_back(new_tuple);
	    } else {
		isGood_ = true;
		if (get<2>(new_tuple) == get<2>(time_qty_px_vector_.back()) ) {
		    value_ += get<1>(new_tuple) - get<1>(time_qty_px_vector_.back());
		} else if (get<2>(new_tuple) < get<2>(time_qty_px_vector_.back()) ) {
		    value_ -= get<1>(time_qty_px_vector_.back());
		} else {
		    value_ += get<1>(new_tuple);
		}
		time_qty_px_vector_.emplace_back(new_tuple);
		auto it = time_qty_px_vector_.begin();
		while ((book.getTimestamp() - get<0>((*it))).count() > dur_) {
		    if(get<2>((*(it+1))) == get<2>((*it))) {
			value_ -= get<1>((*(it+1))) - get<1>((*it));
		    } else if(get<2>(*(it+1)) < get<2>((*it))) {
			value_ += get<1>((*it));
		    } else {
			value_ -= get<1>((*(it+1)));
		    }
		    it++;
		}
	    }

	    // remove old
	    time_stamp lastTimestamp(book.getTimestamp());
	    time_qty_px_vector_.erase(remove_if(time_qty_px_vector_.begin(),
						time_qty_px_vector_.end(),
						[&](time_qty_px_tuple &time_qty) { return (lastTimestamp - get<0>(time_qty)).count() > dur_; }),
				      time_qty_px_vector_.end());
	}
    };

    template<int numMillisec>
    class BookSizeChg2<numMillisec, static_cast<int>(BuySellSide::sell)> : public Feature
    {
    private:
	string createFeatureName()
	{
	    stringstream ss;
	    ss << "BookSizeChg2<" << numMillisec << " Millisec:Ask>";
	    return ss.str();
	}
	
	typedef tuple<time_stamp, int, Px> time_qty_px_tuple;

	vector<time_qty_px_tuple> time_qty_px_vector_;
	unsigned long long dur_ = numMillisec * 1000000ULL;

    public:
	BookSizeChg2() : 
	    Feature(createFeatureName())
	{}

	virtual void update(BookUpdate<true> book)
	{
	    time_qty_px_tuple new_tuple = make_tuple(book.getTimestamp(), (book.getAskQty())[0], (book.getAskPx()[0]));

	    if (time_qty_px_vector_.size() == 0) {
		isGood_ = false;
		time_qty_px_vector_.emplace_back(new_tuple);
	    } else {
		isGood_ = true;
		if (get<2>(new_tuple) == get<2>(time_qty_px_vector_.back()) ) {
		    value_ += get<1>(new_tuple) - get<1>(time_qty_px_vector_.back());
		} else if (get<2>(new_tuple) > get<2>(time_qty_px_vector_.back()) ) {
		    value_ -= get<1>(time_qty_px_vector_.back());
		} else {
		    value_ += get<1>(new_tuple);
		}
		time_qty_px_vector_.emplace_back(new_tuple);
		auto it = time_qty_px_vector_.begin();
		while ((book.getTimestamp() - get<0>((*it))).count()  > dur_) {
		    if(get<2>((*(it+1))) == get<2>((*it))) {
			value_ -= get<1>((*(it+1))) - get<1>((*it));
		    } else if(get<2>(*(it+1)) > get<2>((*it))) {
			value_ += get<1>((*it));
		    } else {
			value_ -= get<1>((*(it+1)));
		    }
		    it++;
		}
	    }

	    // remove old
	    time_stamp lastTimestamp(book.getTimestamp());
	    time_qty_px_vector_.erase(remove_if(time_qty_px_vector_.begin(),
						time_qty_px_vector_.end(),
						[&](time_qty_px_tuple &time_qty) { return (lastTimestamp - get<0>(time_qty)).count() > dur_; }),
				      time_qty_px_vector_.end());
	}
    };

    /* Volatility */
    template<int numSec, int numPeriodsPerYear>
    class Volatility : public Feature
    {
    private:
	string createFeatureName()
	{
	    stringstream ss;
	    ss << "Volatility<" << numSec << " Second>";
	    return ss.str();
	}
	deque<double> queueMid_;
	deque<time_stamp> queueTs_;
	double variance_;
	
	duration_nano dur_ = duration_nano(numSec * 1000000000ULL);
	
    public:
	Volatility() : 
	    Feature(createFeatureName()),
	    variance_(0.0)
	{}
	
	virtual void update(BookUpdate<true> book)
	{
	    double midPx;
	    // compute volatility if mid
	    if (book.getMidPx(&midPx) && midPx > TOL) {
		// push new
		variance_ += (!queueTs_.empty() ? pow((midPx - queueMid_.back()) / queueMid_.back(), 2) : 0.0);
		queueTs_.push_back(book.getTimestamp());
		queueMid_.push_back(midPx);
		// remove old
		time_stamp ts_old = book.getTimestamp() - dur_;
		while (!queueTs_.empty() && queueTs_.front() < ts_old) {
		    double mid_old = queueMid_.front();
		    queueTs_.pop_front();
		    queueMid_.pop_front();
		    variance_ -= (!queueTs_.empty() ? pow((queueMid_.front() - mid_old) / mid_old, 2) : 0.0);
		    variance_ = max(variance_, 0.0);
		}
		if (queueTs_.size() > 1) {
		    isGood_ = true;
		    value_ = sqrt(variance_ / (queueTs_.size() - 1) * numPeriodsPerYear);
		}
		else {
		    isGood_ = false;
		    variance_ = 0.0;
		    badValue_ = 0.0;
		}
	    }
	    else {
		isGood_ = false;
		// remove old
		time_stamp ts_old = book.getTimestamp() - dur_;
		while (!queueTs_.empty() && queueTs_.front() < ts_old) {
		    double mid_old = queueMid_.front();
		    queueTs_.pop_front();
		    queueMid_.pop_front();
		    variance_ -= (!queueTs_.empty() ? pow((queueMid_.front() - mid_old) / mid_old, 2) : 0.0);
		    variance_ = max(variance_, 0.0);
		}
		if (queueTs_.size() > 1) {
		    badValue_ = sqrt(variance_ / (queueTs_.size() - 1) * numPeriodsPerYear);
		} else {
		    variance_ = 0.0;
		    badValue_ = 0.0;
		}
	    }
	}
    };

    /* SignedVolumeWithBookSizeChg */
    // Signed volume with the help of book size change information
    template<int numMillisec>
    class SignedVolumeWithBookSizeChg : public Feature
    {
    private:
	string createFeatureName()
	{
	    stringstream ss;
	    ss << "SignedVolumeWithBookSizeChg<" << numMillisec << " Millisec>";
	    return ss.str();
	}

	deque<time_stamp> queueTs_;
	deque<int> queueVolume_;
	int prevVolume_;
	BookSizeChg2<numMillisec, static_cast<int>(BuySellSide::buy)> bidSizeChg_;
	BookSizeChg2<numMillisec, static_cast<int>(BuySellSide::sell)> askSizeChg_;

	duration_nano dur_ = duration_nano(numMillisec * 1000000ULL);
      
    public:
	SignedVolumeWithBookSizeChg() : 
	    Feature(createFeatureName()),
	    prevVolume_(0),
	    bidSizeChg_(), askSizeChg_()
	{
	    isGood_ = true;
	}

	virtual void update(BookUpdate<true> book, TradeUpdate<true> trade)
	{
	    // update book size change
	    bidSizeChg_.update(book);
	    askSizeChg_.update(book);

	    int tradeQty = trade.getQty() - prevVolume_;
	    int tradeOnBid, tradeOnAsk;

	    // magic formula applied here
	    // Xb - Qsell = BidSizeChg
	    // Xa - Qbuy = AskSizeChg
	    // Qbuy + Qsell = TradeQty
	    // Xb / Xa = Qbuy / Qsell
	    // 1. Unsolvable linear system
	    if (std::abs(2 * tradeQty + bidSizeChg_.getValue() + askSizeChg_.getValue()) < 0.5) {
		tradeOnBid = 0;
		tradeOnAsk = 0;
	    } else if ((tradeQty + askSizeChg_.getValue()) * (2 * tradeQty + bidSizeChg_.getValue() + askSizeChg_.getValue()) > 0) {
		//tradeOnBid = min(static_cast<int>(floor(static_cast<double>(tradeQty + askSizeChg_.getValue()) / static_cast<double>(2 * tradeQty + bidSizeChg_.getValue() + askSizeChg_.getValue()) * static_cast<double>(tradeQty) + 1.0/2)), tradeQty);
		const int bb = (2 * tradeQty + static_cast<int>(bidSizeChg_.getValue()) + static_cast<int>(askSizeChg_.getValue()));
		const int aa = (tradeQty + static_cast<int>(askSizeChg_.getValue()));
		const int cc = tradeQty;
		tradeOnBid = min((2*aa*cc + bb) / (2 * bb), tradeQty);
		//tradeOnBid = min(static_cast<int>(floor((tradeQty + askSizeChg_.getValue()) / (2 * tradeQty + bidSizeChg_.getValue() + askSizeChg_.getValue()) * (tradeQty) + 1.0/2)), tradeQty);
		//tradeOnBid = min(static_cast<int>((static_cast<double>(tradeQty + askSizeChg_.getValue()) / static_cast<double>(2 * tradeQty + bidSizeChg_.getValue() + askSizeChg_.getValue()) * static_cast<double>(tradeQty) + 0.5000001)), tradeQty);
		tradeOnAsk = tradeQty - tradeOnBid;
	    } else {
		tradeOnBid = 0;
		tradeOnAsk = tradeQty;
	    }

	    queueTs_.push_back(trade.getTimestamp());
	    queueVolume_.push_back(tradeOnAsk-tradeOnBid);
	    value_ += (tradeOnAsk-tradeOnBid);

	    // update prevVolume_
	    prevVolume_ = trade.getQty();
  		
	    // remove old
	    time_stamp ts_old = trade.getTimestamp() - dur_;
	    while (!queueTs_.empty() && queueTs_.front() < ts_old) {
		value_ -= queueVolume_.front();
		queueTs_.pop_front();
		queueVolume_.pop_front();
	    }
		
	    // check validatity
	    if (queueTs_.empty()) {
		value_ = 0;
	    }
	}
    };

    /* High-Frequency Volatility Estimator */
    template<int numSec, int numSecPred>
    class HighFrequencyVolatility : public Feature
    {
    private:
    	string createFeatureName()
    	{
    	    stringstream ss;
    	    ss << "HighFrequencyVolatility<" << numSec << ":" << numSecPred << " Second>";
    	    return ss.str();
    	}
	
    	QtMVar * ma_px_var_;
	
    public:
    	HighFrequencyVolatility()
    	    : Feature(createFeatureName()),
    	      ma_px_var_(new QtMVar(4, 2, numSec, 0, 0, QtEMA::IntrpType::Next))
    	{}

    	~HighFrequencyVolatility()
    	{
    	    delete ma_px_var_;
    	}
	
    	virtual void update(BookUpdate<true> book)
    	{
    	    double midPx;
    	    // compute volatility if mid
    	    if (book.getMidPx(&midPx)) {
    		isGood_ = true;
		ma_px_var_->update(static_cast<double>(book.getTimestamp().time_since_epoch().count())/1e9, midPx);
    		value_ = ma_px_var_->stdev() * sqrt(static_cast<double>(numSecPred)/static_cast<double>(numSec));
    	    } else {
    		isGood_ = false;
    		badValue_ = value_;
    	    }
    	}
    };

    /* High-Frequency Volatility Estimator for Spread Contract */
    template<int numSec, int numSecPred, int BUYSELLSIDE>
    class HighFrequencyVolatilitySpread;

    template<int numSec, int numSecPred>
    class HighFrequencyVolatilitySpread<numSec, numSecPred, static_cast<int>(BuySellSide::mid)> : public Feature 
    {
    private:
    	string createFeatureName() 
    	{
    	    stringstream ss;
    	    ss << "HighFrequencyVolatilitySpread<" << numSec << ":" << numSecPred << " Second:Mid>";
    	    return ss.str();
    	}
	
	ImpliedSpreadPrice<static_cast<int>(BuySellSide::mid)> iSpreadPrice_;
    	QtMVar * ma_px_var_;
	
    public:
    	HighFrequencyVolatilitySpread() 
    	    : Feature(createFeatureName()),
    	      ma_px_var_(new QtMVar(4, 2, numSec, 0, 0, QtEMA::IntrpType::Next))
    	{}

    	~HighFrequencyVolatilitySpread()
    	{
    	    delete ma_px_var_;
    	}
	
    	virtual void update(BookUpdate<true> book, bool frontQ)
    	{
	    iSpreadPrice_.update(book, frontQ);
    	    // compute volatility if mid
    	    if (iSpreadPrice_.isGood()) {
    		isGood_ = true;
		ma_px_var_->update(static_cast<double>(book.getTimestamp().time_since_epoch().count())/1e9, iSpreadPrice_.getValue());
    		value_ = ma_px_var_->stdev() * sqrt(static_cast<double>(numSecPred)/static_cast<double>(numSec));
    	    } else {
    		isGood_ = false;
    		badValue_ = value_;
    	    }
    	}
    };

    template<int numSec, int numSecPred>
    class HighFrequencyVolatilitySpread<numSec, numSecPred, static_cast<int>(BuySellSide::buy)> : public Feature 
    {
    private:
    	string createFeatureName() 
    	{
    	    stringstream ss;
    	    ss << "HighFrequencyVolatilitySpread<" << numSec << ":" << numSecPred << " Second:Bid>";
    	    return ss.str();
    	}
	
	ImpliedSpreadPrice<static_cast<int>(BuySellSide::buy)> iSpreadPrice_;
    	QtMVar * ma_px_var_;
	
    public:
    	HighFrequencyVolatilitySpread() 
    	    : Feature(createFeatureName()),
    	      ma_px_var_(new QtMVar(4, 2, numSec, 0, 0, QtEMA::IntrpType::Next))
    	{}

    	~HighFrequencyVolatilitySpread()
    	{
    	    delete ma_px_var_;
    	}
	
    	virtual void update(BookUpdate<true> book, bool frontQ)
    	{
	    iSpreadPrice_.update(book, frontQ);
    	    // compute volatility if mid
    	    if (iSpreadPrice_.isGood()) {
    		isGood_ = true;
		ma_px_var_->update(static_cast<double>(book.getTimestamp().time_since_epoch().count())/1e9, iSpreadPrice_.getValue());
    		value_ = ma_px_var_->stdev() * sqrt(static_cast<double>(numSecPred)/static_cast<double>(numSec));
    	    } else {
    		isGood_ = false;
    		badValue_ = value_;
    	    }
    	}
    };

    template<int numSec, int numSecPred>
    class HighFrequencyVolatilitySpread<numSec, numSecPred, static_cast<int>(BuySellSide::sell)> : public Feature 
    {
    private:
    	string createFeatureName() 
    	{
    	    stringstream ss;
    	    ss << "HighFrequencyVolatilitySpread<" << numSec << ":" << numSecPred << " Second:Ask>";
    	    return ss.str();
    	}
	
	ImpliedSpreadPrice<static_cast<int>(BuySellSide::sell)> iSpreadPrice_;
    	QtMVar * ma_px_var_;
	
    public:
    	HighFrequencyVolatilitySpread() 
    	    : Feature(createFeatureName()),
    	      ma_px_var_(new QtMVar(4, 2, numSec, 0, 0, QtEMA::IntrpType::Next))
    	{}

    	~HighFrequencyVolatilitySpread()
    	{
    	    delete ma_px_var_;
    	}
	
    	virtual void update(BookUpdate<true> book, bool frontQ)
    	{
	    iSpreadPrice_.update(book, frontQ);
    	    // compute volatility if mid
    	    if (iSpreadPrice_.isGood()) {
    		isGood_ = true;
		ma_px_var_->update(static_cast<double>(book.getTimestamp().time_since_epoch().count())/1e9, iSpreadPrice_.getValue());
    		value_ = ma_px_var_->stdev() * sqrt(static_cast<double>(numSecPred)/static_cast<double>(numSec));
    	    } else {
    		isGood_ = false;
    		badValue_ = value_;
    	    }
    	}
    };

    /* running High */
    template<int numSec>
    class WindowedBreakthrough : public Feature
    {
    private:
	string createFeatureName()
	{
	    stringstream ss;
	    ss << "WindowedBreakThrough<" << numSec << " Second>";
	    return ss.str();
	}

	typedef tuple<time_stamp, double> time_px_tuple;
	deque<time_px_tuple> time_px_deque_;
	double high_;
	double low_;
	double last_;
	unsigned long long dur_ = numSec * 1000000000ULL;
	bool   isReady_;

    public:
	WindowedBreakthrough()
	    : Feature(createFeatureName()),
	      high_(numeric_limits<double>::min()), low_(numeric_limits<double>::max()), last_(0.0), isReady_(false)
	{}

	virtual void update(TradeUpdate<true> trade)
	{
	    last_ = trade.getPx().getPx();
	    isGood_ = last_ > TOL ? true : false;
	    if (isReady_ && last_ > high_ + TOL) {
		value_ = 1.0;
	    } else if (isReady_ && last_ < low_ - TOL) {
		value_ = -1.0;
	    } else {
		value_ = 0.0;
	    }
	    time_px_deque_.push_back(make_tuple(trade.getTimestamp(), last_));
	    
	    if (time_px_deque_.size() >= 1 &&
	       (get<0>(time_px_deque_.back()) - get<0>(time_px_deque_.front())).count() >= dur_ ) {
		isReady_ = true;
	    } else {
		isReady_ = false;
	    }
	    high_ = std::max(high_, last_);
	    low_ = std::min(low_, last_);
	    
	    time_stamp ts_old = trade.getTimestamp() - duration_nano(dur_);
	    bool renew_high_low = false;
	    while(!time_px_deque_.empty() && get<0>(time_px_deque_.front()) < ts_old) {
		double old_px = get<1>(time_px_deque_.front());
		if(old_px >= high_ - TOL ||
		   old_px <= low_ + TOL ) {
		    renew_high_low = true;
		}
		time_px_deque_.pop_front();
	    }
	    if (renew_high_low) {
		high_ = last_;
		low_ = last_;
		for (auto it = time_px_deque_.begin();
		    it != time_px_deque_.end(); it++) {
		    high_ = std::max(high_, get<1>(*it));
		    low_ = std::min(low_, get<1>(*it));
		}
	    }
	}
    };

    /* ema of running break through */
    template<int numSec, int decay>
    class EMAWindowedBreakthrough : public Feature
    {
    private:
	string createFeatureName()
	{
	    stringstream ss;
	    ss << "EMAWindowedBreakthrough<" << numSec << " Second:" 
	       << decay << ">";
	    return ss.str();
	}

	WindowedBreakthrough<numSec> wbt_;
	double decay_;

    public:
	EMAWindowedBreakthrough()
	    : Feature(createFeatureName()),
	      wbt_(), decay_(1.0 - 1.0 / decay)
	{}

	virtual void update(TradeUpdate<true> trade)
	{
	    wbt_.update(trade);
	    //value_ = wbt_.getValue() + value_ * decay_;
	    /* TOCHECK */
	    // Dicuss with Mingyuan?
	    if (wbt_.isGood()) {
		isGood_ = true;
		value_ += (wbt_.getValue() - value_) * decay_;
	    } else {
		isGood_ = false;
	    }
	}
    };

    // in ticks, band in ticks
    template<int numSec, int ntick >
    class WindowedBreakBand : public Feature
    {
    private:
	string createFeatureName()
	{
	    stringstream ss;
	    ss << "WindowedBreakBand<" << numSec << " Second:" << ntick << " Ticks>";
	    return ss.str();
	}

	typedef tuple<time_stamp, double> time_px_tuple;
	deque<time_px_tuple> time_px_deque_;
	double high_;
	double low_;
	double last_;
	unsigned long long dur_ = numSec * 1000000000ULL;
	int band_ = ntick;
	bool   isReady_;

    public:
	WindowedBreakBand()
	    : Feature(createFeatureName()),
	      high_(numeric_limits<double>::min()), low_(numeric_limits<double>::max()), last_(0.0), isReady_(false)
	{}

	virtual void update(TradeUpdate<true> trade)
	{
	    last_ = trade.getPx().getPx();
	    isGood_ = last_ > TOL ? true : false;
	    if (isReady_ && last_ > high_ + TOL) {
		value_ = high_ - low_ < band_ * trade.getPx().getTickSize() ? 1.0 : 0.0;
	    } else if (isReady_ && last_ < low_ - TOL) {
		value_ = high_ - low_ < band_ * trade.getPx().getTickSize() ? -1.0 : 0.0;
	    } else {
		value_ = 0.0;
	    }
	    time_px_deque_.push_back(make_tuple(trade.getTimestamp(), last_));
	    
	    if (time_px_deque_.size() >= 2 &&
	       (get<0>(time_px_deque_.back()) - get<0>(time_px_deque_.front())).count() >= dur_ ) {
		isReady_ = true;
	    } else {
		isReady_ = false;
	    }
	    high_ = std::max(high_, last_);
	    low_ = std::min(low_, last_);
	    
	    time_stamp ts_old = trade.getTimestamp() - duration_nano(dur_);
	    bool renew_high_low = false;
	    while (!time_px_deque_.empty() && get<0>(time_px_deque_.front()) < ts_old) {
		double old_px = get<1>(time_px_deque_.front());
		if (old_px >= high_ - TOL ||
		   old_px <= low_ + TOL ) {
		    renew_high_low = true;
		}
		time_px_deque_.pop_front();
	    }
	    if (renew_high_low) {
		high_ = last_;
		low_ = last_;
		for (auto it = time_px_deque_.begin();
		    it != time_px_deque_.end(); it++) {
		    high_ = std::max(high_, get<1>(*it));
		    low_ = std::min(low_, get<1>(*it));
		}
	    }
	}
    };

    //template<int numSec, int ntick, int minCount>
    class TradeBreakOut : public Feature
    {
    private:
	string createFeatureName(int numSec, int ntick, int minCount)
	{
	    stringstream ss;
	    ss << "TradeBreakOut<" << numSec << " Second:" << ntick << " Ticks:" << minCount << ">";
	    return ss.str();
	}

	SupportResistence supres_;
	int ntick_;
	int minCount_;

    public:
	TradeBreakOut(int numSec, int ntick, int minCount);
	void update(TradeUpdate<true> trade);
    };

    // observed sweep force in ticks
    template<int bidAskInt, int tickNum, int tickDenum>
    class SweepWeak;

    template<int tickNum, int tickDenum>
    class SweepWeak<1, tickNum, tickDenum> : public Feature
    {
    private:
	string createFeatureName()
	{
	    static_assert(tickNum > 0, "Incorrect template parameter for SweepWeak feature");
	    static_assert(tickDenum > 0, "Incorrect template parameter for SweepWeak feature");
	    stringstream ss;
	    ss << "SweepWeak<" << "Ask"
	       << " " << tickNum
	       << "/" << tickDenum << ">";
	    return ss.str();
	}
	static constexpr double tickSize_ = tickNum * 1.0 / tickDenum;
	double lastPx_, change_;

    public:
	SweepWeak() : Feature(createFeatureName(), 0.0, 0.0)
	{}

	void update(BookUpdate<true> book)
	{
	    double px = book.getAskPx()[0].getPx();
	    if (book.getAskQty()[0] > 0) {
		isGood_ = true;
		change_ = px - lastPx_;
		value_ = (change_ > TOL) ? round(change_ / tickSize_) : 0.0;
		lastPx_ = px;
	    } else {
		isGood_ = false;
	    }
	}
    };

    template<int tickNum, int tickDenum>
    class SweepWeak<-1, tickNum, tickDenum> : public Feature
    {
    private:
	string createFeatureName()
	{
	    static_assert(tickNum > 0, "Incorrect template parameter for SweepWeak feature");
	    static_assert(tickDenum > 0, "Incorrect template parameter for SweepWeak feature");
	    stringstream ss;
	    ss << "SweepWeak<" << "Bid"
	       << " " << tickNum
	       << "/" << tickDenum << ">";
	    return ss.str();
	}
	static constexpr double tickSize_ = tickNum * 1.0 / tickDenum;
	double lastPx_, change_;

    public:
	SweepWeak() : Feature(createFeatureName(), 0.0, 0.0)
	{}

	void update(BookUpdate<true> book)
	{
	    double px = book.getBidPx()[0].getPx();
	    if (book.getBidQty()[0] > 0) {
		isGood_ = true;
		change_ = lastPx_ - px;
		value_ = (change_ > TOL) ? round(change_ / tickSize_) : 0.0;
		lastPx_ = px;
	    } else {
		isGood_ = false;
	    }
	}
    };

    // observed sweep force in ticks
    template<int bidAskInt, int tickNum, int tickDenum>
    class SweepStrong;

    template<int tickNum, int tickDenum>
    class SweepStrong<1, tickNum, tickDenum> : public Feature
    {
    private:
	string createFeatureName()
	{
	    static_assert(tickNum > 0, "Incorrect template parameter for SweepStrong feature");
	    static_assert(tickDenum > 0, "Incorrect template parameter for SweepStrong feature");
	    stringstream ss;
	    ss << "SweepStrong<" << "Ask"
	       << " " << tickNum
	       << "/" << tickDenum << ">";
	    return ss.str();
	}
	static constexpr double tickSize_ = tickNum * 1.0 / tickDenum;
	double lastPx_, bidAskPxSpread_, askPxChange_;

    public:
	SweepStrong() : Feature(createFeatureName(), 0.0, 0.0)
	{}

	void update(BookUpdate<true> book)
	{
	    double bidPx = book.getBidPx()[0].getPx();
	    double askPx = book.getAskPx()[0].getPx();
	    if (book.getAskQty()[0] > 0) {
		if (book.getBidQty()[0] > 0) {
		    isGood_ = true;
		    askPxChange_ = askPx - lastPx_;
		    bidAskPxSpread_ = bidPx - lastPx_ + tickSize_;
		    value_ = (askPxChange_ > TOL && bidAskPxSpread_ > TOL) ? round(askPxChange_ / tickSize_) : 0.0;
		} else {
		    isGood_ = false;
		}
		lastPx_ = askPx;
	    } else {
		isGood_ = false;
	    }
	}
    };

    template<int tickNum, int tickDenum>
    class SweepStrong<-1, tickNum, tickDenum> : public Feature
    {
    private:
	string createFeatureName()
	{
	    static_assert(tickNum > 0, "Incorrect template parameter for SweepStrong feature");
	    static_assert(tickDenum > 0, "Incorrect template parameter for SweepStrong feature");
	    stringstream ss;
	    ss << "SweepStrong<" << "Bid"
	       << " " << tickNum
	       << "/" << tickDenum << ">";
	    return ss.str();
	}
	static constexpr double tickSize_ = tickNum * 1.0 / tickDenum;
	double lastPx_, bidAskPxSpread_, bidPxChange_;

    public:
	SweepStrong() : Feature(createFeatureName(), 0.0, 0.0)
	{}

	void update(BookUpdate<true> book)
	{
	    double bidPx = book.getBidPx()[0].getPx();
	    double askPx = book.getAskPx()[0].getPx();
	    if (book.getBidQty()[0] > 0) {
		if (book.getAskQty()[0] > 0) {
		    isGood_ = true;
		    bidPxChange_ = lastPx_ - bidPx;
		    bidAskPxSpread_ = askPx - lastPx_ - tickSize_;
		    value_ = (bidPxChange_ > TOL && bidAskPxSpread_ < NTOL) ? round(bidPxChange_ / tickSize_) : 0.0;
		} else {
		    isGood_ = false;
		}
		lastPx_ = bidPx;
	    } else {
		isGood_ = false;
	    }
	}
    };

}

#endif  // FEATURE_HPP
