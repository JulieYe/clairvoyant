//
// File:        $ Logger.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/05/29 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <algorithm>
#include <functional>
#include <fstream>
#include <vector>
#include <thread>

using std::swap;
using std::function;
using std::ofstream;
using std::vector;
using std::thread;

#include "PrintingStack.hpp"

using simulator::PrintingStack;

namespace simulator
{

  class Logger
  {
  private:
      ofstream os;
      PrintingStack stack;
      atomic<bool> shutdown;
      function<void()> fnxn;
      thread workingthread;
      
  public:
      Logger(string logFileName);
      ~Logger();
      
      inline void push(const string & str) { stack.push(str); }
      
      void start();
      void end();
  };

}

#endif    // LOGGER_HPP
