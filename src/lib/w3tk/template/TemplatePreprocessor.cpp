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
#include <ctype.h>
#include <string.h>
#include <TemplatePreprocessor.hpp>
#include <Template.hpp>
#include <TemplateParams.hpp>
#include <TemplateParamNode.hpp>
#include <ErrorSystem.hpp>
#include <XMLComment.hpp>
#include <XMLElement.hpp>
#include <XMLText.hpp>
#include <XMLAttribute.hpp>
#include <XMLUnknown.hpp>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTemplatePreprocessor::CTemplatePreprocessor(void)
{
    Template = NULL;
    Output = NULL;
    TemplateParams = NULL;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTemplatePreprocessor::SetInputTemplate(CTemplate* p_tmp)
{
    Template = p_tmp;
}

//------------------------------------------------------------------------------

void CTemplatePreprocessor::SetOutputDocument(CXMLDocument* p_out)
{
    Output = p_out;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTemplatePreprocessor::PreprocessTemplate(CTemplateParams* p_params)
{
    if( Template == NULL ) {
       ES_ERROR("Template is NULL");
        return(false);
    }
    if( Output == NULL ) {
       ES_ERROR("Output is NULL");
        return(false);
    }

    // clear entire output document
    Output->RemoveAllChildNodes();

    // create copy of template
    Output->CopyChildNodesFrom(Template);

    // quit if no parameters
    if( p_params == NULL ) {
        return(true);
    }

    TemplateParams = p_params;

    if( TemplateParams->IsFinalized() == false ) {
       ES_ERROR("parameters are not finalized");
        return(false);
    }

    // -----------------------------------------------------------------------

    if( ExpandBlocks(Output,TemplateParams->GetMainNode()) == false ) {
       ES_ERROR("unable to expand nodes");
        return(false);
    }

    if( PreprocessNode(Output) == false ) {
       ES_ERROR("unable to process nodes");
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTemplatePreprocessor::ExpandBlocks(CXMLNode* p_node,
        CTemplateParamNode* p_parnode)
{
    if( TemplateParams == NULL ) return(false);
    if( p_node == NULL ) {
       ES_ERROR("p_node is NULL");
        return(false);
    }
    if( p_parnode == NULL ) {
       ES_ERROR("p_first is NULL");
        return(false);
    }

    p_node->SetUserData(p_parnode);

    CXMLNode* p_sele = p_node->GetFirstChildNode();
    if( p_sele != NULL ) return(ExpandChildBlocks(p_sele,NULL,p_parnode));

    return(true);
}

//------------------------------------------------------------------------------

bool CTemplatePreprocessor::ExpandChildBlocks(CXMLNode* p_first,
        CXMLNode* p_stop,
        CTemplateParamNode* p_parnode)
{
    if( TemplateParams == NULL ) return(false);
    if( p_first == NULL ) {
       ES_ERROR("p_first is NULL");
        return(false);
    }
    if( p_parnode == NULL ) {
       ES_ERROR("p_first is NULL");
        return(false);
    }

    CXMLNode* p_sele = p_first;

    while( p_sele != p_stop ) {
        // is this node comment?
        if( p_sele->GetNodeType() == EXNT_COMMENT ) {
            CXMLComment* p_comment = dynamic_cast<CXMLComment*>(p_sele);
            // is it cycle?
            if( strstr(p_comment->GetComment(),"DO CYCLE") == p_comment->GetComment() ) {
                if( ExpandCycle(p_comment,p_sele,p_parnode) == false ) {
                   ES_ERROR("unable to expand cycle");
                    return(false);
                }
            } else if( strstr(p_comment->GetComment(),"IF") == p_comment->GetComment() ) {
                if( ExpandCondition(p_comment,p_sele,p_parnode) == false ) {
                   ES_ERROR("unable to expand condition");
                    return(false);
                }
            } else {
                if( strstr(p_comment->GetComment(),"INCLUDE") == p_comment->GetComment() ) {
                    if( ExpandInclude(p_comment,p_sele,p_parnode) == false ) {
                       ES_ERROR("unable to expand include block");
                        return(false);
                    }
                } else {
                    p_sele->SetUserData(p_parnode);
                    p_sele = p_sele->GetNextSiblingNode();
                }
            }
        } else {
            if( ExpandBlocks(p_sele,p_parnode) == false ) return(false);
            p_sele = p_sele->GetNextSiblingNode();
        }
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CTemplatePreprocessor::ExpandCycle(CXMLComment* p_start,
        CXMLNode*   &p_next,
        CTemplateParamNode* p_parnode)
{
    if( p_start == NULL ) {
        p_next = NULL;
        return(false);
    }

    CXMLComment* p_stop = NULL;

    CSmallString cycle_name;
    cycle_name = p_start->GetComment().GetSubStringFromTo(9,p_start->GetComment().GetLength()-1);

    if( cycle_name == NULL ) {
       ES_ERROR("name of cycle is not know");
        p_next = NULL;
        return(false);
    }

    // find the end of the cycle
    CXMLNode* p_cycele;
    CXMLNode* p_pointer=p_start;

    p_cycele = p_start->GetNextSiblingNode();
    while( p_cycele != NULL ) {
        if( p_cycele->GetNodeType() == EXNT_COMMENT ) {
            p_stop = dynamic_cast<CXMLComment*>(p_cycele);
            if( strstr(p_stop->GetComment(),"END CYCLE") == p_stop->GetComment() ) {

                CSmallString stop_cycle_name;
                stop_cycle_name = p_stop->GetComment().GetSubStringFromTo(10,p_stop->GetComment().GetLength()-1);

                if( stop_cycle_name == cycle_name ) break;
            }
        }
        p_stop = NULL;
        p_pointer = p_cycele;
        p_cycele = p_cycele->GetNextSiblingNode();
    }

    if( p_stop == NULL ) {
        CSmallString error;
        error << "end of cycle '" << cycle_name << "' was not found";
       ES_ERROR(error);
        p_next = NULL;
        return(false);
    }

    CTemplateParamNode* p_start_cycpar = p_parnode->FindCycle(cycle_name);
    CTemplateParamNode* p_cycpar = p_start_cycpar;

    p_start->SetUserData(p_start_cycpar);
    p_stop->SetUserData(p_start_cycpar);

    if( p_cycpar == NULL ) {
        // cycle is not registered - delete nodes
        p_cycele = p_start->GetNextSiblingNode();
        while( p_cycele != p_stop ) {
            CXMLNode* p_delnode = p_cycele;
            p_cycele = p_cycele->GetNextSiblingNode();
            delete p_delnode;
        }

        // move to next node
        p_next = p_stop->GetNextSiblingNode();
        return(true);
    }

    // CXMLNode* p_copy_stop = p_pointer;
    p_pointer = p_stop;

    // skip first item
    p_cycpar = p_cycpar->NextInCycle;

    // duplicate nodes
    while( p_cycpar != NULL ) {
        CXMLNode* p_cfirst = NULL;
        CXMLNode* p_cstop = NULL;

        p_cycele = p_start;

        bool do_copy = true;

        while( do_copy ) {

            if( p_cycele == p_stop ) do_copy = false;

            switch(p_cycele->GetNodeType()) {
            case EXNT_COMMENT:
                p_pointer = p_pointer->CreateSiblingComment();
                break;
            case EXNT_ELEMENT:
                p_pointer = p_pointer->CreateSiblingElement("");
                break;
            case EXNT_TEXT:
                p_pointer = p_pointer->CreateSiblingText(true);
                break;
            default:
               ES_ERROR("unsupported node type");
                p_next = NULL;
                return(false);
            }
            if( p_pointer == NULL ) {
               ES_ERROR("unable to create sibling node");
                p_next = NULL;
                return(false);
            }
            p_pointer->CopyContentsFrom(p_cycele);

            // expand node - except start and stop
            if( p_cycele == p_start ) {
                p_cfirst = p_pointer;
                p_pointer->SetUserData(p_cycpar);
            }

            // expand node - except start and stop
            if( p_cycele == p_stop ) {
                p_cstop = p_pointer;
                p_pointer->SetUserData(p_cycpar);
            }

            p_cycele = p_cycele->GetNextSiblingNode();
        }

        if( p_cfirst != NULL ) p_cfirst = p_cfirst->GetNextSiblingNode(); // skip cycle ID
        if( p_cfirst != NULL ) {
            if( ExpandChildBlocks(p_cfirst,p_cstop,p_cycpar) == false ) {
               ES_ERROR("unable to expand child elements");
                p_next = NULL;
                return(false);
            }
        }

        p_cycpar = p_cycpar->NextInCycle;
    }

    // expand first items
    if( p_start != NULL ) {
        CXMLNode* p_cfirst = p_start;
        CXMLNode* p_cstop = p_stop;
        if( p_cfirst != NULL ) p_cfirst = p_cfirst->GetNextSiblingNode(); // skip cycle ID
        if( p_cfirst != NULL ) {
            if( ExpandChildBlocks(p_cfirst,p_cstop,p_start_cycpar) == false ) {
               ES_ERROR("unable to expand child elements");
                p_next = NULL;
                return(false);
            }
        }
    }

    // set next element for processing
    p_next = p_pointer->GetNextSiblingNode();

    return(true);
}

//------------------------------------------------------------------------------

bool CTemplatePreprocessor::ExpandCondition(CXMLComment* p_start,
        CXMLNode*   &p_next,
        CTemplateParamNode* p_parnode)
{
    if( p_start == NULL ) {
        p_next = NULL;
        return(false);
    }

    CXMLComment* p_else = NULL;
    CXMLComment* p_stop = NULL;

    CSmallString cond_name;
    cond_name = p_start->GetComment().GetSubStringFromTo(3,p_start->GetComment().GetLength()-1);

    if( cond_name == NULL ) {
       ES_ERROR("name of condition is not know");
        p_next = NULL;
        return(false);
    }

    // find the else of the condition
    CXMLNode* p_cycele;
    //CXMLNode* p_pointer=p_start;

    p_cycele = p_start->GetNextSiblingNode();
    while( p_cycele != NULL ) {
        if( p_cycele->GetNodeType() == EXNT_COMMENT ) {
            p_else = dynamic_cast<CXMLComment*>(p_cycele);
            if( strstr(p_else->GetComment(),"ELSE") == p_else->GetComment() ) {

                CSmallString else_cond_name;
                else_cond_name = p_else->GetComment().GetSubStringFromTo(5,p_else->GetComment().GetLength()-1);
                if( else_cond_name == cond_name ) break;
            }
        }
        p_else = NULL;
       // p_pointer = p_cycele;
        p_cycele = p_cycele->GetNextSiblingNode();
    }


    // find the end of the condition
   // p_pointer=p_start;

    p_cycele = p_start->GetNextSiblingNode();
    while( p_cycele != NULL ) {
        if( p_cycele->GetNodeType() == EXNT_COMMENT ) {
            p_stop = dynamic_cast<CXMLComment*>(p_cycele);
            if( strstr(p_stop->GetComment(),"END IF") == p_stop->GetComment() ) {

                CSmallString stop_cond_name;
                stop_cond_name = p_stop->GetComment().GetSubStringFromTo(7,p_stop->GetComment().GetLength()-1);

                if( stop_cond_name == cond_name ) break;
            }
        }
        p_stop = NULL;
     //   p_pointer = p_cycele;
        p_cycele = p_cycele->GetNextSiblingNode();
    }

    if( p_stop == NULL ) {
        CSmallString error;
        error << "end of condition '" << cond_name << "' was not found";
       ES_ERROR(error);
        p_next = NULL;
        return(false);
    }

    // find condition
    CTemplateParamNode* p_condpar = p_parnode->FindCondition(cond_name);

    bool condition = (p_condpar != NULL) && p_condpar->GetConditionValue();

    if( p_condpar == NULL ) p_condpar = p_parnode; // it is required for proper expansion

    p_start->SetUserData(p_condpar);
    if( p_else != NULL ) p_else->SetUserData(p_condpar);
    p_stop->SetUserData(p_condpar);

    if( p_else == NULL ) {
        if( condition ) {
            CXMLNode* p_cfirst = p_start;
            CXMLNode* p_cstop = p_stop;
            if( p_cfirst != NULL ) p_cfirst = p_cfirst->GetNextSiblingNode(); // skip cycle ID
            if( p_cfirst != NULL ) {
                if( ExpandChildBlocks(p_cfirst,p_cstop,p_condpar) == false ) {
                   ES_ERROR("unable to expand child elements");
                    p_next = NULL;
                    return(false);
                }
            }
        } else {
            // remove condition part - delete nodes
            p_cycele = p_start->GetNextSiblingNode();
            while( p_cycele != p_stop ) {
                CXMLNode* p_delnode = p_cycele;
                p_cycele = p_cycele->GetNextSiblingNode();
                delete p_delnode;
            }
        }
    } else {
        if( condition ) {
            // remove else part - delete nodes
            p_cycele = p_else->GetNextSiblingNode();
            while( p_cycele != p_stop ) {
                CXMLNode* p_delnode = p_cycele;
                p_cycele = p_cycele->GetNextSiblingNode();
                delete p_delnode;
            }
            // expand if part
            p_cycele = p_start;

            CXMLNode* p_cfirst = p_start;
            CXMLNode* p_cstop = p_else;
            if( p_cfirst != NULL ) p_cfirst = p_cfirst->GetNextSiblingNode(); // skip cycle ID
            if( p_cfirst != NULL ) {
                if( ExpandChildBlocks(p_cfirst,p_cstop,p_condpar) == false ) {
                   ES_ERROR("unable to expand child elements");
                    p_next = NULL;
                    return(false);
                }
            }
        } else {
            // remove if part - delete nodes
            p_cycele = p_start->GetNextSiblingNode();
            while( p_cycele != p_else ) {
                CXMLNode* p_delnode = p_cycele;
                p_cycele = p_cycele->GetNextSiblingNode();
                delete p_delnode;
            }
            // expand else part
            CXMLNode* p_cfirst = p_else;
            CXMLNode* p_cstop = p_stop;
            if( p_cfirst != NULL ) p_cfirst = p_cfirst->GetNextSiblingNode(); // skip cycle ID
            if( p_cfirst != NULL ) {
                if( ExpandChildBlocks(p_cfirst,p_cstop,p_condpar) == false ) {
                   ES_ERROR("unable to expand child elements");
                    p_next = NULL;
                    return(false);
                }
            }
        }
    }

    // move to next node
    p_next = p_stop->GetNextSiblingNode();
    return(true);
}

//------------------------------------------------------------------------------

bool CTemplatePreprocessor::ExpandInclude(CXMLComment* p_start,
        CXMLNode*   &p_next,
        CTemplateParamNode* p_parnode)
{
    if( p_start == NULL ) {
        p_next = NULL;
        return(false);
    }

    CSmallString inc_name;
    inc_name = p_start->GetComment().GetSubStringFromTo(8,p_start->GetComment().GetLength()-1);

    if( inc_name == NULL ) {
       ES_ERROR("name of include block is not know");
        p_next = NULL;
        return(false);
    }

    // set paramters for tag
    p_start->SetUserData(p_parnode);

    // find condition
    CTemplateParamNode* p_includepar = p_parnode->FindInclude(inc_name);
    if( p_includepar == NULL ) {
        // move to next node
        p_next = p_start->GetNextSiblingNode();
        return(true);
    }

    CXMLNode* p_incnode = p_includepar->GetIncludeNode();
    if( p_incnode == NULL ) {
        // move to next node
        p_next = p_start->GetNextSiblingNode();
        return(true);
    }

    // inject nodes
    CXMLNode* p_cycele;
    CXMLNode* p_pointer;

    p_cycele = p_incnode->GetFirstChildNode();
    p_pointer = p_start;

    while( p_cycele != NULL ) {
        switch(p_cycele->GetNodeType()) {
        case EXNT_COMMENT:
            p_pointer = p_pointer->CreateSiblingComment();
            break;
        case EXNT_ELEMENT:
            p_pointer = p_pointer->CreateSiblingElement("");
            break;
        case EXNT_TEXT:
            p_pointer = p_pointer->CreateSiblingText(true);
            break;
        default:
           ES_ERROR("unsupported node type");
            p_next = NULL;
            return(false);
        }
        if( p_pointer == NULL ) {
           ES_ERROR("unable to create sibling node");
            p_next = NULL; // move to next node
            return(false);
        }
        p_pointer->CopyContentsFrom(p_cycele);

        // expand node
        if( ExpandBlocks(p_pointer,p_includepar) == false ) {
           ES_ERROR("unable to expand cloned block");
            p_next = NULL;
            return(false);
        }
        p_cycele = p_cycele->GetNextSiblingNode();
    }

    // move to next node
    p_next = p_start->GetNextSiblingNode();
    return(true);
}

//------------------------------------------------------------------------------

bool CTemplatePreprocessor::PreprocessNode(CXMLNode* p_node)
{
    if( TemplateParams == NULL ) return(false);
    if( p_node == NULL ) return(false);

    CTemplateParamNode* p_parnode = (CTemplateParamNode*)p_node->GetUserData();
    if( p_parnode == NULL ) return(true);

    // if this is element node process its attributes ------------
    if( p_node->GetNodeType() == EXNT_ELEMENT ) {
        CXMLElement* p_ele = dynamic_cast<CXMLElement*>(p_node);

        // process attributes
        CXMLAttribute* p_attr = p_ele->GetFirstAttribute();

        while( p_attr != NULL ) {
            CSmallString    value,newvalue;
            bool            changed;
            value = p_attr->Value;
            if( PreprocessValue(p_parnode,value,changed,newvalue) == false ) return(false);
            if( changed == true ) {
                p_attr->Value = newvalue;
            }
            p_attr = p_attr->GetNextSiblingAttribute();
        }
    } else if( p_node->GetNodeType() == EXNT_COMMENT ) {
        CXMLComment* p_ele = dynamic_cast<CXMLComment*>(p_node);
        CSmallString    value,newvalue;
        bool            changed;
        value = p_ele->GetComment();
        if( PreprocessValue(p_parnode,value,changed,newvalue) == false ) return(false);
        if( changed == true ) {
            p_ele->SetComment(newvalue);
        }
    } else if( p_node->GetNodeType() == EXNT_TEXT ) {
        CXMLText* p_ele = dynamic_cast<CXMLText*>(p_node);
        CSmallString    value,newvalue;
        bool            changed;
        value = p_ele->GetText();
        if( PreprocessValue(p_parnode,value,changed,newvalue) == false ) return(false);
        if( changed == true ) {
            p_ele->SetText(newvalue);
        }
    }

    // any childs ?
    if( p_node->HasChildNodes() == false ) return(true);

    // process childs ---------
    CXMLNode* p_sele = p_node->GetFirstChildNode();

    while( p_sele != NULL ) {
        if( PreprocessNode(p_sele) == false ) return(false);
        p_sele = p_sele->GetNextSiblingNode();
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CTemplatePreprocessor::PreprocessValue(CTemplateParamNode* p_parnode,
        const CSmallString& value,
        bool& changed,CSmallString& newvalue)
{
    if( TemplateParams == NULL ) return(false);
    if( p_parnode == NULL ) return(false);

    changed = false;

    int start = 0;
    int end = -1;
    int gstart = 0;
    int gend = -1;

    unsigned int pos = 0;

    while( pos < value.GetLength() ) {
        for(; pos < value.GetLength(); pos++) {
            // try to find beginning
            if( value.GetBuffer()[pos] == '_' ) break;
        }
        if( pos >= value.GetLength() ) break; // end of string
        pos++;
        start = pos;
        for(; pos < value.GetLength(); pos++) {
            // try to find end
            if( isalpha(value.GetBuffer()[pos]) == 0 ) break;
        }
        end = pos - 1;
        if( end < start ) continue;

        // cut template name
        CSmallString tvariable;
        tvariable = value.GetSubString(start,end-start+1);

        // try to find name
        CSmallString tnewvalue;
        if( TemplateParams->FindValue(p_parnode,tvariable,tnewvalue) == true ) {
            gend = start-2;   // underscore is filtered out
            changed = true;
            if( gend >= gstart ) {
                // add previous gap
                newvalue += value.GetSubString(gstart,gend-gstart+1);
            }
            newvalue += tnewvalue;
            gstart = end + 1;
        }
    }

    if( changed == true ) {
        gend = value.GetLength() - 1;
        if( gend >= gstart ) {
            // add previous gap
            newvalue += value.GetSubString(gstart,gend-gstart+1);
        }
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================



