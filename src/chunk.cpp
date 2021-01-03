/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2021                     
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

/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
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

#include "exceptions.h"
#include "memalloc.h"
#include "chunk.h"
#include "exceptions.h"
#include "misc_utils.h"
#include "renderer.h"
#include "plugin.h"


using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

Chunk::Chunk( void ) : 
m_renderer( NULL ), 
m_scenenodegraph( NULL ),
m_meshe( NULL ),
m_ignore_camera( false )
{
    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
}

Chunk::~Chunk( void )
{
}


void Chunk::SetDrawingState( Pass* p_pass, bool p_drawing )
{
    if( m_passesnodes.count( p_pass ) > 0 )
    {
        m_passesnodes[p_pass]->SetDrawingState( p_drawing );
        return;
    }

    dsstring msg = "Chunk : unknown pass";
    _DSEXCEPTION( msg )

}

void Chunk::SetImpostorsDisplayList( const DrawSpace::ImpostorsDisplayList& p_idl )
{
    m_idl = p_idl;
}

void Chunk::GetImpostorsDisplayList( DrawSpace::ImpostorsDisplayList& p_idl )
{
    p_idl = m_idl;
}

void Chunk::ImpostorsInit( int p_nbmax_impostors )
{
    m_meshe->ClearTriangles();
    m_meshe->ClearVertices();

    for( size_t i = 0; i < m_idl.size(); i++ )
    {
        Vertex v1, v2, v3, v4;

        // vertex x,y,z set by impostors shaders
        v1.x = 0.0;
        v1.y = 0.0;
        v1.z = 0.0;

        v1.tu[0] = m_idl[i].u1;
        v1.tv[0] = m_idl[i].v1;
        v1.nx = 1.0;

        v1.tu[6] = m_idl[i].spherical_ray;
        v1.tv[6] = m_idl[i].spherical_longitud;
        v1.tw[6] = m_idl[i].spherical_latitud;

        v1.tu[7] = m_idl[i].localpos[0];
        v1.tv[7] = m_idl[i].localpos[1];
        v1.tw[7] = m_idl[i].localpos[2];
        v1.tu[8] = m_idl[i].width_scale;
        v1.tv[8] = m_idl[i].height_scale;

        // vertex x,y,z set by impostors shaders
        v2.x = 0.0;
        v2.y = 0.0;
        v2.z = 0.0;

        v2.tu[0] = m_idl[i].u2;
        v2.tv[0] = m_idl[i].v2;
        v2.nx = 2.0;

        v2.tu[6] = m_idl[i].spherical_ray;
        v2.tv[6] = m_idl[i].spherical_longitud;
        v2.tw[6] = m_idl[i].spherical_latitud;

        v2.tu[7] = m_idl[i].localpos[0];
        v2.tv[7] = m_idl[i].localpos[1];
        v2.tw[7] = m_idl[i].localpos[2];
        v2.tu[8] = m_idl[i].width_scale;
        v2.tv[8] = m_idl[i].height_scale;

        // vertex x,y,z set by impostors shaders
        v3.x = 0.0;
        v3.y = 0.0;
        v3.z = 0.0;

        v3.tu[0] = m_idl[i].u3;
        v3.tv[0] = m_idl[i].v3;
        v3.nx = 3.0;

        v3.tu[6] = m_idl[i].spherical_ray;
        v3.tv[6] = m_idl[i].spherical_longitud;
        v3.tw[6] = m_idl[i].spherical_latitud;

        v3.tu[7] = m_idl[i].localpos[0];
        v3.tv[7] = m_idl[i].localpos[1];
        v3.tw[7] = m_idl[i].localpos[2];
        v3.tu[8] = m_idl[i].width_scale;
        v3.tv[8] = m_idl[i].height_scale;

        // vertex x,y,z set by impostors shaders
        v4.x = 0.0;
        v4.y = 0.0;
        v4.z = 0.0;

        v4.tu[0] = m_idl[i].u4;
        v4.tv[0] = m_idl[i].v4;
        v4.nx = 4.0;

        v4.tu[6] = m_idl[i].spherical_ray;
        v4.tv[6] = m_idl[i].spherical_longitud;
        v4.tw[6] = m_idl[i].spherical_latitud;

        v4.tu[7] = m_idl[i].localpos[0];
        v4.tv[7] = m_idl[i].localpos[1];
        v4.tw[7] = m_idl[i].localpos[2];
        v4.tu[8] = m_idl[i].width_scale;
        v4.tv[8] = m_idl[i].height_scale;
    
        m_meshe->AddVertex( v1 );
        m_meshe->AddVertex( v2 );
        m_meshe->AddVertex( v3 );
        m_meshe->AddVertex( v4 );

        size_t index_base = 4 * i;

        m_meshe->AddTriangle( Triangle( index_base, 3 + index_base, 1 + index_base ) );
        m_meshe->AddTriangle( Triangle( 1 + index_base, 3 + index_base, 2 + index_base ) );
    }

    if( p_nbmax_impostors > -1 )
    {
        // padding : on va a la fin allouer des buffers correspondant � la taille max d'impostors qu'on souhaite.
        // Utile si mise a jour des impostors avec ImpostorsUpdate() (comme c'est le cas avec les clouds volumetrique par exemple)

        for( size_t i = 0; i < p_nbmax_impostors - m_idl.size(); i++ )
        {
            Vertex v1, v2, v3, v4;
             
            m_meshe->AddVertex( v1 );
            m_meshe->AddVertex( v2 );
            m_meshe->AddVertex( v3 );
            m_meshe->AddVertex( v4 );

            size_t index_base = 4 * i;

            m_meshe->AddTriangle( Triangle( index_base, 3 + index_base, 1 + index_base ) );
            m_meshe->AddTriangle( Triangle( 1 + index_base, 3 + index_base, 2 + index_base ) );                
        }
    }
}

void Chunk::ImpostorsUpdate( void )
{
    Renderer* renderer = SingletonPlugin<Renderer>::GetInstance()->m_interface;
    renderer->UpdateMesheVerticesFromImpostors( m_idl, m_meshe->GetRenderData() );
}

void Chunk::OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node )
{
    if( NULL == m_meshe )
    {
        _DSEXCEPTION( "NULL meshe ; please allocate a meshe object for chunk prior to other operations" );
    }

    for( auto it = m_passesnodes.begin(); it != m_passesnodes.end(); ++it )
    {
        Pass* current_pass = it->first;

        current_pass->GetRenderingQueue()->Add( (*it).second );
    }
    m_scenenodegraph = p_scenegraph;
}

void Chunk::OnUnregister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node )
{
	for (auto it = m_passesnodes.begin(); it != m_passesnodes.end(); ++it)
	{
		Pass* current_pass = it->first;

		current_pass->GetRenderingQueue()->Remove( (*it).second );
	}
	m_scenenodegraph = NULL;
}

Core::Meshe* Chunk::GetMeshe( void )
{
    return m_meshe;
}

void Chunk::SetMeshe( DrawSpace::Core::Meshe* p_meshe )
{
    m_meshe = p_meshe;
}


void Chunk::on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node )
{
    DrawSpace::Utils::Matrix view;
    DrawSpace::Utils::Matrix proj;

    view.Identity();
    if( m_scenenodegraph )
    {
        if( !m_ignore_camera )
        {
            m_scenenodegraph->GetCurrentCameraView( view );
        }
        m_scenenodegraph->GetCurrentCameraProj( proj );
    }
    m_renderer->DrawMeshe( m_globaltransformation, view, proj );
}

void Chunk::RegisterPassSlot( Pass* p_pass )
{
    m_passesnodes[p_pass] = _DRAWSPACE_NEW_( RenderingNode, RenderingNode );

    if( NULL == m_meshe )
    {
        _DSEXCEPTION( "NULL meshe ; please allocate a meshe object for chunk prior to other operations" );
    }

    m_passesnodes[p_pass]->SetMeshe( m_meshe );

    RenderingNodeDrawCallback* cb = _DRAWSPACE_NEW_( RenderingNodeDrawCallback, RenderingNodeDrawCallback( this, &Chunk::on_renderingnode_draw ) );

    m_passesnodes[p_pass]->RegisterHandler( cb );
    m_callbacks.push_back( cb );

}

DrawSpace::Core::RenderingNode* Chunk::GetNodeFromPass( Pass* p_pass )
{
    if( 0 == m_passesnodes.count( p_pass ) )
    {
        return NULL;
    }

    return m_passesnodes[p_pass];
}


void Chunk::GetBaseTransform( DrawSpace::Utils::Matrix& p_mat )
{
    p_mat.Identity();
}

void Chunk::SetFinalTransform( const DrawSpace::Utils::Matrix& p_mat )
{
    m_globaltransformation = p_mat;
}

void Chunk::Update2( DrawSpace::Utils::TimeManager& p_timemanager )
{
}

void Chunk::IgnoreCamera( bool p_ignore )
{
    m_ignore_camera = p_ignore;
}

void Chunk::GetPassesNodesList( std::map<Pass*, DrawSpace::Core::RenderingNode*>& p_list )
{
    p_list = m_passesnodes;
}
