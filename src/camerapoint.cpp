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


#include "camerapoint.h"
#include "maths.h"

using namespace DrawSpace;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

CameraPoint::CameraPoint( const dsstring& p_name, Body* p_body ) : TransformNode( p_name ), 
m_attached_body( p_body ), 
m_movement( NULL ),
m_locked_body( NULL ),
m_locked_node( NULL )
{
}


CameraPoint::~CameraPoint( void )
{

}


void CameraPoint::OnRegister( Scenegraph* p_scenegraph )
{
    std::map<dsstring, Core::TransformNode*>& camera_list = p_scenegraph->GetCamerasList();
    camera_list[m_scenename] = this; 
}


void CameraPoint::RegisterMovement( DrawSpace::Core::Movement* p_movement )
{
    m_movement = p_movement;
}

void CameraPoint::ComputeFinalTransform( Utils::TimeManager& p_timemanager )
{
    if( m_movement )
    {
        m_movement->Compute( p_timemanager );
        m_movement->GetResult( m_localtransformation );
    }

    if( m_attached_body )
    {
        Matrix body_trans;
        m_attached_body->GetLastWorldTransformation( body_trans );

        m_globaltransformation = m_localtransformation * body_trans;
    }
    else
    {
        m_globaltransformation = m_localtransformation;
    }

    if( m_locked_body || m_locked_node )
    {
        Matrix body_transf;

        if( m_locked_body )
        {
            m_locked_body->GetLastWorldTransformation( body_transf );
        }
        else if( m_locked_node )
        {
            m_locked_node->GetSceneWorld( body_transf );
        }

        Matrix camera_transf = m_globaltransformation;
        camera_transf.Inverse();

        Matrix res = body_transf * camera_transf;

        // point (0,0,0) local au body, exprimé dans le repere de la camera

        Vector body_center( 0.0, 0.0, 0.0, 1.0 );
        Vector body_center_2;

        res.Transform( &body_center, &body_center_2 );

        m_locked_body_center = body_center_2;


        body_center_2.Normalize();

        dsreal theta = atan2( body_center_2[0], body_center_2[2] );        
        theta = 3.1415927 + theta;
 
        Matrix roty;
        roty.Rotation( Vector( 0.0, 1.0, 0.0, 1.0 ), theta );

        Vector theta_dir( body_center_2[0], 0.0, body_center_2[2], 1.0 );

        dsreal phi = atan2( body_center_2[1], theta_dir.Length() );
        
        Matrix rotx;
        rotx.Rotation( Vector( 1.0, 0.0, 0.0, 1.0 ), phi );



        Matrix final_lock;


        final_lock = rotx * roty;

        Matrix final_res = final_lock * m_globaltransformation;
        m_globaltransformation = final_res;

    }
}

void CameraPoint::LockOnBody( Body* p_locked_body )
{
    m_locked_body = p_locked_body;
}

void CameraPoint::LockOnTransformNode( TransformNode* p_locked_node )
{
    m_locked_node = p_locked_node;
}

void CameraPoint::GetLockedBodyCenter( Vector& p_vector )
{
    p_vector = m_locked_body_center;
}
