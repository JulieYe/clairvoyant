//
// File:        $ Logger.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2012/12/19 09:36:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2012 by Zhengzhang Futures Co. Ltd.

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <thread>
#include <mutex>
#include <deque>
#include <condition_variable>
#include <string>
#include <iostream>
#include <fstream>

using std::deque;
using std::string;
using std::ofstream;

namespace simulator
{

  class Logger
  {
  private:
    deque<string> stringQueue_;
    std::mutex queueMutex_;
    std::mutex writeMutex_;
    std::condition_variable condVar_;

    string logFileName_;
    ofstream logFile_;
    bool end_;
    bool wakeWriter_;

    std::thread writeThread_;

  public:
    Logger(string logFileName);
    ~Logger();

    void push(const string & str);
    string pop();
    bool empty();

    void start();
    void write();
    void end();
  };

}

#endif    // LOGGER_HPP
