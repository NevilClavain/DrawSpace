/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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
/* -*-LIC_END-*- */


#include "ac3dmeshe.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

dsstring AC3DMesheImport::m_rootpath = ".";

AC3DMesheImport::AC3DMesheImport( void )
{
}

AC3DMesheImport::~AC3DMesheImport( void )
{
}

bool AC3DMesheImport::on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words )
{
    switch( m_state )
    {
        case SEARCH_OBJECT_BEGIN:

            if( "OBJECT" == p_words[0] )
            {
                if( "world" != p_words[1] )
                {					
                    if( m_object_count == m_object_index )
                    {
                        //m_state	= SEARCH_OBJECT_LOC;
                        m_state	= SEARCH_VERT_BEGIN;
                    }
                    m_object_count++;
                }
            }
            break;

        case SEARCH_VERT_BEGIN:

            if( "loc" == p_words[0] )
            {
                m_object_loc[0] = atof( p_words[1].c_str() );
                m_object_loc[1] = atof( p_words[2].c_str() );
                m_object_loc[2] = atof( p_words[3].c_str() );
            }
            if( "numvert" == p_words[0] )
            {
                m_state = VERT_INPUT;
                m_numvert = atoi( p_words[1].c_str() );
                m_vertcount = 0;
            }
            break;

        case VERT_INPUT:
            {
                DrawSpace::Core::Vertex vertex;
                vertex.x = atof( p_words[0].c_str() );
                vertex.x += m_object_loc[0];

                vertex.y = atof( p_words[1].c_str() );
                vertex.y += m_object_loc[1];

                vertex.z = atof( p_words[2].c_str() );
                vertex.z += m_object_loc[2];

                m_meshe->AddVertex( vertex );				
                m_vertcount++;

                if( m_vertcount == m_numvert )
                {
                    m_state = SEARCH_TRILIST_BEGIN;
                }
            }
            break;

        case SEARCH_TRILIST_BEGIN:

            if( "numsurf" == p_words[0] )
            {				
                m_state = SEARCH_TRI_BEGIN;
                m_numtri = atoi( p_words[1].c_str() );
                m_tricount = 0;
            }
            break;

        case SEARCH_TRI_BEGIN:

            if( "refs" == p_words[0] )
            {
                m_trilinecount = 0;
                m_state = TRI_INPUT;	
            }
            break;

        case TRI_INPUT:
            {
                long index = atoi( p_words[0].c_str() );
                float u = (float)atof( p_words[1].c_str() );
                float v = 1.0f - (float)atof( p_words[2].c_str() );

                if( 0 == m_vertices_uv_mem.count( index ) )
                {
                    Vertex vertex;
                    m_meshe->GetVertex( index, vertex );
                    vertex.tu[0] = u;
                    vertex.tv[0] = v;
                    m_meshe->SetVertex( index, vertex );
                    // memoriser les uv pour cet index
                    m_vertices_uv_mem[index].first = u;
                    m_vertices_uv_mem[index].second = v;
                }
                else
                {
                    if( u != m_vertices_uv_mem[index].first || 
                        v != m_vertices_uv_mem[index].second )
                    {
                        Vertex vertex;
                        m_meshe->GetVertex( index, vertex );
                        vertex.tu[0] = u;
                        vertex.tv[0] = v;
                        m_meshe->AddVertex( vertex );

                        index = m_meshe->GetVertexListSize() - 1;
                    }
                }
        
                switch( m_trilinecount )
                {
                    case 0:
                        m_triangle.vertex1 = index;
                        break;
                    case 1:
                        m_triangle.vertex2 = index;
                        break;
                    case 2:
                        m_triangle.vertex3 = index;
                        break;
                }

                m_trilinecount++;
                if( 3 == m_trilinecount )
                {
                    // storing triangle...
                    m_meshe->AddTriangle( m_triangle );
                    m_tricount++;
                    if( m_tricount == m_numtri )
                    {
                        m_state = DONE;
                    }
                    else
                    {
                        m_state = SEARCH_TRI_BEGIN;
                    }
                }
            }
            break;
    }

    return true;
}

bool AC3DMesheImport::LoadFromFile( const dsstring& p_filepath, long p_index, Meshe* p_meshe )
{
    dsstring final_path = m_rootpath + "/";
    
    final_path += p_filepath;

    m_meshe = p_meshe;
    m_object_index = p_index;
    m_state = SEARCH_OBJECT_BEGIN;
    m_object_count = 0;
    m_vertcount = 0;
    m_tricount = 0;
    m_vertices_uv_mem.clear();
    m_object_loc[0] = m_object_loc[1] = m_object_loc[2] = m_object_loc[3] = 0.0;
    return Parser::Run( final_path, " " );
}

void AC3DMesheImport::SetRootPath( const dsstring& p_path )
{
    m_rootpath = p_path;
}
