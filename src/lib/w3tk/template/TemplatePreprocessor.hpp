#ifndef TemplatePreprocessorH
#define TemplatePreprocessorH
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
#include <XMLDocument.hpp>

//------------------------------------------------------------------------------

class CTemplate;
class CTemplateParams;
class CTemplateParamNode;

//------------------------------------------------------------------------------

class W3TK_PACKAGE CTemplatePreprocessor {
public:
// constructor and destructor -------------------------------------------------
    CTemplatePreprocessor(void);

// setup methods --------------------------------------------------------------
    //! set input template
    void SetInputTemplate(CTemplate* p_tmp);

    //! set output document
    void SetOutputDocument(CXMLDocument* p_out);

// main execution method ------------------------------------------------------
    //! process template with parameters
    bool PreprocessTemplate(CTemplateParams* p_params);

// section of private data ----------------------------------------------------
private:
    CTemplate*          Template;
    CXMLDocument*       Output;
    CTemplateParams*    TemplateParams;

    bool ExpandBlocks(CXMLNode* p_node,CTemplateParamNode* p_parnode);
    bool ExpandChildBlocks(CXMLNode* p_first,
                                      CXMLNode* p_stop,
                                      CTemplateParamNode* p_parnode);
    bool PreprocessNode(CXMLNode* p_node);
    bool PreprocessValue(CTemplateParamNode* p_parnode,
                                    const CSmallString& value,
                                    bool& changed,CSmallString& newvalue);
    bool ExpandCycle(CXMLComment* p_start,
                                CXMLNode*   &p_next,
                                CTemplateParamNode* p_parnode);
    bool ExpandCondition(CXMLComment* p_start,
                                    CXMLNode*   &p_next,
                                    CTemplateParamNode* p_parnode);
    bool ExpandInclude(CXMLComment* p_start,
                                  CXMLNode*   &p_next,
                                  CTemplateParamNode* p_parnode);
};

//---------------------------------------------------------------------------
#endif
