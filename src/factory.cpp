/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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

#include "factory.h"
#include "assetsbase.h"
#include "configsbase.h"

DrawSpace::Core::Factory* DrawSpace::Core::Factory::m_instance = NULL;

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

Factory::Factory( void )
{


}

Factory::~Factory( void )
{

}

bool Factory::on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words )
{
    if( m_capture_texture_props )
    {
        m_text_properties.push_back( p_line );

        if( "end_texture" == p_words[0] )
        {
            m_capture_texture_props = false;

            // instanciate texture object
            // call ParseProperties() method
            // register in Assetsbase
        }
    }
    else
    {
        if( "declare_texture" == p_words[0] )
        {
            m_capture_texture_props = true;
            m_text_properties.clear();

            m_text_properties.push_back( p_line );
        }
        else
        {
            _PARSER_UNEXPECTED_KEYWORD_
        }
    }
    return true;
}

bool Factory::ExecuteFromTextFile( const dsstring& p_path )
{
    m_capture_texture_props = false;

    char seps[] = { 0x09, 0x020, 0x00 };
    // run parser
    return Run( p_path, seps );
}

bool Factory::ExecuteFromBinaryFile( const dsstring& p_path )
{
    return false;
}

bool Factory::ExecuteFromArchiveChunk( const DrawSpace::Utils::Archive& p_arc )
{
    return false;
}

bool Factory::ExecuteFromTextChunk( const dsstring& p_text )
{
    return false;
}

void Factory::RegisterInstanciationFuncByText( const dsstring& p_keyword, Asset::InstanciateFunc p_func )
{
    m_instanciationfuncs_bytext[p_keyword] = p_func;
}
