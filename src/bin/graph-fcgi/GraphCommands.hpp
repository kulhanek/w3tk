#ifndef GraphCommandsH
#define GraphCommandsH
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

#include <FCGIParams.hpp>
#include <SmallString.hpp>
#include <SimpleList.hpp>

//------------------------------------------------------------------------------

class CGraphCommand {
public:
    CSmallString    Key;
    CSmallString    Value;
};

//------------------------------------------------------------------------------

enum EGraphType {
    EGT_UNKNOWN,
    EGT_PIE_CHART
};

//------------------------------------------------------------------------------

class CGraphCommands : public CSimpleList<CGraphCommand> {
public:
    //! parse QUERY_STRING to command list
    bool ParseCommands(CFCGIParams& params);

    //! get graph type
    EGraphType GetGraphType(void);

// section of private data ----------------------------------------------------
private:
    bool AddCommandItem(CSmallString string);
    bool AddCommandItem(const CSmallString& key, const CSmallString& value);
};

//------------------------------------------------------------------------------

#endif
