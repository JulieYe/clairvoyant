//
// File:        $ test_feature.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/07/26 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE regression_test_feature
#include <boost/test/unit_test.hpp>

#include <fstream>
#include <sstream>

using std::ifstream;
using std::getline;
using std::stringstream;

#include "Feature.hpp"
#include "CFFEX.hpp"
#include "FeatureCollector.hpp"

using simulator::SignedVolume;
using simulator::BookUpdate;
using simulator::TradeUpdate;
using simulator::Px;
using simulator::Logger;
using simulator::Matchy;
using simulator::CFFEX;
using simulator::SimulatorCore;
using simulator::FeatureCollector;
using simulator::PFeature;

using simulator::MidPoint;
using simulator::BookPressure;
using simulator::BookPressureNL;
using simulator::BookPressureLog;
using simulator::ShortTermReturn;
using simulator::VolumeTrend;
using simulator::TradeDeviation;
using simulator::TradeDeviationMA;
using simulator::SignedVolume;
using simulator::SignedVolumeTradeVsMid;
using simulator::SignedVolumeWithBookSizeChg;
using simulator::Vwap;
using simulator::VwapMidReturn;
using simulator::BookSizeChg;
using simulator::BookSizeChg2;
using simulator::Volatility;
using simulator::HighFrequencyVolatility;
using simulator::WindowedBreakthrough;
using simulator::SweepWeak;
using simulator::SweepStrong;

BOOST_AUTO_TEST_CASE(test_features)
{

    // please update ".gold.test.feature.csv" after update features and test_features
    // update ".gold_feature.csv"?
    {
	SimulatorCore sc("2012-08-15T09:15:00.0", "2012-08-15T15:16:00.0", 500, "millisecond");
	Logger logger("../tmp/test_feature.csv");
	logger.start();
	CFFEX exch("IF1209", "../test/IF.test.csv", &sc);
	FeatureCollector fc("CFFEX", "IF1209", &sc, &logger);

	constexpr int numPeriodsPerYear = 252 * 4.5 * 60 * 60 * 2;
		
	fc.addFeature(PFeature(new MidPoint()), 0); // manually checked in R
	fc.addFeature(PFeature(new BookPressure()), 0); // manually checked in R
	fc.addFeature(PFeature(new BookPressureNL<1,4>()), 0); // checked
	fc.addFeature(PFeature(new BookPressureNL<1,2>()), 0); // checked
	fc.addFeature(PFeature(new BookPressureNL<2,1>()), 0); // checked
	fc.addFeature(PFeature(new BookPressureNL<3,1>()), 0); // checked
	fc.addFeature(PFeature(new BookPressureLog()), 0); // checked
	fc.addFeature(PFeature(new ShortTermReturn<500>()), 0); // manually checked in R
	fc.addFeature(PFeature(new ShortTermReturn<2000>()), 0); // manually checked in R
	fc.addFeature(PFeature(new VolumeTrend<2, 10>()), 0); // manually checked in R
	fc.addFeature(PFeature(new TradeDeviation()), 0); // manually checked in R
	fc.addFeature(PFeature(new TradeDeviationMA<10>()), 0); // manually checked in R
	fc.addFeature(PFeature(new SignedVolume<500>()), 0); // manually checked in R
	fc.addFeature(PFeature(new SignedVolume<1000>()), 0); // manually checked in R
	fc.addFeature(PFeature(new SignedVolumeTradeVsMid<500>()), 0); // checked
	fc.addFeature(PFeature(new SignedVolumeTradeVsMid<1000>()), 0); // checked
	fc.addFeature(PFeature(new SignedVolumeWithBookSizeChg<500>()), 0); // checked
	fc.addFeature(PFeature(new SignedVolumeWithBookSizeChg<1000>()), 0); // checked
	fc.addFeature(PFeature(new Vwap<60000>()), 0); // manually checked in R
	fc.addFeature(PFeature(new VwapMidReturn<60000>()), 0); // manually checked in R
	fc.addFeature(PFeature(new BookSizeChg<500, static_cast<int>(BuySellSide::buy) >()), 0); // manually checked in R
	fc.addFeature(PFeature(new BookSizeChg<500, static_cast<int>(BuySellSide::sell) >()), 0); // manually checked in R
	fc.addFeature(PFeature(new BookSizeChg2<500, static_cast<int>(BuySellSide::buy) >()), 0); // checked
	fc.addFeature(PFeature(new BookSizeChg2<500, static_cast<int>(BuySellSide::sell) >()), 0); // checked
	fc.addFeature(PFeature(new BookSizeChg2<1000, static_cast<int>(BuySellSide::buy) >()), 0); // checked
	fc.addFeature(PFeature(new BookSizeChg2<1000, static_cast<int>(BuySellSide::sell) >()), 0); // checked
	fc.addFeature(PFeature(new Volatility<5, numPeriodsPerYear>()), 0); // manually checked in R
	fc.addFeature(PFeature(new Volatility<10, numPeriodsPerYear>()), 0); // manually checked in R
	fc.addFeature(PFeature(new Volatility<20, numPeriodsPerYear>()), 0); // mannually checked in R
	fc.addFeature(PFeature(new Volatility<60, numPeriodsPerYear>()), 0); // mannually checked in R
	fc.addFeature(PFeature(new HighFrequencyVolatility<60, 2>()), 0); // checked, not sure about any numerical issue
	fc.addFeature(PFeature(new WindowedBreakthrough<60>()), 0); // checked
	fc.addFeature(PFeature(new WindowedBreakthrough<300>()), 0); // checked
	fc.addFeature(PFeature(new SweepWeak<1, 1, 5>()), 0); // checked
	fc.addFeature(PFeature(new SweepWeak<-1, 1, 5>()), 0); // checked
	fc.addFeature(PFeature(new SweepStrong<1, 1, 5>()), 0); // checked
	fc.addFeature(PFeature(new SweepStrong<-1, 1, 5>()), 0); // checked

	fc.writeHeader();
	sc.registerMatchy(&exch);
	sc.registerStrategy(&fc);

	while (sc.next()) {}
	logger.end();
    }

    ifstream fd, fg;
    fd.open("../tmp/test_feature.csv", ifstream::in);
    fg.open(".gold_feature.csv", ifstream::in);
    string lined, lineg;
    getline(fd, lined);
    getline(fg, lineg);
    BOOST_CHECK(lined == lineg);
    // known numerical issue for different platforms are handled here
    int vwap6000_idx=-1, volatility5_idx=-1, volatility10_idx=-1, volatility20_idx=-1, volatility60_idx=-1;
    int signedvolumewithbooksizechg500_idx=-1, signedvolumewithbooksizechg1000_idx=-1;
    string item_header;
    stringstream string_header(lined);
    int count=0;
    while(getline(string_header, item_header, ',')) {
	if(item_header == "SignedVolumeWithBookSizeChg<500 Millisec>") {
	    signedvolumewithbooksizechg500_idx = count;
	}
	if(item_header == "SignedVolumeWithBookSizeChg<1000 Millisec>") {
	    signedvolumewithbooksizechg1000_idx = count;
	}
	if(item_header == "Vwap<60000 Millisec>") {
	    vwap6000_idx = count;
	} 
	if(item_header == "Volatility<5 Second>") {
	    volatility5_idx = count;
	}
	if(item_header == "Volatility<10 Second>") {
	    volatility10_idx = count;
	}
	if(item_header == "Volatility<20 Second>") {
	    volatility20_idx = count;
	}
	if(item_header == "Volatility<60 Second>") {
	    volatility60_idx = count;
	}
	count++;
    }
    while(fd.is_open() && fd.good() && !fd.eof() &&
	  fg.is_open() && fg.good() && !fg.eof() ) {
	getline(fd, lined);
	getline(fg, lineg);
	string itemd, itemg;
	stringstream stringd(lined);
	stringstream stringg(lineg);
	getline(stringd, itemd, ',');
	getline(stringg, itemg, ',');
	BOOST_CHECK(itemg == itemd);
	getline(stringd, itemd, ',');
	getline(stringg, itemg, ',');
	BOOST_CHECK(itemg == itemd);
	int itemcount = 2;
	while(getline(stringd, itemd, ',') && getline(stringg, itemg, ',')) {
	    //vwap exception: it should only differ upto the last digits
	    if(itemcount == vwap6000_idx) {
		BOOST_CHECK(std::abs(std::atof(itemd.c_str()) - std::atof(itemg.c_str())) < 0.011);
		if(std::abs(std::atof(itemd.c_str()) - std::atof(itemg.c_str())) >= 0.011) {
		    std::cout << lined << std::endl;
		    std::cout << lineg << std::endl;
		    std::cout << itemd << std::endl;
		    std::cout << itemg << std::endl;
		}
	    } else if(itemcount == signedvolumewithbooksizechg500_idx || 
		      itemcount == signedvolumewithbooksizechg1000_idx) {
		// signed volume with booksize change exception: it should only differ upto 2.
		BOOST_CHECK(std::abs(std::atof(itemd.c_str()) - std::atof(itemg.c_str())) < 2.01);
		if(std::abs(std::atof(itemd.c_str()) - std::atof(itemg.c_str())) >= 2.01) {
		    std::cout << lined << std::endl;
		    std::cout << lineg << std::endl;
		    std::cout << itemd << std::endl;
		    std::cout << itemg << std::endl;
		}
	    } else if(itemcount == volatility5_idx ||
		      itemcount == volatility10_idx ||
		      itemcount == volatility20_idx ||
		      itemcount == volatility60_idx) {
		// volatility exception: it should only different when very small and upto 1e-5
		if(std::atof(itemd.c_str()) >= 1e-5 || std::atof(itemg.c_str()) >= 1e-5) {
		    BOOST_CHECK(std::abs(std::atof(itemd.c_str()) - std::atof(itemg.c_str())) < 1e-7);
		    if(std::abs(std::atof(itemd.c_str()) - std::atof(itemg.c_str())) >= 1e-7) {
			std::cout << lined << std::endl;
			std::cout << lineg << std::endl;
			std::cout << itemd << std::endl;
			std::cout << itemg << std::endl;
		    }
		}
	    } else {
		// require equal up to 1e-7: 
		BOOST_CHECK(std::abs(std::atof(itemd.c_str()) - std::atof(itemg.c_str())) < 1e-7);
		if(std::abs(std::atof(itemd.c_str()) - std::atof(itemg.c_str())) >= 1e-7) {
		    std::cout << lined << std::endl;
		    std::cout << lineg << std::endl;
		    std::cout << itemd << std::endl;
		    std::cout << itemg << std::endl;
		}
	    }
	    itemcount++;
	}
    }

}
