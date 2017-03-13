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

#include "PieChart.h"
#include <SimpleIterator.hpp>
#include <string.h>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CGraphCommands::ParseCommands(CFCGIParams& params)
{
    CSmallString query_string;

    query_string = params.GetValue("QUERY_STRING");
    if( query_string == "" ) return(true);     // empty string

    char* p_pointer = NULL;
    char* p_item;

    p_item = strtok_r(query_string.GetBuffer(),",;",&p_pointer);
    while( p_item != NULL ) {
        // parse item
        if( AddCommandItem(params.TransformQueryString(p_item)) == false ) return(false);

        // next item
        p_item = strtok_r(NULL,",",&p_pointer);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CGraphCommands::AddCommandItem(CSmallString string)
{
    int len = string.GetLength();
    char* p_item = string.GetBuffer();
    int eq = 0;

    do {
        if( p_item[eq] == '=' ) break;
        eq++;
    } while( eq < len );

    if( eq < len ) p_item[eq]='\0';

    if( eq+1 < len ) {
        return(AddCommandItem(p_item,&p_item[eq+1]));
    } else {
        return(AddCommandItem(p_item,""));
    }
}

//------------------------------------------------------------------------------

bool CGraphCommands::AddCommandItem(const CSmallString& key, const CSmallString& value)
{
    CGraphCommand* p_param;

    try {
        p_param = new CGraphCommand;
    } catch(...) {
        return(false);
    }
    p_param->Key = key;
    p_param->Value = value;

    if( InsertToEnd(p_param,0,true) == false ) {
        delete p_param;
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

EGraphType CGraphCommands::GetGraphType(void)
{
    CSimpleIteratorC<CGraphCommand> I(this);
    CGraphCommand*                 p_param;

    while((p_param = I.Current()) != NULL) {
        if( p_param->Key == "type" ) {
            if( p_param->Value == "pie" ) return(EGT_PIE_CHART);
            return(EGT_UNKNOWN);
        }
        I++;
    }

    return(EGT_UNKNOWN);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

