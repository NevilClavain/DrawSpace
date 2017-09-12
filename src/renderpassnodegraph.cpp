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

#include "renderpassnodegraph.h"
#include "memalloc.h"
#include "renderer.h"
#include "plugin.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

RenderPassNodeGraph::RenderPassNodeGraph( void )
{
}

RenderPassNodeGraph::~RenderPassNodeGraph( void )
{
    cleanup_treenodes();
}

void RenderPassNodeGraph::cleanup_treenodes( void )
{
    for( PassDescrTree::df_post_iterator it = m_tree.df_post_begin(); it != m_tree.df_post_end(); ++it ) 
    {
        it->data()->CleanUp();
        _DRAWSPACE_DELETE_( it->data() );
    }
}

RenderPassNode RenderPassNodeGraph::CreateRoot( const dsstring& p_name )
{
    // Creer PassDescr dont la cible rendu est l'ecran (final pass)

    Core::RenderingQueue* renderingqueue = _DRAWSPACE_NEW_( Core::RenderingQueue, Core::RenderingQueue );


    RenderPassNode::PassDescr* descr = _DRAWSPACE_NEW_( RenderPassNode::PassDescr, RenderPassNode::PassDescr( p_name ) );
    descr->m_renderingqueue = renderingqueue;

    m_tree.insert( descr );

    RenderPassNode node( m_tree.root() );
    return node;
}

// nettoie et efface toute la hierarchie
void RenderPassNodeGraph::Erase( void )
{
	cleanup_treenodes();
    m_tree.root().erase();
}

void RenderPassNodeGraph::CreateViewportQuad( dsreal p_z_offset )
{
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    DrawSpace::Interface::Renderer::Characteristics renderer_characteristics;
    renderer->GetRenderCharacteristics( renderer_characteristics );

    ViewportQuad* viewportquad = _DRAWSPACE_NEW_( ViewportQuad, ViewportQuad( renderer_characteristics.width_viewport, renderer_characteristics.height_viewport, p_z_offset ) );

    RenderPassNode::PassDescr* descr = m_tree.root().data();

    descr->m_viewportquad = viewportquad;
    descr->m_renderingqueue->Add( viewportquad );
}

RenderingQueue* RenderPassNodeGraph::GetRenderingQueue( void ) const
{
    RenderPassNode::PassDescr* descr = m_tree.root().data();
    return descr->m_renderingqueue;
}

void RenderPassNodeGraph::Run( void ) const
{
    for( PassDescrTree::df_post_iterator it = m_tree.df_post_begin(); it != m_tree.df_post_end(); ++it ) 
    {
        it->data()->m_renderingqueue->Draw();
    }
}