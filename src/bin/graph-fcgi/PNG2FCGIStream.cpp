//==============================================================================
//    Copyright 2008 Petr Kulhanek
//
//    This file is part of WWW ToolKit (w3tk) library.
//
//    This library is free software; you can redistribute it and/or
//    modify it under the terms of the GNU Lesser General Public
//    License as published by the Free Software Foundation; either
//    version 2.1 of the License, or (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public
//    License along with this library; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin Street, Fifth Floor,
//    Boston, MA  02110-1301  USA
//==============================================================================

#include "PNG2FCGIStream.h"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

static int FCGIStreamSink(void* context,const char* buffer, int len);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void gdImagePng2FCGIStream(gdImagePtr im, CFCGIStream* p_stream)
{
    gdSink mySink;
    mySink.context = (void *)p_stream;
    mySink.sink = FCGIStreamSink;

    gdImagePngToSink(im, &mySink);
}

//------------------------------------------------------------------------------

static int FCGIStreamSink(void* context,const char* buffer, int len)
{
    CFCGIStream* p_stream = (CFCGIStream*)context;
    return(p_stream->PutStr(buffer,len));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

