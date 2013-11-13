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

#include "chunk.h"
#include "memalloc.h"

using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;

Chunk::Chunk( void ) : m_renderer( NULL ), m_scenegraph( NULL )
{
    m_meshe = _DRAWSPACE_NEW_( Core::Meshe, Core::Meshe );
}

Chunk::~Chunk( void )
{
    _DRAWSPACE_DELETE_( m_meshe );
}

void Chunk::GetDescr( dsstring& p_descr )
{
    p_descr = "Chunk";
}

void Chunk::DumpMemoryAllocs( void )
{
    DrawSpace::Utils::MemAlloc::GetInstance()->DumpContent();
}

void Chunk::SetRenderer( Renderer * p_renderer )
{
    m_renderer = p_renderer;
}

void Chunk::OnRegister( Scenegraph* p_scenegraph )
{
    for( std::map<dsstring, RenderingNode*>::iterator it = m_passesnodes.begin(); it != m_passesnodes.end(); ++it )
    {
        Pass* current_pass = p_scenegraph->GetPass( (*it).first );
        if( current_pass != NULL )
        {
            current_pass->GetRenderingQueue()->Add( (*it).second );
        }
    }
    m_scenegraph = p_scenegraph;
}

bool Chunk::LoadAssets( void )
{
    for( std::map<dsstring, Core::RenderingNode*>::iterator it = m_passesnodes.begin(); it != m_passesnodes.end(); ++it )
    {
        if( false == m_renderer->CreateRenderingNode( (*it).second ) )
        {
            return false;
        }
        if( false == m_renderer->AddMesheToNode( m_meshe, (*it).second, "0" ) )
        {
            return false;
        }
    }
    return true;		
}

Core::Meshe* Chunk::GetMeshe( const dsstring& p_mesheid )
{
    return m_meshe;
}

void Chunk::on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node )
{
    DrawSpace::Utils::Matrix view;
    m_scenegraph->GetCurrentCameraView( view );
    m_renderer->RenderNodeMeshe( m_globaltransformation, view, p_rendering_node, "0" );
}

void Chunk::RegisterPassSlot( const dsstring p_passname )
{
    m_passesnodes[p_passname] = _DRAWSPACE_NEW_( RenderingNode, RenderingNode );

    RenderingNodeDrawCallback* cb = _DRAWSPACE_NEW_( RenderingNodeDrawCallback, RenderingNodeDrawCallback( this, &Chunk::on_renderingnode_draw ) );

    m_passesnodes[p_passname]->RegisterHandler( cb );
    m_callbacks.push_back( cb );

}

DrawSpace::Core::RenderingNode* Chunk::GetNodeFromPass( const dsstring p_passname, const dsstring& p_nodeid )
{
    if( 0 == m_passesnodes.count( p_passname ) )
    {
        return NULL;
    }

    return m_passesnodes[p_passname];
}

void Chunk::GetNodesIdsList( std::vector<dsstring>& p_ids )
{
    p_ids.push_back( "" );
}