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

Face::Face( void ) : 
m_rootpatch( NULL ), 
m_planet_diameter( 10.0 ),
m_currentleaf( NULL ),
m_ratio_split_threshold( 0.03 ),
m_ratio_merge_threshold( 0.04 )
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

/*
Maps* Face::GetMapsFactory( void )
{
    return &m_maps_factory;
}
*/

void Face::on_nodeinstanciation( BaseQuadtreeNode* p_node )
{
    if( NULL == m_rootpatch )
    {
        QuadtreeNode<Patch>* root = static_cast<QuadtreeNode<Patch>*>( p_node );

        Patch* patch = _DRAWSPACE_NEW_( Patch, Patch( m_planet_diameter / 2.0, m_orientation, ".0", NULL, -1, root ) );
        root->SetContent( patch );

        dsstring patch_name;
        patch->GetName( patch_name );        
        m_patches[patch_name] = p_node;

        for( size_t i = 0; i < m_inst_handlers.size(); i++ )
        {
            (*( m_inst_handlers[i] ) )( m_orientation, patch );
        }

        m_patchesleafs[patch_name] = patch;

        /*
        // request texture from maps factory

        void* color_texture;
        void* elevation_texture;
        
        color_texture = m_maps_factory.RequestTexture( Maps::COLOR_TEXTURE, patch );
        elevation_texture = m_maps_factory.RequestTexture( Maps::ELEVATION_TEXTURE, patch );

        patch->SetTexture( Maps::COLOR_TEXTURE, color_texture );
        patch->SetTexture( Maps::ELEVATION_TEXTURE, elevation_texture );
        */

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

        Patch* patch = _DRAWSPACE_NEW_( Patch, Patch( m_planet_diameter / 2.0, m_orientation, node_name, parent->GetContent(), node->GetId(), node ) );
        node->SetContent( patch );

        dsstring patch_name;
        patch->GetName( patch_name );
        m_patches[patch_name] = p_node;

        for( size_t i = 0; i < m_inst_handlers.size(); i++ )
        {
            (*( m_inst_handlers[i] ) )( m_orientation, patch );
        }

        m_patchesleafs[patch_name] = patch;

        /*
        // request texture from maps factory

        void* color_texture;
        void* elevation_texture;
        
        color_texture = m_maps_factory.RequestTexture( Maps::COLOR_TEXTURE, patch );
        elevation_texture = m_maps_factory.RequestTexture( Maps::ELEVATION_TEXTURE, patch );

        patch->SetTexture( Maps::COLOR_TEXTURE, color_texture );
        patch->SetTexture( Maps::ELEVATION_TEXTURE, elevation_texture );
        */
    }
}

void Face::on_nodedeletion( DrawSpace::Utils::BaseQuadtreeNode* p_node )
{    
    QuadtreeNode<Patch>* node = static_cast<QuadtreeNode<Patch>*>( p_node );
    
    Patch* patch = node->GetContent();

    for( size_t i = 0; i < m_del_handlers.size(); i++ )
    {
        (*( m_del_handlers[i] ) )( m_orientation, patch );
    }

    dsstring patch_name;
    patch->GetName( patch_name );   
    m_patches.erase( patch_name );
    _DRAWSPACE_DELETE_( patch );

    if( m_patchesleafs.count( patch_name ) > 0 )
    {
        m_patchesleafs.erase( patch_name );
    }
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

    for( size_t i = 0; i < m_split_handlers.size(); i++ )
    {
        (*( m_split_handlers[i] ) )( m_orientation, patch );
    }

    dsstring patch_name;
    patch->GetName( patch_name );   
    if( m_patchesleafs.count( patch_name ) > 0 )
    {
        m_patchesleafs.erase( patch_name );
    }
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

    for( size_t i = 0; i < m_merge_handlers.size(); i++ )
    {
        (*( m_merge_handlers[i] ) )( m_orientation, patch );
    }

    dsstring patch_name;
    patch->GetName( patch_name );
    m_patchesleafs[patch_name] = patch;
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

void Face::UpdateRelativeHotpoint( const DrawSpace::Utils::Vector& p_point )
{
    m_relative_hotpoint = p_point;

    m_movement[0] = m_relative_hotpoint[0] - m_prev_relative_hotpoint[0];
    m_movement[1] = m_relative_hotpoint[1] - m_prev_relative_hotpoint[1];
    m_movement[2] = m_relative_hotpoint[2] - m_prev_relative_hotpoint[2];

    m_prev_relative_hotpoint = m_relative_hotpoint;
}

void Face::split_group( DrawSpace::Utils::BaseQuadtreeNode* p_node )
{
    Patch* current_patch = static_cast<QuadtreeNode<Patch>*>( p_node )->GetContent();
    p_node->Split();
    
    for( long i = 0; i < 8; i++ )
    {
        if( current_patch->GetNeighbour( i ) != NULL )
        {
            current_patch->GetNeighbour( i )->Split();
        }
    }
    
}

void Face::merge_group( DrawSpace::Utils::BaseQuadtreeNode* p_node )
{
    Patch* current_patch = static_cast<QuadtreeNode<Patch>*>( p_node )->GetContent();
    p_node->Merge();
    
    for( long i = 0; i < 8; i++ )
    {
        if( current_patch->GetNeighbour( i ) != NULL )
        {
            current_patch->GetNeighbour( i )->Merge();
        }
    }    
}


bool Face::is_hotpoint_bound_in_node( BaseQuadtreeNode* p_node, const Vector& p_hotpoint )
{
    Vector viewer;

    if( m_orientation == Patch::FrontPlanetFace )
    {
        viewer[0] = p_hotpoint[0];
        viewer[1] = p_hotpoint[1];
        viewer[2] = p_hotpoint[2];
        viewer[3] = 0.0;
    }

    if( m_orientation == Patch::RearPlanetFace )
    {
        viewer[0] = -p_hotpoint[0];
        viewer[1] = p_hotpoint[1];
        viewer[2] = -p_hotpoint[2];
        viewer[3] = 0.0;
    }

    if( m_orientation == Patch::TopPlanetFace )
    {
        viewer[0] = p_hotpoint[0];
        viewer[1] = -p_hotpoint[2];
        viewer[2] = p_hotpoint[1];
        viewer[3] = 0.0;
    }

    if( m_orientation == Patch::BottomPlanetFace )
    {
        viewer[0] = p_hotpoint[0];
        viewer[1] = p_hotpoint[2];
        viewer[2] = -p_hotpoint[1];
        viewer[3] = 0.0;
    }


    if( m_orientation == Patch::RightPlanetFace )
    {
        viewer[0] = -p_hotpoint[2];
        viewer[1] = p_hotpoint[1];
        viewer[2] = p_hotpoint[0];
        viewer[3] = 0.0;
    }

    if( m_orientation == Patch::LeftPlanetFace )
    {
        viewer[0] = p_hotpoint[2];
        viewer[1] = p_hotpoint[1];
        viewer[2] = -p_hotpoint[0];
        viewer[3] = 0.0;
    }

 
    viewer.Normalize();
    Vector projected_viewer;
    Patch::SphereToCube( viewer, projected_viewer );
    //projected_viewer = viewer;
    projected_viewer.Scale( m_planet_diameter / 2.0 );
   

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

dsreal Face::alt_ratio( dsreal p_altitud )
{
    dsreal side_length = m_currentleaf->GetContent()->GetSideLength();
    dsreal ratio = p_altitud / side_length;
    return ratio;
}

bool Face::check_split( Vector& p_hotpoint )
{
    bool status = false;

    Vector sphericals;
    Maths::CartesiantoSpherical( p_hotpoint, sphericals );

    dsreal alt = sphericals[0] - m_planet_diameter / 2.0;
    
    if( alt >= 0.0 )
    {
        while( alt_ratio( alt ) < m_ratio_split_threshold )
        {

            long depth_level = m_currentleaf->GetDepthLevel();

            if( depth_level >= /*11*/ 7 )
            {
                break;
            }
        
            // split necessaire
            split_group( m_currentleaf );

            if( is_hotpoint_bound_in_node( m_currentleaf->GetChild( BaseQuadtreeNode::NorthWestNode ), p_hotpoint ) )
            {
                m_currentleaf = static_cast<QuadtreeNode<Patch>*>( m_currentleaf->GetChild( BaseQuadtreeNode::NorthWestNode ) );
            }
            else if( is_hotpoint_bound_in_node( m_currentleaf->GetChild( BaseQuadtreeNode::NorthEastNode ), p_hotpoint ) )
            {
                m_currentleaf = static_cast<QuadtreeNode<Patch>*>( m_currentleaf->GetChild( BaseQuadtreeNode::NorthEastNode ) );
            }
            else if( is_hotpoint_bound_in_node( m_currentleaf->GetChild( BaseQuadtreeNode::SouthEastNode ), p_hotpoint ) )
            {
                m_currentleaf = static_cast<QuadtreeNode<Patch>*>( m_currentleaf->GetChild( BaseQuadtreeNode::SouthEastNode ) );
            }
            else if( is_hotpoint_bound_in_node( m_currentleaf->GetChild( BaseQuadtreeNode::SouthWestNode ), p_hotpoint ) )
            {
                m_currentleaf = static_cast<QuadtreeNode<Patch>*>( m_currentleaf->GetChild( BaseQuadtreeNode::SouthWestNode ) );
            }
            
            status = true;    
        }
    }
        
    return status;
}

bool Face::check_merge( Vector& p_hotpoint )
{
    bool status = false;

    Vector sphericals;
    Maths::CartesiantoSpherical( p_hotpoint, sphericals );

    dsreal alt = sphericals[0] - m_planet_diameter / 2.0;
    
    while( alt_ratio( alt ) > m_ratio_merge_threshold )
    {
    
        if( m_currentleaf->GetParent() )
        {
            m_currentleaf = static_cast<QuadtreeNode<Patch>*>( m_currentleaf->GetParent() );
            merge_group( m_currentleaf );

            status = true;
        }
        
        else
        {
            break;
        }
        
    }
    return status;
}



QuadtreeNode<Patch>* Face::find_leaf_under( QuadtreeNode<Patch>* p_current, Vector& p_point )
{
    QuadtreeNode<Patch>* child;

    if( is_hotpoint_bound_in_node( p_current, p_point ) )
    {
        if( p_current->HasChildren() )
        {
            child = find_leaf_under( static_cast<QuadtreeNode<Patch>*>( p_current->GetChild( BaseQuadtreeNode::NorthWestNode ) ), p_point );
            if( child )
            {
                return child;
            }

            child = find_leaf_under( static_cast<QuadtreeNode<Patch>*>( p_current->GetChild( BaseQuadtreeNode::NorthEastNode ) ), p_point );
            if( child )
            {
                return child;
            }

            child = find_leaf_under( static_cast<QuadtreeNode<Patch>*>( p_current->GetChild( BaseQuadtreeNode::SouthEastNode ) ), p_point );
            if( child )
            {
                return child;
            }

            child = find_leaf_under( static_cast<QuadtreeNode<Patch>*>( p_current->GetChild( BaseQuadtreeNode::SouthWestNode ) ), p_point );
            if( child )
            {
                return child;
            }
        }
        return p_current;
    }
    return NULL;
}



bool Face::Compute( void )
{

    bool status = false;

    Vector face_dir;
    switch( m_orientation )
    {
        case Patch::FrontPlanetFace:

            face_dir[0] = 0.0;
            face_dir[1] = 0.0;
            face_dir[2] = 1.0;
            face_dir[3] = 1.0;
            break;

        case Patch::RearPlanetFace:

            face_dir[0] = 0.0;
            face_dir[1] = 0.0;
            face_dir[2] = -1.0;
            face_dir[3] = 1.0;
            break;

        case Patch::TopPlanetFace:

            face_dir[0] = 0.0;
            face_dir[1] = 1.0;
            face_dir[2] = 0.0;
            face_dir[3] = 1.0;
            break;

        case Patch::BottomPlanetFace:

            face_dir[0] = 0.0;
            face_dir[1] = -1.0;
            face_dir[2] = 0.0;
            face_dir[3] = 1.0;
            break;

        case Patch::RightPlanetFace:

            face_dir[0] = 1.0;
            face_dir[1] = 0.0;
            face_dir[2] = 0.0;
            face_dir[3] = 1.0;
            break;

        case Patch::LeftPlanetFace:

            face_dir[0] = -1.0;
            face_dir[1] = 0.0;
            face_dir[2] = 0.0;
            face_dir[3] = 1.0;
            break;
    }

    Vector norm_hp = m_relative_hotpoint;
    norm_hp.Normalize();
    m_alignment_factor = norm_hp * face_dir;

    if( m_alignment_factor < 0 )
    {
        return false;
    }

    /*
    if( 0.0 == m_movement.LengthPow2() )
    {
        return false;
    }
    */

    if( m_currentleaf == NULL )
    {
        if( m_rootpatch )
        {
            m_currentleaf = find_leaf_under( m_rootpatch, m_relative_hotpoint );
        }
    }
    else
    {
        /*
        if( is_hotpoint_bound_in_node( m_currentleaf, m_relative_hotpoint ) )
        {
            if( m_movement * m_relative_hotpoint < 0.0 ) // on descend vers la surface
            {
                if( check_split( m_relative_hotpoint ) )
                {
                    status = true;
                }
            }
            else if( m_movement * m_relative_hotpoint > 0.0 ) // on prend de l'altitude
            {
                if( check_merge( m_relative_hotpoint ) )
                {
                    status = true;
                }
            }              
        }
        */
       
        if( is_hotpoint_bound_in_node( m_currentleaf, m_relative_hotpoint ) )
        {
            dsreal lod = m_currentleaf->GetContent()->GetTriangleSideLength() / ( m_relative_hotpoint.Length() - ( m_planet_diameter / 2.0 ) );

            if( lod > 0.5 )
            {
                if( check_split( m_relative_hotpoint ) )
                {
                    status = true;
                }
            }
            else if( lod < 0.1 )
            {
                if( check_merge( m_relative_hotpoint ) )
                {
                    status = true;
                }
            }
        }
        else
        {
            DrawSpace::Utils::QuadtreeNode<Patch>* bounding_parent = static_cast<DrawSpace::Utils::QuadtreeNode<Patch>*>( m_currentleaf->GetParent() );

            if( bounding_parent )
            {
                merge_group( bounding_parent );
                m_currentleaf = bounding_parent;
                status = true;
            }
            else
            {
                m_currentleaf = NULL;
            }

        }
    }
    return status;
}

void Face::AddInstHandler( Face::PatchInstanciationHandler* p_handler )
{
    m_inst_handlers.push_back( p_handler );
}

void Face::AddSplitHandler( Face::PatchSplitHandler* p_handler )
{
    m_split_handlers.push_back( p_handler );
}

void Face::AddDelHandler( Face::PatchDeletionHandler* p_handler )
{
    m_del_handlers.push_back( p_handler );
}

void Face::AddMergeHandler( Face::PatchMergeHandler* p_handler )
{
    m_merge_handlers.push_back( p_handler );
}

DrawSpace::Utils::QuadtreeNode<Patch>* Face::GetCurrentLeaf( void )
{
    return m_currentleaf;
}

dsreal Face::GetAlignmentFactor( void )
{
    return m_alignment_factor;
}

void Face::ResetMeshe( void )
{
    if( !m_currentleaf )
    {
        return;
    }

    DrawSpace::Utils::QuadtreeNode<Patch>* parent; 
    do
    {
        parent = static_cast<DrawSpace::Utils::QuadtreeNode<Patch>*>( m_currentleaf->GetParent() );
        if( parent )
        {
            merge_group( parent );
            m_currentleaf = parent;
        }

    } while( parent );  
}

void Face::GetLeafs( std::map<dsstring, Patch*>& p_list )
{
    p_list = m_patchesleafs;
}
