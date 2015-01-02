//
// File:        $ IncludeHelper.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/08/26 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef INCLUDEHELPER_HPP
#define INCLUDEHELPER_HPP

#ifdef TICKERHEAD_IF
#include "IF.hpp"
namespace simulator { typedef IF TradingModel; }
#endif

#ifdef TICKERHEAD_RB
#include "RB.hpp"
namespace simulator { typedef RB TradingModel; }
#endif

#ifdef TICKERHEAD_RU
#include "RU.hpp"
namespace simulator { typedef RU TradingModel; }
#endif

#ifdef TICKERHEAD_CU
#include "CU.hpp"
namespace simulator { typedef CU TradingModel; }
#endif

#ifdef TICKERHEAD_AG
#include "AG.hpp"
namespace simulator { typedef AG TradingModel; }
#endif

#endif  // INCLUDEHELPER_HPP
