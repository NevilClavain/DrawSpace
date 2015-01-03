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

#include "cbfgfont.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

CBFGFontImport::CBFGFontImport( void ) : m_font( NULL )
{
}

CBFGFontImport::~CBFGFontImport( void )
{
}

bool CBFGFontImport::on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words )
{
    if( "Cell" == p_words[0] && "Width" == p_words[1] )
    {
        m_cell_width = atoi( p_words[2].c_str() );
    }
    if( "Cell" == p_words[0] && "Height" == p_words[1] )
    {
        m_cell_height = atoi( p_words[2].c_str() );
    }
    if( "Start" == p_words[0] && "Char" == p_words[1] )
    {
        m_start_char = atoi( p_words[2].c_str() );
    }
    if( "Image" == p_words[0] && "Width" == p_words[1] )
    {
        m_texture_width = atoi( p_words[2].c_str() );
    }
    if( "Image" == p_words[0] && "Height" == p_words[1] )
    {
        m_texture_height = atoi( p_words[2].c_str() );
    }
    if( "Char" == p_words[0] && "Base" == p_words[2] && "Width" == p_words[3] )
    {
        unsigned char current_ch = atoi( p_words[1].c_str() );
        if( current_ch >= m_start_char )
        {
            unsigned char char_width = atoi( p_words[4].c_str() );

            long nb_cell_width = m_texture_width / m_cell_width;
            long nb_cell_height = m_texture_height / m_cell_height;

            // numero de la "case"
            unsigned char cell_num = current_ch - m_start_char;

            // en deduire les coordonnees de la "case"
            long x_cell = cell_num % nb_cell_width;
            long y_cell = cell_num / nb_cell_width;

            // en deduire les coords u,v

            float u,v;
            u = ( (float)x_cell * m_cell_width ) / (float)m_texture_width;
            v = ( (float)y_cell * m_cell_height ) / (float)m_texture_height;

            // dimensions image
            float width, height;
            width = (float)char_width / (float)m_texture_width;
            height = (float)m_cell_height / (float)m_texture_height;

            m_font->AddCharMapping( current_ch, u, v, width, height );
        }
    }

    return true;
}

bool CBFGFontImport::LoadFromFile( const dsstring& p_metricsfilepath, Core::Font* p_font )
{
    m_font = p_font;
    return Parser::Run( p_metricsfilepath, " ," );
}
