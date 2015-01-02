//
// File:        $ Technical.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2014/01/25 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2014 by Zhengzhang Futures Co. Ltd.

#define TOL 1e-6

//#include "../core/Message.hpp"
#include "Technical.hpp"

namespace simulator
{
    LocalOptima::LocalOptima(int noiseband)
	: lookingForHigh_(true), lookingForLow_(true),
	  previousHigh_(-1e9), previousLow_(1e9), 
	  recordedHigh_(-1e9), recordedLow_(1e9),
	  last_(0.0), noiseBand_(noiseband),
	  newHigh_(false), newLow_(false)
    {
    }

    void LocalOptima::update(double price, double ticksize)
    {
	last_ = price;
	newHigh_ = false;
	newLow_ = false;
	if(lookingForHigh_) {
	    previousHigh_ = max(previousHigh_, last_);
	    if(last_ <= previousHigh_ - noiseBand_ * ticksize + TOL) {
		lookingForHigh_ = false;
		lookingForLow_ = true;
		previousLow_ = last_;
		recordedHigh_ = previousHigh_;
		newHigh_ = true;
	    }
	}
	if(lookingForLow_) {
	    previousLow_ = min(previousLow_, last_);
	    if(last_ >= previousLow_ + noiseBand_ * ticksize - TOL) {
		lookingForLow_ = false;
		lookingForHigh_ = true;
		previousHigh_ = last_;
		recordedLow_ = previousLow_;
		newLow_ = true;
	    }
	}
    }


    SupportResistence::SupportResistence(int nsec, int ntick)
	: localOptima_(ntick), 
	  time_px_localhigh_deque_(), time_px_locallow_deque_(), time_queue_(),
	  dur_(nsec * 1000000000ULL), 
	  isSupportEstablished_(false), isResistenceEstablished_(false),
	  supportLevel_(1e9), 
	  supportTouchCount_(0), 
	  resistenceLevel_(-1e9),
	  resistenceTouchCount_(0),
	  ntick_(ntick)
    { }


    void SupportResistence::updateResistence(double price, double ticksize)
    {
	if(resistenceLevel_ < price - ticksize * 0.5) {
	    // new resistenceLevel
	    resistenceLevel_ = price;
	    resistenceTouchCount_ = 1;
	} else if(resistenceLevel_ < price + ntick_ * ticksize + TOL) {
	    // resistenceLevel established
	    resistenceTouchCount_ += 1;
	}		    
    }
    
    void SupportResistence::updateSupport(double price, double ticksize)
    {
	if(supportLevel_ > price + ticksize * 0.5) {
	    // new supportLevel
	    supportLevel_ = price;
	    supportTouchCount_ = 1;
	} else if(supportLevel_ > price - ntick_ * ticksize - TOL) {
	    // supportLevel established 
	    supportTouchCount_ += 1;
	}
    }

    void SupportResistence::cleanOld(time_stamp ts, double ticksize)
    {
	time_stamp ts_old = ts - duration_nano(dur_);
	bool renew = false;
	while(!time_px_localhigh_deque_.empty() && get<0>(time_px_localhigh_deque_.front()) < ts_old) {
	    double old_px = get<1>(time_px_localhigh_deque_.front());
	    if(old_px > resistenceLevel_ - TOL ) {
		renew = true;
	    }
	    time_px_localhigh_deque_.pop_front();
	}
	if(renew) {
	    // renew resistenceLevel_ and resistencTouchCount_;
	    isResistenceEstablished_ = false;
	    resistenceTouchCount_ = 0;
	    resistenceLevel_ = -1e9;
	    for(auto it = time_px_localhigh_deque_.begin();
		it != time_px_localhigh_deque_.end(); it++) {
		updateResistence(get<1>(*it), ticksize);
	    }
	}
	
	renew = false;
	while(!time_px_locallow_deque_.empty() && get<0>(time_px_locallow_deque_.front()) < ts_old) {
	    double old_px = get<1>(time_px_locallow_deque_.front());
	    if(old_px < supportLevel_ + TOL) {
		renew = true;
	    }
	    time_px_locallow_deque_.pop_front();
	}
	if(renew) {
	    // renew supportLevel_ and supportTouchCount_;
	    isSupportEstablished_ = false;
	    supportTouchCount_ = 0;
	    supportLevel_ = 1e9;
	    for(auto it = time_px_locallow_deque_.begin();
		it != time_px_locallow_deque_.end(); it++) {
		updateSupport(get<1>(*it), ticksize);
	    }
	}
	while(!time_queue_.empty() && time_queue_.front() < ts_old) {
	    time_queue_.pop_front();
	}
	
    }
    
    void SupportResistence::update(time_stamp ts, double price, double ticksize)
    {
	cleanOld(ts, ticksize);
	
	time_queue_.push_back(ts);
	
	localOptima_.update(price, ticksize);
	if(localOptima_.isNewHigh()) {
	    time_px_localhigh_deque_.push_back(make_tuple(ts, localOptima_.recentLocalHigh()));
	    updateResistence(localOptima_.recentLocalHigh(), ticksize);
	} else if (localOptima_.isNewLow()) {
	    time_px_locallow_deque_.push_back(make_tuple(ts, localOptima_.recentLocalLow()));
	    updateSupport(localOptima_.recentLocalLow(), ticksize);
	}
	
	if(time_px_localhigh_deque_.size() > 1 && time_queue_.size() > 1 &&
	   (ts - time_queue_.front()).count() >= dur_ &&
	   (resistenceTouchCount_ > 1) &&
	   price <= resistenceLevel_ + TOL ) {
	    isResistenceEstablished_ = true;
	} else {
	    isResistenceEstablished_ = false;
	}
	
	if(time_px_locallow_deque_.size() > 1 && time_queue_.size() > 1 &&
	   (ts - time_queue_.front()).count() >= dur_ &&
	   (supportTouchCount_ > 1) &&
	   price >= supportLevel_ - TOL) {
	    isSupportEstablished_ = true;
	} else {
	    isSupportEstablished_ = false;
	}
	
    }

}

