//
// File:        $ test_featurecollector.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/01/26 16:44:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2012 by Zhengzhang Futures Co. Ltd.

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE regression_test_featurecollector
#include <boost/test/unit_test.hpp>

#include <fstream>

using std::ifstream;
using std::getline;

#include "../FeatureCollector.hpp"
#include "../Feature.hpp"
#include "../CFFEX.hpp"

using simulator::Logger;
using simulator::Matchy;
using simulator::CFFEX;
using simulator::SimulatorCore;
using simulator::FeatureCollector;
using simulator::MidPoint;

BOOST_AUTO_TEST_CASE(featurecollector_historical_file)
{
    {
	SimulatorCore sc("2012-08-15T09:15:00.0", "2012-08-15T15:16:00.0", 500, "millisecond");
	
	Logger logger("./tmp/test_fc.csv");
	logger.start();
	CFFEX exch("IF1209", "./test/IF.test.csv", &sc);
	FeatureCollector fc("CFFEX", "IF1209", &sc, &logger);
	
	fc.addFeature(PFeature(new MidPoint()), 0);
	fc.addFeature(PFeature(new MidPoint()), 500);
	fc.addFeature(PFeature(new MidPoint()), 1000);
	
	fc.writeHeader();
	
	sc.registerMatchy(&exch);
	sc.registerStrategy(&fc);
	
	while (sc.next()) {}
	logger.end();
    }
    
    ifstream fd, fg;
    fd.open("./tmp/test_fc.csv", ifstream::in);
    fg.open(".gold_feature_collector.csv", ifstream::in);
    string lined, lineg;
    while(fd.is_open() && fd.good() && !fd.eof() &&
	  fg.is_open() && fg.good() && !fg.eof() ) {
	getline(fd, lined);
	getline(fg, lineg);
	BOOST_CHECK(lined == lineg);
    }
}
