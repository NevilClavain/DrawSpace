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

#ifndef _INERTBODY_H_
#define _INERTBODY_H_

#include "body.h"
#include "vector.h"
#include "matrix.h"

namespace DrawSpace
{
namespace Dynamics
{
class InertBody : public Body
{
public:

    typedef struct
    {       
        DrawSpace::Utils::Vector    initial_pos;
        DrawSpace::Utils::Matrix    inital_rot;

        dsreal                      mass;

        /*
        Body::Shape                 shape;

        DrawSpace::Utils::Vector    box_dims;
        */

        Body::ShapeDescr            shape_descr;

    } Parameters;

protected:

    Parameters                      m_parameters;

    btRigidBody*                    m_rigidBody;
    btCollisionShape*               m_collisionShape;
    btDefaultMotionState*           m_motionState;


public:

    InertBody( World* p_world, DrawSpace::Interface::Drawable* p_drawable, const Parameters& p_parameters );
    virtual ~InertBody( void );

    void GetParameters( Parameters& p_parameters );

    void Update( void );
    
};
}
}
#endif
