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

#include "exceptions.h"
#include "memalloc.h"
#include "chunk.h"
#include "exceptions.h"
#include "configsbase.h"
#include "assetsbase.h"
#include "misc_utils.h"


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

}

Chunk::~Chunk( void )
{
}


void Chunk::SetRenderer( Renderer * p_renderer )
{
    m_renderer = p_renderer;
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

void Chunk::OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node )
{
    if( NULL == m_meshe )
    {
        _DSEXCEPTION( "NULL meshe ; please allocate a meshe object for chunk prior to other operations" );
    }

    for( std::map<Pass*, RenderingNode*>::iterator it = m_passesnodes.begin(); it != m_passesnodes.end(); ++it )
    {
        Pass* current_pass = it->first;

        current_pass->GetRenderingQueue()->Add( (*it).second );
    }
    m_scenenodegraph = p_scenegraph;
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
    m_renderer->DrawMeshe( p_rendering_node->GetMeshe()->GetVertexListSize(), p_rendering_node->GetMeshe()->GetTrianglesListSize(), m_globaltransformation, view, proj );
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