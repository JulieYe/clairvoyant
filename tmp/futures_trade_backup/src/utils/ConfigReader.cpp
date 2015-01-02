//
// File:        $ ConfigReader.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/05/07 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include "ConfigReader.hpp"


namespace simulator
{
    ConfigReader::ConfigReader(string filename)
    {
	content_.clear();
	ifstream datafile;
	datafile.open(filename.c_str(), ifstream::in);
	if (!datafile.is_open() || !datafile.good())
	    throw runtime_error("Bad contract specficiation file..");
	else {
	    string line;
	    string key;
	    char delimiter = '=';
	    string value;
	    while(datafile.good() && !datafile.eof() && !datafile.fail()) {
		getline(datafile, line);
		unsigned found = line.find(delimiter);
		if(found == string::npos) {
		    std::cerr << "cannot parse line : " << line << std::endl;
		    continue;
		}
		istringstream iss(line);
		getline(iss, key, delimiter);
		getline(iss, value, delimiter);
		content_[key] = value;
	    }
	}
	if(datafile.is_open()) {
	    datafile.close();
	}
    }

    string ConfigReader::getString(string key)
    {
	return content_[key];
    }

    int ConfigReader::getInteger(string key) 
    {
	return atoi(content_[key].c_str());
    }

    double ConfigReader::getDouble(string key)
    {
	return strtod(content_[key].c_str(), NULL);
    }


    void ConfigReader::writeConfig(string filename)
    {
	ofstream datafile;
	datafile.open(filename.c_str(), ofstream::out);
	if (!datafile.is_open() || !datafile.good())
	    throw runtime_error("bad output file ...");
	else {
	    for(auto it = content_.begin(); it != content_.end(); it++) {
		datafile << it->first << "=" << it->second << "\n";
	    }

	}
	if (datafile.is_open()) {
	    datafile.close();
	}

    }

}
