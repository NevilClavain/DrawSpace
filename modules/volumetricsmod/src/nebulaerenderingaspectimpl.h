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
protected:
    // data model
    using UVPairList = std::vector<std::pair<int, int>>;
    using Bloc = std::tuple<DrawSpace::Utils::Vector, DrawSpace::Utils::Vector, dsreal, UVPairList, UVPairList>;
    using DataModel = std::vector<Bloc>;

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

        //static const int atlasResolution = 8;
        //static const int maskAtlasResolution = 4;

        dsstring                                                m_pass_name;
        RenderingNodeDrawCallback*                              m_cb;
        DrawSpace::Interface::Renderer*                         m_renderer;

        DrawSpace::Core::RenderingNode*                         m_rendering_node;

        int                                                     m_uv_index;
        int                                                     m_uv_mask_index;



        std::vector<std::pair<DrawSpace::Core::Meshe*, void*>> m_meshes;

        virtual void on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node );

        void        create_axis_quad(DrawSpace::Core::Meshe* p_meshe, int p_texture_atlas_resolution, int p_mask_atlas_resolution, const Utils::Vector& p_pos, const Utils::Vector& p_color, dsreal p_scale, QuadAxis p_axis, int p_angle_step, const UVPairList& p_uvs, const UVPairList& p_uvs_mask, int& p_nb_vertex);
        
        //// temp
        void        generate_uvcoords(int p_atlasResolution, int& p_u_count, int& p_v_count, dsreal& p_u1, dsreal& p_v1, dsreal& p_u2, dsreal& p_v2);

        void        create_bloc(DrawSpace::Core::Meshe* p_meshe, int p_texture_atlas_resolution, int p_mask_atlas_resolution, const Utils::Vector& p_pos, const Utils::Vector& p_color, dsreal p_scale, const UVPairList& p_uvs, const UVPairList& p_uvs_mask, int& p_nb_vertex);

    public:

        Utils::Matrix                           m_world;
        Utils::Matrix                           m_view;
        Utils::Matrix                           m_proj;

       

        PassSlot( const dsstring& p_pass_name, const DataModel& p_dataModel, int p_textureAtlasResolution, int p_maskAtlasResolution);
        ~PassSlot( void );

        DrawSpace::Core::RenderingNode* GetRenderingNode( void ) const { return m_rendering_node; };

        friend class NebulaeRenderingAspectImpl;
    };

protected:

    DataModel               m_data_model;

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

    bool Init( DrawSpace::Core::Entity* p_entity, DrawSpace::Utils::TimeManager* p_timemanager) { return true; };
    void Release(void) {};
    void Run( DrawSpace::Core::Entity* p_entity );
    void SetEntityNodeGraph(EntityGraph::EntityNodeGraph* p_entitynodegraph) {};
};
}
}
#endif
