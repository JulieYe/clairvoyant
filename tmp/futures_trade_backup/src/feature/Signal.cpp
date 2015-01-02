//
// File:        $ Signal.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/02/02 14:21:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include "Signal.hpp"

namespace simulator
{

    BuySellSide Signal::getSignal() const
    {
	return signal_;
    }
    BuySellSide & Signal::signal()
    {
	return signal_;
    }

}

//  SIGNAL_CPP
