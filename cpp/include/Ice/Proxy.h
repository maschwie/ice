// **********************************************************************
//
// Copyright (c) 2003-2015 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_PROXY_H
#define ICE_PROXY_H

#include <IceUtil/Shared.h>
#include <IceUtil/Mutex.h>
#include <Ice/ProxyF.h>
#include <Ice/ProxyFactoryF.h>
#include <Ice/ConnectionIF.h>
#include <Ice/RequestHandlerF.h>
#include <Ice/EndpointF.h>
#include <Ice/EndpointTypes.h>
#include <Ice/ObjectF.h>
#include <Ice/ObjectAdapterF.h>
#include <Ice/ReferenceF.h>
#include <Ice/BatchRequestQueueF.h>
#include <Ice/AsyncResult.h>
//#include <Ice/RouterF.h> // Can't include RouterF.h here, otherwise we have cyclic includes
//#include <Ice/LocatorF.h> // Can't include RouterF.h here, otherwise we have cyclic includes
#include <Ice/Current.h>
#include <Ice/CommunicatorF.h>
#include <Ice/ObserverHelper.h>
#include <Ice/LocalException.h>
#include <iosfwd>

namespace Ice
{
ICE_API extern const Context noExplicitContext;
}

namespace IceInternal
{

class Outgoing;

}

#ifdef ICE_CPP11_MAPPING // C++11 mapping

namespace IceInternal
{
template<typename P>
::std::shared_ptr<P> createProxy()
{
    return ::std::shared_ptr<P>(new P());
}

}

namespace Ice
{

class RouterPrx;
typedef ::std::shared_ptr<::Ice::RouterPrx> RouterPrxPtr;

class LocatorPrx;
typedef ::std::shared_ptr<::Ice::LocatorPrx> LocatorPrxPtr;

class LocalException;
class OutputStream;

class ICE_API ObjectPrx : public ::std::enable_shared_from_this<ObjectPrx>
{
public:

    virtual ~ObjectPrx() = default;

    bool operator==(const ObjectPrx&) const;
    bool operator!=(const ObjectPrx&) const;
    bool operator<(const ObjectPrx&) const;

    ::std::shared_ptr<::Ice::Communicator> ice_getCommunicator() const;

    ::std::string ice_toString() const;

    bool ice_isA(const ::std::string& typeId, const ::Ice::Context& context = ::Ice::noExplicitContext);

    ::std::function<void()>
    ice_isA_async(const ::std::string& typeId,
                  ::std::function<void (bool)> response,
                  ::std::function<void (::std::exception_ptr)> = nullptr,
                  ::std::function<void (bool)> sent = nullptr,
                  const ::Ice::Context& context = ::Ice::noExplicitContext);

    template<template<typename> class P = std::promise>
    auto ice_isA_async(const ::std::string& typeId, const ::Ice::Context& context = ::Ice::noExplicitContext)
        -> decltype(std::declval<P<bool>>().get_future())
    {
        auto promise = ::std::make_shared<P<bool>>();

        ice_isA_async(
            typeId,
            [promise](bool value)
            {
                promise->set_value(value);
            },
            [promise](::std::exception_ptr ex)
            {
                promise->set_exception(::std::move(ex));
            },
            nullptr, context);

        return promise->get_future();
    }

    void
    ice_ping(const ::Ice::Context& context = ::Ice::noExplicitContext);

    ::std::function<void()>
    ice_ping_async(::std::function<void ()> response,
                   ::std::function<void (::std::exception_ptr)> exception = nullptr,
                   ::std::function<void (bool)> sent = nullptr,
                   const ::Ice::Context& context = ::Ice::noExplicitContext);

    template<template<typename> class P = std::promise>
    auto ice_ping_async(const ::Ice::Context& context = ::Ice::noExplicitContext)
        -> decltype(std::declval<P<void>>().get_future())
    {
        auto promise = ::std::make_shared<P<void>>();
        if(ice_isTwoway())
        {
            ice_ping_async(
                [promise]()
                {
                    promise->set_value();
                },
                [promise](::std::exception_ptr ex)
                {
                    promise->set_exception(::std::move(ex));
                },
                nullptr, context);
        }
        else if(ice_isOneway() || ice_isDatagram())
        {
            ice_ping_async(
                nullptr,
                [promise](::std::exception_ptr ex)
                {
                    promise->set_exception(::std::move(ex));
                },
                [promise](bool)
                {
                    promise->set_value();
                },
                context);
        }
        else
        {
            ice_ping_async(nullptr, nullptr, nullptr, context);
            promise->set_value();
        }
        return promise->get_future();
    }

    ::std::vector<::std::string>
    ice_ids(const ::Ice::Context& context = ::Ice::noExplicitContext);

    ::std::function<void()>
    ice_ids_async(::std::function<void (::std::vector< ::std::string>)> response,
                  ::std::function<void (::std::exception_ptr)> exception = nullptr,
                  ::std::function<void (bool)> sent = nullptr,
                  const ::Ice::Context& context = ::Ice::noExplicitContext);

    template<template<typename> class P = std::promise>
    auto ice_ids_async(const ::Ice::Context& context = ::Ice::noExplicitContext)
        -> decltype(std::declval<P<::std::vector<::std::string>>>().get_future())
    {
        auto promise = ::std::make_shared<P<::std::vector<::std::string>>>();
        ice_ids_async(
            [promise](::std::vector<::std::string> ids)
            {
                promise->set_value(::std::move(ids));
            },
            [promise](::std::exception_ptr ex)
            {
                promise->set_exception(::std::move(ex));
            },
            nullptr, context);
        return promise->get_future();
    }

    ::std::string
    ice_id(const ::Ice::Context& context = ::Ice::noExplicitContext);

    ::std::function<void ()>
    ice_id_async(::std::function<void (::std::string)> response,
                 ::std::function<void (::std::exception_ptr)> exception = nullptr,
                 ::std::function<void (bool)> sent = nullptr,
                 const ::Ice::Context& context = ::Ice::noExplicitContext);

    template<template<typename> class P = std::promise>
    auto ice_id_async(const ::Ice::Context& context = ::Ice::noExplicitContext)
        -> decltype(std::declval<P<::std::string>>().get_future())
    {
        auto promise = ::std::make_shared<P<::std::string>>();
        ice_id_async(
            [promise](::std::string id)
            {
                promise->set_value(::std::move(id));
            },
            [promise](::std::exception_ptr ex)
            {
                promise->set_exception(::std::move(ex));
            },
            nullptr, context);
        return promise->get_future();
    }

    static const ::std::string& ice_staticId()
    {
        return ::Ice::Object::ice_staticId();
    }

    // Returns true if ok, false if user exception.
    bool
    ice_invoke(const ::std::string& operation,
               ::Ice::OperationMode mode,
               const ::std::vector< ::Ice::Byte>& inParams,
               ::std::vector< ::Ice::Byte>& outParams,
               const ::Ice::Context& context = ::Ice::noExplicitContext);
    
    bool
    ice_invoke(const ::std::string& operation,
               ::Ice::OperationMode mode,
               const ::std::pair<const ::Ice::Byte*, const ::Ice::Byte*>& inParams,
               ::std::vector< ::Ice::Byte>& outParams,
               const ::Ice::Context& context = ::Ice::noExplicitContext);

    ::std::function<void ()>
    ice_invoke_async(
        const ::std::string&,
        ::Ice::OperationMode,
        const ::std::vector<::Ice::Byte>&,
        ::std::function<void (bool, ::std::vector<::Ice::Byte>)> response,
        ::std::function<void (::std::exception_ptr)> exception = nullptr,
        ::std::function<void (bool)> sent = nullptr,
        const ::Ice::Context& context = ::Ice::noExplicitContext);
    
    struct Result_invoke1
    {
        bool ok;
        std::vector<::Ice::Byte> outParams;
    };
    
    template<template<typename> class P = std::promise>
    auto ice_invoke_async(
        const ::std::string& operation,
        ::Ice::OperationMode mode,
        const ::std::vector<::Ice::Byte>& inParams,
        const ::Ice::Context& context = ::Ice::noExplicitContext)
        -> decltype(std::declval<P<Result_invoke1>>().get_future())
    {
        auto promise = ::std::make_shared<P<Result_invoke1>>();
        if(ice_isTwoway())
        {
            ice_invoke_async(operation, mode, inParams,
                [promise](bool ok, ::std::vector<::Ice::Byte> outParams)
                {
                    Result_invoke1 result = {ok, move(outParams)};
                    promise->set_value(::std::move(result));
                },
                [promise](::std::exception_ptr ex)
                {
                    promise->set_exception(::std::move(ex));
                },
                nullptr, context);
        }
        else if(ice_isOneway() || ice_isDatagram())
        {
            ice_invoke_async(operation, mode, inParams,
                nullptr,
                [promise](::std::exception_ptr ex)
                {
                    promise->set_exception(::std::move(ex));
                },
                [promise](bool)
                {
                    Result_invoke1 result;
                    result.ok = true;
                    promise->set_value(::std::move(result));
                },
                context);
        }
        else // Batch request
        {
            ice_invoke_async(operation, mode, inParams, nullptr, nullptr, nullptr, context);
            Result_invoke1 result;
            result.ok = true;
            promise->set_value(::std::move(result));
        }
        return promise->get_future();
    }
    
    ::std::function<void ()>
    ice_invoke_async(
        const ::std::string&,
        ::Ice::OperationMode,
        const ::std::pair<const ::Ice::Byte*, const ::Ice::Byte*>& inParams,
        ::std::function<void (bool, ::std::pair<const ::Ice::Byte*, const ::Ice::Byte*>)> response,
        ::std::function<void (::std::exception_ptr)> exception = nullptr,
        ::std::function<void (bool)> sent = nullptr,
        const ::Ice::Context& context = ::Ice::noExplicitContext);
    
    struct Result_invoke2
    {
        bool ok;
        std::pair<const ::Ice::Byte*, const ::Ice::Byte*> outParams;
    };
    
    template<template<typename> class P = std::promise>
    auto ice_invoke_async(
        const ::std::string& operation,
        ::Ice::OperationMode mode,
        const ::std::pair<const ::Ice::Byte*, const ::Ice::Byte*>& inParams,
        const ::Ice::Context& context = ::Ice::noExplicitContext)
        -> decltype(std::declval<P<Result_invoke2>>().get_future())
    {
        auto promise = ::std::make_shared<P<Result_invoke2>>();
        if(ice_isTwoway())
        {
            ice_invoke_async(operation, mode, inParams,
                [promise](bool ok, ::std::pair<const ::Ice::Byte*, const ::Ice::Byte*> outParams)
                {
                    Result_invoke2 result = {ok, move(outParams)};
                    promise->set_value(::std::move(result));
                },
                [promise](::std::exception_ptr ex)
                {
                    promise->set_exception(::std::move(ex));
                },
                nullptr, context);
        }
        else if(ice_isOneway() || ice_isDatagram())
        {
            ice_invoke_async(operation, mode, inParams,
                nullptr,
                [promise](::std::exception_ptr ex)
                {
                    promise->set_exception(::std::move(ex));
                },
                [promise](bool)
                {
                    Result_invoke2 result;
                    result.ok = true;
                    promise->set_value(::std::move(result));
                },
                context);
        }
        else // Batch request
        {
            ice_invoke_async(operation, mode, inParams, nullptr, nullptr, nullptr, context);
            Result_invoke2 result;
            result.ok = true;
            promise->set_value(::std::move(result));
        }
        return promise->get_future();
    }

    ::Ice::Identity ice_getIdentity() const;
    ::std::shared_ptr<::Ice::ObjectPrx> ice_identity(const ::Ice::Identity&) const;

    ::Ice::Context ice_getContext() const;
    ::std::shared_ptr<::Ice::ObjectPrx> ice_context(const ::Ice::Context&) const;

    const ::std::string& ice_getFacet() const;
    ::std::shared_ptr<::Ice::ObjectPrx> ice_facet(const ::std::string&) const;

    ::std::string ice_getAdapterId() const;
    ::std::shared_ptr<::Ice::ObjectPrx> ice_adapterId(const ::std::string&) const;

    ::Ice::EndpointSeq ice_getEndpoints() const;
    ::std::shared_ptr<::Ice::ObjectPrx> ice_endpoints(const ::Ice::EndpointSeq&) const;

    ::Ice::Int ice_getLocatorCacheTimeout() const;
    ::std::shared_ptr<::Ice::ObjectPrx> ice_locatorCacheTimeout(::Ice::Int) const;

    bool ice_isConnectionCached() const;
    ::std::shared_ptr<::Ice::ObjectPrx> ice_connectionCached(bool) const;

    ::Ice::EndpointSelectionType ice_getEndpointSelection() const;
    ::std::shared_ptr<::Ice::ObjectPrx> ice_endpointSelection(::Ice::EndpointSelectionType) const;

    bool ice_isSecure() const;
    ::std::shared_ptr<::Ice::ObjectPrx> ice_secure(bool) const;

    ::Ice::EncodingVersion ice_getEncodingVersion() const;
    ::std::shared_ptr<::Ice::ObjectPrx> ice_encodingVersion(const ::Ice::EncodingVersion&) const;

    bool ice_isPreferSecure() const;
    ::std::shared_ptr<::Ice::ObjectPrx> ice_preferSecure(bool) const;

    ::std::shared_ptr<::Ice::RouterPrx> ice_getRouter() const;
    ::std::shared_ptr<::Ice::ObjectPrx> ice_router(const ::std::shared_ptr<::Ice::RouterPrx>&) const;

    ::std::shared_ptr<::Ice::LocatorPrx> ice_getLocator() const;
    ::std::shared_ptr<::Ice::ObjectPrx> ice_locator(const ::std::shared_ptr<::Ice::LocatorPrx>&) const;

    bool ice_isCollocationOptimized() const;
    ::std::shared_ptr<::Ice::ObjectPrx> ice_collocationOptimized(bool) const;

    ::Ice::Int ice_getInvocationTimeout() const;
    ::std::shared_ptr<::Ice::ObjectPrx> ice_invocationTimeout(::Ice::Int) const;

    ::std::shared_ptr<::Ice::ObjectPrx> ice_twoway() const;
    bool ice_isTwoway() const;
    ::std::shared_ptr<::Ice::ObjectPrx> ice_oneway() const;
    bool ice_isOneway() const;
    ::std::shared_ptr<::Ice::ObjectPrx> ice_batchOneway() const;
    bool ice_isBatchOneway() const;
    ::std::shared_ptr<::Ice::ObjectPrx> ice_datagram() const;
    bool ice_isDatagram() const;
    ::std::shared_ptr<::Ice::ObjectPrx> ice_batchDatagram() const;
    bool ice_isBatchDatagram() const;

    ::std::shared_ptr<::Ice::ObjectPrx> ice_compress(bool) const;
    ::std::shared_ptr<::Ice::ObjectPrx> ice_timeout(int) const;

    ::std::shared_ptr<::Ice::ObjectPrx> ice_connectionId(const ::std::string&) const;
    ::std::string ice_getConnectionId() const;


    ::std::shared_ptr<::Ice::Connection> ice_getConnection();

    ::std::function<void ()>
    ice_getConnection_async(
        ::std::function<void (::std::shared_ptr<::Ice::Connection>)> response,
        ::std::function<void (::std::exception_ptr)> exception = nullptr,
        ::std::function<void (bool)> sent = nullptr);


    template<template<typename> class P = std::promise>
    auto ice_getConnection_async()
        -> decltype(std::declval<P<::std::shared_ptr<::Ice::Connection>>>().get_future())
    {
        auto promise = ::std::make_shared<P<::std::shared_ptr<::Ice::Connection>>>();
        ice_getConnection_async(
            [promise](::std::shared_ptr<::Ice::Connection> connection)
            {
                promise->set_value(::std::move(connection));
            },
            [promise](::std::exception_ptr ex)
            {
                promise->set_exception(::std::move(ex));
            });
        return promise->get_future();
    }

    ::std::shared_ptr<::Ice::Connection> ice_getCachedConnection() const;

    void ice_flushBatchRequests();

    std::function<void ()>
    ice_flushBatchRequests_async(
        ::std::function<void (::std::exception_ptr)> exception,
        ::std::function<void (bool)> sent = nullptr);


    template<template<typename> class P = std::promise>
    auto ice_flushBatchRequests_async()
        -> decltype(std::declval<P<bool>>().get_future())
    {
        auto promise = ::std::make_shared<P<bool>>();
        ice_flushBatchRequests_async(
            [promise](::std::exception_ptr ex)
            {
                promise->set_exception(::std::move(ex));
            },
            [promise](bool sent)
            {
                promise->set_value(sent);
            });
        return promise->get_future();
    }

    const ::IceInternal::ReferencePtr& __reference() const { return _reference; }

    void __copyFrom(const std::shared_ptr<::Ice::ObjectPrx>&);

    int __handleException(const ::Ice::Exception&, const ::IceInternal::RequestHandlerPtr&, ::Ice::OperationMode,
                          bool, int&);

    void __checkAsyncTwowayOnly(const ::std::string&) const;

    ::IceInternal::RequestHandlerPtr __getRequestHandler();
    ::IceInternal::BatchRequestQueuePtr __getBatchRequestQueue();
    ::IceInternal::RequestHandlerPtr __setRequestHandler(const ::IceInternal::RequestHandlerPtr&);
    void __updateRequestHandler(const ::IceInternal::RequestHandlerPtr&, const ::IceInternal::RequestHandlerPtr&);

    int __hash() const;

    void __write(OutputStream&) const;

protected:

    virtual ::std::shared_ptr<ObjectPrx> __newInstance() const;
    ObjectPrx() = default;
    friend ::std::shared_ptr<ObjectPrx> IceInternal::createProxy<ObjectPrx>();

private:

    void setup(const ::IceInternal::ReferencePtr&);
    friend class ::IceInternal::ProxyFactory;

    ::IceInternal::ReferencePtr _reference;
    ::IceInternal::RequestHandlerPtr _requestHandler;
    ::IceInternal::BatchRequestQueuePtr _batchRequestQueue;
    IceUtil::Mutex _mutex;
};

template<typename Prx, typename... Bases>
class Proxy : public virtual Bases...
{
public:

    ::std::shared_ptr<Prx> ice_context(const ::Ice::Context& context) const
    {
        return ::std::dynamic_pointer_cast<Prx>(ObjectPrx::ice_context(context));
    }

    ::std::shared_ptr<Prx> ice_adapterId(const ::std::string& id) const
    {
        return ::std::dynamic_pointer_cast<Prx>(ObjectPrx::ice_adapterId(id));
    }

    ::std::shared_ptr<Prx> ice_endpoints(const ::Ice::EndpointSeq& endpoints) const
    {
        return ::std::dynamic_pointer_cast<Prx>(ObjectPrx::ice_endpoints(endpoints));
    }

    ::std::shared_ptr<Prx> ice_locatorCacheTimeout(int timeout) const
    {
        return ::std::dynamic_pointer_cast<Prx>(ObjectPrx::ice_locatorCacheTimeout(timeout));
    }

    ::std::shared_ptr<Prx> ice_connectionCached(bool cached) const
    {
        return ::std::dynamic_pointer_cast<Prx>(ObjectPrx::ice_connectionCached(cached));
    }

    ::std::shared_ptr<Prx> ice_endpointSelection(::Ice::EndpointSelectionType selection) const
    {
        return ::std::dynamic_pointer_cast<Prx>(ObjectPrx::ice_endpointSelection(selection));
    }

    ::std::shared_ptr<Prx> ice_secure(bool secure) const
    {
        return ::std::dynamic_pointer_cast<Prx>(ObjectPrx::ice_secure(secure));
    }

    ::std::shared_ptr<Prx> ice_preferSecure(bool preferSecure) const
    {
        return ::std::dynamic_pointer_cast<Prx>(ObjectPrx::ice_preferSecure(preferSecure));
    }

    ::std::shared_ptr<Prx> ice_router(const ::std::shared_ptr<::Ice::RouterPrx>& router) const
    {
        return ::std::dynamic_pointer_cast<Prx>(ObjectPrx::ice_router(router));
    }

    ::std::shared_ptr<Prx> ice_locator(const ::std::shared_ptr<::Ice::LocatorPrx>& locator) const
    {
        return ::std::dynamic_pointer_cast<Prx>(ObjectPrx::ice_locator(locator));
    }

    ::std::shared_ptr<Prx> ice_collocationOptimized(bool collocated) const
    {
        return ::std::dynamic_pointer_cast<Prx>(ObjectPrx::ice_collocationOptimized(collocated));
    }

    ::std::shared_ptr<Prx> ice_invocationTimeout(int timeout) const
    {
        return ::std::dynamic_pointer_cast<Prx>(ObjectPrx::ice_invocationTimeout(timeout));
    }

    ::std::shared_ptr<Prx> ice_twoway() const
    {
        return ::std::dynamic_pointer_cast<Prx>(ObjectPrx::ice_twoway());
    }

    ::std::shared_ptr<Prx> ice_oneway() const
    {
        return ::std::dynamic_pointer_cast<Prx>(ObjectPrx::ice_oneway());
    }

    ::std::shared_ptr<Prx> ice_batchOneway() const
    {
        return ::std::dynamic_pointer_cast<Prx>(ObjectPrx::ice_batchOneway());
    }

    ::std::shared_ptr<Prx> ice_datagram() const
    {
        return ::std::dynamic_pointer_cast<Prx>(ObjectPrx::ice_datagram());
    }

    ::std::shared_ptr<Prx> ice_batchDatagram() const
    {
        return ::std::dynamic_pointer_cast<Prx>(ObjectPrx::ice_batchDatagram());
    }

    ::std::shared_ptr<Prx> ice_compress(bool compress) const
    {
        return ::std::dynamic_pointer_cast<Prx>(ObjectPrx::ice_compress(compress));
    }

    ::std::shared_ptr<Prx> ice_timeout(int timeout) const
    {
        return ::std::dynamic_pointer_cast<Prx>(ObjectPrx::ice_timeout(timeout));
    }

    ::std::shared_ptr<Prx> ice_connectionId(const ::std::string& id) const
    {
        return ::std::dynamic_pointer_cast<Prx>(ObjectPrx::ice_connectionId(id));
    }

    ::std::shared_ptr<Prx> ice_encodingVersion(const ::Ice::EncodingVersion& version) const
    {
        return ::std::dynamic_pointer_cast<Prx>(ObjectPrx::ice_encodingVersion(version));
    }
    
protected:
    
    virtual ::std::shared_ptr<ObjectPrx> __newInstance() const
    {
        return IceInternal::createProxy<Prx>();
    }
};

ICE_API ::std::ostream& operator<<(::std::ostream&, const ::Ice::ObjectPrx&);

ICE_API bool proxyIdentityLess(const ::std::shared_ptr<ObjectPrx>&, const ::std::shared_ptr<ObjectPrx>&);
ICE_API bool proxyIdentityEqual(const ::std::shared_ptr<ObjectPrx>&, const ::std::shared_ptr<ObjectPrx>&);

ICE_API bool proxyIdentityAndFacetLess(const ::std::shared_ptr<ObjectPrx>&, const ::std::shared_ptr<ObjectPrx>&);
ICE_API bool proxyIdentityAndFacetEqual(const ::std::shared_ptr<ObjectPrx>&, const ::std::shared_ptr<ObjectPrx>&);

struct ProxyIdentityLess : std::binary_function<bool, ::std::shared_ptr<ObjectPrx>&, ::std::shared_ptr<ObjectPrx>&>
{
    bool operator()(const ::std::shared_ptr<ObjectPrx>& lhs, const ::std::shared_ptr<ObjectPrx>& rhs) const
    {
        return proxyIdentityLess(lhs, rhs);
    }
};

struct ProxyIdentityEqual : std::binary_function<bool, ::std::shared_ptr<ObjectPrx>&, ::std::shared_ptr<ObjectPrx>&>
{
    bool operator()(const ::std::shared_ptr<ObjectPrx>& lhs, const ::std::shared_ptr<ObjectPrx>& rhs) const
    {
        return proxyIdentityEqual(lhs, rhs);
    }
};

struct ProxyIdentityAndFacetLess : std::binary_function<bool, ::std::shared_ptr<ObjectPrx>&, ::std::shared_ptr<ObjectPrx>&>
{
    bool operator()(const ::std::shared_ptr<ObjectPrx>& lhs, const ::std::shared_ptr<ObjectPrx>& rhs) const
    {
        return proxyIdentityAndFacetLess(lhs, rhs);
    }
};

struct ProxyIdentityAndFacetEqual : std::binary_function<bool, ::std::shared_ptr<ObjectPrx>&, ::std::shared_ptr<ObjectPrx>&>
{
    bool operator()(const ::std::shared_ptr<ObjectPrx>& lhs, const ::std::shared_ptr<ObjectPrx>& rhs) const
    {
        return proxyIdentityAndFacetEqual(lhs, rhs);
    }
};

template<typename P,
         typename T,
         typename ::std::enable_if<::std::is_base_of<::Ice::ObjectPrx, P>::value>::type* = nullptr,
         typename ::std::enable_if<::std::is_base_of<::Ice::ObjectPrx, T>::value>::type* = nullptr> ::std::shared_ptr<P>
uncheckedCast(const ::std::shared_ptr<T>& b)
{
    ::std::shared_ptr<P> r;
    if(b)
    {
        r = ::std::dynamic_pointer_cast<P>(b);
        if(!r)
        {
            r = IceInternal::createProxy<P>();
            r->__copyFrom(b);
        }
    }
    return r;
}

template<typename P,
         typename T,
         typename ::std::enable_if<::std::is_base_of<::Ice::ObjectPrx, P>::value>::type* = nullptr,
         typename ::std::enable_if<::std::is_base_of<::Ice::ObjectPrx, T>::value>::type* = nullptr> ::std::shared_ptr<P>
uncheckedCast(const ::std::shared_ptr<T>& b, const std::string& f)
{
    ::std::shared_ptr<P> r;
    if(b)
    {
        r = IceInternal::createProxy<P>();
        r->__copyFrom(b->ice_facet(f));
    }
    return r;
}

template<typename P,
         typename T,
         typename ::std::enable_if<::std::is_base_of<::Ice::ObjectPrx, P>::value>::type* = nullptr,
         typename ::std::enable_if<::std::is_base_of<::Ice::ObjectPrx, T>::value>::type* = nullptr> ::std::shared_ptr<P>
checkedCast(const ::std::shared_ptr<T>& b, const ::Ice::Context& context = Ice::noExplicitContext)
{
    ::std::shared_ptr<P> r;
    if(b)
    {
        if(b->ice_isA(P::ice_staticId(), context))
        {
            r = IceInternal::createProxy<P>();
            r->__copyFrom(b);
        }
    }
    return r;
}

template<typename P,
         typename T,
         typename ::std::enable_if<::std::is_base_of<::Ice::ObjectPrx, P>::value>::type* = nullptr,
         typename ::std::enable_if<::std::is_base_of<::Ice::ObjectPrx, T>::value>::type* = nullptr> ::std::shared_ptr<P>
checkedCast(const ::std::shared_ptr<T>& b, const std::string& f, const ::Ice::Context& context = Ice::noExplicitContext)
{
    ::std::shared_ptr<P> r;
    if(b)
    {
        try
        {
            ::std::shared_ptr<::Ice::ObjectPrx> bb = b->ice_facet(f);
            if(bb->ice_isA(P::ice_staticId(), context))
            {
                r = IceInternal::createProxy<P>();
                r->__copyFrom(bb);
            }
        }
        catch(const Ice::FacetNotExistException&)
        {
        }
    }
    return r;
}

ICE_API ::std::ostream& operator<<(::std::ostream&, const Ice::ObjectPrx&);

}

#else // C++98 mapping

namespace IceProxy
{

namespace Ice
{

class Locator;
ICE_API ::IceProxy::Ice::Object* upCast(::IceProxy::Ice::Locator*);

class Router;
ICE_API ::IceProxy::Ice::Object* upCast(::IceProxy::Ice::Router*);

}

}

namespace Ice
{

typedef ::IceInternal::ProxyHandle< ::IceProxy::Ice::Router> RouterPrx;
typedef RouterPrx RouterPrxPtr;
typedef ::IceInternal::ProxyHandle< ::IceProxy::Ice::Locator> LocatorPrx;
typedef LocatorPrx LocatorPrxPtr;

class LocalException;
class OutputStream;

class Callback_Object_ice_isA_Base : public virtual ::IceInternal::CallbackBase { };
typedef ::IceUtil::Handle< Callback_Object_ice_isA_Base> Callback_Object_ice_isAPtr;

class Callback_Object_ice_ping_Base : public virtual ::IceInternal::CallbackBase { };
typedef ::IceUtil::Handle< Callback_Object_ice_ping_Base> Callback_Object_ice_pingPtr;

class Callback_Object_ice_ids_Base : public virtual ::IceInternal::CallbackBase { };
typedef ::IceUtil::Handle< Callback_Object_ice_ids_Base> Callback_Object_ice_idsPtr;

class Callback_Object_ice_id_Base : public virtual ::IceInternal::CallbackBase { };
typedef ::IceUtil::Handle< Callback_Object_ice_id_Base> Callback_Object_ice_idPtr;

class Callback_Object_ice_invoke_Base : public virtual ::IceInternal::CallbackBase { };
typedef ::IceUtil::Handle< Callback_Object_ice_invoke_Base> Callback_Object_ice_invokePtr;

class Callback_Object_ice_flushBatchRequests_Base : public virtual ::IceInternal::CallbackBase { };
typedef ::IceUtil::Handle< Callback_Object_ice_flushBatchRequests_Base> Callback_Object_ice_flushBatchRequestsPtr;

class Callback_Object_ice_getConnection_Base : public virtual ::IceInternal::CallbackBase { };
typedef ::IceUtil::Handle< Callback_Object_ice_getConnection_Base> Callback_Object_ice_getConnectionPtr;

}

namespace IceProxy { namespace Ice
{

class ICE_API Object : public ::IceUtil::Shared
{
public:

    bool operator==(const Object&) const;
    bool operator!=(const Object&) const;
    bool operator<(const Object&) const;

    ::Ice::CommunicatorPtr ice_getCommunicator() const;

    ::std::string ice_toString() const;

    bool ice_isA(const ::std::string& typeId, const ::Ice::Context& = ::Ice::noExplicitContext);

    ::Ice::AsyncResultPtr begin_ice_isA(const ::std::string& typeId,
                                        const ::Ice::Context& __ctx = ::Ice::noExplicitContext)
    {
        return __begin_ice_isA(typeId, __ctx, ::IceInternal::__dummyCallback, 0);
    }

    ::Ice::AsyncResultPtr begin_ice_isA(const ::std::string& typeId,
                                        const ::Ice::CallbackPtr& __del,
                                        const ::Ice::LocalObjectPtr& __cookie = 0)
    {
        return __begin_ice_isA(typeId, ::Ice::noExplicitContext, __del, __cookie);
    }

    ::Ice::AsyncResultPtr begin_ice_isA(const ::std::string& typeId,
                                        const ::Ice::Context& __ctx,
                                        const ::Ice::CallbackPtr& __del,
                                        const ::Ice::LocalObjectPtr& __cookie = 0)
    {
        return __begin_ice_isA(typeId, __ctx, __del, __cookie);
    }

    ::Ice::AsyncResultPtr begin_ice_isA(const ::std::string& typeId,
                                        const ::Ice::Callback_Object_ice_isAPtr& __del,
                                        const ::Ice::LocalObjectPtr& __cookie = 0)
    {
        return __begin_ice_isA(typeId, ::Ice::noExplicitContext, __del, __cookie);
    }

    ::Ice::AsyncResultPtr begin_ice_isA(const ::std::string& typeId,
                                        const ::Ice::Context& __ctx,
                                        const ::Ice::Callback_Object_ice_isAPtr& __del,
                                        const ::Ice::LocalObjectPtr& __cookie = 0)
    {
        return __begin_ice_isA(typeId, __ctx, __del, __cookie);
    }

    bool end_ice_isA(const ::Ice::AsyncResultPtr&);

    void ice_ping(const ::Ice::Context& = ::Ice::noExplicitContext);

    ::Ice::AsyncResultPtr begin_ice_ping(const ::Ice::Context& __ctx = ::Ice::noExplicitContext)
    {
        return __begin_ice_ping(__ctx, ::IceInternal::__dummyCallback, 0);
    }

    ::Ice::AsyncResultPtr begin_ice_ping(const ::Ice::CallbackPtr& __del, const ::Ice::LocalObjectPtr& __cookie = 0)
    {
        return __begin_ice_ping(::Ice::noExplicitContext, __del, __cookie);
    }

    ::Ice::AsyncResultPtr begin_ice_ping(const ::Ice::Context& __ctx, const ::Ice::CallbackPtr& __del,
                                         const ::Ice::LocalObjectPtr& __cookie = 0)
    {
        return __begin_ice_ping(__ctx, __del, __cookie);
    }


    ::Ice::AsyncResultPtr begin_ice_ping(const ::Ice::Callback_Object_ice_pingPtr& __del,
                                         const ::Ice::LocalObjectPtr& __cookie = 0)
    {
        return __begin_ice_ping(::Ice::noExplicitContext, __del, __cookie);
    }

    ::Ice::AsyncResultPtr begin_ice_ping(const ::Ice::Context& __ctx, const ::Ice::Callback_Object_ice_pingPtr& __del,
                                         const ::Ice::LocalObjectPtr& __cookie = 0)
    {
        return __begin_ice_ping(__ctx, __del, __cookie);
    }

    void end_ice_ping(const ::Ice::AsyncResultPtr&);

    ::std::vector< ::std::string> ice_ids(const ::Ice::Context& context = ::Ice::noExplicitContext);

    ::Ice::AsyncResultPtr begin_ice_ids(const ::Ice::Context& __ctx = ::Ice::noExplicitContext)
    {
        return __begin_ice_ids(__ctx, ::IceInternal::__dummyCallback, 0);
    }

    ::Ice::AsyncResultPtr begin_ice_ids(const ::Ice::CallbackPtr& __del,
                                        const ::Ice::LocalObjectPtr& __cookie = 0)
    {
        return __begin_ice_ids(::Ice::noExplicitContext, __del, __cookie);
    }

    ::Ice::AsyncResultPtr begin_ice_ids(const ::Ice::Context& __ctx,
                                        const ::Ice::CallbackPtr& __del,
                                        const ::Ice::LocalObjectPtr& __cookie = 0)
    {
        return __begin_ice_ids(__ctx, __del, __cookie);
    }

    ::Ice::AsyncResultPtr begin_ice_ids(const ::Ice::Callback_Object_ice_idsPtr& __del,
                                        const ::Ice::LocalObjectPtr& __cookie = 0)
    {
        return __begin_ice_ids(::Ice::noExplicitContext, __del, __cookie);
    }

    ::Ice::AsyncResultPtr begin_ice_ids(const ::Ice::Context& __ctx,
                                        const ::Ice::Callback_Object_ice_idsPtr& __del,
                                        const ::Ice::LocalObjectPtr& __cookie = 0)
    {
        return __begin_ice_ids(__ctx, __del, __cookie);
    }

    ::std::vector< ::std::string> end_ice_ids(const ::Ice::AsyncResultPtr&);

    ::std::string ice_id(const ::Ice::Context& context = ::Ice::noExplicitContext);

    ::Ice::AsyncResultPtr begin_ice_id(const ::Ice::Context& __ctx = ::Ice::noExplicitContext)
    {
        return __begin_ice_id(__ctx, ::IceInternal::__dummyCallback, 0);
    }

    ::Ice::AsyncResultPtr begin_ice_id(const ::Ice::CallbackPtr& __del,
                                       const ::Ice::LocalObjectPtr& __cookie = 0)
    {
        return __begin_ice_id(::Ice::noExplicitContext, __del, __cookie);
    }

    ::Ice::AsyncResultPtr begin_ice_id(const ::Ice::Context& __ctx,
                                       const ::Ice::CallbackPtr& __del,
                                       const ::Ice::LocalObjectPtr& __cookie = 0)
    {
        return __begin_ice_id(__ctx, __del, __cookie);
    }

    ::Ice::AsyncResultPtr begin_ice_id(const ::Ice::Callback_Object_ice_idPtr& __del,
                                       const ::Ice::LocalObjectPtr& __cookie = 0)
    {
        return __begin_ice_id(::Ice::noExplicitContext, __del, __cookie);
    }

    ::Ice::AsyncResultPtr begin_ice_id(const ::Ice::Context& __ctx,
                                       const ::Ice::Callback_Object_ice_idPtr& __del,
                                       const ::Ice::LocalObjectPtr& __cookie = 0)
    {
        return __begin_ice_id(__ctx, __del, __cookie);
    }

    ::std::string end_ice_id(const ::Ice::AsyncResultPtr&);


    static const ::std::string& ice_staticId()
    {
        return ::Ice::Object::ice_staticId();
    }


    // Returns true if ok, false if user exception.
    bool ice_invoke(const ::std::string&,
                    ::Ice::OperationMode,
                    const ::std::vector< ::Ice::Byte>&,
                    ::std::vector< ::Ice::Byte>&,
                    const ::Ice::Context& context = ::Ice::noExplicitContext);

    ::Ice::AsyncResultPtr begin_ice_invoke(const ::std::string& operation,
                                           ::Ice::OperationMode mode,
                                           const ::std::vector< ::Ice::Byte>& inParams)
    {
        return __begin_ice_invoke(operation, mode, inParams, ::Ice::noExplicitContext, ::IceInternal::__dummyCallback, 0);
    }

    ::Ice::AsyncResultPtr begin_ice_invoke(const ::std::string& operation,
                                           ::Ice::OperationMode mode,
                                           const ::std::vector< ::Ice::Byte>& inParams,
                                           const ::Ice::Context& __ctx)
    {
        return __begin_ice_invoke(operation, mode, inParams, __ctx, ::IceInternal::__dummyCallback, 0);
    }

    ::Ice::AsyncResultPtr begin_ice_invoke(const ::std::string& operation,
                                           ::Ice::OperationMode mode,
                                           const ::std::vector< ::Ice::Byte>& inParams,
                                           const ::Ice::CallbackPtr& __del,
                                           const ::Ice::LocalObjectPtr& __cookie = 0)
    {
        return __begin_ice_invoke(operation, mode, inParams, ::Ice::noExplicitContext, __del, __cookie);
    }

    ::Ice::AsyncResultPtr begin_ice_invoke(const ::std::string& operation,
                                           ::Ice::OperationMode mode,
                                           const ::std::vector< ::Ice::Byte>& inParams,
                                           const ::Ice::Context& __ctx,
                                           const ::Ice::CallbackPtr& __del,
                                           const ::Ice::LocalObjectPtr& __cookie = 0)
    {
        return __begin_ice_invoke(operation, mode, inParams, __ctx, __del, __cookie);
    }

    ::Ice::AsyncResultPtr begin_ice_invoke(const ::std::string& operation,
                                           ::Ice::OperationMode mode,
                                           const ::std::vector< ::Ice::Byte>& inParams,
                                           const ::Ice::Callback_Object_ice_invokePtr& __del,
                                           const ::Ice::LocalObjectPtr& __cookie = 0)
    {
        return __begin_ice_invoke(operation, mode, inParams, ::Ice::noExplicitContext, __del, __cookie);
    }

    ::Ice::AsyncResultPtr begin_ice_invoke(const ::std::string& operation,
                                           ::Ice::OperationMode mode,
                                           const ::std::vector< ::Ice::Byte>& inParams,
                                           const ::Ice::Context& __ctx,
                                           const ::Ice::Callback_Object_ice_invokePtr& __del,
                                           const ::Ice::LocalObjectPtr& __cookie = 0)
    {
        return __begin_ice_invoke(operation, mode, inParams, __ctx, __del, __cookie);
    }

    bool end_ice_invoke(::std::vector< ::Ice::Byte>&, const ::Ice::AsyncResultPtr&);

    bool ice_invoke(const ::std::string&,
                    ::Ice::OperationMode,
                    const ::std::pair<const ::Ice::Byte*, const ::Ice::Byte*>&,
                    ::std::vector< ::Ice::Byte>&,
                    const ::Ice::Context& = ::Ice::noExplicitContext);

    ::Ice::AsyncResultPtr begin_ice_invoke(const ::std::string& operation,
                                           ::Ice::OperationMode mode,
                                           const ::std::pair<const ::Ice::Byte*, const ::Ice::Byte*>& inParams)
    {
        return __begin_ice_invoke(operation, mode, inParams, ::Ice::noExplicitContext, ::IceInternal::__dummyCallback, 0);
    }

    ::Ice::AsyncResultPtr begin_ice_invoke(const ::std::string& operation,
                                           ::Ice::OperationMode mode,
                                           const ::std::pair<const ::Ice::Byte*, const ::Ice::Byte*>& inParams,
                                           const ::Ice::Context& __ctx,
                                           const ::Ice::LocalObjectPtr& __cookie = 0)
    {
        return __begin_ice_invoke(operation, mode, inParams, __ctx, ::IceInternal::__dummyCallback, __cookie);
    }

    ::Ice::AsyncResultPtr begin_ice_invoke(const ::std::string& operation,
                                           ::Ice::OperationMode mode,
                                           const ::std::pair<const ::Ice::Byte*, const ::Ice::Byte*>& inParams,
                                           const ::Ice::CallbackPtr& __del,
                                           const ::Ice::LocalObjectPtr& __cookie = 0)
    {
        return __begin_ice_invoke(operation, mode, inParams, ::Ice::noExplicitContext, __del, __cookie);
    }

    ::Ice::AsyncResultPtr begin_ice_invoke(const ::std::string& operation,
                                           ::Ice::OperationMode mode,
                                           const ::std::pair<const ::Ice::Byte*, const ::Ice::Byte*>& inParams,
                                           const ::Ice::Context& __ctx,
                                           const ::Ice::CallbackPtr& __del,
                                           const ::Ice::LocalObjectPtr& __cookie = 0)
    {
        return __begin_ice_invoke(operation, mode, inParams, __ctx, __del, __cookie);
    }

    ::Ice::AsyncResultPtr begin_ice_invoke(const ::std::string& operation,
                                           ::Ice::OperationMode mode,
                                           const ::std::pair<const ::Ice::Byte*, const ::Ice::Byte*>& inParams,
                                           const ::Ice::Callback_Object_ice_invokePtr& __del,
                                           const ::Ice::LocalObjectPtr& __cookie = 0)
    {
        return __begin_ice_invoke(operation, mode, inParams, ::Ice::noExplicitContext, __del, __cookie);
    }

    ::Ice::AsyncResultPtr begin_ice_invoke(const ::std::string& operation,
                                           ::Ice::OperationMode mode,
                                           const ::std::pair<const ::Ice::Byte*, const ::Ice::Byte*>& inParams,
                                           const ::Ice::Context& __ctx,
                                           const ::Ice::Callback_Object_ice_invokePtr& __del,
                                           const ::Ice::LocalObjectPtr& __cookie = 0)
    {
        return __begin_ice_invoke(operation, mode, inParams, __ctx, __del, __cookie);
    }

    bool ___end_ice_invoke(::std::pair<const ::Ice::Byte*, const ::Ice::Byte*>&, const ::Ice::AsyncResultPtr&);

    ::Ice::Identity ice_getIdentity() const;
    ::Ice::ObjectPrx ice_identity(const ::Ice::Identity&) const;

    ::Ice::Context ice_getContext() const;
    ::Ice::ObjectPrx ice_context(const ::Ice::Context&) const;

    const ::std::string& ice_getFacet() const;
    ::Ice::ObjectPrx ice_facet(const ::std::string&) const;

    ::std::string ice_getAdapterId() const;
    ::Ice::ObjectPrx ice_adapterId(const ::std::string&) const;

    ::Ice::EndpointSeq ice_getEndpoints() const;
    ::Ice::ObjectPrx ice_endpoints(const ::Ice::EndpointSeq&) const;

    ::Ice::Int ice_getLocatorCacheTimeout() const;
    ::Ice::ObjectPrx ice_locatorCacheTimeout(::Ice::Int) const;

    bool ice_isConnectionCached() const;
    ::Ice::ObjectPrx ice_connectionCached(bool) const;

    ::Ice::EndpointSelectionType ice_getEndpointSelection() const;
    ::Ice::ObjectPrx ice_endpointSelection(::Ice::EndpointSelectionType) const;

    bool ice_isSecure() const;
    ::Ice::ObjectPrx ice_secure(bool) const;

    ::Ice::EncodingVersion ice_getEncodingVersion() const;
    ::Ice::ObjectPrx ice_encodingVersion(const ::Ice::EncodingVersion&) const;

    bool ice_isPreferSecure() const;
    ::Ice::ObjectPrx ice_preferSecure(bool) const;

    ::Ice::RouterPrx ice_getRouter() const;
    ::Ice::ObjectPrx ice_router(const ::Ice::RouterPrx&) const;

    ::Ice::LocatorPrx ice_getLocator() const;
    ::Ice::ObjectPrx ice_locator(const ::Ice::LocatorPrx&) const;

    bool ice_isCollocationOptimized() const;
    ::Ice::ObjectPrx ice_collocationOptimized(bool) const;

    ::Ice::Int ice_getInvocationTimeout() const;
    ::Ice::ObjectPrx ice_invocationTimeout(::Ice::Int) const;

    ::Ice::ObjectPrx ice_twoway() const;
    bool ice_isTwoway() const;
    ::Ice::ObjectPrx ice_oneway() const;
    bool ice_isOneway() const;
    ::Ice::ObjectPrx ice_batchOneway() const;
    bool ice_isBatchOneway() const;
    ::Ice::ObjectPrx ice_datagram() const;
    bool ice_isDatagram() const;
    ::Ice::ObjectPrx ice_batchDatagram() const;
    bool ice_isBatchDatagram() const;

    ::Ice::ObjectPrx ice_compress(bool) const;
    ::Ice::ObjectPrx ice_timeout(int) const;

    ::Ice::ObjectPrx ice_connectionId(const ::std::string&) const;
    ::std::string ice_getConnectionId() const;

    ::Ice::ConnectionPtr ice_getConnection();

    ::Ice::AsyncResultPtr begin_ice_getConnection()
    {
        return begin_ice_getConnectionInternal(::IceInternal::__dummyCallback, 0);
    }

    ::Ice::AsyncResultPtr begin_ice_getConnection(const ::Ice::CallbackPtr& __del,
                                                  const ::Ice::LocalObjectPtr& __cookie = 0)
    {
        return begin_ice_getConnectionInternal(__del, __cookie);
    }

    ::Ice::AsyncResultPtr begin_ice_getConnection(const ::Ice::Callback_Object_ice_getConnectionPtr& __del,
                                                  const ::Ice::LocalObjectPtr& __cookie = 0)
    {
        return begin_ice_getConnectionInternal(__del, __cookie);
    }

    ::Ice::ConnectionPtr end_ice_getConnection(const ::Ice::AsyncResultPtr&);

    ::Ice::ConnectionPtr ice_getCachedConnection() const;

    void ice_flushBatchRequests();

    ::Ice::AsyncResultPtr begin_ice_flushBatchRequests()
    {
        return begin_ice_flushBatchRequestsInternal(::IceInternal::__dummyCallback, 0);
    }

    ::Ice::AsyncResultPtr begin_ice_flushBatchRequests(const ::Ice::CallbackPtr& __del,
                                                       const ::Ice::LocalObjectPtr& __cookie = 0)
    {
        return begin_ice_flushBatchRequestsInternal(__del, __cookie);
    }

    ::Ice::AsyncResultPtr begin_ice_flushBatchRequests(const ::Ice::Callback_Object_ice_flushBatchRequestsPtr& __del,
                                                       const ::Ice::LocalObjectPtr& __cookie = 0)
    {
        return begin_ice_flushBatchRequestsInternal(__del, __cookie);
    }

    void end_ice_flushBatchRequests(const ::Ice::AsyncResultPtr&);

    const ::IceInternal::ReferencePtr& __reference() const { return _reference; }

    ::Ice::Int __hash() const;

    void __copyFrom(const ::Ice::ObjectPrx&);

    int __handleException(const ::Ice::Exception&, const ::IceInternal::RequestHandlerPtr&, ::Ice::OperationMode,
                          bool, int&);

    void __checkTwowayOnly(const ::std::string&) const;
    void __checkAsyncTwowayOnly(const ::std::string&) const;

    void __invoke(::IceInternal::Outgoing&) const;
    void __end(const ::Ice::AsyncResultPtr&, const std::string&) const;

    ::IceInternal::RequestHandlerPtr __getRequestHandler();
    ::IceInternal::BatchRequestQueuePtr __getBatchRequestQueue();
    ::IceInternal::RequestHandlerPtr __setRequestHandler(const ::IceInternal::RequestHandlerPtr&);
    void __updateRequestHandler(const ::IceInternal::RequestHandlerPtr&, const ::IceInternal::RequestHandlerPtr&);

    void __write(::Ice::OutputStream&) const;

protected:

    virtual Object* __newInstance() const;

private:

    ::Ice::AsyncResultPtr __begin_ice_isA(const ::std::string&,
                                          const ::Ice::Context&,
                                          const ::IceInternal::CallbackBasePtr&,
                                          const ::Ice::LocalObjectPtr&);

    ::Ice::AsyncResultPtr __begin_ice_ping(const ::Ice::Context&,
                                           const ::IceInternal::CallbackBasePtr&,
                                           const ::Ice::LocalObjectPtr&);

    ::Ice::AsyncResultPtr __begin_ice_ids(const ::Ice::Context&,
                                          const ::IceInternal::CallbackBasePtr&,
                                          const ::Ice::LocalObjectPtr&);

    ::Ice::AsyncResultPtr __begin_ice_id(const ::Ice::Context&,
                                         const ::IceInternal::CallbackBasePtr&,
                                         const ::Ice::LocalObjectPtr&);

    ::Ice::AsyncResultPtr __begin_ice_invoke(const ::std::string&,
                                             ::Ice::OperationMode,
                                             const ::std::vector< ::Ice::Byte>&,
                                             const ::Ice::Context&,
                                             const ::IceInternal::CallbackBasePtr&,
                                             const ::Ice::LocalObjectPtr&);

    ::Ice::AsyncResultPtr __begin_ice_invoke(const ::std::string&,
                                             ::Ice::OperationMode,
                                             const ::std::pair<const ::Ice::Byte*, const ::Ice::Byte*>&,
                                             const ::Ice::Context&,
                                             const ::IceInternal::CallbackBasePtr&,
                                             const ::Ice::LocalObjectPtr&);

    ::Ice::AsyncResultPtr begin_ice_getConnectionInternal(const ::IceInternal::CallbackBasePtr&,
                                                          const ::Ice::LocalObjectPtr&);

    ::Ice::AsyncResultPtr begin_ice_flushBatchRequestsInternal(const ::IceInternal::CallbackBasePtr&,
                                                               const ::Ice::LocalObjectPtr&);

    void setup(const ::IceInternal::ReferencePtr&);
    friend class ::IceInternal::ProxyFactory;

    ::IceInternal::ReferencePtr _reference;
    ::IceInternal::RequestHandlerPtr _requestHandler;
    ::IceInternal::BatchRequestQueuePtr _batchRequestQueue;
    IceUtil::Mutex _mutex;
};

template<typename Prx, typename Base>
class Proxy : public virtual Base
{
public:

    IceInternal::ProxyHandle<Prx> ice_context(const ::Ice::Context& context) const
    {
        return dynamic_cast<Prx*>(::IceProxy::Ice::Object::ice_context(context).get());
    }

    IceInternal::ProxyHandle<Prx> ice_adapterId(const ::std::string& id) const
    {
        return dynamic_cast<Prx*>(::IceProxy::Ice::Object::ice_adapterId(id).get());
    }

    IceInternal::ProxyHandle<Prx> ice_endpoints(const ::Ice::EndpointSeq& endpoints) const
    {
        return dynamic_cast<Prx*>(::IceProxy::Ice::Object::ice_endpoints(endpoints).get());
    }

    IceInternal::ProxyHandle<Prx> ice_locatorCacheTimeout(int timeout) const
    {
        return dynamic_cast<Prx*>(::IceProxy::Ice::Object::ice_locatorCacheTimeout(timeout).get());
    }

    IceInternal::ProxyHandle<Prx> ice_connectionCached(bool cached) const
    {
        return dynamic_cast<Prx*>(::IceProxy::Ice::Object::ice_connectionCached(cached).get());
    }

    IceInternal::ProxyHandle<Prx> ice_endpointSelection(::Ice::EndpointSelectionType selection) const
    {
        return dynamic_cast<Prx*>(::IceProxy::Ice::Object::ice_endpointSelection(selection).get());
    }

    IceInternal::ProxyHandle<Prx> ice_secure(bool secure) const
    {
        return dynamic_cast<Prx*>(::IceProxy::Ice::Object::ice_secure(secure).get());
    }

    IceInternal::ProxyHandle<Prx> ice_preferSecure(bool preferSecure) const
    {
        return dynamic_cast<Prx*>(::IceProxy::Ice::Object::ice_preferSecure(preferSecure).get());
    }

    IceInternal::ProxyHandle<Prx> ice_router(const ::Ice::RouterPrx& router) const
    {
        return dynamic_cast<Prx*>(::IceProxy::Ice::Object::ice_router(router).get());
    }

    IceInternal::ProxyHandle<Prx> ice_locator(const ::Ice::LocatorPrx& locator) const
    {
        return dynamic_cast<Prx*>(::IceProxy::Ice::Object::ice_locator(locator).get());
    }

    IceInternal::ProxyHandle<Prx> ice_collocationOptimized(bool collocated) const
    {
        return dynamic_cast<Prx*>(::IceProxy::Ice::Object::ice_collocationOptimized(collocated).get());
    }

    IceInternal::ProxyHandle<Prx> ice_invocationTimeout(int timeout) const
    {
        return dynamic_cast<Prx*>(::IceProxy::Ice::Object::ice_invocationTimeout(timeout).get());
    }

    IceInternal::ProxyHandle<Prx> ice_twoway() const
    {
        return dynamic_cast<Prx*>(::IceProxy::Ice::Object::ice_twoway().get());
    }

    IceInternal::ProxyHandle<Prx> ice_oneway() const
    {
        return dynamic_cast<Prx*>(::IceProxy::Ice::Object::ice_oneway().get());
    }

    IceInternal::ProxyHandle<Prx> ice_batchOneway() const
    {
        return dynamic_cast<Prx*>(::IceProxy::Ice::Object::ice_batchOneway().get());
    }

    IceInternal::ProxyHandle<Prx> ice_datagram() const
    {
        return dynamic_cast<Prx*>(::IceProxy::Ice::Object::ice_datagram().get());
    }

    IceInternal::ProxyHandle<Prx> ice_batchDatagram() const
    {
        return dynamic_cast<Prx*>(::IceProxy::Ice::Object::ice_batchDatagram().get());
    }

    IceInternal::ProxyHandle<Prx> ice_compress(bool compress) const
    {
        return dynamic_cast<Prx*>(::IceProxy::Ice::Object::ice_compress(compress).get());
    }

    IceInternal::ProxyHandle<Prx> ice_timeout(int timeout) const
    {
        return dynamic_cast<Prx*>(::IceProxy::Ice::Object::ice_timeout(timeout).get());
    }

    IceInternal::ProxyHandle<Prx> ice_connectionId(const ::std::string& id) const
    {
        return dynamic_cast<Prx*>(::IceProxy::Ice::Object::ice_connectionId(id).get());
    }

    IceInternal::ProxyHandle<Prx> ice_encodingVersion(const ::Ice::EncodingVersion& version) const
    {
        return dynamic_cast<Prx*>(::IceProxy::Ice::Object::ice_encodingVersion(version).get());
    }
    
protected:
    
    virtual Object* __newInstance() const
    {
        return new Prx();
    }
};

} }

ICE_API ::std::ostream& operator<<(::std::ostream&, const ::IceProxy::Ice::Object&);

namespace Ice
{

ICE_API bool proxyIdentityLess(const ObjectPrx&, const ObjectPrx&);
ICE_API bool proxyIdentityEqual(const ObjectPrx&, const ObjectPrx&);

ICE_API bool proxyIdentityAndFacetLess(const ObjectPrx&, const ObjectPrx&);
ICE_API bool proxyIdentityAndFacetEqual(const ObjectPrx&, const ObjectPrx&);

struct ProxyIdentityLess : std::binary_function<bool, ObjectPrx&, ObjectPrx&>
{
    bool operator()(const ObjectPrx& lhs, const ObjectPrx& rhs) const
    {
        return proxyIdentityLess(lhs, rhs);
    }
};

struct ProxyIdentityEqual : std::binary_function<bool, ObjectPrx&, ObjectPrx&>
{
    bool operator()(const ObjectPrx& lhs, const ObjectPrx& rhs) const
    {
        return proxyIdentityEqual(lhs, rhs);
    }
};

struct ProxyIdentityAndFacetLess : std::binary_function<bool, ObjectPrx&, ObjectPrx&>
{
    bool operator()(const ObjectPrx& lhs, const ObjectPrx& rhs) const
    {
        return proxyIdentityAndFacetLess(lhs, rhs);
    }
};

struct ProxyIdentityAndFacetEqual : std::binary_function<bool, ObjectPrx&, ObjectPrx&>
{
    bool operator()(const ObjectPrx& lhs, const ObjectPrx& rhs) const
    {
        return proxyIdentityAndFacetEqual(lhs, rhs);
    }
};

}

namespace IceInternal
{

//
// Inline comparison functions for proxies
//
template<typename T, typename U>
inline bool operator==(const ProxyHandle<T>& lhs, const ProxyHandle<U>& rhs)
{
    ::IceProxy::Ice::Object* l = lhs.__upCast();
    ::IceProxy::Ice::Object* r = rhs.__upCast();
    if(l && r)
    {
        return *l == *r;
    }
    else
    {
        return !l && !r;
    }
}

template<typename T, typename U>
inline bool operator!=(const ProxyHandle<T>& lhs, const ProxyHandle<U>& rhs)
{
    return !operator==(lhs, rhs);
}

template<typename T, typename U>
inline bool operator<(const ProxyHandle<T>& lhs, const ProxyHandle<U>& rhs)
{
    ::IceProxy::Ice::Object* l = lhs.__upCast();
    ::IceProxy::Ice::Object* r = rhs.__upCast();
    if(l && r)
    {
        return *l < *r;
    }
    else
    {
        return !l && r;
    }
}

template<typename T, typename U>
inline bool operator<=(const ProxyHandle<T>& lhs, const ProxyHandle<U>& rhs)
{
    return lhs < rhs || lhs == rhs;
}

template<typename T, typename U>
inline bool operator>(const ProxyHandle<T>& lhs, const ProxyHandle<U>& rhs)
{
    return !(lhs < rhs || lhs == rhs);
}

template<typename T, typename U>
inline bool operator>=(const ProxyHandle<T>& lhs, const ProxyHandle<U>& rhs)
{
    return !(lhs < rhs);
}


//
// checkedCast and uncheckedCast functions without facet:
//
template<typename P> P
checkedCastImpl(const ::Ice::ObjectPrx& b, const ::Ice::Context& context)
{
    P d = 0;
    if(b.get())
    {
        typedef typename P::element_type T;

        if(b->ice_isA(T::ice_staticId(), context))
        {
            d = new T;
            d->__copyFrom(b);
        }
    }
    return d;
}

template<typename P> P
uncheckedCastImpl(const ::Ice::ObjectPrx& b)
{
    P d = 0;
    if(b)
    {
        typedef typename P::element_type T;

        d = dynamic_cast<T*>(b.get());
        if(!d)
        {
            d = new T;
            d->__copyFrom(b);
        }
    }
    return d;
}

//
// checkedCast and uncheckedCast with facet:
//

//
// Helper with type ID.
//
ICE_API ::Ice::ObjectPrx checkedCastImpl(const ::Ice::ObjectPrx&, const std::string&, const std::string&,
                                            const ::Ice::Context&);

//
// Specializations for P = ::Ice::ObjectPrx
// We have to use inline functions for broken compilers such as VC7.
//

template<> inline ::Ice::ObjectPrx
checkedCastImpl< ::Ice::ObjectPrx>(const ::Ice::ObjectPrx& b, const std::string& f, const ::Ice::Context& context)
{
    return checkedCastImpl(b, f, "::Ice::Object", context);
}

template<> inline ::Ice::ObjectPrx
uncheckedCastImpl< ::Ice::ObjectPrx>(const ::Ice::ObjectPrx& b, const std::string& f)
{
    ::Ice::ObjectPrx d = 0;
    if(b)
    {
        d = b->ice_facet(f);
    }
    return d;
}

template<typename P> P
checkedCastImpl(const ::Ice::ObjectPrx& b, const std::string& f, const ::Ice::Context& context)
{
    P d = 0;

    typedef typename P::element_type T;
    ::Ice::ObjectPrx bb = checkedCastImpl(b, f, T::ice_staticId(), context);

    if(bb)
    {
        d = new T;
        d->__copyFrom(bb);
    }
    return d;
}

template<typename P> P
uncheckedCastImpl(const ::Ice::ObjectPrx& b, const std::string& f)
{
    P d = 0;
    if(b)
    {
        typedef typename P::element_type T;

        ::Ice::ObjectPrx bb = b->ice_facet(f);
        d = new T;
        d->__copyFrom(bb);
    }
    return d;
}
}

//
// checkedCast and uncheckedCast functions provided in the Ice namespace
//
namespace Ice
{

template<typename P, typename Y> inline P
checkedCast(const ::IceInternal::ProxyHandle<Y>& b, const ::Ice::Context& ctx = ::Ice::noExplicitContext)
{
    Y* tag = 0;
    return ::IceInternal::checkedCastHelper<typename P::element_type>(b, tag, ctx);
}

template<typename P, typename Y> inline P
uncheckedCast(const ::IceInternal::ProxyHandle<Y>& b)
{
    Y* tag = 0;
    return ::IceInternal::uncheckedCastHelper<typename P::element_type>(b, tag);
}

template<typename P> inline P
checkedCast(const ::Ice::ObjectPrx& b, const std::string& f, const ::Ice::Context& ctx = ::Ice::noExplicitContext)
{
    return ::IceInternal::checkedCastImpl<P>(b, f, ctx);
}

template<typename P> inline P
uncheckedCast(const ::Ice::ObjectPrx& b, const std::string& f)
{
    return ::IceInternal::uncheckedCastImpl<P>(b, f);
}

}

namespace IceInternal
{

//
// Base template for operation callbacks.
//
template<class T>
class CallbackNC : public virtual CallbackBase
{
public:

    typedef T callback_type;

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Exception)(const ::Ice::Exception&);
    typedef void (T::*Sent)(bool);

    CallbackNC(const TPtr& instance, Exception excb, Sent sentcb) : _callback(instance), _exception(excb), _sent(sentcb)
    {
    }

    virtual CallbackBasePtr verify(const ::Ice::LocalObjectPtr& cookie)
    {
        if(cookie != 0) // Makes sure begin_ was called without a cookie
        {
            throw IceUtil::IllegalArgumentException(__FILE__, __LINE__, "cookie specified for callback without cookie");
        }
        return this;
    }

    virtual void sent(const ::Ice::AsyncResultPtr& result) const
    {
        if(_sent)
        {
            (_callback.get()->*_sent)(result->sentSynchronously());
        }
    }

    virtual bool hasSentCallback() const
    {
        return _sent != 0;
    }

protected:

    void exception(const ::Ice::AsyncResultPtr&, const ::Ice::Exception& ex) const
    {
        if(_exception)
        {
            (_callback.get()->*_exception)(ex);
        }
    }

    TPtr _callback;

private:

    Exception _exception;
    Sent _sent;
};

template<class T, typename CT>
class Callback : public virtual CallbackBase
{
public:

    typedef T callback_type;
    typedef CT cookie_type;

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Exception)(const ::Ice::Exception&, const CT&);
    typedef void (T::*Sent)(bool, const CT&);

    Callback(const TPtr& instance, Exception excb, Sent sentcb) : _callback(instance), _exception(excb), _sent(sentcb)
    {
    }

    virtual CallbackBasePtr verify(const ::Ice::LocalObjectPtr& cookie)
    {
        if(cookie && !CT::dynamicCast(cookie))
        {
            throw IceUtil::IllegalArgumentException(__FILE__, __LINE__, "unexpected cookie type");
        }
        return this;
    }

    virtual void sent(const ::Ice::AsyncResultPtr& result) const
    {
        if(_sent)
        {
            (_callback.get()->*_sent)(result->sentSynchronously(), CT::dynamicCast(result->getCookie()));
        }
    }

    virtual bool hasSentCallback() const
    {
        return _sent != 0;
    }

protected:

    void exception(const ::Ice::AsyncResultPtr& result, const ::Ice::Exception& ex) const
    {
        if(_exception)
        {
            (_callback.get()->*_exception)(ex, CT::dynamicCast(result->getCookie()));
        }
    }

    TPtr _callback;

private:

    Exception _exception;
    Sent _sent;
};

//
// Base class for twoway operation callbacks.
//
template<class T>
class TwowayCallbackNC : public CallbackNC<T>
{
public:

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Exception)(const ::Ice::Exception&);
    typedef void (T::*Sent)(bool);

    TwowayCallbackNC(const TPtr& instance, bool cb, Exception excb, Sent sentcb) : CallbackNC<T>(instance, excb, sentcb)
    {
        CallbackBase::checkCallback(instance, cb || excb != 0);
    }
};

template<class T, typename CT>
class TwowayCallback : public Callback<T, CT>
{
public:

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Exception)(const ::Ice::Exception&, const CT&);
    typedef void (T::*Sent)(bool, const CT&);

    TwowayCallback(const TPtr& instance, bool cb, Exception excb, Sent sentcb) : Callback<T, CT>(instance, excb, sentcb)
    {
        CallbackBase::checkCallback(instance, cb || excb != 0);
    }
};

//
// Base template class for oneway operations callbacks.
//
template<class T>
class OnewayCallbackNC : public CallbackNC<T>
{
public:

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Exception)(const ::Ice::Exception&);
    typedef void (T::*Sent)(bool);
    typedef void (T::*Response)();

    OnewayCallbackNC(const TPtr& instance, Response cb, Exception excb, Sent sentcb) :
        CallbackNC<T>(instance, excb, sentcb), _response(cb)
    {
        CallbackBase::checkCallback(instance, cb != 0 || excb != 0);
    }

    virtual void completed(const ::Ice::AsyncResultPtr& result) const
    {
        try
        {
            result->getProxy()->__end(result, result->getOperation());
        }
        catch(const ::Ice::Exception& ex)
        {
            CallbackNC<T>::exception(result, ex);
            return;
        }
        if(_response)
        {
            (CallbackNC<T>::_callback.get()->*_response)();
        }
    }

private:

    Response _response;
};

template<class T, typename CT>
class OnewayCallback : public Callback<T, CT>
{
public:

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Exception)(const ::Ice::Exception&, const CT&);
    typedef void (T::*Sent)(bool, const CT&);
    typedef void (T::*Response)(const CT&);

    OnewayCallback(const TPtr& instance, Response cb, Exception excb, Sent sentcb) :
        Callback<T, CT>(instance, excb, sentcb), _response(cb)
    {
        CallbackBase::checkCallback(instance, cb != 0 || excb != 0);
    }

    virtual void completed(const ::Ice::AsyncResultPtr& result) const
    {
        try
        {
            result->getProxy()->__end(result, result->getOperation());
        }
        catch(const ::Ice::Exception& ex)
        {
            Callback<T, CT>::exception(result, ex);
            return;
        }
        if(_response)
        {
            (Callback<T, CT>::_callback.get()->*_response)(CT::dynamicCast(result->getCookie()));
        }
    }

private:

    Response _response;
};

}

namespace Ice
{

template<class T>
class CallbackNC_Object_ice_isA : public Callback_Object_ice_isA_Base, public ::IceInternal::TwowayCallbackNC<T>
{
public:

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Exception)(const ::Ice::Exception&);
    typedef void (T::*Sent)(bool);
    typedef void (T::*Response)(bool);

    CallbackNC_Object_ice_isA(const TPtr& instance, Response cb, Exception excb, Sent sentcb) :
        ::IceInternal::TwowayCallbackNC<T>(instance, cb != 0, excb, sentcb), _response(cb)
    {
    }

    virtual void completed(const ::Ice::AsyncResultPtr& __result) const
    {
        bool __ret;
        try
        {
            __ret = __result->getProxy()->end_ice_isA(__result);
        }
        catch(const ::Ice::Exception& ex)
        {
            ::IceInternal::CallbackNC<T>::exception(__result, ex);
            return;
        }
        if(_response)
        {
            (::IceInternal::CallbackNC<T>::_callback.get()->*_response)(__ret);
        }
    }

private:

    Response _response;
};

template<class T, typename CT>
class Callback_Object_ice_isA : public Callback_Object_ice_isA_Base, public ::IceInternal::TwowayCallback<T, CT>
{
public:

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Exception)(const ::Ice::Exception&, const CT&);
    typedef void (T::*Sent)(bool, const CT&);
    typedef void (T::*Response)(bool, const CT&);

    Callback_Object_ice_isA(const TPtr& instance, Response cb, Exception excb, Sent sentcb) :
        ::IceInternal::TwowayCallback<T, CT>(instance, cb != 0, excb, sentcb), _response(cb)
    {
    }

    virtual void completed(const ::Ice::AsyncResultPtr& __result) const
    {
        bool __ret;
        try
        {
            __ret = __result->getProxy()->end_ice_isA(__result);
        }
        catch(const ::Ice::Exception& ex)
        {
            ::IceInternal::Callback<T, CT>::exception(__result, ex);
            return;
        }
        if(_response)
        {
            (::IceInternal::Callback<T, CT>::_callback.get()->*_response)(__ret,
                                                                          CT::dynamicCast(__result->getCookie()));
        }
    }

private:

    Response _response;
};

template<class T>
class CallbackNC_Object_ice_ping : public Callback_Object_ice_ping_Base, public ::IceInternal::OnewayCallbackNC<T>
{
public:

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Exception)(const ::Ice::Exception&);
    typedef void (T::*Sent)(bool);
    typedef void (T::*Response)();

    CallbackNC_Object_ice_ping(const TPtr& instance, Response cb, Exception excb, Sent sentcb) :
        ::IceInternal::OnewayCallbackNC<T>(instance, cb, excb, sentcb)
    {
    }
};

template<class T, typename CT>
class Callback_Object_ice_ping : public Callback_Object_ice_ping_Base, public ::IceInternal::OnewayCallback<T, CT>
{
public:

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Exception)(const ::Ice::Exception&, const CT&);
    typedef void (T::*Sent)(bool, const CT&);
    typedef void (T::*Response)(const CT&);

    Callback_Object_ice_ping(const TPtr& instance, Response cb, Exception excb, Sent sentcb) :
        ::IceInternal::OnewayCallback<T, CT>(instance, cb, excb, sentcb)
    {
    }
};

template<class T>
class CallbackNC_Object_ice_ids : public Callback_Object_ice_ids_Base, public ::IceInternal::TwowayCallbackNC<T>
{
public:

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Exception)(const ::Ice::Exception&);
    typedef void (T::*Sent)(bool);
    typedef void (T::*Response)(const ::std::vector< ::std::string>&);

    CallbackNC_Object_ice_ids(const TPtr& instance, Response cb, Exception excb, Sent sentcb) :
        ::IceInternal::TwowayCallbackNC<T>(instance, cb != 0, excb, sentcb), _response(cb)
    {
    }

    virtual void completed(const ::Ice::AsyncResultPtr& __result) const
    {
        ::std::vector< ::std::string> __ret;
        try
        {
            __ret = __result->getProxy()->end_ice_ids(__result);
        }
        catch(const ::Ice::Exception& ex)
        {
            ::IceInternal::CallbackNC<T>::exception(__result, ex);
            return;
        }
        if(_response)
        {
            (::IceInternal::CallbackNC<T>::_callback.get()->*_response)(__ret);
        }
    }

private:

    Response _response;
};

template<class T, typename CT>
class Callback_Object_ice_ids : public Callback_Object_ice_ids_Base, public ::IceInternal::TwowayCallback<T, CT>
{
public:

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Exception)(const ::Ice::Exception&, const CT&);
    typedef void (T::*Sent)(bool, const CT&);
    typedef void (T::*Response)(const ::std::vector< ::std::string>&, const CT&);

    Callback_Object_ice_ids(const TPtr& instance, Response cb, Exception excb, Sent sentcb) :
        ::IceInternal::TwowayCallback<T, CT>(instance, cb != 0, excb, sentcb), _response(cb)
    {
    }

    virtual void completed(const ::Ice::AsyncResultPtr& __result) const
    {
        ::std::vector< ::std::string> __ret;
        try
        {
            __ret = __result->getProxy()->end_ice_ids(__result);
        }
        catch(const ::Ice::Exception& ex)
        {
            ::IceInternal::Callback<T, CT>::exception(__result, ex);
            return;
        }
        if(_response)
        {
            (::IceInternal::Callback<T, CT>::_callback.get()->*_response)(__ret,
                                                                          CT::dynamicCast(__result->getCookie()));
        }
    }

private:

    Response _response;
};

template<class T>
class CallbackNC_Object_ice_id : public Callback_Object_ice_id_Base, public ::IceInternal::TwowayCallbackNC<T>
{
public:

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Exception)(const ::Ice::Exception&);
    typedef void (T::*Sent)(bool);
    typedef void (T::*Response)(const ::std::string&);

    CallbackNC_Object_ice_id(const TPtr& instance, Response cb, Exception excb, Sent sentcb) :
        ::IceInternal::TwowayCallbackNC<T>(instance, cb != 0, excb, sentcb), _response(cb)
    {
    }

    virtual void completed(const ::Ice::AsyncResultPtr& __result) const
    {
        ::std::string __ret;
        try
        {
            __ret = __result->getProxy()->end_ice_id(__result);
        }
        catch(const ::Ice::Exception& ex)
        {
            ::IceInternal::CallbackNC<T>::exception(__result, ex);
            return;
        }
        if(_response)
        {
            (::IceInternal::CallbackNC<T>::_callback.get()->*_response)(__ret);
        }
    }

private:

    Response _response;
};

template<class T, typename CT>
class Callback_Object_ice_id : public Callback_Object_ice_id_Base, public ::IceInternal::TwowayCallback<T, CT>
{
public:

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Exception)(const ::Ice::Exception&, const CT&);
    typedef void (T::*Sent)(bool, const CT&);
    typedef void (T::*Response)(const ::std::string&, const CT&);

    Callback_Object_ice_id(const TPtr& instance, Response cb, Exception excb, Sent sentcb) :
        ::IceInternal::TwowayCallback<T, CT>(instance, cb != 0, excb, sentcb), _response(cb)
    {
    }

    virtual void completed(const ::Ice::AsyncResultPtr& __result) const
    {
        ::std::string __ret;
        try
        {
            __ret = __result->getProxy()->end_ice_id(__result);
        }
        catch(const ::Ice::Exception& ex)
        {
            ::IceInternal::Callback<T, CT>::exception(__result, ex);
            return;
        }
        if(_response)
        {
            (::IceInternal::Callback<T, CT>::_callback.get()->*_response)(__ret,
                                                                          CT::dynamicCast(__result->getCookie()));
        }
    }

private:

    Response _response;
};

template<class T>
class CallbackNC_Object_ice_invoke : public Callback_Object_ice_invoke_Base, public ::IceInternal::TwowayCallbackNC<T>
{
public:

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Exception)(const ::Ice::Exception&);
    typedef void (T::*Sent)(bool);
    typedef void (T::*Response)(bool, const std::vector< ::Ice::Byte>&);
    typedef void (T::*ResponseArray)(bool, const std::pair<const ::Ice::Byte*, const ::Ice::Byte*>&);

    CallbackNC_Object_ice_invoke(const TPtr& instance, Response cb, Exception excb, Sent sentcb) :
        ::IceInternal::TwowayCallbackNC<T>(instance, cb != 0, excb, sentcb), _response(cb), _responseArray(0)
    {
    }

    CallbackNC_Object_ice_invoke(const TPtr& instance, ResponseArray cb, Exception excb, Sent sentcb) :
        ::IceInternal::TwowayCallbackNC<T>(instance, cb != 0, excb, sentcb), _response(0), _responseArray(cb)
    {
    }

    virtual void completed(const ::Ice::AsyncResultPtr& __result) const
    {
        if(_response)
        {
            bool __ok;
            std::vector< ::Ice::Byte> outParams;
            try
            {
                __ok = __result->getProxy()->end_ice_invoke(outParams, __result);
            }
            catch(const ::Ice::Exception& ex)
            {
                ::IceInternal::CallbackNC<T>::exception(__result, ex);
                return;
            }
            (::IceInternal::CallbackNC<T>::_callback.get()->*_response)(__ok, outParams);
        }
        else
        {
            bool __ok;
            std::pair<const ::Ice::Byte*, const::Ice::Byte*> outParams;
            try
            {
                __ok = __result->getProxy()->___end_ice_invoke(outParams, __result);
            }
            catch(const ::Ice::Exception& ex)
            {
                ::IceInternal::CallbackNC<T>::exception(__result, ex);
                return;
            }
            if(_responseArray)
            {
                (::IceInternal::CallbackNC<T>::_callback.get()->*_responseArray)(__ok, outParams);
            }
        }
    }

private:

    Response _response;
    ResponseArray _responseArray;
};

template<class T, typename CT>
class Callback_Object_ice_invoke : public Callback_Object_ice_invoke_Base, public ::IceInternal::TwowayCallback<T, CT>
{
public:

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Exception)(const ::Ice::Exception&, const CT&);
    typedef void (T::*Sent)(bool, const CT&);
    typedef void (T::*Response)(bool, const std::vector< ::Ice::Byte>&, const CT&);
    typedef void (T::*ResponseArray)(bool, const std::pair<const ::Ice::Byte*, const ::Ice::Byte*>&, const CT&);

    Callback_Object_ice_invoke(const TPtr& instance, Response cb, Exception excb, Sent sentcb) :
        ::IceInternal::TwowayCallback<T, CT>(instance, cb != 0, excb, sentcb), _response(cb), _responseArray(0)
    {
    }

    Callback_Object_ice_invoke(const TPtr& instance, ResponseArray cb, Exception excb, Sent sentcb) :
        ::IceInternal::TwowayCallback<T, CT>(instance, cb != 0, excb, sentcb), _response(0), _responseArray(cb)
    {
    }

    virtual void completed(const ::Ice::AsyncResultPtr& __result) const
    {
        if(_response)
        {
            bool __ok;
            std::vector< ::Ice::Byte> outParams;
            try
            {
                __ok = __result->getProxy()->end_ice_invoke(outParams, __result);
            }
            catch(const ::Ice::Exception& ex)
            {
                ::IceInternal::Callback<T, CT>::exception(__result, ex);
                return;
            }
            (::IceInternal::Callback<T, CT>::_callback.get()->*_response)(__ok,
                                                                          outParams,
                                                                          CT::dynamicCast(__result->getCookie()));
        }
        else
        {
            bool __ok;
            std::pair<const ::Ice::Byte*, const::Ice::Byte*> outParams;
            try
            {
                __ok = __result->getProxy()->___end_ice_invoke(outParams, __result);
            }
            catch(const ::Ice::Exception& ex)
            {
                ::IceInternal::Callback<T, CT>::exception(__result, ex);
                return;
            }
            if(_responseArray)
            {
                (::IceInternal::Callback<T, CT>::_callback.get()->*_responseArray)(__ok,
                                                                                   outParams,
                                                                                   CT::dynamicCast(
                                                                                       __result->getCookie()));
            }
        }
    }

private:

    Response _response;
    ResponseArray _responseArray;
};

template<class T>
class CallbackNC_Object_ice_getConnection : public Callback_Object_ice_getConnection_Base,
                                            public ::IceInternal::CallbackNC<T>
{
public:

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Response)(const ::Ice::ConnectionPtr&);
    typedef void (T::*Exception)(const ::Ice::Exception&);
    typedef void (T::*Sent)(bool);

    CallbackNC_Object_ice_getConnection(const TPtr& instance, Response cb, Exception excb, Sent sentcb) :
        ::IceInternal::CallbackNC<T>(instance, excb, sentcb), _response(cb)
    {
    }


    virtual void completed(const ::Ice::AsyncResultPtr& __result) const
    {
        ::Ice::ConnectionPtr __ret;
        try
        {
            __ret = __result->getProxy()->end_ice_getConnection(__result);
        }
        catch(const ::Ice::Exception& ex)
        {
            ::IceInternal::CallbackNC<T>::exception(__result, ex);
            return;
        }
        if(_response)
        {
            (::IceInternal::CallbackNC<T>::_callback.get()->*_response)(__ret);
        }
    }

private:

    Response _response;
};

template<class T, typename CT>
class Callback_Object_ice_getConnection : public Callback_Object_ice_getConnection_Base,
                                          public ::IceInternal::Callback<T, CT>
{
public:

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Response)(const ::Ice::ConnectionPtr&, const CT&);
    typedef void (T::*Exception)(const ::Ice::Exception&, const CT&);
    typedef void (T::*Sent)(bool, const CT&);

    Callback_Object_ice_getConnection(const TPtr& instance, Response cb, Exception excb, Sent sentcb) :
        ::IceInternal::Callback<T, CT>(instance, excb, sentcb), _response(cb)
    {
    }

    virtual void completed(const ::Ice::AsyncResultPtr& __result) const
    {
        ::Ice::ConnectionPtr __ret;
        try
        {
            __ret = __result->getProxy()->end_ice_getConnection(__result);
        }
        catch(const ::Ice::Exception& ex)
        {
            ::IceInternal::Callback<T, CT>::exception(__result, ex);
            return;
        }
        if(_response)
        {
            (::IceInternal::Callback<T, CT>::_callback.get()->*_response)(__ret,
                                                                          CT::dynamicCast(__result->getCookie()));
        }
    }

private:

    Response _response;
};

template<class T>
class CallbackNC_Object_ice_flushBatchRequests : public Callback_Object_ice_flushBatchRequests_Base,
                                                 public ::IceInternal::OnewayCallbackNC<T>
{
public:

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Exception)(const ::Ice::Exception&);
    typedef void (T::*Sent)(bool);

    CallbackNC_Object_ice_flushBatchRequests(const TPtr& instance, Exception excb, Sent sentcb) :
        ::IceInternal::OnewayCallbackNC<T>(instance, 0, excb, sentcb)
    {
    }
};

template<class T, typename CT>
class Callback_Object_ice_flushBatchRequests : public Callback_Object_ice_flushBatchRequests_Base,
                                               public ::IceInternal::OnewayCallback<T, CT>
{
public:

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Exception)(const ::Ice::Exception&, const CT&);
    typedef void (T::*Sent)(bool, const CT&);

    Callback_Object_ice_flushBatchRequests(const TPtr& instance, Exception excb, Sent sentcb) :
        ::IceInternal::OnewayCallback<T, CT>(instance, 0, excb, sentcb)
    {
    }
};

template<class T> Callback_Object_ice_isAPtr
newCallback_Object_ice_isA(const IceUtil::Handle<T>& instance,
                           void (T::*cb)(bool),
                           void (T::*excb)(const ::Ice::Exception&),
                           void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Object_ice_isA<T>(instance, cb, excb, sentcb);
}

template<class T, typename CT> Callback_Object_ice_isAPtr
newCallback_Object_ice_isA(const IceUtil::Handle<T>& instance,
                           void (T::*cb)(bool, const CT&),
                           void (T::*excb)(const ::Ice::Exception&, const CT&),
                           void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Object_ice_isA<T, CT>(instance, cb, excb, sentcb);
}

template<class T> Callback_Object_ice_isAPtr
newCallback_Object_ice_isA(const IceUtil::Handle<T>& instance,
                           void (T::*excb)(const ::Ice::Exception&),
                           void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Object_ice_isA<T>(instance, 0, excb, sentcb);
}

template<class T, typename CT> Callback_Object_ice_isAPtr
newCallback_Object_ice_isA(const IceUtil::Handle<T>& instance,
                           void (T::*excb)(const ::Ice::Exception&, const CT&),
                           void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Object_ice_isA<T, CT>(instance, 0, excb, sentcb);
}

template<class T> Callback_Object_ice_isAPtr
newCallback_Object_ice_isA(T* instance,
                           void (T::*cb)(bool),
                           void (T::*excb)(const ::Ice::Exception&),
                           void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Object_ice_isA<T>(instance, cb, excb, sentcb);
}

template<class T, typename CT> Callback_Object_ice_isAPtr
newCallback_Object_ice_isA(T* instance,
                           void (T::*cb)(bool, const CT&),
                           void (T::*excb)(const ::Ice::Exception&, const CT&),
                           void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Object_ice_isA<T, CT>(instance, cb, excb, sentcb);
}

template<class T> Callback_Object_ice_isAPtr
newCallback_Object_ice_isA(T* instance,
                           void (T::*excb)(const ::Ice::Exception&),
                           void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Object_ice_isA<T>(instance, 0, excb, sentcb);
}

template<class T, typename CT> Callback_Object_ice_isAPtr
newCallback_Object_ice_isA(T* instance,
                           void (T::*excb)(const ::Ice::Exception&, const CT&),
                           void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Object_ice_isA<T, CT>(instance, 0, excb, sentcb);
}

template<class T> Callback_Object_ice_pingPtr
newCallback_Object_ice_ping(const IceUtil::Handle<T>& instance,
                            void (T::*cb)(),
                            void (T::*excb)(const ::Ice::Exception&),
                            void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Object_ice_ping<T>(instance, cb, excb, sentcb);
}

template<class T, typename CT> Callback_Object_ice_pingPtr
newCallback_Object_ice_ping(const IceUtil::Handle<T>& instance,
                            void (T::*cb)(const CT&),
                            void (T::*excb)(const ::Ice::Exception&, const CT&),
                            void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Object_ice_ping<T, CT>(instance, cb, excb, sentcb);
}

template<class T> Callback_Object_ice_pingPtr
newCallback_Object_ice_ping(const IceUtil::Handle<T>& instance,
                            void (T::*excb)(const ::Ice::Exception&),
                            void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Object_ice_ping<T>(instance, 0, excb, sentcb);
}

template<class T, typename CT> Callback_Object_ice_pingPtr
newCallback_Object_ice_ping(const IceUtil::Handle<T>& instance,
                            void (T::*excb)(const ::Ice::Exception&, const CT&),
                            void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Object_ice_ping<T, CT>(instance, 0, excb, sentcb);
}

template<class T> Callback_Object_ice_pingPtr
newCallback_Object_ice_ping(T* instance,
                            void (T::*cb)(),
                            void (T::*excb)(const ::Ice::Exception&),
                            void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Object_ice_ping<T>(instance, cb, excb, sentcb);
}

template<class T, typename CT> Callback_Object_ice_pingPtr
newCallback_Object_ice_ping(T* instance,
                            void (T::*cb)(const CT&),
                            void (T::*excb)(const ::Ice::Exception&, const CT&),
                            void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Object_ice_ping<T, CT>(instance, cb, excb, sentcb);
}

template<class T> Callback_Object_ice_pingPtr
newCallback_Object_ice_ping(T* instance,
                            void (T::*excb)(const ::Ice::Exception&),
                            void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Object_ice_ping<T>(instance, 0, excb, sentcb);
}

template<class T, typename CT> Callback_Object_ice_pingPtr
newCallback_Object_ice_ping(T* instance,
                            void (T::*excb)(const ::Ice::Exception&, const CT&),
                            void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Object_ice_ping<T, CT>(instance, 0, excb, sentcb);
}

template<class T> Callback_Object_ice_idsPtr
newCallback_Object_ice_ids(const IceUtil::Handle<T>& instance,
                           void (T::*cb)(const ::std::vector< ::std::string>&),
                           void (T::*excb)(const ::Ice::Exception&),
                           void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Object_ice_ids<T>(instance, cb, excb, sentcb);
}

template<class T, typename CT> Callback_Object_ice_idsPtr
newCallback_Object_ice_ids(const IceUtil::Handle<T>& instance,
                           void (T::*cb)(const ::std::vector< ::std::string>&, const CT&),
                           void (T::*excb)(const ::Ice::Exception&, const CT&),
                           void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Object_ice_ids<T, CT>(instance, cb, excb, sentcb);
}

template<class T> Callback_Object_ice_idsPtr
newCallback_Object_ice_ids(const IceUtil::Handle<T>& instance,
                           void (T::*excb)(const ::Ice::Exception&),
                           void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Object_ice_ids<T>(instance, 0, excb, sentcb);
}

template<class T, typename CT> Callback_Object_ice_idsPtr
newCallback_Object_ice_ids(const IceUtil::Handle<T>& instance,
                           void (T::*excb)(const ::Ice::Exception&, const CT&),
                           void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Object_ice_ids<T, CT>(instance, 0, excb, sentcb);
}

template<class T> Callback_Object_ice_idsPtr
newCallback_Object_ice_ids(T* instance,
                           void (T::*cb)(const ::std::vector< ::std::string>&),
                           void (T::*excb)(const ::Ice::Exception&),
                           void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Object_ice_ids<T>(instance, cb, excb, sentcb);
}

template<class T, typename CT> Callback_Object_ice_idsPtr
newCallback_Object_ice_ids(T* instance,
                           void (T::*cb)(const ::std::vector< ::std::string>&, const CT&),
                           void (T::*excb)(const ::Ice::Exception&, const CT&),
                           void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Object_ice_ids<T, CT>(instance, cb, excb, sentcb);
}

template<class T> Callback_Object_ice_idsPtr
newCallback_Object_ice_ids(T* instance,
                           void (T::*excb)(const ::Ice::Exception&),
                           void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Object_ice_ids<T>(instance, 0, excb, sentcb);
}

template<class T, typename CT> Callback_Object_ice_idsPtr
newCallback_Object_ice_ids(T* instance,
                           void (T::*excb)(const ::Ice::Exception&, const CT&),
                           void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Object_ice_ids<T, CT>(instance, 0, excb, sentcb);
}

template<class T> Callback_Object_ice_idPtr
newCallback_Object_ice_id(const IceUtil::Handle<T>& instance,
                          void (T::*cb)(const ::std::string&),
                          void (T::*excb)(const ::Ice::Exception&),
                          void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Object_ice_id<T>(instance, cb, excb, sentcb);
}

template<class T, typename CT> Callback_Object_ice_idPtr
newCallback_Object_ice_id(const IceUtil::Handle<T>& instance,
                          void (T::*cb)(const ::std::string&, const CT&),
                          void (T::*excb)(const ::Ice::Exception&, const CT&),
                          void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Object_ice_id<T, CT>(instance, cb, excb, sentcb);
}

template<class T> Callback_Object_ice_idPtr
newCallback_Object_ice_id(const IceUtil::Handle<T>& instance,
                          void (T::*excb)(const ::Ice::Exception&),
                          void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Object_ice_id<T>(instance, 0, excb, sentcb);
}

template<class T, typename CT> Callback_Object_ice_idPtr
newCallback_Object_ice_id(const IceUtil::Handle<T>& instance,
                          void (T::*excb)(const ::Ice::Exception&, const CT&),
                          void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Object_ice_id<T, CT>(instance, 0, excb, sentcb);
}

template<class T> Callback_Object_ice_idPtr
newCallback_Object_ice_id(T* instance,
                          void (T::*cb)(const ::std::string&),
                          void (T::*excb)(const ::Ice::Exception&),
                          void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Object_ice_id<T>(instance, cb, excb, sentcb);
}

template<class T, typename CT> Callback_Object_ice_idPtr
newCallback_Object_ice_id(T* instance,
                          void (T::*cb)(const ::std::string&, const CT&),
                          void (T::*excb)(const ::Ice::Exception&, const CT&),
                          void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Object_ice_id<T, CT>(instance, cb, excb, sentcb);
}

template<class T> Callback_Object_ice_idPtr
newCallback_Object_ice_id(T* instance,
                          void (T::*excb)(const ::Ice::Exception&),
                          void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Object_ice_id<T>(instance, 0, excb, sentcb);
}

template<class T, typename CT> Callback_Object_ice_idPtr
newCallback_Object_ice_id(T* instance,
                          void (T::*excb)(const ::Ice::Exception&, const CT&),
                          void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Object_ice_id<T, CT>(instance, 0, excb, sentcb);
}

template<class T> Callback_Object_ice_invokePtr
newCallback_Object_ice_invoke(const IceUtil::Handle<T>& instance,
                              void (T::*cb)(bool, const std::vector<Ice::Byte>&),
                              void (T::*excb)(const ::Ice::Exception&),
                              void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Object_ice_invoke<T>(instance, cb, excb, sentcb);
}

template<class T> Callback_Object_ice_invokePtr
newCallback_Object_ice_invoke(const IceUtil::Handle<T>& instance,
                              void (T::*cb)(bool, const std::pair<const Byte*, const Byte*>&),
                              void (T::*excb)(const ::Ice::Exception&),
                              void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Object_ice_invoke<T>(instance, cb, excb, sentcb);
}

template<class T, typename CT> Callback_Object_ice_invokePtr
newCallback_Object_ice_invoke(const IceUtil::Handle<T>& instance,
                              void (T::*cb)(bool, const std::vector<Ice::Byte>&, const CT&),
                              void (T::*excb)(const ::Ice::Exception&, const CT&),
                              void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Object_ice_invoke<T, CT>(instance, cb, excb, sentcb);
}

template<class T, typename CT> Callback_Object_ice_invokePtr
newCallback_Object_ice_invoke(const IceUtil::Handle<T>& instance,
                              void (T::*cb)(bool, const std::pair<const Byte*, const Byte*>&,
                                            const CT&),
                              void (T::*excb)(const ::Ice::Exception&, const CT&),
                              void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Object_ice_invoke<T, CT>(instance, cb, excb, sentcb);
}

template<class T> Callback_Object_ice_invokePtr
newCallback_Object_ice_invoke(const IceUtil::Handle<T>& instance,
                              void (T::*excb)(const ::Ice::Exception&),
                              void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Object_ice_invoke<T>(instance, 0, excb, sentcb);
}

template<class T, typename CT> Callback_Object_ice_invokePtr
newCallback_Object_ice_invoke(const IceUtil::Handle<T>& instance,
                              void (T::*excb)(const ::Ice::Exception&, const CT&),
                              void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Object_ice_invoke<T, CT>(instance, 0, excb, sentcb);
}

template<class T> Callback_Object_ice_invokePtr
newCallback_Object_ice_invoke(T* instance,
                              void (T::*cb)(bool, const std::vector<Ice::Byte>&),
                              void (T::*excb)(const ::Ice::Exception&),
                              void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Object_ice_invoke<T>(instance, cb, excb, sentcb);
}

template<class T> Callback_Object_ice_invokePtr
newCallback_Object_ice_invoke(T* instance,
                              void (T::*cb)(bool, const std::pair<const Byte*, const Byte*>&),
                              void (T::*excb)(const ::Ice::Exception&),
                              void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Object_ice_invoke<T>(instance, cb, excb, sentcb);
}

template<class T, typename CT> Callback_Object_ice_invokePtr
newCallback_Object_ice_invoke(T* instance,
                              void (T::*cb)(bool, const std::vector<Ice::Byte>&, const CT&),
                              void (T::*excb)(const ::Ice::Exception&, const CT&),
                              void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Object_ice_invoke<T, CT>(instance, cb, excb, sentcb);
}

template<class T, typename CT> Callback_Object_ice_invokePtr
newCallback_Object_ice_invoke(T* instance,
                              void (T::*cb)(bool, const std::pair<const Byte*, const Byte*>&, const CT&),
                              void (T::*excb)(const ::Ice::Exception&, const CT&),
                              void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Object_ice_invoke<T, CT>(instance, cb, excb, sentcb);
}

template<class T> Callback_Object_ice_invokePtr
newCallback_Object_ice_invoke(T* instance,
                              void (T::*excb)(const ::Ice::Exception&),
                              void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Object_ice_invoke<T>(
        instance, static_cast<void (T::*)(bool, const std::vector<Ice::Byte>&)>(0), excb, sentcb);
}

template<class T, typename CT> Callback_Object_ice_invokePtr
newCallback_Object_ice_invoke(T* instance,
                              void (T::*excb)(const ::Ice::Exception&, const CT&),
                              void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Object_ice_invoke<T, CT>(
        instance, static_cast<void (T::*)(bool, const std::vector<Ice::Byte>&, const CT&)>(0), excb, sentcb);
}

template<class T> Callback_Object_ice_getConnectionPtr
newCallback_Object_ice_getConnection(const IceUtil::Handle<T>& instance,
                                     void (T::*cb)(const ::Ice::ConnectionPtr&),
                                     void (T::*excb)(const ::Ice::Exception&))
{
    return new CallbackNC_Object_ice_getConnection<T>(instance, cb, excb, 0);
}

template<class T, typename CT> Callback_Object_ice_getConnectionPtr
newCallback_Object_ice_getConnection(const IceUtil::Handle<T>& instance,
                                     void (T::*cb)(const ::Ice::ConnectionPtr&, const CT&),
                                     void (T::*excb)(const ::Ice::Exception&, const CT&))
{
    return new Callback_Object_ice_getConnection<T, CT>(instance, cb, excb, 0);
}

template<class T> Callback_Object_ice_getConnectionPtr
newCallback_Object_ice_getConnection(T* instance,
                                     void (T::*cb)(const ::Ice::ConnectionPtr&),
                                     void (T::*excb)(const ::Ice::Exception&))
{
    return new CallbackNC_Object_ice_getConnection<T>(instance, cb, excb, 0);
}

template<class T, typename CT> Callback_Object_ice_getConnectionPtr
newCallback_Object_ice_getConnection(T* instance,
                                     void (T::*cb)(const ::Ice::ConnectionPtr&, const CT&),
                                     void (T::*excb)(const ::Ice::Exception&, const CT&))
{
    return new Callback_Object_ice_getConnection<T, CT>(instance, cb, excb, 0);
}

template<class T> Callback_Object_ice_flushBatchRequestsPtr
newCallback_Object_ice_flushBatchRequests(const IceUtil::Handle<T>& instance,
                                          void (T::*excb)(const ::Ice::Exception&),
                                          void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Object_ice_flushBatchRequests<T>(instance, excb, sentcb);
}

template<class T, typename CT> Callback_Object_ice_flushBatchRequestsPtr
newCallback_Object_ice_flushBatchRequests(const IceUtil::Handle<T>& instance,
                                          void (T::*excb)(const ::Ice::Exception&, const CT&),
                                          void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Object_ice_flushBatchRequests<T, CT>(instance, excb, sentcb);
}

template<class T> Callback_Object_ice_flushBatchRequestsPtr
newCallback_Object_ice_flushBatchRequests(T* instance,
                                          void (T::*excb)(const ::Ice::Exception&),
                                          void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Object_ice_flushBatchRequests<T>(instance, excb, sentcb);
}

template<class T, typename CT> Callback_Object_ice_flushBatchRequestsPtr
newCallback_Object_ice_flushBatchRequests(T* instance,
                                          void (T::*excb)(const ::Ice::Exception&, const CT&),
                                          void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Object_ice_flushBatchRequests<T, CT>(instance, excb, sentcb);
}

}
#endif

#endif
