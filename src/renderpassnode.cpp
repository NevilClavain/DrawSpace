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
#include "renderer.h"
#include "plugin.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;


RenderPassNode::RenderPassNode( st_tree::tree<RenderPassNode::PassDescr*>::node_type& p_node ) :
    m_tree_node( p_node )
{
};

RenderPassNode RenderPassNode::CreateChild( const dsstring& p_name, int p_targetstage )
{
    // Creer PassDescr dont la cible rendu est une texture (intermediate pass)

    RenderPassNode::PassDescr* descr = _DRAWSPACE_NEW_( RenderPassNode::PassDescr, RenderPassNode::PassDescr( p_name ) );

    Core::RenderingQueue* renderingqueue = _DRAWSPACE_NEW_( Core::RenderingQueue, Core::RenderingQueue );
    descr->m_renderingqueue = renderingqueue;

    // TODO : creer la texture cible

    st_tree::tree<RenderPassNode::PassDescr*>::node_type::iterator it = m_tree_node.insert( descr );


    // TODO : connecter les deux passes

    RenderPassNode::PassDescr* current_descr = m_tree_node.data();

    current_descr->m_viewportquad->SetTexture( descr->m_targettexture, p_targetstage );

    RenderPassNode node( *it );
    return node;
};

void RenderPassNode::Erase( void )
{
    RenderPassNode::PassDescr* pass_descr = m_tree_node.data();

    _DRAWSPACE_DELETE_( pass_descr );
    m_tree_node.erase();
};

void RenderPassNode::CreateViewportQuad( dsreal p_z_offset )
{
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    DrawSpace::Interface::Renderer::Characteristics renderer_characteristics;
    renderer->GetRenderCharacteristics( renderer_characteristics );

    ViewportQuad* viewportquad = _DRAWSPACE_NEW_( ViewportQuad, ViewportQuad( renderer_characteristics.width_viewport, renderer_characteristics.height_viewport, p_z_offset ) );

    //m_renderingqueue->Add( m_viewportquad );

    PassDescr* descr = m_tree_node.data();

    descr->m_viewportquad = viewportquad;
    descr->m_renderingqueue->Add( viewportquad );
}

RenderingQueue* RenderPassNode::GetRenderingQueue( void ) const
{
    PassDescr* descr = m_tree_node.data();
    return descr->m_renderingqueue;
}