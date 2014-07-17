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

#ifndef _COLLIDER_H_
#define _COLLIDER_H_

#include "body.h"

namespace DrawSpace
{
namespace Dynamics
{
class Collider : public Body
{
protected:

    btRigidBody*                    m_rigidBody;
    btCollisionShape*               m_collisionShape;
    btTriangleMesh*                 m_meshe_data;
    btDefaultMotionState*           m_motionState;


public:

    Collider( World* p_world );
    virtual ~Collider( void );

    void Update( const DrawSpace::Utils::Matrix& p_mat );

    void SetKinematic( const Body::Parameters& p_parameters );
    void UnsetKinematic( void );

    void AddToWorld( void );
    void RemoveFromWorld( void );


    virtual btRigidBody* GetRigidBody( void );

};
}
}

#endif
