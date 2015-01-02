//
// File:        $ OrderInputCache.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/08/22 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include "OrderInputCache.hpp"

namespace simulator
{

    OrderInputCache::OrderInputCache()
    {}

    OrderInputCache::~OrderInputCache()
    {}

    bool OrderInputCache::find(int orderId, OrderInputCacheType::iterator & it)
    {
	it = content_.find(orderId);
	return (it == content_.end()) ? false : true;
    }

    void OrderInputCache::insert(CThostFtdcInputOrderField *pInputOrder)
    {
	content_[atoi(pInputOrder->OrderRef)] = *pInputOrder;
    }

    void OrderInputCache::erase(int orderId)
    {
	content_.erase(orderId);
    }

}

// ORDERINPUTCACHE_CPP
