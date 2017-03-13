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

#include "GraphFCGIServer.h"
#include <FCGIRequest.hpp>
#include <ErrorSystem.hpp>
#include <SmallTimeAndDate.hpp>
#include <signal.h>
#include "PieChart.h"

//------------------------------------------------------------------------------

CGraphFCGIServer GraphFCGIServer;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CGraphFCGIServer::CGraphFCGIServer(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CGraphFCGIServer::Init(int argc,char* argv[])
{
    // encode program options
    int result = Options.ParseCmdLine(argc,argv);

    // should we exit or was it error?
    if( result != SO_CONTINUE ) return(result);

    // print header --------------------------------------------------------------
    if( Options.GetOptVerbose() ) {
        CSmallTimeAndDate dt;
        dt.GetActualTimeAndDate();

        printf("\n");
        printf("# ==============================================================================\n");
        printf("# Graph FastCGIServer (W3TK utility) started at %s\n",(const char*)dt.GetSDateAndTime());
        printf("# ==============================================================================\n");
        printf("# Port : %d\n",Options.GetOptPort());

        //    printf("# Output format      : %s\n",(const char*)Options.GetOptOutputFormat());
        printf("# ------------------------------------------------------------------------------\n");
        printf("\n");
    }

    return( result );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CGraphFCGIServer::Run(void)
{

    // CtrlC signal
    signal(SIGINT,CtrlCSignalHandler);

    SetPort(Options.GetOptPort());

    if( StartServer() == false ) {
        return(false);
    }

    if( Options.GetOptVerbose() ) printf("Waiting for server termination ...\n");
    WaitForServer();

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CGraphFCGIServer::Finalize(void)
{
    if( Options.GetOptVerbose() ) {
        ErrorSystem.PrintErrors(stderr);

        CSmallTimeAndDate dt;
        dt.GetActualTimeAndDate();

        fprintf(stdout,"\n");
        fprintf(stdout,"# ==============================================================================\n");
        fprintf(stdout,"# Graph FastCGIServer terminated at %s\n",(const char*)dt.GetSDateAndTime());
        fprintf(stdout,"# ==============================================================================\n");
        fprintf(stdout,"\n");
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool __w3tkspec CGraphFCGIServer::AcceptRequest(void)
{
    // this is simple FCGI Application with 'Hello world!'

    CFCGIRequest request;

    // accept request
    if( request.AcceptRequest(this) == false ) {
        // unable to accept request
        return(false);
    }

    // write document
    request.OutStream.PutStr("Content-type: image/png\r\n");
    request.OutStream.PutStr("\r\n");

    CGraphCommands graph_commands;

    if( graph_commands.ParseCommands(request.Params) == false ) {
        ES_ERROR("unable to parse graph commands","CGraphFCGIServer::AcceptRequest");
        // finish request
        request.FinishRequest();
        return(true);
    }

    // plot graph
    switch(graph_commands.GetGraphType()) {
    case EGT_PIE_CHART: {
        CPieChart pie_chart;
        if( pie_chart.Plot(graph_commands) == false ) {
            ES_ERROR("unable to plot the graph","CGraphFCGIServer::AcceptRequest");
            request.FinishRequest();
            return(true);
        }
        if( pie_chart.Print(request.OutStream) == false ) {
            ES_ERROR("unable to print the graph","CGraphFCGIServer::AcceptRequest");
            request.FinishRequest();
            return(true);
        }
        request.FinishRequest();
        return(true);
    }
    default:
        ES_ERROR("unsupported graph type","CGraphFCGIServer::AcceptRequest");
        request.FinishRequest();
        return(true);
    }

}


//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CGraphFCGIServer::CtrlCSignalHandler(int signal)
{
    printf("\nCtrl+C signal recieved.\n   Initiating server shutdown ... \n");
    printf("Waiting for server finalization ... \n");
    fflush(stdout);
    GraphFCGIServer.TerminateServer();
    if( ! GraphFCGIServer.Options.GetOptVerbose() )printf("\n");
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
