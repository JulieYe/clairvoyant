//
// File:        $ Message.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/05/24 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include "Message.hpp"
//#include <typeid>
#include <cstring>

namespace simulator
{

    std::ostream& operator<<(std::ostream& os, const BuySellSide& obj)
    {
	if(obj == BuySellSide::buy) {
	    os << 1;
	    return os;
	} 
	if(obj == BuySellSide::sell) {
	    os << -1;
	    return os;
	}
	if(obj == BuySellSide::unknown) {
	    os << 0;
	    return os;
	}

	return os;
    }
    
    string & AbstractMessage::exchangeId()
    {
	return exchangeId_;
    }

    string AbstractMessage::getExchangeId() const
    {
	return exchangeId_;
    }
    
    string & AbstractMessage::contractId()
    {
	return contractId_;
    }

    string AbstractMessage::getContractId() const
    {
	return contractId_;
    }
    
    time_stamp &
    AbstractMessage::timestamp()
    {
	return timestamp_;
    }

    time_stamp
    AbstractMessage::getTimestamp() const
    {
	return timestamp_;
    }

    void AbstractMessage::addLatency(duration_nano dur)
    {
	timestamp_ += dur;
    }
    
    int & AbstractMessage::stratId()
    {
	return stratId_;
    }

    int AbstractMessage::getStratId() const
    {
	return stratId_;
    }
    
    string AbstractMessage::getTicker() const
    {
	return exchangeId_ + ":" + contractId_;
    }
    
    long & AbstractMessage::messageId()
    {
	return messageId_;
    }

    long AbstractMessage::getMessageId() const
    {
	return messageId_;
    }
    
    int & AbstractMessage::orderId()
    {
	return orderId_;
    }

    int AbstractMessage::getOrderId() const
    {
	return orderId_;
    }

    AbstractMessage::AbstractMessage()
    {}    

    AbstractMessage::AbstractMessage(string exchangeId, string contractId, time_stamp timestamp, int stratId, long messageId, int orderId)
	: exchangeId_(exchangeId), contractId_(contractId), timestamp_(timestamp), stratId_(stratId), messageId_(messageId), orderId_(orderId)
    {}

    AbstractMessage::AbstractMessage(AbstractMessage const & other)
	: AbstractMessage(other.getExchangeId(),
			  other.getContractId(),
			  other.getTimestamp(),
			  other.getStratId(),
			  other.getMessageId(),
			  other.getOrderId())
    {}

    AbstractMessage::AbstractMessage(AbstractMessage && other)
	: AbstractMessage(other.getExchangeId(),
			  other.getContractId(),
			  other.getTimestamp(),
			  other.getStratId(),
			  other.getMessageId(),
			  other.getOrderId())
    {}

    AbstractMessage & AbstractMessage::operator=(AbstractMessage const & other)
    {
	this->exchangeId() = other.getExchangeId();
	this->contractId() = other.getContractId();
	this->timestamp() = other.getTimestamp();
	this->stratId() = other.getStratId();
	this->messageId() = other.getMessageId();
	this->orderId() = other.getOrderId();
	return *this;
    }

    AbstractMessage & AbstractMessage::operator=(AbstractMessage && other)
    {
	this->exchangeId() = other.getExchangeId();
	this->contractId() = other.getContractId();
	this->timestamp() = other.getTimestamp();
	this->stratId() = other.getStratId();
	this->messageId() = other.getMessageId();
	this->orderId() = other.getOrderId();
	return *this;
    }

    AbstractMessage::~AbstractMessage()
    {}

    void AbstractMessage::printMessage()
    {
	std::cout << "ExchangeId = " << getExchangeId() << "\n"
		  << "ContractId = " << getContractId() << "\n"
		  << "TimeStamp = " << getTimestamp() << "\n"
		  << "Ts = " << getTimestamp().time_since_epoch().count() << "\n"
		  << "StratId = " << getStratId() << "\n"
		  << "MessageId = " << getMessageId() << "\n"
		  << "OrderId = " << getOrderId() << std::endl;
    }
    
    string AbstractMessage::logMessage()
    {
	stringstream ss;
	ss << "ExchangeId=" << getExchangeId() << ","
	   << "ContractId=" << getContractId() << ","
	   << "TimeStamp=" << getTimestamp() << ","
	   << "Ts=" << getTimestamp().time_since_epoch().count() << ","
	   << "StratId=" << getStratId() << ","
	   << "MessageId=" << getMessageId() << ","
	   << "OrderId=" << getOrderId();
	return ss.str();
    }

    std::ostream& operator<<(std::ostream& os, const time_stamp& obj) 
    {
	os << convertTimePointToString(obj);
	return os;
    }

    time_stamp convertStringToTimePoint(string datetime)
    {
	char year[5];
	char mon[3];
	char mday[3];
	char hour[3];
	char min[3];
	char sec[3];
	char nanosec[10];
	
	strcpy(year, datetime.substr(0,4).c_str());
	strcpy(mon, datetime.substr(5,2).c_str());
	strcpy(mday, datetime.substr(8,2).c_str());
	strcpy(hour, datetime.substr(11,2).c_str());
	strcpy(min, datetime.substr(14,2).c_str());
	strcpy(sec, datetime.substr(17,2).c_str());
	strcpy(nanosec, datetime.substr(20).c_str());
	memset(nanosec + strlen(nanosec), '0', 9 - strlen(nanosec));
	memset(nanosec + 9, '\0', 1);
	
	tm dt;
	dt.tm_year = atoi(year) - 1900;
	dt.tm_mon = atoi(mon) - 1;
	dt.tm_mday = atoi(mday);
	dt.tm_hour = atoi(hour);
	dt.tm_min = atoi(min);
	dt.tm_sec = atoi(sec);
	dt.tm_isdst = -1;
	time_t tv = mktime(&dt);
	
	time_stamp tp(duration_nano(tv * duration_nano::period::den / duration_nano::period::num));
	duration_nano numNanoseconds(atoll(nanosec));
	tp += numNanoseconds;
	
	return tp;
    }

    string convertTimePointToString(time_stamp tp)
    {
	unsigned long long nanoSecondsEpoch = tp.time_since_epoch().count();

	char nanosec[10];
	sprintf(nanosec, "%09llu", nanoSecondsEpoch % 1000000000ULL);

	time_t tv = nanoSecondsEpoch / 1000000000ULL;
	tm *dt;
	dt = localtime(&tv);

	char year[5];
	char mon[3];
	char mday[3];
	char hour[3];
	char min[3];
	char sec[3];
	sprintf(year, "%d", dt->tm_year + 1900);
	sprintf(mon, "%02d", dt->tm_mon + 1);
	sprintf(mday, "%02d", dt->tm_mday);
	sprintf(hour, "%02d", dt->tm_hour);
	sprintf(min, "%02d", dt->tm_min);
	sprintf(sec, "%02d", dt->tm_sec);

	char ret[30];
	memcpy(ret, year, 4);
	memset(ret + 4, '-', 1);
	memcpy(ret + 5, mon, 2);
	memset(ret + 7, '-', 1);
	memcpy(ret + 8, mday, 2);
	memset(ret + 10, 'T', 1);
	memcpy(ret + 11, hour, 2);
	memset(ret + 13, ':', 1);
	memcpy(ret + 14, min, 2);
	memset(ret + 16, ':', 1);
	memcpy(ret + 17, sec, 2);
	memset(ret + 19, '.', 1);
	memcpy(ret + 20, nanosec, 9);
	memset(ret + 29, '\0', 1);

	return ret;
    }
    
    int & MarketOrderMessage::qty()
    {
	return qty_;
    }

    int MarketOrderMessage::getQty() const
    {
	return qty_;
    }
    
    BuySellSide & MarketOrderMessage::side()
    {
	return side_;
    }

    BuySellSide MarketOrderMessage::getSide() const
    {
	return side_;
    }

    MarketOrderMessage::MarketOrderMessage(string exchangeId, string contractId, time_stamp timestamp, int stratId, long messageId, int orderId, int qty, BuySellSide side) 
	: AbstractMessage(exchangeId, contractId, timestamp, stratId, messageId, orderId), qty_(qty), side_(side)
    {}

    MarketOrderMessage::MarketOrderMessage(MarketOrderMessage const & other) 
	: MarketOrderMessage(other.getExchangeId(), other.getContractId(), other.getTimestamp(), other.getStratId(), other.getMessageId(), other.getOrderId(), other.getQty(), other.getSide())
    {}

    MarketOrderMessage::MarketOrderMessage(MarketOrderMessage && other) 
	: MarketOrderMessage(other.getExchangeId(), other.getContractId(), other.getTimestamp(), other.getStratId(), other.getMessageId(), other.getOrderId(), other.getQty(), other.getSide())
    {}

    MarketOrderMessage & 
    MarketOrderMessage::operator=(MarketOrderMessage const & other)
    {
	this->exchangeId() = other.getExchangeId();
	this->contractId() = other.getContractId();
	this->timestamp() = other.getTimestamp();
	this->stratId() = other.getStratId();
	this->messageId() = other.getMessageId();
	this->orderId() = other.getOrderId();
	this->qty() = other.getQty();
	this->side() = other.getSide();
	return *this;
    }

    MarketOrderMessage & 
    MarketOrderMessage::operator=(MarketOrderMessage && other)
    {
	this->exchangeId() = other.getExchangeId();
	this->contractId() = other.getContractId();
	this->timestamp() = other.getTimestamp();
	this->stratId() = other.getStratId();
	this->messageId() = other.getMessageId();
	this->orderId() = other.getOrderId();
	this->qty() = other.getQty();
	this->side() = other.getSide();
	return *this;
    }

    MarketOrderMessage::~MarketOrderMessage()
    {}


    void MarketOrderMessage::printMessage()
    {
	std::cout << "MarketOrderMessage" << "\n"
		  << "Side = " << getSide() << "\n"
		  << "Qty = " << getQty() << "\n";
	AbstractMessage::printMessage();
    }

    string MarketOrderMessage::logMessage()
    {
	stringstream ss;
	ss << "MarketOrderMessage" << ","
	   << "Side=" << getSide() << ","
	   << "Qty=" << getQty() << ","
	   << AbstractMessage::logMessage();
	return ss.str();
    }


    void LimitOrderMessage::printMessage()
    {
	std::cout << "LimitOrderMessage" << "\n"
		  << "Px = " << getPx() << "\n"
		  << "Side = " << getSide() << "\n"
		  << "Qty = " << getQty() << "\n";
	AbstractMessage::printMessage();
    }

    string LimitOrderMessage::logMessage()
    {
	stringstream ss;
	ss << "LimitOrderMessage" << ","
	   << "Px=" << getPx() << ","
	   << "Side=" << getSide() << ","
	   << "Qty=" << getQty() << ","
	   << AbstractMessage::logMessage();
	return ss.str();
    }

    Px & LimitOrderMessage::px()
    {
	return px_;
    }

    Px LimitOrderMessage::getPx() const
    {
	return px_;
    }

    LimitOrderMessage::LimitOrderMessage(string exchangeId, string contractId, time_stamp timestamp, int stratId, long messageId, int orderId, int qty, BuySellSide side, Px px) 
	: MarketOrderMessage(exchangeId, contractId, timestamp, stratId, messageId, orderId, qty, side), px_(px)
    {}
  
    LimitOrderMessage::LimitOrderMessage(LimitOrderMessage const & other) 
	: LimitOrderMessage(other.getExchangeId(), other.getContractId(), other.getTimestamp(), other.getStratId(), other.getMessageId(), other.getOrderId(), other.getQty(), other.getSide(), other.getPx())
    {}

    LimitOrderMessage::LimitOrderMessage(LimitOrderMessage && other) 
	: LimitOrderMessage(other.getExchangeId(), other.getContractId(), other.getTimestamp(), other.getStratId(), other.getMessageId(), other.getOrderId(), other.getQty(), other.getSide(), other.getPx())
    {}

    LimitOrderMessage & LimitOrderMessage::operator=(LimitOrderMessage const & other)
    {
	this->exchangeId() = other.getExchangeId();
	this->contractId() = other.getContractId();
	this->timestamp() = other.getTimestamp();
	this->stratId() = other.getStratId();
	this->messageId() = other.getMessageId();
	this->orderId() = other.getOrderId();
	this->qty() = other.getQty();
	this->side() = other.getSide();
	this->px() = other.getPx();
	return *this;
    }

    LimitOrderMessage & LimitOrderMessage::operator=(LimitOrderMessage && other)
    {
	this->exchangeId() = other.getExchangeId();
	this->contractId() = other.getContractId();
	this->timestamp() = other.getTimestamp();
	this->stratId() = other.getStratId();
	this->messageId() = other.getMessageId();
	this->orderId() = other.getOrderId();
	this->qty() = other.getQty();
	this->side() = other.getSide();
	this->px() = other.getPx();
	return *this;
    }

    LimitOrderMessage::~LimitOrderMessage()
    {}

    bool & AcknowledgedOrderMessage::acknowledgeCancelQ()
    {
	return acknowledgeCancelQ_;
    }

    bool AcknowledgedOrderMessage::getAcknowledgeCancelQ() const
    {
	return acknowledgeCancelQ_;
    }

    AcknowledgedOrderMessage::AcknowledgedOrderMessage(string exchangeId, string contractId, time_stamp timestamp, int stratId, long messageId, int orderId, bool acknowledgeCancelQ)
	: AbstractMessage(exchangeId, contractId, timestamp, stratId, messageId, orderId), acknowledgeCancelQ_(acknowledgeCancelQ)
    {}
    
    AcknowledgedOrderMessage::AcknowledgedOrderMessage(AcknowledgedOrderMessage const & other)
	: AcknowledgedOrderMessage(other.getExchangeId(),
				   other.getContractId(),
				   other.getTimestamp(),
				   other.getStratId(),
				   other.getMessageId(),
				   other.getOrderId(),
				   other.getAcknowledgeCancelQ())
    {}

    AcknowledgedOrderMessage::AcknowledgedOrderMessage(AcknowledgedOrderMessage && other)
	: AcknowledgedOrderMessage(other.getExchangeId(),
				   other.getContractId(),
				   other.getTimestamp(),
				   other.getStratId(),
				   other.getMessageId(),
				   other.getOrderId(),
				   other.getAcknowledgeCancelQ())
    {}

    AcknowledgedOrderMessage & AcknowledgedOrderMessage::operator=(AcknowledgedOrderMessage const & other)
    {
        this->exchangeId() = other.getExchangeId();
	this->contractId() = other.getContractId();
	this->timestamp() = other.getTimestamp();
	this->stratId() = other.getStratId();
	this->messageId() = other.getMessageId();
	this->orderId() = other.getOrderId();
	this->acknowledgeCancelQ() = other.getAcknowledgeCancelQ();
	return *this;
    }

    AcknowledgedOrderMessage & AcknowledgedOrderMessage::operator=(AcknowledgedOrderMessage && other)
    {
        this->exchangeId() = other.getExchangeId();
	this->contractId() = other.getContractId();
	this->timestamp() = other.getTimestamp();
	this->stratId() = other.getStratId();
	this->messageId() = other.getMessageId();
	this->orderId() = other.getOrderId();
	this->acknowledgeCancelQ() = other.getAcknowledgeCancelQ();
	return *this;
    }

    AcknowledgedOrderMessage::~AcknowledgedOrderMessage()
    {}

    FilledOrderMessage::FilledOrderMessage(string exchangeId, string contractId, time_stamp timestamp, int stratId, long messageId, int orderId, int qty, BuySellSide side, Px px) 
	: LimitOrderMessage(exchangeId, contractId, timestamp, stratId, messageId, orderId, qty, side, px)
    {}
    
    FilledOrderMessage::FilledOrderMessage(FilledOrderMessage const & other) 
	: FilledOrderMessage(other.getExchangeId(), other.getContractId(), other.getTimestamp(), other.getStratId(), other.getMessageId(), other.getOrderId(), other.getQty(), other.getSide(), other.getPx())
    {}

    FilledOrderMessage::FilledOrderMessage(FilledOrderMessage && other) 
	: FilledOrderMessage(other.getExchangeId(), other.getContractId(), other.getTimestamp(), other.getStratId(), other.getMessageId(), other.getOrderId(), other.getQty(), other.getSide(), other.getPx())
    {}
    
    FilledOrderMessage & FilledOrderMessage::operator=(FilledOrderMessage const & other)
    {
        this->exchangeId() = other.getExchangeId();
	this->contractId() = other.getContractId();
	this->timestamp() = other.getTimestamp();
	this->stratId() = other.getStratId();
	this->messageId() = other.getMessageId();
	this->orderId() = other.getOrderId();
	this->qty() = other.getQty();
	this->side() = other.getSide();
	this->px() = other.getPx();
	return *this;
    }

    FilledOrderMessage & FilledOrderMessage::operator=(FilledOrderMessage && other)
    {
        this->exchangeId() = other.getExchangeId();
	this->contractId() = other.getContractId();
	this->timestamp() = other.getTimestamp();
	this->stratId() = other.getStratId();
	this->messageId() = other.getMessageId();
	this->orderId() = other.getOrderId();
	this->qty() = other.getQty();
	this->side() = other.getSide();
	this->px() = other.getPx();
	return *this;
    }
    
    FilledOrderMessage::~FilledOrderMessage()
    {}

    void FilledOrderMessage::printMessage() 
    {
	std::cout << "FilledOrderMessage" << "\n"
		  << "Px = " << getPx() << "\n"
		  << "Side = " << getSide() << "\n"
		  << "Qty = " << getQty() << "\n";
	AbstractMessage::printMessage();
    }
    
    string FilledOrderMessage::logMessage()
    {
	stringstream ss;
	ss << "FilledOrderMessage" << ","
	   << "Px=" << getPx() << ","
	   << "Side=" << getSide() << ","
	   << "Qty=" << getQty() << ","
	   << AbstractMessage::logMessage();
	return ss.str();
    }

    void AcknowledgedOrderMessage::printMessage()
    {
	std::cout << "AcknowledgedOrderMessage" << "\n"
		  << "AckCancel = " << acknowledgeCancelQ_ << "\n";
	AbstractMessage::printMessage();
    }

    string AcknowledgedOrderMessage::logMessage()
    {
	stringstream ss;
	ss << "AcknowledgedOrderMessage" << ","
	   << "AckCancel=" << acknowledgeCancelQ_ << ","
	   << AbstractMessage::logMessage();
	return ss.str();

    }
    
}
