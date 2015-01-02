//
// File:        $ ConfigReader.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/05/07 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.


#ifndef CONFIGREADER_HPP
#define CONFIGREADER_HPP

#include <map>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <iostream>

using std::map;
using std::string;
using std::ifstream;
using std::istringstream;
using std::runtime_error;


namespace simulator
{
    class ConfigReader
    {
    private:
	map<string, string> content_;
    public:
	ConfigReader() = delete;
	ConfigReader(ConfigReader const &) = delete;
	ConfigReader(ConfigReader&&) = delete;
	
	ConfigReader(string filename);


	string getString(string key);
	int getInteger(string key);
	double getDouble(string key);

    };
}

#endif
