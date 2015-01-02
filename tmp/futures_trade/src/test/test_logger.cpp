//
// File:        $ test_logger.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/01/09 10:52:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2012 by Zhengzhang Futures Co. Ltd.

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE unit_test_logger

#include <unistd.h>
#include <thread>
#include <string>
#include <fstream>

using std::thread;
using std::getline;
using std::ifstream;

#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>

#include "../Logger.hpp"

using simulator::Logger;

Logger logger("./tmp/test.log");

void insertToLogger()
{
    logger.push("insertToLogger");
}

void sleepThenInsertToLogger(long usecond)
{
    usleep(usecond);
    logger.push("sleepThenInsertToLogger");
}

// Simply make sure that the file it logs is the same as we input.
BOOST_AUTO_TEST_CASE(logger_same_as_input)
{
    logger.start();
    logger.push("start");

    for(int ii=0; ii<500; ii++) {
	logger.push(boost::lexical_cast<string>( ii ));
    }

    thread thrd1(insertToLogger);
    thread thrd2(sleepThenInsertToLogger, 100000);

    sleep(2);

    for(int ii=500; ii<1000; ii++) {
	logger.push(boost::lexical_cast<string>( ii ));
    }

    logger.push("end");
    logger.end();

    thrd1.join();
    thrd2.join();

    ifstream fd;
    fd.open("./tmp/test.log", ifstream::in);
    int count = 0;
    string line;
    while (fd.is_open() && fd.good() && !fd.eof()) {
	getline(fd, line, '\n');
	if (count == 0) {
	    BOOST_CHECK(line == "start");
	}
	else if (count < 501) {
	    BOOST_CHECK(line == boost::lexical_cast<string>(count-1));
	}
	else if (count == 501) {
	    BOOST_CHECK(line == "insertToLogger");
	}
	else if (count == 502) {
	    BOOST_CHECK(line == "sleepThenInsertToLogger");
	}
	else if (count < 1003) {
	    BOOST_CHECK(line == boost::lexical_cast<string>(count-3));
	}
	else if (count == 1003) {
	    BOOST_CHECK(line == "end");
	}
	++count;
    }
    fd.close();
}
