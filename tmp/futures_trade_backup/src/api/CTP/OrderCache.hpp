//
// File:        $ OrderCache.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/11/22 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef ORDERCACHE_HPP
#define ORDERCACHE_HPP

#include <cstdlib>
#include <cstring>
#include <map>

using std::atoi;
using std::memset;
using std::memcmp;
using std::map;

// #include "ThostFtdcMdApi.h"
// #include "ThostFtdcTraderApi.h"
#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcUserApiStruct.h"

namespace simulator
{

    typedef map<int, CThostFtdcOrderField> OrderCacheType;

    class OrderCache
    {
    private:
	OrderCacheType content_;

    public:
	/* Constructor */
	OrderCache();

	/* Destructor */
	~OrderCache();

	/* Find */
	bool find(int orderId, OrderCacheType::iterator & it);

	/* Insert */
	void insert(CThostFtdcOrderField * pOrder);

	/* Erase */
	void erase(int orderId);

	/* Does this order field correpond to a cancel order? */
	// Scooped out by try-and-test, damn API...
	// FUCK CFFEX double acknowledged limit order (the only different is SequenceNo increases!).
	inline bool cancelOrderQ(CThostFtdcOrderField * pOrder) { return (pOrder->ActiveUserID[0]) != '\0'; }

	inline bool canceledOrderQ(CThostFtdcOrderField * pOrder) { return pOrder->OrderStatus == THOST_FTDC_OST_Canceled; }

	inline void resetActiveUserID(CThostFtdcOrderField * pOrder) { memset(&(pOrder->ActiveUserID), 0, sizeof(TThostFtdcUserIDType)); }

	// bool diffOnlyAtSequenceNoQ(CThostFtdcOrderField *, CThostFtdcOrderField *);

	bool diffQ(CThostFtdcOrderField *, CThostFtdcOrderField *);
    };

}

#endif  // ORDERCACHE_HPP
