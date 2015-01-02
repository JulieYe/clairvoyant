//
// File:        $ Logger.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2012/12/26 12:37:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include "Logger.hpp"

namespace simulator
{

    void Logger::push(const string & str) {
        std::lock_guard<std::mutex> lock(queueMutex_);
	stringQueue_.push_back(str);
	if(!wakeWriter_) {
	    std::unique_lock<std::mutex> lock2(writeMutex_);
	    wakeWriter_ = true;
	    lock2.unlock();
	    condVar_.notify_one();
	} 
    }
    
    string Logger::pop() {
        std::lock_guard<std::mutex> lock(queueMutex_);
	string str(stringQueue_.front());
	stringQueue_.pop_front();
	return str;
    }

    Logger::Logger(string logFileName) : 
	logFileName_(logFileName),
	end_(false),
	wakeWriter_(false)
    { 
	logFile_.open(logFileName_);
	// here: check if file opens fine
    }

    bool Logger::empty() {
	std::lock_guard<std::mutex> log(queueMutex_);
	return stringQueue_.empty();
    }
    
    Logger::~Logger() 
    {
	if(logFile_.is_open()) {
	    logFile_.close();
	}
    }
    
    void Logger::start() 
    {
	std::thread th([&]() {write();});
	std::swap(writeThread_, th);
    }

    void Logger::write()
    {
	std::unique_lock<std::mutex> lok(writeMutex_);
	while(!end_) {
	    while(!wakeWriter_) {
		condVar_.wait(lok);
	    }
	    while(!empty()) {
		logFile_ << pop() << std::endl;
	    }
	    wakeWriter_ = false;
	}
    }

    void Logger::end()
    {
	std::unique_lock<std::mutex> lok(writeMutex_);
	wakeWriter_ = true; 
	end_ = true;
	lok.unlock();
	condVar_.notify_one();
	writeThread_.join();
    }

}

// LOGGER_CPP
