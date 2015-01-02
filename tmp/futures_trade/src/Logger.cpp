//
// File:        $ Logger.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/05/29 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include "Logger.hpp"

namespace simulator
{
    Logger::Logger(string logFileName) 
	: shutdown(false)
    {
	os.open(logFileName);

	fnxn = [&] ()
	{
	    while (!stack.empty() || !shutdown.load()) {
		unique_ptr<PrintingStack::Message> lastMessage = stack.popAll();
		if (lastMessage) {
		    vector<PrintingStack::Message *> contiguousMessages;
		    for (PrintingStack::Message * itr = lastMessage.get(); itr != 0; itr = itr->next) { 
			contiguousMessages.push_back(itr); 
		    }
		    
		    for (auto itr = contiguousMessages.rbegin();
			 itr != contiguousMessages.rend();
			 ++itr) {
			(*itr)->print(os);
		    }
		}
	    }
	};
    }
    
    Logger::~Logger() 
    {
	if(os.is_open()) {
	    os.close();
	}
    }

    void Logger::start() 
    {
	thread th([&] () { fnxn(); });
	swap(workingthread, th);
    }

    void Logger::end()
    {
	shutdown.store(true);
	workingthread.join();
    }
}

// LOGGER_CPP
