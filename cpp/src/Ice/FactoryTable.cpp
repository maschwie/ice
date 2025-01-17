// **********************************************************************
//
// Copyright (c) 2003-2015 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/FactoryTable.h>
#include <Ice/ValueFactory.h>

using namespace std;

//
// Add a factory to the exception factory table.
// If the factory is present already, increment its reference count.
//
void
IceInternal::FactoryTable::addExceptionFactory(const string& t, const Ice::UserExceptionFactoryPtr& f)
{
    IceUtil::Mutex::Lock lock(_m);
    assert(f);
    EFTable::iterator i = _eft.find(t);
    if(i == _eft.end())
    {
        _eft[t] = EFPair(f, 1);
    }
    else
    {
        i->second.second++;
    }
}

//
// Return the exception factory for a given type ID
//
Ice::UserExceptionFactoryPtr
IceInternal::FactoryTable::getExceptionFactory(const string& t) const
{
    IceUtil::Mutex::Lock lock(_m);
    EFTable::const_iterator i = _eft.find(t);
    return i != _eft.end() ? i->second.first : Ice::UserExceptionFactoryPtr();
}

//
// Remove a factory from the exception factory table. If the factory
// is not present, do nothing; otherwise, decrement the factory's
// reference count; if the count drops to zero, remove the factory's
// entry from the table.
//
void
IceInternal::FactoryTable::removeExceptionFactory(const string& t)
{
    IceUtil::Mutex::Lock lock(_m);
    EFTable::iterator i = _eft.find(t);
    if(i != _eft.end())
    {
        if(--i->second.second == 0)
        {
            _eft.erase(i);
        }
    }
}

//
// Add a factory to the value factory table.
//
#ifdef ICE_CPP11_MAPPING
void
IceInternal::FactoryTable::addValueFactory(const string& t, function<::Ice::ValuePtr (string)> f)
#else
void
IceInternal::FactoryTable::addValueFactory(const string& t, const ::Ice::ValueFactoryPtr& f)
#endif
{
    IceUtil::Mutex::Lock lock(_m);
    assert(f);
    VFTable::iterator i = _vft.find(t);
    if(i == _vft.end())
    {
        _vft[t] = VFPair(f, 1);
    }
    else
    {
        i->second.second++;
    }
}

//
// Return the value factory for a given type ID
//
#ifdef ICE_CPP11_MAPPING
function<Ice::ValuePtr(const string&)>
IceInternal::FactoryTable::getValueFactory(const string& t) const
{
    IceUtil::Mutex::Lock lock(_m);
    VFTable::const_iterator i = _vft.find(t);
    return i != _vft.end() ? i->second.first : nullptr;
}
#else
Ice::ValueFactoryPtr
IceInternal::FactoryTable::getValueFactory(const string& t) const
{
    IceUtil::Mutex::Lock lock(_m);
    VFTable::const_iterator i = _vft.find(t);
    return i != _vft.end() ? i->second.first : Ice::ValueFactoryPtr();
}
#endif

//
// Remove a factory from the value factory table. If the factory
// is not present, do nothing; otherwise, decrement the factory's
// reference count if the count drops to zero, remove the factory's
// entry from the table.
//
void
IceInternal::FactoryTable::removeValueFactory(const string& t)
{
    IceUtil::Mutex::Lock lock(_m);
    VFTable::iterator i = _vft.find(t);
    if(i != _vft.end())
    {
        if(--i->second.second == 0)
        {
            _vft.erase(i);
        }
    }
}

//
// Add a factory to the value factory table.
//
void
IceInternal::FactoryTable::addTypeId(int compactId, const string& typeId)
{
    IceUtil::Mutex::Lock lock(_m);
    assert(!typeId.empty() && compactId >= 0);
    TypeIdTable::iterator i = _typeIdTable.find(compactId);
    if(i == _typeIdTable.end())
    {
        _typeIdTable[compactId] = TypeIdPair(typeId, 1);
    }
    else
    {
        i->second.second++;
    }
}

//
// Return the type ID for the given compact ID
//
string
IceInternal::FactoryTable::getTypeId(int compactId) const
{
    IceUtil::Mutex::Lock lock(_m);
    TypeIdTable::const_iterator i = _typeIdTable.find(compactId);
    return i != _typeIdTable.end() ? i->second.first : string();
}

void
IceInternal::FactoryTable::removeTypeId(int compactId)
{
    IceUtil::Mutex::Lock lock(_m);
    TypeIdTable::iterator i = _typeIdTable.find(compactId);
    if(i != _typeIdTable.end())
    {
        if(--i->second.second == 0)
        {
            _typeIdTable.erase(i);
        }
    }
}


