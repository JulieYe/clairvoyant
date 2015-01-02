#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "FormattedPrintingStack.hpp"

#include <algorithm>
#include <functional>
#include <fstream>
#include <vector>
#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>

std::unique_ptr<std::ofstream> createLogFile(std::string fileName);

class LocklessLogger
{
public:
    LocklessLogger(std::ostream& os_);
    
    void push(std::string str);
    
    void finalize();
    
    void operator()();

private:
	std::ostream& os;
	FormattedPrintingStack stack;
	std::atomic<bool> shutdown;
	std::function<void()> fnxn;
};

inline LocklessLogger::LocklessLogger(std::ostream& os_)
	: os(os_)
	, shutdown(false)
{
	fnxn = [&]()
	{
	    // using namespace range;
	    while (!stack.empty() || !shutdown.load())
		{
		    std::unique_ptr<FormattedPrintingStack::Message>
		    lastMessage = stack.popAll();
		    if (lastMessage)
			{
			    std::vector<FormattedPrintingStack::Message*> contiguousMessages;
			    
			    for (FormattedPrintingStack::Message* itr = lastMessage.get();
				 itr != 0; itr = itr->next)
				{ contiguousMessages.push_back(itr); }
			    
			    // contiguousMessages
			    // 	| reverse()
			    // 	| apply([&](FormattedPrintingStack::Message* msg){ msg->print(os); });

			    for (auto it = contiguousMessages.rbegin();
				 it != contiguousMessages.rend();
				 ++it) {
				(*it)->print(os);
			    }
			}
		}
	};
}

inline void LocklessLogger::push(std::string str)
{ stack.push(str); }

inline void LocklessLogger::finalize() { shutdown.store(true); }
inline void LocklessLogger::operator()() { fnxn(); }

inline std::unique_ptr<std::ofstream> createLogFile(std::string fileName)
{
    // boost::posix_time::time_facet* t_facet(new boost::posix_time::time_facet(""));
    // t_facet->set_iso_format();
    std::unique_ptr<std::ofstream> logFile(new std::ofstream(fileName));
    // logFile->imbue(std::locale(logFile->getloc(), t_facet));
    return logFile;
}

#endif

