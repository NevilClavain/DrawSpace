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
#include "quadtree.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;


FaceRenderingNode::FaceRenderingNode( DrawSpace::Interface::Renderer* p_renderer ) : m_renderer( p_renderer )
{
    m_patchinstanciationcallback = _DRAWSPACE_NEW_( PatchInstanciationCallback, PatchInstanciationCallback( this, &FaceRenderingNode::on_patchinstanciation ) );
    m_patchdelcallback = _DRAWSPACE_NEW_( PatchDelCallback, PatchDelCallback( this, &FaceRenderingNode::on_patchdel ) );
    m_patchsplitcallback = _DRAWSPACE_NEW_( PatchSplitCallback, PatchSplitCallback( this, &FaceRenderingNode::on_patchsplit ) );
    m_patchmergecallback = _DRAWSPACE_NEW_( PatchMergeCallback, PatchMergeCallback( this, &FaceRenderingNode::on_patchmerge ) );
}

FaceRenderingNode::~FaceRenderingNode( void )
{
}

void FaceRenderingNode::Draw( const DrawSpace::Utils::Matrix& p_world, DrawSpace::Utils::Matrix& p_view )
{
    for( std::map<dsstring, Patch*>::iterator it = m_patchesleafs.begin(); it != m_patchesleafs.end(); ++it )
    {
        // rendu du patch leaf
        dsstring name;
        (*it).second->GetName( name );
        m_renderer->RenderNodeMeshe( p_world, p_view, this, name );
    }
}

void FaceRenderingNode::on_patchinstanciation( int p_orientation, Patch* p_patch )
{
    dsstring patch_name;
    p_patch->GetName( patch_name );
    m_renderer->AddMesheToNode( p_patch, this, patch_name );

    m_patchesleafs[patch_name] = p_patch;
    m_patches[patch_name] = p_patch;
}

void FaceRenderingNode::on_patchdel( int p_orientation, Patch* p_patch )
{
    dsstring patch_name;
    p_patch->GetName( patch_name );

    if( m_patchesleafs.count( patch_name ) > 0 )
    {
        m_patchesleafs.erase( patch_name );
    }
    if( m_patches.count( patch_name ) > 0 )
    {
        m_patches.erase( patch_name );
    }

    m_renderer->RemoveNodeMeshe( p_patch, this, patch_name );
}

void FaceRenderingNode::on_patchsplit( int p_orientation, Patch* p_patch )
{
    dsstring patch_name;
    p_patch->GetName( patch_name );
    if( m_patchesleafs.count( patch_name ) > 0 )
    {
        m_patchesleafs.erase( patch_name );
    }
}

void FaceRenderingNode::on_patchmerge( int p_orientation, Patch* p_patch )
{
    dsstring patch_name;
    p_patch->GetName( patch_name );

    m_patchesleafs[patch_name] = p_patch;
}

Face::PatchInstanciationHandler* FaceRenderingNode::GetPatchInstanciationHandler( void )
{
    return m_patchinstanciationcallback;
}

Face::PatchDeletionHandler* FaceRenderingNode::GetPatchDelHandler( void )
{
    return m_patchdelcallback;
}

Face::PatchSplitHandler* FaceRenderingNode::GetPatchSplitHandler( void )
{
    return m_patchsplitcallback;
}

Face::PatchMergeHandler* FaceRenderingNode::GetPatchMergeHandler( void )
{
    return m_patchmergecallback;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Body::Body( void ) : 
m_renderer( NULL ), 
m_scenegraph( NULL ),
m_diameter( "diameter" )
{
    for( long i = 0; i < 6; i++ )
    {
        m_faces[i] = NULL;
    }

    m_diameter.m_value = 10.0;
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
        else
        {
            return NULL;
        }

        return m_faces[faceidval]->GetPatch( patch_name );
    }
    return NULL;
}

void Body::on_renderingnode_draw( Core::RenderingNode* p_rendering_node )
{
	DrawSpace::Utils::Matrix view;
	m_scenegraph->GetCurrentCameraView( view );

    FaceRenderingNode* face_node = static_cast<FaceRenderingNode*>( p_rendering_node );
    face_node->Draw( m_globaltransformation, view );
}

void Body::RegisterPassSlot( const dsstring p_passname )
{
    NodesSet nodeset;
    for( long i = 0; i < 6; i++ )
    {
        nodeset.nodes[i] = _DRAWSPACE_NEW_( FaceRenderingNode, FaceRenderingNode( m_renderer ) );

        RenderingNodeDrawCallback* cb = _DRAWSPACE_NEW_( RenderingNodeDrawCallback, RenderingNodeDrawCallback( this, &Body::on_renderingnode_draw ) );
        nodeset.nodes[i]->RegisterHandler( cb );
        m_callbacks.push_back( cb );

        m_faces[i] = _DRAWSPACE_NEW_( Face, Face( nodeset.nodes[i]->GetPatchInstanciationHandler(), 
                                                    nodeset.nodes[i]->GetPatchDelHandler(),                                                    
                                                    nodeset.nodes[i]->GetPatchSplitHandler(),
                                                    nodeset.nodes[i]->GetPatchMergeHandler() ) );

        m_faces[i]->SetPlanetDiameter( m_diameter.m_value );
    }
    m_passesnodes[p_passname] = nodeset;
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

void Body::GetNodesIdsList( std::vector<dsstring>& p_ids )
{
    p_ids.push_back( "front" );
    p_ids.push_back( "rear" );
    p_ids.push_back( "top" );
    p_ids.push_back( "bottom" );
    p_ids.push_back( "left" );
    p_ids.push_back( "right" );
}

void Body::ComputeSpecifics( void )
{
    // PROVISOIRE TESTS

    static int foo;

    if( 0 == foo )
    {
        m_faces[Patch::FrontPlanetFace]->Split( ".0" );
        m_faces[Patch::TopPlanetFace]->Split( ".0" );
        m_faces[Patch::TopPlanetFace]->Split( ".0.3" );
        m_faces[Patch::TopPlanetFace]->Split( ".0.3.1" );
        m_faces[Patch::TopPlanetFace]->Split( ".0.3.1.2" );
    }
    else if( 1 == foo )
    {
        m_faces[Patch::FrontPlanetFace]->Merge( ".0" );
    }

    foo++;
}

void Body::GetPropertiesList( std::vector<dsstring>& p_props )
{
    dsstring name;

    m_diameter.GetName( name );
    p_props.push_back( name );
}

Property* Body::GetProperty( const dsstring& p_name )
{
    dsstring name;

    m_diameter.GetName( name );
    if( p_name == name )
    {
        return &m_diameter;
    }

    return NULL;
}

void Body::SetProperty( const dsstring& p_name, Property* p_prop )
{
    dsstring name;

    m_diameter.GetName( name );
    if( p_name == name )
    {
        TypedProperty<dsreal>* input_diameter = static_cast<TypedProperty<dsreal>*>( p_prop );
        m_diameter.m_value = input_diameter->m_value;

        for( long i = 0; i < 6; i++ )
        {
            if( m_faces[i] != NULL )
            {
                m_faces[i]->SetPlanetDiameter( m_diameter.m_value );
            }
        }
    }
}
