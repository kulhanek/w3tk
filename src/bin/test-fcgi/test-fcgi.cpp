//==============================================================================
//    Copyright 2008 Petr Kulhanek
//
//    This file is part of Ligthweight FastCGI library.
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

#include "fcgiserver.hpp"
#include <ErrorSystem.hpp>
#include <signal.h>

//------------------------------------------------------------------------------

CTestFCGIServer FCGIServer;

//------------------------------------------------------------------------------

void CtrlCSignalHandler(int signal);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int main(void)
{
    // CtrlC signal
    signal(SIGINT,CtrlCSignalHandler);

    printf("\n");
    printf("# Simple FastCGI server");
    printf("\n");

    printf("Starting server - listenning on port 10000 ...\n");
    FCGIServer.SetPort(10000);

    if( FCGIServer.StartServer() == false ) {
        ErrorSystem.PrintErrors();
        return(-1);
    }

    printf("Waiting for server termination ...\n");
    FCGIServer.WaitForServer();

    // print final errors
    ErrorSystem.PrintErrors();
    printf("\n");
    return(0);
}


//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CtrlCSignalHandler(int signal)
{
    printf("\nCtrl+C signal recieved.\n   Initiating server shutdown ... \n");
    printf("Waiting for server finalization ... \n");
    fflush(stdout);
    FCGIServer.TerminateServer();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
