/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2020                     
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

#ifndef _RENDERPASSNODE_H_
#define _RENDERPASSNODE_H_

#include "st_tree.h"
#include "texture.h"
#include "viewportquad.h"
#include "renderingqueue.h"
#include "memalloc.h"


namespace DrawSpace
{
namespace RenderGraph
{
class RenderPassNode
{
private:
    class PassDescr
    {
    public:
        DrawSpace::Core::RenderingQueue*   m_renderingqueue;
        DrawSpace::ViewportQuad*           m_viewportquad;
        DrawSpace::Core::Texture*          m_targettexture;
        dsstring                           m_name;
        bool                               m_renderingqueue_update_flag;

    public:

        PassDescr( const dsstring& p_name ) :
            m_renderingqueue( NULL ),
            m_viewportquad( NULL ),
            m_targettexture( NULL ),
            m_name( p_name ),
            m_renderingqueue_update_flag( false )
        {
        };

        void CleanUp( void )
        {
            if( m_renderingqueue )
            {
                _DRAWSPACE_DELETE_( m_renderingqueue );
            }

            if( m_viewportquad )
            {
                _DRAWSPACE_DELETE_( m_viewportquad );
            }

            if( m_targettexture )
            {
                _DRAWSPACE_DELETE_( m_targettexture );
            }
        }
    };

    using PassDescrTree = st_tree::tree<RenderPassNode::PassDescr*>;

    PassDescrTree::node_type* m_tree_node;

    RenderPassNode( PassDescrTree::node_type* p_node );

public:

    static const int noTextureStageConnection = -1;

    RenderPassNode( void );
    

    RenderPassNode CreateChild( const dsstring& p_name, int p_targetstage, 
                                Core::Texture::RenderPurpose p_renderpurpose = Core::Texture::RENDERPURPOSE_COLOR, 
                                Core::Texture::RenderTarget p_rendertarget = Core::Texture::RENDERTARGET_GPU,
                                bool p_targetdims_fromrenderer = true, 
                                long p_targetdims_width = 255, 
                                long p_targetdims_height = 255 );

    void Erase( void );

    void CreateViewportQuad( dsreal p_z_offset = 0.0 );
    void RemoveViewportQuad( void );
    Core::RenderingQueue* GetRenderingQueue( void ) const;
    ViewportQuad* GetViewportQuad( void ) const;
    Core::Texture* GetTargetTexture( void ) const;

    void SetRenderingQueueUpdateFlag( void );

    friend class RenderPassNodeGraph;
};

}
}

#endif

