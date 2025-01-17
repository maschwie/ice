#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2015 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import os, sys, traceback

import Ice
Ice.loadSlice('Test.ice')
Ice.loadSlice('ServerPrivate.ice')
import Test, TestI

def MyValueFactory(type):
    if type == '::Test::I':
        return TestI.II()
    elif type == '::Test::J':
        return TestI.JI()
    elif type == '::Test::H':
        return TestI.HI()
    assert(False) # Should never be reached

def run(args, communicator):
    communicator.getValueFactoryManager().add(MyValueFactory, '::Test::I')
    communicator.getValueFactoryManager().add(MyValueFactory, '::Test::J')
    communicator.getValueFactoryManager().add(MyValueFactory, '::Test::H')

    communicator.getProperties().setProperty("TestAdapter.Endpoints", "default -p 12010")
    adapter = communicator.createObjectAdapter("TestAdapter")
    initial = TestI.InitialI(adapter)
    adapter.add(initial, communicator.stringToIdentity("initial"))
    uoet = TestI.UnexpectedObjectExceptionTestI()
    adapter.add(uoet, communicator.stringToIdentity("uoet"))
    adapter.activate()
    communicator.waitForShutdown()
    return True

try:
    communicator = Ice.initialize(sys.argv)
    status = run(sys.argv, communicator)
except:
    traceback.print_exc()
    status = False

if communicator:
    try:
        communicator.destroy()
    except:
        traceback.print_exc()
        status = False

sys.exit(not status)
