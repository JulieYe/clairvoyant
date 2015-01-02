#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <atomic>
#include <string>

struct FormattedPrintingStack
{
public:
    struct Message
    {
	Message() : next(0) {}
	Message(Message const&) = delete;
	Message& operator=(Message const&) = delete;

	Message(std::string str) : str_(str), next(0)
	{}

	~Message() { if (next) delete next; }

	void print(std::ostream& os) const { os << str_ << std::endl; }
	Message* next;
	std::string str_;
    };

public:
    FormattedPrintingStack() : head(0) {}

    void push(std::string str)
    {
	auto newMessage = new Message(str);
	newMessage->next = head.load();
	while (!head.compare_exchange_weak(newMessage->next, newMessage))
	    ;
    }

    bool empty() const { return head.load() == 0; }

    std::unique_ptr<Message> popAll()
    {
	Message* toReturn = head.load();
	//std::atomic<Message*> newHead(0);
	while (toReturn && !head.compare_exchange_weak(toReturn, 0))
	    ;
	return toReturn ? std::unique_ptr<Message>(toReturn) : std::unique_ptr<Message>();
    }

private:
    std::atomic<Message *> head;
};

#endif

