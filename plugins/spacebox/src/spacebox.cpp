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

#include "spacebox.h"
#include "memalloc.h"

using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;

Spacebox::Spacebox( void ) : m_renderer( NULL ), m_scenegraph( NULL )
{
    for( long i = 0; i < 6; i++ )
    {
        m_meshes[i] = _DRAWSPACE_NEW_( Core::Meshe, Core::Meshe );

        // TODO
        switch( i )
        {
            case FrontQuad:

                break;

            case RearQuad:

                break;

            case LeftQuad:

                break;

            case RightQuad:

                break;

            case TopQuad:

                break;

            case BottomQuad:

                break;
        }

        m_meshes[i]->AddTriangle( Triangle( 0, 1, 3 ) );
        m_meshes[i]->AddTriangle( Triangle( 1, 2, 3 ) );
    }
}

Spacebox::~Spacebox( void )
{
}

void Spacebox::GetDescr( dsstring& p_descr )
{
    p_descr = "Spacebox";
}

void Spacebox::DumpMemoryAllocs( void )
{
    DrawSpace::Utils::MemAlloc::GetInstance()->DumpContent();
}

void Spacebox::SetRenderer( Renderer * p_renderer )
{
    m_renderer = p_renderer;
}

void Spacebox::OnRegister( Scenegraph* p_scenegraph )
{
    // TODO
}

bool Spacebox::LoadAssets( void )
{
    // TODO

    return false;
}

Core::Meshe* Spacebox::GetMeshe( const dsstring& p_mesheid )
{
    if( "front" == p_mesheid )
    {
        return m_meshes[FrontQuad];
    }
    else if( "rear" == p_mesheid )
    {
        return m_meshes[RearQuad];
    }
    else if( "left" == p_mesheid )
    {
        return m_meshes[LeftQuad];
    }
    else if( "right" == p_mesheid )
    {
        return m_meshes[RightQuad];
    }
    else if( "top" == p_mesheid )
    {
        return m_meshes[TopQuad];
    }
    else if( "bottom" == p_mesheid )
    {
        return m_meshes[BottomQuad];
    }
    return NULL;
}

void Spacebox::on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node )
{
    DrawSpace::Utils::Matrix view;
    DrawSpace::Utils::Matrix world;

    world.Identity();
    m_scenegraph->GetCurrentCameraView( view );
    view.ClearTranslation();

    m_renderer->RenderNodeMeshe( world, view, p_rendering_node, m_nodes_mesheid[p_rendering_node] );
}

void Spacebox::RegisterPassSlot( const dsstring p_passname )
{
    NodesSet nodeset;
    for( long i = 0; i < 6; i++ )
    {
        nodeset.nodes[i] = _DRAWSPACE_NEW_( RenderingNode, RenderingNode );
        RenderingNodeDrawCallback* cb = _DRAWSPACE_NEW_( RenderingNodeDrawCallback, RenderingNodeDrawCallback( this, &Spacebox::on_renderingnode_draw ) );

        nodeset.nodes[i]->RegisterHandler( cb );
        m_callbacks.push_back( cb );

        switch( i )
        {
            case FrontQuad:

                m_nodes_mesheid[nodeset.nodes[i]] = "front";
                break;
            
            case RearQuad:

                m_nodes_mesheid[nodeset.nodes[i]] = "rear";
                break;

            case LeftQuad:

                m_nodes_mesheid[nodeset.nodes[i]] = "left";
                break;

            case RightQuad:

                m_nodes_mesheid[nodeset.nodes[i]] = "right";
                break;

            case TopQuad:

                m_nodes_mesheid[nodeset.nodes[i]] = "top";
                break;

            case BottomQuad:

                m_nodes_mesheid[nodeset.nodes[i]] = "bottom";
                break;
        }
    }

    m_passesnodes[p_passname] = nodeset;
}

void Spacebox::GetNodeFromPass( const dsstring p_passname, const dsstring& p_nodeid )
{
    // TODO
}

