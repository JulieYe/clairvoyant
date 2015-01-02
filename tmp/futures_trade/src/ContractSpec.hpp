//
// File:        $ ContractSpec.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/03/20 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef CONTRACTSPEC_HPP
#define CONTRACTSPEC_HPP

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

namespace simulator
{

    class ContractSpec
    {
    private:
	ifstream data_file_;
	map<string, tuple<double, double, string, string> > content_;
	char delimiter_ = ',';

    public:
	ContractSpec() = delete;
	ContractSpec(ContractSpec const &) = delete;
	ContractSpec(ContractSpec&&) = delete;

	ContractSpec(string filename);
	~ContractSpec();

	bool getMultiplier(string tickerHead, double & multiplier) const;
	bool getTickSize(string tickerHead, double & tickSize) const;
	bool getDefaultStartTradingTime(string tickerHead, string & startTradingTime) const;
	bool getDefaultExitTradingTime(string tickerHead, string & stopTradingTime) const;
    };

}

#endif  // CONTRACTSPEC_HPP
