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

#ifndef _LONGLATMOVEMENT_H_
#define _LONGLATMOVEMENT_H_

#include "movement.h"
#include "quaternion.h"

namespace DrawSpace
{
namespace Core
{
class LongLatMovement : public Movement
{
protected:

    dsreal                      m_longitud_theta;
    dsreal                      m_latitud_phi;
    dsreal                      m_alt;

    // camera orientation control
    dsreal                      m_current_theta;
    dsreal                      m_current_phi;
   
    Utils::Quaternion		    m_qyaw;
	Utils::Quaternion		    m_qpitch;
    Utils::Quaternion		    m_rot_res;



public:

    LongLatMovement( void );
    virtual ~LongLatMovement( void );

    void Init( dsreal p_init_longitud_theta, dsreal p_init_latitud_phi, dsreal p_init_alt, dsreal p_init_theta, dsreal p_init_phi );
    virtual void Compute( Utils::TimeManager& p_timemanager );

    void SetTheta( dsreal p_theta );
    void SetPhi( dsreal p_phi );


};
}
}


#endif