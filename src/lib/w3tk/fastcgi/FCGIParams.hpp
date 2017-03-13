#ifndef FCGIParamsH
#define FCGIParamsH
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
#include <FCGIStream.hpp>

//------------------------------------------------------------------------------

class W3TK_PACKAGE CFCGIParams {
public:
// constructor and destructor -------------------------------------------------
    CFCGIParams(void);
    ~CFCGIParams(void);

// access methods -------------------------------------------------------------
    //! get the value associated with the key
    const CSmallString& GetValue(const CSmallString& key) const;

    //! is key set?
    bool HasKey(const CSmallString& key) const;

    //! print parameters as KEY=VALUE list
    void PrintParams(FILE* fout=NULL);

    //! print parameters as HTML table
    void PrintParams(CFCGIStream& stream);

// list population ------------------------------------------------------------
    //! set key-value pair in the list of parameters
    bool SetParam(const CSmallString& key, const CSmallString& value);

    //! remove all params from the list
    void ClearAllParams(void);

    //! load params from the FCGI stream
    bool LoadParamsFromStream(CFCGIStream& stream);

    //! load params from the query_string
    bool LoadParamsFromQuery(const CSmallString& query_string,bool do_not_overwrite=true);

    //! load params from the QUERY_STRING - inplace
    bool LoadParamsFromQuery(bool do_not_overwrite=true);

    //! load params from the post - stream is inputstream
    bool LoadParamsFromPost(CFCGIStream& stream,bool do_not_overwrite=true);

    //! transform string query string
    static CSmallString DecodeString(const CSmallString& string);

    //! transform string query string
    static CSmallString EncodeString(const CSmallString& string);

// section of private data ----------------------------------------------------
private:
    // internal structure, which holds key and value couple
    class CParam {
    public:
        CSmallString    Key;
        CSmallString    Value;
    };
    CSimpleList<CParam> Params; // list of params

    //! add query item
    bool AddQueryItem(CSmallString string,bool do_not_overwrite=true);
};

//------------------------------------------------------------------------------

#endif
