// **********************************************************************
//
// Copyright (c) 2003-2015 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceUtil/DisableWarnings.h>
#include <Ice/AsyncResult.h>
#include <Ice/ThreadPool.h>
#include <Ice/Instance.h>
#include <Ice/LoggerUtil.h>
#include <Ice/Properties.h>
#include <Ice/RequestHandler.h>
#include <Ice/OutgoingAsync.h>

using namespace std;
using namespace Ice;
using namespace IceInternal;

#ifndef ICE_CPP11_MAPPING
IceUtil::Shared* Ice::upCast(AsyncResult* p) { return p; }
#endif

const unsigned char Ice::AsyncResult::OK = 0x1;
const unsigned char Ice::AsyncResult::Done = 0x2;
const unsigned char Ice::AsyncResult::Sent = 0x4;
const unsigned char Ice::AsyncResult::EndCalled = 0x8;

void
AsyncResult::cancel()
{
    cancel(InvocationCanceledException(__FILE__, __LINE__));
}

Int
AsyncResult::getHash() const
{
    return static_cast<Int>(reinterpret_cast<Long>(this) >> 4);
}

CommunicatorPtr 
AsyncResult::getCommunicator() const
{
    return _communicator;
}

ConnectionPtr 
AsyncResult::getConnection() const
{
    return ICE_NULLPTR;
}

ObjectPrxPtr
AsyncResult::getProxy() const
{
    return ICE_NULLPTR;
}

bool
AsyncResult::isCompleted() const
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(_monitor);
    return _state & Done;
}

void
AsyncResult::waitForCompleted()
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(_monitor);
    while(!(_state & Done))
    {
        _monitor.wait();
    }
}

bool
AsyncResult::isSent() const
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(_monitor);
    return _state & Sent;
}

void
AsyncResult::waitForSent()
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(_monitor);
    while(!(_state & Sent) && !ICE_EXCEPTION_GET(_exception))
    {
        _monitor.wait();
    }
}

void
AsyncResult::throwLocalException() const
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(_monitor);
    if(ICE_EXCEPTION_GET(_exception))
    {
        ICE_RETHROW_EXCEPTION(_exception);
    }
}

bool 
AsyncResult::sentSynchronously() const
{
    return _sentSynchronously;
}

#ifndef ICE_CPP11_MAPPING
LocalObjectPtr 
AsyncResult::getCookie() const
{
    return _cookie;
}
#endif

const std::string& 
AsyncResult::getOperation() const
{
    return _operation;
}

void
AsyncResult::__throwUserException()
{
    try
    {
        _is.startEncapsulation();
        _is.throwException();
    }
    catch(const Ice::UserException&)
    {
        _is.endEncapsulation();
        throw;
    }
}

bool
AsyncResult::__wait()
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(_monitor);
    if(_state & EndCalled)
    {
        throw IceUtil::IllegalArgumentException(__FILE__, __LINE__, "end_ method called more than once");
    }
    _state |= EndCalled;
    while(!(_state & Done))
    {
        _monitor.wait();
    }

    if(ICE_EXCEPTION_GET(_exception))
    {
        ICE_RETHROW_EXCEPTION(_exception);
    }

    return _state & OK;
}

void
AsyncResult::__check(const AsyncResultPtr& r, const IceProxy::Ice::Object* prx, const string& operation)
{
    __check(r, operation);
    if(r->getProxy().get() != prx)
    {
        throw IceUtil::IllegalArgumentException(__FILE__, __LINE__, "Proxy for call to end_" + operation +
                                                " does not match proxy that was used to call corresponding begin_" +
                                                operation + " method");
    }
}

void
AsyncResult::__check(const AsyncResultPtr& r, const Ice::Communicator* com, const string& operation)
{
    __check(r, operation);
    if(r->getCommunicator().get() != com)
    {
        throw IceUtil::IllegalArgumentException(__FILE__, __LINE__, "Communicator for call to end_" + operation +
                                                " does not match communicator that was used to call corresponding " +
                                                "begin_" + operation + " method");
    }
}

void
AsyncResult::__check(const AsyncResultPtr& r, const Ice::Connection* con, const string& operation)
{
    __check(r, operation);
    if(r->getConnection().get() != con)
    {
        throw IceUtil::IllegalArgumentException(__FILE__, __LINE__, "Connection for call to end_" + operation +
                                                " does not match connection that was used to call corresponding " +
                                                "begin_" + operation + " method");
    }
}

void
AsyncResult::__check(const AsyncResultPtr& r, const string& operation)
{
    if(!r)
    {
        throw IceUtil::IllegalArgumentException(__FILE__, __LINE__, "AsyncResult == null");
    }
    else if(&r->_operation != &operation)
    {
        throw IceUtil::IllegalArgumentException(__FILE__, __LINE__, "Incorrect operation for end_" + operation +
                                                " method: " + r->_operation);
    }
}

#ifdef ICE_CPP11_MAPPING
AsyncResult::AsyncResult(const CommunicatorPtr& communicator,
                         const IceInternal::InstancePtr& instance,
                         const string& op,
                         const CallbackBasePtr& callback) :
    _instance(instance),
    _sentSynchronously(false),
    _is(instance.get(), Ice::currentProtocolEncoding),
    _communicator(communicator),
    _operation(op),
    _callback(callback),
    _state(0)
{
    if(!_callback)
    {
        throw IceUtil::IllegalArgumentException(__FILE__, __LINE__);
    }
}
#else
AsyncResult::AsyncResult(const CommunicatorPtr& communicator,
                         const IceInternal::InstancePtr& instance,
                         const string& op,
                         const CallbackBasePtr& del,
                         const LocalObjectPtr& cookie) :
    _instance(instance),
    _sentSynchronously(false),
    _is(instance.get(), Ice::currentProtocolEncoding),
    _communicator(communicator),
    _operation(op),
    _callback(del),
    _cookie(cookie),
    _state(0)
{
    if(!_callback)
    {
        throw IceUtil::IllegalArgumentException(__FILE__, __LINE__);
    }
    const_cast<CallbackBasePtr&>(_callback) = _callback->verify(_cookie);
}
#endif

AsyncResult::~AsyncResult()
{
}

bool
AsyncResult::sent(bool done)
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(_monitor);
    assert(!ICE_EXCEPTION_GET(_exception));
    bool alreadySent = _state & Sent;
    _state |= Sent;
    if(done)
    {
        _state |= Done | OK;
        _cancellationHandler = 0;
        if(!_callback || !_callback->hasSentCallback())
        {
            _observer.detach();
        }
    }

    _monitor.notifyAll();
    return !alreadySent && _callback && _callback->hasSentCallback();
}

bool
AsyncResult::finished(bool ok)
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(_monitor);
    _state |= Done;
    if(ok)
    {
        _state |= OK;
    }
    _cancellationHandler = 0;
    if(!_callback)
    {
        _observer.detach();
    }
    _monitor.notifyAll();
    return _callback != ICE_NULLPTR;
}

bool
AsyncResult::finished(const Ice::Exception& ex)
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(_monitor);
    _state |= Done;
    ICE_RESET_EXCEPTION(_exception, ex.ice_clone());
    _cancellationHandler = 0;
    _observer.failed(ex.ice_id());
    if(!_callback)
    {
        _observer.detach();
    }
    _monitor.notifyAll();
    return _callback != ICE_NULLPTR;
}

void
AsyncResult::invokeSentAsync()
{
    class AsynchronousSent : public DispatchWorkItem
    {
    public:

        AsynchronousSent(const ConnectionPtr& connection, const AsyncResultPtr& result) :
            DispatchWorkItem(connection), _result(result)
        {
        }

        virtual void
        run()
        {
            _result->invokeSent();
        }
        
    private:

        const AsyncResultPtr _result;
    };

    //
    // This is called when it's not safe to call the sent callback
    // synchronously from this thread. Instead the exception callback
    // is called asynchronously from the client thread pool.
    //
    try
    {
        _instance->clientThreadPool()->dispatch(new AsynchronousSent(_cachedConnection, ICE_SHARED_FROM_THIS));
    }
    catch(const Ice::CommunicatorDestroyedException&)
    {
    }
}

void
AsyncResult::invokeCompletedAsync()
{
    class AsynchronousCompleted : public DispatchWorkItem
    {
    public:

        AsynchronousCompleted(const ConnectionPtr& connection, const AsyncResultPtr& result) :
            DispatchWorkItem(connection), _result(result)
        {
        }

        virtual void
        run()
        {
            _result->invokeCompleted();
        }

    private:

        const AsyncResultPtr _result;
    };

    //
    // CommunicatorDestroyedCompleted is the only exception that can propagate directly
    // from this method.
    //
    _instance->clientThreadPool()->dispatch(new AsynchronousCompleted(_cachedConnection, ICE_SHARED_FROM_THIS));
}

void
AsyncResult::invokeSent()
{
    assert(_callback);

    try
    {
        AsyncResultPtr self(ICE_SHARED_FROM_THIS);
        _callback->sent(self);
    }
    catch(const std::exception& ex)
    {
        warning(ex);
    }
    catch(...)
    {
        warning();
    }

    if(_observer)
    {
        ObjectPrxPtr proxy = getProxy();
        if(!proxy || !proxy->ice_isTwoway())
        {
            _observer.detach();
        }            
    }
}

void
AsyncResult::invokeCompleted()
{
    assert(_callback);

    try
    {
        AsyncResultPtr self(ICE_SHARED_FROM_THIS);
        _callback->completed(self);
    }
    catch(const std::exception& ex)
    {
        warning(ex);
    }
    catch(...)
    {
        warning();
    }

    _observer.detach();
}

void
AsyncResult::cancel(const Ice::LocalException& ex)
{
    CancellationHandlerPtr handler;
    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock sync(_monitor);
        ICE_RESET_EXCEPTION(_cancellationException, ex.ice_clone());
        if(!_cancellationHandler)
        {
            return;
        }
        handler = _cancellationHandler;
    }
    handler->asyncRequestCanceled(ICE_DYNAMIC_CAST(OutgoingAsyncBase, ICE_SHARED_FROM_THIS), ex);
}

void
AsyncResult::cancelable(const CancellationHandlerPtr& handler)
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock sync(_monitor);
    

    if(ICE_EXCEPTION_GET(_cancellationException))
    {
        try
        {
            ICE_RETHROW_EXCEPTION(_cancellationException);
        }
        catch(const Ice::LocalException&)
        {
            ICE_RESET_EXCEPTION(_cancellationException, ICE_NULLPTR);
            throw;
        }
    }
    _cancellationHandler = handler;
}

void
AsyncResult::warning(const std::exception& exc) const
{
    if(_instance->initializationData().properties->getPropertyAsIntWithDefault("Ice.Warn.AMICallback", 1) > 0)
    {
        Warning out(_instance->initializationData().logger);
        const Exception* ex = dynamic_cast<const Exception*>(&exc);
        if(ex)
        {
            out << "Ice::Exception raised by AMI callback:\n" << *ex;
        }
        else
        {
            out << "std::exception raised by AMI callback:\n" << exc.what();
        }
    }
}

void
AsyncResult::warning() const
{
    if(_instance->initializationData().properties->getPropertyAsIntWithDefault("Ice.Warn.AMICallback", 1) > 0)
    {
        Warning out(_instance->initializationData().logger);
        out << "unknown exception raised by AMI callback";
    }
}


namespace
{

//
// Dummy class derived from CallbackBase
// We use this class for the __dummyCallback extern pointer in OutgoingAsync. In turn,
// this allows us to test whether the user supplied a null delegate instance to the
// generated begin_ method without having to generate a separate test to throw IllegalArgumentException
// in the inlined versions of the begin_ method. In other words, this reduces the amount of generated
// object code.
//
class DummyCallback : public CallbackBase
{
public:

    DummyCallback()
    {
    }

    virtual void
    completed(const Ice::AsyncResultPtr&) const
    {
         assert(false);
    }

    virtual CallbackBasePtr
    verify(const Ice::LocalObjectPtr&)
    {
        //
        // Called by the AsyncResult constructor to verify the delegate. The dummy
        // delegate is passed when the user used a begin_ method without delegate.
        // By returning 0 here, we tell the AsyncResult that no delegates was
        // provided.
        //
        return 0;
    }

    virtual void
    sent(const AsyncResultPtr&) const
    {
         assert(false);
    }

    virtual bool
    hasSentCallback() const
    {
        assert(false);
        return false;
    }
};

}

//
// This gives a pointer value to compare against in the generated
// begin_ method to decide whether the caller passed a null pointer
// versus the generated inline version of the begin_ method having
// passed a pointer to the dummy delegate.
//
CallbackBasePtr IceInternal::__dummyCallback = ICE_MAKE_SHARED(DummyCallback);

void
IceInternal::CallbackBase::checkCallback(bool obj, bool cb)
{
    if(!obj)
    {
        throw IceUtil::IllegalArgumentException(__FILE__, __LINE__, "callback object cannot be null");
    }
    if(!cb)
    {
        throw IceUtil::IllegalArgumentException(__FILE__, __LINE__, "callback cannot be null");
    }
}

