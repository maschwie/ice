// **********************************************************************
//
// Copyright (c) 2003-2015 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_ROUTER_INFO_H
#define ICE_ROUTER_INFO_H

#include <IceUtil/Shared.h>
#include <IceUtil/Mutex.h>
#include <Ice/RouterInfoF.h>
#include <Ice/Router.h>
#include <Ice/ProxyF.h>
#include <Ice/ReferenceF.h>
#include <Ice/EndpointIF.h>
#include <Ice/BuiltinSequences.h>
#include <Ice/Identity.h>
#include <Ice/Comparable.h>

#include <set>

namespace IceInternal
{

class RouterManager : public IceUtil::Shared, public IceUtil::Mutex
{
public:

    RouterManager();

    void destroy();

    //
    // Returns router info for a given router. Automatically creates
    // the router info if it doesn't exist yet.
    //
    RouterInfoPtr get(const Ice::RouterPrxPtr&);
    RouterInfoPtr erase(const Ice::RouterPrxPtr&);

private:

#ifdef ICE_CPP11_MAPPING
    using RouterTableMap = std::map<std::shared_ptr<Ice::RouterPrx>, 
    RouterInfoPtr,
                                    Ice::TargetLess<std::shared_ptr<::Ice::RouterPrx>>>;
#else
    typedef std::map<Ice::RouterPrxPtr, RouterInfoPtr> RouterTableMap;
#endif
    
    RouterTableMap _table;
    RouterTableMap::iterator _tableHint;
};

class RouterInfo : public IceUtil::Shared, public IceUtil::Mutex
{
public:

    class GetClientEndpointsCallback : public virtual Ice::LocalObject
    {
    public:
        
        virtual void setEndpoints(const std::vector<EndpointIPtr>&) = 0;
        virtual void setException(const Ice::LocalException&) = 0;
    };
    typedef IceUtil::Handle<GetClientEndpointsCallback> GetClientEndpointsCallbackPtr;

    class AddProxyCallback 
#ifndef ICE_CPP11_MAPPING
        : public virtual IceUtil::Shared
#endif
    {
    public:
        
        virtual void addedProxy() = 0;
        virtual void setException(const Ice::LocalException&) = 0;
    };
    ICE_DEFINE_PTR(AddProxyCallbackPtr, AddProxyCallback); 

    RouterInfo(const Ice::RouterPrxPtr&);

    void destroy();

    bool operator==(const RouterInfo&) const;
    bool operator!=(const RouterInfo&) const;
    bool operator<(const RouterInfo&) const;

    const Ice::RouterPrxPtr& getRouter() const
    {
        //
        // No mutex lock necessary, _router is immutable.
        //
        return _router;
    }
    void getClientProxyResponse(const Ice::ObjectPrxPtr&, const GetClientEndpointsCallbackPtr&);
    void getClientProxyException(const Ice::Exception&, const GetClientEndpointsCallbackPtr&);
    std::vector<EndpointIPtr> getClientEndpoints();
    void getClientEndpoints(const GetClientEndpointsCallbackPtr&);
    std::vector<EndpointIPtr> getServerEndpoints();
    
    class AddProxyCookie : public Ice::LocalObject
    {
    public:
        
        AddProxyCookie(const AddProxyCallbackPtr cb, const Ice::ObjectPrxPtr& proxy) :
            _cb(cb),
            _proxy(proxy)
        {
        }
        
        AddProxyCallbackPtr cb() const
        {
            return _cb;
        }
        
        Ice::ObjectPrxPtr proxy() const
        {
            return _proxy;
        }
        
    private:
        
        const AddProxyCallbackPtr _cb;
        const Ice::ObjectPrxPtr _proxy;
    };
    typedef IceUtil::Handle<AddProxyCookie> AddProxyCookiePtr;
    
    void addProxyResponse(const Ice::ObjectProxySeq&, const AddProxyCookiePtr&);
    void addProxyException(const Ice::Exception&, const AddProxyCookiePtr&);
    void addProxy(const Ice::ObjectPrxPtr&);
    bool addProxy(const Ice::ObjectPrxPtr&, const AddProxyCallbackPtr&);

    void setAdapter(const Ice::ObjectAdapterPtr&);
    Ice::ObjectAdapterPtr getAdapter() const;

    void clearCache(const ReferencePtr&);

    //
    // The following methods need to be public for access by AMI callbacks.
    //
    std::vector<EndpointIPtr> setClientEndpoints(const Ice::ObjectPrxPtr&);
    std::vector<EndpointIPtr> setServerEndpoints(const Ice::ObjectPrxPtr&);
    void addAndEvictProxies(const Ice::ObjectPrxPtr&, const Ice::ObjectProxySeq&);

private:

    const Ice::RouterPrxPtr _router;
    std::vector<EndpointIPtr> _clientEndpoints;
    std::vector<EndpointIPtr> _serverEndpoints;
    Ice::ObjectAdapterPtr _adapter;
    std::set<Ice::Identity> _identities;
    std::multiset<Ice::Identity> _evictedIdentities;
};

}

#endif
