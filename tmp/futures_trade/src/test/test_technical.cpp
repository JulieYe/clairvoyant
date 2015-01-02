//
// File:        $ test_technical.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/04/18 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE unit_test_technical

#include <cstdlib>
#include <iostream>
#include <vector>

using std::cout;
using std::boolalpha;
using std::endl;
using std::vector;

#include <boost/test/unit_test.hpp>

#include "../Technical.hpp"

using simulator::LocalOptima;
using simulator::SupportResistence;
using simulator::time_stamp;
using simulator::duration_nano;
using simulator::convertStringToTimePoint;

#define TOL 1e-6

BOOST_AUTO_TEST_CASE(localoptima)
{
    vector<double> dvec = {1.0, 1.0, 2.0, 1.0, 3.0, 4.0, 3.0, 2.0, 10.0, 3.0, 5.0};
    LocalOptima lo1(1);
    LocalOptima lo2(2);
    const double ticksize = 1.0;

    for(unsigned int ii = 0; ii < dvec.size(); ii++) {
	lo1.update(dvec[ii], ticksize);
	lo2.update(dvec[ii], ticksize);
	if(ii == 0)  {
	    BOOST_CHECK(!lo1.isNewHigh() );
	    BOOST_CHECK(!lo1.isNewLow() ) ;
	    BOOST_CHECK(std::abs(lo1.recentLocalHigh() - (-1e9)) < TOL);
	    BOOST_CHECK(std::abs(lo1.recentLocalLow() - 1e9) < TOL);
	    BOOST_CHECK(!lo2.isNewHigh() );
	    BOOST_CHECK(!lo2.isNewLow() ) ;
	    BOOST_CHECK(std::abs(lo2.recentLocalHigh() - (-1e9)) < TOL);
	    BOOST_CHECK(std::abs(lo2.recentLocalLow() - 1e9) < TOL);
	}
	if(ii == 1)  {
	    BOOST_CHECK(!lo1.isNewHigh() );
	    BOOST_CHECK(!lo1.isNewLow() ) ;
	    BOOST_CHECK(std::abs(lo1.recentLocalHigh() - (-1e9)) < TOL);
	    BOOST_CHECK(std::abs(lo1.recentLocalLow() - 1e9) < TOL);
	    BOOST_CHECK(!lo2.isNewHigh() );
	    BOOST_CHECK(!lo2.isNewLow() ) ;
	    BOOST_CHECK(std::abs(lo2.recentLocalHigh() - (-1e9)) < TOL);
	    BOOST_CHECK(std::abs(lo2.recentLocalLow() - 1e9) < TOL);
	}
	if(ii == 2)  {
	    BOOST_CHECK(!lo1.isNewHigh() );
	    BOOST_CHECK(lo1.isNewLow() ) ;
	    BOOST_CHECK(std::abs(lo1.recentLocalHigh() - (-1e9)) < TOL);
	    BOOST_CHECK(std::abs(lo1.recentLocalLow() - 1.0) < TOL);
	    BOOST_CHECK(!lo2.isNewHigh() );
	    BOOST_CHECK(!lo2.isNewLow() ) ;
	    BOOST_CHECK(std::abs(lo2.recentLocalHigh() - (-1e9)) < TOL);
	    BOOST_CHECK(std::abs(lo2.recentLocalLow() - 1e9) < TOL);
	}
	if(ii == 3)  {
	    BOOST_CHECK(lo1.isNewHigh() );
	    BOOST_CHECK(!lo1.isNewLow() ) ;
	    BOOST_CHECK(std::abs(lo1.recentLocalHigh() - 2.0) < TOL);
	    BOOST_CHECK(std::abs(lo1.recentLocalLow() - 1.0) < TOL);
	    BOOST_CHECK(!lo2.isNewHigh() );
	    BOOST_CHECK(!lo2.isNewLow() ) ;
	    BOOST_CHECK(std::abs(lo2.recentLocalHigh() - (-1e9)) < TOL);
	    BOOST_CHECK(std::abs(lo2.recentLocalLow() - 1e9) < TOL);
	}
	if(ii == 4)  {
	    BOOST_CHECK(!lo1.isNewHigh() );
	    BOOST_CHECK(lo1.isNewLow() ) ;
	    BOOST_CHECK(std::abs(lo1.recentLocalHigh() - 2.0) < TOL);
	    BOOST_CHECK(std::abs(lo1.recentLocalLow() - 1.0) < TOL);
	    BOOST_CHECK(!lo2.isNewHigh() );
	    BOOST_CHECK(lo2.isNewLow() ) ;
	    BOOST_CHECK(std::abs(lo2.recentLocalHigh() - (-1e9)) < TOL);
	    BOOST_CHECK(std::abs(lo2.recentLocalLow() - 1.0) < TOL);
	}
	if(ii == 5)  {
	    BOOST_CHECK(!lo1.isNewHigh() );
	    BOOST_CHECK(!lo1.isNewLow() ) ;
	    BOOST_CHECK(std::abs(lo1.recentLocalHigh() - 2.0) < TOL);
	    BOOST_CHECK(std::abs(lo1.recentLocalLow() - 1.0) < TOL);
	    BOOST_CHECK(!lo2.isNewHigh() );
	    BOOST_CHECK(!lo2.isNewLow() ) ;
	    BOOST_CHECK(std::abs(lo2.recentLocalHigh() - (-1e9)) < TOL);
	    BOOST_CHECK(std::abs(lo2.recentLocalLow() - 1.0) < TOL);
	}
	if(ii == 6)  {
	    BOOST_CHECK(lo1.isNewHigh() );
	    BOOST_CHECK(!lo1.isNewLow() ) ;
	    BOOST_CHECK(std::abs(lo1.recentLocalHigh() - 4.0) < TOL);
	    BOOST_CHECK(std::abs(lo1.recentLocalLow() - 1.0) < TOL);
	    BOOST_CHECK(!lo2.isNewHigh() );
	    BOOST_CHECK(!lo2.isNewLow() ) ;
	    BOOST_CHECK(std::abs(lo2.recentLocalHigh() - (-1e9)) < TOL);
	    BOOST_CHECK(std::abs(lo2.recentLocalLow() - 1.0) < TOL);
	}
	if(ii == 7)  {
	    BOOST_CHECK(!lo1.isNewHigh() );
	    BOOST_CHECK(!lo1.isNewLow() ) ;
	    BOOST_CHECK(std::abs(lo1.recentLocalHigh() - 4.0) < TOL);
	    BOOST_CHECK(std::abs(lo1.recentLocalLow() - 1.0) < TOL);
	    BOOST_CHECK(lo2.isNewHigh() );
	    BOOST_CHECK(!lo2.isNewLow() ) ;
	    BOOST_CHECK(std::abs(lo2.recentLocalHigh() - 4.0) < TOL);
	    BOOST_CHECK(std::abs(lo2.recentLocalLow() - 1.0) < TOL);
	}
	if(ii == 8)  {
	    BOOST_CHECK(!lo1.isNewHigh() );
	    BOOST_CHECK(lo1.isNewLow() ) ;
	    BOOST_CHECK(std::abs(lo1.recentLocalHigh() - 4.0) < TOL);
	    BOOST_CHECK(std::abs(lo1.recentLocalLow() - 2.0) < TOL);
	    BOOST_CHECK(!lo2.isNewHigh() );
	    BOOST_CHECK(lo2.isNewLow() ) ;
	    BOOST_CHECK(std::abs(lo2.recentLocalHigh() - 4.0) < TOL);
	    BOOST_CHECK(std::abs(lo2.recentLocalLow() - 2.0) < TOL);
	}
	if(ii == 9)  {
	    BOOST_CHECK(lo1.isNewHigh() );
	    BOOST_CHECK(!lo1.isNewLow() ) ;
	    BOOST_CHECK(std::abs(lo1.recentLocalHigh() - 10.0) < TOL);
	    BOOST_CHECK(std::abs(lo1.recentLocalLow() - 2.0) < TOL);
	    BOOST_CHECK(lo2.isNewHigh() );
	    BOOST_CHECK(!lo2.isNewLow() ) ;
	    BOOST_CHECK(std::abs(lo2.recentLocalHigh() - 10.0) < TOL);
	    BOOST_CHECK(std::abs(lo2.recentLocalLow() - 2.0) < TOL);
	}
	if(ii == 10)  {
	    BOOST_CHECK(!lo1.isNewHigh() );
	    BOOST_CHECK(lo1.isNewLow() ) ;
	    BOOST_CHECK(std::abs(lo1.recentLocalHigh() - 10.0) < TOL);
	    BOOST_CHECK(std::abs(lo1.recentLocalLow() - 3.0) < TOL);
	    BOOST_CHECK(!lo2.isNewHigh() );
	    BOOST_CHECK(lo2.isNewLow() ) ;
	    BOOST_CHECK(std::abs(lo2.recentLocalHigh() - 10.0) < TOL);
	    BOOST_CHECK(std::abs(lo2.recentLocalLow() - 3.0) < TOL);
	}

    }


}


BOOST_AUTO_TEST_CASE(support_resistence)
{
    vector<double> dvec = {1.0, 2.0, 3.0, 4.0, 3.0, 2.0, 1.0, 2.0, 3.0, 4.0,
			   3.0, 2.0, 1.0, 3.0, 4.0, 5.0, 6.0, 7.0, 6.0, 5.0,
			   4.0, 3.0, 2.0, 1.0, 0.0, 3.0, 7.0, 9.0, 8.0, 5.0};
    SupportResistence sr1(10, 1);
    SupportResistence sr2(13, 2);
    const double ticksize = 1.0;
    time_stamp  start_time = convertStringToTimePoint("2013-01-01T09:00:00.000000000");
    
    for(int ii=0; ii < 30; ii++) {
	time_stamp ts = start_time + duration_nano(ii * 1000000000ULL);
	sr1.update(ts, dvec[ii], ticksize);
	if(ii >= 10 && ii <= 11 ) {
	    BOOST_CHECK(sr1.isSupportEstablished());
	    BOOST_CHECK(std::abs(sr1.supportLevel() - 1.0) < TOL);
	    BOOST_CHECK(sr1.supportTouchCount() == 2);
	} else if (ii >= 13 && ii <= 17) {
	    BOOST_CHECK(sr1.isSupportEstablished());
	    BOOST_CHECK(std::abs(sr1.supportLevel() - 1.0) < TOL);
	    BOOST_CHECK(sr1.supportTouchCount() == 2);
	} else {
	    BOOST_CHECK(!sr1.isSupportEstablished());
	}

	if (ii >= 10 && ii <= 14) {
	    BOOST_CHECK(sr1.isResistenceEstablished());
	    BOOST_CHECK(std::abs(sr1.resistenceLevel() - 4.0) < TOL);
	    BOOST_CHECK(sr1.resistenceTouchCount() == 2);
	} else {
	    BOOST_CHECK(!sr1.isResistenceEstablished());
	}	    

	sr2.update(ts, dvec[ii], ticksize);
	if(ii >= 13 && ii <= 15) {
	    BOOST_CHECK(sr2.isSupportEstablished());
	    BOOST_CHECK(std::abs(sr2.supportLevel() - 1.0) < TOL);
	    BOOST_CHECK(sr2.supportTouchCount() == 3);
	} else if(ii >= 16 && ii <= 21) {
	    BOOST_CHECK(sr2.isSupportEstablished());
	    BOOST_CHECK(std::abs(sr2.supportLevel() - 1.0) < TOL);
	    BOOST_CHECK(sr2.supportTouchCount() == 2);
	} else {
	    BOOST_CHECK(!sr2.isSupportEstablished());
	}

	if(ii >= 13 && ii <= 14) {
	    BOOST_CHECK(sr2.isResistenceEstablished());
	    BOOST_CHECK(std::abs(sr2.resistenceLevel() - 4.0) < TOL);
	    BOOST_CHECK(sr2.resistenceTouchCount() == 2);
	} else {
	    BOOST_CHECK(!sr2.isResistenceEstablished());
	}

    }
    





}







