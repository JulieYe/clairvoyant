//
// File:        $ PrintingStack.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/05/29 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef PRINTINGSTACK_HPP
#define PRINTINGSTACK_HPP

#include <atomic>
#include <string>
#include <ostream>
#include <iostream>
#include <memory>

using std::atomic;
using std::ostream;
using std::string;
using std::cout;
using std::endl;
using std::unique_ptr;

namespace simulator
{

    struct PrintingStack
    {
    public:
	struct Message
	{
	    Message() : next(0) {}
	    Message(Message const&) = delete;
	    Message(Message &&) = delete;
	    Message& operator=(Message const&) = delete;
	    Message& operator=(Message &&) = delete;

	    Message(string content) : content_(content), next(0)
	    {}

	    ~Message() { if (next) delete next; }

	    void print(ostream& os) const { os << content_ << endl; }

	    string content_;
	    Message* next;
	};

    public:
	PrintingStack() : head(0) {}

	void push(string message)
	{
	    auto newMessage = new Message(message);
	    newMessage->next = head.load();
	    while (!head.compare_exchange_weak(newMessage->next, newMessage))
		;
	}

	bool empty() const { return head.load() == 0; }

	unique_ptr<Message> popAll()
	{
	    Message* toReturn = head.load();
	    while (toReturn && !head.compare_exchange_weak(toReturn, 0))
		;
	    return toReturn ? unique_ptr<Message>(toReturn) : unique_ptr<Message>();
	}

    private:
	atomic<Message *> head;
    };

}

#endif  // PRINTINGSTACK_HPP
