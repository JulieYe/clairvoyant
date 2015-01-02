//
// File:        $ MarketDataCollector.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/01/26 16:06:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2012 by Zhengzhang Futures Co. Ltd.

#ifndef MARKETDATACOLLECTOR_HPP
#define MARKETDATACOLLECTOR_HPP

#include <vector>
#include <string>
#include <sstream>

using std::vector;
using std::string;
using std::stringstream;

#include "SimulatorCore.hpp"
#include "Strategy.hpp"
#include "Message.hpp"
#include "Logger.hpp"

using simulator::Px;
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
using simulator::OrderBook;
using simulator::SimulatorCore;
using simulator::Logger;
using simulator::time_stamp;

namespace simulator
{

	class MarketDataCollector : public Strategy
	{
	private:
		const unsigned bookDepth_;
	public:
		MarketDataCollector(const unsigned book_depth, SimulatorCore * simcore_ptr, Logger * logger_ptr);
		void process();
	};

}


		
#endif
