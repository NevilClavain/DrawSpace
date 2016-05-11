/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2016                              
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

#ifndef _SPHERELOD_LAYER_H_
#define _SPHERELOD_LAYER_H_

#include "spherelod_drawing.h"
#include "spherelod_collisions.h"
//#include "spherelod_subpass.h"
#include "orbiter.h"
#include "collider.h"
#include "camerapoint.h"
#include "inertbody.h"
#include "rocket.h"
#include "mediator.h"
#include "runner.h"
#include "task.h"

namespace DrawSpace
{

// fwd declaration
namespace Planetoid
{
    class Body;
}

namespace SphericalLOD
{

class Layer
{
public:    
    typedef DrawSpace::Core::BaseCallback2<DrawSpace::SphericalLOD::SubPass::EntryInfos, DrawSpace::SphericalLOD::SubPass*, int>                SubPassCreationHandler;

protected:
    typedef DrawSpace::Core::CallBack2<Layer, void, DrawSpace::SphericalLOD::Patch*, int>                                                    PatchUpdateCb;

    typedef DrawSpace::Core::CallBack<Layer, void, DrawSpace::Core::PropertyPool*>                                                           RunnerMsgCb;
    typedef DrawSpace::Core::CallBack<Layer, void, Core::Runner::State>                                                                      RunnerEvtCb;

    DrawSpace::Dynamics::World*                                 m_world;

    DrawSpace::SphericalLOD::Config*                            m_config;

    DrawSpace::SphericalLOD::Body*                              m_planetbody;

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

    SphericalLOD::Patch*                                        m_current_patch;
    
    bool                                                        m_draw_collidinghm;

    DrawSpace::SphericalLOD::Collisions*                        m_collisions_hms[6];
    DrawSpace::SphericalLOD::Collisions*                        m_current_collisions_hm;

    // pos point de vue camera par rapport au centre layer
    // mis a jour quand on est relatif
    DrawSpace::Utils::Vector                                    m_relative_hotviewerpos;

    Layer::SubPassCreationHandler*                             m_handler;

    Dynamics::Orbiter*                                          m_owner;

    void on_patchupdate( DrawSpace::SphericalLOD::Patch* p_patch, int p_patch_lod );
    
    void build_meshe( DrawSpace::Core::Meshe& p_patchmeshe, SphericalLOD::Patch* p_patch, DrawSpace::Core::Meshe& p_outmeshe, float* p_heightmap );

public:

    Layer( Dynamics::Orbiter* p_owner, DrawSpace::SphericalLOD::Config* p_config, DrawSpace::Dynamics::World* p_world, DrawSpace::SphericalLOD::Body* p_planetbody, 
                DrawSpace::Dynamics::Collider* p_collider, Layer::SubPassCreationHandler* p_handler, int p_index );
    virtual ~Layer( void );

    void Compute( DrawSpace::Planetoid::Body* p_owner );

    void SetHotState( bool p_hotstate );
    bool GetHotState( void );
    void SetCamera( DrawSpace::Dynamics::CameraPoint* p_camera );
    void SetInertBody( DrawSpace::Dynamics::InertBody* p_body );

    DrawSpace::Dynamics::CameraPoint* GetCamera( void );
    DrawSpace::Dynamics::InertBody* GetInertBody( void );

    void RemoveColliderFromWorld( void );
    DrawSpace::SphericalLOD::Body* GetPlanetBody( void );

    void UpdateRelativeAlt( dsreal p_alt );

    void GetCollisionMesheBuildStats( long& p_nb_collisionmeshebuild_done );

    void ResetPlanetBody( void );

    void UpdateRelativeHotViewerPos( const Utils::Vector& p_pos );

    Layer::SubPassCreationHandler* GetSubPassCreationHandler( void );

    virtual void SubPassDone( DrawSpace::SphericalLOD::Collisions* p_collider );
};

}
}

#endif
