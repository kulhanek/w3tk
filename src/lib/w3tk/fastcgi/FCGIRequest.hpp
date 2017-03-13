#ifndef FCGIRequestH
#define FCGIRequestH
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
#include <FCGIParams.hpp>
#include <FCGIProtocol.hpp>

//------------------------------------------------------------------------------

class CFCGIServer;

//------------------------------------------------------------------------------

enum ERequestStatus {
    ERS_EMPTY,          // request is not accepted
    ERS_READY,          // request was accepted and is ready for further processing
    ERS_FINISHED,       // request is finished
    ERS_ABORTED         // request was aborted
};

//------------------------------------------------------------------------------

class W3TK_PACKAGE CFCGIRequest {
public:
// constructor and destructor -------------------------------------------------
    CFCGIRequest(void);
    ~CFCGIRequest(void);

// main execution methods -----------------------------------------------------
    //! accept request from server
    bool AcceptRequest(CFCGIServer* p_server);

    //! finish request
    bool FinishRequest(void);

    //! abort request
    bool AbortRequest(void);

    //! return FCGI role of the request
    int GetRole(void) const;

    //! set exit code for the request
    void SetExitCode(int err);

// section of public data -----------------------------------------------------
    CFCGIParams   Params;             // list of parameters
    CFCGIStream   InStream;           // input stream
    CFCGIStream   OutStream;          // output stream
    CFCGIStream   ErrStream;          // error stream

// section of private data ----------------------------------------------------
private:
    CFCGIServer*  FCGIServer;
    CSocket         Socket;
    ERequestStatus  RequestStatus;
    int             RequestID;
    int             Role;
    int             ExitCode;

// helper methods -------------------------------------------------------------
    bool BeginRequest(void);
    bool ReadParams(void);
    bool ReadStreamBuffer(CFCGIStream* p_stream);
    bool WriteStreamBuffer(CFCGIStream* p_stream);
    bool EndRequest(void);

    //! read packet header
    bool ReadHeader(SFCGIHeader& header);

    //! read binary data from network stream
    bool ReadData(unsigned char* p_data,unsigned int length);

    //! read padding data
    bool ReadPaddingData(unsigned int length);

    //! write binary data to network stream
    bool WriteData(unsigned char* p_data,unsigned int length);

    //! write padding data
    bool WritePaddingData(unsigned int length);

    friend class CFCGIStream;
};

//------------------------------------------------------------------------------

#endif
