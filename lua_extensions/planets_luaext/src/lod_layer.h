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

#include "lod_patch.h"
#include "lod_heightmapsubpass.h"

#include "collisionaspect.h"
#include "csts.h"

namespace DrawSpace
{
namespace EntityGraph
{
class EntityNodeGraph;
}

namespace Aspect
{
class CollisionAspect;
}
}
namespace LOD
{
struct Config;
class Body;

class Layer
{
public:

    using CollisionMesheUpdateHandler       = DrawSpace::Core::BaseCallback3<void, dsstring, DrawSpace::Aspect::CollisionAspect::MesheCollisionShape, bool>;
    using NewCollisionMesheCreationHandler  = DrawSpace::Core::BaseCallback<void, const DrawSpace::Core::Meshe&>;

    Layer(DrawSpace::EntityGraph::EntityNodeGraph* p_eg, Config* p_config, Body* p_body,
            HeighmapSubPass::SubPassCreationHandler* p_subpass_creation_handler,
            CollisionMesheUpdateHandler* p_collision_meshe_update_handler,
            int p_index, bool p_freecamera);

    ~Layer(void);

    Body*                           GetBody(void) const;
    bool                            GetHotState(void) const;

    int                             GetCurrentLOD(void) const;
    
    void                            SetHotState(bool p_hotstate);
    void                            UpdateRelativeAlt(dsreal p_alt);
    void                            UpdateInvariantViewerPos(const DrawSpace::Maths::Vector& p_pos);
    void                            UpdateHotPoint( const DrawSpace::Maths::Vector& p_vector );
    void                            Compute( void );

    
    void                            ResetBody(void);

    void                            RegisterNewCollisionMesheCreationHandler(NewCollisionMesheCreationHandler* p_handler);

    void                            RemoveCollider(void);

    dsreal                          GetCurrentPatchMaxHeight(void) const;
    dsreal                          GetCurrentPatchMinHeight(void) const;
    dsreal                          GetCurrentPatchCurrentHeight(void) const;

    int                             GetLayerIndex(void) const;

    dsreal                          GetCurrentPatchTemperature(void) const;
    dsreal                          GetCurrentPatchHumidity(void) const;


private:

    using SubpassDoneCb = DrawSpace::Core::CallBack<Layer, void, HeighmapSubPass*>;
    using SubpassAbortedCb = DrawSpace::Core::CallBack<Layer, void, HeighmapSubPass*>;

    DrawSpace::EntityGraph::EntityNodeGraph*                    m_entitynodegraph{ nullptr };

    Config* m_config{ nullptr };
    Body* m_body{ nullptr };

    SubPass::SubPassCreationHandler*                            m_subpass_creation_handler{ nullptr };

    CollisionMesheUpdateHandler*                                m_collision_meshe_update_handler{ nullptr };
    bool                                                        m_hot;
    int                                                         m_current_lod;

    std::vector<NewCollisionMesheCreationHandler*>              m_collision_meshe_creation_handler;

    dsreal                                                      m_planetray;
    bool                                                        m_collisions{ false };



    dsstring                                                    m_description; // for debug purpose :)

    dsreal                                                      m_currentpatch_max_height{ -2.0 };
    dsreal                                                      m_currentpatch_min_height{ -2.0 };
    dsreal                                                      m_currentpatch_current_height{ -2.0 };

    dsreal                                                      m_currentpatch_temperature{ -0.666 };
    dsreal                                                      m_currentpatch_humidity{ -0.666 };

    dsreal                                                      m_alt_grid[cst::patchResolution * cst::patchResolution];

    DrawSpace::Core::Meshe                                      m_hm_meshe; // meshe produced with heightmap result and used in bullet
    DrawSpace::Aspect::CollisionAspect::MesheCollisionShape     m_meshe_collision_shape;
    bool                                                        m_collisions_active{ false };

    Patch* m_current_patch{ nullptr };

    int                                                         m_layer_index;

    std::map<LOD::HeighmapSubPass*, Patch*>                     m_heightmap_source_patches;

    SubpassDoneCb                                               m_subpassDoneCb;
    SubpassAbortedCb                                            m_subpassAbortedCb;


    void build_meshe(float* p_heightmap, DrawSpace::Core::Meshe& p_patchmeshe, LOD::Patch* p_patch, DrawSpace::Core::Meshe& p_outmeshe);
    dsreal get_interpolated_height(dsreal p_coord_x, dsreal p_coord_y);

    void generate_heightmap(Patch* p_patch, LOD::HeighmapSubPass::Purpose p_purpose);

    void on_subpassdone(LOD::HeighmapSubPass* p_subpass);
    void on_subpassaborted(LOD::HeighmapSubPass* p_subpass);
};
}
