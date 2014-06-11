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

#ifndef _CAMERAPOINT_H_
#define _CAMERAPOINT_H_

#include "scenegraph.h"
#include "body.h"
#include "movement.h"

namespace DrawSpace
{
namespace Dynamics
{
class CameraPoint : public Core::TransformNode
{
protected:

    Body*                               m_attached_body;

    Body*                               m_locked_body;
    DrawSpace::Core::TransformNode*     m_locked_node;

    DrawSpace::Core::Movement*          m_movement;

    DrawSpace::Utils::Vector            m_locked_body_center;

public:

    CameraPoint( const dsstring& p_name, Body* p_body = NULL );
    virtual ~CameraPoint( void );

    virtual void OnRegister( Scenegraph* p_scenegraph );
   
    virtual void RegisterMovement( DrawSpace::Core::Movement* p_movement );
    virtual void ComputeFinalTransform( Utils::TimeManager& p_timemanager );

    virtual void LockOnBody( Body* p_locked_body );
    virtual void LockOnTransformNode( DrawSpace::Core::TransformNode* p_locked_node );


    virtual void GetLockedBodyCenter( DrawSpace::Utils::Vector& p_vector );
};
}
}


#endif