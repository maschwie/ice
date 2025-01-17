// **********************************************************************
//
// Copyright (c) 2003-2015 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#pragma once

[["cpp:header-ext:h", "objc:header-dir:objc"]]

["objc:prefix:ICE"]
module Ice
{

/**
 *
 * A factory for values. Value factories are used in several
 * places, such as when Ice receives a class instance and
 * when Freeze restores a persistent value. Value factories
 * must be implemented by the application writer and registered
 * with the communicator.
 *
 **/
["delegate"]
local interface ValueFactory
{
    /**
     *
     * Create a new value for a given value type. The type is the
     * absolute Slice type id, i.e., the id relative to the
     * unnamed top-level Slice module. For example, the absolute
     * Slice type id for an interface <tt>Bar</tt> in the module
     * <tt>Foo</tt> is <tt>::Foo::Bar</tt>.
     *
     * <p class="Note">The leading "<tt>::</tt>" is required.
     *
     * @param type The value type.
     *
     * @return The value created for the given type, or nil if the
     * factory is unable to create the value.
     *
     **/
    Value create(string type);
};

/**
 *
 * A value factory manager maintains a collection of value factories.
 * An application can supply a custom implementation during communicator
 * initialization, otherwise Ice provides a default implementation.
 *
 * @see ValueFactory
 *
 **/
local interface ValueFactoryManager
{
    /**
     *
     * <p>Add a value factory. Attempting to add a
     * factory with an id for which a factory is already registered
     * throws {@link AlreadyRegisteredException}.</p>
     *
     * <p>When unmarshaling an Ice value, the Ice run time reads the
     * most-derived type id off the wire and attempts to create an
     * instance of the type using a factory. If no instance is created,
     * either because no factory was found, or because all factories
     * returned nil, the behavior of the Ice run time depends on the
     * format with which the value was marshaled:</p>
     *
     * <p>If the value uses the "sliced" format, Ice ascends the class
     * hierarchy until it finds a type that is recognized by a factory,
     * or it reaches the least-derived type. If no factory is found that
     * can create an instance, the run time throws
     * {@link NoValueFactoryException}.</p>
     *
     * <p>If the value uses the "compact" format, Ice immediately raises
     * {@link NoValueFactoryException}.</p>
     *
     * <p>The following order is used to locate a factory for a type:</p>
     *
     * <ol>
     *
     * <li>The Ice run-time looks for a factory registered
     * specifically for the type.</li>
     *
     * <li>If no instance has been created, the Ice run-time looks
     * for the default factory, which is registered with an empty type id.
     * </li>
     *
     * <li>If no instance has been created by any of the preceding
     * steps, the Ice run-time looks for a factory that may have been
     * statically generated by the language mapping for non-abstract classes.
     * </li>
     *
     * </ol>
     *
     * @param factory The factory to add.
     *
     * @param id The type id for which the factory can create instances, or
     * an empty string for the default factory.
     *
     **/
    void add(ValueFactory factory, ["objc:param:sliceId"] string id);

    /**
     *
     * Find an value factory registered with this communicator.
     *
     * @param id The type id for which the factory can create instances,
     * or an empty string for the default factory.
     *
     * @return The value factory, or null if no value factory was
     * found for the given id.
     *
     **/
    ["cpp:const"] ValueFactory find(string id);
};

};
