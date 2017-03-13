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

#include <FCGIRequest.hpp>
#include <FCGIProtocol.hpp>
#include <FCGIServer.hpp>
#include <ErrorSystem.hpp>
#include <errno.h>
#include <string.h>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CFCGIRequest::CFCGIRequest(void)
    : InStream(this,FCGI_STDIN),
      OutStream(this,FCGI_STDOUT),
      ErrStream(this,FCGI_STDERR)
{
    FCGIServer = NULL;
    RequestStatus = ERS_EMPTY;
    Socket = INVALID_SOCKET;
    Role = 0;
    ExitCode = 0;
    RequestID = FCGI_NULL_REQUEST_ID;
}

//------------------------------------------------------------------------------

CFCGIRequest::~CFCGIRequest(void)
{
    if( Socket != INVALID_SOCKET ) closesocket(Socket);
    Socket = INVALID_SOCKET;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CFCGIRequest::AcceptRequest(CFCGIServer* p_server)
{
    if( p_server == NULL ) {
        ES_ERROR("p_server is NULL");
        return(false);
    }

    if( RequestStatus == ERS_READY ) {
        CSmallString error;
        error << "request is already ready";
        ES_ERROR(error);
        return(false);
    }

    FCGIServer = p_server;
    RequestStatus = ERS_EMPTY;
    RequestID = 0;
    Role = 0;
    ExitCode = 0;

    // accept client connection
    sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);

    Socket = accept(p_server->ListenSocket,(sockaddr*)&client_addr,&addrlen);

    if( Socket == INVALID_SOCKET ) {
        CSmallString error;
        error << "unable to accept connection :" << strerror(errno);
        ES_ERROR(error);
        AbortRequest();
        return(false);
    }

    if( BeginRequest() == false ) {
        ES_ERROR("unable to begin request");
        return(false);
    }

    if( ReadParams() == false ) {
        ES_ERROR("unable to begin request");
        return(false);
    }

    RequestStatus = ERS_READY;

    return(true);
}

//------------------------------------------------------------------------------

bool CFCGIRequest::FinishRequest(void)
{
    if( RequestStatus != ERS_READY ) {
        CSmallString error;
        error << "request is not ready";
        ES_ERROR(error);
        return(false);
    }

    // close output streams
    OutStream.Close();
    ErrStream.Close();

    // and terminate request
    if( EndRequest() == false ) {
        ES_ERROR("unable to end request");
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CFCGIRequest::AbortRequest(void)
{
    RequestStatus = ERS_ABORTED;
    Role = 0;
    if( Socket != INVALID_SOCKET ) closesocket(Socket);
    Socket = INVALID_SOCKET;

    return(true);
}

//------------------------------------------------------------------------------

int CFCGIRequest::GetRole(void) const
{
    return(Role);
}

//------------------------------------------------------------------------------

void CFCGIRequest::SetExitCode(int err)
{
    ExitCode = err;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CFCGIRequest::BeginRequest(void)
{
    SFCGIHeader header;

    // read datagram header
    if( ReadHeader(header) == false ) {
        ES_ERROR("unable to read header");
        return(false);
    }

    if( header.GetType() != FCGI_BEGIN_REQUEST ) {
        ES_ERROR("expecting FCGI_BEGIN_REQUEST packet");
        AbortRequest();
        return(false);
    }

    // read begin data
    SFCGIBeginRequest begin_request;

    if( sizeof(begin_request) != header.GetDataLength() ) {
        ES_ERROR("data length mismatch");
        AbortRequest();
        return(false);
    }

    if( ReadData((unsigned char*)&begin_request,sizeof(begin_request)) == false ) {
        ES_ERROR("unable to read begin data");
        AbortRequest();
        return(false);
    }

    if( begin_request.GetFlags() != 0 ) {
        ES_ERROR("unsupported flag");
        AbortRequest();
        return(false);
    }

    // get role
    Role = begin_request.GetRole();

    // is it supported role?
    switch(Role) {
    case FCGI_RESPONDER:
    case FCGI_AUTHORIZER:
    case FCGI_FILTER:
        break;
    default:
        ES_ERROR("unsupported role");
        AbortRequest();
        return(false);
    }

    // add role to the list of params
    switch(Role) {
    case FCGI_RESPONDER:
        Params.SetParam("FCGI_ROLE","RESPONDER");
        break;
    case FCGI_AUTHORIZER:
        Params.SetParam("FCGI_ROLE","AUTHORIZER");
        break;
    case FCGI_FILTER:
        Params.SetParam("FCGI_ROLE","FILTER");
        break;
    }

    // any padding data in packet?
    if( header.GetPaddingLength() > 0 ) ReadPaddingData(header.GetPaddingLength());

    return(true);
}

//------------------------------------------------------------------------------

bool CFCGIRequest::ReadParams(void)
{
    CFCGIStream stream(this,FCGI_PARAMS);

    if( Params.LoadParamsFromStream(stream) == false ) {
        ES_ERROR("unable to read params");
        AbortRequest();
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CFCGIRequest::EndRequest(void)
{
    // prepare end request data
    SFCGIEndRequest end_request;

    end_request.SetApplicationStatus(ExitCode);
    end_request.SetProtocolStatus(FCGI_REQUEST_COMPLETE);

    SFCGIHeader header;

    header.SetVersion(FCGI_VERSION_1);
    header.SetType(FCGI_END_REQUEST);
    header.SetRequestID(RequestID);
    header.SetDataLength(sizeof(end_request));


    if( WriteData((unsigned char*)&header,sizeof(header)) == false ) {
        ES_ERROR("unable to write end data header");
        AbortRequest();
        return(false);
    }

    if( WriteData((unsigned char*)&end_request,sizeof(end_request)) == false ) {
        ES_ERROR("unable to write end data");
        AbortRequest();
        return(false);
    }

    // any padding data in packet?
    if( header.GetPaddingLength() > 0 ) WritePaddingData(header.GetPaddingLength());

    RequestStatus = ERS_FINISHED;

    //   No, keep selecting for 'read' and wait until 'read' returns zero. That's
    // how a closed connection is indicated to a non-blocking program.

    fd_set     ready_to_read;
    timeval    wait_time;

    for(int i=0; i < 100; i++ ) {
        if( RequestStatus == ERS_ABORTED ) {
            ES_ERROR("request was aborted");
            return(false);
        }

        wait_time.tv_sec = 0;
        wait_time.tv_usec  = 500000;

        // complete list of sockets to select
        FD_ZERO(&ready_to_read);
        FD_SET(Socket,&ready_to_read);

        int result = select(Socket+1,&ready_to_read,NULL,NULL,&wait_time);
        if( result <= 0 ) continue;

        // data ready?
        if( FD_ISSET(Socket,&ready_to_read) == 0 ) {
            continue;
        }
        char buffer[100];
        int recv_len = recv(Socket,buffer,100,MSG_DONTWAIT);
        if( recv_len == SOCKET_ERROR ) {
            if( errno == EAGAIN ) continue;
            ES_ERROR("recv failed");
            return(false);
        }
        if( recv_len == 0 ) break;  // connection was terminated by a client
    }

    if( Socket != INVALID_SOCKET ) closesocket(Socket);
    Socket = INVALID_SOCKET;

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CFCGIRequest::ReadHeader(SFCGIHeader& header)
{
    // read data
    if( ReadData((unsigned char*)&header,sizeof(header)) == false ) {
        ES_ERROR("unable to read header data");
        AbortRequest();
        return(false);
    }

    // check version
    if( header.GetVersion() != FCGI_VERSION_1 ) {
        ES_ERROR("unsupported FCGI version");
        AbortRequest();
        return(false);
    }

    // unsupported type?
    switch(header.GetType()) {
    case FCGI_BEGIN_REQUEST:
    case FCGI_ABORT_REQUEST:
    case FCGI_END_REQUEST:
    case FCGI_PARAMS:
    case FCGI_STDIN:
    case FCGI_STDOUT:
    case FCGI_STDERR:
        break;
    default:
        ES_ERROR("unsupported FCGI packet type");
        AbortRequest();
        return(false);
    }

    // is it FCGI_BEGIN_REQUEST ?
    if( header.GetType() == FCGI_BEGIN_REQUEST ) {
        // set request ID
        RequestID = header.GetRequestID();
    }

    if( RequestID != header.GetRequestID() ) {
        ES_ERROR("request ID mismatch");
        AbortRequest();
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CFCGIRequest::ReadStreamBuffer(CFCGIStream* p_stream)
{
    if( p_stream == NULL ) {
        ES_ERROR("p_stream is NULL");
        AbortRequest();
        return(false);
    }

    switch(p_stream->GetType()) {
    case FCGI_PARAMS:
    case FCGI_STDIN:
        // these are only allowed streams for reading
        break;
    default:
        ES_ERROR("incompatible stream type");
        AbortRequest();
        return(false);
    }

    // read header
    SFCGIHeader header;

    // read datagram header
    if( ReadHeader(header) == false ) {
        ES_ERROR("unable to read header");
        return(false);
    }

    if( header.GetType() != p_stream->GetType() ) {
        ES_ERROR("stream type mismatch");
        AbortRequest();
        return(false);
    }

    // is it the end of stream?
    if( header.GetDataLength() == 0 ) {
        p_stream->Closed = true;
        p_stream->Position = p_stream->Buffer;
        p_stream->Stop = p_stream->Buffer;
        return(true);
    }

    // if not - then read next buffer
    if( ReadData((unsigned char*)p_stream->Buffer,header.GetDataLength()) == false ) {
        ES_ERROR("unable to read stream data");
        AbortRequest();
        return(false);
    }

    p_stream->Position = p_stream->Buffer;
    p_stream->Stop = p_stream->Buffer + header.GetDataLength();

    // any padding data in packet?
    if( header.GetPaddingLength() > 0 ) ReadPaddingData(header.GetPaddingLength());

    return(true);
}

//------------------------------------------------------------------------------

bool CFCGIRequest::WriteStreamBuffer(CFCGIStream* p_stream)
{
    if( p_stream == NULL ) {
        ES_ERROR("p_stream is NULL");
        AbortRequest();
        return(false);
    }

    switch(p_stream->GetType()) {
    case FCGI_STDOUT:
    case FCGI_STDERR:
        // these are only allowed streams for writing
        break;
    default:
        ES_ERROR("incompatible stream type");
        AbortRequest();
        return(false);
    }

    // prepare header
    SFCGIHeader header;

    header.SetVersion(FCGI_VERSION_1);
    header.SetType(p_stream->GetType());
    header.SetRequestID(RequestID);
    header.SetDataLength(p_stream->Position - p_stream->Buffer);

    if( WriteData((unsigned char*)&header,sizeof(header)) == false ) {
        ES_ERROR("unable to write stream header");
        AbortRequest();
        return(false);
    }

    // is it the end of stream?
    if( header.GetDataLength() == 0 ) {
        p_stream->Closed = true;
        p_stream->Position = p_stream->Buffer;
        p_stream->Stop = p_stream->Buffer + p_stream->BufferLength;
        return(true);
    }

    if( WriteData((unsigned char*)p_stream->Buffer,header.GetDataLength()) == false ) {
        ES_ERROR("unable to stream data");
        AbortRequest();
        return(false);
    }

    // any padding data in packet?
    if( header.GetPaddingLength() > 0 ) WritePaddingData(header.GetPaddingLength());

    p_stream->Position = p_stream->Buffer;
    p_stream->Stop = p_stream->Buffer + p_stream->BufferLength;

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CFCGIRequest::ReadData(unsigned char* p_data,unsigned int length)
{
    if( Socket == INVALID_SOCKET ) {
        ES_ERROR("invalid socket");
        return(false);
    }

    fd_set ready_to_read;

    timeval wait_time;

    unsigned int read_len = 0;

    while( read_len != length ) {
        if( RequestStatus == ERS_ABORTED ) {
            ES_ERROR("request was aborted");
            return(false);
        }

        wait_time.tv_sec = 0;
        wait_time.tv_usec  = 200000;

        // complete list of sockets to select
        FD_ZERO(&ready_to_read);
        FD_SET(Socket,&ready_to_read);

        int result = select(Socket+1,&ready_to_read,NULL,NULL,&wait_time);
        if( result <= 0 ) continue;

        // data ready?
        if( FD_ISSET(Socket,&ready_to_read) == 0 ) {
            continue;
        }
        int recv_len = recv(Socket,&p_data[read_len],length-read_len,MSG_DONTWAIT);
        if( recv_len == SOCKET_ERROR ) {
            if( errno == EAGAIN ) continue;
            ES_ERROR("recv failed");
            return(false);
        }
        if( recv_len == 0 ) {
            ES_ERROR("send_len is 0");
            return(false);
        }
        read_len += recv_len;
    }

    bool result = read_len == length;
    if( result == false ) {
        ES_ERROR("transaction was not completed");
        return(false);
    }

    return(result);
}

//---------------------------------------------------------------------------

bool CFCGIRequest::ReadPaddingData(unsigned int length)
{
    unsigned char buffer[10];
    unsigned int remaining = length;
    while(remaining > 0) {
        if( remaining > 10 ) {
            if( ReadData(buffer,10) == false ) {
                ES_ERROR("unable to read padding data");
                return(false);
            }
            remaining -= 10;
        } else {
            if( ReadData(buffer,remaining) == false ) {
                ES_ERROR("unable to read padding data");
                return(false);
            }
            remaining = 0;
        }
    }
    return(true);
}

//---------------------------------------------------------------------------

bool CFCGIRequest::WriteData(unsigned char* p_data,unsigned int length)
{
    if( Socket == INVALID_SOCKET ) {
        ES_ERROR("invalid socket");
        return(false);
    }

    fd_set ready_to_write;

    timeval wait_time;

    unsigned int wrote_len = 0;

    while( wrote_len != length ) {
        if( RequestStatus == ERS_ABORTED ) {
            ES_ERROR("request was aborted");
            return(false);
        }

        wait_time.tv_sec = 0;
        wait_time.tv_usec  = 200000;

        // complete list of sockets to select
        FD_ZERO(&ready_to_write);
        FD_SET(Socket,&ready_to_write);

        int result = select(Socket+1,NULL,&ready_to_write,NULL,&wait_time);
        if( result <= 0 ) continue;

        // write ready?
        if( FD_ISSET(Socket,&ready_to_write) == 0 ) {
            continue;
        }
        int send_len = send(Socket,&p_data[wrote_len],length-wrote_len,MSG_DONTWAIT|MSG_NOSIGNAL);
        if( send_len == SOCKET_ERROR ) {
            if( errno == EAGAIN ) continue;
            if( errno == EWOULDBLOCK ) continue;
            ES_ERROR("send failed");
            return(false);
        }
        if( send_len == 0 ) {
            ES_ERROR("send_len is 0");
            return(false);
        }
        wrote_len += send_len;
    }

    bool result = wrote_len == length;
    if( result == false ) {
        ES_ERROR("transaction was not completed");
        return(false);
    }

    return(result);
}

//---------------------------------------------------------------------------

bool CFCGIRequest::WritePaddingData(unsigned int length)
{
    unsigned char buffer[10];
    unsigned int remaining = length;

    memset(buffer,0,10);

    while(remaining > 0) {
        if( remaining > 10 ) {
            if( WriteData(buffer,10) == false ) {
                ES_ERROR("unable to write padding data");
                return(false);
            }
            remaining -= 10;
        } else {
            if( WriteData(buffer,remaining) == false ) {
                ES_ERROR("unable to write padding data");
                return(false);
            }
            remaining = 0;
        }
    }
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
