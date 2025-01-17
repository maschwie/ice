// **********************************************************************
//
// Copyright (c) 2003-2015 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_REQUEST_HANDLER_H
#define ICE_REQUEST_HANDLER_H

#include <IceUtil/Shared.h>
#include <IceUtil/UniquePtr.h>

#include <Ice/RequestHandlerF.h>
#include <Ice/ReferenceF.h>
#include <Ice/OutgoingAsyncF.h>
#include <Ice/ProxyF.h>
#include <Ice/ConnectionIF.h>

namespace Ice
{

class LocalException;

};

namespace IceInternal
{

class OutgoingBase;
class ProxyOutgoingBase;

//
// An exception wrapper, which is used to notify that the request
// handler should be cleared and the invocation retried.
//
#ifdef ICE_CPP11_MAPPING
class RetryException
{
public:

    RetryException(std::exception_ptr);
    RetryException(const RetryException&);

    std::exception_ptr get() const;

private:

    std::exception_ptr _ex;
};
#else
class RetryException
{
public:

    RetryException(const Ice::LocalException&);
    RetryException(const RetryException&);

    const Ice::LocalException* get() const;

private:

    IceUtil::UniquePtr<Ice::LocalException> _ex;
};
#endif


class CancellationHandler
#ifndef ICE_CPP11_MAPPING
    : public virtual IceUtil::Shared
#endif
{
public:

    virtual void requestCanceled(OutgoingBase*, const Ice::LocalException&) = 0;
    virtual void asyncRequestCanceled(const OutgoingAsyncBasePtr&, const Ice::LocalException&) = 0;
};

class RequestHandler : public CancellationHandler
{
public:
    
    RequestHandler(const ReferencePtr&);

    virtual RequestHandlerPtr update(const RequestHandlerPtr&, const RequestHandlerPtr&) = 0;

    virtual bool sendRequest(ProxyOutgoingBase*) = 0;
    virtual AsyncStatus sendAsyncRequest(const ProxyOutgoingAsyncBasePtr&) = 0;

    const ReferencePtr& getReference() const { return _reference; } // Inlined for performances.

    virtual Ice::ConnectionIPtr getConnection() = 0;
    virtual Ice::ConnectionIPtr waitForConnection() = 0;

protected:

    const ReferencePtr _reference;
    const bool _response;
};

}

#endif
