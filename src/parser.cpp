/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
*                                                                          
* This file is part of DrawSpace.                                          
*                                                                          
*    DrawSpace is free software: you can redistribute it and/or modify     
*    it under the terms of the GNU General Public License as published by  
*    the Free Software Foundation, either version 3 of the License, or     
*    (at your option) any later version.                                   
*                                                                          
*    DrawSpace is distributed in the hope that it will be useful,          
*    but WITHOUT ANY WARRANTY; without even the implied warranty of        
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         
*    GNU General Public License for more details.                          
*                                                                          
*    You should have received a copy of the GNU General Public License     
*    along with DrawSpace.  If not, see <http://www.gnu.org/licenses/>.    
*
*/

#include "parser.h"
#include "misc_utils.h"

using namespace DrawSpace::Utils;

Parser::Parser( void )
{

}

Parser::~Parser( void )
{


}

void Parser::split_text( const dsstring& p_text, std::vector<dsstring>& p_lines )
{
    char* pch;
    char* text = new char[p_text.size() + 1];
    strcpy( text, p_text.c_str() );

    char seps[] = { 0xd, 0xa, 0x00 };

    pch = strtok( text, seps );
    if( pch )
    {
        p_lines.push_back( pch );
    }
    while( pch != NULL )
    {
        pch = strtok( NULL, seps );
        if( pch )
        {
            p_lines.push_back( pch );
        }
    }

    delete[] text;
}

void Parser::split_line( const dsstring& p_line, const dsstring& p_separators, std::vector<dsstring>& p_words )
{
    char* pch;
    char line[256];
    strcpy( line, p_line.c_str() );
    pch = strtok( line, p_separators.c_str() );
    if( pch )
    {
        p_words.push_back( pch );
    }
    while( pch != NULL )
    {
        pch = strtok( NULL, p_separators.c_str() );
        if( pch )
        {
            p_words.push_back( pch );
        }
    }
}

bool Parser::Run( const dsstring& p_filepath, const dsstring& p_separators )
{
    m_lasterror = "";
    bool status = true;
    long line_count = 0;

    FILE* fp = fopen( p_filepath.c_str(), "r" );
    if( fp )
    {
        char line[256];

        while( fgets( line, 256, fp ) )
        {
            line_count++;
            // supprimer le retour chariot en fin de ligne
            if( 0x0a != line[0] )
            {
                size_t len = strlen( line );
                if( 0x0a == line[len - 1 ] )
                {
                    line[len - 1] = 0x00;
                }

                std::vector<dsstring> words;
                split_line( line, p_separators, words );

                if( !on_new_line( line, line_count, words ) )
                {
                    status = false;
                    break;
                }
            }
        }
        fclose( fp );
    }
    else
    {
        status = false;
    }

    return status;
}

bool Parser::RunOnTextChunk( const dsstring& p_text, const dsstring& p_separators )
{
    m_lasterror = "";
    long line_count = 0;
    bool status = true;

    std::vector<dsstring> lines;
    split_text( p_text, lines );
    for( size_t i = 0; i < lines.size(); i++ )
    {
        line_count++;
        std::vector<dsstring> words;
        split_line( lines[i], p_separators, words );

        if( !on_new_line( lines[i], line_count, words ) )
        {
            status = false;
            break;
        }        
    }
    return status;
}

void Parser::GetLastError( dsstring& p_lasterror )
{
    p_lasterror = m_lasterror;
}

void Parser::error_message( long p_line_num, const dsstring& p_msg )
{
    dsstring line_num;

    IntToString( p_line_num, line_num );

    m_lasterror = "line " + line_num;
    m_lasterror += ": " + p_msg;
}
