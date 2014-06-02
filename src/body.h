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

#include "transformnode.h"
#include "meshe.h"
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
        MESHE_SHAPE,

    } Shape;

    typedef struct
    {
        Shape                       shape;
        DrawSpace::Utils::Vector    box_dims;
        dsreal                      sphere_radius;
        DrawSpace::Core::Meshe      meshe;

    } ShapeDescr;

    typedef struct
    {       
        DrawSpace::Utils::Vector    initial_pos;
        DrawSpace::Utils::Matrix    initial_rot;

        dsreal                      mass;

        Body::ShapeDescr            shape_descr;

    } Parameters;


protected:
    DrawSpace::Core::TransformNode*     m_drawable;
    World*                              m_world;
    bool                                m_contact_state;

    DrawSpace::Utils::Matrix            m_lastworldtrans;

    btCollisionShape*                   instanciate_collision_shape( const ShapeDescr& p_shapedescr, btTriangleMesh** p_btmeshe = NULL );



public:
    Body( World* p_world, DrawSpace::Core::TransformNode* p_drawable );
    virtual ~Body( void );

    DrawSpace::Core::TransformNode* GetDrawable( void );
    void GetLastWorldTransformation( DrawSpace::Utils::Matrix& p_transfo );
    World* GetWorld( void );

    virtual btRigidBody* GetRigidBody( void ) = 0;

    virtual bool GetContactState( void );
    virtual void SetContactState( bool p_state );

};
}
}
#endif
