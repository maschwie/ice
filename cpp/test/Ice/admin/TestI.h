// **********************************************************************
//
// Copyright (c) 2003-2015 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef TEST_I_H
#define TEST_I_H

#include <Test.h>
#include <TestCommon.h>
#include <Ice/NativePropertiesAdmin.h>

class RemoteCommunicatorI : public virtual Test::RemoteCommunicator,
                            public virtual Ice::PropertiesAdminUpdateCallback,
                            public IceUtil::Monitor<IceUtil::Mutex>
{
public:

    RemoteCommunicatorI(const Ice::CommunicatorPtr&);

    virtual Ice::ObjectPrxPtr getAdmin(const Ice::Current&);
    virtual Ice::PropertyDict getChanges(const Ice::Current&);

    virtual void print(ICE_IN(std::string), const Ice::Current&);
    virtual void trace(ICE_IN(std::string), ICE_IN(std::string), const Ice::Current&);
    virtual void warning(ICE_IN(std::string), const Ice::Current&);
    virtual void error(ICE_IN(std::string), const Ice::Current&);

    virtual void shutdown(const Ice::Current&);
    virtual void waitForShutdown(const Ice::Current&);
    virtual void destroy(const Ice::Current&);

    virtual void updated(const Ice::PropertyDict&);

private:

    Ice::CommunicatorPtr _communicator;
    Ice::PropertyDict _changes;
    bool _called;
};
ICE_DEFINE_PTR(RemoteCommunicatorIPtr, RemoteCommunicatorI);

class RemoteCommunicatorFactoryI : public Test::RemoteCommunicatorFactory
{
public:

    virtual Test::RemoteCommunicatorPrxPtr createCommunicator(ICE_IN(Ice::PropertyDict), const Ice::Current&);
    virtual void shutdown(const Ice::Current&);
};

class TestFacetI : public Test::TestFacet
{
public:

    virtual void op(const Ice::Current&)
    {
    }
};

#endif
