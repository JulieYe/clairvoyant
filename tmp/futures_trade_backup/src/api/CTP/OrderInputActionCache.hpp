//
// File:        $ OrderInputActionCache.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/08/28 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef ORDERINPUTACTIONCACHE_HPP
#define ORDERINPUTACTIONCACHE_HPP

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

    typedef map<int, CThostFtdcInputOrderActionField> OrderInputActionCacheType;

    class OrderInputActionCache
    {
    private:
	OrderInputActionCacheType content_;

    public:
	/* Constructor */
	OrderInputActionCache();

	/* Destructor */
	~OrderInputActionCache();

	/* Find */
	bool find(int orderId, OrderInputActionCacheType::iterator & it);

	/* Insert */
	void insert(CThostFtdcInputOrderActionField * pInputOrderAction);

	/* Erase */
	void erase(int orderId);
    };

}

#endif  // ORDERINPUTACTIONCACHE_HPP
