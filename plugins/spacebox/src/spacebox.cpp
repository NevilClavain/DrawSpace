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
    Vertex v1, v2, v3, v4;

    for( long i = 0; i < 6; i++ )
    {
        m_meshes[i] = _DRAWSPACE_NEW_( Core::Meshe, Core::Meshe );
        switch( i )
        {
            case FrontQuad:

                v1.x = -0.5;
                v1.y = 0.5;
                v1.z = -0.5;
                v1.tu[0] = 0.0;
                v1.tv[0] = 0.0;

                v2.x = 0.5;
                v2.y = 0.5;
                v2.z = -0.5;
                v2.tu[0] = 1.0;
                v2.tv[0] = 0.0;

                v3.x = 0.5;
                v3.y = -0.5;
                v3.z = -0.5;
                v3.tu[0] = 1.0;
                v3.tv[0] = 1.0;

                v4.x = -0.5;
                v4.y = -0.5;
                v4.z = -0.5;
                v4.tu[0] = 0.0;
                v4.tv[0] = 1.0;
                break;

            case RearQuad:

                v1.x = 0.5;
                v1.y = 0.5;
                v1.z = 0.5;
                v1.tu[0] = 0.0;
                v1.tv[0] = 0.0;

                v2.x = -0.5;
                v2.y = 0.5;
                v2.z = 0.5;
                v2.tu[0] = 1.0;
                v2.tv[0] = 0.0;

                v3.x = -0.5;
                v3.y = -0.5;
                v3.z = 0.5;
                v3.tu[0] = 1.0;
                v3.tv[0] = 1.0;

                v4.x = 0.5;
                v4.y = -0.5;
                v4.z = 0.5;
                v4.tu[0] = 0.0;
                v4.tv[0] = 1.0;
                break;

            case LeftQuad:

                v1.x = -0.5;
                v1.y = 0.5;
                v1.z = 0.5;
                v1.tu[0] = 0.0;
                v1.tv[0] = 0.0;

                v2.x = -0.5;
                v2.y = 0.5;
                v2.z = -0.5;
                v2.tu[0] = 1.0;
                v2.tv[0] = 0.0;

                v3.x = -0.5;
                v3.y = -0.5;
                v3.z = -0.5;
                v3.tu[0] = 1.0;
                v3.tv[0] = 1.0;

                v4.x = -0.5;
                v4.y = -0.5;
                v4.z = 0.5;
                v4.tu[0] = 0.0;
                v4.tv[0] = 1.0;

                break;

            case RightQuad:

                v1.x = 0.5;
                v1.y = 0.5;
                v1.z = -0.5;
                v1.tu[0] = 0.0;
                v1.tv[0] = 0.0;

                v2.x = 0.5;
                v2.y = 0.5;
                v2.z = 0.5;
                v2.tu[0] = 1.0;
                v2.tv[0] = 0.0;

                v3.x = 0.5;
                v3.y = -0.5;
                v3.z = 0.5;
                v3.tu[0] = 1.0;
                v3.tv[0] = 1.0;

                v4.x = 0.5;
                v4.y = -0.5;
                v4.z = -0.5;
                v4.tu[0] = 0.0;
                v4.tv[0] = 1.0;

                break;

            case TopQuad:

                v1.x = -0.5;
                v1.y = 0.5;
                v1.z = 0.5;
                v1.tu[0] = 0.0;
                v1.tv[0] = 0.0;

                v2.x = 0.5;
                v2.y = 0.5;
                v2.z = 0.5;
                v2.tu[0] = 1.0;
                v2.tv[0] = 0.0;

                v3.x = 0.5;
                v3.y = 0.5;
                v3.z = -0.5;
                v3.tu[0] = 1.0;
                v3.tv[0] = 1.0;

                v4.x = -0.5;
                v4.y = 0.5;
                v4.z = -0.5;
                v4.tu[0] = 0.0;
                v4.tv[0] = 1.0;

                break;

            case BottomQuad:

                v1.x = -0.5;
                v1.y = -0.5;
                v1.z = -0.5;
                v1.tu[0] = 0.0;
                v1.tv[0] = 0.0;

                v2.x = 0.5;
                v2.y = -0.5;
                v2.z = -0.5;
                v2.tu[0] = 1.0;
                v2.tv[0] = 0.0;

                v3.x = 0.5;
                v3.y = -0.5;
                v3.z = 0.5;
                v3.tu[0] = 1.0;
                v3.tv[0] = 1.0;

                v4.x = -0.5;
                v4.y = -0.5;
                v4.z = 0.5;
                v4.tu[0] = 0.0;
                v4.tv[0] = 1.0;

                break;
        }

        m_meshes[i]->AddVertex( v1 );
        m_meshes[i]->AddVertex( v2 );
        m_meshes[i]->AddVertex( v3 );
        m_meshes[i]->AddVertex( v4 );

        m_meshes[i]->AddTriangle( Triangle( 0, 3, 1 ) );
        m_meshes[i]->AddTriangle( Triangle( 1, 3, 2 ) );
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
    for( std::map<dsstring, NodesSet>::iterator it = m_passesnodes.begin(); it != m_passesnodes.end(); ++it )
    {
        Pass* current_pass = p_scenegraph->GetPass( (*it).first );
        if( current_pass != NULL )
        {
            for( long i = 0; i < 6; i++ )
            {
                current_pass->GetRenderingQueue()->Add( (*it).second.nodes[i] );
            }
        }
    }
    m_scenegraph = p_scenegraph;
}

/*
bool Spacebox::LoadAssets( void )
{
    void* meshe_data;

    if( false == m_renderer->CreateMeshe( m_meshes[FrontQuad], &meshe_data ) )
    {
        return false;
    }
    m_meshe_datas["front"] = meshe_data;

    if( false == m_renderer->CreateMeshe( m_meshes[RearQuad], &meshe_data ) )
    {
        return false;
    }
    m_meshe_datas["rear"] = meshe_data;

    if( false == m_renderer->CreateMeshe( m_meshes[LeftQuad], &meshe_data ) )
    {
        return false;
    }
    m_meshe_datas["left"] = meshe_data;

    if( false == m_renderer->CreateMeshe( m_meshes[RightQuad], &meshe_data ) )
    {
        return false;
    }
    m_meshe_datas["right"] = meshe_data;

    if( false == m_renderer->CreateMeshe( m_meshes[TopQuad], &meshe_data ) )
    {
        return false;
    }
    m_meshe_datas["top"] = meshe_data;

    if( false == m_renderer->CreateMeshe( m_meshes[BottomQuad], &meshe_data ) )
    {
        return false;
    }
    m_meshe_datas["bottom"] = meshe_data;
   
    for( std::map<dsstring, NodesSet>::iterator it = m_passesnodes.begin(); it != m_passesnodes.end(); ++it )
    {
        for( long i = 0; i < 6; i++ )
        {            
		    if( false == m_renderer->CreateRenderingNode( (*it).second.nodes[i] ) )
		    {
			    return false;
		    }
        }
    }
    return true;
}
*/

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

    world = m_globaltransformation;
    world.ClearTranslation();
    m_scenegraph->GetCurrentCameraView( view );
    view.ClearTranslation();
  
    //m_renderer->RenderMeshe( world, view, m_meshe_datas[m_nodes_mesheid[p_rendering_node]] );    
    m_renderer->DrawMeshe( p_rendering_node->GetMeshe()->GetVertexListSize(), p_rendering_node->GetMeshe()->GetTrianglesListSize(), world, view );
}

void Spacebox::RegisterPassSlot( const dsstring p_passname )
{
    NodesSet nodeset;
    for( long i = 0; i < 6; i++ )
    {
        nodeset.nodes[i] = _DRAWSPACE_NEW_( RenderingNode, RenderingNode );
        RenderingNodeDrawCallback* cb = _DRAWSPACE_NEW_( RenderingNodeDrawCallback, RenderingNodeDrawCallback( this, &Spacebox::on_renderingnode_draw ) );

        nodeset.nodes[i]->SetMeshe( m_meshes[i] );

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

DrawSpace::Core::RenderingNode* Spacebox::GetNodeFromPass( const dsstring p_passname, const dsstring& p_nodeid )
{
    if( 0 == m_passesnodes.count( p_passname ) )
    {
        return NULL;
    }

    if( p_nodeid == "front" )
    {
        return m_passesnodes[p_passname].nodes[FrontQuad];
    }
    else if( p_nodeid == "rear" )
    {
        return m_passesnodes[p_passname].nodes[RearQuad];
    }
    else if( p_nodeid == "left" )
    {
        return m_passesnodes[p_passname].nodes[LeftQuad];
    }
    else if( p_nodeid == "right" )
    {
        return m_passesnodes[p_passname].nodes[RightQuad];
    }
    else if( p_nodeid == "top" )
    {
        return m_passesnodes[p_passname].nodes[TopQuad];
    }
    else if( p_nodeid == "bottom" )
    {
        return m_passesnodes[p_passname].nodes[BottomQuad];
    }
    
    return NULL;
}

void Spacebox::GetNodesIdsList( std::vector<dsstring>& p_ids )
{
    p_ids.push_back( "front" );
    p_ids.push_back( "rear" );
    p_ids.push_back( "top" );
    p_ids.push_back( "bottom" );
    p_ids.push_back( "left" );
    p_ids.push_back( "right" );
}

void Spacebox::ComputeSpecifics( void )
{
}

void Spacebox::GetPropertiesList( std::vector<dsstring>& p_props )
{
}

DrawSpace::Core::Property* Spacebox::GetProperty( const dsstring& p_name )
{
    return NULL;
}

void Spacebox::SetProperty( const dsstring& p_name, DrawSpace::Core::Property* p_prop )
{
}
