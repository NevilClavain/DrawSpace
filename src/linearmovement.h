/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2022
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


#ifndef _LINEARMOVEMENT_H_
#define _LINEARMOVEMENT_H_

#include "movement.h"
#include "quaternion.h"
#include "scenenodegraph.h"

namespace DrawSpace
{
namespace Core
{
class LinearMovement// : public Movement
{
protected:

    DrawSpace::Utils::Matrix    m_result;

    dsreal                      m_speed;
    DrawSpace::Utils::Vector    m_direction;

    DrawSpace::Utils::Vector    m_init_pos;
    DrawSpace::Utils::Vector    m_current_pos;

    // camera orientation control
    dsreal                      m_current_theta;
    dsreal                      m_current_phi;
   
    Utils::Quaternion		    m_qyaw;
	Utils::Quaternion		    m_qpitch;
    Utils::Quaternion		    m_rot_res;

public:

    LinearMovement( void );
    virtual ~LinearMovement( void );

    void Init( const Utils::Vector& p_init_pos, const Utils::Vector& p_direction, dsreal p_theta, dsreal p_phi );
    virtual void Compute( Utils::TimeManager& p_timemanager );

    void SetSpeed( dsreal p_speed );
    void SetTheta( dsreal p_theta );
    void SetPhi( dsreal p_phi );

    void Reset( void );

    dsreal GetTranslationLength( void );

    void GetInitPos( Utils::Vector& p_init_pos );
    void GetCurrentPos( Utils::Vector& p_current_pos );
    void GetDirection( Utils::Vector& p_direction );
    dsreal GetCurrentTheta( void );
    dsreal GetCurrentPhi( void );
    dsreal GetCurrentSpeed( void );


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
