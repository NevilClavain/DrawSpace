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

#ifndef _MESHERENDERINGASPECT_H_
#define _MESHERENDERINGASPECT_H_

#include "aspect.h"
#include "renderer.h"
#include "plugin.h"
#include "renderingnode.h"
#include "renderpassnodegraph.h"

namespace DrawSpace
{
namespace Core
{
class MesheRenderingAspect : public Aspect
{
public:
    class PassSlot
    {
    private:
        typedef DrawSpace::Core::CallBack<PassSlot, void, DrawSpace::Core::RenderingNode*>                                 RenderingNodeDrawCallback;

        dsstring                            m_pass_name;
        RenderingNode*                      m_rendering_node;
        RenderingNodeDrawCallback*          m_cb;
        DrawSpace::Interface::Renderer*     m_renderer;

        virtual void on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node );

    public:
        PassSlot( const dsstring& p_pass_name ):
            m_pass_name( p_pass_name )
        {
            m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

            m_rendering_node = _DRAWSPACE_NEW_( RenderingNode, RenderingNode );

            m_cb = _DRAWSPACE_NEW_( RenderingNodeDrawCallback, RenderingNodeDrawCallback( this, &PassSlot::on_renderingnode_draw ) );
            m_rendering_node->RegisterHandler( m_cb );
        }

        ~PassSlot( void )
        {
            _DRAWSPACE_DELETE_( m_rendering_node );
        }

        RenderingNode* GetRenderingNode( void ) { return m_rendering_node; };

        friend class MesheRenderingAspect;
    };
    
protected:

    bool                                m_add_in_rendergraph;
        

public:
    MesheRenderingAspect( void );

    bool VisitRenderPassDescr( const dsstring& p_name, RenderingQueue* p_passqueue );

    void RegisterToRendering( RenderPassNodeGraph* p_rendergraph );
    void UnregisterFromRendering( RenderPassNodeGraph* p_rendergraph );
    
};

}
}

#endif