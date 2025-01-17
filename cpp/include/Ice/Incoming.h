// **********************************************************************
//
// Copyright (c) 2003-2015 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_INCOMING_H
#define ICE_INCOMING_H

#include <Ice/InstanceF.h>
#include <Ice/ConnectionIF.h>
#include <Ice/ServantLocatorF.h>
#include <Ice/ServantManagerF.h>
#include <Ice/OutputStream.h>
#include <Ice/InputStream.h>
#include <Ice/Object.h>
#include <Ice/Current.h>
#include <Ice/IncomingAsyncF.h>
#include <Ice/ObserverHelper.h>
#include <Ice/ResponseHandlerF.h>

#include <deque>

namespace IceInternal
{

class ICE_API IncomingBase : private IceUtil::noncopyable
{
public:

    void __adopt(IncomingBase&);

    Ice::OutputStream* __startWriteParams(Ice::FormatType);
    void __endWriteParams(bool);
    void __writeEmptyParams();
    void __writeParamEncaps(const Ice::Byte*, Ice::Int, bool);
    void __writeUserException(const Ice::UserException&, Ice::FormatType);

protected:

    IncomingBase(Instance*, ResponseHandler*, Ice::Connection*, const Ice::ObjectAdapterPtr&, bool, Ice::Byte, 
                 Ice::Int);
    IncomingBase(IncomingBase&); // Adopts the argument. It must not be used afterwards.

    void __warning(const Ice::Exception&) const;
    void __warning(const std::string&) const;

    bool __servantLocatorFinished(bool);

    void __handleException(const std::exception&, bool);
    void __handleException(bool);

    Ice::Current _current;
    Ice::ObjectPtr _servant;
    Ice::ServantLocatorPtr _locator;
#ifdef ICE_CPP11_MAPPING
    ::std::shared_ptr<void> _cookie;
#else
    Ice::LocalObjectPtr _cookie;
#endif
    DispatchObserver _observer;
    bool _response;
    Ice::Byte _compress;

    Ice::OutputStream _os;

    //
    // Optimization. The request handler may not be deleted while a
    // stack-allocated Incoming still holds it.
    //
    ResponseHandler* _responseHandler;

    std::deque<Ice::DispatchInterceptorAsyncCallbackPtr> _interceptorAsyncCallbackQueue;
};

class ICE_API Incoming : public IncomingBase
{
public:

    Incoming(Instance*, ResponseHandler*, Ice::Connection*, const Ice::ObjectAdapterPtr&, bool, Ice::Byte, Ice::Int);

    const Ice::Current& getCurrent()
    {
        return _current;
    }

    void push(const Ice::DispatchInterceptorAsyncCallbackPtr&);
    void pop();
    void startOver();
    void killAsync();
    void setActive(IncomingAsync&);
    
    bool isRetriable()
    {
        return _inParamPos != 0;
    }

    void invoke(const ServantManagerPtr&, Ice::InputStream*);

    // Inlined for speed optimization.
    Ice::InputStream* startReadParams()
    {
        //
        // Remember the encoding used by the input parameters, we'll
        // encode the response parameters with the same encoding.
        //
        _current.encoding = _is->startEncapsulation();
        return _is;
    }
    void endReadParams() const
    {
        _is->endEncapsulation();
    }
    void readEmptyParams()
    {
        _current.encoding = _is->skipEmptyEncapsulation();
    }
    void readParamEncaps(const Ice::Byte*& v, Ice::Int& sz)
    {
        _current.encoding = _is->readEncapsulation(v, sz);
    }

private:

    Ice::InputStream* _is;
    
    IncomingAsyncPtr _cb;
    Ice::Byte* _inParamPos;
};

}

#endif
