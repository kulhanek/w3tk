#ifndef FCGIServerH
#define FCGIServerH
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
#include <SmartThread.hpp>
#include <SimpleMutex.hpp>
#include <SimpleList.hpp>
#include <FCGIRequest.hpp>

//------------------------------------------------------------------------------

class W3TK_PACKAGE CFCGIServer : public CSmartThread {
public:
// constructor and destructor -------------------------------------------------
    CFCGIServer(void);
    ~CFCGIServer(void);

// setup methods --------------------------------------------------------------
    //! set listenning port
    bool SetPort(int port);

    //! set max number of allowed connections
    bool SetMaxNumOfConnections(int maxcons);

// server methods -------------------------------------------------------------
    //! it initiates server
    bool StartServer(void);

    //! request for server termination
    bool TerminateServer(void);

    //! wait until the server execution is terminated
    bool WaitForServer(void);

    //! accept request from server
    virtual bool AcceptRequest(void);

// section of private data ----------------------------------------------------
private:
    int                         ServerPort;             // network port of server
    int                         MaxNumOfConnections;    // maximum allowed connections
    CSocket                     ListenSocket;           // listenning socket
    int                         NumOfRequests;          // number of requests
    CSimpleList<CFCGIRequest> Requests;               // list of active requests

    // main thread execution point
    virtual void ExecuteThread(void);

    friend class CFCGIRequest;
};

//------------------------------------------------------------------------------

#endif
