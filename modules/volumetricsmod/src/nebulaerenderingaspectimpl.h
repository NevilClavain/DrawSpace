/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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


#ifndef _NEBULAERENDERINGASPECTIMPL_H_
#define _NEBULAERENDERINGASPECTIMPL_H_

#include "renderingaspectimpl.h"
#include "renderer.h"
#include "plugin.h"
#include "renderingnode.h"
#include "renderpassnodegraph.h"


namespace DrawSpace
{
namespace AspectImplementations
{
class NebulaeRenderingAspectImpl : public DrawSpace::Interface::AspectImplementations::RenderingAspectImpl
{
public:
    
    class PassSlot
    {
    private:

        using QuadAxis = enum
        {
            X_AXIS,
            Y_AXIS,
            Z_AXIS
        };

        using RenderingNodeDrawCallback = DrawSpace::Core::CallBack<PassSlot, void, DrawSpace::Core::RenderingNode*>;

        static const int atlasResolution = 8;

        dsstring                                m_pass_name;        
        RenderingNodeDrawCallback*              m_cb;
        DrawSpace::Interface::Renderer*         m_renderer;

        DrawSpace::Core::RenderingNode*         m_rendering_node;
        DrawSpace::Core::Meshe*                 m_meshe;

        virtual void on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node );

        void        create_axis_quad( QuadAxis p_axis, int p_angle_step, int& p_nb_vertex );

        void        generate_uvcoords(dsreal& p_u1, dsreal& p_v1, dsreal& p_u2, dsreal& p_v2);
    public:

        Utils::Matrix                           m_world;
        Utils::Matrix                           m_view;
        Utils::Matrix                           m_proj;

       

        PassSlot( const dsstring& p_pass_name );
        ~PassSlot( void );

        DrawSpace::Core::RenderingNode* GetRenderingNode( void ) const { return m_rendering_node; };

        friend class NebulaeRenderingAspectImpl;
    };

protected:

    std::vector<PassSlot*>  m_pass_slots;

    bool                    m_add_in_rendergraph;

    void        init_rendering_objects( void );
    void        release_rendering_objects( void );

    void        update_shader_params( void ); // for all passes
   
public:
    NebulaeRenderingAspectImpl( void );

    bool VisitRenderPassDescr( const dsstring& p_name, DrawSpace::Core::RenderingQueue* p_passqueue );

    void RegisterToRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph );
    void UnregisterFromRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph );

    virtual bool Init( DrawSpace::Core::Entity* p_entity ) { return true; };
    virtual void Run( DrawSpace::Core::Entity* p_entity );
};
}
}
#endif
