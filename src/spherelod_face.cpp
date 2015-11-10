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

#include "spherelod_face.h"
#include "memalloc.h"
#include "maths.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::SphericalLOD;

Face::Face( dsreal p_diameter ) : 
m_rootpatch( NULL ), 
m_planet_diameter( p_diameter ),
m_currentleaf( NULL ),
m_currentPatch( NULL ),
m_currentPatchLOD( -1 ),
m_hot( false ),
m_relative_alt( 0.0 ),
m_lod_slipping_sup( NB_LOD_RANGES - 1 ),
m_lod_slipping_inf( NB_LOD_RANGES - 4 )
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

    init_lodranges();

    return true;
}

void Face::init_lodranges( void )
{
    dsreal k = 1.0;
    for( int i = NB_LOD_RANGES - 1; i >= 0; i--)
    {
        m_lodranges[i] = k * m_planet_diameter / 2.0;        
        k *= 0.5;
    }
}

void Face::on_nodeinstanciation( BaseQuadtreeNode* p_node )
{
    if( NULL == m_rootpatch )
    {
        QuadtreeNode<Patch>* root = static_cast<QuadtreeNode<Patch>*>( p_node );

        Patch* patch = _DRAWSPACE_NEW_( Patch, Patch( m_planet_diameter / 2.0, m_orientation, NULL, -1, root, true, Vector( 0.0, 0.0, 1.0, 1.0 ) ) );
        root->SetContent( patch );      
    }
    else
    {
        QuadtreeNode<Patch>* node = static_cast<QuadtreeNode<Patch>*>( p_node );
        QuadtreeNode<Patch>* parent = static_cast<QuadtreeNode<Patch>*>( node->GetParent() );

        Patch* patch = _DRAWSPACE_NEW_( Patch, Patch( m_planet_diameter / 2.0, m_orientation, parent->GetContent(), node->GetId(), node, false, Vector() ) );
        node->SetContent( patch );      
    }
}

void Face::on_nodedeletion( DrawSpace::Utils::BaseQuadtreeNode* p_node )
{    
    QuadtreeNode<Patch>* node = static_cast<QuadtreeNode<Patch>*>( p_node );
    
    Patch* patch = node->GetContent();    
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

void Face::UpdateRelativeHotpoint( const DrawSpace::Utils::Vector& p_point )
{
    m_relative_hotpoint = p_point;
    compute_cubeface_hotpoint();
}

bool Face::is_hotpoint_bound_in_node( BaseQuadtreeNode* p_node )
{

    Vector projected_viewer = m_cubeface_hotpoint;

    Patch* current_patch = static_cast<QuadtreeNode<Patch>*>( p_node )->GetContent();

    dsreal patch_xpos, patch_ypos;
    current_patch->GetPos( patch_xpos, patch_ypos );

    dsreal patch_side_size = current_patch->GetSideLength();

    if( ( patch_xpos - ( patch_side_size * 0.5 ) ) <= projected_viewer[0] && ( patch_xpos + ( patch_side_size * 0.5 ) ) >= projected_viewer[0] &&
        ( patch_ypos - ( patch_side_size * 0.5 ) ) <= projected_viewer[1] && ( patch_ypos + ( patch_side_size * 0.5 ) ) >= projected_viewer[1] )
    {
        return true;
    }
    return false;
}


QuadtreeNode<Patch>* Face::find_leaf_under( QuadtreeNode<Patch>* p_current )
{
    QuadtreeNode<Patch>* child;

    if( is_hotpoint_bound_in_node( p_current ) )
    {
        if( p_current->HasChildren() )
        {
            child = find_leaf_under( static_cast<QuadtreeNode<Patch>*>( p_current->GetChild( BaseQuadtreeNode::NorthWestNode ) ) );
            if( child )
            {
                return child;
            }

            child = find_leaf_under( static_cast<QuadtreeNode<Patch>*>( p_current->GetChild( BaseQuadtreeNode::NorthEastNode ) ) );
            if( child )
            {
                return child;
            }

            child = find_leaf_under( static_cast<QuadtreeNode<Patch>*>( p_current->GetChild( BaseQuadtreeNode::SouthEastNode ) ) );
            if( child )
            {
                return child;
            }

            child = find_leaf_under( static_cast<QuadtreeNode<Patch>*>( p_current->GetChild( BaseQuadtreeNode::SouthWestNode ) ) );
            if( child )
            {
                return child;
            }
        }
        return p_current;
    }
    return NULL;
}

void Face::UpdateLODComputationParams( void )
{
    m_work_cubeface_hotpoint = m_cubeface_hotpoint;
    m_work_lod_slipping_sup = m_lod_slipping_sup;
    m_work_lod_slipping_inf = m_lod_slipping_inf;
}

void Face::Compute( void )
{
    if( m_hot )
    {
        //m_displaylist.clear();
        //m_currentPatch = NULL;

        m_work_displaylist.clear();
        m_work_currentPatch = NULL;

        recursive_build_displaylist( m_rootpatch, NB_LOD_RANGES - 1 );
    }
}

void Face::UpdateLODComputationResults( void )
{
    m_displaylist = m_work_displaylist;
    m_currentPatch = m_work_currentPatch;
    m_currentPatchLOD = m_work_currentPatchLOD;
}

bool Face::ComputeAlignmentFactor( void )
{
    Vector face_dir;

    Patch::GetNormalVector( m_orientation, face_dir );
    Vector norm_hp = m_relative_hotpoint;
    norm_hp.Normalize();
    m_alignment_factor = norm_hp * face_dir;

    if( m_alignment_factor < 0 )
    {
        return false;
    }
    return true;
}

void Face::ResetDisplayList( void )
{
    m_displaylist.clear();
    m_displaylist.push_back( m_rootpatch->GetContent() );
}

void Face::compute_cubeface_hotpoint( void )
{
    Vector viewer;

    Patch::ConvertVectorToFrontFaceCoords( m_orientation, m_relative_hotpoint, viewer );
 
    viewer.Normalize();
    Vector projected_viewer;
    Patch::SphereToCube( viewer, projected_viewer );    
    projected_viewer.Scale( m_planet_diameter / 2.0 );

    m_cubeface_hotpoint = projected_viewer;
}

DrawSpace::Utils::QuadtreeNode<Patch>* Face::GetCurrentLeaf( void )
{
    return m_currentleaf;
}

dsreal Face::GetAlignmentFactor( void )
{
    return m_alignment_factor;
}

void Face::GetDisplayList( std::vector<Patch*>& p_displaylist )
{
    p_displaylist = m_displaylist;
}

void Face::SetHotState( bool p_hotstate )
{
    m_hot = p_hotstate;
    ResetDisplayList();
    if( !m_hot )
    {
        m_currentPatch = NULL;
    }
}

bool Face::recursive_build_displaylist( BaseQuadtreeNode* p_current_node, int p_lodlevel )
{
    QuadtreeNode<Patch>* patch_node = static_cast<QuadtreeNode<Patch>*>( p_current_node );

    if( !patch_node->GetContent()->IsCircleIntersection( m_work_cubeface_hotpoint[0], m_work_cubeface_hotpoint[1], m_lodranges[p_lodlevel] ) )
    {
        return false;
    }
    
    if( m_work_lod_slipping_inf == p_lodlevel )
    {
        m_work_displaylist.push_back( patch_node->GetContent() );
        ///////////////////////////////////
        if( is_hotpoint_bound_in_node( patch_node ) )
        {
            m_work_currentPatch = patch_node->GetContent();
            m_work_currentPatchLOD = m_work_lod_slipping_inf;
        }
        ///////////////////////////////////
        return true;
    }
    else
    {
        if( !patch_node->GetContent()->IsCircleIntersection( m_work_cubeface_hotpoint[0], m_work_cubeface_hotpoint[1], m_lodranges[p_lodlevel - 1] ) )
        {
            if( p_lodlevel <= m_work_lod_slipping_sup )
            {
                m_work_displaylist.push_back( patch_node->GetContent() );   
            }
        }
        else
        {
            // explore les 4 fils

            // si pas de fils, splitter...
            if( !patch_node->HasChildren() )
            {
                patch_node->Split();
            }

            for( int i = 0; i < 4; i++ )
            {
                BaseQuadtreeNode* sub = patch_node->GetChild( i );

                if( !recursive_build_displaylist( sub, p_lodlevel - 1 ) )
                {
                    if( p_lodlevel - 1 <= m_work_lod_slipping_sup )
                    {
                        QuadtreeNode<Patch>* sub_patch_node = static_cast<QuadtreeNode<Patch>*>( sub );
                        m_work_displaylist.push_back( sub_patch_node->GetContent() );
                    }
                }
            }
        }
    }
    return true;
}

void Face::UpdateRelativeAlt( dsreal p_alt )
{
    m_relative_alt = p_alt;
    if( m_hot )
    { 
        // calcul de la fenetre glissante de lod (m_lod_slipping_sup et m_lod_slipping_inf) en fct de l'altitude relative
        // c'est la fct atan qui est choisie pour son profil.

        // ramener dans l'intervale 1.0 - 0.0
        dsreal factor = Maths::Clamp( 0.0, 1.0, m_relative_alt - 1.0 );

        dsreal factor2 = 2.0 * atan( 180.0 * factor ) / PI;


        m_lod_slipping_sup = Maths::Clamp( 0, NB_LOD_RANGES - 1, Maths::Lerp( 4, NB_LOD_RANGES - 1, factor2 ) );
        m_lod_slipping_inf = Maths::Clamp( 0, NB_LOD_RANGES - 1, Maths::Lerp( 0, NB_LOD_RANGES - 2, factor2 ) );

    }
}

Patch* Face::GetCurrentPatch( void )
{
    return m_currentPatch;
}

int Face::GetCurrentPatchLOD( void )
{
    return m_currentPatchLOD;
}

Patch* Face::GetRootPatch( void )
{
    return m_rootpatch->GetContent();
}