#ifndef TemplateParamNodeH
#define TemplateParamNodeH
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
#include <TemplateParamItem.hpp>
#include <XMLNode.hpp>

//------------------------------------------------------------------------------

enum EParamNodeType {
    EPNT_REGULAR,
    EPNT_CYCLE,
    EPNT_CONDITION,
    EPNT_INCLUDE
};

//------------------------------------------------------------------------------

class W3TK_PACKAGE CTemplateParamNode {
public:
// constructor and destructor -------------------------------------------------
    CTemplateParamNode(CTemplateParamNode* p_owner);
    ~CTemplateParamNode(void);

// information methods --------------------------------------------------------
    bool                FindValue(const CSmallString& variable,
            CSmallString& value);
    // find sublocks
    CTemplateParamNode* FindCycle(const CSmallString& name);
    CTemplateParamNode* FindCondition(const CSmallString& name);
    CTemplateParamNode* FindInclude(const CSmallString& name);

    // get values
    bool                GetConditionValue(void);
    CXMLNode*           GetIncludeNode(void);

// section of private data ----------------------------------------------------
private:
    EParamNodeType                  NodeType;
    CTemplateParamNode*             OwnerNode;
    CSimpleList<CTemplateParamNode> SubNodes;
    CSimpleList<CTemplateParamNode> OtherSubNodes;
    CTemplateParamNode*             NextInCycle;
    CTemplateParamNode*             PrevInCycle;

    CSmallString                    Name;
    int                             Index;
    bool                            Condition;
    CXMLNode*                       IncludeNode;
    CSimpleList<CTemplateParamItem> ParamItems;

    friend class CTemplateParams;
    friend class CTemplatePreprocessor;
};

//---------------------------------------------------------------------------

#endif
