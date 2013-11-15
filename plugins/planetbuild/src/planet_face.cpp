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


Face::Face( PatchInstanciationHandler* p_inst_handler ): m_rootpatch( NULL ), m_inst_handler( p_inst_handler )
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

    InstanciationCallback* cb = _DRAWSPACE_NEW_( InstanciationCallback, InstanciationCallback( this, &Face::on_nodeinstanciation ) );
    m_rootpatch = _DRAWSPACE_NEW_( QuadtreeNode<Patch>, QuadtreeNode<Patch>( cb ) );

    return true;
}

Patch* Face::GetPatch( const dsstring& p_name )
{
    if( m_patches.count( p_name ) > 0 )
    {
        return m_patches[p_name];
    }
    return NULL;
}


void Face::on_nodeinstanciation( BaseQuadtreeNode* p_node )
{
    if( NULL == m_rootpatch )
    {
        QuadtreeNode<Patch>* root = static_cast<QuadtreeNode<Patch>*>( p_node );

        Patch* patch = _DRAWSPACE_NEW_( Patch, Patch( 9, 5.0, m_orientation, ".0", NULL, -1 ) );
        root->SetContent( patch );

        dsstring patch_name;
        patch->GetName( patch_name );
        m_patches[patch_name] = patch;

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

        Patch* patch = _DRAWSPACE_NEW_( Patch, Patch( 9, 5.0, m_orientation, node_name, parent->GetContent(), node->GetId() ) );
        node->SetContent( patch );

        dsstring patch_name;
        patch->GetName( patch_name );
        m_patches[patch_name] = patch;

        (*m_inst_handler)( m_orientation, patch );
    }
}