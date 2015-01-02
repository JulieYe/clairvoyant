//
// File:        $ Px.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/10/08 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef PX_HPP
#define PX_HPP

#include <iostream>
#include <cmath>
#include <stdexcept>

using std::fabs;
using std::fmod;
using std::runtime_error;

namespace simulator
{

    class Px
    {
    private:
	double tickSize_;
	double rawPx_;
	long numTicks_;  // derived by rawPx_ / tickSize_

    public:
	Px(double rawPx, double tickSize);

	/* deprecate default constructor */
        Px(); // = delete;

	/* copy constructor */
	Px(Px const &);

	/* move constructor */
	Px(Px &&);

	/* copy assignment operator overloading */
	Px & operator=(Px const &);

	/* move assignment operator overloading */
	Px & operator=(Px &&);

	/* default destructor */
	~Px();

	double getPx() const;
	long getNumTicks() const;
	double getTickSize() const;

	void setPx(double rawPx);
	void setTickSize(double tickSize);

	static Px round(double rawPx, double tickSize, int upOrDown);

	// friend bool operator==(Px &, Px &);
	// friend bool operator!=(Px &, Px &);
	// friend bool operator< (Px &, Px &);
	// friend bool operator<=(Px &, Px &);
	// friend bool operator> (Px &, Px &);
	// friend bool operator>=(Px &, Px &);

	friend bool operator==(Px const, Px const);
	friend bool operator!=(Px const, Px const);
	friend bool operator< (Px const, Px const);
	friend bool operator<=(Px const, Px const);
	friend bool operator> (Px const, Px const);
	friend bool operator>=(Px const, Px const);
	friend Px   operator+ (Px const, Px const);
	friend Px   operator- (Px const, Px const);
    };

    // bool operator==(Px &, Px &);
    // bool operator!=(Px &, Px &);
    // bool operator< (Px &, Px &);
    // bool operator<=(Px &, Px &);
    // bool operator> (Px &, Px &);
    // bool operator>=(Px &, Px &);

    std::ostream& operator<<(std::ostream& os, const Px& obj) ;

    bool operator==(Px const, Px const);
    bool operator!=(Px const, Px const);
    bool operator< (Px const, Px const);
    bool operator<=(Px const, Px const);
    bool operator> (Px const, Px const);
    bool operator>=(Px const, Px const);
    Px   operator+ (Px const, Px const);
    Px   operator- (Px const, Px const);

}

#endif    // PX_HPP
