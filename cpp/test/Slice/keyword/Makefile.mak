# **********************************************************************
#
# Copyright (c) 2003-2015 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..\..\..

CLIENT		= client.exe

TARGETS		= $(CLIENT)

SLICE_OBJS	= .\Key.obj

OBJS		= $(SLICE_OBJS) \
		  .\Client.obj

!include $(top_srcdir)/config/Make.rules.mak

CPPFLAGS	= -I. -I../../include $(CPPFLAGS) -DWIN32_LEAN_AND_MEAN
SLICE2CPPFLAGS	= --underscore $(SLICE2CPPFLAGS)

!if "$(GENERATE_PDB)" == "yes"
CPDBFLAGS        = /pdb:$(CLIENT:.exe=.pdb)
!endif

$(CLIENT): $(OBJS)
	$(LINK) $(LD_EXEFLAGS) $(CPDBFLAGS) $(SETARGV) $(OBJS) $(PREOUT)$@ $(PRELIBS)$(LIBS)
	@if exist $@.manifest echo ^ ^ ^ Embedding manifest using $(MT) && \
	    $(MT) -nologo -manifest $@.manifest -outputresource:$@;#1 && del /q $@.manifest

clean::
	del /q Key.cpp Key.h
