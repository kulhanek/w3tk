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

#include <FCGIStream.hpp>
#include <FCGIRequest.hpp>
#include <FCGIProtocol.hpp>
#include <string.h>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CFCGIStream::CFCGIStream(CFCGIRequest* p_request,int type)
{
    Request = p_request;
    Type = type;
    Closed = false;
    BufferLength = FCGI_MAX_LENGTH;
    Buffer = new char[BufferLength];
    Position = Buffer;

    switch(Type) {
    case FCGI_PARAMS:
    case FCGI_STDIN:
        Stop = Buffer;
        break;
    case FCGI_STDOUT:
    case FCGI_STDERR:
        Stop = Buffer + BufferLength;
        break;
    default:
        break;
    }
}

//------------------------------------------------------------------------------

CFCGIStream::~CFCGIStream(void)
{
    if( Buffer != NULL ) delete[] Buffer;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CFCGIStream::GetType(void) const
{
    return(Type);
}

//------------------------------------------------------------------------------

bool CFCGIStream::IsItClosed(void) const
{
    return(Closed);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CFCGIStream::GetChar(void)
{
    // is it reader?
    switch(Type) {
    case FCGI_PARAMS:
    case FCGI_STDIN:
        // allowed streams
        break;
    default:
        return(EOF);
        break;
    }

    // is stream closed?
    if( Closed == true ) return(EOF);

    // is there room in the buffer?
    if( Position != Stop ) {
        int c = *Position;  // get data
        Position++; // move in buffer
        return(c);
    }

    if( Request->ReadStreamBuffer(this) == false ) return(EOF);

    // is stream closed?
    if( Closed == true ) return(EOF);

    // is there room in the buffer?
    if( Position != Stop ) {
        int c = *Position;  // get data
        Position++; // move in buffer
        return(c);
    }

    // error
    return(EOF);
}

//------------------------------------------------------------------------------

int CFCGIStream::PutChar(int c)
{
    // is it writer?
    switch(Type) {
    case FCGI_STDOUT:
    case FCGI_STDERR:
        // allowed streams
        break;
    default:
        return(EOF);
        break;
    }

    // is stream closed?
    if( Closed == true ) return(EOF);

    // is there room in the buffer?
    if( Position != Stop ) {
        *Position = c;  // set data
        Position++; // move in buffer
        return(c);
    }

    if( Request->WriteStreamBuffer(this) == false ) return(EOF);

    // is stream closed?buffer
    if( Closed == true ) return(EOF);

    // is there room in the buffer?
    if( Position != Stop ) {
        *Position = c;  // set data
        Position++; // move in buffer
        return(c);
    }

    // error
    return(EOF);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CFCGIStream::GetStr(char* str,int length)
{
    // is it reader?
    switch(Type) {
    case FCGI_PARAMS:
    case FCGI_STDIN:
        // allowed streams
        break;
    default:
        return(-1);
        break;
    }

    // is stream closed?
    if( Closed == true ) return(-1);

    // buffer contains enough to get string immediately
    if( Stop - Position >= length ) {
        memcpy(str,Position,length);
        Position += length;
        return(length);
    }

    int remaining = length;

    for(;;) {
        // is buffer empty?
        if( Stop == Position ) {
            if( Request->ReadStreamBuffer(this) == false ) {
                return(length-remaining);
            }
        }
        if( Closed == true ) return(length-remaining);

        // do we have enough to read the rest of string?
        if( Stop - Position >= remaining ) {
            memcpy(&str[length-remaining],Position,remaining);
            Position += remaining;
            return(length);     // everything was read
        }

        // read to the end of buffer
        memcpy(&str[length-remaining],Position,Stop - Position);
        remaining -= Stop - Position;
        Position = Stop;
    }

    // this code cannot be reached
    return(-1);
}

//------------------------------------------------------------------------------

int CFCGIStream::GetStr(CSmallString& str,int length)
{
    str.SetLength(length);
    int result = 0;
    if( length > 0 ) {
        result = GetStr(str.GetBuffer(),length);
        str.GetBuffer()[length] = '\0';    // set terminal character
    }
    return(result);
}

//------------------------------------------------------------------------------

int CFCGIStream::PutStr(const char* str,int length)
{
    // is it writer?
    switch(Type) {
    case FCGI_STDOUT:
    case FCGI_STDERR:
        // allowed streams
        break;
    default:
        return(0);
        break;
    }

    // is stream closed?
    if( Closed == true ) return(EOF);

    // get string length
    if(length == -1) length = strlen(str);

    // buffer contains enough room to put string immediately
    if( Stop - Position >= length ) {
        memcpy(Position,str,length);
        Position += length;
        return(length);
    }

    int remaining = length;

    for(;;) {
        // is buffer full?
        if( Stop == Position ) {
            if( Request->WriteStreamBuffer(this) == false ) {
                return(length-remaining);
            }
        }
        if( Closed == true ) return(length-remaining);

        // do we have enough room to write the rest of string?
        if( Stop - Position >= remaining ) {
            memcpy(Position,&str[length-remaining],remaining);
            Position += remaining;
            return(length);     // everything was written
        }

        // write to the end of buffer
        memcpy(Position,&str[length-remaining],Stop - Position);
        remaining -= Stop - Position;
        Position = Stop;
    }

    // this code cannot be reached
    return(false);
}

//------------------------------------------------------------------------------

int CFCGIStream::PutStr(const CSmallString& str,int length)
{
    int result = PutStr(str.GetBuffer(),str.GetLength());
    return(result);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CFCGIStream::Close(void)
{
    // write data in the buffer
    if( Position != Buffer ) {
        if( Request->WriteStreamBuffer(this) == false ) {
            return(false);
        }
    }

    // write EOF to the stream
    if( Request->WriteStreamBuffer(this) == false ) {
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
