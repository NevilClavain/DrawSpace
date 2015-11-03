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

    typedef DrawSpace::Core::CallBack<Fragment, void, DrawSpace::SphericalLOD::Patch*>          PatchUpdateCb;

    typedef DrawSpace::Core::CallBack<Fragment, void, DrawSpace::Core::PropertyPool*>           RunnerMsgCb;
    typedef DrawSpace::Core::CallBack<Fragment, void, Core::Runner::State>                      RunnerEvtCb;


    DrawSpace::SphericalLOD::Body*                              m_planetbody;
    DrawSpace::Dynamics::Collider*                              m_collider;
    
    //DrawSpace::Core::Mediator::MessageQueue*                    m_buildmeshereq_msg;
    DrawSpace::Core::Runner*                                    m_runner;
    //DrawSpace::Core::Task<DrawSpace::Core::Runner>*             m_task;

    long                                                        m_nb_collisionmeshebuild_req;
    long                                                        m_nb_collisionmeshebuild_done;
    long                                                        m_nb_collisionmeshebuild_added;


    //SphereLODEvtCb*                                             m_spherelod_evt_cb;

    PatchUpdateCb*                                              m_patch_update_cb;
    RunnerMsgCb*                                                m_runnercb;
    RunnerEvtCb*                                                m_runnerevt;

    bool                                                        m_suspend_update;
    bool                                                        m_collision_state;

    DrawSpace::Utils::Mutex                                     m_meshe_ready_mutex;
    bool                                                        m_meshe_ready;

    dsreal                                                      m_planetray;

    bool                                                        m_hot;

    DrawSpace::Dynamics::CameraPoint*                           m_camera;
    DrawSpace::Dynamics::InertBody*                             m_inertbody;

    bool                                                        m_collisions;

    dsstring                                                    m_name;

    Dynamics::InertBody::Body::Parameters                       m_params;

    SphericalLOD::Patch*                                        m_current_patch;

    void on_meshebuild_request( DrawSpace::Core::PropertyPool* p_args );
    void on_meshebuild_result( DrawSpace::Core::Runner::State p_runnerstate );

    void on_spherelod_event( DrawSpace::SphericalLOD::Body* p_body, int p_currentface );

    void on_patchupdate( DrawSpace::SphericalLOD::Patch* p_patch );
    
    void build_meshe( DrawSpace::Core::Meshe& p_patchmeshe, SphericalLOD::Patch* p_patch, DrawSpace::Core::Meshe& p_outmeshe );

public:

    Fragment( const dsstring& p_name, DrawSpace::SphericalLOD::Body* p_planetbody, DrawSpace::Dynamics::Collider* p_collider, dsreal p_planetray, bool p_collisions );
    virtual ~Fragment( void );

    void Update( DrawSpace::Dynamics::World* p_world, DrawSpace::Planetoid::Body* p_owner );

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

    void GetCollisionMesheBuildStats( long& p_nb_collisionmeshebuild_req, long& p_nb_collisionmeshebuild_done, long& p_nb_collisionmeshebuild_added );
};

}
}

#endif
