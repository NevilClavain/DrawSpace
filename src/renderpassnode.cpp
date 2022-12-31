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

    RenderPassNode::PassDescr* descr { _DRAWSPACE_NEW_(RenderPassNode::PassDescr, RenderPassNode::PassDescr(p_name)) };

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

    auto renderingqueue{ _DRAWSPACE_NEW_WITH_COMMENT(Core::RenderingQueue, Core::RenderingQueue(p_name, descr->m_targettexture), p_name) };
    descr->m_renderingqueue = renderingqueue;

    auto it { m_tree_node->insert(descr) };

    RenderPassNode::PassDescr* current_descr{ m_tree_node->data() };

    if( p_targetstage != noTextureStageConnection )
    {
        current_descr->m_viewportquad->SetTexture( descr->m_targettexture, p_targetstage );
        current_descr->m_renderingqueue_update_flag = true;
    }

    RenderPassNode node( &(*it) );
    return node;
};

void RenderPassNode::Erase( void )
{
    PassDescr* pass { m_tree_node->data() };
    if( !m_tree_node->is_root() )
    {
        PassDescr* parent_pass = m_tree_node->parent().data();
        for( long i = 0; i < parent_pass->m_viewportquad->GetTextureListSize(); i++ )
        {
            Texture* parent_texture = parent_pass->m_viewportquad->GetTexture( i );

            if( parent_texture != NULL && parent_texture == pass->m_targettexture )
            {
                parent_pass->m_viewportquad->SetTexture( NULL, i ); // supprimer la m_targettexture texture chez le parent; La m_targettexture en elle meme est proprement dessallou�e un peu plus bas lors du pass->CleanUp();
            }
        }
    }    
    pass->CleanUp();
    _DRAWSPACE_DELETE_( pass );
	m_tree_node->erase();
}

void RenderPassNode::CreateViewportQuad( dsreal p_z_offset )
{
    DrawSpace::Interface::Renderer* renderer { DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface };

    DrawSpace::Interface::Renderer::Characteristics renderer_characteristics;
    renderer->GetRenderCharacteristics( renderer_characteristics );

    ViewportQuad* viewportquad = _DRAWSPACE_NEW_( ViewportQuad, ViewportQuad( renderer_characteristics.width_viewport, renderer_characteristics.height_viewport, p_z_offset ) );

    PassDescr* descr { m_tree_node->data() };

    descr->m_viewportquad = viewportquad;

    descr->m_renderingqueue->Add( viewportquad );
    descr->m_renderingqueue_update_flag = true;
}

void RenderPassNode::RemoveViewportQuad( void )
{
    PassDescr* descr { m_tree_node->data() };

    descr->m_renderingqueue->Remove( descr->m_viewportquad );
    descr->m_renderingqueue_update_flag = true;

    _DRAWSPACE_DELETE_( descr->m_viewportquad );
}

RenderingQueue* RenderPassNode::GetRenderingQueue( void ) const
{
    PassDescr* descr { m_tree_node->data() };
    return descr->m_renderingqueue;
}

ViewportQuad* RenderPassNode::GetViewportQuad(void) const
{
    PassDescr* descr { m_tree_node->data() };
    return descr->m_viewportquad;
}

Texture* RenderPassNode::GetTargetTexture( void ) const
{
    PassDescr* descr { m_tree_node->data() };
    return descr->m_targettexture;
}

void RenderPassNode::SetRenderingQueueUpdateFlag( void )
{
    PassDescr* descr { m_tree_node->data() };
    descr->m_renderingqueue_update_flag = true;
}
