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

#include "planet_body.h"
#include "renderer.h"
#include "plugin.h"
#include "memalloc.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;


Body::Body( void ) : m_renderer( NULL ), m_scenegraph( NULL )
{
    PatchInstanciationCallback* cb = _DRAWSPACE_NEW_( PatchInstanciationCallback, PatchInstanciationCallback( this, &Body::on_patchinstanciation ) );

    for( long i = 0; i < 6; i++ )
    {
        m_faces[i] = _DRAWSPACE_NEW_( Face, Face( cb ) );
    }
}

Body::~Body( void )
{
}

void Body::GetDescr( dsstring& p_descr )
{
    p_descr = "planet build";
}

void Body::DumpMemoryAllocs( void )
{
    DrawSpace::Utils::MemAlloc::GetInstance()->DumpContent();
}

void Body::SetRenderer( DrawSpace::Interface::Renderer * p_renderer )
{
    m_renderer = p_renderer;
}

void Body::OnRegister( DrawSpace::Scenegraph* p_scenegraph )
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

bool Body::LoadAssets( void )
{
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
    for( long i = 0; i < 6; i++ )
    {
        m_faces[i]->Init( i );
    }

	return true;
}

DrawSpace::Core::Meshe* Body::GetMeshe( const dsstring& p_mesheid )
{
    size_t sep = p_mesheid.find_first_of( ":" );
    if( sep != dsstring::npos && sep > 0 && sep < p_mesheid.length() - 2 )
    {
        dsstring face_id = p_mesheid.substr( 0, sep );
        dsstring patch_name = p_mesheid.substr( sep + 1, p_mesheid.length() - ( sep + 1 ) );

        int faceidval;

        if( "front" == face_id )
        {
            faceidval = Patch::FrontPlanetFace;
        }
        else if( "rear" == face_id )
        {
            faceidval = Patch::RearPlanetFace;
        }
        else if( "top" == face_id )
        {
            faceidval = Patch::TopPlanetFace;
        }
        else if( "bottom" == face_id )
        {
            faceidval = Patch::BottomPlanetFace;
        }
        else if( "left" == face_id )
        {
            faceidval = Patch::LeftPlanetFace;
        }
        else if( "right" == face_id )
        {
            faceidval = Patch::RightPlanetFace;
        }

        // TO BE CONTINUED .....

    }
    return NULL;
}

void Body::on_renderingnode_draw( Core::RenderingNode* p_rendering_node )
{
    /*
	DrawSpace::Utils::Matrix view;
	m_scenegraph->GetCurrentCameraView( view );

	Face* face = static_cast<Face*>( p_rendering_node );
	face->Draw( m_globaltransformation, view );
    */
}

void Body::on_patchinstanciation( int p_orientation, Patch* p_patch )
{
    dsstring patch_name;
    p_patch->GetName( patch_name );

	for( std::map<dsstring, NodesSet>::iterator it = m_passesnodes.begin(); it != m_passesnodes.end(); ++it )
	{
        m_renderer->AddMesheToNode( p_patch, (*it).second.nodes[p_orientation], patch_name );
    }    
}

void Body::RegisterPassSlot( const dsstring p_passname )
{
    NodesSet nodeset;
    for( long i = 0; i < 6; i++ )
    {
        nodeset.nodes[i] = _DRAWSPACE_NEW_( RenderingNode, RenderingNode );
        RenderingNodeDrawCallback* cb = _DRAWSPACE_NEW_( RenderingNodeDrawCallback, RenderingNodeDrawCallback( this, &Body::on_renderingnode_draw ) );

        nodeset.nodes[i]->RegisterHandler( cb );
        m_callbacks.push_back( cb );
    }
}

DrawSpace::Core::RenderingNode* Body::GetNodeFromPass( const dsstring p_passname, const dsstring& p_nodeid )
{
	if( 0 == m_passesnodes.count( p_passname ) )
	{
		return NULL;
	}
	NodesSet nodeset = m_passesnodes[p_passname];
	
    int faceid;

    if( "front" == p_nodeid )
    {
        faceid = Patch::FrontPlanetFace;
    }
    else if( "rear" == p_nodeid )
    {
        faceid = Patch::RearPlanetFace;
    }
    else if( "top" == p_nodeid )
    {
        faceid = Patch::TopPlanetFace;
    }
    else if( "bottom" == p_nodeid )
    {
        faceid = Patch::BottomPlanetFace;
    }
    else if( "left" == p_nodeid )
    {
        faceid = Patch::LeftPlanetFace;
    }
    else if( "right" == p_nodeid )
    {
        faceid = Patch::RightPlanetFace;
    }
    return nodeset.nodes[faceid];
}