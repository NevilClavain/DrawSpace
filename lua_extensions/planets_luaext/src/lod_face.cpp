/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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
/* -*-LIC_END-*- */

#include "lod_face.h"
#include "lod_config.h"
#include "memalloc.h"
#include "maths.h"
#include "exceptions.h"

using namespace DrawSpace;
using namespace DrawSpace::Commons;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Maths;
using namespace LOD;

Face::Face( Config* p_config, int p_layer_index, SubPass::SubPassCreationHandler* p_handler, int p_nbLODRanges ) :
m_config( p_config ),
m_subpasscreation_handler( p_handler ),
m_layer_index( p_layer_index ),
m_nbLODRanges( p_nbLODRanges )
{
    m_diameter = 1000.0 * m_config->m_layers_descr[p_layer_index].ray * 2.0;
    m_min_lodlevel = m_config->m_layers_descr[p_layer_index].min_lodlevel;

    m_lod_slipping_sup = m_nbLODRanges - 1;
    m_lod_slipping_inf = m_nbLODRanges - 4;
}

Face::~Face( void )
{
    if( m_rootpatch )
    {
        m_rootpatch->recursiveMerge();
        _DRAWSPACE_DELETE_( m_rootpatch );
    }

    if( m_cb_inst )
    {
         _DRAWSPACE_DELETE_( m_cb_inst );
    }

    if( m_cb_del )
    {
         _DRAWSPACE_DELETE_( m_cb_del );
    }

    if( m_cb_merge )
    {
         _DRAWSPACE_DELETE_( m_cb_merge );
    }

    if( m_cb_split )
    {
         _DRAWSPACE_DELETE_( m_cb_split );
    }
}

// create face's root patch
bool Face::Init( int p_orientation )
{
    m_orientation = p_orientation;

    const auto cb_inst { _DRAWSPACE_NEW_(InstanciationCallback, InstanciationCallback(this, &Face::on_nodeinstanciation)) };
    const auto cb_del { _DRAWSPACE_NEW_( DeletionCallback, DeletionCallback( this, &Face::on_nodedeletion ) ) };
    const auto cb_merge { _DRAWSPACE_NEW_( InstanciationCallback, InstanciationCallback( this, &Face::on_nodemerge ) ) };
    const auto cb_split { _DRAWSPACE_NEW_( DeletionCallback, DeletionCallback( this, &Face::on_nodesplit ) ) };

    m_rootpatch = _DRAWSPACE_NEW_( QuadtreeNode<Patch>, QuadtreeNode<Patch>( cb_inst, cb_del, cb_split, cb_merge ) );    
    m_cb_inst = cb_inst;
    m_cb_del = cb_del;
    m_cb_merge = cb_merge;
    m_cb_split = cb_split;
    init_lodranges();

    return true;
}

void Face::init_lodranges( void )
{
    auto k { 1.0 };    
    for (int i = m_nbLODRanges - 1; i >= 0; i--)
    {
        m_lodranges.insert(m_lodranges.begin(), k * m_diameter / 2.0);
        k *= 0.5;
    }
}

void Face::on_nodeinstanciation( BaseQuadtreeNode* p_node )
{
    if( nullptr == m_rootpatch )
    {
        const auto root { static_cast<QuadtreeNode<Patch>*>(p_node) };
        const auto patch { _DRAWSPACE_NEW_(Patch, Patch(m_diameter / 2.0, m_orientation, nullptr, -1,
                           root, m_subpasscreation_handler, m_config, m_layer_index, m_nbLODRanges)) };

        root->setContent( patch );      
    }
    else
    {
        const auto node { static_cast<QuadtreeNode<Patch>*>(p_node) };
        const auto parent { static_cast<QuadtreeNode<Patch>*>(node->getParent()) };

        const auto patch { _DRAWSPACE_NEW_(Patch, Patch(m_diameter / 2.0, m_orientation, parent->getContent(), node->getId(),
                                                        node, m_subpasscreation_handler, m_config, m_layer_index, m_nbLODRanges)) };
        node->setContent( patch );      
    }
}

void Face::on_nodedeletion( DrawSpace::BaseQuadtreeNode* p_node )
{    
    const auto node { static_cast<QuadtreeNode<Patch>*>(p_node) };    
    auto patch { node->getContent() };
    patch->CleanupSubpasses();
    _DRAWSPACE_DELETE_( patch );
}

void Face::on_nodesplit( DrawSpace::BaseQuadtreeNode* p_node )
{
    const auto node { static_cast<QuadtreeNode<Patch>*>(p_node) };
    const auto patch { node->getContent() };

    const auto nw_child_node { static_cast<QuadtreeNode<Patch>*>(node->getChild(BaseQuadtreeNode::NorthWestNode)) };
    const auto ne_child_node { static_cast<QuadtreeNode<Patch>*>(node->getChild(BaseQuadtreeNode::NorthEastNode)) };
    const auto se_child_node { static_cast<QuadtreeNode<Patch>*>(node->getChild(BaseQuadtreeNode::SouthEastNode)) };
    const auto sw_child_node { static_cast<QuadtreeNode<Patch>*>(node->getChild(BaseQuadtreeNode::SouthWestNode)) };

    nw_child_node->getContent()->SetNeighbour( ne_child_node, Patch::EastNeighbour );
    nw_child_node->getContent()->SetNeighbour( sw_child_node, Patch::SouthNeighbour );
    nw_child_node->getContent()->SetNeighbour( se_child_node, Patch::SouthEastNeighbour );

    ne_child_node->getContent()->SetNeighbour( nw_child_node, Patch::WestNeighbour );
    ne_child_node->getContent()->SetNeighbour( sw_child_node, Patch::SouthWestNeighbour );
    ne_child_node->getContent()->SetNeighbour( se_child_node, Patch::SouthNeighbour );

    se_child_node->getContent()->SetNeighbour( nw_child_node, Patch::NorthWestNeighbour );
    se_child_node->getContent()->SetNeighbour( ne_child_node, Patch::NorthNeighbour );
    se_child_node->getContent()->SetNeighbour( sw_child_node, Patch::WestNeighbour );

    sw_child_node->getContent()->SetNeighbour( nw_child_node, Patch::NorthNeighbour );
    sw_child_node->getContent()->SetNeighbour( ne_child_node, Patch::NorthEastNeighbour );
    sw_child_node->getContent()->SetNeighbour( se_child_node, Patch::EastNeighbour );

    set_border_neighbours( nw_child_node );
    set_border_neighbours( ne_child_node );
    set_border_neighbours( se_child_node );
    set_border_neighbours( sw_child_node );
}

void Face::set_border_neighbours( QuadtreeNode<Patch>* p_node )
{
    BaseQuadtreeNode* parent_neighb;
    const auto parent { static_cast<QuadtreeNode<Patch>*>(p_node->getParent()) };

    switch( p_node->getId() )
    {
        case BaseQuadtreeNode::NorthWestNode:
            {
                parent_neighb = parent->getContent()->GetNeighbour( Patch::NorthNeighbour );
                if( parent_neighb && parent_neighb->hasChildren() )
                {
                    p_node->getContent()->SetNeighbour( parent_neighb->getChild( BaseQuadtreeNode::SouthWestNode ), Patch::NorthNeighbour );
                    p_node->getContent()->SetNeighbour( parent_neighb->getChild( BaseQuadtreeNode::SouthEastNode ), Patch::NorthEastNeighbour );

                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::SouthWestNode ) )->getContent()->SetNeighbour( p_node, Patch::SouthNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::SouthEastNode ) )->getContent()->SetNeighbour( p_node, Patch::SouthWestNeighbour );
                }

                parent_neighb = parent->getContent()->GetNeighbour( Patch::WestNeighbour );
                if( parent_neighb && parent_neighb->hasChildren() )
                {
                    p_node->getContent()->SetNeighbour( parent_neighb->getChild( BaseQuadtreeNode::NorthEastNode ), Patch::WestNeighbour );
                    p_node->getContent()->SetNeighbour( parent_neighb->getChild( BaseQuadtreeNode::SouthEastNode ), Patch::SouthWestNeighbour );

                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::NorthEastNode ) )->getContent()->SetNeighbour( p_node, Patch::EastNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::SouthEastNode ) )->getContent()->SetNeighbour( p_node, Patch::NorthEastNeighbour );
                }

                parent_neighb = parent->getContent()->GetNeighbour( Patch::NorthWestNeighbour );
                if( parent_neighb && parent_neighb->hasChildren() )
                {
                    p_node->getContent()->SetNeighbour( parent_neighb->getChild( BaseQuadtreeNode::SouthEastNode ), Patch::NorthWestNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::SouthEastNode ) )->getContent()->SetNeighbour( p_node, Patch::SouthEastNeighbour );
                }
            }
            break;

        case BaseQuadtreeNode::NorthEastNode:
            {
                parent_neighb = parent->getContent()->GetNeighbour( Patch::NorthNeighbour );
                if( parent_neighb && parent_neighb->hasChildren() )
                {
                    p_node->getContent()->SetNeighbour( parent_neighb->getChild( BaseQuadtreeNode::SouthEastNode ), Patch::NorthNeighbour );
                    p_node->getContent()->SetNeighbour( parent_neighb->getChild( BaseQuadtreeNode::SouthWestNode ), Patch::NorthWestNeighbour );

                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::SouthEastNode ) )->getContent()->SetNeighbour( p_node, Patch::SouthNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::SouthWestNode ) )->getContent()->SetNeighbour( p_node, Patch::SouthEastNeighbour );
                }

                parent_neighb = parent->getContent()->GetNeighbour( Patch::EastNeighbour );
                if( parent_neighb && parent_neighb->hasChildren() )
                {
                    p_node->getContent()->SetNeighbour( parent_neighb->getChild( BaseQuadtreeNode::NorthWestNode ), Patch::EastNeighbour );
                    p_node->getContent()->SetNeighbour( parent_neighb->getChild( BaseQuadtreeNode::SouthWestNode ), Patch::SouthEastNeighbour );

                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::NorthWestNode ) )->getContent()->SetNeighbour( p_node, Patch::WestNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::SouthWestNode ) )->getContent()->SetNeighbour( p_node, Patch::NorthWestNeighbour );
                }

                parent_neighb = parent->getContent()->GetNeighbour( Patch::NorthEastNeighbour );
                if( parent_neighb && parent_neighb->hasChildren() )
                {
                    p_node->getContent()->SetNeighbour( parent_neighb->getChild( BaseQuadtreeNode::SouthWestNode ), Patch::NorthEastNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::SouthWestNode ) )->getContent()->SetNeighbour( p_node, Patch::SouthWestNeighbour );
                }
            }
            break;

        case BaseQuadtreeNode::SouthEastNode:
            {
                parent_neighb = parent->getContent()->GetNeighbour( Patch::EastNeighbour );
                if( parent_neighb && parent_neighb->hasChildren() )
                {
                    p_node->getContent()->SetNeighbour( parent_neighb->getChild( BaseQuadtreeNode::SouthWestNode ), Patch::EastNeighbour );
                    p_node->getContent()->SetNeighbour( parent_neighb->getChild( BaseQuadtreeNode::NorthWestNode ), Patch::NorthEastNeighbour );

                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::SouthWestNode ) )->getContent()->SetNeighbour( p_node, Patch::WestNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::NorthWestNode ) )->getContent()->SetNeighbour( p_node, Patch::SouthWestNeighbour );
                }

                parent_neighb = parent->getContent()->GetNeighbour( Patch::SouthNeighbour );
                if( parent_neighb && parent_neighb->hasChildren() )
                {
                    p_node->getContent()->SetNeighbour( parent_neighb->getChild( BaseQuadtreeNode::NorthEastNode ), Patch::SouthNeighbour );
                    p_node->getContent()->SetNeighbour( parent_neighb->getChild( BaseQuadtreeNode::NorthWestNode ), Patch::SouthWestNeighbour );

                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::NorthEastNode ) )->getContent()->SetNeighbour( p_node, Patch::NorthNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::NorthWestNode ) )->getContent()->SetNeighbour( p_node, Patch::NorthEastNeighbour );
                }

                parent_neighb = parent->getContent()->GetNeighbour( Patch::SouthEastNeighbour );
                if( parent_neighb && parent_neighb->hasChildren() )
                {
                    p_node->getContent()->SetNeighbour( parent_neighb->getChild( BaseQuadtreeNode::NorthWestNode ), Patch::SouthEastNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::NorthWestNode ) )->getContent()->SetNeighbour( p_node, Patch::NorthWestNeighbour );
                }
            }
            break;

        case BaseQuadtreeNode::SouthWestNode:
            {
                parent_neighb = parent->getContent()->GetNeighbour( Patch::WestNeighbour );
                if( parent_neighb && parent_neighb->hasChildren() )
                {
                    p_node->getContent()->SetNeighbour( parent_neighb->getChild( BaseQuadtreeNode::SouthEastNode ), Patch::WestNeighbour );
                    p_node->getContent()->SetNeighbour( parent_neighb->getChild( BaseQuadtreeNode::NorthEastNode ), Patch::NorthWestNeighbour );

                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::SouthEastNode ) )->getContent()->SetNeighbour( p_node, Patch::EastNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::NorthEastNode ) )->getContent()->SetNeighbour( p_node, Patch::SouthEastNeighbour );
                }

                parent_neighb = parent->getContent()->GetNeighbour( Patch::SouthNeighbour );
                if( parent_neighb && parent_neighb->hasChildren() )
                {
                    p_node->getContent()->SetNeighbour( parent_neighb->getChild( BaseQuadtreeNode::NorthWestNode ), Patch::SouthNeighbour );
                    p_node->getContent()->SetNeighbour( parent_neighb->getChild( BaseQuadtreeNode::NorthEastNode ), Patch::SouthEastNeighbour );

                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::NorthWestNode ) )->getContent()->SetNeighbour( p_node, Patch::NorthNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::NorthEastNode ) )->getContent()->SetNeighbour( p_node, Patch::NorthWestNeighbour );
                }

                parent_neighb = parent->getContent()->GetNeighbour( Patch::SouthWestNeighbour );
                if( parent_neighb && parent_neighb->hasChildren() )
                {
                    p_node->getContent()->SetNeighbour( parent_neighb->getChild( BaseQuadtreeNode::NorthEastNode ), Patch::SouthWestNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::NorthEastNode ) )->getContent()->SetNeighbour( p_node, Patch::NorthEastNeighbour );
                }
            }
            break;
    }
}

void Face::on_nodemerge(DrawSpace::BaseQuadtreeNode* p_node)
{
    const auto node{ static_cast<QuadtreeNode<Patch>*>(p_node) };
    const auto patch{ node->getContent() };

    const auto nw_child_node { static_cast<QuadtreeNode<Patch>*>(node->getChild(BaseQuadtreeNode::NorthWestNode)) };
    const auto ne_child_node { static_cast<QuadtreeNode<Patch>*>(node->getChild(BaseQuadtreeNode::NorthEastNode)) };
    const auto se_child_node { static_cast<QuadtreeNode<Patch>*>( node->getChild( BaseQuadtreeNode::SouthEastNode ) ) };
    const auto sw_child_node { static_cast<QuadtreeNode<Patch>*>( node->getChild( BaseQuadtreeNode::SouthWestNode ) ) };

    unset_border_neighbours( nw_child_node );
    unset_border_neighbours( ne_child_node );
    unset_border_neighbours( se_child_node );
    unset_border_neighbours( sw_child_node );
}

void Face::unset_border_neighbours( DrawSpace::QuadtreeNode<Patch>* p_node )
{
    BaseQuadtreeNode* parent_neighb;
    const auto parent { static_cast<QuadtreeNode<Patch>*>(p_node->getParent()) };

    switch( p_node->getId() )
    {
        case BaseQuadtreeNode::NorthWestNode:
            {
                parent_neighb = parent->getContent()->GetNeighbour( Patch::NorthNeighbour );
                if( parent_neighb && parent_neighb->hasChildren() )
                {
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::SouthWestNode ) )->getContent()->SetNeighbour( nullptr, Patch::SouthNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::SouthEastNode ) )->getContent()->SetNeighbour( nullptr, Patch::SouthWestNeighbour );
                }

                parent_neighb = parent->getContent()->GetNeighbour( Patch::WestNeighbour );
                if( parent_neighb && parent_neighb->hasChildren() )
                {
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::NorthEastNode ) )->getContent()->SetNeighbour( nullptr, Patch::EastNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::SouthEastNode ) )->getContent()->SetNeighbour( nullptr, Patch::NorthEastNeighbour );
                }

                parent_neighb = parent->getContent()->GetNeighbour( Patch::NorthWestNeighbour );
                if( parent_neighb && parent_neighb->hasChildren() )
                {
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::SouthEastNode ) )->getContent()->SetNeighbour( nullptr, Patch::SouthEastNeighbour );
                }
            }
            break;

        case BaseQuadtreeNode::NorthEastNode:
            {
                parent_neighb = parent->getContent()->GetNeighbour( Patch::NorthNeighbour );
                if( parent_neighb && parent_neighb->hasChildren() )
                {
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::SouthEastNode ) )->getContent()->SetNeighbour( nullptr, Patch::SouthNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::SouthWestNode ) )->getContent()->SetNeighbour( nullptr, Patch::SouthEastNeighbour );
                }

                parent_neighb = parent->getContent()->GetNeighbour( Patch::EastNeighbour );
                if( parent_neighb && parent_neighb->hasChildren() )
                {
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::NorthWestNode ) )->getContent()->SetNeighbour( nullptr, Patch::WestNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::SouthWestNode ) )->getContent()->SetNeighbour( nullptr, Patch::NorthWestNeighbour );
                }

                parent_neighb = parent->getContent()->GetNeighbour( Patch::NorthEastNeighbour );
                if( parent_neighb && parent_neighb->hasChildren() )
                {
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::SouthWestNode ) )->getContent()->SetNeighbour( nullptr, Patch::SouthWestNeighbour );
                }
            }
            break;

        case BaseQuadtreeNode::SouthEastNode:
            {
                parent_neighb = parent->getContent()->GetNeighbour( Patch::EastNeighbour );
                if( parent_neighb && parent_neighb->hasChildren() )
                {
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::SouthWestNode ) )->getContent()->SetNeighbour( nullptr, Patch::WestNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::NorthWestNode ) )->getContent()->SetNeighbour( nullptr, Patch::SouthWestNeighbour );
                }

                parent_neighb = parent->getContent()->GetNeighbour( Patch::SouthNeighbour );
                if( parent_neighb && parent_neighb->hasChildren() )
                {
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::NorthEastNode ) )->getContent()->SetNeighbour( nullptr, Patch::NorthNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::NorthWestNode ) )->getContent()->SetNeighbour( nullptr, Patch::NorthEastNeighbour );
                }

                parent_neighb = parent->getContent()->GetNeighbour( Patch::SouthEastNeighbour );
                if( parent_neighb && parent_neighb->hasChildren() )
                {
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::NorthWestNode ) )->getContent()->SetNeighbour( nullptr, Patch::NorthWestNeighbour );
                }
            }
            break;

        case BaseQuadtreeNode::SouthWestNode:
            {
                parent_neighb = parent->getContent()->GetNeighbour( Patch::WestNeighbour );
                if( parent_neighb && parent_neighb->hasChildren() )
                {
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::SouthEastNode ) )->getContent()->SetNeighbour( nullptr, Patch::EastNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::NorthEastNode ) )->getContent()->SetNeighbour( nullptr, Patch::SouthEastNeighbour );
                }

                parent_neighb = parent->getContent()->GetNeighbour( Patch::SouthNeighbour );
                if( parent_neighb && parent_neighb->hasChildren() )
                {
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::NorthWestNode ) )->getContent()->SetNeighbour( nullptr, Patch::NorthNeighbour );
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::NorthEastNode ) )->getContent()->SetNeighbour( nullptr, Patch::NorthWestNeighbour );
                }

                parent_neighb = parent->getContent()->GetNeighbour( Patch::SouthWestNeighbour );
                if( parent_neighb && parent_neighb->hasChildren() )
                {
                    static_cast<QuadtreeNode<Patch>*>( parent_neighb->getChild( BaseQuadtreeNode::NorthEastNode ) )->getContent()->SetNeighbour( nullptr, Patch::NorthEastNeighbour );
                }
            }
            break;
    }
}

void Face::UpdateRelativeHotpoint( const DrawSpace::Maths::Vector& p_point )
{
    m_relative_hotpoint = p_point;
    compute_cubeface_hotpoint();    
}

bool Face::is_hotpoint_bound_in_node( BaseQuadtreeNode* p_node )
{
    const auto projected_viewer{ m_cubeface_hotpoint };
    const auto current_patch { static_cast<QuadtreeNode<Patch>*>(p_node)->getContent() };

    dsreal patch_xpos, patch_ypos;
    current_patch->GetPos( patch_xpos, patch_ypos );

    dsreal patch_side_size = current_patch->GetSideLength();

    if( ( patch_xpos - ( patch_side_size * 0.5 ) ) <= projected_viewer[0] && ( patch_xpos + ( patch_side_size * 0.5 ) ) >= projected_viewer[0] &&
        ( patch_ypos - ( patch_side_size * 0.5 ) ) <= projected_viewer[1] && ( patch_ypos + ( patch_side_size * 0.5 ) ) >= projected_viewer[1] )
    {
        m_currentPatchViewCoords[0] = ( projected_viewer[0] - patch_xpos ) / patch_side_size;
        m_currentPatchViewCoords[1] = ( projected_viewer[1] - patch_ypos ) / patch_side_size;
        return true;
    }
    return false;
}

void Face::GetCurrentPatchViewCoords( DrawSpace::Maths::Vector& p_outcoords ) const
{
    p_outcoords = m_currentPatchViewCoords;
}

QuadtreeNode<Patch>* Face::find_leaf_under( QuadtreeNode<Patch>* p_current )
{
    QuadtreeNode<Patch>* child;

    if( is_hotpoint_bound_in_node( p_current ) )
    {
        if( p_current->hasChildren() )
        {
            child = find_leaf_under( static_cast<QuadtreeNode<Patch>*>( p_current->getChild( BaseQuadtreeNode::NorthWestNode ) ) );
            if( child )
            {
                return child;
            }

            child = find_leaf_under( static_cast<QuadtreeNode<Patch>*>( p_current->getChild( BaseQuadtreeNode::NorthEastNode ) ) );
            if( child )
            {
                return child;
            }

            child = find_leaf_under( static_cast<QuadtreeNode<Patch>*>( p_current->getChild( BaseQuadtreeNode::SouthEastNode ) ) );
            if( child )
            {
                return child;
            }

            child = find_leaf_under( static_cast<QuadtreeNode<Patch>*>( p_current->getChild( BaseQuadtreeNode::SouthWestNode ) ) );
            if( child )
            {
                return child;
            }
        }
        return p_current;
    }
    return nullptr;
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
        m_work_displaylist.clear();
        m_work_currentPatch = nullptr;

        recursive_build_displaylist( m_rootpatch, /*NB_LOD_RANGES - 1*/ m_nbLODRanges - 1 );
    }
}

void Face::UpdateLODComputationResults( void )
{
    if( nullptr == m_work_currentPatch )
    {
        if( m_currentPatch == nullptr )
        {
            // si c'est la 1ere exe du CDLOD apres passage en "HOT"

            //_DSEXCEPTION( "LOD computation result : no current patch found!" )

            m_work_displaylist.push_back( m_rootpatch->getContent() );
            m_displaylist = m_work_displaylist;
            m_work_currentPatch = m_rootpatch->getContent();
            m_work_currentPatchLOD = m_nbLODRanges - 1;
        }
        else
        {
            // sinon, ne rien faire (on ne met rien a jour)
            return;
        }
    }

    m_displaylist = m_work_displaylist;
    m_currentPatch = m_work_currentPatch;
    m_currentPatchLOD = m_work_currentPatchLOD;
}

bool Face::ComputeAlignmentFactor( void )
{
    Vector face_dir;

    Patch::GetNormalVector( m_orientation, face_dir );
    auto norm_hp { m_relative_hotpoint };
    norm_hp.normalize();
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
    m_displaylist.push_back( m_rootpatch->getContent() );
}

void Face::compute_cubeface_hotpoint( void )
{
    Vector viewer;

    Patch::ConvertVectorToFrontFaceCoords( m_orientation, m_relative_hotpoint, viewer );
 
    viewer.normalize();
    Vector projected_viewer;
    Patch::SphereToCube( viewer, projected_viewer );    
    projected_viewer.scale( m_diameter / 2.0 );

    m_cubeface_hotpoint = projected_viewer;
}

DrawSpace::QuadtreeNode<Patch>* Face::GetCurrentLeaf( void ) const
{
    return m_currentleaf;
}

dsreal Face::GetAlignmentFactor( void ) const
{
    return m_alignment_factor;
}

void Face::GetDisplayList( std::vector<Patch*>& p_displaylist ) const
{
    p_displaylist = m_displaylist;
}

void Face::SetHotState( bool p_hotstate )
{
    m_hot = p_hotstate;
    ResetDisplayList();
    if( !m_hot )
    {
        m_currentPatch = nullptr;
    }
}

// coeur algo CDLOD
bool Face::recursive_build_displaylist( BaseQuadtreeNode* p_current_node, int p_lodlevel )
{
    const auto patch_node { static_cast<QuadtreeNode<Patch>*>(p_current_node) };

    if( !patch_node->getContent()->IsCircleIntersection( m_work_cubeface_hotpoint[0], m_work_cubeface_hotpoint[1], m_lodranges.at(p_lodlevel) ) )
    {
        return false;
    }
    
    if( m_work_lod_slipping_inf == p_lodlevel )
    {
        m_work_displaylist.push_back( patch_node->getContent() );
        ///////////////////////////////////
        if( is_hotpoint_bound_in_node( patch_node ) )
        {
            m_work_currentPatch = patch_node->getContent();
            m_work_currentPatchLOD = m_work_lod_slipping_inf;
        }
        ///////////////////////////////////
        recursive_merge( p_current_node );

        return true;
    }
    else
    {
        if( !patch_node->getContent()->IsCircleIntersection( m_work_cubeface_hotpoint[0], m_work_cubeface_hotpoint[1], m_lodranges.at(p_lodlevel - 1) ) )
        {
            if( p_lodlevel <= m_work_lod_slipping_sup )
            {
                m_work_displaylist.push_back( patch_node->getContent() );
                recursive_merge( p_current_node );
            }
        }
        else
        {
            // explore les 4 fils

            // si pas de fils, splitter...
            if( !patch_node->hasChildren() )
            {
                patch_node->split();
            }

            for( int i = 0; i < 4; i++ )
            {
                const auto sub { patch_node->getChild(i) };
                if( !recursive_build_displaylist( sub, p_lodlevel - 1 ) )
                {
                    if( p_lodlevel - 1 <= m_work_lod_slipping_sup )
                    {
                        QuadtreeNode<Patch>* sub_patch_node = static_cast<QuadtreeNode<Patch>*>( sub );
                        m_work_displaylist.push_back( sub_patch_node->getContent() );
                        recursive_merge( sub );
                    }
                }
            }
        }
    }
    return true;
}

dsreal Face::GetRelativeAlt( void ) const
{
    return m_relative_alt;
}

dsreal Face::GetRelativeAltSphere( void ) const
{
    return m_relative_alt_sphere;
}

void Face::UpdateRelativeAlt( dsreal p_alt )
{
    m_relative_alt = p_alt;

    m_relative_alt_sphere = p_alt;
    if( m_hot )
    { 
        // calcul de la fenetre glissante de lod (m_lod_slipping_sup et m_lod_slipping_inf) en fct de l'altitude relative
        // c'est la fct atan qui est choisie pour son profil.

        // ramener dans l'intervale 1.0 - 0.0
        const auto factor { Maths::clamp(0.0, 1.0, m_relative_alt - 1.0) };
        const auto factor2 { atan(60.0 * factor) / 1.57 };

        m_lod_slipping_sup = Maths::clamp( m_min_lodlevel, /*NB_LOD_RANGES - 1*/ m_nbLODRanges - 1, Maths::lerp( 12, /*NB_LOD_RANGES - 1*/ m_nbLODRanges - 1, factor2 ) );
        m_lod_slipping_inf = Maths::clamp( m_min_lodlevel, /*NB_LOD_RANGES - 1*/ m_nbLODRanges - 1, Maths::lerp( 0, /*NB_LOD_RANGES - 2*/ m_nbLODRanges - 2, factor2 ) );
    }
}

Patch* Face::GetCurrentPatch( void ) const
{
    return m_currentPatch;
}

int Face::GetCurrentPatchLOD( void ) const
{
    return m_currentPatchLOD;
}

Patch* Face::GetRootPatch( void ) const
{
    return m_rootpatch->getContent();
}

void Face::recursive_split( BaseQuadtreeNode* p_currpatch, int p_dest_depth, int p_current_depth )
{
    if( p_dest_depth == p_current_depth )
    {
        return;
    }
    p_currpatch->split();

    for( int i = 0; i < 4; i++ )
    {
        const auto child { p_currpatch->getChild(i) };
        recursive_split( child, p_dest_depth, p_current_depth + 1 );
    }
}


// 3/2/2019 : pas sur que ca serve...
/*
void Face::Split( int p_depth )
{
    if( !m_rootpatch )
    {
        return;
    }
    recursive_split( m_rootpatch, p_depth, 0 );
}
*/
void Face::Reset( void )
{
    recursive_merge( m_rootpatch );
}

void Face::recursive_merge( DrawSpace::BaseQuadtreeNode* p_currpatch )
{
    if( p_currpatch->hasChildren() )
    {
        for( int i = 0; i < 4; i++ )
        {
            const auto child { p_currpatch->getChild(i) };
            recursive_merge( child );
        }
        p_currpatch->merge();
    }
}

void Face::GetCurrentCubeFaceHotPoint( DrawSpace::Maths::Vector& p_cubeface_hotpoint ) const
{
    p_cubeface_hotpoint = m_cubeface_hotpoint;
}

void Face::GetRelativeHotPoint( DrawSpace::Maths::Vector& p_rhotpoint ) const
{
    p_rhotpoint = m_relative_hotpoint;
}
