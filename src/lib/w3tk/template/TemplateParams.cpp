/*******************************************************************************
Copyright 2001,2002,2003,2004,2007 Petr Kulhanek

    This file is part of KERBEROS program.

    KERBEROS program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    KERBEROS program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with KERBEROS program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/

#include <stdio.h>
#include <TemplateParams.hpp>
#include <TemplateParamNode.hpp>
#include <ErrorSystem.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTemplateParams::CTemplateParams(void)
{
    MainNode = NULL;
    CurrentNode = NULL;
    ErrorState = false;
    Finalized = false;
}

//------------------------------------------------------------------------------

CTemplateParams::~CTemplateParams(void)
{
    if( MainNode != NULL ) {
        delete MainNode;
        MainNode = NULL;
        CurrentNode = NULL;
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTemplateParams::Initialize(void)
{
    if( MainNode != NULL ) {
        delete MainNode;
        MainNode = NULL;
        CurrentNode = NULL;
    }

    ErrorState = true;

    MainNode = new CTemplateParamNode(NULL);
    CurrentNode = MainNode;

    if( MainNode == NULL ) {
        ES_ERROR("unable to create new node");
        ErrorState = false;
    }

    Finalized = false;
    return( MainNode != NULL );
}

//------------------------------------------------------------------------------

bool CTemplateParams::Finalize(void)
{
    // check if all was closed
    if( CurrentNode->OwnerNode != NULL ) {
        ES_ERROR("current node is not main node");
        ErrorState = false;
    }
    Finalized = true;
    return(ErrorState);
}

//------------------------------------------------------------------------------

bool CTemplateParams::IsFinalized(void)
{
    return( Finalized && (CurrentNode != NULL) );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTemplateParams::StartCycle(const CSmallString& name)
{
    if( CurrentNode == NULL ) {
        ES_ERROR("current node is NULL");
        ErrorState = false;
        return(false);
    }

    // create new node
    CTemplateParamNode* p_new = new CTemplateParamNode(CurrentNode);

    if( p_new == NULL ) {
        ES_ERROR("unable to create new node");
        ErrorState = false;
        return(false);
    }
    p_new->Name = name;
    p_new->Index = 0;
    p_new->NodeType = EPNT_CYCLE;

    CurrentNode->SubNodes.InsertToEnd(p_new,0,true);
    CurrentNode = p_new;

    SetParam(CurrentNode->Name,CurrentNode->Index);

    return(true);
}

//------------------------------------------------------------------------------

bool CTemplateParams::NextRun(void)
{
    if( CurrentNode == NULL ) {
        ES_ERROR("current node is NULL");
        ErrorState = false;
        return(false);
    }

    if( CurrentNode->OwnerNode == NULL ) {
        ES_ERROR("this cycle cannot be extended - no master was found");
        ErrorState = false;
        return(false);
    }

    // create new node
    CTemplateParamNode* p_next = new CTemplateParamNode(CurrentNode->OwnerNode);

    if( p_next == NULL ) {
        ES_ERROR("unable to create new node");
        ErrorState = false;
        return(false);
    }

    CurrentNode->OwnerNode->OtherSubNodes.InsertToEnd(p_next,0,true);

    p_next->Name = CurrentNode->Name;
    p_next->Index = CurrentNode->Index + 1;
    p_next->NodeType = EPNT_CYCLE;

    p_next->PrevInCycle = CurrentNode;
    CurrentNode->NextInCycle = p_next;
    CurrentNode = p_next;

    SetParam(CurrentNode->Name,CurrentNode->Index);

    return(true);
}

//------------------------------------------------------------------------------

bool CTemplateParams::EndCycle(const CSmallString& name)
{
    if( CurrentNode == NULL ) {
        ES_ERROR("current node is NULL");
        ErrorState = false;
        return(false);
    }

    if( CurrentNode->Name != name ) {
        ES_ERROR("end cycle name does not match curent name");
        ErrorState = false;
        return(false);
    }

    //move up

    if( CurrentNode->OwnerNode == NULL ) {
        ES_ERROR("this cycle cannot be terminated - no master was found");
        ErrorState = false;
        return(false);
    }
    CTemplateParamNode * p_last = CurrentNode;
    CurrentNode = CurrentNode->OwnerNode;

    // remove last node
    if( p_last != NULL ) {
        if( p_last->PrevInCycle != NULL ) {
            p_last->PrevInCycle->NextInCycle = NULL;
            p_last->OwnerNode->OtherSubNodes.Remove(p_last);
        } else {
            if( p_last->OwnerNode != NULL ) {
                p_last->OwnerNode->SubNodes.Remove(p_last);
            }
        }
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTemplateParams::StartCondition(const CSmallString& name,bool condition)
{
    if( CurrentNode == NULL ) {
        ES_ERROR("current node is NULL");
        ErrorState = false;
        return(false);
    }

    // create new node
    CTemplateParamNode* p_new = new CTemplateParamNode(CurrentNode);

    if( p_new == NULL ) {
        ES_ERROR("unable to create new node");
        ErrorState = false;
        return(false);
    }
    p_new->Name = name;
    p_new->Index = 0;
    p_new->NodeType = EPNT_CONDITION;
    p_new->Condition = condition;

    CurrentNode->SubNodes.InsertToEnd(p_new,0,true);
    CurrentNode = p_new;

    SetParam(CurrentNode->Name,p_new->Condition);

    return(true);
}

//------------------------------------------------------------------------------

bool CTemplateParams::EndCondition(const CSmallString& name)
{
    if( CurrentNode == NULL ) {
        ES_ERROR("current node is NULL");
        ErrorState = false;
        return(false);
    }

    if( CurrentNode->Name != name ) {
        ES_ERROR("end condition name does not match curent name");
        ErrorState = false;
        return(false);
    }

    //move up

    if( CurrentNode->OwnerNode == NULL ) {
        ES_ERROR("this condition cannot be terminated - no master was found");
        ErrorState = false;
        return(false);
    }

    CurrentNode = CurrentNode->OwnerNode;

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTemplateParams::Include(const CSmallString& name,CXMLNode* p_node)
{
    if( CurrentNode == NULL ) {
        ES_ERROR("current node is NULL");
        ErrorState = false;
        return(false);
    }

    if( p_node == NULL ) {
        ES_ERROR("include node is NULL");
        ErrorState = false;
        return(false);
    }

    // create new node
    CTemplateParamNode* p_new = new CTemplateParamNode(CurrentNode);

    if( p_new == NULL ) {
        ES_ERROR("unable to create new node");
        ErrorState = false;
        return(false);
    }
    p_new->Name = name;
    p_new->Index = 0;
    p_new->NodeType = EPNT_INCLUDE;
    p_new->IncludeNode = p_node;

    CurrentNode->SubNodes.InsertToEnd(p_new,0,true);

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTemplateParams::SetParam(const CSmallString& variable,
        const CSmallString& value)
{
    if( CurrentNode == NULL ) {
        ErrorState = false;
        ES_ERROR("current node is NULL");
        return(false);
    }

    CSimpleIterator<CTemplateParamItem> D(CurrentNode->ParamItems);
    CTemplateParamItem* p_db;

    while((p_db = D.Current())!=NULL) {
        if( p_db->Variable == variable ) {
            p_db->Value = value;
            return(true);
        }
        D++;
    }

    p_db = new CTemplateParamItem;
    if( p_db == NULL ) {
        ErrorState = false;
        ES_ERROR("unable to create new param item");
        return(false);
    }
    p_db->Variable = variable;
    p_db->Value = value;

    CurrentNode->ParamItems.InsertToEnd(p_db,0,true);

    return(true);
}

//------------------------------------------------------------------------------

bool CTemplateParams::SetParam(const CSmallString& variable,int value)
{
    CSmallString svalue;
    svalue.IntToStr(value);
    return( SetParam(variable,svalue) );
}

//------------------------------------------------------------------------------

bool CTemplateParams::SetParam(const CSmallString& variable,const char* value)
{
    CSmallString svalue;
    svalue = value;
    return( SetParam(variable,svalue) );
}

//------------------------------------------------------------------------------

bool CTemplateParams::SetParam(const CSmallString& variable,bool value)
{
    CSmallString svalue;
    if( value == true ) {
        svalue = "true";
    } else {
        svalue = "false";
    }

    return( SetParam(variable,svalue) );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTemplateParams::FindValue(CTemplateParamNode* p_node,
        const CSmallString& variable,
        CSmallString& value)
{
    while( p_node != NULL ) {
        if( p_node->FindValue(variable,value) == true ) return(true);
        p_node = p_node->OwnerNode;
    }

    return(false);
}

//------------------------------------------------------------------------------

CTemplateParamNode* CTemplateParams::GetMainNode(void)
{
    return(MainNode);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



