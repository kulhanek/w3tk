#ifndef FCGIStreamH
#define FCGIStreamH
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
#include <SmallString.hpp>

//------------------------------------------------------------------------------

class CFCGIRequest;

//------------------------------------------------------------------------------

class W3TK_PACKAGE CFCGIStream {
public:
// constructor and destructor -------------------------------------------------
    CFCGIStream(CFCGIRequest* p_request,int type);
    ~CFCGIStream(void);

// setup methods --------------------------------------------------------------
    //! return stream type
    int GetType(void) const;

    //! is it closed?
    bool IsItClosed(void) const;

// IO methods -----------------------------------------------------------------
    //! return one character from the stream or EOF in the case of error
    int GetChar(void);

    //! put character c to the stream
    /** it returns character c on success or EOF in the case of error */
    int PutChar(int c);

    //! get string of specified length from the stream
    /** it returns the number of read characters, if the number is smaller then
        length then end of file has been reached or error has been occured */
    int GetStr(char* str,int length);

    //! get string of specified length from the stream
    /** it returns the number of read characters, if the number is smaller then
        length then end of file has been reached or error has been occured */
    int GetStr(CSmallString& str,int length);

    //! put string of specified length to the stream
    /** it returns number of written character or EOF if en error occured */
    int PutStr(const char* str,int length=-1);

    //! put string of specified length to the stream
    /** it returns number of written character or EOF if en error occured */
    int PutStr(const CSmallString& str,int length=-1);

// section of private data ----------------------------------------------------
private:
    CFCGIRequest* Request;            // associated request
    int             Type;               // stream type
    bool            Closed;             // stream is closed
    char*           Buffer;             // IO buffer
    char*           Position;           // position in buffer
    char*           Stop;               // Buffer+Length
    int             BufferLength;       // buffer length

    //! close output stream
    bool Close(void);

    friend class CFCGIRequest;
};

//------------------------------------------------------------------------------

#endif
