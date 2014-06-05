// **********************************************************************
//
// Copyright (c) 2003-2014 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <TestCommon.h>
#include <Test.h>

using namespace std;
using namespace Test;

class CallbackBase : public IceUtil::Monitor<IceUtil::Mutex>
{
public:

    CallbackBase() :
        _called(false)
    {
    }

    virtual ~CallbackBase()
    {
    }

    void check()
    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);
        while(!_called)
        {
            wait();
        }
        _called = false;
    }

protected:

    void called()
    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock sync(*this);
        assert(!_called);
        _called = true;
        notify();
    }

private:

    bool _called;
};

class Callback : public IceUtil::Shared, public CallbackBase
{
public:

    void response()
    {
        called();
    }

    void exception(const ::Ice::Exception&)
    {
        test(false);
    }

    void responseEx()
    {
        test(false);
    }

    void exceptionEx(const ::Ice::Exception& ex)
    {
        test(dynamic_cast<const Ice::InvocationTimeoutException*>(&ex));
        called();
    }
};
typedef IceUtil::Handle<Callback> CallbackPtr;

TimeoutPrx
allTests(const Ice::CommunicatorPtr& communicator)
{
    string sref = "timeout:default -p 12010";
    Ice::ObjectPrx obj = communicator->stringToProxy(sref);
    test(obj);

    TimeoutPrx timeout = TimeoutPrx::checkedCast(obj);
    test(timeout);

    cout << "testing connect timeout... " << flush;
    {
        //
        // Expect ConnectTimeoutException.
        //
        TimeoutPrx to = TimeoutPrx::uncheckedCast(obj->ice_timeout(250));
        to->holdAdapter(500);
        to->ice_getConnection()->close(true); // Force a reconnect.
        try
        {
            to->op();
            test(false);
        }
        catch(const Ice::ConnectTimeoutException&)
        {
            // Expected.
        }
    }
    {
        //
        // Expect success.
        //
        timeout->op(); // Ensure adapter is active.
        TimeoutPrx to = TimeoutPrx::uncheckedCast(obj->ice_timeout(1000));
        to->holdAdapter(500);
        to->ice_getConnection()->close(true); // Force a reconnect.
        try
        {
            to->op();
        }
        catch(const Ice::ConnectTimeoutException&)
        {
            test(false);
        }
    }
    cout << "ok" << endl;

    cout << "testing connection timeout... " << flush;
    {
        //
        // Expect TimeoutException.
        //
        TimeoutPrx to = TimeoutPrx::uncheckedCast(obj->ice_timeout(250));
        to->holdAdapter(500);
        try
        {
            ByteSeq seq(10000000);
            to->sendData(seq);
            test(false);
        }
        catch(const Ice::TimeoutException&)
        {
            // Expected.
        }
    }
    {
        //
        // Expect success.
        //
        timeout->op(); // Ensure adapter is active.
        TimeoutPrx to = TimeoutPrx::uncheckedCast(obj->ice_timeout(1000));
        to->holdAdapter(500);
        try
        {
            ByteSeq seq(1000000);
            to->sendData(seq);
        }
        catch(const Ice::TimeoutException&)
        {
            test(false);
        }
    }
    cout << "ok" << endl;

    cout << "testing invocation timeout... " << flush;
    {
        Ice::ConnectionPtr connection = obj->ice_getConnection();
        TimeoutPrx to = TimeoutPrx::uncheckedCast(obj->ice_invocationTimeout(250));
        test(connection == to->ice_getConnection());
        try
        {
            to->sleep(500);
            test(false);
        }
        catch(const Ice::InvocationTimeoutException&)
        {
        }
        to = TimeoutPrx::uncheckedCast(obj->ice_invocationTimeout(500));
        test(connection == to->ice_getConnection());
        try
        {
            to->sleep(250);
        }
        catch(const Ice::InvocationTimeoutException&)
        {
            test(false);
        }
        test(connection == to->ice_getConnection());
    }
    {
        //
        // Expect InvocationTimeoutException.
        //
        TimeoutPrx to = TimeoutPrx::uncheckedCast(obj->ice_invocationTimeout(250));
        CallbackPtr cb = new Callback();
        to->begin_sleep(500, newCallback_Timeout_sleep(cb, &Callback::responseEx, &Callback::exceptionEx));
        cb->check();
    }
    {
        //
        // Expect success.
        //
        TimeoutPrx to = TimeoutPrx::uncheckedCast(obj->ice_invocationTimeout(500));
        CallbackPtr cb = new Callback();
        to->begin_sleep(250, newCallback_Timeout_sleep(cb, &Callback::response, &Callback::exception));
        cb->check();
    }
    cout << "ok" << endl;

    cout << "testing close timeout... " << flush;
    {
        TimeoutPrx to = TimeoutPrx::checkedCast(obj->ice_timeout(250));
        Ice::ConnectionPtr connection = to->ice_getConnection();
        timeout->holdAdapter(750);
        connection->close(false);
        try
        {
            connection->getInfo(); // getInfo() doesn't throw in the closing state.
        }
        catch(const Ice::LocalException&)
        {
            test(false);
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(500));
        try
        {
            connection->getInfo();
            test(false);
        }
        catch(const Ice::CloseConnectionException&)
        {
            // Expected.
        }
        timeout->op(); // Ensure adapter is active.
    }
    cout << "ok" << endl;

    cout << "testing timeout overrides... " << flush;
    {
        //
        // Test Ice.Override.Timeout. This property overrides all
        // endpoint timeouts.
        //
        Ice::InitializationData initData;
        initData.properties = communicator->getProperties()->clone();
        initData.properties->setProperty("Ice.Override.Timeout", "250");
        Ice::CommunicatorPtr comm = Ice::initialize(initData);
        TimeoutPrx to = TimeoutPrx::checkedCast(comm->stringToProxy(sref));
        to->holdAdapter(500);
        try
        {
            ByteSeq seq(10000000);
            to->sendData(seq);
            test(false);
        }
        catch(const Ice::TimeoutException&)
        {
            // Expected.
        }
        //
        // Calling ice_timeout() should have no effect.
        //
        timeout->op(); // Ensure adapter is active.
        to = TimeoutPrx::checkedCast(to->ice_timeout(1000));
        to->holdAdapter(500);
        try
        {
            ByteSeq seq(10000000);
            to->sendData(seq);
            test(false);
        }
        catch(const Ice::TimeoutException&)
        {
            // Expected.
        }
        comm->destroy();
    }
    {
        //
        // Test Ice.Override.ConnectTimeout.
        //
        Ice::InitializationData initData;
        initData.properties = communicator->getProperties()->clone();
        initData.properties->setProperty("Ice.Override.ConnectTimeout", "250");
        Ice::CommunicatorPtr comm = Ice::initialize(initData);
        timeout->holdAdapter(500);
        TimeoutPrx to = TimeoutPrx::uncheckedCast(comm->stringToProxy(sref));
        try
        {
            to->op();
            test(false);
        }
        catch(const Ice::ConnectTimeoutException&)
        {
            // Expected.
        }
        //
        // Calling ice_timeout() should have no effect on the connect timeout.
        //
        timeout->op(); // Ensure adapter is active.
        timeout->holdAdapter(500);
        to = TimeoutPrx::uncheckedCast(to->ice_timeout(750));
        try
        {
            to->op();
            test(false);
        }
        catch(const Ice::ConnectTimeoutException&)
        {
            // Expected.
        }
        //
        // Verify that timeout set via ice_timeout() is still used for requests.
        //
        to->op(); // Force connection.
        timeout->holdAdapter(500);
        to = TimeoutPrx::uncheckedCast(to->ice_timeout(250));
        try
        {
            ByteSeq seq(10000000);
            to->sendData(seq);
            test(false);
        }
        catch(const Ice::TimeoutException&)
        {
            // Expected.
        }
        comm->destroy();
    }
    {
        //
        // Test Ice.Override.CloseTimeout.
        //
        Ice::InitializationData initData;
        initData.properties = communicator->getProperties()->clone();
        initData.properties->setProperty("Ice.Override.CloseTimeout", "200");
        Ice::CommunicatorPtr comm = Ice::initialize(initData);
        Ice::ConnectionPtr connection = comm->stringToProxy(sref)->ice_getConnection();
        timeout->holdAdapter(750);
        IceUtil::Time now = IceUtil::Time::now();
        comm->destroy();
        test(IceUtil::Time::now() - now < IceUtil::Time::milliSeconds(500));
    }
    cout << "ok" << endl;

    return timeout;
}
