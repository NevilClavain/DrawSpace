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
#include "orbiter.h"
#include "longlatmovement.h"


namespace DrawSpace
{
namespace Dynamics
{
class CameraPoint : public Core::TransformNode
{
public:

    typedef struct
    {
        DrawSpace::Dynamics::Orbiter*   relative_orbiter;      
        dsreal                          altitud;         // only if relative_planet != NULL

        bool                            attached_to_body;
        dsstring                        attached_body_classname; // only if attached_to_body == true

        bool                            locked_on_body;
        bool                            locked_on_transformnode;

        bool                            has_movement;
        dsstring                        movement_classname; // only if has_movement == true

        bool                            has_longlatmovement;

    } Infos;

protected:

    Body*                               m_attached_body;

    Body*                               m_locked_body;
    DrawSpace::Core::TransformNode*     m_locked_node;

    DrawSpace::Core::Movement*          m_movement;
    DrawSpace::Core::LongLatMovement*   m_longlatmovement;

    DrawSpace::Utils::Vector            m_locked_body_center;

    DrawSpace::Utils::Matrix            m_projection;

    ////
    DrawSpace::Dynamics::Orbiter*       m_relative_orbiter;
    dsreal                              m_relative_altitud;         // only if relative_planet != NULL


public:

    CameraPoint( const dsstring& p_name, Body* p_body = NULL );
    virtual ~CameraPoint( void );

    virtual void OnRegister( Scenegraph* p_scenegraph );
   
    virtual void RegisterMovement( DrawSpace::Core::Movement* p_movement );
    virtual void RegisterLongLatMovement( DrawSpace::Core::LongLatMovement* p_longlatmovement );
    virtual void ComputeFinalTransform( Utils::TimeManager& p_timemanager );

    virtual void LockOnBody( Body* p_locked_body );
    virtual void LockOnTransformNode( DrawSpace::Core::TransformNode* p_locked_node );


    virtual void GetLockedBodyCenter( DrawSpace::Utils::Vector& p_vector );
    virtual void GetLocalTransform( DrawSpace::Utils::Matrix& p_localtransf );

    virtual Body* GetAttachedBody( void );

    virtual void GetInfos( Infos& p_infos );

    virtual void SetRelativeOrbiter( DrawSpace::Dynamics::Orbiter* p_relative_orbiter );
    virtual void SetRelativeAltitude( dsreal p_relative_altitud );

    virtual void GetProjection( DrawSpace::Utils::Matrix& p_mat );
};
}
}


#endif