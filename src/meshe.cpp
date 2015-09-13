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

#include "meshe.h"
#include "mesheimport.h"
#include "md5.h"
#include "exceptions.h"
#include "misc_utils.h"
#include "pimanager.h"
#include "ac3dmeshe.h"
#include "assetsbase.h"
#include "renderer.h"
#include "plugin.h"



using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface;

Meshe::Meshe( void ) : 
m_importer( NULL ), 
m_render_data( NULL )
{
    // properties array creation
    m_properties["filepath"].AddPropValue<dsstring>( "" );
    m_properties["assetname"].AddPropValue<dsstring>( m_assetname );
    m_properties["index"].AddPropValue<long>( 0 );
    m_properties["mode"].AddPropValue<dsstring>( "" );
    m_properties["spherical_normales"].AddPropValue<bool>( false );
}

Meshe::~Meshe( void )
{

}

bool Meshe::on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words )
{
    if( "filepath" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["filepath"].SetPropValue<dsstring>( p_words[1] );
    }
    else if( "assetname" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["assetname"].SetPropValue<dsstring>( p_words[1] );
    }
    else if( "index" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["index"].SetPropValue<long>( StringToInt( p_words[1] ) );
    }
    else if( "mode" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["mode"].SetPropValue<dsstring>( p_words[1] );
    }
    else if( "spherical_normales" == p_words[0] )
    {
        if( p_words.size() < 2 )
        {
            _PARSER_MISSING_ARG__
            return false;
        }

        m_properties["spherical_normales"].SetPropValue<bool>( StringToInt( p_words[1] ) );
    }
    else
    {
        _PARSER_UNEXPECTED_KEYWORD_
        return false;
    }
    return true;
}

void Meshe::SetImporter( DrawSpace::Interface::MesheImport* p_importer )
{
    m_importer = p_importer;
}

bool Meshe::LoadFromFile( const dsstring& p_filepath, long p_index )
{
    if( NULL == m_importer )
    {
        return false;
    }
    return m_importer->LoadFromFile( p_filepath, p_index, this );
}

long Meshe::GetVertexListSize( void )
{
    return (long)m_vertices.size();
}

long Meshe::GetTrianglesListSize( void )
{
    return (long)m_triangles.size();
}

void Meshe::GetVertex( long p_index, Vertex& p_vertex )
{
    p_vertex = m_vertices[p_index];
}

void Meshe::GetTriangles( long p_index, Triangle& p_triangle )
{
    p_triangle = m_triangles[p_index];
}

void Meshe::AddVertex( const Vertex& p_vertex )
{
    m_vertices.push_back( p_vertex );
}

void Meshe::SetVertex( long p_index, const Vertex& p_vertex )
{
    m_vertices[p_index] = p_vertex;
}

void Meshe::AddTriangle( const Triangle& p_triangle )
{
    m_triangles.push_back( p_triangle );

    m_triangles_for_vertex[p_triangle.vertex1].push_back( p_triangle );
    m_triangles_for_vertex[p_triangle.vertex2].push_back( p_triangle );
    m_triangles_for_vertex[p_triangle.vertex3].push_back( p_triangle );
}

void Meshe::ClearTriangles( void )
{
    m_triangles.clear();
    m_triangles_for_vertex.clear();
}

void Meshe::ClearVertices( void )
{
    m_vertices.clear();
}


bool Meshe::UpdateIndexes( void )
{
    if( !m_render_data )
    {       
        return false;
    }

    Renderer* renderer = SingletonPlugin<Renderer>::GetInstance()->m_interface;
    renderer->UpdateMesheIndexes( this, m_render_data );
    return true;
}

bool Meshe::UpdateVertices( void )
{
    if( !m_render_data )
    {       
        return false;
    }

    Renderer* renderer = SingletonPlugin<Renderer>::GetInstance()->m_interface;
    renderer->UpdateMesheVertices( this, m_render_data );
    return true;
}

void Meshe::GetCenter( Vector& p_vector )
{
    dsreal xsum = 0.0;
    dsreal ysum = 0.0;
    dsreal zsum = 0.0;

    for( size_t i = 0; i < m_vertices.size(); i++ )
    {
        xsum += m_vertices[i].x;
        ysum += m_vertices[i].y;
        zsum += m_vertices[i].z;
    }

    p_vector[0] = xsum / m_vertices.size();
    p_vector[1] = ysum / m_vertices.size();
    p_vector[2] = zsum / m_vertices.size();
    p_vector[3] = 1.0;
}

void Meshe::GetAABB( Vector& p_min, Vector& p_max )
{
    if( m_vertices.size() > 0 )
    {
        dsreal minx, maxx;
        maxx = minx = m_vertices[0].x;

        dsreal miny, maxy;
        maxy = miny = m_vertices[0].y;

        dsreal minz, maxz;
        maxz = minz = m_vertices[0].z;

        for( size_t i = 1; i < m_vertices.size(); i++ )
        {
            if( m_vertices[i].x < minx )
            {
                minx = m_vertices[i].x;
            }

            if( m_vertices[i].x > maxx )
            {
                maxx = m_vertices[i].x;
            }

            if( m_vertices[i].y < miny )
            {
                miny = m_vertices[i].y;
            }

            if( m_vertices[i].y > maxy )
            {
                maxy = m_vertices[i].y;
            }

            if( m_vertices[i].z < minz )
            {
                minz = m_vertices[i].z;
            }

            if( m_vertices[i].z > maxz )
            {
                maxz = m_vertices[i].z;
            }
        }

        p_min[0] = minx;
        p_min[1] = miny;
        p_min[2] = minz;
        p_min[3] = 1.0;

        p_max[0] = maxx;
        p_max[1] = maxy;
        p_max[2] = maxz;
        p_max[3] = 1.0;
    }
}

bool Meshe::ApplyProperties( void )
{
    m_assetname = m_properties["assetname"].GetPropValue<dsstring>();

    dsstring path = m_properties["filepath"].GetPropValue<dsstring>();
    long index = m_properties["index"].GetPropValue<long>();

    dsstring mode = m_properties["mode"].GetPropValue<dsstring>();
    

    MesheImport* mesheimp;

    if( mode == "ac3d" )
    {
        //mesheimp = new DrawSpace::Utils::AC3DMesheImport();
        //mesheimp = _DRAWSPACE_NEW_( AC3DMesheImport, AC3DMesheImport );
        mesheimp = AssetsBase::GetInstance()->GetAC3CMesheImporter();
    }
    else
    {
        _DSEXCEPTION( "Bad meshe import name : " << mode )
    }
    SetImporter( mesheimp );
    if( LoadFromFile( path, index ) )
    {
        ComputeNormales( m_properties["spherical_normales"].GetPropValue<bool>() );
        return true;
    }
    return false;
}

void Meshe::Serialize( Archive& p_archive  )
{

}

bool Meshe::Unserialize( Archive& p_archive )
{
    return false;
}

void Meshe::DumpProperties( dsstring& p_text )
{
    dsstring text_value;

    //p_text = "declare_asset ";
    //p_text += dsstring( MESHE_TEXT_KEYWORD );

    //p_text += "\n";

    p_text += "assetname ";
    p_text += m_properties["assetname"].GetPropValue<dsstring>();
    p_text += "\r\n";

    p_text += "filepath ";
    p_text += m_properties["filepath"].GetPropValue<dsstring>();
    p_text += "\r\n";

    p_text += "index ";

    IntToString( m_properties["index"].GetPropValue<long>(), text_value );
    p_text += text_value;
    p_text += "\r\n";

    p_text += "mode ";
    p_text += m_properties["plugin"].GetPropValue<dsstring>();
    p_text += "\r\n";

    IntToString( m_properties["spherical_normales"].GetPropValue<bool>(), text_value );
    p_text += text_value;
    p_text += "\r\n";


    //p_text += "end_asset\n";

}

bool Meshe::ParseProperties( const dsstring& p_text )
{
    char seps[] = { 0x09, 0x020, 0x00 };
    return RunOnTextChunk( p_text, seps );
}


void Meshe::GetMD5( dsstring& p_md5 )
{
    MD5 md5;

    Vertex* vbuff = new Vertex[m_vertices.size()];
    Vertex* curr = vbuff;

    for( size_t i = 0; i < m_vertices.size(); i++ )
    {
        *curr = m_vertices[i];
        curr++;
    }
    dsstring hash_v = md5.digestMemory( (BYTE*)vbuff, (int)( m_vertices.size() * sizeof( Vertex ) ) );

    Triangle* tbuff = new Triangle[m_triangles.size()];
    Triangle* curr2 = tbuff;

    for( size_t i = 0; i < m_triangles.size(); i++ )
    {
        *curr2 = m_triangles[i];
        curr2++;
    }
    dsstring hash_t = md5.digestMemory( (BYTE*)tbuff, (int)( m_triangles.size() * sizeof( Triangle ) ) );


    p_md5 = hash_v + hash_t;

    delete[] vbuff;
    delete[] tbuff;
}

Asset* Meshe::Instanciate( void )
{
    return _DRAWSPACE_NEW_( Meshe, Meshe );
}

void Meshe::GetKeyword( dsstring& p_outkeyword )
{
    p_outkeyword = MESHE_TEXT_KEYWORD;
}

void Meshe::ComputeNormales( bool p_spherical_normales )
{
    if( p_spherical_normales )
    {
        for( size_t i = 0; i < m_vertices.size(); i++ )
        {
            Vector v( m_vertices[i].x, m_vertices[i].y, m_vertices[i].z, 1.0 );

            v.Normalize();

            m_vertices[i].nx = v[0];
            m_vertices[i].ny = v[1];
            m_vertices[i].nz = v[2];
        }
    }
    else
    {
        for( std::map<long, std::vector<Triangle>>::iterator it = m_triangles_for_vertex.begin(); it != m_triangles_for_vertex.end(); ++it )
        {
            Vector normales_sum;

            std::vector<Triangle> triangles_list = it->second;

            for( size_t i = 0; i < triangles_list.size(); i++ )
            {
                Triangle triangle = triangles_list[i];
                Vertex v1 = m_vertices[triangle.vertex1];
                Vertex v2 = m_vertices[triangle.vertex2];
                Vertex v3 = m_vertices[triangle.vertex3];

                Vector d1( v2.x - v1.x, v2.y - v1.y, v2.z - v1.z, 1.0 );
                Vector d2( v3.x - v1.x, v3.y - v1.y, v3.z - v1.z, 1.0 );

                Vector res = ProdVec( d1, d2 );
                res.Normalize();

                normales_sum = normales_sum + res;
            }

            normales_sum.Scale( 1.0 / triangles_list.size() );
            normales_sum.Normalize();

            m_vertices[it->first].nx = normales_sum[0];
            m_vertices[it->first].ny = normales_sum[1];
            m_vertices[it->first].nz = normales_sum[2];
        }
    }
}

void Meshe::SetRenderData( void* p_renderdata )
{
    m_render_data = p_renderdata;
}

void* Meshe::GetRenderData( void )
{
    return m_render_data;
}