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

#ifndef _ROCKET_H_
#define _ROCKET_H_

#include "inertbody.h"
#include "vector.h"
#include "matrix.h"

namespace DrawSpace
{
namespace Dynamics
{
class Rocket : public InertBody
{
protected:

    DrawSpace::Utils::Vector        m_fwd;
    DrawSpace::Utils::Vector        m_rev;
    DrawSpace::Utils::Vector        m_right;
    DrawSpace::Utils::Vector        m_left;
    DrawSpace::Utils::Vector        m_up;
    DrawSpace::Utils::Vector        m_down;

    DrawSpace::Utils::Vector        m_transformed_fwd;
    DrawSpace::Utils::Vector        m_transformed_rev;
    DrawSpace::Utils::Vector        m_transformed_left;
    DrawSpace::Utils::Vector        m_transformed_right;
    DrawSpace::Utils::Vector        m_transformed_up;
    DrawSpace::Utils::Vector        m_transformed_down;


public:

    Rocket( World* p_world, const Body::Parameters& p_parameters );
    virtual ~Rocket( void );
    
    void Update( DrawSpace::Utils::TimeManager& p_timemanager );

    void ApplyFwdForce( dsreal p_norm );
    void ApplyRevForce( dsreal p_norm );

    void ApplyDownForce( dsreal p_norm );

    void ApplyLeftYaw( dsreal p_norm );
    void ApplyRightYaw( dsreal p_norm );

    void ApplyUpPitch( dsreal p_norm );
    void ApplyDownPitch( dsreal p_norm );

    void ApplyLeftRoll( dsreal p_norm );
    void ApplyRightRoll( dsreal p_norm );

    void ZeroSpeed( void );
    void ZeroLSpeed( void );
    void ZeroASpeed( void );

    void ForceLinearSpeed( const DrawSpace::Utils::Vector& p_speed ); 
};
}
}

#endif
