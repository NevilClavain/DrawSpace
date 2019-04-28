/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2019                     
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

#ifndef _IMPOSTORSRENDERINGASPECTIMPL_H_
#define _IMPOSTORSRENDERINGASPECTIMPL_H_

#include "renderingaspectimpl.h"
#include "renderer.h"
#include "plugin.h"
#include "renderingnode.h"
#include "renderpassnodegraph.h"

namespace DrawSpace
{
namespace AspectImplementations
{
class ImpostorsRenderingAspectImpl : public DrawSpace::Interface::AspectImplementations::RenderingAspectImpl
{
public:

    struct ImpostorDescriptor
    {
        dsreal                      width_scale;
        dsreal                      height_scale;
        DrawSpace::Utils::Vector    localpos;
        dsreal                      u1, v1;
        dsreal                      u2, v2;
        dsreal                      u3, v3;
        dsreal                      u4, v4;     
    };

    class PassSlot
    {
    private:
        typedef DrawSpace::Core::CallBack<PassSlot, void, DrawSpace::Core::RenderingNode*>                                 RenderingNodeDrawCallback;

        dsstring                                m_pass_name;
        DrawSpace::Core::RenderingNode*         m_rendering_node;  
        RenderingNodeDrawCallback*              m_cb;
        DrawSpace::Interface::Renderer*         m_renderer;

        virtual void on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node );

    public:

        Utils::Matrix                           m_world;
        Utils::Matrix                           m_view;
        Utils::Matrix                           m_proj;

        PassSlot( const dsstring& p_pass_name );
        ~PassSlot( void );

        DrawSpace::Core::RenderingNode* GetRenderingNode( void ) const { return m_rendering_node; };
        
        friend class ImpostorsRenderingAspectImpl;
    };

private:

    void build_quads( const PassSlot& p_pass_slot );

    bool                                m_add_in_rendergraph;

    
public:
    ImpostorsRenderingAspectImpl( void );

    bool VisitRenderPassDescr( const dsstring& p_name, DrawSpace::Core::RenderingQueue* p_passqueue );

    void RegisterToRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph );
    void UnregisterFromRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph );

    bool Init( DrawSpace::Core::Entity* p_entity, DrawSpace::Utils::TimeManager* p_timemanager) { return true; };
    void Release(void) {};
    void Run( DrawSpace::Core::Entity* p_entity );

    void SetEntityNodeGraph(EntityGraph::EntityNodeGraph* p_entitynodegraph) {};
};
}
}

#endif
