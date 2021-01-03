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

#ifndef _HEADMOVEMENT_H_
#define _HEADMOVEMENT_H_

#include "movement.h"
#include "inertbody.h"
#include "timemanager.h"
#include "scenenodegraph.h"

namespace DrawSpace
{
namespace Core
{
class HeadMovement// : public Movement
{
protected:

    DrawSpace::Utils::Matrix        m_result;

    dsreal                          m_scalefactor;
    DrawSpace::Dynamics::InertBody* m_refbody;
    dsreal                          m_ref_force;

    dsreal                          m_body_mass;

    DrawSpace::Utils::Vector        m_head_pos;

    DrawSpace::Utils::Vector        m_player_view_pos;
    dsreal                          m_player_view_linear_acc[3];
    dsreal                          m_player_view_linear_acc_2[3];
    dsreal                          m_player_view_linear_speed[3];

    dsreal                          m_player_view_linear_speed_clamp_up[3];
    dsreal                          m_player_view_linear_speed_clamp_down[3];


    dsreal                          m_player_view_theta;
    dsreal                          m_player_view_phi;
    dsreal                          m_player_view_rho;

    dsreal                          m_player_view_angular_acc[3];
    dsreal                          m_player_view_angular_acc_2[3];

    dsreal                          m_player_view_angular_speed_clamp_up[3];
    dsreal                          m_player_view_angular_speed_clamp_down[3];

    dsreal                          m_player_view_angular_speed[3];

    DrawSpace::Utils::Matrix        m_rotx;
    DrawSpace::Utils::Matrix        m_roty;
    DrawSpace::Utils::Matrix        m_rotz;
    DrawSpace::Utils::Matrix        m_cam_delta_pos;
    DrawSpace::Utils::Matrix        m_cam_base_pos;

public:

    HeadMovement( void );
    virtual ~HeadMovement( void );

    void Init( dsreal p_scalefactor, dsreal p_ref_force, const DrawSpace::Utils::Vector& p_head_pos );

    void SetRefBody( DrawSpace::Dynamics::InertBody* p_refbody );
    void Compute( DrawSpace::Utils::TimeManager& p_timemanager );

    dsreal GetScaleFactor( void );
    dsreal GetRefForce( void );
    void GetHeadPos( Utils::Vector& p_headpos );

    void Update( DrawSpace::Utils::TimeManager& p_timemanager );
    void Update2( DrawSpace::Utils::TimeManager& p_timemanager ) {};

    void GetBaseTransform( DrawSpace::Utils::Matrix& p_mat );
    void SetFinalTransform( const DrawSpace::Utils::Matrix& p_mat ) { };
    
    void OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node ) { };
    void OnUnregister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node ) { };

};

}
}

#endif
