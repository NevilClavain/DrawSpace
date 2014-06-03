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

#ifndef _LINEARMOVEMENT_H_
#define _LINEARMOVEMENT_H_

#include "movement.h"

namespace DrawSpace
{
namespace Core
{
class LinearMovement : public Movement
{
protected:

public:

    LinearMovement( void );
    virtual ~LinearMovement( void );

    virtual void Init( const Utils::Vector& p_init_pos, const Utils::Vector& p_direction, dsreal p_theta, dsreal p_phi );
    virtual void Compute( Utils::TimeManager& p_timemanager );

};
}
}

#endif