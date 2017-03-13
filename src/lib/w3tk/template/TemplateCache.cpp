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
#include <TemplateCache.hpp>
#include <Template.hpp>
#include <ErrorSystem.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTemplateCache TemplateCache;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTemplateCache::CTemplateCache(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void  CTemplateCache::SetTemplatePath(const CFileName& path)
{
    TemplatePath = path;
}

//------------------------------------------------------------------------------

const CFileName& CTemplateCache::GetTemplatePath(void)
{
    return(TemplatePath);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTemplate*  CTemplateCache::OpenTemplate(const CSmallString& name)
{
    CSimpleIterator<CTemplate> D(Templates);
    CTemplate*                 p_db;

    // find pre-cached templates --------------------------
    while((p_db = D.Current())!=NULL) {
        if( p_db->GetTemplateName() == name ) {
            return(p_db);
        }
        D++;
    }

    // not found - try to open it -------------------------
    p_db = new CTemplate(this);
    if( p_db == NULL ) {
       ES_ERROR("unable to create empty template");
        return(NULL);
    }

    // open file
    if( p_db->OpenTemplate(name) == false ) {
        delete p_db;
       ES_ERROR("unable to open template");
        return(NULL);
    }

    Templates.InsertToEnd(p_db,0,true);

    return(p_db);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


