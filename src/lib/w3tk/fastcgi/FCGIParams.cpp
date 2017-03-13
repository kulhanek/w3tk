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

#include <FCGIParams.hpp>
#include <SimpleIterator.hpp>
#include <ErrorSystem.hpp>
#include <string.h>
#include <sstream>

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CFCGIParams::CFCGIParams(void)
{

}

//------------------------------------------------------------------------------

CFCGIParams::~CFCGIParams(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const CSmallString& CFCGIParams::GetValue(const CSmallString& key) const
{
    static CSmallString void_string;

    CSimpleIteratorC<CParam> I(Params);
    CParam*                 p_param;

    while((p_param = I.Current()) != NULL) {
        if( p_param->Key == key ) return(p_param->Value);
        I++;
    }

    // not found return empty string
    return(void_string);
}

//------------------------------------------------------------------------------

bool CFCGIParams::HasKey(const CSmallString& key) const
{
    CSimpleIteratorC<CParam> I(Params);
    CParam*                 p_param;

    while((p_param = I.Current()) != NULL) {
        if( p_param->Key == key ) return(true);
        I++;
    }

    // not found
    return(false);
}

//------------------------------------------------------------------------------

void CFCGIParams::PrintParams(FILE* fout)
{
    if( fout == NULL ) fout = stdout;

    CSimpleIteratorC<CParam> I(Params);
    CParam*                 p_param;

    while((p_param = I.Current()) != NULL) {
        fprintf(fout,"%s=%s\n",(const char*)p_param->Key,(const char*)p_param->Value);
        I++;
    }
}

//------------------------------------------------------------------------------

void CFCGIParams::PrintParams(CFCGIStream& stream)
{
    CSimpleIteratorC<CParam> I(Params);
    CParam*                 p_param;

    stream.PutStr("<table>\n\r");

    while((p_param = I.Current()) != NULL) {
        stream.PutStr("<tr><td>");
        stream.PutStr(p_param->Key);
        stream.PutStr("</td><td>");
        stream.PutStr(p_param->Value);
        stream.PutStr("</td></tr>\n\r");
        I++;
    }

    stream.PutStr("</table>\n\r");
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CFCGIParams::SetParam(const CSmallString& key, const CSmallString& value)
{
    CSimpleIteratorC<CParam> I(Params);
    CParam*                 p_param;

    while((p_param = I.Current()) != NULL) {
        if( p_param->Key == key ) {
            p_param->Value = value;
            return(true);
        }
        I++;
    }

    try {
        p_param = new CParam;
    } catch(...) {
        return(false);
    }
    p_param->Key = key;
    p_param->Value = value;

    Params.InsertToEnd(p_param,0,true);

    return(true);
}

//------------------------------------------------------------------------------

void CFCGIParams::ClearAllParams(void)
{
    Params.RemoveAll();
}

//------------------------------------------------------------------------------

bool CFCGIParams::LoadParamsFromStream(CFCGIStream& stream)
{
    unsigned char  len_buffer[4];
    char  len_byte;

    while( (len_byte = stream.GetChar()) != EOF) {
        int key_len;
        int value_len;

        len_buffer[0] = len_byte;

        // is it one byte length or four byte length?
        if( len_buffer[0] & 0x80 ) {
            // it is four byte length, read the rest
            if( stream.GetStr((char*)&len_buffer[1],3) != 3 ) {
                ES_ERROR("unable to finish reading of key length (4)");
                return(false);
            }
            key_len = ((len_buffer[0] & 0x7f) << 24) +
                      (len_buffer[1] << 16) +
                      (len_buffer[2] << 8) +
                      len_buffer[3];
        } else {
            key_len = len_buffer[0];
        }
        // now read value length
        if( (len_byte = stream.GetChar()) == EOF ) {
            ES_ERROR("unable to finish reading of value length (1)");
            return(false);
        }
        len_buffer[0] = len_byte;

        // is it one byte length or four byte length?
        if( len_buffer[0] & 0x80 ) {
            // it is four byte length, read the rest
            if( stream.GetStr((char*)&len_buffer[1],3) != 3 ) {
                ES_ERROR("unable to finish reading of value length (4)");
                return(false);
            }
            value_len = ((len_buffer[0] & 0x7f) << 24) +
                        (len_buffer[1] << 16) +
                        (len_buffer[2] << 8) +
                        len_buffer[3];
        } else {
            value_len = len_buffer[0];
        }

        CParam* p_param;

        try {
            p_param = new CParam;
        } catch(...) {
            return(false);
        }

        if( stream.GetStr(p_param->Key,key_len) != key_len ) {
            ES_ERROR("unable to get key");
            delete p_param;
            return(false);
        }

        if( stream.GetStr(p_param->Value,value_len) != value_len ) {
            ES_ERROR("unable to get value");
            delete p_param;
            return(false);
        }

        Params.InsertToEnd(p_param,0,true);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CFCGIParams::LoadParamsFromQuery(const CSmallString& query_string,bool do_not_overwrite)
{
    CSmallString loc_copy = query_string;

    if( loc_copy == "" ) return(true);

    char* p_pointer = NULL;
    char* p_item;

    p_item = strtok_r(loc_copy.GetBuffer(),"&",&p_pointer);
    while( p_item != NULL ) {
        // parse item
        if( AddQueryItem(DecodeString(p_item),do_not_overwrite) == false ) return(false);

        // next item
        p_item = strtok_r(NULL,"&",&p_pointer);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CFCGIParams::LoadParamsFromQuery(bool do_not_overwrite)
{
    CSmallString query_string;
    query_string = GetValue("QUERY_STRING");
    return( LoadParamsFromQuery(query_string, do_not_overwrite) );
}

//------------------------------------------------------------------------------

bool CFCGIParams::LoadParamsFromPost(CFCGIStream& stream,bool do_not_overwrite)
{
    std::stringstream str;
    int c;
    while( (c = stream.GetChar()) != EOF ){
        str << (char)c;
    }
    return( LoadParamsFromQuery(str.str(), do_not_overwrite) );
}

//------------------------------------------------------------------------------

bool CFCGIParams::AddQueryItem(CSmallString string,bool do_not_overwrite)
{
    int len = string.GetLength();
    char* p_item = string.GetBuffer();
    int eq = 0;

    do {
        if( p_item[eq] == '=' ) break;
        eq++;
    } while( eq < len );

    if( eq < len ) p_item[eq]='\0';

    if( ! (HasKey(p_item) && do_not_overwrite) ){
        if( eq+1 < len ) {
            return(SetParam(p_item,&p_item[eq+1]));
        } else {
            return(SetParam(p_item,""));
        }
    }
    return(true);
}

//------------------------------------------------------------------------------

CSmallString CFCGIParams::DecodeString(const CSmallString& string)
{
    CSmallString output;

    // calculate length of string
    unsigned int new_len = 0;

    for(unsigned int i = 0; i < string.GetLength(); i++) {
        if(string[i] == '%') {
            if( i+2 < string.GetLength() ) {
                if( ( (string[i+1] == '0') ||
                        (string[i+1] == '1') ||
                        (string[i+1] == '2') ||
                        (string[i+1] == '3') ||
                        (string[i+1] == '4') ||
                        (string[i+1] == '5') ||
                        (string[i+1] == '6') ||
                        (string[i+1] == '7') ||
                        (string[i+1] == '8') ||
                        (string[i+1] == '9') ||
                        (string[i+1] == 'A') ||
                        (string[i+1] == 'B') ||
                        (string[i+1] == 'C') ||
                        (string[i+1] == 'D') ||
                        (string[i+1] == 'E') ||
                        (string[i+1] == 'F') ) && (
                            (string[i+2] == '0') ||
                            (string[i+2] == '1') ||
                            (string[i+2] == '2') ||
                            (string[i+2] == '3') ||
                            (string[i+2] == '4') ||
                            (string[i+2] == '5') ||
                            (string[i+2] == '6') ||
                            (string[i+2] == '7') ||
                            (string[i+2] == '8') ||
                            (string[i+2] == '9') ||
                            (string[i+2] == 'A') ||
                            (string[i+2] == 'B') ||
                            (string[i+2] == 'C') ||
                            (string[i+2] == 'D') ||
                            (string[i+2] == 'E') ||
                            (string[i+2] == 'F') ) ) {
                    i += 2;
                }
            }
        }
        new_len++;
    }

    // allocate output length
    output.SetLength(new_len);

    // decode and store in output
    unsigned int output_pos = 0;

    for(unsigned int i = 0; i < string.GetLength(); i++) {
        int my_char = string[i];
        if( string[i] == '+' ){
            my_char = ' ';
        }
        if(string[i] == '%') {
            if( i+2 < string.GetLength() ) {

                bool passed = true;
                unsigned char my_number = 0;

                if( (string[i+1] == '0') ||
                        (string[i+1] == '1') ||
                        (string[i+1] == '2') ||
                        (string[i+1] == '3') ||
                        (string[i+1] == '4') ||
                        (string[i+1] == '5') ||
                        (string[i+1] == '6') ||
                        (string[i+1] == '7') ||
                        (string[i+1] == '8') ||
                        (string[i+1] == '9') ) {
                    my_number = (string[i+1] - '0')*16;
                } else {
                    if( (string[i+1] == 'A') ||
                            (string[i+1] == 'B') ||
                            (string[i+1] == 'C') ||
                            (string[i+1] == 'D') ||
                            (string[i+1] == 'E') ||
                            (string[i+1] == 'F') ) {
                        my_number = (string[i+1] - 'A' + 10)*16;
                    } else {
                        passed = false;
                    }
                }

                if( (string[i+2] == '0') ||
                        (string[i+2] == '1') ||
                        (string[i+2] == '2') ||
                        (string[i+2] == '3') ||
                        (string[i+2] == '4') ||
                        (string[i+2] == '5') ||
                        (string[i+2] == '6') ||
                        (string[i+2] == '7') ||
                        (string[i+2] == '8') ||
                        (string[i+2] == '9') ) {
                    my_number = my_number + (string[i+2] - '0');
                } else {
                    if( (string[i+2] == 'A') ||
                            (string[i+2] == 'B') ||
                            (string[i+2] == 'C') ||
                            (string[i+2] == 'D') ||
                            (string[i+2] == 'E') ||
                            (string[i+2] == 'F') ) {
                        my_number = my_number + (string[i+2] - 'A' + 10);
                    } else {
                        passed = false;
                    }
                }

                if( passed ) {
                    my_char = my_number;
                    i += 2;
                }

            }
        }

        output[output_pos] = my_char;
        output_pos++;
    }

    return(output);
}

//------------------------------------------------------------------------------

CSmallString CFCGIParams::EncodeString(const CSmallString& string)
{
    CSmallString buffer;
    for(unsigned int i=0; i < string.GetLength(); i++){
        char c = string[i];
        if(isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            buffer << c;
        } else {
            char sbuff[4];
            sprintf(sbuff,"%%%02X",(unsigned int)c);
            buffer << sbuff;
        }
    }

    return(buffer);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
