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

#ifndef _MESHERENDERINGASPECTIMPL_H_
#define _MESHERENDERINGASPECTIMPL_H_

#include "renderingaspectimpl.h"
#include "renderer.h"
#include "plugin.h"
#include "renderingnode.h"
#include "renderpassnodegraph.h"

namespace DrawSpace
{
namespace Core
{
class MesheRenderingAspectImpl : public RenderingAspectImpl
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

        Utils::Matrix                       m_world;
        Utils::Matrix                       m_view;
        Utils::Matrix                       m_proj;

        virtual void on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node );

    public:
        PassSlot( const dsstring& p_pass_name );
        ~PassSlot( void );

        RenderingNode* GetRenderingNode( void ) { return m_rendering_node; };

        friend class MesheRenderingAspectImpl;
    };
    
protected:

    bool                                m_add_in_rendergraph;

    virtual void run( Entity* p_entity );

public:
    MesheRenderingAspectImpl( void );

    bool VisitRenderPassDescr( const dsstring& p_name, RenderingQueue* p_passqueue );

    void RegisterToRendering( RenderPassNodeGraph& p_rendergraph );
    void UnregisterFromRendering( RenderPassNodeGraph& p_rendergraph );

};
}
}

#endif