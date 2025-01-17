// **********************************************************************
//
// Copyright (c) 2003-2015 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_VALUE_FACTORY_MANAGER_I_H
#define ICE_VALUE_FACTORY_MANAGER_I_H

#include <Ice/ValueFactory.h>
#include <Ice/VirtualShared.h>
#include <IceUtil/Mutex.h>

namespace IceInternal
{

class ValueFactoryManagerI;
ICE_DEFINE_PTR(ValueFactoryManagerIPtr, ValueFactoryManagerI);

class ValueFactoryManagerI : public Ice::EnableSharedFromThis<ValueFactoryManagerI>,
                             public Ice::ValueFactoryManager,
                             public IceUtil::Mutex
{
public:

    ValueFactoryManagerI();

#ifdef ICE_CPP11_MAPPING
    virtual void add(std::function<std::shared_ptr<Ice::Value> (std::string)>, const std::string&);
    virtual std::function<std::shared_ptr<Ice::Value> (const std::string&)> find(const std::string&) const;
#else
    virtual void add(const Ice::ValueFactoryPtr&, const std::string&);
    virtual Ice::ValueFactoryPtr find(const std::string&) const;
#endif

private:

    typedef std::map<std::string, ICE_VALUE_FACTORY> FactoryMap;

    FactoryMap _factoryMap;
    mutable FactoryMap::iterator _factoryMapHint;
};

}

#endif
