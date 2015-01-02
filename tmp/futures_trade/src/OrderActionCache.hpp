//
// File:        $ OrderActionCache.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/08/15 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef ORDERACTIONCACHE_HPP
#define ORDERACTIONCACHE_HPP

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

    typedef map<int, CThostFtdcOrderActionField> OrderActionCacheType;

    class OrderActionCache
    {
    private:
	OrderActionCacheType content_;

    public:
	/* Constructor */
	OrderActionCache();

	/* Destructor */
	~OrderActionCache();

	/* Find */
	bool find(int orderId, OrderActionCacheType::iterator & it);

	/* Insert */
	void insert(CThostFtdcOrderActionField * pOrderAction);

	/* Erase */
	void erase(int orderId);

	// FUCK CFFEX double reject cancel limit order (send the same message twice in a row).
	bool diffQ(CThostFtdcOrderActionField *, CThostFtdcOrderActionField *);
    };

}

#endif  // ORDERACTIONCACHE_HPP
