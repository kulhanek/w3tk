#ifndef FCGIProtocolH
#define FCGIProtocolH
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

#include <W3TKMainHeader.hpp>

//------------------------------------------------------------------------------

struct SFCGIHeader {
public:
    SFCGIHeader(void);

    int GetVersion(void) const;
    int GetType(void) const;

    int GetRequestID(void) const;
    int GetDataLength(void) const;
    int GetPaddingLength(void) const;

    void SetVersion(int _version);
    void SetType(int _type);
    void SetRequestID(int id);
    void SetDataLength(int length);

private:
    unsigned char version;
    unsigned char type;
    unsigned char requestIdB1;
    unsigned char requestIdB0;
    unsigned char contentLengthB1;
    unsigned char contentLengthB0;
    unsigned char paddingLength;
    unsigned char reserved;
};

// allowed version values ------------------------
#define FCGI_VERSION_1           1

// allowed type values ---------------------------
#define FCGI_BEGIN_REQUEST       1
#define FCGI_ABORT_REQUEST       2
#define FCGI_END_REQUEST         3
#define FCGI_PARAMS              4
#define FCGI_STDIN               5
#define FCGI_STDOUT              6
#define FCGI_STDERR              7

// unsupported types
// #define FCGI_DATA                8
// #define FCGI_GET_VALUES          9
// #define FCGI_GET_VALUES_RESULT  10
// #define FCGI_UNKNOWN_TYPE       11

// null request ID
#define FCGI_NULL_REQUEST_ID     0

// max data packet length
#define FCGI_MAX_LENGTH 0xFFFF

//------------------------------------------------------------------------------

struct SFCGIBeginRequest {
public:
    SFCGIBeginRequest(void);
    int GetRole(void) const;
    int GetFlags(void) const;

private:
    unsigned char roleB1;
    unsigned char roleB0;
    unsigned char flags;
    unsigned char reserved[5];
};

// allowed role values ---------------------------
#define FCGI_RESPONDER  1
#define FCGI_AUTHORIZER 2
#define FCGI_FILTER     3

// allowed flags values --------------------------
// unsupported values
// #define FCGI_KEEP_CONN  1

//------------------------------------------------------------------------------

struct SFCGIEndRequest {
public:
    SFCGIEndRequest(void);
    void SetApplicationStatus(int status);
    void SetProtocolStatus(int status);

private:
    unsigned char appStatusB3;
    unsigned char appStatusB2;
    unsigned char appStatusB1;
    unsigned char appStatusB0;
    unsigned char protocolStatus;
    unsigned char reserved[3];
};

// allowed protocolStatus values -----------------
#define FCGI_REQUEST_COMPLETE 0

// unsupported values
// #define FCGI_CANT_MPX_CONN    1
// #define FCGI_OVERLOADED       2
// #define FCGI_UNKNOWN_ROLE     3

//------------------------------------------------------------------------------

#endif
