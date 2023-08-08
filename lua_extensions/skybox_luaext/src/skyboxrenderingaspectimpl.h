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

#pragma once

#include "renderingaspectimpl.h"
#include "renderer.h"
#include "plugin.h"
#include "renderingnode.h"
#include "renderpassnodegraph.h"

class SkyboxRenderingAspectImpl : public DrawSpace::Interface::AspectImplementations::RenderingAspectImpl
{
public:
    
    class PassSlot
    {
    public:
        static const int    FrontQuad    = 0;
	    static const int    RearQuad     = 1;
	    static const int    LeftQuad     = 2;
	    static const int    RightQuad    = 3;
	    static const int    TopQuad      = 4;
	    static const int    BottomQuad   = 5;

    private:
        using RenderingNodeDrawCallback = DrawSpace::Core::CallBack<PassSlot, void, DrawSpace::Core::RenderingNode*>;

        dsstring                                m_pass_name;        
        RenderingNodeDrawCallback*              m_cb            { nullptr };
        DrawSpace::Interface::Renderer*         m_renderer      { nullptr };

        DrawSpace::Core::RenderingNode*         m_rendering_node[6];
        DrawSpace::Core::Meshe*                 m_meshes[6];

        virtual void on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node );

    public:

        DrawSpace::Maths::Matrix                m_world;
        DrawSpace::Maths::Matrix                m_view;
        DrawSpace::Maths::Matrix                m_proj;
      
        PassSlot( const dsstring& p_pass_name );
        ~PassSlot( void );

        DrawSpace::Core::RenderingNode* GetRenderingNode( int p_quad ) const { return m_rendering_node[p_quad]; };

        friend class SkyboxRenderingAspectImpl;
    };

private:
    bool                    m_add_in_rendergraph;

public:
    SkyboxRenderingAspectImpl( void );

    bool VisitRenderPassDescr( const dsstring& p_name, DrawSpace::Core::RenderingQueue* p_passqueue );

    void RegisterToRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph );
    void UnregisterFromRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph );

    bool Init(DrawSpace::Core::Entity* p_entity, DrawSpace::TimeManager* p_timemanager) { return true; };
    void Release(void) {};
    void Run(DrawSpace::Core::Entity* p_entity);
    void SetEntityNodeGraph(DrawSpace::EntityGraph::EntityNodeGraph* p_entitynodegraph) {};
};


