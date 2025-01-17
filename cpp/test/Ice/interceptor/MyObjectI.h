// **********************************************************************
//
// Copyright (c) 2003-2015 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef MY_OBJECT_I_H
#define MY_OBJECT_I_H

#include <Test.h>

class MySystemException : public Ice::SystemException
{
public:
  
    MySystemException(const char*, int);

    virtual ~MySystemException() ICE_NOEXCEPT;

    virtual std::string ice_id() const;
#ifndef ICE_CPP11_MAPPING
    virtual MySystemException* ice_clone() const;
#endif
    virtual void ice_throw() const;
};

class MyObjectI : public Test::MyObject
{
public:

    virtual int add(int, int, const Ice::Current&);
    virtual int addWithRetry(int, int, const Ice::Current&);
    virtual int badAdd(int, int, const Ice::Current&);
    virtual int notExistAdd(int, int, const Ice::Current&);
    virtual int badSystemAdd(int, int, const Ice::Current&);
    
#ifdef ICE_CPP11_MAPPING
    virtual void amdAdd_async(int, int, std::function<void (int)>, std::function<void (std::exception_ptr)>, const Ice::Current&);
    virtual void amdAddWithRetry_async(int, int, std::function<void (int)>, std::function<void (std::exception_ptr)>, const Ice::Current&);
    virtual void amdBadAdd_async(int, int, std::function<void (int)>, std::function<void (std::exception_ptr)>, const Ice::Current&);
    virtual void amdNotExistAdd_async(int, int, std::function<void (int)>, std::function<void (std::exception_ptr)>, const Ice::Current&);
    virtual void amdBadSystemAdd_async(int, int, std::function<void (int)>, std::function<void (std::exception_ptr)>, const Ice::Current&);
#else
    virtual void amdAdd_async(const Test::AMD_MyObject_amdAddPtr&, int, int, const Ice::Current&);
    virtual void amdAddWithRetry_async(const Test::AMD_MyObject_amdAddWithRetryPtr&, int, int, const Ice::Current&);
    virtual void amdBadAdd_async(const Test::AMD_MyObject_amdBadAddPtr&, int, int, const Ice::Current&);
    virtual void amdNotExistAdd_async(const Test::AMD_MyObject_amdNotExistAddPtr&, int, int, const Ice::Current&);
    virtual void amdBadSystemAdd_async(const Test::AMD_MyObject_amdBadSystemAddPtr&, int, int, const Ice::Current&);
#endif
};

#endif
