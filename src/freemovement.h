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

#ifndef _FREEMOVEMENT_H_
#define _FREEMOVEMENT_H_

#include "movement.h"
#include "quaternion.h"
#include "scenenodegraph.h"

namespace DrawSpace
{
namespace Core
{
class FreeMovement : public Movement
{
protected:
    Utils::Vector           m_local_speed;

	Utils::Quaternion	    m_current_res;
    Utils::Vector		    m_current_x_axis;
	Utils::Vector		    m_current_y_axis;
	Utils::Vector		    m_current_z_axis;
	
	// les sorties
	Utils::Matrix		    m_position;
	Utils::Matrix		    m_orientation;

public:
    FreeMovement( void );
    virtual ~FreeMovement( void );

    void Init( const Utils::Vector& p_init_pos );

    void RotateYaw( dsreal p_rspeed, Utils::TimeManager& p_timemanager );
	void RotatePitch( dsreal p_rspeed, Utils::TimeManager& p_timemanager );
	void RotateRoll( dsreal p_rspeed, Utils::TimeManager& p_timemanager );
    void RotateAxis( Utils::Vector& p_axis, dsreal p_rspeed, Utils::TimeManager& p_timemanager );

    void SetSpeed( dsreal p_speed );
    void Compute( Utils::TimeManager& p_timemanager );

    void GetCurrentPos( Utils::Vector& p_pos );

    void Update( DrawSpace::Utils::TimeManager& p_timemanager );
    void Update2( DrawSpace::Utils::TimeManager& p_timemanager ) {};

    void GetBaseTransform( DrawSpace::Utils::Matrix& p_mat );
    void SetFinalTransform( const DrawSpace::Utils::Matrix& p_mat ) { };
    
    void OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node ) { };

};
}
}

#endif
