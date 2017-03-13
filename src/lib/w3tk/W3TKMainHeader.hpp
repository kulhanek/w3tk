#ifndef W3TKMainHeaderH
#define W3TKMainHeaderH
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

extern const char* LibBuildVersion_w3tk;

//------------------------------------------------------------------------------

/* currently only UNIX and WIN32 architectures are supported */

#ifdef WIN32 // setup for WIN32 architecture
#ifdef W3TK_COMPILE_DLL
#define W3TK_PACKAGE _export
#define EXPORT
#else
#define W3TK_PACKAGE _import
#define IMPORT
#endif
#define W3TK_VCL
#define W3TK_WIN32
#define __w3tkspec __fastcall
#endif

#ifdef UNIX // setup for UNIX architecture
#define W3TK_PACKAGE
#define __w3tkspec
#define W3TK_UNIX
#endif

#if ! (defined WIN32 ||  defined UNIX)
#error "w3tk: Unsupported system!"
#endif

#define INVALID_PORT        -1

//------------------------------------------------------------------------------

#ifdef UNIX
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

//! illegal socket identifier
#define INVALID_SOCKET -1

//! illegal socket identifier
#define SOCKET_ERROR -1

//! socket ID type
typedef int     CSocket;

//! close socket
inline int closesocket(CSocket socket)
{
    return(close(socket));
}
#endif


//------------------------------------------------------------------------------

#endif
