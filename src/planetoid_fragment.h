/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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

#ifndef _PLANETOID_FRAGMENT_H_
#define _PLANETOID_FRAGMENT_H_

#include "spherelod_drawing.h"
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
namespace Planetoid
{

class Body;

class Fragment
{
protected:

    typedef DrawSpace::Core::BaseCallback2<void, const std::vector<DrawSpace::SphericalLOD::Patch*>&, int>  PatchsDrawRequestHandler;

    typedef DrawSpace::Core::CallBack2<Fragment, void, DrawSpace::SphericalLOD::Patch*, int>                PatchUpdateCb;

    typedef DrawSpace::Core::CallBack<Fragment, void, DrawSpace::Core::PropertyPool*>                       RunnerMsgCb;
    typedef DrawSpace::Core::CallBack<Fragment, void, Core::Runner::State>                                  RunnerEvtCb;

    DrawSpace::Dynamics::World*                                 m_world;

    DrawSpace::SphericalLOD::Body*                              m_planetbody;
    DrawSpace::Dynamics::Collider*                              m_collider;
    
    DrawSpace::Core::Runner*                                    m_runner;

    long                                                        m_nb_collisionmeshebuild_done;

    PatchUpdateCb*                                              m_patch_update_cb;
    RunnerMsgCb*                                                m_runnercb;
    RunnerEvtCb*                                                m_runnerevt;
    bool                                                        m_collision_state;

    DrawSpace::Utils::Mutex                                     m_meshe_ready_mutex;
    bool                                                        m_meshe_ready;

    dsreal                                                      m_planetray;

    bool                                                        m_hot;

    DrawSpace::Dynamics::CameraPoint*                           m_camera;
    DrawSpace::Dynamics::InertBody*                             m_inertbody;

    bool                                                        m_collisions;

    Dynamics::InertBody::Body::Parameters                       m_params;

    SphericalLOD::Patch*                                        m_current_patch;
    int                                                         m_current_patch_lod;

    std::vector<PatchsDrawRequestHandler*>                      m_patchsdrawrequest_handlers;
    int                                                         m_collidinghm_subpassindex;

    void on_meshebuild_request( DrawSpace::Core::PropertyPool* p_args );
    void on_meshebuild_result( DrawSpace::Core::Runner::State p_runnerstate );

    void on_patchupdate( DrawSpace::SphericalLOD::Patch* p_patch, int p_patch_lod );
    
    void build_meshe( DrawSpace::Core::Meshe& p_patchmeshe, SphericalLOD::Patch* p_patch, DrawSpace::Core::Meshe& p_outmeshe );

public:

    Fragment( DrawSpace::Dynamics::World* p_world, DrawSpace::SphericalLOD::Body* p_planetbody, DrawSpace::Dynamics::Collider* p_collider, dsreal p_planetray, bool p_collisions );
    virtual ~Fragment( void );

    void Update( DrawSpace::Planetoid::Body* p_owner );

    void SetHotState( bool p_hotstate );
    bool GetHotState( void );
    void SetCamera( DrawSpace::Dynamics::CameraPoint* p_camera );
    void SetInertBody( DrawSpace::Dynamics::InertBody* p_body );

    DrawSpace::Dynamics::CameraPoint* GetCamera( void );
    DrawSpace::Dynamics::InertBody* GetInertBody( void );

    void RemoveColliderFromWorld( void );
    DrawSpace::SphericalLOD::Body* GetPlanetBody( void );

    void UpdateRelativeAlt( dsreal p_alt );

    SphericalLOD::Patch* GetCurrentPatch( void );
    int GetCurrentPatchLOD( void );

    void GetCollisionMesheBuildStats( long& p_nb_collisionmeshebuild_done );

    void RegisterPatchsDrawRequestHandler( PatchsDrawRequestHandler* p_handler );
    void SetCollidingHMSubPassIndex( int p_index );
};

}
}

#endif
