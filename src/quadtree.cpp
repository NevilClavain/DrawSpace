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

#include "quadtree.h"


using namespace DrawSpace;
using namespace DrawSpace::Utils;

BaseQuadtreeNode::BaseQuadtreeNode( BaseQuadtreeNode* p_parent, int p_id ) : m_parent( p_parent ), m_id( p_id ), m_splitted( false )
{
}

BaseQuadtreeNode::~BaseQuadtreeNode( void )
{
}

void BaseQuadtreeNode::SetParent( BaseQuadtreeNode* p_parent )
{
	m_parent = p_parent;
}

BaseQuadtreeNode* BaseQuadtreeNode::GetParent( void )
{
    return m_parent;
}

bool BaseQuadtreeNode::HasChildren( void )
{
	return m_splitted;
}

BaseQuadtreeNode* BaseQuadtreeNode::GetChild( int p_id )
{
	return m_children[p_id];
}

int BaseQuadtreeNode::GetId( void )
{
    return m_id;
}
