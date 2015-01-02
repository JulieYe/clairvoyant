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
#include "ConfigReader.hpp"

using simulator::ConfigReader;

BOOST_AUTO_TEST_CASE(config_reader)
{
    string filename("../test/test_configreader.config");
    ConfigReader cfr(filename);
    BOOST_CHECK(cfr.getString("Name") == "config_test");
    BOOST_CHECK(cfr.getInteger("Int") == 108);
    BOOST_CHECK(cfr.getDouble("Double") == 1.2);
}

BOOST_AUTO_TEST_CASE(config_reader_write)
{
    string filename("../test/test_configreader.config");
    string newfile("../test/test_configreader.test.config");
    ConfigReader cfg(filename);
    cfg.setValue("Int", 101);
    cfg.setValue("Double", 1.5);
    cfg.setValue("Name", "new");
    cfg.writeConfig(newfile);
    ConfigReader cfg2(newfile);
    BOOST_CHECK(cfg2.getString("Name") == "new");
    BOOST_CHECK(cfg2.getInteger("Int") == 101);
    BOOST_CHECK(cfg2.getDouble("Double") == 1.5);



}
