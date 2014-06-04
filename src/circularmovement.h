/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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

#ifndef _CIRCULARMOVEMENT_H_
#define _CIRCULARMOVEMENT_H_

#include "movement.h"
#include "quaternion.h"

namespace DrawSpace
{
namespace Core
{
class CircularMovement : public Movement
{
protected:

    dsreal                      m_angular_speed;
    Utils::Vector               m_rotaxis;
    dsreal                      m_ray;
    Utils::Vector               m_center_pos;

    dsreal                      m_init_angle;
    dsreal                      m_current_angle;


    // camera orientation control
    Utils::Quaternion		    m_qyaw;
	Utils::Quaternion		    m_qpitch;
    Utils::Quaternion		    m_rot_res;

    dsreal                      m_current_theta;
    dsreal                      m_current_phi;

public:

    CircularMovement( void );
    virtual ~CircularMovement( void );

    virtual void Init( const Utils::Vector& p_center_pos, dsreal p_ray, DrawSpace::Utils::Vector p_rotaxis, dsreal p_init_angle, dsreal p_theta, dsreal p_phi );
    virtual void Compute( Utils::TimeManager& p_timemanager );

    void SetAngularSpeed( dsreal p_angular_speed );
    void SetTheta( dsreal p_theta );
    void SetPhi( dsreal p_phi );

    void Reset( void );

};
}
}


#endif