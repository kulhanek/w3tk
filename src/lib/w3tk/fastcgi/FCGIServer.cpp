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

#include <FCGIServer.hpp>
#include <ErrorSystem.hpp>
#include <SmallString.hpp>
#include <SimpleIterator.hpp>
#include <errno.h>
#include <string.h>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CFCGIServer::CFCGIServer(void)
{
    // server information
    ServerPort = INVALID_PORT;
    MaxNumOfConnections = 30;

    NumOfRequests = 0;

    // variable initialization
    ListenSocket = INVALID_SOCKET;
}

//------------------------------------------------------------------------------

CFCGIServer::~CFCGIServer(void)
{
    if( ListenSocket != INVALID_SOCKET ) {
        closesocket(ListenSocket);
        ListenSocket = INVALID_SOCKET;
    }
    TerminateThread();
    WaitForThread();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CFCGIServer::SetPort(int port)
{
    // check if port is in correct range
    if( (port <= 0) && (port > 65535) ) {
        ES_ERROR("server port is not in correct range <1,65535>");
        return(false);
    }
    ServerPort = port;
    return(true);
}

//------------------------------------------------------------------------------

bool CFCGIServer::SetMaxNumOfConnections(int maxcons)
{
    // check if number of maximal conncetions is correct range
    if( maxcons < 0 ) {
        ES_ERROR("number of connection has to greater than zero");
        return(false);
    }
    MaxNumOfConnections = maxcons;
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CFCGIServer::StartServer(void)
{
    int result;
    if( ListenSocket != INVALID_SOCKET ) {
        ES_ERROR("server socket already exists");
        return(false);
    }

    // create socket
    ListenSocket = socket(AF_INET,SOCK_STREAM,0);
    if( ListenSocket == INVALID_SOCKET ) {
        CSmallString error;
        error << "unable to open server socket: " << strerror(errno);
        ES_ERROR(error);
        return(false);
    }

    // bind to port
    sockaddr_in    server_addr;

    server_addr.sin_port = htons(ServerPort);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;

    result = bind(ListenSocket,(sockaddr*)&server_addr,sizeof(server_addr));
    if( result == SOCKET_ERROR ) {
        CSmallString error;
        error << "unable to bind to port: " << strerror(errno);
        ES_ERROR(error);
        closesocket(ListenSocket);
        ListenSocket = INVALID_SOCKET;
        return(false);
    }

    // start listenning
    result = listen(ListenSocket,MaxNumOfConnections);
    if(result == SOCKET_ERROR) {
        CSmallString error;
        error << "unable to listen on server port: " << strerror(errno);
        ES_ERROR(error);
        closesocket(ListenSocket);
        ListenSocket = INVALID_SOCKET;
        return(false);
    }

    // start thread
    if( StartThread() == false ) {
        ES_ERROR("unable to start server thread");
        closesocket(ListenSocket);
        ListenSocket = INVALID_SOCKET;
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CFCGIServer::TerminateServer(void)
{
    TerminateThread();
    return(true);
}

//------------------------------------------------------------------------------

bool CFCGIServer::WaitForServer(void)
{
    WaitForThread();
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CFCGIServer::ExecuteThread(void)
{
    fd_set     ready_to_read;
    timeval    wait_time;

    NumOfRequests = 0;

    while( ThreadTerminated != true ) {
        // it is necessary to reset time since it is modified by select
        wait_time.tv_sec = 0;
        wait_time.tv_usec  = 200000;

        // complete list of sockets to select
        FD_ZERO(&ready_to_read);
        FD_SET(ListenSocket,&ready_to_read);

        int result = select(ListenSocket+1,&ready_to_read,NULL,NULL,&wait_time);
        if( result == 0 ) continue;
        if( result == -1 ) {
            CSmallString error;
            error << "unable to select :" << strerror(errno);
            ES_ERROR(error);
            sleep(5);
            continue;
        }

        // new connection ?
        if( FD_ISSET(ListenSocket,&ready_to_read) != 0 ) {
            if( AcceptRequest() == false ) {
                CSmallString error;
                error << "unable to accept connection";
                ES_ERROR(error);
                continue;
            }
            NumOfRequests++;
        }
    }

    // finish all running requests if they are still pending
    CSimpleIterator<CFCGIRequest>    R(Requests);
    CFCGIRequest*                    p_request;

    while((p_request=R.Current())!=NULL) {
        p_request->FinishRequest();
        R++;
    }

    Requests.RemoveAll();

    // close socket
    closesocket(ListenSocket);
    ListenSocket = INVALID_SOCKET;
}

//------------------------------------------------------------------------------

bool CFCGIServer::AcceptRequest(void)
{
    // accept client connection
    sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);

    CSocket accept_socket = accept(ListenSocket,(sockaddr*)&client_addr,&addrlen);

    if( accept_socket == INVALID_SOCKET ) {
        CSmallString error;
        error << "unable to accept connection :" << strerror(errno);
        ES_ERROR(error);
    } else {
        // and immdiatelly drop connection
        closesocket(accept_socket);
    }

    ES_ERROR("it has to be redefined");
    return(false);
}

//------------------------------------------------------------------------------
