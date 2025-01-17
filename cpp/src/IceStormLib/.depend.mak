
IceStorm.h: \
    "$(slicedir)\IceStorm\IceStorm.ice" \
    "$(slicedir)/Ice/Identity.ice" \
    "$(slicedir)/Ice/SliceChecksumDict.ice" \
    "$(slicedir)/IceStorm/Metrics.ice" \
    "$(slicedir)/Ice/Metrics.ice" \
    "$(slicedir)/Ice/BuiltinSequences.ice"

Metrics.h: \
    "$(slicedir)\IceStorm\Metrics.ice" \
    "$(slicedir)/Ice/Metrics.ice" \
    "$(slicedir)/Ice/BuiltinSequences.ice"

IceStorm.obj: \
	IceStorm.cpp \
    "$(includedir)\IceStorm\IceStorm.h" \
    "$(includedir)\IceUtil\PushDisableWarnings.h" \
    "$(includedir)\Ice\ProxyF.h" \
    "$(includedir)\IceUtil\Shared.h" \
    "$(includedir)\IceUtil\Config.h" \
    "$(includedir)\IceUtil\Atomic.h" \
    "$(includedir)\Ice\Config.h" \
    "$(includedir)\Ice\ProxyHandle.h" \
    "$(includedir)\IceUtil\Handle.h" \
    "$(includedir)\IceUtil\Exception.h" \
    "$(includedir)\Ice\ObjectF.h" \
    "$(includedir)\Ice\Handle.h" \
    "$(includedir)\Ice\ValueF.h" \
    "$(includedir)\Ice\Exception.h" \
    "$(includedir)\Ice\Format.h" \
    "$(includedir)\Ice\LocalObject.h" \
    "$(includedir)\Ice\LocalObjectF.h" \
    "$(includedir)\Ice\StreamHelpers.h" \
    "$(includedir)\IceUtil\ScopedArray.h" \
    "$(includedir)\IceUtil\Iterator.h" \
    "$(includedir)\Ice\Traits.h" \
    "$(includedir)\Ice\Proxy.h" \
    "$(includedir)\IceUtil\Mutex.h" \
    "$(includedir)\IceUtil\Lock.h" \
    "$(includedir)\IceUtil\ThreadException.h" \
    "$(includedir)\IceUtil\Time.h" \
    "$(includedir)\IceUtil\MutexProtocol.h" \
    "$(includedir)\Ice\ProxyFactoryF.h" \
    "$(includedir)\Ice\ConnectionIF.h" \
    "$(includedir)\Ice\RequestHandlerF.h" \
    "$(includedir)\Ice\EndpointF.h" \
    "$(includedir)\IceUtil\Optional.h" \
    "$(includedir)\IceUtil\UndefSysMacros.h" \
    "$(includedir)\IceUtil\PopDisableWarnings.h" \
    "$(includedir)\Ice\EndpointTypes.h" \
    "$(includedir)\Ice\ObjectAdapterF.h" \
    "$(includedir)\Ice\ReferenceF.h" \
    "$(includedir)\Ice\BatchRequestQueueF.h" \
    "$(includedir)\Ice\AsyncResult.h" \
    "$(includedir)\IceUtil\Monitor.h" \
    "$(includedir)\IceUtil\Cond.h" \
    "$(includedir)\IceUtil\UniquePtr.h" \
    "$(includedir)\Ice\CommunicatorF.h" \
    "$(includedir)\Ice\ConnectionF.h" \
    "$(includedir)\Ice\InstanceF.h" \
    "$(includedir)\Ice\AsyncResultF.h" \
    "$(includedir)\Ice\ObserverHelper.h" \
    "$(includedir)\Ice\Instrumentation.h" \
    "$(includedir)\Ice\Current.h" \
    "$(includedir)\Ice\Identity.h" \
    "$(includedir)\Ice\Version.h" \
    "$(includedir)\Ice\InputStream.h" \
    "$(includedir)\IceUtil\StringConverter.h" \
    "$(includedir)\Ice\Object.h" \
    "$(includedir)\Ice\IncomingAsyncF.h" \
    "$(includedir)\Ice\LoggerF.h" \
    "$(includedir)\Ice\ValueFactory.h" \
    "$(includedir)\Ice\Buffer.h" \
    "$(includedir)\Ice\Protocol.h" \
    "$(includedir)\Ice\SlicedDataF.h" \
    "$(includedir)\Ice\UserExceptionFactory.h" \
    "$(includedir)\Ice\FactoryTable.h" \
    "$(includedir)\Ice\VirtualShared.h" \
    "$(includedir)\Ice\LocalException.h" \
    "$(includedir)\Ice\BuiltinSequences.h" \
    "$(includedir)\Ice\GCObject.h" \
    "$(includedir)\IceUtil\MutexPtrLock.h" \
    "$(includedir)\Ice\Value.h" \
    "$(includedir)\Ice\Incoming.h" \
    "$(includedir)\Ice\ServantLocatorF.h" \
    "$(includedir)\Ice\ServantManagerF.h" \
    "$(includedir)\Ice\OutputStream.h" \
    "$(includedir)\Ice\ResponseHandlerF.h" \
    "$(includedir)\Ice\FactoryTableInit.h" \
    "$(includedir)\Ice\DefaultValueFactory.h" \
    "$(includedir)\Ice\SliceChecksumDict.h" \
    "$(includedir)\IceStorm\Metrics.h" \
    "$(includedir)\Ice\Metrics.h" \
    "$(includedir)\IceStorm\Config.h" \
    "$(includedir)\Ice\Outgoing.h" \
    "$(includedir)\Ice\OutgoingAsync.h" \
    "$(includedir)\IceUtil\Timer.h" \
    "$(includedir)\IceUtil\Thread.h" \
    "$(includedir)\Ice\OutgoingAsyncF.h" \
    "$(includedir)\Ice\SliceChecksums.h" \

Metrics.obj: \
	Metrics.cpp \
    "$(includedir)\IceStorm\Metrics.h" \
    "$(includedir)\IceUtil\PushDisableWarnings.h" \
    "$(includedir)\Ice\ProxyF.h" \
    "$(includedir)\IceUtil\Shared.h" \
    "$(includedir)\IceUtil\Config.h" \
    "$(includedir)\IceUtil\Atomic.h" \
    "$(includedir)\Ice\Config.h" \
    "$(includedir)\Ice\ProxyHandle.h" \
    "$(includedir)\IceUtil\Handle.h" \
    "$(includedir)\IceUtil\Exception.h" \
    "$(includedir)\Ice\ObjectF.h" \
    "$(includedir)\Ice\Handle.h" \
    "$(includedir)\Ice\ValueF.h" \
    "$(includedir)\Ice\Exception.h" \
    "$(includedir)\Ice\Format.h" \
    "$(includedir)\Ice\LocalObject.h" \
    "$(includedir)\Ice\LocalObjectF.h" \
    "$(includedir)\Ice\StreamHelpers.h" \
    "$(includedir)\IceUtil\ScopedArray.h" \
    "$(includedir)\IceUtil\Iterator.h" \
    "$(includedir)\Ice\Traits.h" \
    "$(includedir)\Ice\Proxy.h" \
    "$(includedir)\IceUtil\Mutex.h" \
    "$(includedir)\IceUtil\Lock.h" \
    "$(includedir)\IceUtil\ThreadException.h" \
    "$(includedir)\IceUtil\Time.h" \
    "$(includedir)\IceUtil\MutexProtocol.h" \
    "$(includedir)\Ice\ProxyFactoryF.h" \
    "$(includedir)\Ice\ConnectionIF.h" \
    "$(includedir)\Ice\RequestHandlerF.h" \
    "$(includedir)\Ice\EndpointF.h" \
    "$(includedir)\IceUtil\Optional.h" \
    "$(includedir)\IceUtil\UndefSysMacros.h" \
    "$(includedir)\IceUtil\PopDisableWarnings.h" \
    "$(includedir)\Ice\EndpointTypes.h" \
    "$(includedir)\Ice\ObjectAdapterF.h" \
    "$(includedir)\Ice\ReferenceF.h" \
    "$(includedir)\Ice\BatchRequestQueueF.h" \
    "$(includedir)\Ice\AsyncResult.h" \
    "$(includedir)\IceUtil\Monitor.h" \
    "$(includedir)\IceUtil\Cond.h" \
    "$(includedir)\IceUtil\UniquePtr.h" \
    "$(includedir)\Ice\CommunicatorF.h" \
    "$(includedir)\Ice\ConnectionF.h" \
    "$(includedir)\Ice\InstanceF.h" \
    "$(includedir)\Ice\AsyncResultF.h" \
    "$(includedir)\Ice\ObserverHelper.h" \
    "$(includedir)\Ice\Instrumentation.h" \
    "$(includedir)\Ice\Current.h" \
    "$(includedir)\Ice\Identity.h" \
    "$(includedir)\Ice\Version.h" \
    "$(includedir)\Ice\InputStream.h" \
    "$(includedir)\IceUtil\StringConverter.h" \
    "$(includedir)\Ice\Object.h" \
    "$(includedir)\Ice\IncomingAsyncF.h" \
    "$(includedir)\Ice\LoggerF.h" \
    "$(includedir)\Ice\ValueFactory.h" \
    "$(includedir)\Ice\Buffer.h" \
    "$(includedir)\Ice\Protocol.h" \
    "$(includedir)\Ice\SlicedDataF.h" \
    "$(includedir)\Ice\UserExceptionFactory.h" \
    "$(includedir)\Ice\FactoryTable.h" \
    "$(includedir)\Ice\VirtualShared.h" \
    "$(includedir)\Ice\LocalException.h" \
    "$(includedir)\Ice\BuiltinSequences.h" \
    "$(includedir)\Ice\GCObject.h" \
    "$(includedir)\IceUtil\MutexPtrLock.h" \
    "$(includedir)\Ice\Value.h" \
    "$(includedir)\Ice\Incoming.h" \
    "$(includedir)\Ice\ServantLocatorF.h" \
    "$(includedir)\Ice\ServantManagerF.h" \
    "$(includedir)\Ice\OutputStream.h" \
    "$(includedir)\Ice\ResponseHandlerF.h" \
    "$(includedir)\Ice\FactoryTableInit.h" \
    "$(includedir)\Ice\DefaultValueFactory.h" \
    "$(includedir)\Ice\Metrics.h" \
    "$(includedir)\IceStorm\Config.h" \
    "$(includedir)\Ice\Outgoing.h" \
    "$(includedir)\Ice\OutgoingAsync.h" \
    "$(includedir)\IceUtil\Timer.h" \
    "$(includedir)\IceUtil\Thread.h" \
    "$(includedir)\Ice\OutgoingAsyncF.h" \
    "$(includedir)\Ice\SliceChecksums.h" \
    "$(includedir)\Ice\SliceChecksumDict.h" \
