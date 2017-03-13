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

#include <stdio.h>
#include <Template.hpp>
#include <TemplateCache.hpp>
#include <ErrorSystem.hpp>
#include <XMLParser.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


CTemplate::CTemplate(void)
{
    TemplateCache = NULL;
}

//------------------------------------------------------------------------------

CTemplate::CTemplate(CTemplateCache* p_owner)
{
    TemplateCache = p_owner;
}

//------------------------------------------------------------------------------

CTemplate::~CTemplate(void)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTemplate::OpenTemplate(const CSmallString& name)
{
    TemplateName = name;

    // complete name
    CSmallString tmp_name;

    if( TemplateCache ) {
        tmp_name = TemplateCache->GetTemplatePath() / TemplateName;
    } else {
        tmp_name = TemplateName;
    }

    CXMLParser xml_parser;

    xml_parser.SetOutputXMLNode(this);
    xml_parser.EnableWhiteCharacters(true);

    if( xml_parser.Parse(tmp_name) == false ) {
       ES_ERROR("unable to parse template");
    }

    return(true);
}

//------------------------------------------------------------------------------

const CSmallString& CTemplate::GetTemplateName(void)
{
    return(TemplateName);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



