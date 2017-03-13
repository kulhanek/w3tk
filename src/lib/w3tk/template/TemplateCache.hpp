#ifndef TemplateCacheH
#define TemplateCacheH
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
#include <FileName.hpp>
#include <SimpleList.hpp>

//------------------------------------------------------------------------------

class CTemplate;

//------------------------------------------------------------------------------

class W3TK_PACKAGE CTemplateCache {
public:
// constructor and destructor -------------------------------------------------
    CTemplateCache(void);

// setup template cache ---------------------------------
    void             SetTemplatePath(const CFileName& path);
    const CFileName& GetTemplatePath(void);

// actions ----------------------------------------------
    CTemplate*  OpenTemplate(const CSmallString& name);
    bool        PurgeCache(void);

// section of private data ----------------------------------------------------
private:
    CFileName               TemplatePath;
    CSimpleList<CTemplate>  Templates;
};

//------------------------------------------------------------------------------

extern CTemplateCache TemplateCache;

//------------------------------------------------------------------------------
#endif
