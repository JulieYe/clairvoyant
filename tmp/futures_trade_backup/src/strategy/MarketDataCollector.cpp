//
// File:        $ MarketDataCollector.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/01/26 16:06:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2012 by Zhengzhang Futures Co. Ltd.

#include "MarketDataCollector.hpp"

namespace simulator
{

	MarketDataCollector::MarketDataCollector(const unsigned book_depth, SimulatorCore * simcore_ptr, Logger * logger_ptr) :
		Strategy("MarketDataCollector", simcore_ptr, logger_ptr),
		bookDepth_(book_depth)
	{ 
		stringstream ss;
		ss << "ticker,date_time,last_px,total_volume";
		for(unsigned ii = 0; ii < book_depth; ii++) {
			ss << ',' << "bid" << ii+1 << "_px"
			   << ',' << "bid" << ii+1 << "_qty";
		}
		for(unsigned ii = 0; ii < book_depth; ii++) {
			ss << ',' << "ask" << ii+1 << "_px"
			   << ',' << "ask" << ii+1 << "_qty";
		}
		loggerPtr_->push(ss.str());
	}

	void MarketDataCollector::process()
	{
		for(size_t ii = 0; ii < getNumProducts(); ii++) {
			// assuming bookUpdates and tradeUpdates have the same size
			auto ibu = bookUpdates_[ii].begin();
			auto itu = tradeUpdates_[ii].begin();
			while(ibu != bookUpdates_[ii].end() && itu != tradeUpdates_[ii].end() ) {
				stringstream ss;
				ss << tickers_[ii] << ','
				   << SimulatorCore::convertTimePointToString((*ibu).getTimestamp()) << ',';
				ss << (*itu).getPx().getPx() << ','
				   << (*itu).getQty();
				for(unsigned ii = 0; ii < bookDepth_; ii++) {
					ss << ',' << (*ibu).getBidPx()[ii].getPx()
					   << ',' << (*ibu).getBidQty()[ii];
				}
				for(unsigned ii = 0; ii < bookDepth_; ii++) {
					ss << ',' << (*ibu).getAskPx()[ii].getPx()
					   << ',' << (*ibu).getAskQty()[ii];
				}
				loggerPtr_->push(ss.str());
				ibu++;
				itu++;
			}
		}
	}

}


		
