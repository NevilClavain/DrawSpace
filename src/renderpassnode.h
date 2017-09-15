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

#ifndef _RENDERPASSNODE_H_
#define _RENDERPASSNODE_H_

#include "st_tree.h"
#include "texture.h"
#include "viewportquad.h"
#include "renderingqueue.h"
#include "memalloc.h"


namespace DrawSpace
{
namespace Core
{
class RenderPassNode sealed
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
            _DRAWSPACE_DELETE_( m_renderingqueue );

            if( m_viewportquad )
            {
                _DRAWSPACE_DELETE_( m_viewportquad );
            }

            _DRAWSPACE_DELETE_( m_targettexture );            
        }
    };

    using PassDescrTree = st_tree::tree<RenderPassNode::PassDescr*>;

    PassDescrTree::node_type* m_tree_node;

public:

    RenderPassNode( void );
    RenderPassNode( PassDescrTree::node_type* p_node );

    RenderPassNode CreateChild( const dsstring& p_name, int p_targetstage, 
                                Core::Texture::RenderPurpose p_renderpurpose = Texture::RENDERPURPOSE_COLOR, 
                                Core::Texture::RenderTarget p_rendertarget = Texture::RENDERTARGET_GPU,
                                bool p_targetdims_fromrenderer = true, 
                                long p_targetdims_width = 255, 
                                long p_targetdims_height = 255 );

    void CreateViewportQuad( dsreal p_z_offset = 0.0 );
    RenderingQueue* GetRenderingQueue( void ) const;
    ViewportQuad* GetViewportQuad( void ) const;

    friend class RenderPassNodeGraph;
    friend class RenderingSystem;
};

}
}

#endif
