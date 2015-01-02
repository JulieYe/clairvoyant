//
// File:        $ OrderInputActionCache.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/08/28 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include "OrderInputActionCache.hpp"

namespace simulator
{

    OrderInputActionCache::OrderInputActionCache()
    {}

    OrderInputActionCache::~OrderInputActionCache()
    {}

    bool OrderInputActionCache::find(int orderId, OrderInputActionCacheType::iterator & it)
    {
	it = content_.find(orderId);
	return (it == content_.end()) ? false : true;
    }

    void OrderInputActionCache::insert(CThostFtdcInputOrderActionField *pInputOrderAction)
    {
	content_[atoi(pInputOrderAction->OrderRef)] = *pInputOrderAction;
    }

    void OrderInputActionCache::erase(int orderId)
    {
	content_.erase(orderId);
    }

}

// ORDERINPUTACTIONCACHE_CPP
