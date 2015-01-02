//
// File:        $ test_configreader.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/05/07 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE unit_test_px
#include <boost/test/unit_test.hpp>
#include "../ConfigReader.hpp"

using simulator::ConfigReader;

BOOST_AUTO_TEST_CASE(config_reader)
{
    string filename("./test/test_configreader.config");
    ConfigReader cfr(filename);
    BOOST_CHECK(cfr.getString("Name") == "config_test");
    BOOST_CHECK(cfr.getInteger("Int") == 108);
    BOOST_CHECK(cfr.getDouble("Double") == 1.2);
}

