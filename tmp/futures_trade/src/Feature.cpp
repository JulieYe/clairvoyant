//
// File:        $ Feature.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/10/14 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include "Feature.hpp"

namespace simulator
{
    Feature::Feature() :
	featureName_("feature"),
	value_(0.0),
	badValue_(0.0),
	isGood_(false),
	useBound_(false),
	upperBound_(1e6),
	lowerBound_(-1e6)
    {}
    
    Feature::Feature(string featureName) :
	featureName_(featureName),
	value_(0.0),
	badValue_(0.0),
	isGood_(false),
	useBound_(false),
	upperBound_(1e6),
	lowerBound_(-1e6)
    {}

    Feature::Feature(string featureName, double initValue) :
	featureName_(featureName),
	value_(initValue),
	badValue_(0.0),
	isGood_(false),
	useBound_(false),
	upperBound_(1e6),
	lowerBound_(-1e6)
    {}
    
    Feature::Feature(string featureName, double initValue, double badValue) :
	featureName_(featureName),
	value_(initValue),
	badValue_(badValue),
	isGood_(false),
	useBound_(false),
	upperBound_(1e6),
	lowerBound_(-1e6)
    {}

    Feature::~Feature()
    {}

    void Feature::setBound(bool useBound, double bound)
    {
	useBound_ = useBound;
	upperBound_ = std::abs(bound);
	lowerBound_ = -1.0 * upperBound_;
    }

    BookPressure::BookPressure() :
	Feature("bookPressure")
    {}

    void BookPressure::update(BookUpdate<true> book)
    {
	if (book.getBidQty()[0] > 0 && book.getAskQty()[0] > 0) {
	    isGood_ = true;
	    value_ = static_cast<double>(book.getBidQty()[0]) / static_cast<double>(book.getBidQty()[0] + book.getAskQty()[0]) - 0.5;
	} else {
	    isGood_ = false;
	}
    }

    BookPressureLog::BookPressureLog() :
	Feature("BookPressureLog")
    {}

    void BookPressureLog::update(BookUpdate<true> book)
    {
	if (book.getBidQty()[0] > 1 && book.getAskQty()[0] > 1) {
	    isGood_ = true;
	    value_ = log(static_cast<double>(book.getBidQty()[0])) / (log(static_cast<double>(book.getBidQty()[0])) + log(static_cast<double>(book.getAskQty()[0]))) - 0.5;
	} else if (book.getAskQty()[0] > 1 && book.getBidQty()[0] == 1) {
	    isGood_ = true;
	    value_ = -0.5;
	} else if (book.getBidQty()[0] > 1 && book.getAskQty()[0] == 1) {
	    isGood_ = true;
	    value_ = 0.5;
	} else if (book.getBidQty()[0] == 1 && book.getAskQty()[0] == 1) {
	    isGood_ = true;
	    value_ = 0.0;
	} else {
	    isGood_ = false;
	}
    }
	
    MidPoint::MidPoint() : 
	Feature("MidPoint")
    {}
	
    void MidPoint::update(BookUpdate<true> book)
    {
	double mid;
	if (book.getMidPx(&mid)) {
	    isGood_ = true;
	    value_ = mid;
	} else {
	    isGood_ = false;
	}
    }

    BidAskSpread::BidAskSpread() : 
	Feature("BidAskSpread") 
    {}

    void BidAskSpread::update(BookUpdate<true> book) 
    {
	double spread;
	if (book.getBidAskSpread(&spread)) {
	    isGood_ = true;
	    value_ = spread;
	} else {
	    isGood_ = false;
	}
    }

    TradeDeviation::TradeDeviation() :
	Feature("TradeDeviation")
    {}
	
    void TradeDeviation::update(BookUpdate<true> book, TradeUpdate<true> trade)
    {
	double midPx;
	if (book.getMidPx(&midPx) && midPx > TOL && trade.getPx().getPx() > TOL) {
	    isGood_ = true;
	    value_ = (trade.getPx().getPx() - midPx) / midPx;
	} else {
	    isGood_ = false;
	}
    }

    TradeBreakOut::TradeBreakOut(int numSec, int ntick, int minCount)
	: Feature(createFeatureName(numSec, ntick, minCount), 0.0, 0.0),
	  supres_(numSec, ntick),
	  ntick_(ntick), minCount_(minCount)
    {
	isGood_ = true;
    }

    void TradeBreakOut::update(TradeUpdate<true> trade)
    {
	const double price = trade.getPx().getPx();
	const double ticksize = trade.getPx().getTickSize();
	if (supres_.isResistenceEstablished() &&
	   supres_.resistenceTouchCount() >= minCount_ &&
	   price >= supres_.resistenceLevel() + ntick_ * ticksize - TOL) {
	    value_ = 1.0;
	} else if (supres_.isSupportEstablished() &&
		   supres_.supportTouchCount() >= minCount_ &&
		   price <= supres_.supportLevel() - ntick_ * ticksize + TOL) {
	    value_ = -1.0;
	} else {
	    value_ = 0.0;
	}
	supres_.update(trade.getTimestamp(), price, ticksize);
    }

}

// FEATURE_CPP
