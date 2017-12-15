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
#include "renderer.h"
#include "plugin.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::RenderGraph;

RenderPassNode::RenderPassNode( void ) :
    m_tree_node( NULL )
{
}

RenderPassNode::RenderPassNode( PassDescrTree::node_type* p_node ) :
    m_tree_node( p_node )
{
}

RenderPassNode RenderPassNode::CreateChild( const dsstring& p_name, int p_targetstage, 
                                                Core::Texture::RenderPurpose p_renderpurpose, 
                                                Core::Texture::RenderTarget p_rendertarget,
                                                bool p_targetdims_fromrenderer, 
                                                long p_targetdims_width, 
                                                long p_targetdims_height )
{
    // Creer PassDescr dont la cible rendu est une texture (intermediate pass)

    RenderPassNode::PassDescr* descr = _DRAWSPACE_NEW_( RenderPassNode::PassDescr, RenderPassNode::PassDescr( p_name ) );

    long h_resol, w_resol;

    if( p_targetdims_fromrenderer )
    {
        DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

        DrawSpace::Interface::Renderer::Characteristics renderer_characteristics;
        renderer->GetRenderCharacteristics( renderer_characteristics );

        h_resol = renderer_characteristics.height_resol;
        w_resol = renderer_characteristics.width_resol;

    }
    else
    {
        h_resol = p_targetdims_height;
        w_resol = p_targetdims_width;
    }

    descr->m_targettexture = _DRAWSPACE_NEW_( Texture, Texture( p_name + dsstring( "/target" ), true, w_resol, h_resol, p_renderpurpose, p_rendertarget ) );

    Core::RenderingQueue* renderingqueue = _DRAWSPACE_NEW_( Core::RenderingQueue, Core::RenderingQueue( descr->m_targettexture ) );
    descr->m_renderingqueue = renderingqueue;

    PassDescrTree::node_type::iterator it = m_tree_node->insert( descr );

    RenderPassNode::PassDescr* current_descr = m_tree_node->data();

    if( p_targetstage != noTextureStageConnection )
    {
        current_descr->m_viewportquad->SetTexture( descr->m_targettexture, p_targetstage );
    }

    RenderPassNode node( &(*it) );
    return node;
};

void RenderPassNode::CreateViewportQuad( dsreal p_z_offset )
{
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    DrawSpace::Interface::Renderer::Characteristics renderer_characteristics;
    renderer->GetRenderCharacteristics( renderer_characteristics );

    ViewportQuad* viewportquad = _DRAWSPACE_NEW_( ViewportQuad, ViewportQuad( renderer_characteristics.width_viewport, renderer_characteristics.height_viewport, p_z_offset ) );

    PassDescr* descr = m_tree_node->data();

    descr->m_viewportquad = viewportquad;

    descr->m_renderingqueue->Add( viewportquad );
    descr->m_renderingqueue_update_flag = true;
}

RenderingQueue* RenderPassNode::GetRenderingQueue( void ) const
{
    PassDescr* descr = m_tree_node->data();
    return descr->m_renderingqueue;
}

ViewportQuad* RenderPassNode::GetViewportQuad( void ) const
{
    PassDescr* descr = m_tree_node->data();
    return descr->m_viewportquad;
}

Texture* RenderPassNode::GetTargetTexture( void ) const
{
    PassDescr* descr = m_tree_node->data();
    return descr->m_targettexture;
}