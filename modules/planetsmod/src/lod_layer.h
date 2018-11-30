#pragma once

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

#include "lod_drawing.h"
#include "orbiter.h"
#include "collider.h"
#include "camerapoint.h"
#include "inertbody.h"
#include "rocket.h"
#include "mediator.h"
#include "runner.h"
#include "task.h"

namespace LOD
{

// fwd declaration
class Root;
class Collisions;

class Layer
{
public:    
    using SubPassCreationHandler = DrawSpace::Core::BaseCallback2<SubPass::EntryInfos, SubPass*, int>;

protected:
    using PatchUpdateCb = DrawSpace::Core::CallBack2<Layer, void, Patch*, int>;


    DrawSpace::Dynamics::World*                                 m_world;

    Config*                                                     m_config;

    Body*                                                       m_body;
    DrawSpace::Dynamics::Collider*                              m_collider;
    
    long                                                        m_nb_collisionmeshebuild_done;

    PatchUpdateCb*                                              m_patch_update_cb;
    
    bool                                                        m_collision_state;

    DrawSpace::Utils::Mutex                                     m_meshe_ready_mutex;
    bool                                                        m_meshe_ready;

    dsreal                                                      m_planetray;

    bool                                                        m_hot;

    DrawSpace::Dynamics::CameraPoint*                           m_camera;
    DrawSpace::Dynamics::InertBody*                             m_inertbody;

    bool                                                        m_collisions;

    Patch*                                                      m_current_patch;
    
    bool                                                        m_draw_collidinghm;

    Collisions*                                                 m_collisions_hms[6];
    Collisions*                                                 m_current_collisions_hm;

    // pos point de vue camera par rapport au centre layer
    // mis a jour quand on est relatif
    DrawSpace::Utils::Vector                                    m_relative_hotviewerpos;

    Layer::SubPassCreationHandler*                              m_handler;

    DrawSpace::Dynamics::Orbiter*                               m_owner;

    dsreal                                                      m_currentpatch_max_height;
    dsreal                                                      m_currentpatch_min_height;
    dsreal                                                      m_currentpatch_current_height;

    dsreal                                                      m_alt_grid[PATCH_RESOLUTION * PATCH_RESOLUTION];

    void on_patchupdate( Patch* p_patch, int p_patch_lod );
    
    void build_meshe( DrawSpace::Core::Meshe& p_patchmeshe, Patch* p_patch, DrawSpace::Core::Meshe& p_outmeshe, float* p_heightmap );

    dsreal get_interpolated_height( dsreal p_coord_x, dsreal p_coord_y );

public:

    Layer( DrawSpace::Dynamics::Orbiter* p_owner, Config* p_config, DrawSpace::Dynamics::World* p_world, Body* p_planetbody, 
                DrawSpace::Dynamics::Collider* p_collider, Layer::SubPassCreationHandler* p_handler, int p_index );
    virtual ~Layer( void );

    DrawSpace::Dynamics::CameraPoint*       GetCamera(void) const;
    DrawSpace::Dynamics::InertBody*         GetInertBody(void) const;
    Body*                                   GetBody(void) const;
    dsreal                                  GetLastMaxHeight(void) const;
    dsreal                                  GetCurrentHeight(void) const;
    Body*                                   GetSlodBody(void) const;
    DrawSpace::Dynamics::Collider*          GetCollider(void) const;
    void                                    GetCollisionMesheBuildStats(long& p_nb_collisionmeshebuild_done) const;

    void Compute( Root* p_owner );

    void SetHotState( bool p_hotstate );
    bool GetHotState( void );
    void SetCamera( DrawSpace::Dynamics::CameraPoint* p_camera );
    void SetInertBody( DrawSpace::Dynamics::InertBody* p_body );

    void RemoveColliderFromWorld( void );

    void UpdateRelativeAlt( dsreal p_alt );

    void ResetBody( void );

    void UpdateRelativeHotViewerPos( const DrawSpace::Utils::Vector& p_pos );

    Layer::SubPassCreationHandler* GetSubPassCreationHandler( void );

    virtual void SubPassDone( Collisions* p_collider );

    bool HasCollisions( void );
};
}
