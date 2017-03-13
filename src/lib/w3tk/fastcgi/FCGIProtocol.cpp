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

#include <FCGIProtocol.hpp>
#include <string.h>
#include <limits.h>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

SFCGIHeader::SFCGIHeader(void)
{
    version = 0;
    type = 0;
    requestIdB1 = 0;
    requestIdB0 = 0;
    contentLengthB1 = 0;
    contentLengthB0 = 0;
    paddingLength = 0;
    reserved = 0;
}

//------------------------------------------------------------------------------

int SFCGIHeader::GetVersion(void) const
{
    return(version);
}

//------------------------------------------------------------------------------

int SFCGIHeader::GetType(void) const
{
    return(type);
}

//------------------------------------------------------------------------------

int SFCGIHeader::GetRequestID(void) const
{
    return( (requestIdB1 << 8) + requestIdB0 );
}

//------------------------------------------------------------------------------

int SFCGIHeader::GetDataLength(void) const
{
    return( (contentLengthB1 << 8) + contentLengthB0 );
}

//------------------------------------------------------------------------------

int SFCGIHeader::GetPaddingLength(void) const
{
    return( paddingLength );
}

//------------------------------------------------------------------------------

void SFCGIHeader::SetVersion(int _version)
{
    version = _version;
}

//------------------------------------------------------------------------------

void SFCGIHeader::SetType(int _type)
{
    type = _type;
}

//------------------------------------------------------------------------------

void SFCGIHeader::SetRequestID(int id)
{
    requestIdB1 = (unsigned char) ((id >>  8) & 0xFF);
    requestIdB0 = (unsigned char) ((id      ) & 0xFF);
}

//------------------------------------------------------------------------------

void SFCGIHeader::SetDataLength(int length)
{
    contentLengthB1 = (unsigned char) ((length >>  8) & 0xFF);
    contentLengthB0 = (unsigned char) ((length      ) & 0xFF);
    paddingLength = 0;
    unsigned char padded_length = (unsigned char) (length & 0x07);
    if( padded_length > 0 ) paddingLength = 8 - padded_length;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

SFCGIBeginRequest::SFCGIBeginRequest(void)
{
    roleB1 = 0;
    roleB0 = 0;
    flags = 0;
    memset(reserved,0,5);
}

//------------------------------------------------------------------------------

int SFCGIBeginRequest::GetRole(void) const
{
    return( (roleB1 << 8) + roleB0 );
}

//------------------------------------------------------------------------------

int SFCGIBeginRequest::GetFlags(void) const
{
    return(flags);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

SFCGIEndRequest::SFCGIEndRequest(void)
{
    appStatusB3 = 0;
    appStatusB2 = 0;
    appStatusB1 = 0;
    appStatusB0 = 0;
    protocolStatus = 0;
    memset(reserved,0,3);
}

//------------------------------------------------------------------------------

void SFCGIEndRequest::SetApplicationStatus(int status)
{
    appStatusB3 = (unsigned char) ((status >> 24) & 0xFF);
    appStatusB2 = (unsigned char) ((status >> 16) & 0xFF);
    appStatusB1 = (unsigned char) ((status >>  8) & 0xFF);
    appStatusB0 = (unsigned char) ((status      ) & 0xFF);
}

//------------------------------------------------------------------------------

void SFCGIEndRequest::SetProtocolStatus(int status)
{
    protocolStatus = status;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


