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

#include "quadtree.h"
#include "memalloc.h"

using namespace DrawSpace;
using namespace DrawSpace::Utils;

BaseQuadtreeNode::BaseQuadtreeNode( std::map<BaseQuadtreeNode*, BaseQuadtreeNode*>& p_leafs ) : m_parent( NULL ), m_id( RootNode ), m_leafs( p_leafs ), m_splitted( false )
{
    m_leafs[this] = this;
}

BaseQuadtreeNode::BaseQuadtreeNode( std::map<BaseQuadtreeNode*, BaseQuadtreeNode*>& p_leafs, BaseQuadtreeNode* p_parent, int p_id ) : m_parent( p_parent ), m_id( p_id ), m_leafs( p_leafs ), m_splitted( false )
{

}

BaseQuadtreeNode::~BaseQuadtreeNode( void )
{


}

void BaseQuadtreeNode::SetParent( BaseQuadtreeNode* p_parent )
{
    m_parent = p_parent;
}

void BaseQuadtreeNode::Split( void )
{
    if( m_splitted )
    {
        // deja splitte
        return;
    }

    m_children[NorthWestNode] = _DRAWSPACE_NEW_( BaseQuadtreeNode, BaseQuadtreeNode( m_leafs, this, NorthWestNode ) );
    m_children[NorthEastNode] = _DRAWSPACE_NEW_( BaseQuadtreeNode, BaseQuadtreeNode( m_leafs, this, NorthEastNode ) );
    m_children[SouthEastNode] = _DRAWSPACE_NEW_( BaseQuadtreeNode, BaseQuadtreeNode( m_leafs, this, SouthEastNode ) );
    m_children[SouthWestNode] = _DRAWSPACE_NEW_( BaseQuadtreeNode, BaseQuadtreeNode( m_leafs, this, SouthWestNode ) );

    m_splitted = true;

    // retirer celui-ci de la liste des leafs
    if( m_leafs.count( this ) > 0 )
    {
        m_leafs.erase( this );
    }

    // inscrire les fils dans la liste des leafs
    m_leafs[m_children[NorthWestNode]] = m_children[NorthWestNode];
    m_leafs[m_children[NorthEastNode]] = m_children[NorthEastNode];
    m_leafs[m_children[SouthEastNode]] = m_children[SouthEastNode];
    m_leafs[m_children[SouthWestNode]] = m_children[SouthWestNode];

}

void BaseQuadtreeNode::Merge( void )
{

}

bool BaseQuadtreeNode::HasChildren( void )
{
    return m_splitted;
}

BaseQuadtreeNode* BaseQuadtreeNode::GetChild( int p_id )
{
    return m_children[p_id];
}