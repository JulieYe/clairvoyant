//
// File:        $ ConfigSpec.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/04/07 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include "ConfigSpec.hpp"

namespace simulator
{

    ConfigSpec::ConfigSpec(string filename)
    {
	data_file_.open(filename.c_str(), ifstream::in);
	
	if (!data_file_.is_open() || !data_file_.good())
	    throw runtime_error("Bad contract specficiation file..");
	else {
	    string line;
	    string exchange, tickerHead, stopLoss, takeProfit, threshold, volThreshold, startTradingTime, exitTradingTime, slippage, bookSize, buyOpenPosition, sellOpenPosition, orderSize;
	    getline(data_file_, line, '\n');  // Discard header.
	    while (data_file_.good() && !data_file_.eof() && !data_file_.fail()) {
		getline(data_file_, line, '\n');
		istringstream iss(line);
		getline(iss, exchange, delimiter_);
		getline(iss, tickerHead, delimiter_);
		getline(iss, stopLoss, delimiter_);
		getline(iss, takeProfit, delimiter_);
		getline(iss, threshold, delimiter_);
		getline(iss, volThreshold, delimiter_);
		getline(iss, startTradingTime, delimiter_);
		getline(iss, exitTradingTime, delimiter_);
		getline(iss, slippage, delimiter_);
		getline(iss, bookSize, delimiter_);
		getline(iss, buyOpenPosition, delimiter_);
		getline(iss, sellOpenPosition, delimiter_);
		getline(iss, orderSize, delimiter_);
		content_[exchange + ":" + tickerHead] = make_tuple(atof(stopLoss.c_str()), 
								   atof(takeProfit.c_str()), 
								   atof(threshold.c_str()),
								   atof(volThreshold.c_str()), 
								   startTradingTime, 
								   exitTradingTime, 
								   atof(slippage.c_str()),
								   atoi(bookSize.c_str()),
								   atoi(buyOpenPosition.c_str()),
								   atoi(sellOpenPosition.c_str()),
								   atoi(orderSize.c_str())
								   );
	    }
	}
    }

    ConfigSpec::~ConfigSpec()
    {
	if (data_file_.is_open())
	    data_file_.close();
    }
    
    bool ConfigSpec::getStopLoss(string tickerHead, double & stopLoss) const
    {
	auto it = content_.find(tickerHead);
	if (it == content_.end()) {
	    return false;
	} else {
	    stopLoss = get<0>(it->second);
	    return true;
	}
    }

    bool ConfigSpec::getTakeProfit(string tickerHead, double & takeProfit) const
    {
	auto it = content_.find(tickerHead);
	if (it == content_.end()) {
	    return false;
	} else {
	    takeProfit = get<1>(it->second);
	    return true;
	}
    }

    bool ConfigSpec::getThreshold(string tickerHead, double & threshold) const
    {
    	auto it = content_.find(tickerHead);
	if (it == content_.end()) {
	    return false;
	} else {
	    threshold = get<2>(it->second);
	    return true;
	}
    }

    bool ConfigSpec::getVolThreshold(string tickerHead, double & volThreshold) const
    {
    	auto it = content_.find(tickerHead);
	if (it == content_.end()) {
	    return false;
	} else {
	    volThreshold = get<3>(it->second);
	    return true;
	}
    }

    bool ConfigSpec::getStartTradingTime(string tickerHead, string & startTradingTime) const
    {
    	auto it = content_.find(tickerHead);
	if (it == content_.end()) {
	    return false;
	} else {
	    startTradingTime = get<4>(it->second);
	    return true;
	}
    }

    bool ConfigSpec::getExitTradingTime(string tickerHead, string & stopTradingTime) const
    {
	auto it = content_.find(tickerHead);
	if (it == content_.end()) {
	    return false;
	} else {
	    stopTradingTime = get<5>(it->second);
	    return true;
	}
    }

    bool ConfigSpec::getSlippage(string tickerHead, double & slippage) const
    {
    	auto it = content_.find(tickerHead);
	if (it == content_.end()) {
	    return false;
	} else {
	    slippage = get<6>(it->second);
	    return true;
	}
    }

    bool ConfigSpec::getMedianBookSize(string tickerHead, int & bookSize) const
    {
    	auto it = content_.find(tickerHead);
	if (it == content_.end()) {
	    return false;
	} else {
	    bookSize = get<7>(it->second);
	    return true;
	}
    }

    bool ConfigSpec::getBuyOpenPosition(string tickerHead, int & buyOpenPosition) const
    {
    	auto it = content_.find(tickerHead);
	if (it == content_.end()) {
	    return false;
	} else {
	    buyOpenPosition = get<8>(it->second);
	    return true;
	}
    }

    bool ConfigSpec::getSellOpenPosition(string tickerHead, int & sellOpenPosition) const
    {
    	auto it = content_.find(tickerHead);
	if (it == content_.end()) {
	    return false;
	} else {
	    sellOpenPosition = get<9>(it->second);
	    return true;
	}
    }

    bool ConfigSpec::getOrderSize(string tickerHead, int & orderSize) const
    {
    	auto it = content_.find(tickerHead);
	if (it == content_.end()) {
	    return false;
	} else {
	    orderSize = get<10>(it->second);
	    return true;
	}
    }
}

// CONFIGSPEC_CPP
