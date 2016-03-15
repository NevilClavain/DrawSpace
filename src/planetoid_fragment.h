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

#ifndef _PLANETOID_FRAGMENT_H_
#define _PLANETOID_FRAGMENT_H_

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
namespace Planetoid
{

class Body;

class Fragment // : public DrawSpace::SphericalLOD::SubPass
{
public:    
    typedef DrawSpace::Core::BaseCallback2<DrawSpace::SphericalLOD::SubPass::EntryInfos, DrawSpace::SphericalLOD::SubPass*, int>                SubPassCreationHandler;

protected:
    typedef DrawSpace::Core::CallBack2<Fragment, void, DrawSpace::SphericalLOD::Patch*, int>                                                    PatchUpdateCb;

    typedef DrawSpace::Core::CallBack<Fragment, void, DrawSpace::Core::PropertyPool*>                                                           RunnerMsgCb;
    typedef DrawSpace::Core::CallBack<Fragment, void, Core::Runner::State>                                                                      RunnerEvtCb;

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
    int                                                         m_current_patch_lod;

    bool                                                        m_draw_collidinghm;

    //DrawSpace::SphericalLOD::Collisions*                        m_collisions_hm;


    DrawSpace::SphericalLOD::Collisions*                        m_collisions_hms[6];
    DrawSpace::SphericalLOD::Collisions*                        m_current_collisions_hm;

    Fragment::SubPassCreationHandler*                           m_handler;

    void on_patchupdate( DrawSpace::SphericalLOD::Patch* p_patch, int p_patch_lod );
    
    void build_meshe( DrawSpace::Core::Meshe& p_patchmeshe, SphericalLOD::Patch* p_patch, DrawSpace::Core::Meshe& p_outmeshe, float* p_heightmap );

public:

    Fragment( DrawSpace::SphericalLOD::Config* p_config, DrawSpace::Dynamics::World* p_world, DrawSpace::SphericalLOD::Body* p_planetbody, 
                DrawSpace::Dynamics::Collider* p_collider, dsreal p_planetray, bool p_collisions, Fragment::SubPassCreationHandler* p_handler );
    virtual ~Fragment( void );

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

    SphericalLOD::Patch* GetCurrentPatch( void );
    int GetCurrentPatchLOD( void );

    void GetCollisionMesheBuildStats( long& p_nb_collisionmeshebuild_done );

    void ResetPlanetBody( void );

    Fragment::SubPassCreationHandler* GetSubPassCreationHandler( void );

    //virtual void DrawSubPass( void );


    //virtual void SubPassDone( void );

    virtual void SubPassDone( DrawSpace::SphericalLOD::Collisions* p_collider );
};

}
}

#endif
