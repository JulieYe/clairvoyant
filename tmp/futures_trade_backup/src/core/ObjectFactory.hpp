//
// File:        $ ObjectFactory.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/11/24 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.


#ifndef OBJECTFACTORY_HPP
#define OBJECTFACTORY_HPP

#include "Matchy.hpp"
#include "Strategy.hpp"
#include <map>
#include <string>

namespace simulator
{
    template <class T>
    class ObjectFactory
    {
    public:
	void registerByName(std::string tName,  T* pT);
	T* getByName(std::string tName);

	~ObjectFactory() {
	    for(typename std::map<std::string, T*>::iterator it = objectMap_.begin();
	    	it != objectMap_.end(); it++)
	    	delete it->second;
	}
    protected:
	ObjectFactory() { }
	std::map<std::string, T*> objectMap_;
    };


    template <class T>
    void ObjectFactory<T>::registerByName(std::string tName, T * pT)
    {
	typename std::map<std::string, T*>::iterator it = objectMap_.find(tName);
	if(it == objectMap_.end()) 
	    objectMap_[tName] = pT;
	else {
	    std::cerr << "duplicate names in factory!" << std::endl;
	    throw;
	}
    }
    
    template <class T>
    T * ObjectFactory<T>::getByName(std::string tName)
    {
	typename std::map<std::string, T*>::iterator it = objectMap_.find(tName);
	if(it == objectMap_.end()) {
	    std::cerr << tName << " not found in factory" << std::endl;
	    throw;
	} else 
	    return it->second;
    }

    class MatchyFactory : public ObjectFactory<Matchy>
    {
    public:
	static MatchyFactory * instance();
    private:
	MatchyFactory() { }
	static MatchyFactory * pFactory_;
    };

    class SimMatchyFactory : public ObjectFactory<SimMatchy>
    {
    public:
	static SimMatchyFactory * instance();
    private:
	SimMatchyFactory() { }
	static SimMatchyFactory * pFactory_;
    };

    class StrategyFactory : public ObjectFactory<Strategy>
    {
    public:
	static StrategyFactory * instance();
    private:
	StrategyFactory() { }
	static StrategyFactory * pFactory_;
    };


}

#endif
