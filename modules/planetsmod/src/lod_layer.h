/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2021                     
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
#include "lod_collisions.h"
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
private:

    using PatchUpdateCb = DrawSpace::Core::CallBack2<Layer, void, Patch*, int>;

public:

    using SubPassCreationHandler = DrawSpace::Core::BaseCallback2<SubPass::EntryInfos, SubPass*, SubPass::Destination>;

    using NewCollisionMesheCreationHandler = DrawSpace::Core::BaseCallback<void, const DrawSpace::Core::Meshe&>;

private:

    DrawSpace::Core::Entity*                                    m_owner_entity{ nullptr };
    DrawSpace::EntityGraph::EntityNodeGraph*                    m_entitynodegraph{ nullptr };

    Config*                                                     m_config{ nullptr };
    Body*                                                       m_body{ nullptr };
    SubPassCreationHandler*                                     m_handler{ nullptr };
    bool                                                        m_hot;
    int                                                         m_current_lod;

    std::vector<NewCollisionMesheCreationHandler*>              m_collision_meshe_creation_handler;

    dsreal                                                      m_planetray;
    bool                                                        m_collisions;

    LOD::Collisions*                                            m_collisions_hms[6];
    LOD::Collisions*                                            m_current_collisions_hm{ nullptr };

    LOD::Patch*                                                 m_collision_patch{ nullptr };

    bool                                                        m_draw_collidinghm{ false };
    bool                                                        m_collision_state{ false };

    PatchUpdateCb                                               m_patch_update_cb;

    dsstring                                                    m_description; // for debug purpose :)


    DrawSpace::Aspect::CollisionAspect*                         m_collision_aspect{ nullptr };

    dsreal                                                      m_currentpatch_max_height{ -2.0 };
    dsreal                                                      m_currentpatch_min_height{ -2.0 };
    dsreal                                                      m_currentpatch_current_height{ -2.0 };

    dsreal                                                      m_alt_grid[cst::patchResolution * cst::patchResolution];

    DrawSpace::Core::Meshe                                      m_hm_meshe; // meshe produced with heightmap result and used in bullet
    DrawSpace::Aspect::CollisionAspect::MesheCollisionShape     m_meshe_collision_shape;

    void on_patchupdate(Patch* p_patch, int p_patch_lod);
    void build_meshe(DrawSpace::Core::Meshe& p_patchmeshe, LOD::Patch* p_patch, DrawSpace::Core::Meshe& p_outmeshe, float* p_heightmap);
    dsreal get_interpolated_height(dsreal p_coord_x, dsreal p_coord_y);

    void setup_collider(void);
    void remove_collider(void);


public:
    Layer(DrawSpace::Core::Entity* p_entity, DrawSpace::EntityGraph::EntityNodeGraph* p_eg, 
                    Config* p_config, Body* p_body, Layer::SubPassCreationHandler* p_handler, int p_index );

    ~Layer(void);

    Body*                           GetBody(void) const;
    bool                            GetHotState(void) const;
    Layer::SubPassCreationHandler*  GetSubPassCreationHandler(void) const;
    int                             GetCurrentLOD(void) const;
    
    void                            SetHotState(bool p_hotstate);
    void                            UpdateRelativeAlt(dsreal p_alt);
    void                            UpdateInvariantViewerPos(const DrawSpace::Utils::Vector& p_pos);
    void                            UpdateHotPoint( const DrawSpace::Utils::Vector& p_vector );
    void                            Compute( void );
    void                            SubPassDone(LOD::Collisions* p_collider);
    void                            ResetBody(void);

    void                            RegisterNewCollisionMesheCreationHandler(NewCollisionMesheCreationHandler* p_handler);

    void                            RemoveCollider(void);

    dsreal                          GetCurrentPatchMaxHeight(void) const;
    dsreal                          GetCurrentPatchMinHeight(void) const;
    dsreal                          GetCurrentPatchCurrentHeight(void) const;
};
}
