//
// File:        $ OrderInputCache.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/08/22 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef ORDERINPUTCACHE_HPP
#define ORDERINPUTCACHE_HPP

#include <cstdlib>
#include <cstring>
#include <map>

using std::atoi;
using std::memcmp;
using std::map;

// #include "ThostFtdcMdApi.h"
// #include "ThostFtdcTraderApi.h"
#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcUserApiStruct.h"

namespace simulator
{

    typedef map<int, CThostFtdcInputOrderField> OrderInputCacheType;

    class OrderInputCache
    {
    private:
	OrderInputCacheType content_;

    public:
	/* Constructor */
	OrderInputCache();

	/* Destructor */
	~OrderInputCache();

	/* Find */
	bool find(int orderId, OrderInputCacheType::iterator & it);

	/* Insert */
	void insert(CThostFtdcInputOrderField *);

	/* Erase */
	void erase(int orderId);
    };

}

#endif  // ORDERINPUTCACHE_HPP
