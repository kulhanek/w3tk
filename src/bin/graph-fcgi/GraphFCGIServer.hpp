#ifndef GraphFCGIServerH
#define GraphFCGIServerH
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
#include "GraphFCGIOptions.h"

//------------------------------------------------------------------------------

class CGraphFCGIServer : public CFCGIServer {
public:
    CGraphFCGIServer(void);

// main methods ---------------------------------------------------------------
    //! init options
    int Init(int argc,char* argv[]);

    //! main part of program
    bool Run(void);

    //! finalize program
    bool Finalize(void);

// section of private data ----------------------------------------------------
private:
    CGraphFCGIOptions   Options;

    static  void CtrlCSignalHandler(int signal);
    virtual bool __w3tkspec AcceptRequest(void);
};

//------------------------------------------------------------------------------

extern CGraphFCGIServer GraphFCGIServer;

//------------------------------------------------------------------------------

#endif
