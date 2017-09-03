/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
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

#include "renderpassnode.h"
#include "memalloc.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;


RenderPassNode::RenderPassNode( st_tree::tree<RenderPassNode::PassDescr*>::node_type& p_node ) :
    m_tree_node( p_node )
{
};

RenderPassNode RenderPassNode::CreateChild( const dsstring& p_name )
{
    st_tree::tree<RenderPassNode::PassDescr*>::node_type::iterator it = m_tree_node.insert( _DRAWSPACE_NEW_( PassDescr, PassDescr( p_name ) ) );

    RenderPassNode node( *it );
    return node;
};

void RenderPassNode::Erase( void )
{
    RenderPassNode::PassDescr* pass_descr = m_tree_node.data();

    _DRAWSPACE_DELETE_( pass_descr );
    m_tree_node.erase();
};