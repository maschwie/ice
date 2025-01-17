// **********************************************************************
//
// Copyright (c) 2003-2015 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_INPUT_STREAM_H
#define ICE_INPUT_STREAM_H

#include <IceUtil/StringConverter.h>
#include <Ice/CommunicatorF.h>
#include <Ice/InstanceF.h>
#include <Ice/Object.h>
#include <Ice/ValueF.h>
#include <Ice/ProxyF.h>
#include <Ice/LoggerF.h>
#include <Ice/ValueFactory.h>
#include <Ice/Buffer.h>
#include <Ice/Protocol.h>
#include <Ice/SlicedDataF.h>
#include <Ice/UserExceptionFactory.h>
#include <Ice/StreamHelpers.h>
#include <Ice/FactoryTable.h>
#include <Ice/Traits.h>

namespace Ice
{

class UserException;

template<typename T> inline void
patchHandle(void* addr, const ValuePtr& v)
{
#ifdef ICE_CPP11_MAPPING
    ::std::shared_ptr<T>* handle = static_cast<::std::shared_ptr<T>*>(addr);
    *handle = ::std::dynamic_pointer_cast<T>(v);
    if(v && !(*handle))
    {
        IceInternal::Ex::throwUOE(T::ice_staticId(), v);
    }
#else
    IceInternal::Handle<T>* p = static_cast<IceInternal::Handle<T>*>(addr);
    __patch(*p, v); // Generated __patch method, necessary for forward declarations.
#endif
}

class ICE_API InputStream : public IceInternal::Buffer
{
public:

    typedef size_t size_type;
    typedef void (*PatchFunc)(void*, const ValuePtr&);

    //
    // These constructors use the latest encoding version. Without a communicator, the stream
    // will not be able to unmarshal a proxy. For other unmarshaling tasks, you can provide
    // Helpers for objects that are normally provided by a communicator.
    //
    InputStream();
    InputStream(const std::vector<Byte>&);
    InputStream(const std::pair<const Byte*, const Byte*>&);
    InputStream(IceInternal::Buffer&, bool = false);

    //
    // These constructors use the communicator's default encoding version.
    //
    InputStream(const CommunicatorPtr&);
    InputStream(const CommunicatorPtr&, const std::vector<Byte>&);
    InputStream(const CommunicatorPtr&, const std::pair<const Byte*, const Byte*>&);
    InputStream(const CommunicatorPtr&, IceInternal::Buffer&, bool = false);

    //
    // These constructors use the given encoding version. Without a communicator, the stream
    // will not be able to unmarshal a proxy. For other unmarshaling tasks, you can provide
    // Helpers for objects that are normally provided by a communicator.
    //
    InputStream(const EncodingVersion&);
    InputStream(const EncodingVersion&, const std::vector<Byte>&);
    InputStream(const EncodingVersion&, const std::pair<const Byte*, const Byte*>&);
    InputStream(const EncodingVersion&, IceInternal::Buffer&, bool = false);

    //
    // These constructors use the given communicator and encoding version.
    //
    InputStream(const CommunicatorPtr&, const EncodingVersion&);
    InputStream(const CommunicatorPtr&, const EncodingVersion&, const std::vector<Byte>&);
    InputStream(const CommunicatorPtr&, const EncodingVersion&, const std::pair<const Byte*, const Byte*>&);
    InputStream(const CommunicatorPtr&, const EncodingVersion&, IceInternal::Buffer&, bool = false);

    ~InputStream()
    {
        // Inlined for performance reasons.

        if(_currentEncaps != &_preAllocatedEncaps)
        {
            clear(); // Not inlined.
        }
    }

    //
    // Use initialize() if you originally constructed the stream without a communicator.
    //
    void initialize(const CommunicatorPtr&);
    void initialize(const CommunicatorPtr&, const EncodingVersion&);

    void clear();

    //
    // Must return Instance*, because we don't hold an InstancePtr for
    // optimization reasons (see comments below).
    //
    IceInternal::Instance* instance() const { return _instance; } // Inlined for performance reasons.

    void setStringConverters(const IceUtil::StringConverterPtr&, const IceUtil::WstringConverterPtr&);

    void setValueFactoryManager(const ValueFactoryManagerPtr&);

    void setLogger(const LoggerPtr&);

#ifdef ICE_CPP11_MAPPING
    void setCompactIdResolver(std::function<std::string (int)>);
#else
    void setCompactIdResolver(const CompactIdResolverPtr&);
#endif

#ifndef ICE_CPP11_MAPPING
    void setCollectObjects(bool);
#endif

    void setSliceObjects(bool);

    void setTraceSlicing(bool);

    void* getClosure() const;
    void* setClosure(void*);

    void swap(InputStream&);

    void resetEncapsulation();

    void resize(Container::size_type sz)
    {
        b.resize(sz);
        i = b.end();
    }

    void startObject()
    {
        assert(_currentEncaps && _currentEncaps->decoder);
        _currentEncaps->decoder->startInstance(ObjectSlice);
    }
    SlicedDataPtr endObject(bool preserve)
    {
        assert(_currentEncaps && _currentEncaps->decoder);
        return _currentEncaps->decoder->endInstance(preserve);
    }

    void startException()
    {
        assert(_currentEncaps && _currentEncaps->decoder);
        _currentEncaps->decoder->startInstance(ExceptionSlice);
    }
    SlicedDataPtr endException(bool preserve)
    {
        assert(_currentEncaps && _currentEncaps->decoder);
        return _currentEncaps->decoder->endInstance(preserve);
    }

    const EncodingVersion& startEncapsulation()
    {
        Encaps* oldEncaps = _currentEncaps;
        if(!oldEncaps) // First allocated encaps?
        {
            _currentEncaps = &_preAllocatedEncaps;
        }
        else
        {
            _currentEncaps = new Encaps();
            _currentEncaps->previous = oldEncaps;
        }
        _currentEncaps->start = i - b.begin();

        //
        // I don't use readSize() and writeSize() for encapsulations,
        // because when creating an encapsulation, I must know in advance
        // how many bytes the size information will require in the data
        // stream. If I use an Int, it is always 4 bytes. For
        // readSize()/writeSize(), it could be 1 or 5 bytes.
        //
        Int sz;
        read(sz);
        if(sz < 6)
        {
            throwUnmarshalOutOfBoundsException(__FILE__, __LINE__);
        }
        if(i - sizeof(Int) + sz > b.end())
        {
            throwUnmarshalOutOfBoundsException(__FILE__, __LINE__);
        }
        _currentEncaps->sz = sz;

        read(_currentEncaps->encoding);
        IceInternal::checkSupportedEncoding(_currentEncaps->encoding); // Make sure the encoding is supported

        return _currentEncaps->encoding;
    }

    void endEncapsulation()
    {
        assert(_currentEncaps);

        if(_currentEncaps->encoding != Encoding_1_0)
        {
            skipOpts();
            if(i != b.begin() + _currentEncaps->start + _currentEncaps->sz)
            {
                throwEncapsulationException(__FILE__, __LINE__);
            }
        }
        else if(i != b.begin() + _currentEncaps->start + _currentEncaps->sz)
        {
            if(i + 1 != b.begin() + _currentEncaps->start + _currentEncaps->sz)
            {
                throwEncapsulationException(__FILE__, __LINE__);
            }

            //
            // Ice version < 3.3 had a bug where user exceptions with
            // class members could be encoded with a trailing byte
            // when dispatched with AMD. So we tolerate an extra byte
            // in the encapsulation.
            //
            ++i;
        }

        Encaps* oldEncaps = _currentEncaps;
        _currentEncaps = _currentEncaps->previous;
        if(oldEncaps == &_preAllocatedEncaps)
        {
            oldEncaps->reset();
        }
        else
        {
            delete oldEncaps;
        }
    }

    EncodingVersion skipEmptyEncapsulation()
    {
        Int sz;
        read(sz);
        if(sz != static_cast<Int>(sizeof(Int)) + 2)
        {
            throwEncapsulationException(__FILE__, __LINE__);
        }

        if(i + 2 > b.end())
        {
            throwUnmarshalOutOfBoundsException(__FILE__, __LINE__);
        }

        EncodingVersion encoding;
        read(encoding);
        return encoding;
    }

    EncodingVersion readEncapsulation(const Byte*& v, Int& sz)
    {
        EncodingVersion encoding;
        v = i;
        read(sz);
        if(sz < 6)
        {
            throwEncapsulationException(__FILE__, __LINE__);
        }
        if(i - sizeof(Int) + sz > b.end())
        {
            throwUnmarshalOutOfBoundsException(__FILE__, __LINE__);
        }

        read(encoding);
        i += sz - sizeof(Int) - 2;
        return encoding;
    }

    const EncodingVersion& getEncoding() const
    {
        return _currentEncaps ? _currentEncaps->encoding : _encoding;
    }

    Int getEncapsSize();
    EncodingVersion skipEncapsulation();

    std::string startSlice()
    {
        assert(_currentEncaps && _currentEncaps->decoder);
        return _currentEncaps->decoder->startSlice();
    }
    void endSlice()
    {
        assert(_currentEncaps && _currentEncaps->decoder);
        _currentEncaps->decoder->endSlice();
    }
    void skipSlice()
    {
        assert(_currentEncaps && _currentEncaps->decoder);
        _currentEncaps->decoder->skipSlice();
    }

    void readPendingObjects();

    Int readSize() // Inlined for performance reasons.
    {
        Byte byte;
        read(byte);
        unsigned char val = static_cast<unsigned char>(byte);
        if(val == 255)
        {
            Int v;
            read(v);
            if(v < 0)
            {
                throwUnmarshalOutOfBoundsException(__FILE__, __LINE__);
            }
            return v;
        }
        else
        {
            return static_cast<Int>(static_cast<unsigned char>(byte));
        }
    }

    Int readAndCheckSeqSize(int);

    void readBlob(std::vector<Byte>&, Int);

    void readBlob(const Byte*& v, Container::size_type sz)
    {
        if(sz > 0)
        {
            v = i;
            if(static_cast<Container::size_type>(b.end() - i) < sz)
            {
                throwUnmarshalOutOfBoundsException(__FILE__, __LINE__);
            }
            i += sz;
        }
        else
        {
            v = i;
        }
    }

    template<typename T> void read(T& v)
    {
        StreamHelper<T, StreamableTraits<T>::helper>::read(this, v);
    }

    template<typename T> void read(Int tag, IceUtil::Optional<T>& v)
    {
        if(readOpt(tag, StreamOptionalHelper<T,
                                                  StreamableTraits<T>::helper,
                                                  StreamableTraits<T>::fixedLength>::optionalFormat))
        {
            v.__setIsSet();
            StreamOptionalHelper<T,
                                      StreamableTraits<T>::helper,
                                      StreamableTraits<T>::fixedLength>::read(this, *v);
        }
        else
        {
            v = IceUtil::None;
        }
    }

    // Read type and tag for optionals
    bool readOpt(Int tag, OptionalFormat expectedFormat)
    {
        assert(_currentEncaps);
        if(_currentEncaps->decoder)
        {
            return _currentEncaps->decoder->readOpt(tag, expectedFormat);
        }
        else
        {
            return readOptImpl(tag, expectedFormat);
        }
    }

    // Byte
    void read(Byte& v)
    {
        if(i >= b.end())
        {
            throwUnmarshalOutOfBoundsException(__FILE__, __LINE__);
        }
        v = *i++;
    }
    void read(std::vector<Byte>&);
    void read(std::pair<const Byte*, const Byte*>&);

    // This method is useful for generic stream helpers
    void read(std::pair<const Byte*, const Byte*>& p, ::IceUtil::ScopedArray<Byte>& result)
    {
        result.reset();
        read(p);
    }

    // Bool
    void read(bool& v)
    {
        if(i >= b.end())
        {
            throwUnmarshalOutOfBoundsException(__FILE__, __LINE__);
        }
        v = (0 != *i++);
    }
    void read(std::vector<bool>&);
    void read(std::pair<const bool*, const bool*>&, ::IceUtil::ScopedArray<bool>&);

    // Short
    void read(Short&);
    void read(std::vector<Short>&);
    void read(std::pair<const Short*, const Short*>&, ::IceUtil::ScopedArray<Short>&);

    // Int
    void read(Int& v) // Inlined for performance reasons.
    {
        if(b.end() - i < static_cast<int>(sizeof(Int)))
        {
            throwUnmarshalOutOfBoundsException(__FILE__, __LINE__);
        }
        const Byte* src = &(*i);
        i += sizeof(Int);
#ifdef ICE_BIG_ENDIAN
        Byte* dest = reinterpret_cast<Byte*>(&v) + sizeof(Int) - 1;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest = *src;
#else
        Byte* dest = reinterpret_cast<Byte*>(&v);
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest = *src;
#endif
    }

    void read(std::vector<Int>&);
    void read(std::pair<const Int*, const Int*>&, ::IceUtil::ScopedArray<Int>&);

    // Long

    void read(Long&);
    void read(std::vector<Long>&);
    void read(std::pair<const Long*, const Long*>&, ::IceUtil::ScopedArray<Long>&);

    // Float
    void read(Float&);
    void read(std::vector<Float>&);
    void read(std::pair<const Float*, const Float*>&, ::IceUtil::ScopedArray<Float>&);

    // Double
    void read(Double&);
    void read(std::vector<Double>&);
    void read(std::pair<const Double*, const Double*>&, ::IceUtil::ScopedArray<Double>&);

    // String
    void read(std::string& v, bool convert = true)
    {
        Int sz = readSize();
        if(sz > 0)
        {
            if(b.end() - i < sz)
            {
                throwUnmarshalOutOfBoundsException(__FILE__, __LINE__);
            }
            if(convert && _stringConverter)
            {
                readConverted(v, sz);
            }
            else
            {
                std::string(reinterpret_cast<const char*>(&*i), reinterpret_cast<const char*>(&*i) + sz).swap(v);
            }
            i += sz;
        }
        else
        {
            v.clear();
        }
    }

    // For custom strings, convert = false
    void read(const char*& vdata, size_t& vsize)
    {
        Int sz = readSize();
        if(sz > 0)
        {
            if(b.end() - i < sz)
            {
                throwUnmarshalOutOfBoundsException(__FILE__, __LINE__);
            }

            vdata = reinterpret_cast<const char*>(&*i);
            vsize = static_cast<size_t>(sz);
            i += sz;
        }
        else
        {
            vdata = 0;
            vsize = 0;
        }
    }

    // For custom strings, convert = true
    void read(const char*& vdata, size_t& vsize, std::string& holder)
    {
        if(!_stringConverter)
        {
            holder.clear();
            read(vdata, vsize);
        }
        else
        {
            Int sz = readSize();
            if(sz > 0)
            {
                if(b.end() - i < sz)
                {
                    throwUnmarshalOutOfBoundsException(__FILE__, __LINE__);
                }

                readConverted(holder, sz);
                vdata = holder.data();
                vsize = holder.size();
            }
            else
            {
                holder.clear();
                vdata = 0;
                vsize = 0;
            }
        }
    }

    void read(std::vector<std::string>&, bool = true);

    void read(std::wstring&);
    void read(std::vector<std::wstring>&);

    // Proxy
#ifdef ICE_CPP11_MAPPING
    std::shared_ptr<ObjectPrx> readProxy();

    template<typename T, typename ::std::enable_if<::std::is_base_of<ObjectPrx, T>::value>::type* = nullptr>
    void read(::std::shared_ptr<T>& v)
    {
        ::std::shared_ptr<ObjectPrx> proxy(readProxy());
        if(!proxy)
        {
            v = 0;
        }
        else
        {
            v = ::IceInternal::createProxy<T>();
            v->__copyFrom(proxy);
        }
    }
#else
    void read(ObjectPrx&);
    template<typename T> void read(IceInternal::ProxyHandle<T>& v)
    {
        __read(this, v); // Generated __read method, necessary for forward declarations.
    }
#endif

    // Class
#ifdef ICE_CPP11_MAPPING // C++11 mapping
    template<typename T, typename ::std::enable_if<::std::is_base_of<Value, T>::value>::type* = nullptr>
    void read(::std::shared_ptr<T>& v)
    {
        read(&patchHandle<T>, &v);
    }
#else // C++98 mapping
    template<typename T> void read(IceInternal::Handle<T>& v)
    {
        read(&patchHandle<T>, &v);
    }
#endif

    void read(PatchFunc patchFunc, void* patchAddr)
    {
        initEncaps();
        _currentEncaps->decoder->read(patchFunc, patchAddr);
    }

    // Enum
    Int readEnum(Int);

    // Exception
    void throwException(const Ice::UserExceptionFactoryPtr& = 0);

    // Read/write/skip optionals
    bool readOptImpl(Int, OptionalFormat);
    void skipOpt(OptionalFormat);
    void skipOpts();

    // Skip bytes from the stream
    void skip(size_type size)
    {
        if(i + size > b.end())
        {
            throwUnmarshalOutOfBoundsException(__FILE__, __LINE__);
        }
        i += size;
    }
    void skipSize()
    {
        Byte bt;
        read(bt);
        if(static_cast<unsigned char>(bt) == 255)
        {
            skip(4);
        }
    }

    size_type pos()
    {
        return i - b.begin();
    }

    InputStream(IceInternal::Instance*, const EncodingVersion&);
    InputStream(IceInternal::Instance*, const EncodingVersion&, IceInternal::Buffer&, bool = false);

    void initialize(IceInternal::Instance*, const EncodingVersion&);

private:

    void initialize(const EncodingVersion&);

    //
    // String
    //
    void readConverted(std::string&, Int);

    //
    // We can't throw these exception from inline functions from within
    // this file, because we cannot include the header with the
    // exceptions. Doing so would screw up the whole include file
    // ordering.
    //
    void throwUnmarshalOutOfBoundsException(const char*, int);
    void throwEncapsulationException(const char*, int);

    std::string resolveCompactId(int) const;

    void postUnmarshal(const ValuePtr&) const;

    class Encaps;
    enum SliceType { NoSlice, ObjectSlice, ExceptionSlice };

    void traceSkipSlice(const std::string&, SliceType) const;

    ValueFactoryManagerPtr valueFactoryManager() const;

    LoggerPtr logger() const;

#ifdef ICE_CPP11_MAPPING
    std::function<std::string (int)> compactIdResolver() const;
#else
    CompactIdResolverPtr compactIdResolver() const;
#endif

    //
    // Optimization. The instance may not be deleted while a
    // stack-allocated stream still holds it.
    //
    IceInternal::Instance* _instance;

    typedef std::vector<ValuePtr> ObjectList;

    class ICE_API EncapsDecoder : private ::IceUtil::noncopyable
    {
    public:

        virtual ~EncapsDecoder() { }

        virtual void read(PatchFunc, void*) = 0;
        virtual void throwException(const Ice::UserExceptionFactoryPtr&) = 0;

        virtual void startInstance(SliceType) = 0;
        virtual SlicedDataPtr endInstance(bool) = 0;
        virtual const std::string& startSlice() = 0;
        virtual void endSlice() = 0;
        virtual void skipSlice() = 0;

        virtual bool readOpt(Int, OptionalFormat)
        {
            return false;
        }

        virtual void readPendingObjects()
        {
        }

    protected:

        EncapsDecoder(InputStream* stream, Encaps* encaps, bool sliceObjects, const Ice::ValueFactoryManagerPtr& f) :
            _stream(stream), _encaps(encaps), _sliceObjects(sliceObjects), _valueFactoryManager(f), _typeIdIndex(0)
        {
        }

        std::string readTypeId(bool);
        ValuePtr newInstance(const std::string&);

        void addPatchEntry(Int, PatchFunc, void*);
        void unmarshal(Int, const ValuePtr&);

        typedef std::map<Int, ValuePtr> IndexToPtrMap;
        typedef std::map<Int, std::string> TypeIdMap;

        struct PatchEntry
        {
            PatchFunc patchFunc;
            void* patchAddr;
        };
        typedef std::vector<PatchEntry> PatchList;
        typedef std::map<Int, PatchList> PatchMap;

        InputStream* _stream;
        Encaps* _encaps;
        const bool _sliceObjects;
        Ice::ValueFactoryManagerPtr _valueFactoryManager;

        // Encapsulation attributes for object un-marshalling
        PatchMap _patchMap;

    private:

        // Encapsulation attributes for object un-marshalling
        IndexToPtrMap _unmarshaledMap;
        TypeIdMap _typeIdMap;
        Int _typeIdIndex;
        ObjectList _objectList;
    };

    class ICE_API EncapsDecoder10 : public EncapsDecoder
    {
    public:

        EncapsDecoder10(InputStream* stream, Encaps* encaps, bool sliceObjects, const Ice::ValueFactoryManagerPtr& f) :
            EncapsDecoder(stream, encaps, sliceObjects, f), _sliceType(NoSlice)
        {
        }

        virtual void read(PatchFunc, void*);
        virtual void throwException(const Ice::UserExceptionFactoryPtr&);

        virtual void startInstance(SliceType);
        virtual SlicedDataPtr endInstance(bool);
        virtual const std::string& startSlice();
        virtual void endSlice();
        virtual void skipSlice();

        virtual void readPendingObjects();

    private:

        void readInstance();

        // Instance attributes
        SliceType _sliceType;
        bool _skipFirstSlice;

        // Slice attributes
        Int _sliceSize;
        std::string _typeId;
    };

    class ICE_API EncapsDecoder11 : public EncapsDecoder
    {
    public:

        EncapsDecoder11(InputStream* stream, Encaps* encaps, bool sliceObjects, const Ice::ValueFactoryManagerPtr& f) :
            EncapsDecoder(stream, encaps, sliceObjects, f), _preAllocatedInstanceData(0), _current(0), _objectIdIndex(1)
        {
        }

        virtual void read(PatchFunc, void*);
        virtual void throwException(const Ice::UserExceptionFactoryPtr&);

        virtual void startInstance(SliceType);
        virtual SlicedDataPtr endInstance(bool);
        virtual const std::string& startSlice();
        virtual void endSlice();
        virtual void skipSlice();

        virtual bool readOpt(Int, OptionalFormat);

    private:

        Int readInstance(Int, PatchFunc, void*);
        SlicedDataPtr readSlicedData();

        struct IndirectPatchEntry
        {
            Int index;
            PatchFunc patchFunc;
            void* patchAddr;
        };
        typedef std::vector<IndirectPatchEntry> IndirectPatchList;

        typedef std::vector<Int> IndexList;
        typedef std::vector<IndexList> IndexListList;

        struct InstanceData
        {
            InstanceData(InstanceData* p) : previous(p), next(0)
            {
                if(previous)
                {
                    previous->next = this;
                }
            }

            ~InstanceData()
            {
                if(next)
                {
                    delete next;
                }
            }

            // Instance attributes
            SliceType sliceType;
            bool skipFirstSlice;
            SliceInfoSeq slices; // Preserved slices.
            IndexListList indirectionTables;

            // Slice attributes
            Byte sliceFlags;
            Int sliceSize;
            std::string typeId;
            int compactId;
            IndirectPatchList indirectPatchList;

            InstanceData* previous;
            InstanceData* next;
        };
        InstanceData _preAllocatedInstanceData;
        InstanceData* _current;

        void push(SliceType sliceType)
        {
            if(!_current)
            {
                _current = &_preAllocatedInstanceData;
            }
            else
            {
                _current = _current->next ? _current->next : new InstanceData(_current);
            }
            _current->sliceType = sliceType;
            _current->skipFirstSlice = false;
        }

        Int _objectIdIndex; // The ID of the next object to un-marshal.
    };

    class Encaps : private ::IceUtil::noncopyable
    {
    public:

        Encaps() : start(0), decoder(0), previous(0)
        {
            // Inlined for performance reasons.
        }
        ~Encaps()
        {
            // Inlined for performance reasons.
            delete decoder;
        }
        void reset()
        {
            // Inlined for performance reasons.
            delete decoder;
            decoder = 0;

            previous = 0;
        }

        Container::size_type start;
        Int sz;
        EncodingVersion encoding;

        EncapsDecoder* decoder;

        Encaps* previous;
    };

    //
    // The encoding version to use when there's no encapsulation to
    // read from. This is for example used to read message headers.
    //
    EncodingVersion _encoding;

    Encaps* _currentEncaps;

    void initEncaps();

    Encaps _preAllocatedEncaps;

#ifndef ICE_CPP11_MAPPING
    bool _collectObjects;
#endif

    bool _traceSlicing;

    void* _closure;

    bool _sliceObjects;

    int _startSeq;
    int _minSeqSize;

    IceUtil::StringConverterPtr _stringConverter;
    IceUtil::WstringConverterPtr _wstringConverter;
    ValueFactoryManagerPtr _valueFactoryManager;
    LoggerPtr _logger;
#ifdef ICE_CPP11_MAPPING
    std::function<std::string (int)> _compactIdResolver;
#else
    CompactIdResolverPtr _compactIdResolver;
#endif
};

} // End namespace Ice

#endif
