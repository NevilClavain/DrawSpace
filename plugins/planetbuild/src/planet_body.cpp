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

#include <renderer.h>
#include <plugin.h>
#include <memalloc.h>
#include <quadtree.h>
#include "planet_body.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;


FaceRenderingNode::FaceRenderingNode( Body* p_owner, Face* p_face_1, Face* p_face_2, DrawSpace::Interface::Renderer* p_renderer ) : 
m_renderer( p_renderer ),
m_face_1( p_face_1 ),
m_face_2( p_face_2 ),
m_owner( p_owner ),
m_current_quadtree( NONE_QUADTREE ),
m_last_current_quadtree( NONE_QUADTREE )
{
    m_patchinstanciationcallback = _DRAWSPACE_NEW_( PatchInstanciationCallback, PatchInstanciationCallback( this, &FaceRenderingNode::on_patchinstanciation ) );
    m_patchdelcallback = _DRAWSPACE_NEW_( PatchDelCallback, PatchDelCallback( this, &FaceRenderingNode::on_patchdel ) );
    m_patchsplitcallback = _DRAWSPACE_NEW_( PatchSplitCallback, PatchSplitCallback( this, &FaceRenderingNode::on_patchsplit ) );
    m_patchmergecallback = _DRAWSPACE_NEW_( PatchMergeCallback, PatchMergeCallback( this, &FaceRenderingNode::on_patchmerge ) );

    
    /*
    m_face->AddInstHandler( GetPatchInstanciationHandler() );
    m_face->AddDelHandler( GetPatchDelHandler() );
    m_face->AddSplitHandler( GetPatchSplitHandler() );
    m_face->AddMergeHandler( GetPatchMergeHandler() );
    */

    m_face_1->AddInstHandler( GetPatchInstanciationHandler() );
    m_face_1->AddDelHandler( GetPatchDelHandler() );
    m_face_1->AddSplitHandler( GetPatchSplitHandler() );
    m_face_1->AddMergeHandler( GetPatchMergeHandler() );

    m_face_2->AddInstHandler( GetPatchInstanciationHandler() );
    m_face_2->AddDelHandler( GetPatchDelHandler() );
    m_face_2->AddSplitHandler( GetPatchSplitHandler() );
    m_face_2->AddMergeHandler( GetPatchMergeHandler() );
}

FaceRenderingNode::~FaceRenderingNode( void )
{
}

void FaceRenderingNode::SetWorkingQuadtree( WorkingQuadtree p_working_quadtree )
{
    m_current_quadtree_mutex.WaitInfinite();
    m_current_quadtree = p_working_quadtree;
    m_last_current_quadtree = m_current_quadtree;
    m_current_quadtree_mutex.Release();
}

void FaceRenderingNode::Draw( const Matrix& p_world, Matrix& p_view )
{
    // get current quadtree to render

    WorkingQuadtree current_quadtree;
    Face* face;
    std::map<dsstring, Patch*>* leafs;

    if( m_current_quadtree_mutex.Wait( 0 ) )
    {
        current_quadtree = m_current_quadtree;
    }
    else
    {
        current_quadtree = m_last_current_quadtree;
    }
    m_current_quadtree_mutex.Release();

    if( NONE_QUADTREE == current_quadtree )
    {
        return;
    }
    else if( QUADTREE_1 == current_quadtree )
    {
        face = m_face_1;
        leafs = &m_patchesleafs_1;
    }
    else
    {
        face = m_face_2;
        leafs = &m_patchesleafs_2;
    }

    long currentleaf_depth = -1;   
    if( face->GetCurrentLeaf() )
    {
        currentleaf_depth = face->GetCurrentLeaf()->GetDepthLevel();
    }

    if( -1 == currentleaf_depth || currentleaf_depth < 4 )
    {
        for( std::map<dsstring, Patch*>::iterator it = leafs->begin(); it != leafs->end(); ++it )
        {                                             
            // rendu du patch leaf
            dsstring name;
            (*it).second->GetName( name );

            if( QUADTREE_1 == current_quadtree )
            {
                name = dsstring( "QUADTREE_1/" ) + name;
            }
            else
            {
                name = dsstring( "QUADTREE_2/" ) + name;
            }

            m_renderer->RenderNodeMeshe( p_world, p_view, this, name );
        }
    }
    else
    {
        dsstring name;

        QuadtreeNode<Patch>* current_leaf = face->GetCurrentLeaf();

        current_leaf->GetContent()->GetName( name );

        if( QUADTREE_1 == current_quadtree )
        {
            name = dsstring( "QUADTREE_1/" ) + name;
        }
        else
        {
            name = dsstring( "QUADTREE_2/" ) + name;
        }

        m_renderer->RenderNodeMeshe( p_world, p_view, this, name );

        for( long i = 0; i < 8;i++ )
        {
            QuadtreeNode<Patch>* neighb = static_cast<QuadtreeNode<Patch>*>( current_leaf->GetContent()->GetNeighbour( i ) );

            if( neighb )
            {
                neighb->GetContent()->GetName( name );

                if( QUADTREE_1 == current_quadtree )
                {
                    name = dsstring( "QUADTREE_1/" ) + name;
                }
                else
                {
                    name = dsstring( "QUADTREE_2/" ) + name;
                }

                m_renderer->RenderNodeMeshe( p_world, p_view, this, name );
            }
        }
    }



/*
    long currentleaf_depth = -1;   
    if( m_face->GetCurrentLeaf() )
    {
        currentleaf_depth = m_face->GetCurrentLeaf()->GetDepthLevel();
    }

    if( -1 == currentleaf_depth || currentleaf_depth < 4 )
    {
        for( std::map<dsstring, Patch*>::iterator it = m_patchesleafs.begin(); it != m_patchesleafs.end(); ++it )
        {                                             
            // rendu du patch leaf
            dsstring name;
            (*it).second->GetName( name );
            m_renderer->RenderNodeMeshe( p_world, p_view, this, name );
        }
    }
    else
    {
        dsstring name;

        QuadtreeNode<Patch>* current_leaf = m_face->GetCurrentLeaf();

        current_leaf->GetContent()->GetName( name );
        m_renderer->RenderNodeMeshe( p_world, p_view, this, name );

        for( long i = 0; i < 8;i++ )
        {
            QuadtreeNode<Patch>* neighb = static_cast<QuadtreeNode<Patch>*>( current_leaf->GetContent()->GetNeighbour( i ) );

            if( neighb )
            {
                neighb->GetContent()->GetName( name );
                m_renderer->RenderNodeMeshe( p_world, p_view, this, name );
            }
        }
    }
    */
}

void FaceRenderingNode::complete_patchname( dsstring& p_patchname )
{
    if( QUADTREE_1 == m_owner->GetWorkingQuadtree() )
    {
        p_patchname = dsstring( "QUADTREE_1/" ) + p_patchname;
    }
    else
    {
        p_patchname = dsstring( "QUADTREE_2/" ) + p_patchname;
    }
}

bool FaceRenderingNode::is_working_quadtree1( void )
{
    if( QUADTREE_1 == m_owner->GetWorkingQuadtree() )
    {
        return true;
    }

    return false;
}

void FaceRenderingNode::on_patchinstanciation( int p_orientation, Patch* p_patch )
{
    dsstring patch_name;
    p_patch->GetName( patch_name );
    complete_patchname( patch_name );

    m_renderer->AddMesheToNode( p_patch, this, patch_name );

    if( is_working_quadtree1() )
    {
        m_patchesleafs_1[patch_name] = p_patch;
    }
    else
    {
        m_patchesleafs_2[patch_name] = p_patch;
    }
}

void FaceRenderingNode::on_patchdel( int p_orientation, Patch* p_patch )
{
    dsstring patch_name;
    p_patch->GetName( patch_name );
    complete_patchname( patch_name );

    /*
    if( m_patchesleafs.count( patch_name ) > 0 )
    {
        m_patchesleafs.erase( patch_name );
    }
    */

    if( is_working_quadtree1() )
    {
        if( m_patchesleafs_1.count( patch_name ) > 0 )
        {
            m_patchesleafs_1.erase( patch_name );
        }
    }
    else
    {
        if( m_patchesleafs_2.count( patch_name ) > 0 )
        {
            m_patchesleafs_2.erase( patch_name );
        }
    }

    m_renderer->RemoveNodeMeshe( p_patch, this, patch_name );
}

void FaceRenderingNode::on_patchsplit( int p_orientation, Patch* p_patch )
{
    dsstring patch_name;
    p_patch->GetName( patch_name );
    complete_patchname( patch_name );

    /*
    if( m_patchesleafs.count( patch_name ) > 0 )
    {
        m_patchesleafs.erase( patch_name );
    }
    */

    if( is_working_quadtree1() )
    {
        if( m_patchesleafs_1.count( patch_name ) > 0 )
        {
            m_patchesleafs_1.erase( patch_name );
        }
    }
    else
    {
        if( m_patchesleafs_2.count( patch_name ) > 0 )
        {
            m_patchesleafs_2.erase( patch_name );
        }
    }
}

void FaceRenderingNode::on_patchmerge( int p_orientation, Patch* p_patch )
{
    dsstring patch_name;
    p_patch->GetName( patch_name );
    complete_patchname( patch_name );

    //m_patchesleafs[patch_name] = p_patch;

    if( is_working_quadtree1() )
    {
        m_patchesleafs_1[patch_name] = p_patch;
    }
    else
    {
        m_patchesleafs_2[patch_name] = p_patch;
    }
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
m_diameter( "diameter" ),
m_hotpoint( "hotpoint" ),
m_relative_hotpoint( "relative_hotpoint" ),
m_altitud( "altitud" ),
m_update_state( "update_state" ),
m_working_quadtree( QUADTREE_1 )
{
    for( long i = 0; i < 6; i++ )
    {
        //m_faces[i] = _DRAWSPACE_NEW_( Face, Face );

        m_faces_1[i] = _DRAWSPACE_NEW_( Face, Face );
        m_faces_2[i] = _DRAWSPACE_NEW_( Face, Face );
    }

    m_diameter.m_value = 10.0;
    m_altitud.m_value = -1.0;
    m_update_state.m_value = false;

    m_update_task = _DRAWSPACE_NEW_( Task<Body>, Task<Body>( Task<Body>::Block ) );
}

Body::~Body( void )
{
    _DRAWSPACE_DELETE_( m_update_task );

    for( long i = 0; i < 6; i++ )
    {
        //_DRAWSPACE_DELETE_( m_faces[i] );

        _DRAWSPACE_DELETE_( m_faces_1[i] );
        _DRAWSPACE_DELETE_( m_faces_2[i] );
    }
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
            (*it).second.nodes[i]->SetWorkingQuadtree( QUADTREE_2 );

			if( false == m_renderer->CreateRenderingNode( (*it).second.nodes[i] ) )
			{
				return false;
			}
		}
	}

    m_working_quadtree = QUADTREE_1;
    for( long i = 0; i < 6; i++ )
    {
        //m_faces[i]->Init( i );

        m_faces_1[i]->Init( i );
        m_faces_2[i]->Init( i );
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

        //return m_faces[faceidval]->GetPatch( patch_name );
        return m_faces_1[faceidval]->GetPatch( patch_name );
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
        nodeset.nodes[i] = _DRAWSPACE_NEW_( FaceRenderingNode, FaceRenderingNode( this, m_faces_1[i], m_faces_2[i], m_renderer ) );

        RenderingNodeDrawCallback* cb = _DRAWSPACE_NEW_( RenderingNodeDrawCallback, RenderingNodeDrawCallback( this, &Body::on_renderingnode_draw ) );
        nodeset.nodes[i]->RegisterHandler( cb );
        m_callbacks.push_back( cb );
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
}

void Body::GetPropertiesList( std::vector<dsstring>& p_props )
{
    dsstring name;

    m_diameter.GetName( name );
    p_props.push_back( name );

    m_hotpoint.GetName( name );
    p_props.push_back( name );

    m_relative_hotpoint.GetName( name );
    p_props.push_back( name );

    m_altitud.GetName( name );
    p_props.push_back( name );

    m_update_state.GetName( name );
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

    m_hotpoint.GetName( name );
    if( p_name == name )
    {
        return &m_hotpoint;
    }

    m_relative_hotpoint.GetName( name );
    if( p_name == name )
    {
        return &m_relative_hotpoint;
    }

    m_altitud.GetName( name );
    if( p_name == name )
    {
        return &m_altitud;
    }

    m_update_state.GetName( name );
    if( p_name == name )
    {
        return &m_update_state;
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
            if( m_faces_1[i] != NULL )
            {
                m_faces_1[i]->SetPlanetDiameter( m_diameter.m_value );
            }

            if( m_faces_2[i] != NULL )
            {
                m_faces_2[i]->SetPlanetDiameter( m_diameter.m_value );
            }
        }
    }
    m_hotpoint.GetName( name );
    if( p_name == name )
    {
        TypedProperty<Vector>* hotpoint = static_cast<TypedProperty<Vector>*>( p_prop );
        m_hotpoint.m_value = hotpoint->m_value;

        Matrix inv = m_globaltransformation;
        inv.Inverse();
        inv.Transform( &m_hotpoint.m_value, &m_relative_hotpoint.m_value );

        for( long i = 0; i < 6; i++ )
        {
            if( QUADTREE_1 == m_working_quadtree )
            {
                if( m_faces_1[i] != NULL )
                {
                    m_faces_1[i]->UpdateRelativeHotpoint( m_relative_hotpoint.m_value );
                }                
            }
            else // QUADTREE_2
            {
                if( m_faces_1[i] != NULL )
                {
                    m_faces_1[i]->UpdateRelativeHotpoint( m_relative_hotpoint.m_value );
                }                
            }
        }

        // compute altitud
        m_altitud.m_value = m_relative_hotpoint.m_value.Length() - ( m_diameter.m_value / 2.0 );
    }

    m_update_state.GetName( name );
    if( p_name == name )
    {
        TypedProperty<bool>* state = static_cast<TypedProperty<bool>*>( p_prop );
        m_update_state.m_value = state->m_value;

        if( m_update_state.m_value )
        {
            start_update();
        }
        else
        {
            stop_update();
        }
    }
}

void Body::start_update( void )
{
    m_stop_thread = false;
    m_update_task->Startup( this );
}

void Body::stop_update( void )
{
    m_stop_thread = true;
}

void Body::Run( void )
{ 
    while( !m_stop_thread )
    {
        if( QUADTREE_1 == m_working_quadtree )
        {
            if( m_faces_1[0]->Compute() )
            {
                // signaler le switch
                m_working_quadtree = QUADTREE_2;

	            for( std::map<dsstring, NodesSet>::iterator it = m_passesnodes.begin(); it != m_passesnodes.end(); ++it )
	            {
		            for( long i = 0; i < 6; i++ )
		            {
			            (*it).second.nodes[i]->SetWorkingQuadtree( QUADTREE_1 );
		            }
	            }

            }           
        }
        else // QUADTREE_2
        {
            if( m_faces_2[0]->Compute() )
            {
                // signaler le switch
                m_working_quadtree = QUADTREE_1;

	            for( std::map<dsstring, NodesSet>::iterator it = m_passesnodes.begin(); it != m_passesnodes.end(); ++it )
	            {
		            for( long i = 0; i < 6; i++ )
		            {
			            (*it).second.nodes[i]->SetWorkingQuadtree( QUADTREE_2 );
		            }
	            }
            }
        }
        Sleep( 2000 );
    }
}

WorkingQuadtree Body::GetWorkingQuadtree( void )
{
    return m_working_quadtree;
}

