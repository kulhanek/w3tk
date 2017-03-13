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
#include <TemplateParamNode.hpp>
#include <SimpleIterator.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTemplateParamNode::CTemplateParamNode(CTemplateParamNode* p_owner)
{
    OwnerNode = p_owner;
    NextInCycle = NULL;
    PrevInCycle = NULL;
    Index = 0;
    NodeType = EPNT_REGULAR;
    Condition = false;
    IncludeNode = NULL;
}

//------------------------------------------------------------------------------

CTemplateParamNode::~CTemplateParamNode(void)
{
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTemplateParamNode::FindValue(const CSmallString& variable,CSmallString& value)
{
    CSimpleIterator<CTemplateParamItem>    I(ParamItems);
    CTemplateParamItem* p_item;

    while( (p_item = I.Current()) != NULL ) {
        if( p_item->Variable == variable ) {
            value = p_item->Value;
            return(true);
        }
        I++;
    }

    return(false);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTemplateParamNode* CTemplateParamNode::FindCycle(const CSmallString& name)
{
    CSimpleIterator<CTemplateParamNode>    I(SubNodes);
    CTemplateParamNode* p_item;

    while( (p_item = I.Current()) != NULL ) {
        if( (p_item->Name == name) && (p_item->NodeType == EPNT_CYCLE) ) {
            return(p_item);
        }
        I++;
    }

    return(NULL);
}

//------------------------------------------------------------------------------

CTemplateParamNode* CTemplateParamNode::FindCondition(const CSmallString& name)
{
    CSimpleIterator<CTemplateParamNode>    I(SubNodes);
    CTemplateParamNode* p_item;

    while( (p_item = I.Current()) != NULL ) {
        if( (p_item->Name == name) && (p_item->NodeType == EPNT_CONDITION) ) {
            return(p_item);
        }
        I++;
    }

    return(NULL);
}

//------------------------------------------------------------------------------

CTemplateParamNode* CTemplateParamNode::FindInclude(const CSmallString& name)
{
    CSimpleIterator<CTemplateParamNode>    I(SubNodes);
    CTemplateParamNode* p_item;

    while( (p_item = I.Current()) != NULL ) {
        if( (p_item->Name == name) && (p_item->NodeType == EPNT_INCLUDE) ) {
            return(p_item);
        }
        I++;
    }

    return(NULL);
}

//------------------------------------------------------------------------------

bool CTemplateParamNode::GetConditionValue(void)
{
    return(Condition);
}

//------------------------------------------------------------------------------

CXMLNode* CTemplateParamNode::GetIncludeNode(void)
{
    return(IncludeNode);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


