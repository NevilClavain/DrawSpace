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


    // ici on a besoin d'un renderingNode, mais on ne veut pas que le meshe de celui ci soit directement
    // accessible -> utilisation d'une classe proxy, qui reprende la meme api que RenderingNode, sauf en ce qui concerne le meshe
    class RenderingNodeProxy
    {
    private:
        DrawSpace::Core::RenderingNode*         m_rendering_node;

    public:
        RenderingNodeProxy( DrawSpace::Core::RenderingNode* p_rendering_node );
        ~RenderingNodeProxy( void );

        Core::Fx* GetFx( void );
        void SetFx( Core::Fx* p_fx );

        void SetTexture( Core::Texture* p_texture, long p_stage );
        void SetVertexTexture( Core::Texture* p_texture, long p_stage );

        Core::Texture* GetTexture( long p_index );
        Core::Texture* GetVertexTexture( long p_index );

        Core::Meshe* GetMeshe( void );

        long GetOrderNumber( void );
        void SetOrderNumber( long p_order );

        void SetDrawingState( bool p_drawing );

        void AddShaderParameter( long p_shader_index, const dsstring& p_id, long p_register );
        void SetShaderReal( const dsstring& p_id, dsreal p_value );
        void SetShaderRealVector( const dsstring& p_id, const Utils::Vector& p_value );
        void SetShaderRealMatrix( const dsstring& p_id, const Utils::Matrix& p_value );
        void SetShaderBool( const dsstring& p_id, bool p_value );

        friend class PassSlot;
    };

    class PassSlot
    {
    private:
        typedef DrawSpace::Core::CallBack<PassSlot, void, DrawSpace::Core::RenderingNode*>                                 RenderingNodeDrawCallback;

        dsstring                                m_pass_name;
        DrawSpace::Core::RenderingNode*         m_rendering_node;
        RenderingNodeProxy*                     m_rendering_node_proxy;
        RenderingNodeDrawCallback*              m_cb;
        DrawSpace::Interface::Renderer*         m_renderer;

        virtual void on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node );

    public:

        Utils::Matrix                           m_world;
        Utils::Matrix                           m_view;
        Utils::Matrix                           m_proj;

        PassSlot( const dsstring& p_pass_name );
        ~PassSlot( void );

        RenderingNodeProxy* GetRenderingNodeProxy( void ) const { return m_rendering_node_proxy; };
        
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

    virtual void Run( DrawSpace::Core::Entity* p_entity );
};
}
}

#endif