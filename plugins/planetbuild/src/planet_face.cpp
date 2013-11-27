/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2014
*
* This file is part of DrawSpace.
*
* DrawSpace is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* DrawSpace is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with DrawSpace. If not, see <http://www.gnu.org/licenses/>.
*
*/

#include "planet_face.h"
#include "memalloc.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;


Face::Face( PatchInstanciationHandler* p_inst_handler, PatchDeletionHandler* p_del_handler, PatchSplitHandler* p_split_handler, PatchMergeHandler* p_merge_handler ) : 
m_rootpatch( NULL ), 
m_inst_handler( p_inst_handler ), 
m_del_handler( p_del_handler ),
m_split_handler( p_split_handler ),
m_merge_handler( p_merge_handler ),
m_planet_diameter( 10.0 )
{
}

Face::~Face( void )
{
    _DRAWSPACE_DELETE_( m_rootpatch );
}

// create face's root patch
bool Face::Init( int p_orientation )
{    
    m_orientation = p_orientation;

    InstanciationCallback* cb_inst = _DRAWSPACE_NEW_( InstanciationCallback, InstanciationCallback( this, &Face::on_nodeinstanciation ) );
    DeletionCallback* cb_del = _DRAWSPACE_NEW_( DeletionCallback, DeletionCallback( this, &Face::on_nodedeletion ) );
    InstanciationCallback* cb_merge = _DRAWSPACE_NEW_( InstanciationCallback, InstanciationCallback( this, &Face::on_nodemerge ) );
    DeletionCallback* cb_split = _DRAWSPACE_NEW_( DeletionCallback, DeletionCallback( this, &Face::on_nodesplit ) );

    m_rootpatch = _DRAWSPACE_NEW_( QuadtreeNode<Patch>, QuadtreeNode<Patch>( cb_inst, cb_del, cb_split, cb_merge ) );

    return true;
}

Patch* Face::GetPatch( const dsstring& p_name )
{
    if( m_patches.count( p_name ) > 0 )
    {
        QuadtreeNode<Patch>* node = static_cast<QuadtreeNode<Patch>*>( m_patches[p_name] );
        return node->GetContent();
    }
    return NULL;
}


void Face::on_nodeinstanciation( BaseQuadtreeNode* p_node )
{
    if( NULL == m_rootpatch )
    {
        QuadtreeNode<Patch>* root = static_cast<QuadtreeNode<Patch>*>( p_node );

        Patch* patch = _DRAWSPACE_NEW_( Patch, Patch( patchresol, m_planet_diameter / 2.0, m_orientation, ".0", NULL, -1 ) );
        root->SetContent( patch );

        dsstring patch_name;
        patch->GetName( patch_name );        
        m_patches[patch_name] = p_node;

        (*m_inst_handler)( m_orientation, patch );        
    }
    else
    {
        QuadtreeNode<Patch>* node = static_cast<QuadtreeNode<Patch>*>( p_node );
        QuadtreeNode<Patch>* parent = static_cast<QuadtreeNode<Patch>*>( node->GetParent() );

        dsstring parent_name;
        parent->GetContent()->GetName( parent_name );

        dsstring node_name = parent_name + dsstring( "." );
        char dstbuf[32];
        node_name += dsstring( itoa( node->GetId(), dstbuf, 10 ) );

        Patch* patch = _DRAWSPACE_NEW_( Patch, Patch( patchresol, m_planet_diameter / 2.0, m_orientation, node_name, parent->GetContent(), node->GetId() ) );
        node->SetContent( patch );

        dsstring patch_name;
        patch->GetName( patch_name );
        m_patches[patch_name] = p_node;

        (*m_inst_handler)( m_orientation, patch );        
    }
}

void Face::on_nodedeletion( DrawSpace::Utils::BaseQuadtreeNode* p_node )
{    
    QuadtreeNode<Patch>* node = static_cast<QuadtreeNode<Patch>*>( p_node );
    
    Patch* patch = node->GetContent();

    (*m_del_handler)( m_orientation, patch );    

    dsstring patch_name;
    patch->GetName( patch_name );   
    m_patches.erase( patch_name );
    _DRAWSPACE_DELETE_( patch );    
}

void Face::on_nodesplit( DrawSpace::Utils::BaseQuadtreeNode* p_node )
{
    QuadtreeNode<Patch>* node = static_cast<QuadtreeNode<Patch>*>( p_node );
    Patch* patch = node->GetContent();
    (*m_split_handler)( m_orientation, patch );
}

void Face::on_nodemerge( DrawSpace::Utils::BaseQuadtreeNode* p_node )
{
    QuadtreeNode<Patch>* node = static_cast<QuadtreeNode<Patch>*>( p_node );
    Patch* patch = node->GetContent();
    (*m_merge_handler)( m_orientation, patch );
}


void Face::Split( const dsstring& p_name )
{
    if( m_patches.count( p_name ) > 0 )
    {
        m_patches[p_name]->Split();
    }
}

void Face::Merge( const dsstring& p_name )
{
    if( m_patches.count( p_name ) > 0 )
    {
        m_patches[p_name]->Merge();
    }
}

void Face::SetPlanetDiameter( dsreal p_diameter )
{
    m_planet_diameter = p_diameter;
}