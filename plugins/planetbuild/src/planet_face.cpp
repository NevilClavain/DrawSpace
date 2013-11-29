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

    QuadtreeNode<Patch>* nw_child_node = static_cast<QuadtreeNode<Patch>*>( node->GetChild( BaseQuadtreeNode::NorthWestNode ) );
    QuadtreeNode<Patch>* ne_child_node = static_cast<QuadtreeNode<Patch>*>( node->GetChild( BaseQuadtreeNode::NorthEastNode ) );
    QuadtreeNode<Patch>* se_child_node = static_cast<QuadtreeNode<Patch>*>( node->GetChild( BaseQuadtreeNode::SouthEastNode ) );
    QuadtreeNode<Patch>* sw_child_node = static_cast<QuadtreeNode<Patch>*>( node->GetChild( BaseQuadtreeNode::SouthWestNode ) );

    nw_child_node->GetContent()->SetNeighbour( ne_child_node, Patch::EastNeighbour );
    nw_child_node->GetContent()->SetNeighbour( sw_child_node, Patch::SouthNeighbour );
    nw_child_node->GetContent()->SetNeighbour( se_child_node, Patch::SouthEastNeighbour );

    ne_child_node->GetContent()->SetNeighbour( nw_child_node, Patch::WestNeighbour );
    ne_child_node->GetContent()->SetNeighbour( sw_child_node, Patch::SouthWestNeighbour );
    ne_child_node->GetContent()->SetNeighbour( se_child_node, Patch::SouthNeighbour );

    se_child_node->GetContent()->SetNeighbour( nw_child_node, Patch::NorthWestNeighbour );
    se_child_node->GetContent()->SetNeighbour( ne_child_node, Patch::NorthNeighbour );
    se_child_node->GetContent()->SetNeighbour( sw_child_node, Patch::WestNeighbour );

    sw_child_node->GetContent()->SetNeighbour( nw_child_node, Patch::NorthNeighbour );
    sw_child_node->GetContent()->SetNeighbour( ne_child_node, Patch::NorthEastNeighbour );
    sw_child_node->GetContent()->SetNeighbour( se_child_node, Patch::EastNeighbour );

    set_border_neighbours( nw_child_node );
    set_border_neighbours( ne_child_node );
    set_border_neighbours( se_child_node );
    set_border_neighbours( sw_child_node );

    (*m_split_handler)( m_orientation, patch );
}

void Face::set_border_neighbours( QuadtreeNode<Patch>* p_node )
{
    QuadtreeNode<Patch>* parent;
    BaseQuadtreeNode* parent_neighb;

    parent = static_cast<QuadtreeNode<Patch>*>( p_node->GetParent() );

    switch( p_node->GetId() )
    {
        case BaseQuadtreeNode::NorthWestNode:
            {
                parent_neighb = parent->GetContent()->GetNeighbour( Patch::NorthNeighbour );
                if( parent_neighb && parent_neighb->HasChildren() )
                {
                    p_node->GetContent()->SetNeighbour( parent_neighb->GetChild( BaseQuadtreeNode::SouthWestNode ), Patch::NorthNeighbour );
                    p_node->GetContent()->SetNeighbour( parent_neighb->GetChild( BaseQuadtreeNode::SouthEastNode ), Patch::NorthEastNeighbour );

                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::SouthWestNode ) )->GetContent()->SetNeighbour( p_node, Patch::SouthNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::SouthEastNode ) )->GetContent()->SetNeighbour( p_node, Patch::SouthWestNeighbour );
                }

                parent_neighb = parent->GetContent()->GetNeighbour( Patch::WestNeighbour );
                if( parent_neighb && parent_neighb->HasChildren() )
                {
                    p_node->GetContent()->SetNeighbour( parent_neighb->GetChild( BaseQuadtreeNode::NorthEastNode ), Patch::WestNeighbour );
                    p_node->GetContent()->SetNeighbour( parent_neighb->GetChild( BaseQuadtreeNode::SouthEastNode ), Patch::SouthWestNeighbour );

                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::NorthEastNode ) )->GetContent()->SetNeighbour( p_node, Patch::EastNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::SouthEastNode ) )->GetContent()->SetNeighbour( p_node, Patch::NorthEastNeighbour );
                }

                parent_neighb = parent->GetContent()->GetNeighbour( Patch::NorthWestNeighbour );
                if( parent_neighb && parent_neighb->HasChildren() )
                {
                    p_node->GetContent()->SetNeighbour( parent_neighb->GetChild( BaseQuadtreeNode::SouthEastNode ), Patch::NorthWestNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::SouthEastNode ) )->GetContent()->SetNeighbour( p_node, Patch::SouthEastNeighbour );
                }
            }
            break;

        case BaseQuadtreeNode::NorthEastNode:
            {
                parent_neighb = parent->GetContent()->GetNeighbour( Patch::NorthNeighbour );
                if( parent_neighb && parent_neighb->HasChildren() )
                {
                    p_node->GetContent()->SetNeighbour( parent_neighb->GetChild( BaseQuadtreeNode::SouthEastNode ), Patch::NorthNeighbour );
                    p_node->GetContent()->SetNeighbour( parent_neighb->GetChild( BaseQuadtreeNode::SouthWestNode ), Patch::NorthWestNeighbour );

                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::SouthEastNode ) )->GetContent()->SetNeighbour( p_node, Patch::SouthNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::SouthWestNode ) )->GetContent()->SetNeighbour( p_node, Patch::SouthEastNeighbour );
                }

                parent_neighb = parent->GetContent()->GetNeighbour( Patch::EastNeighbour );
                if( parent_neighb && parent_neighb->HasChildren() )
                {
                    p_node->GetContent()->SetNeighbour( parent_neighb->GetChild( BaseQuadtreeNode::NorthWestNode ), Patch::EastNeighbour );
                    p_node->GetContent()->SetNeighbour( parent_neighb->GetChild( BaseQuadtreeNode::SouthWestNode ), Patch::SouthEastNeighbour );

                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::NorthWestNode ) )->GetContent()->SetNeighbour( p_node, Patch::WestNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::SouthWestNode ) )->GetContent()->SetNeighbour( p_node, Patch::NorthWestNeighbour );
                }

                parent_neighb = parent->GetContent()->GetNeighbour( Patch::NorthEastNeighbour );
                if( parent_neighb && parent_neighb->HasChildren() )
                {
                    p_node->GetContent()->SetNeighbour( parent_neighb->GetChild( BaseQuadtreeNode::SouthWestNode ), Patch::NorthEastNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::SouthWestNode ) )->GetContent()->SetNeighbour( p_node, Patch::SouthWestNeighbour );
                }
            }
            break;

        case BaseQuadtreeNode::SouthEastNode:
            {
                parent_neighb = parent->GetContent()->GetNeighbour( Patch::EastNeighbour );
                if( parent_neighb && parent_neighb->HasChildren() )
                {
                    p_node->GetContent()->SetNeighbour( parent_neighb->GetChild( BaseQuadtreeNode::SouthWestNode ), Patch::EastNeighbour );
                    p_node->GetContent()->SetNeighbour( parent_neighb->GetChild( BaseQuadtreeNode::NorthWestNode ), Patch::NorthEastNeighbour );

                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::SouthWestNode ) )->GetContent()->SetNeighbour( p_node, Patch::WestNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::NorthWestNode ) )->GetContent()->SetNeighbour( p_node, Patch::SouthWestNeighbour );
                }

                parent_neighb = parent->GetContent()->GetNeighbour( Patch::SouthNeighbour );
                if( parent_neighb && parent_neighb->HasChildren() )
                {
                    p_node->GetContent()->SetNeighbour( parent_neighb->GetChild( BaseQuadtreeNode::NorthEastNode ), Patch::SouthNeighbour );
                    p_node->GetContent()->SetNeighbour( parent_neighb->GetChild( BaseQuadtreeNode::NorthWestNode ), Patch::SouthWestNeighbour );

                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::NorthEastNode ) )->GetContent()->SetNeighbour( p_node, Patch::NorthNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::NorthWestNode ) )->GetContent()->SetNeighbour( p_node, Patch::NorthEastNeighbour );
                }

                parent_neighb = parent->GetContent()->GetNeighbour( Patch::SouthEastNeighbour );
                if( parent_neighb && parent_neighb->HasChildren() )
                {
                    p_node->GetContent()->SetNeighbour( parent_neighb->GetChild( BaseQuadtreeNode::NorthWestNode ), Patch::SouthEastNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::NorthWestNode ) )->GetContent()->SetNeighbour( p_node, Patch::NorthWestNeighbour );
                }
            }
            break;

        case BaseQuadtreeNode::SouthWestNode:
            {
                parent_neighb = parent->GetContent()->GetNeighbour( Patch::WestNeighbour );
                if( parent_neighb && parent_neighb->HasChildren() )
                {
                    p_node->GetContent()->SetNeighbour( parent_neighb->GetChild( BaseQuadtreeNode::SouthEastNode ), Patch::WestNeighbour );
                    p_node->GetContent()->SetNeighbour( parent_neighb->GetChild( BaseQuadtreeNode::NorthEastNode ), Patch::NorthWestNeighbour );

                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::SouthEastNode ) )->GetContent()->SetNeighbour( p_node, Patch::EastNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::NorthEastNode ) )->GetContent()->SetNeighbour( p_node, Patch::SouthEastNeighbour );
                }

                parent_neighb = parent->GetContent()->GetNeighbour( Patch::SouthNeighbour );
                if( parent_neighb && parent_neighb->HasChildren() )
                {
                    p_node->GetContent()->SetNeighbour( parent_neighb->GetChild( BaseQuadtreeNode::NorthWestNode ), Patch::SouthNeighbour );
                    p_node->GetContent()->SetNeighbour( parent_neighb->GetChild( BaseQuadtreeNode::NorthEastNode ), Patch::SouthEastNeighbour );

                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::NorthWestNode ) )->GetContent()->SetNeighbour( p_node, Patch::NorthNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::NorthEastNode ) )->GetContent()->SetNeighbour( p_node, Patch::NorthWestNeighbour );
                }

                parent_neighb = parent->GetContent()->GetNeighbour( Patch::SouthWestNeighbour );
                if( parent_neighb && parent_neighb->HasChildren() )
                {
                    p_node->GetContent()->SetNeighbour( parent_neighb->GetChild( BaseQuadtreeNode::NorthEastNode ), Patch::SouthWestNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::NorthEastNode ) )->GetContent()->SetNeighbour( p_node, Patch::NorthEastNeighbour );
                }
            }
            break;
    }
}

void Face::on_nodemerge( DrawSpace::Utils::BaseQuadtreeNode* p_node )
{
    QuadtreeNode<Patch>* node = static_cast<QuadtreeNode<Patch>*>( p_node );
    Patch* patch = node->GetContent();

    QuadtreeNode<Patch>* nw_child_node = static_cast<QuadtreeNode<Patch>*>( node->GetChild( BaseQuadtreeNode::NorthWestNode ) );
    QuadtreeNode<Patch>* ne_child_node = static_cast<QuadtreeNode<Patch>*>( node->GetChild( BaseQuadtreeNode::NorthEastNode ) );
    QuadtreeNode<Patch>* se_child_node = static_cast<QuadtreeNode<Patch>*>( node->GetChild( BaseQuadtreeNode::SouthEastNode ) );
    QuadtreeNode<Patch>* sw_child_node = static_cast<QuadtreeNode<Patch>*>( node->GetChild( BaseQuadtreeNode::SouthWestNode ) );

    unset_border_neighbours( nw_child_node );
    unset_border_neighbours( ne_child_node );
    unset_border_neighbours( se_child_node );
    unset_border_neighbours( sw_child_node );

    (*m_merge_handler)( m_orientation, patch );
}

void Face::unset_border_neighbours( DrawSpace::Utils::QuadtreeNode<Patch>* p_node )
{
    QuadtreeNode<Patch>* parent;
    BaseQuadtreeNode* parent_neighb;

    parent = static_cast<QuadtreeNode<Patch>*>( p_node->GetParent() );

    switch( p_node->GetId() )
    {
        case BaseQuadtreeNode::NorthWestNode:
            {
                parent_neighb = parent->GetContent()->GetNeighbour( Patch::NorthNeighbour );
                if( parent_neighb && parent_neighb->HasChildren() )
                {
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::SouthWestNode ) )->GetContent()->SetNeighbour( NULL, Patch::SouthNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::SouthEastNode ) )->GetContent()->SetNeighbour( NULL, Patch::SouthWestNeighbour );
                }

                parent_neighb = parent->GetContent()->GetNeighbour( Patch::WestNeighbour );
                if( parent_neighb && parent_neighb->HasChildren() )
                {
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::NorthEastNode ) )->GetContent()->SetNeighbour( NULL, Patch::EastNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::SouthEastNode ) )->GetContent()->SetNeighbour( NULL, Patch::NorthEastNeighbour );
                }

                parent_neighb = parent->GetContent()->GetNeighbour( Patch::NorthWestNeighbour );
                if( parent_neighb && parent_neighb->HasChildren() )
                {
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::SouthEastNode ) )->GetContent()->SetNeighbour( NULL, Patch::SouthEastNeighbour );
                }
            }
            break;

        case BaseQuadtreeNode::NorthEastNode:
            {
                parent_neighb = parent->GetContent()->GetNeighbour( Patch::NorthNeighbour );
                if( parent_neighb && parent_neighb->HasChildren() )
                {
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::SouthEastNode ) )->GetContent()->SetNeighbour( NULL, Patch::SouthNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::SouthWestNode ) )->GetContent()->SetNeighbour( NULL, Patch::SouthEastNeighbour );
                }

                parent_neighb = parent->GetContent()->GetNeighbour( Patch::EastNeighbour );
                if( parent_neighb && parent_neighb->HasChildren() )
                {
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::NorthWestNode ) )->GetContent()->SetNeighbour( NULL, Patch::WestNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::SouthWestNode ) )->GetContent()->SetNeighbour( NULL, Patch::NorthWestNeighbour );
                }

                parent_neighb = parent->GetContent()->GetNeighbour( Patch::NorthEastNeighbour );
                if( parent_neighb && parent_neighb->HasChildren() )
                {
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::SouthWestNode ) )->GetContent()->SetNeighbour( NULL, Patch::SouthWestNeighbour );
                }
            }
            break;

        case BaseQuadtreeNode::SouthEastNode:
            {
                parent_neighb = parent->GetContent()->GetNeighbour( Patch::EastNeighbour );
                if( parent_neighb && parent_neighb->HasChildren() )
                {
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::SouthWestNode ) )->GetContent()->SetNeighbour( NULL, Patch::WestNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::NorthWestNode ) )->GetContent()->SetNeighbour( NULL, Patch::SouthWestNeighbour );
                }

                parent_neighb = parent->GetContent()->GetNeighbour( Patch::SouthNeighbour );
                if( parent_neighb && parent_neighb->HasChildren() )
                {
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::NorthEastNode ) )->GetContent()->SetNeighbour( NULL, Patch::NorthNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::NorthWestNode ) )->GetContent()->SetNeighbour( NULL, Patch::NorthEastNeighbour );
                }

                parent_neighb = parent->GetContent()->GetNeighbour( Patch::SouthEastNeighbour );
                if( parent_neighb && parent_neighb->HasChildren() )
                {
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::NorthWestNode ) )->GetContent()->SetNeighbour( NULL, Patch::NorthWestNeighbour );
                }
            }
            break;

        case BaseQuadtreeNode::SouthWestNode:
            {
                parent_neighb = parent->GetContent()->GetNeighbour( Patch::WestNeighbour );
                if( parent_neighb && parent_neighb->HasChildren() )
                {
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::SouthEastNode ) )->GetContent()->SetNeighbour( NULL, Patch::EastNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::NorthEastNode ) )->GetContent()->SetNeighbour( NULL, Patch::SouthEastNeighbour );
                }

                parent_neighb = parent->GetContent()->GetNeighbour( Patch::SouthNeighbour );
                if( parent_neighb && parent_neighb->HasChildren() )
                {
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::NorthWestNode ) )->GetContent()->SetNeighbour( NULL, Patch::NorthNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::NorthEastNode ) )->GetContent()->SetNeighbour( NULL, Patch::NorthWestNeighbour );
                }

                parent_neighb = parent->GetContent()->GetNeighbour( Patch::SouthWestNeighbour );
                if( parent_neighb && parent_neighb->HasChildren() )
                {
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->GetChild( BaseQuadtreeNode::NorthEastNode ) )->GetContent()->SetNeighbour( NULL, Patch::NorthEastNeighbour );
                }
            }
            break;
    }
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