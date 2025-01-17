// **********************************************************************
//
// Copyright (c) 2003-2015 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <EndpointI.h>
#include <Transceiver.h>
#include <Connector.h>
#include <Acceptor.h>

#ifdef _MSC_VER
// For 'Ice::Object::ice_getHash': was declared deprecated
#pragma warning( disable : 4996 )
#endif

#if defined(__GNUC__)
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

using namespace std;

Ice::Short EndpointI::TYPE_BASE = 100;

EndpointI::EndpointI(const IceInternal::EndpointIPtr& endpoint) :
    _endpoint(endpoint),
    _configuration(Configuration::getInstance())
{
}

void
EndpointI::streamWrite(Ice::OutputStream* s) const
{
    s->startEncapsulation();
    s->write(_endpoint->type());
    _endpoint->streamWrite(s);
    s->endEncapsulation();
}

Ice::Short
EndpointI::type() const
{
    return (Ice::Short)(TYPE_BASE + _endpoint->type());
}

const std::string&
EndpointI::protocol() const
{
    return _endpoint->protocol();
}

int
EndpointI::timeout() const
{
    return _endpoint->timeout();
}

const std::string&
EndpointI::connectionId() const
{
    return _endpoint->connectionId();
}

IceInternal::EndpointIPtr
EndpointI::timeout(int timeout) const
{
    IceInternal::EndpointIPtr endpoint = _endpoint->timeout(timeout);
    if(endpoint == _endpoint)
    {
        return shared_from_this();
    }
    else
    {
        return ICE_MAKE_SHARED(EndpointI, endpoint);
    }
}

IceInternal::EndpointIPtr
EndpointI::connectionId(const string& connectionId) const
{
    IceInternal::EndpointIPtr endpoint = _endpoint->connectionId(connectionId);
    if(endpoint == _endpoint)
    {
        return shared_from_this();
    }
    else
    {
        return ICE_MAKE_SHARED(EndpointI, endpoint);
    }
}

bool
EndpointI::compress() const
{
    return _endpoint->compress();
}

IceInternal::EndpointIPtr
EndpointI::compress(bool compress) const
{
    IceInternal::EndpointIPtr endpoint = _endpoint->compress(compress);
    if(endpoint == _endpoint)
    {
        return shared_from_this();
    }
    else
    {
        return ICE_MAKE_SHARED(EndpointI, endpoint);
    }
}

bool
EndpointI::datagram() const
{
    return _endpoint->datagram();
}

bool
EndpointI::secure() const
{
    return _endpoint->secure();
}

IceInternal::TransceiverPtr
EndpointI::transceiver() const
{
    IceInternal::TransceiverPtr transceiver = _endpoint->transceiver();
    if(transceiver)
    {
        return new Transceiver(transceiver);
    }
    else
    {
        return 0;
    }
}

void
EndpointI::connectors_async(Ice::EndpointSelectionType selType, const IceInternal::EndpointI_connectorsPtr& cb) const
{
    class Callback : public IceInternal::EndpointI_connectors
    {
    public:

        Callback(const IceInternal::EndpointI_connectorsPtr& callback) : _callback(callback)
        {
        }

        void
        connectors(const vector<IceInternal::ConnectorPtr>& connectors)
        {
            vector<IceInternal::ConnectorPtr> c;
            for(vector<IceInternal::ConnectorPtr>::const_iterator p = connectors.begin(); p != connectors.end(); ++p)
            {
                c.push_back(new Connector(*p));
            }
            _callback->connectors(c);
        }

        void
        exception(const Ice::LocalException& ex)
        {
            _callback->exception(ex);
        }

    private:

        IceInternal::EndpointI_connectorsPtr _callback;
    };

    try
    {
        _configuration->checkConnectorsException();
        _endpoint->connectors_async(selType, ICE_MAKE_SHARED(Callback, cb));
    }
    catch(const Ice::LocalException& ex)
    {
        cb->exception(ex);
    }
}

IceInternal::AcceptorPtr
EndpointI::acceptor(const string& adapterName) const
{
    return new Acceptor(shared_from_this(), _endpoint->acceptor(adapterName));
}

/*IceInternal::EndpointIPtr
EndpointI::endpoint(const IceInternal::TransceiverPtr& transceiver) const
{
    Transceiver* p = dynamic_cast<Transceiver*>(transceiver.get());
    IceInternal::EndpointIPtr endpt = _endpoint->endpoint(p->delegate());
    if(endpt == _endpoint)
    {
        return const_cast<EndpointI*>(this);
    }
    else
    {
        return new EndpointI(endpt);
    }
}*/

EndpointIPtr
EndpointI::endpoint(const IceInternal::EndpointIPtr& delEndp) const
{
    return ICE_MAKE_SHARED(EndpointI, delEndp);
}

vector<IceInternal::EndpointIPtr>
EndpointI::expand() const
{
    vector<IceInternal::EndpointIPtr> e = _endpoint->expand();
    for(vector<IceInternal::EndpointIPtr>::iterator p = e.begin(); p != e.end(); ++p)
    {
        *p = (*p == _endpoint) ? shared_from_this() : ICE_MAKE_SHARED(EndpointI, *p);
    }
    return e;
}

bool
EndpointI::equivalent(const IceInternal::EndpointIPtr& endpoint) const
{
    const EndpointI* testEndpointI = dynamic_cast<const EndpointI*>(endpoint.get());
    if(!testEndpointI)
    {
        return false;
    }
    return testEndpointI->_endpoint->equivalent(_endpoint);
}

string
EndpointI::toString() const
{
    return "test-" + _endpoint->toString();
}

Ice::EndpointInfoPtr
EndpointI::getInfo() const
{
    return _endpoint->getInfo();
}

bool
#ifdef ICE_CPP11_MAPPING
EndpointI::operator==(const Ice::Endpoint& r) const
#else
EndpointI::operator==(const Ice::LocalObject& r) const
#endif
{
    const EndpointI* p = dynamic_cast<const EndpointI*>(&r);
    if(!p)
    {
        return false;
    }

    if(this == p)
    {
        return true;
    }


    return *p->_endpoint == *_endpoint;
}

bool
#ifdef ICE_CPP11_MAPPING
EndpointI::operator<(const Ice::Endpoint& r) const
#else
EndpointI::operator<(const Ice::LocalObject& r) const
#endif
{
    const EndpointI* p = dynamic_cast<const EndpointI*>(&r);
    if(!p)
    {
        const IceInternal::EndpointI* e = dynamic_cast<const IceInternal::EndpointI*>(&r);
        if(!e)
        {
            return false;
        }
        return type() < e->type();
    }

    if(this == p)
    {
        return false;
    }

    return *p->_endpoint < *_endpoint;
}

int
EndpointI::hash() const
{
    return  _endpoint->hash();
}

string
EndpointI::options() const
{
    return _endpoint->options();
}

IceInternal::EndpointIPtr
EndpointI::delegate() const
{
    return _endpoint;
}
