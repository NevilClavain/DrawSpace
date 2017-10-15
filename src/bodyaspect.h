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

#ifndef _BODYASPECT_H_
#define _BODYASPECT_H_

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include "aspect.h"
#include "bodytransformaspectimpl.h"

#include "matrix.h"
#include "meshe.h"

namespace DrawSpace
{
namespace Aspect
{
class BodyAspect : public Core::Aspect 
{
protected:

    btDefaultMotionState*                                   m_motionState;
    btCollisionShape*                                       m_collisionShape;
    btTriangleMesh*                                         m_mesh;
    btRigidBody*                                            m_rigidBody;

    AspectImplementations::BodyTransformAspectImpl          m_tr_aspectimpl;

    bool                                                    m_body_active;

public:
    typedef enum
    {
        BOX_SHAPE,
        SPHERE_SHAPE,
        MESHE_SHAPE,

    } Shape;

    void body_state( bool p_enabled );

public:

    BodyAspect( void );
    ~BodyAspect( void );

    btRigidBody* Init( void );

    AspectImplementations::BodyTransformAspectImpl* GetTransformAspectImpl( void );

    void Update( void );
};
}
}

#endif