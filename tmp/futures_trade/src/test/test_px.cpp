//
// File:        $ test_px.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/10/08 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE unit_test_px
#include <boost/test/unit_test.hpp>

#include <cstdlib>
#include <time.h>
#include <algorithm>
#include <vector>
#include <stdexcept>

#include "../Px.hpp"

using simulator::Px;

// according to unit.test.to.be.written.px
//Mostly make sure numTicks_ describes the interger of tickSize_ / rawPx_.

// 1. Test copy constructor, assignment operator.
BOOST_AUTO_TEST_CASE(px_copy_constructor_assignment_operator)
{
  Px price(100.5, 0.1);
  Px price2(price);
  Px price3 = price;

  BOOST_CHECK(price2.getPx() == price.getPx() && price2.getNumTicks() == price.getNumTicks() 
	      && price2.getTickSize() == price.getTickSize());
  BOOST_CHECK(price3.getPx() == price.getPx() && price3.getNumTicks() == price.getNumTicks() 
	      && price3.getTickSize() == price.getTickSize());
}

// 2. Test setPx, setTickSize methods so that numTicks_ is changed accordingly.
BOOST_AUTO_TEST_CASE(px_setpx_setticksize)
{
  Px price(100.14001, 0.02);
  BOOST_CHECK(price.getNumTicks() == 5007);
  price.setPx(100.16);
  BOOST_CHECK(price.getNumTicks() == 5008);
  price.setTickSize(0.01);
  BOOST_CHECK(price.getNumTicks() == 10016);
}


// 3. Create a huge vector of Px's and tested the comparison methods using sort algorithm.
BOOST_AUTO_TEST_CASE(px_comparison)
{
  using std::rand;
  using std::srand;
  using std::vector;
  //using std::time;

  srand ( time(NULL) );
  double ticksize = 0.25;
  vector<Px> px_vec;
  for(int ii=0; ii<1000; ii++) {
      px_vec.push_back(Px((rand() % 5000)*ticksize, ticksize));
  }
  // CHECK: why is sort not working????
  // Add move constructor and move assignment operator overloading to Px class,
  // both std::swap and std::sort work now (fye)
  //std::cout << px_vec[0].getNumTicks() << std::endl;
  //std::cout << px_vec[1].getNumTicks() << std::endl;
  //std::cout << std::boolalpha << (px_vec[0] < px_vec[1]) << std::endl;
  std::sort(px_vec.begin(), px_vec.end());
  // std::sort(px_vec.begin(), px_vec.end(), simulator::operator<);
  // std::sort(px_vec.begin(), px_vec.end(), [&](Px const &l, Px const &r) { return l.getNumTicks() < r.getNumTicks(); });
  for(int ii=0; ii<999; ii++) {
      BOOST_CHECK(px_vec[ii] <= px_vec[ii+1]);
  }

}

// 4. Test rounding.
BOOST_AUTO_TEST_CASE(px_rounding)
{
    BOOST_CHECK(Px::round(2200, 0.2, 1) == Px(2200, 0.2));
    BOOST_CHECK(Px::round(2430.2, 0.2, -1) == Px(2430.2, 0.2));
    BOOST_CHECK(Px::round(2200.001, 0.2, 1) == Px(2200.2, 0.2));
    BOOST_CHECK(Px::round(2200.001, 0.2, -1) == Px(2200, 0.2));
    BOOST_CHECK(Px::round(2200.1, 0.2, 1) == Px(2200.2, 0.2));
    BOOST_CHECK(Px::round(2200.1, 0.2, -1) == Px(2200, 0.2));
    BOOST_CHECK(Px::round(2200.199, 0.2, 1) == Px(2200.2, 0.2));
    BOOST_CHECK(Px::round(2200.199, 0.2, -1) == Px(2200, 0.2));
}

// 5. Test overloaded plus and minus operators.
BOOST_AUTO_TEST_CASE(px_plus_minus_overloading)
{
    Px x(2100.2, 0.2);
    Px y(2101.4, 0.2);
    Px z(2000.4, 0.3);
    Px x_plus_y = x + y;
    Px x_minus_y = x - y;
    BOOST_CHECK(x_plus_y == Px(4201.6, 0.2));
    BOOST_CHECK(x_minus_y == Px(-1.2, 0.2));
    try {
	Px invalid_minus = x - z;
    }
    catch (std::exception const & e) {
	std::cout << "Exception: " << e.what() << std::endl;
    }
}
