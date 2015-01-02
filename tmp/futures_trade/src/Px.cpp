//
// File:        $ Px.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/10/08 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include "Px.hpp"

using simulator::Px;

namespace simulator
{

    Px::Px(double rawPx, double tickSize)
	: tickSize_(tickSize), rawPx_(rawPx)
    {
	numTicks_ = rawPx_ / tickSize_ + 0.5;
    }

    Px::Px() : Px(0.0, 0.1)
    {}
  
    Px::Px(Px const & px) : Px(px.getPx(), px.getTickSize())
    {}

    Px::Px(Px && px) : Px(px.getPx(), px.getTickSize())
    {}

    Px & Px::operator=(Px const & px)
    {
	rawPx_ = px.getPx();
	tickSize_ = px.getTickSize();
	numTicks_ = rawPx_ / tickSize_ + 0.5;
	return *this;
    }

    Px & Px::operator=(Px && px)
    {
	rawPx_ = px.getPx();
	tickSize_ = px.getTickSize();
	numTicks_ = rawPx_ / tickSize_ + 0.5;
	return *this;
    }

    Px::~Px()
    {}

    double Px::getPx() const
    {
	return rawPx_;
    }
    
    long Px::getNumTicks() const
    {
	return numTicks_;
    }
    
    double Px::getTickSize() const
    {
	return tickSize_;
    }

    void Px::setPx(double rawPx)
    {
	rawPx_ = rawPx;
	numTicks_ = rawPx_ / tickSize_ + 0.5;
    }

    void Px::setTickSize(double tickSize)
    {
	tickSize_ = tickSize;
	numTicks_ = rawPx_ / tickSize_ + 0.5;
    }

    Px Px::round(double rawPx, double tickSize, int upOrDown)
    {
	Px px(rawPx, tickSize);

	if (!(fabs(fmod(rawPx, tickSize)) <= 1e-6) && !(fabs(fmod(rawPx, tickSize)-tickSize) <= 1e-6)) {
	    int numTicksInt = rawPx / tickSize;
	    switch(upOrDown) {
	    case -1:
		px.setPx(tickSize * numTicksInt);
		break;
	    case 1:
		px.setPx(tickSize * (numTicksInt + 1));
		break;
	    default:
		;
	    }
	}

	return px;
    }

    // bool operator==(Px & px_l, Px & px_r)
    // {
    // 	return px_l.getNumTicks() == px_r.getNumTicks();
    // }
    
    // bool operator!=(Px & px_l, Px & px_r)
    // {
    // 	return !(px_l == px_r);
    // }

    // bool operator< (Px & px_l, Px & px_r)
    // {
    // 	return px_l.getNumTicks() < px_r.getNumTicks();
    // }

    // bool operator<=(Px & px_l, Px & px_r)
    // {
    // 	return (px_l < px_r) || (px_l == px_r);
    // }
    
    // bool operator> (Px & px_l, Px & px_r)
    // {
    // 	return px_l.getNumTicks() > px_r.getNumTicks();
    // }
    
    // bool operator>=(Px & px_l, Px & px_r)
    // {
    // 	return (px_l > px_r) || (px_l == px_r);
    // }

    bool operator==(Px const px_l, Px const px_r)
    {
	return px_l.getNumTicks() == px_r.getNumTicks();
    }
    
    bool operator!=(Px const px_l, Px const px_r)
    {
	return !(px_l == px_r);
    }

    bool operator< (Px const px_l, Px const px_r)
    {
	return px_l.getNumTicks() < px_r.getNumTicks();
    }

    bool operator<=(Px const px_l, Px const px_r)
    {
	return (px_l < px_r) || (px_l == px_r);
    }
    
    bool operator> (Px const px_l, Px const px_r)
    {
	return px_l.getNumTicks() > px_r.getNumTicks();
    }
    
    bool operator>=(Px const px_l, Px const px_r)
    {
	return (px_l > px_r) || (px_l == px_r);
    }

    Px operator+(Px const px_l, Px const px_r)
    {
	if (fabs(px_l.getTickSize()-px_r.getTickSize()) >= 1e-8) {
	    throw runtime_error("Px's with different tick sizes cannot be added.");
	} else{
	    return Px(px_l.getPx()+px_r.getPx(), px_l.getTickSize());  
	}
    }

    Px operator-(Px const px_l, Px const px_r)
    {
	if (fabs(px_l.getTickSize()-px_r.getTickSize()) >= 1e-8) {
	    throw runtime_error("Px's with different tick sizes cannot be substracted.");
	} else{
	    return Px(px_l.getPx()-px_r.getPx(), px_l.getTickSize());  
	}
    }

    std::ostream& operator<<(std::ostream& os, const Px& obj) 
    {
	os << obj.getPx();
	return os;
    }

}

// PX_CPP
