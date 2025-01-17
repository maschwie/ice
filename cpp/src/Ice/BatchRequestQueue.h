// **********************************************************************
//
// Copyright (c) 2003-2015 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_BATCH_REQUEST_QUEUE_H
#define ICE_BATCH_REQUEST_QUEUE_H

#include <IceUtil/Shared.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/Monitor.h>
#include <IceUtil/UniquePtr.h>

#include <Ice/BatchRequestInterceptor.h>
#include <Ice/BatchRequestQueueF.h>
#include <Ice/InstanceF.h>
#include <Ice/OutputStream.h>

namespace IceInternal
{

class BatchRequestQueue : public IceUtil::Shared, private IceUtil::Monitor<IceUtil::Mutex>
{
public:

    BatchRequestQueue(const InstancePtr&, bool);

    void prepareBatchRequest(Ice::OutputStream*);
    void finishBatchRequest(Ice::OutputStream*, const Ice::ObjectPrxPtr&, const std::string&);
    void abortBatchRequest(Ice::OutputStream*);

    int swap(Ice::OutputStream*);

    void destroy(const Ice::LocalException&);
    bool isEmpty();

    void enqueueBatchRequest();

private:

    void waitStreamInUse(bool);

#ifdef ICE_CPP11_MAPPING
    std::function<void(const Ice::BatchRequest&, int, int)> _interceptor;
#else
    Ice::BatchRequestInterceptorPtr _interceptor;
#endif
    Ice::OutputStream _batchStream;
    bool _batchStreamInUse;
    bool _batchStreamCanFlush;
    int _batchRequestNum;
    size_t _batchMarker;
#ifdef ICE_CPP11_MAPPING
    std::exception_ptr _exception;
#else
    IceUtil::UniquePtr<Ice::LocalException> _exception;
#endif
    size_t _maxSize;
};

};

#endif
