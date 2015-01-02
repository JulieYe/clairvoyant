//
// File:        $ MMSpreader.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/08/05 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef MMSPREADER_HPP
#define MMSPREADER_HPP

#include <sstream>
#include <limits>
#include <cstdlib>
#include <cmath>

using std::string;
using std::stringstream;
using std::numeric_limits;
using std::abs;
using std::fabs;

#include "Px.hpp"
#include "Strategy.hpp"
#include "SimulatorCore.hpp"
#include "Logger.hpp"

using simulator::Px;
using simulator::Strategy;
using simulator::SimulatorCore;
using simulator::Logger;

namespace simulator
{
    class MMSpreader : public Strategy
    {
    private:
	// contract specification related
        const double multiplier_;
        const double tickSize_;
    
        // trading specification related
        bool enableTrading_ = false;
        bool closePosition_ = false;
        string startTradingTimeString_;
        string exitTradingTimeString_;
        time_stamp startTradingTs_;
        time_stamp exitTradingTs_;
        double spreads_[2] = {numeric_limits<double>::max(), numeric_limits<double>::max()};
	int spreadPositions_[2] = {1, -1};

    public:
	MMSpreader(string exchange, string contract0, string contract1, double multiple, double tickSize, string startTradingTimeString, string exitTradingTimeString, SimulatorCore * simcore_ptr, Logger * logger_ptr);
	~MMSpreader();

        inline bool tightQ(int productId);

	bool isBookGood(BookUpdate<true> &);
	
	void process();
    };

}

#endif  // MMSPREADER_HPP
