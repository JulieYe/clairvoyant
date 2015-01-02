//
// File:        $ Converter.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/01/15 14:17:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include "Converter.hpp"

namespace simulator
{

    string combineToTimestampString(TThostFtdcDateType const tradingDay, TThostFtdcTimeType const updateTime, TThostFtdcMillisecType const updateMillisec)
    {
	char nanosec[10];
	sprintf(nanosec, "%09d", updateMillisec * 1000000);

	char ret[30];
	memcpy(ret, tradingDay, 4);
	memset(ret + 4, '-', 1);
	memcpy(ret + 5, tradingDay + 4 * sizeof(char), 2);
	memset(ret + 7, '-', 1);
	memcpy(ret + 8, tradingDay + 6 * sizeof(char), 2);
	memset(ret + 10, 'T', 1);
	memcpy(ret + 11, updateTime, 8);
	memset(ret + 19, '.', 1);
	memcpy(ret + 20, nanosec, 9);
	memset(ret + 29, '\0', 1);

	return ret;
    }

}
