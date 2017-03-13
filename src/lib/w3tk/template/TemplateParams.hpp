#ifndef TemplateParamsH
#define TemplateParamsH
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
#include <SimpleList.hpp>
#include <XMLNode.hpp>

//---------------------------------------------------------------------------

class CTemplateParamNode;

//------------------------------------------------------------------------------

class W3TK_PACKAGE CTemplateParams {
public:
// constructor and destructor -------------------------------------------------
    CTemplateParams(void);
    ~CTemplateParams(void);

// sections -------------------------------------------------------------------
    //! initialize paramters
    bool Initialize(void);
    //! finalize parameters
    bool Finalize(void);

    //! are parameters finalized?
    bool IsFinalized(void);

// cycles ---------------------------------------------------------------------
    //! open new cycle section
    bool StartCycle(const CSmallString& name);
    //! commence new cycle run
    bool NextRun(void);
    //! close cycle section
    bool EndCycle(const CSmallString& name);

// conditions -----------------------------------------------------------------
    //! execute condition section
    bool StartCondition(const CSmallString& name,bool condition);
    bool EndCondition(const CSmallString& name);

// code injecion --------------------------------------------------------------
    bool Include(const CSmallString& name,CXMLNode* p_node);

// parameters -----------------------------------------------------------------
    bool SetParam(const CSmallString& variable,const char* value);
    bool SetParam(const CSmallString& variable,const CSmallString& value);
    bool SetParam(const CSmallString& variable,int value);
    bool SetParam(const CSmallString& variable,bool value);

// helper methods for preprocessing -------------------------------------------
    //! return value of variable defined in the scope of p_node
    bool                    FindValue(CTemplateParamNode* p_node,
            const CSmallString& variable,
            CSmallString& value);

    //! return main parametr node
    CTemplateParamNode*     GetMainNode(void);

// section of private data ----------------------------------------------------
private:
    CTemplateParamNode* MainNode;
    CTemplateParamNode* CurrentNode;
    bool                ErrorState;
    bool                Finalized;
};

//---------------------------------------------------------------------------
#endif
