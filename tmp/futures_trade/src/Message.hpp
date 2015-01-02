//
// File:        $ Message.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/10/14 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <vector>
#include <algorithm>
#include <cstdlib>

using std::stringstream;
using std::string;
using std::chrono::time_point;
using std::chrono::system_clock;
using std::chrono::duration;
using std::nano;
using std::vector;
using std::min;
using std::max;
using std::abs;

#include "Px.hpp"

using simulator::Px;

namespace simulator
{

    enum class BuySellSide { buy, sell, mid, unknown };
    enum class FrontBackSide { front, back, mid };
    typedef time_point<system_clock, duration<unsigned long long, nano> > time_stamp;
    typedef duration<unsigned long long, nano> duration_nano;

    time_stamp convertStringToTimePoint(string);
    string convertTimePointToString(time_stamp);

    std::ostream& operator<<(std::ostream& os, const time_stamp& obj) ;
    std::ostream& operator<<(std::ostream& os, const BuySellSide& obj) ;

    class AbstractMessage
    {
    protected:
	string exchangeId_;
	string contractId_;
	time_stamp timestamp_;
	int stratId_;
	long messageId_;
	int orderId_;

    public:
	/* accept default constructor */
	AbstractMessage();

	/* constructor helper */
	AbstractMessage(string, string, time_stamp, int, long, int);

	/* copy constructor */
	AbstractMessage(AbstractMessage const &);

	/* move constructor */
	AbstractMessage(AbstractMessage &&);

	/* copy assignment operator overloading */
	AbstractMessage & operator=(AbstractMessage const &);

	/* move assignment operator overloading */
	AbstractMessage & operator=(AbstractMessage &&);

	/* default destructor */
	virtual ~AbstractMessage();
	
	string & exchangeId();
	string getExchangeId() const;

	string & contractId();
	string getContractId() const;

	string getTicker() const;

	time_stamp & timestamp();
	time_stamp getTimestamp() const;
	void addLatency(duration_nano);

	int & stratId();
	int getStratId() const;

	long & messageId();
	long getMessageId() const;

	int & orderId();
	int getOrderId() const;

	virtual void printMessage();
	virtual string logMessage();

    };
   
    class MarketOrderMessage : public AbstractMessage
    {
    protected:
    	int qty_;
	BuySellSide side_;

    public:
	/* accept default constructor */
	MarketOrderMessage() { }

	/* constructor helper */
	MarketOrderMessage(string, string, time_stamp, int, long, int, int, BuySellSide);

	/* copy constructor */
	MarketOrderMessage(MarketOrderMessage const &);

	/* move constructor */
	MarketOrderMessage(MarketOrderMessage &&);

	/* copy assignment operator overloading */
	MarketOrderMessage & operator=(MarketOrderMessage const &);

	/* move assignment operator overloading */
	MarketOrderMessage & operator=(MarketOrderMessage &&);

	/* default destructor */
	virtual ~MarketOrderMessage();

	int & qty();
	int getQty() const;

	BuySellSide & side();
	BuySellSide getSide() const;
	
	virtual void printMessage();
	virtual string logMessage();
    };
    
    class LimitOrderMessage : public MarketOrderMessage
    {

    protected:
	Px px_;

    public:
	/* accept default constructor */
	LimitOrderMessage(){ }

	/* constructor helper */
	LimitOrderMessage(string, string, time_stamp, int, long, int, int, BuySellSide, Px);

	/* copy constructor */
	LimitOrderMessage(LimitOrderMessage const &);

	/* move constructor */
	LimitOrderMessage(LimitOrderMessage &&);

	/* copy assignment operator overloading */
	LimitOrderMessage & operator=(LimitOrderMessage const &);

	/* move assignment operator overloading */
	LimitOrderMessage & operator=(LimitOrderMessage &&);

	/* default destructor */
	virtual ~LimitOrderMessage();

	Px & px();
	Px getPx() const;

	virtual void printMessage();
	virtual string logMessage();
    };

    template<typename MARKETORLIMIT>
    class CancelOrderMessage;

    template<>
    class CancelOrderMessage<LimitOrderMessage> : public LimitOrderMessage
    {
    public:
	/* accept default constructor */
	CancelOrderMessage(){ }

	/* constructor helper */
	CancelOrderMessage(string exchangeId, string contractId, time_stamp timestamp, int stratId, long messageId, int orderId, int qty, BuySellSide side, Px px) : 
	    LimitOrderMessage(exchangeId, contractId, timestamp, stratId, messageId, orderId, qty, side, px)
	{}

	/* copy constructor */
	CancelOrderMessage(CancelOrderMessage const & other) : 
	    CancelOrderMessage(other.getExchangeId(), other.getContractId(), other.getTimestamp(), other.getStratId(), other.getMessageId(), other.getOrderId(), other.getQty(), other.getSide(), other.getPx())
	{}

	/* move constructor */
	CancelOrderMessage(CancelOrderMessage && other) : 
	    CancelOrderMessage(other.getExchangeId(), other.getContractId(), other.getTimestamp(), other.getStratId(), other.getMessageId(), other.getOrderId(), other.getQty(), other.getSide(), other.getPx())
	{}

	virtual void printMessage() 
	{
	    std::cout << "CancelLimitOrderMessage" << "\n"
		      << "Px = " << getPx() << "\n"
		      << "Side = " << getSide() << "\n"
		      << "Qty = " << getQty() << "\n";
	    AbstractMessage::printMessage();
	}

	virtual string logMessage()
	{
	    stringstream ss;
	    ss << "CancelLimitOrderMessage" << ","
	       << "Px=" << getPx() << ","
	       << "Side=" << getSide() << ","
	       << "Qty=" << getQty() << ","
	       << AbstractMessage::logMessage();
	    return ss.str();
	}

	/* copy assignment operator overloading */
	CancelOrderMessage & operator=(CancelOrderMessage const & other)
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

	/* move assignment operator overloading */
	CancelOrderMessage & operator=(CancelOrderMessage && other)
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

	/* default destructor */
	virtual ~CancelOrderMessage()
	{}
    };

    template<>
    class CancelOrderMessage<MarketOrderMessage> : public MarketOrderMessage
    {
    public:
	/* accept default constructor */
	CancelOrderMessage() { }

	/* constructor helper */
	CancelOrderMessage(string exchangeId, string contractId, time_stamp timestamp, int stratId, long messageId, int orderId, int qty, BuySellSide side) : 
	    MarketOrderMessage(exchangeId, contractId, timestamp, stratId, messageId, orderId, qty, side)
	{}

	/* copy constructor */
	CancelOrderMessage(CancelOrderMessage const & other) : 
	    CancelOrderMessage(other.getExchangeId(), other.getContractId(), other.getTimestamp(), other.getStratId(), other.getMessageId(), other.getOrderId(), other.getQty(), other.getSide())
	{}

	/* move constructor */
	CancelOrderMessage(CancelOrderMessage && other) : 
	    CancelOrderMessage(other.getExchangeId(), other.getContractId(), other.getTimestamp(), other.getStratId(), other.getMessageId(), other.getOrderId(), other.getQty(), other.getSide())
	{}

	/* copy assignment operator overloading */
	CancelOrderMessage & operator=(CancelOrderMessage const & other)
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

	/* move assignment operator overloading */
	CancelOrderMessage & operator=(CancelOrderMessage && other)
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


	virtual void printMessage() 
	{
	    std::cout << "CancelMarketOrderMessage" << "\n"
		      << "Side = " << getSide() << "\n"
		      << "Qty = " << getQty() << "\n";
	    AbstractMessage::printMessage();
	}

	virtual string logMessage()
	{
	    stringstream ss;
	    ss << "CancelMarketOrderMessage" << ","
	       << "Side=" << getSide() << ","
	       << "Qty=" << getQty() << ","
	       << AbstractMessage::logMessage();
	    return ss.str();
	}

	/* default destructor */
	virtual ~CancelOrderMessage()
	{}
    };

    template<typename MARKETORLIMIT>
    class CanceledOrderMessage;

    template<>
    class CanceledOrderMessage<LimitOrderMessage> : public LimitOrderMessage
    {
    public:
	/* accept default constructor */
	CanceledOrderMessage(){ }

	/* constructor helper */
	CanceledOrderMessage(string exchangeId, string contractId, time_stamp timestamp, int stratId, long messageId, int orderId, int qty, BuySellSide side, Px px) : 
	    LimitOrderMessage(exchangeId, contractId, timestamp, stratId, messageId, orderId, qty, side, px)
	{}

	/* copy constructor */
	CanceledOrderMessage(CanceledOrderMessage const & other) : 
	    CanceledOrderMessage(other.getExchangeId(), other.getContractId(), other.getTimestamp(), other.getStratId(), other.getMessageId(), other.getOrderId(), other.getQty(), other.getSide(), other.getPx())
	{}

	/* move constructor */
	CanceledOrderMessage(CanceledOrderMessage && other) : 
	    CanceledOrderMessage(other.getExchangeId(), other.getContractId(), other.getTimestamp(), other.getStratId(), other.getMessageId(), other.getOrderId(), other.getQty(), other.getSide(), other.getPx())
	{}

	/* copy assignment operator overloading */
	CanceledOrderMessage & operator=(CanceledOrderMessage const & other)
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

	/* move assignment operator overloading */
	CanceledOrderMessage & operator=(CanceledOrderMessage && other)
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

	virtual void printMessage() 
	{
	    std::cout << "CanceledLimitOrderMessage" << "\n"
		      << "Px = " << getPx() << "\n"
		      << "Side = " << getSide() << "\n"
		      << "Qty = " << getQty() << "\n";
	    AbstractMessage::printMessage();
	}

	virtual string logMessage()
	{
	    stringstream ss;
	    ss << "CanceledLimitOrderMessage" << ","
	       << "Px=" << getPx() << ","
	       << "Side=" << getSide() << ","
	       << "Qty=" << getQty() << ","
	       << AbstractMessage::logMessage();
	    return ss.str();
	}

	/* default destructor */
	virtual ~CanceledOrderMessage()
	{}
    };

    template<>
    class CanceledOrderMessage<MarketOrderMessage> : public MarketOrderMessage
    {
    public:
	/* accept default constructor */
	CanceledOrderMessage() { }

	/* constructor helper */
	CanceledOrderMessage(string exchangeId, string contractId, time_stamp timestamp, int stratId, long messageId, int orderId, int qty, BuySellSide side) : 
	    MarketOrderMessage(exchangeId, contractId, timestamp, stratId, messageId, orderId, qty, side)
	{}

	/* copy constructor */
	CanceledOrderMessage(CanceledOrderMessage const & other) : 
	    CanceledOrderMessage(other.getExchangeId(), other.getContractId(), other.getTimestamp(), other.getStratId(), other.getMessageId(), other.getOrderId(), other.getQty(), other.getSide())
	{}

	/* move constructor */
	CanceledOrderMessage(CanceledOrderMessage && other) : 
	    CanceledOrderMessage(other.getExchangeId(), other.getContractId(), other.getTimestamp(), other.getStratId(), other.getMessageId(), other.getOrderId(), other.getQty(), other.getSide())
	{}

	/* copy assignment operator overloading */
	CanceledOrderMessage & operator=(CanceledOrderMessage const & other)
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

	/* move assignment operator overloading */
	CanceledOrderMessage & operator=(CanceledOrderMessage && other)
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

	virtual void printMessage() 
	{
	    std::cout << "CanceledMarketOrderMessage" << "\n"
		      << "Side = " << getSide() << "\n"
		      << "Qty = " << getQty() << "\n";
	    AbstractMessage::printMessage();
	}

	virtual string logMessage()
	{
	    stringstream ss;
	    ss << "CanceledMarketOrderMessage" << ","
	       << "Side=" << getSide() << ","
	       << "Qty=" << getQty() << ","
	       << AbstractMessage::logMessage();
	    return ss.str();
	}

	/* default destructor */
	virtual ~CanceledOrderMessage()
	{}
    };
    
    class FilledOrderMessage : public LimitOrderMessage
    {
    public:
	/* accept default constructor */
	FilledOrderMessage() { }
	
	/* constructor helper */
	FilledOrderMessage(string, string, time_stamp, int, long, int, int, BuySellSide, Px);
	
	/* copy constructor */
	FilledOrderMessage(FilledOrderMessage const &);

	/* move constructor */
	FilledOrderMessage(FilledOrderMessage &&);
	
	/* copy assignment operator overloading */
	FilledOrderMessage & operator=(FilledOrderMessage const &);

	/* move assignment operator overloading */
	FilledOrderMessage & operator=(FilledOrderMessage &&);
	
	/* default destructor */
	virtual ~FilledOrderMessage();

	virtual void printMessage() ;

	virtual string logMessage();


    };
    
    class AcknowledgedOrderMessage : public AbstractMessage
    {
    protected:
	bool acknowledgeCancelQ_;

    public:
	/* accept default constructor */
	AcknowledgedOrderMessage() {}

	/* constructor helper */
	AcknowledgedOrderMessage(string, string, time_stamp, int, long, int, bool);

	/* copy constructor */
	AcknowledgedOrderMessage(AcknowledgedOrderMessage const &);

	/* move constructor */
	AcknowledgedOrderMessage(AcknowledgedOrderMessage &&);

	/* copy assignment operator overloading */
	AcknowledgedOrderMessage & operator=(AcknowledgedOrderMessage const &);

	/* move assignment operator overloading */
	AcknowledgedOrderMessage & operator=(AcknowledgedOrderMessage &&);

	bool & acknowledgeCancelQ();
	bool getAcknowledgeCancelQ() const;

	/* default destructor */
	virtual ~AcknowledgedOrderMessage();

	virtual void printMessage();
	virtual string logMessage();
    };

    // Rejected order message (on market order, limit order, cancel order)
    template<typename ORDERTYPE>
    class RejectedOrderMessage;

    /* Rejected order for limit order */
    template<>
    class RejectedOrderMessage<LimitOrderMessage> : public LimitOrderMessage
    {
    public:
	/* accept default constructor */
	RejectedOrderMessage() { }

	/* constructor helper */
	RejectedOrderMessage(string exchangeId, string contractId, time_stamp timestamp, int stratId, long messageId, int orderId, int qty, BuySellSide side, Px px) : 
	    LimitOrderMessage(exchangeId, contractId, timestamp, stratId, messageId, orderId, qty, side, px)
	{}

	/* copy constructor */
	RejectedOrderMessage(RejectedOrderMessage const & other) : 
	    RejectedOrderMessage(other.getExchangeId(), other.getContractId(), other.getTimestamp(), other.getStratId(), other.getMessageId(), other.getOrderId(), other.getQty(), other.getSide(), other.getPx())
	{}

	/* move constructor */
	RejectedOrderMessage(RejectedOrderMessage && other) : 
	    RejectedOrderMessage(other.getExchangeId(), other.getContractId(), other.getTimestamp(), other.getStratId(), other.getMessageId(), other.getOrderId(), other.getQty(), other.getSide(), other.getPx())
	{}

	/* copy assignment operator overloading */
	RejectedOrderMessage & operator=(RejectedOrderMessage const & other)
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

	/* move assignment operator overloading */
	RejectedOrderMessage & operator=(RejectedOrderMessage && other)
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

	/* default destructor */
	virtual ~RejectedOrderMessage()
	{}

	virtual void printMessage() 
	{
	    std::cout << "RejectLimitOrderMessage" << "\n"
		      << "Px = " << getPx() << "\n"
		      << "Side = " << getSide() << "\n"
		      << "Qty = " << getQty() << "\n";
	    AbstractMessage::printMessage();
	}

	virtual string logMessage()
	{
	    stringstream ss;
	    ss << "RejectLimitOrderMessage" << ","
	       << "Px=" << getPx() << ","
	       << "Side=" << getSide() << ","
	       << "Qty=" << getQty() << ","
	       << AbstractMessage::logMessage();
	    return ss.str();
	}


    };

    /* Rejected order for cancel limit order */
    template<>
    class RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage> > : public CancelOrderMessage<LimitOrderMessage>
    {
    public:
	/* accept default constructor */
	RejectedOrderMessage() { }

	/* constructor helper */
	RejectedOrderMessage(string exchangeId, string contractId, time_stamp timestamp, int stratId, long messageId, int orderId, int qty, BuySellSide side, Px px) : 
	    CancelOrderMessage<LimitOrderMessage>(exchangeId, contractId, timestamp, stratId, messageId, orderId, qty, side, px)
	{}

	/* copy constructor */
	RejectedOrderMessage(RejectedOrderMessage const & other) : 
	    RejectedOrderMessage(other.getExchangeId(), other.getContractId(), other.getTimestamp(), other.getStratId(), other.getMessageId(), other.getOrderId(), other.getQty(), other.getSide(), other.getPx())
	{}

	/* move constructor */
	RejectedOrderMessage(RejectedOrderMessage && other) : 
	    RejectedOrderMessage(other.getExchangeId(), other.getContractId(), other.getTimestamp(), other.getStratId(), other.getMessageId(), other.getOrderId(), other.getQty(), other.getSide(), other.getPx())
	{}

	/* copy assignment operator overloading */
	RejectedOrderMessage & operator=(RejectedOrderMessage const & other)
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

	/* move assignment operator overloading */
	RejectedOrderMessage & operator=(RejectedOrderMessage && other)
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

	/* default destructor */
	virtual ~RejectedOrderMessage()
	{}

	virtual void printMessage() 
	{
	    std::cout << "RejectCancelLimitOrderMessage" << "\n"
		      << "Px = " << getPx() << "\n"
		      << "Side = " << getSide() << "\n"
		      << "Qty = " << getQty() << "\n";
	    AbstractMessage::printMessage();
	}

	virtual string logMessage()
	{
	    stringstream ss;
	    ss << "RejectCancelLimitOrderMessage" << ","
	       << "Px=" << getPx() << ","
	       << "Side=" << getSide() << ","
	       << "Qty=" << getQty() << ","
	       << AbstractMessage::logMessage();
	    return ss.str();
	}


    };

    /* Rejected order for market order */
    template<>
    class RejectedOrderMessage<MarketOrderMessage> : public MarketOrderMessage
    {
    public:
	/* accept default constructor */
	RejectedOrderMessage() { }

	/* constructor helper */
	RejectedOrderMessage(string exchangeId, string contractId, time_stamp timestamp, int stratId, long messageId, int orderId, int qty, BuySellSide side) : 
	    MarketOrderMessage(exchangeId, contractId, timestamp, stratId, messageId, orderId, qty, side)
	{}

	/* copy constructor */
	RejectedOrderMessage(RejectedOrderMessage const & other) : 
	    RejectedOrderMessage(other.getExchangeId(), other.getContractId(), other.getTimestamp(), other.getStratId(), other.getMessageId(), other.getOrderId(), other.getQty(), other.getSide())
	{}

	/* move constructor */
	RejectedOrderMessage(RejectedOrderMessage && other) : 
	    RejectedOrderMessage(other.getExchangeId(), other.getContractId(), other.getTimestamp(), other.getStratId(), other.getMessageId(), other.getOrderId(), other.getQty(), other.getSide())
	{}

	/* copy assignment operator overloading */
	RejectedOrderMessage & operator=(RejectedOrderMessage const & other)
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

	/* move assignment operator overloading */
	RejectedOrderMessage & operator=(RejectedOrderMessage && other)
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

	/* default destructor */
	virtual ~RejectedOrderMessage()
	{}
	virtual void printMessage() 
	{
	    std::cout << "RejectMarketOrderMessage" << "\n"
		      << "Side = " << getSide() << "\n"
		      << "Qty = " << getQty() << "\n";
	    AbstractMessage::printMessage();
	}

	virtual string logMessage()
	{
	    stringstream ss;
	    ss << "RejectMarketOrderMessage" << ","
	       << "Side=" << getSide() << ","
	       << "Qty=" << getQty() << ","
	       << AbstractMessage::logMessage();
	    return ss.str();
	}


    };

    /* Rejected order for cancel market order */
    template<>
    class RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage> > : public CancelOrderMessage<MarketOrderMessage>
    {
    public:
	/* accept default constructor */
	RejectedOrderMessage() { }

	/* constructor helper */
	RejectedOrderMessage(string exchangeId, string contractId, time_stamp timestamp, int stratId, long messageId, int orderId, int qty, BuySellSide side) : 
	    CancelOrderMessage<MarketOrderMessage>(exchangeId, contractId, timestamp, stratId, messageId, orderId, qty, side)
	{}

	/* copy constructor */
	RejectedOrderMessage(RejectedOrderMessage const & other) : 
	    RejectedOrderMessage(other.getExchangeId(), other.getContractId(), other.getTimestamp(), other.getStratId(), other.getMessageId(), other.getOrderId(), other.getQty(), other.getSide())
	{}

	/* move constructor */
	RejectedOrderMessage(RejectedOrderMessage && other) : 
	    RejectedOrderMessage(other.getExchangeId(), other.getContractId(), other.getTimestamp(), other.getStratId(), other.getMessageId(), other.getOrderId(), other.getQty(), other.getSide())
	{}

	/* copy assignment operator overloading */
	RejectedOrderMessage & operator=(RejectedOrderMessage const & other)
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

	/* move assignment operator overloading */
	RejectedOrderMessage & operator=(RejectedOrderMessage && other)
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

	/* default destructor */
	virtual ~RejectedOrderMessage()
	{}

	virtual void printMessage() 
	{
	    std::cout << "RejectCancelMarketOrderMessage" << "\n"
		      << "Side = " << getSide() << "\n"
		      << "Qty = " << getQty() << "\n";
	    AbstractMessage::printMessage();
	}

	virtual string logMessage()
	{
	    stringstream ss;
	    ss << "RejectCancelMarketOrderMessage" << ","
	       << "Side=" << getSide() << ","
	       << "Qty=" << getQty() << ","
	       << AbstractMessage::logMessage();
	    return ss.str();
	}

    };

    template<bool isSnapshotQ>
    class BookUpdate;

    template<>
    class BookUpdate<true> : public AbstractMessage
    {
    private:
    	vector<Px> bidPx_;  // ordered by descending prices
    	vector<Px> askPx_;  // ordered by ascending prices
    	vector<int> bidQty_;
    	vector<int> askQty_;

    public:
	/* default constructor */
	BookUpdate()
	{}
	
	/* constructor helper */
	BookUpdate(string exchangeId, string contractId, time_stamp timestamp, long messageId)
	    : AbstractMessage(exchangeId, contractId, timestamp, -1, messageId, -1)
	{}
	
	/* constructor helper */
	BookUpdate(string exchangeId, string contractId, time_stamp timestamp, long messageId, vector<Px> bidPx, vector<int> bidQty, vector<Px> askPx, vector<int> askQty)
	    : AbstractMessage(exchangeId, contractId, timestamp, -1, messageId, -1), bidPx_(bidPx), askPx_(askPx), bidQty_(bidQty), askQty_(askQty)
	{}
	
	/* copy constructor */
	BookUpdate(BookUpdate const & other)
	    : BookUpdate(other.getExchangeId(),
			 other.getContractId(),
			 other.getTimestamp(),
			 other.getMessageId(),
			 other.getBidPx(),
			 other.getBidQty(),
			 other.getAskPx(),
			 other.getAskQty())
	{}

	/* move constructor */
	BookUpdate(BookUpdate && other)
	    : BookUpdate(other.getExchangeId(),
			 other.getContractId(),
			 other.getTimestamp(),
			 other.getMessageId(),
			 other.getBidPx(),
			 other.getBidQty(),
			 other.getAskPx(),
			 other.getAskQty())
	{}
	
	/* copy assignment operator overloading */
	BookUpdate & operator=(BookUpdate const & other)
	{
	    this->exchangeId() = other.getExchangeId();
	    this->contractId() = other.getContractId();
	    this->timestamp() = other.getTimestamp();
	    this->messageId() = other.getMessageId();
	    this->bidPx() = other.getBidPx();
	    this->bidQty() = other.getBidQty();
	    this->askPx() = other.getAskPx();
	    this->askQty() = other.getAskQty();
	    return *this;
	}

	/* move assignment operator overloading */
	BookUpdate & operator=(BookUpdate && other)
	{
	    this->exchangeId() = other.getExchangeId();
	    this->contractId() = other.getContractId();
	    this->timestamp() = other.getTimestamp();
	    this->messageId() = other.getMessageId();
	    this->bidPx() = other.getBidPx();
	    this->bidQty() = other.getBidQty();
	    this->askPx() = other.getAskPx();
	    this->askQty() = other.getAskQty();
	    return *this;
	}
	
	/* default destructor */
	~BookUpdate()
	{}
      
        // method for adding midpoint for strategy tracking
        bool getMidPx(double *px) const
        {
	    if (bidPx_.empty() || askPx_.empty() || bidQty_.front() <= 0 || askQty_.front() <= 0)
		return false;
	    else {
		*px = 0.5 * (bidPx_.front().getPx() + askPx_.front().getPx());
		return true;
	    }
        }

        // bid-ask spread
        bool getBidAskSpread(double *px) const 
        {
	    if (bidPx_.empty() || askPx_.empty() || bidQty_.front() <= 0 || askQty_.front() <= 0)
		return false;
	    else {
		*px = askPx_.front().getPx() - bidPx_.front().getPx();
		return true;
	    }
        }

	vector<Px> & bidPx()
	{
	    return bidPx_;
	}

	vector<Px> getBidPx() const
	{
	    return bidPx_;
	}

	vector<Px> & askPx()
	{
	    return askPx_;
	}

	vector<Px> getAskPx() const
	{
	    return askPx_;
	}

	vector<int> & bidQty()
	{
	    return bidQty_;
	}

	vector<int> getBidQty() const
	{
	    return bidQty_;
	}

	vector<int> & askQty()
	{
	    return askQty_;
	}

	vector<int> getAskQty() const
	{
	    return askQty_;
	}
    };

    template<bool isSnapshotQ>
    class TradeUpdate;

    template<>
    class TradeUpdate<true> : public AbstractMessage
    {
    private:
    	Px px_;
    	int qty_;
    	BuySellSide side_;

    public:
		/* default constructor */
		TradeUpdate() {}
	/* constructor helper */
	TradeUpdate(string exchangeId, string contractId, time_stamp timestamp, long messageId)
	    : AbstractMessage(exchangeId, contractId, timestamp, -1, messageId, -1), px_(0.0, -1.0)
	{}
	
	/* constructor helper */
	TradeUpdate(string exchangeId, string contractId, time_stamp timestamp, long messageId, Px px, int qty, BuySellSide side)
	    : AbstractMessage(exchangeId, contractId, timestamp, -1, messageId, -1), px_(px), qty_(qty), side_(side)
	{}

	/* copy constructor */
	TradeUpdate(TradeUpdate const & other)
	    : TradeUpdate(other.getExchangeId(),
			  other.getContractId(),
			  other.getTimestamp(),
			  other.getMessageId(),
			  other.getPx(),
			  other.getQty(),
			  other.getSide())
	{}

	/* move constructor */
	TradeUpdate(TradeUpdate && other)
	    : TradeUpdate(other.getExchangeId(),
			  other.getContractId(),
			  other.getTimestamp(),
			  other.getMessageId(),
			  other.getPx(),
			  other.getQty(),
			  other.getSide())
	{}
	
	/* copy assignment operator overloading */
	TradeUpdate & operator=(TradeUpdate const & other)
	{
	    this->exchangeId() = other.getExchangeId();
	    this->contractId() = other.getContractId();
	    this->timestamp() = other.getTimestamp();
	    this->messageId() = other.getMessageId();
	    this->px() = other.getPx();
	    this->qty() = other.getQty();
	    this->side() = other.getSide();
	    return *this;
	}

	/* move assignment operator overloading */
	TradeUpdate & operator=(TradeUpdate && other)
	{
	    this->exchangeId() = other.getExchangeId();
	    this->contractId() = other.getContractId();
	    this->timestamp() = other.getTimestamp();
	    this->messageId() = other.getMessageId();
	    this->px() = other.getPx();
	    this->qty() = other.getQty();
	    this->side() = other.getSide();
	    return *this;
	}

	Px & px()
	{
	    return px_;
	}

	Px getPx() const
	{
	    return px_;
	}

	int & qty()
	{
	    return qty_;
	}

	int getQty() const
	{
	    return qty_;
	}

	BuySellSide & side()
	{
	    return side_;
	}

	BuySellSide getSide() const
	{
	    return side_;
	}
    };

    template<bool isSnapshotQ>
    class ImpliedSpreadTopBookUpdate;

    template<>
    class ImpliedSpreadTopBookUpdate<true>
    {
    private:
 	BookUpdate<true> legFront_;
 	BookUpdate<true> legBack_;
	time_stamp timestamp_;

    	Px  bidPx_;      // top bid price
    	Px  askPx_;      // top ask price
    	int bidQty_ = 0; // top bid qty, 0 means invalid implied spread
    	int askQty_ = 0; // top ask qty, 0 means invalid implied spread

    public:
 	/* default constructor */
 	ImpliedSpreadTopBookUpdate() 
	    : timestamp_(duration_nano(0)) 
 	{}

 	/* constructor helper */
 	ImpliedSpreadTopBookUpdate(BookUpdate<true> const legFront, BookUpdate<true> const legBack)
 	    : legFront_(legFront), legBack_(legBack), timestamp_(max(legFront.getTimestamp(), legBack.getTimestamp()))
 	{}
		
 	/* copy constructor */
 	ImpliedSpreadTopBookUpdate(ImpliedSpreadTopBookUpdate const & other)
 	    : ImpliedSpreadTopBookUpdate(other.getFrontLeg(),
					 other.getBackLeg())
 	{}

 	/* move constructor */
 	ImpliedSpreadTopBookUpdate(ImpliedSpreadTopBookUpdate && other) 
 	    : ImpliedSpreadTopBookUpdate(other.getFrontLeg(),
					 other.getBackLeg())
 	{}
	
 	/* copy assignment operator overloading */
 	ImpliedSpreadTopBookUpdate & operator=(ImpliedSpreadTopBookUpdate const & other) 
 	{
 	    this->frontLeg() = other.getFrontLeg();
 	    this->backLeg() = other.getBackLeg();
	    this->timestamp() = other.getTimestamp();
 	    return *this;
 	}

 	/* move assignment operator overloading */
 	ImpliedSpreadTopBookUpdate & operator=(ImpliedSpreadTopBookUpdate && other)
 	{
 	    this->frontLeg() = other.getFrontLeg();
 	    this->backLeg() = other.getBackLeg();
	    this->timestamp() = other.getTimestamp();
 	    return *this;
 	}
	
 	/* default destructor */
 	~ImpliedSpreadTopBookUpdate()
 	{}

        void update(BookUpdate<true> bu, bool frontQ)
	{
	    timestamp_ = max(bu.getTimestamp(), timestamp_);
	    if (frontQ) {
		legFront_ = bu;
	    } else {
		legBack_ = bu;
	    }
	}

 	// method to imply the spread contract
 	void imply()
 	{
	    // working on the implied ask level
 	    if (!legFront_.getBidPx().empty() &&
 		legFront_.getBidQty().front() > 0 &&
 		!legBack_.getAskPx().empty() &&
 		legBack_.getAskQty().front() > 0) {
 		askPx_ = legBack_.getAskPx().front() - legFront_.getBidPx().front();
		askQty_ = min(legBack_.getAskQty().front(), legFront_.getBidQty().front());
	    } else {
		askQty_ = 0;
	    }
	    // working on the implied bid level
 	    if (!legFront_.getAskPx().empty() &&
 		legFront_.getAskQty().front() > 0 &&
 		!legBack_.getBidPx().empty() &&
 		legBack_.getBidQty().front() > 0) {
 		bidPx_ = legBack_.getBidPx().front() - legFront_.getAskPx().front();
		bidQty_ = min(legBack_.getBidQty().front(), legFront_.getAskQty().front());
	    } else {
		bidQty_ = 0;
	    }
 	}

        bool getMidPx(double *px) const
        {
 	    if (bidQty_ == 0 || askQty_ == 0)
 		return false;
 	    else {
 		*px = 0.5 * (bidPx_.getPx() + askPx_.getPx());
 		return true;
 	    }
        }

        bool getBidAskSpread(double *px, bool frontQ) const 
        {
	    if (frontQ) {
		if (legFront_.getBidPx().empty() || 
		    legFront_.getAskPx().empty() || 
		    legFront_.getBidQty().front() <= 0 || 
		    legFront_.getAskQty().front() <= 0)
		    return false;
		else {
		    *px = legFront_.getAskPx().front().getPx() - legFront_.getBidPx().front().getPx();
		    return true;
		}
	    } else {
		if (legBack_.getBidPx().empty() || 
		    legBack_.getAskPx().empty() || 
		    legBack_.getBidQty().front() <= 0 || 
		    legBack_.getAskQty().front() <= 0)
		    return false;
		else {
		    *px = legBack_.getAskPx().front().getPx() - legBack_.getBidPx().front().getPx();
		    return true;
		}
	    }
        }

        bool getBidAskSpread(double *px) const 
        {
	    if (legFront_.getBidPx().empty() || 
		legFront_.getAskPx().empty() || 
		legFront_.getBidQty().front() <= 0 || 
		legFront_.getAskQty().front() <= 0 || 
		legBack_.getBidPx().empty() || 
		legBack_.getAskPx().empty() || 
		legBack_.getBidQty().front() <= 0 || 
		legBack_.getAskQty().front() <= 0) {
		return false;
	    } else {
		*px = (legFront_.getAskPx().front().getPx() + legBack_.getAskPx().front().getPx()) - (legFront_.getBidPx().front().getPx() + legBack_.getBidPx().front().getPx());
		return true;
	    }
        }
	
 	Px & bidPx() 
 	{
 	    return bidPx_;
 	}

 	Px getBidPx() const
 	{
 	    return bidPx_;
 	}

 	Px & askPx()
 	{
 	    return askPx_;
 	}

 	Px getAskPx() const
 	{
 	    return askPx_;
 	}

 	int & bidQty()
 	{
 	    return bidQty_;
 	}

 	int getBidQty() const
 	{
 	    return bidQty_;
 	}

 	int & askQty()
 	{
 	    return askQty_;
 	}

 	int getAskQty() const
 	{
 	    return askQty_;
 	}

	BookUpdate<true> getFrontLeg() const
	{
	    return legFront_;
	}

	BookUpdate<true> & frontLeg()
	{
	    return legFront_;
	}
	
	BookUpdate<true> getBackLeg() const
	{
	    return legBack_;
	}

	BookUpdate<true> & backLeg()
	{
	    return legBack_;
	}

	string getTicker() const
	{
	    return legBack_.getExchangeId() + ":" + legBack_.getContractId() + "-" + legFront_.getExchangeId() + ":" + legFront_.getContractId();
	}

	time_stamp getTimestamp() const
	{
	    return timestamp_;
	}

	time_stamp & timestamp()
	{
	    return timestamp_;
	}

	bool syncQ() const 
	{
#ifdef PRODUCTION
	    return abs((legFront_.getTimestamp()-legBack_.getTimestamp()).count()) <= 5000000ULL;
#elif defined SIMULATION
	    return legFront_.getTimestamp() == legBack_.getTimestamp();
#endif
	}
    };
    
}

#endif    // MESSAGE_HPP
