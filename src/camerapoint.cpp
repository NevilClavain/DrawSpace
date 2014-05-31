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

using namespace DrawSpace;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

CameraPoint::CameraPoint( const dsstring& p_name, Body* p_body ) : TransformNode( p_name ), m_attached_body( p_body ), m_movement( NULL )
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
}
