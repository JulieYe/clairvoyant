//
// File:        $ Technical.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/04/16 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef TECHNICAL_HPP
#define TECHNICAL_HPP

#include <chrono>
#include <string>
#include <tuple>
#include <deque>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <memory>
#include <cmath>

#define TOL 1e-6

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

#include "Message.hpp"

namespace simulator
{
    class LocalOptima 
    {
    private:
	typedef tuple<time_stamp, double> time_px_tuple;
	deque<time_px_tuple> time_px_deque_;
	bool lookingForHigh_;
	bool lookingForLow_;
	double previousHigh_;
	double previousLow_;
	double recordedHigh_;
	double recordedLow_;
	double last_;
	int noiseBand_;
	bool   newHigh_;
	bool   newLow_;
    public:
	LocalOptima(int noiseband);
	void update(double price, double ticksize);
	inline const bool isNewHigh() { return newHigh_;}
	inline const bool isNewLow() { return newLow_;}
	inline const double recentLocalHigh() { return recordedHigh_;}
	inline const double recentLocalLow() { return recordedLow_;}
    };


    class SupportResistence
    {
    private:
	LocalOptima localOptima_;
	typedef tuple<time_stamp, double> time_px_tuple;
	deque<time_px_tuple> time_px_localhigh_deque_;
	deque<time_px_tuple> time_px_locallow_deque_;
	deque<time_stamp> time_queue_;
	unsigned long long dur_;
	bool isSupportEstablished_;
	bool isResistenceEstablished_;
	double supportLevel_;
	int supportTouchCount_;
	double resistenceLevel_;
	int resistenceTouchCount_;
	int ntick_;
    public:
	SupportResistence(int nsec, int ntick);

	inline const bool isSupportEstablished() { return isSupportEstablished_; }
	inline const bool isResistenceEstablished() { return isResistenceEstablished_; }
	inline const double supportLevel() { return supportLevel_; }
	inline const double resistenceLevel() { return resistenceLevel_; }
	inline const int supportTouchCount() { return supportTouchCount_; }
	inline const int resistenceTouchCount() { return resistenceTouchCount_; }

    private:
	void updateResistence(double price, double ticksize);
	void updateSupport(double price, double ticksize);
	void cleanOld(time_stamp ts, double ticksize);
    public:
	void update(time_stamp ts, double price, double ticksize);
    };





}

#endif
