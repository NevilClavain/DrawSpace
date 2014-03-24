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

#ifndef _BODY_H_
#define _BODY_H_

#include "drawable.h"
#include "world.h"

namespace DrawSpace
{
namespace Dynamics
{
class Body
{
public:

    typedef enum
    {
        BOX_SHAPE,
        SPHERE_SHAPE,

    } Shape;

    typedef struct
    {
        Shape shape;
        DrawSpace::Utils::Vector    box_dims;
        dsreal                      sphere_radius;

    } ShapeDescr;

    typedef struct
    {       
        DrawSpace::Utils::Vector    initial_pos;
        DrawSpace::Utils::Matrix    initial_rot;

        dsreal                      mass;

        Body::ShapeDescr            shape_descr;

    } Parameters;


protected:
    DrawSpace::Interface::Drawable* m_drawable;
    World*                          m_world;

    DrawSpace::Utils::Matrix        m_lastworldtrans;

    btCollisionShape*               instanciate_collision_shape( const ShapeDescr& p_shapedescr );



public:
    Body( World* p_world, DrawSpace::Interface::Drawable* p_drawable );
    virtual ~Body( void );

    DrawSpace::Interface::Drawable* GetDrawable( void );
    void GetLastWorldTransformation( DrawSpace::Utils::Matrix& p_transfo );
    World* GetWorld( void );

};
}
}
#endif
