//
// File:        $ ContractSpec.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/03/20 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include "ContractSpec.hpp"

namespace simulator
{

    ContractSpec::ContractSpec(string filename)
    {
	data_file_.open(filename.c_str(), ifstream::in);
	
	if (!data_file_.is_open() || !data_file_.good())
	    throw runtime_error("Bad contract specficiation file..");
	else {
	    string line;
	    string exchange, tickerHead, multiplier, tickSize, startTradingTime, stopTradingTime;
	    getline(data_file_, line, '\n');  // Discard header.
	    while (data_file_.good() && !data_file_.eof() && !data_file_.fail()) {
		getline(data_file_, line, '\n');
		istringstream iss(line);
		getline(iss, exchange, delimiter_);
		getline(iss, tickerHead, delimiter_);
		getline(iss, multiplier, delimiter_);
		getline(iss, tickSize, delimiter_);
		getline(iss, startTradingTime, delimiter_);
		getline(iss, stopTradingTime, delimiter_);
		content_[exchange + ":" + tickerHead] = make_tuple(atof(multiplier.c_str()), atof(tickSize.c_str()), startTradingTime, stopTradingTime);
	    }
	}
    }

    ContractSpec::~ContractSpec()
    {
	if (data_file_.is_open())
	    data_file_.close();
    }
    
    bool ContractSpec::getMultiplier(string tickerHead, double & multiplier) const
    {
	auto it = content_.find(tickerHead);
	if (it == content_.end()) {
	    return false;
	} else {
	    multiplier = get<0>(it->second);
	    return true;
	}
    }

    bool ContractSpec::getTickSize(string tickerHead, double & tickSize) const
    {
	auto it = content_.find(tickerHead);
	if (it == content_.end()) {
	    return false;
	} else {
	    tickSize = get<1>(it->second);
	    return true;
	}
    }

    bool ContractSpec::getDefaultStartTradingTime(string tickerHead, string & startTradingTime) const
    {
    	auto it = content_.find(tickerHead);
	if (it == content_.end()) {
	    return false;
	} else {
	    startTradingTime = get<2>(it->second);
	    return true;
	}
    }

    bool ContractSpec::getDefaultExitTradingTime(string tickerHead, string & stopTradingTime) const
    {
	auto it = content_.find(tickerHead);
	if (it == content_.end()) {
	    return false;
	} else {
	    stopTradingTime = get<3>(it->second);
	    return true;
	}
    }
}

// CONTRACTSPEC_CPP
