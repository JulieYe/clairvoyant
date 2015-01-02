//
// File:        $ ConfigSpec.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/04/07 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef CONFIGSPEC_HPP
#define CONFIGSPEC_HPP

#include <map>
#include <tuple>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <cstdlib>

using std::map;
using std::tuple;
using std::make_tuple;
using std::get;
using std::ifstream;
using std::istringstream;
using std::runtime_error;
using std::string;
using std::atof;
using std::atoi;

namespace simulator
{

    class ConfigSpec
    {
    private:
	ifstream data_file_;
	map<string, tuple<double, double, double, double, string, string, double, int, int, int, int> > content_;
	char delimiter_ = ',';

    public:
	ConfigSpec() = delete;
	ConfigSpec(ConfigSpec const &) = delete;
	ConfigSpec(ConfigSpec&&) = delete;

	ConfigSpec(string filename);
	~ConfigSpec();

	bool getStopLoss(string tickerHead, double & stopLoss) const;
	bool getTakeProfit(string tickerHead, double & takeProfit) const;
	bool getThreshold(string tickerHead, double & threshold) const;
	bool getVolThreshold(string tickerHead, double & volThreshold) const;
	bool getStartTradingTime(string tickerHead, string & startTradingTime) const;
	bool getExitTradingTime(string tickerHead, string & stopTradingTime) const;
	bool getSlippage(string tickerHead, double & slippage) const;
	bool getMedianBookSize(string tickerHead, int & bookSize) const;
	bool getBuyOpenPosition(string tickerHead, int & buyOpenPosition) const;
	bool getSellOpenPosition(string tickerHead, int & sellOpenPosition) const;
	bool getOrderSize(string tickerHead, int & orderSize) const;
    };

}

#endif  // CONFIGSPEC_HPP
