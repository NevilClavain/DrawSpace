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

#include "planetoid_body.h"
#include "plugin.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Planetoid;


DrawSpace::Planetoid::Body::Body( const dsstring& p_name, dsreal p_ray )
{
    m_world.Initialize();
       
    m_drawable = _DRAWSPACE_NEW_( SphericalLOD::Drawing, SphericalLOD::Drawing );
    m_drawable->SetName( p_name );
    m_drawable->SetRenderer( SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface );
    
    m_orbiter = _DRAWSPACE_NEW_( Orbiter, Orbiter( &m_world, m_drawable ) );

    m_camera_evt_cb = _DRAWSPACE_NEW_( CameraEvtCb, CameraEvtCb( this, &DrawSpace::Planetoid::Body::on_camera_event ) );
}

DrawSpace::Planetoid::Body::~Body( void )
{

}

void DrawSpace::Planetoid::Body::GetCameraHotpoint( const dsstring& p_name, Matrix& p_outmat )
{

}

void DrawSpace::Planetoid::Body::attach_body( InertBody* p_body )
{

}

void DrawSpace::Planetoid::Body::detach_body( InertBody* p_body )
{

}

void DrawSpace::Planetoid::Body::body_find_attached_camera( InertBody* p_body, std::vector<dsstring>& p_name )
{

}

void DrawSpace::Planetoid::Body::on_camera_event( Scenegraph::CameraEvent p_event, TransformNode* p_node )
{

}

SphericalLOD::Drawing* DrawSpace::Planetoid::Body::GetDrawable( void )
{
    return m_drawable;
}

Orbiter* DrawSpace::Planetoid::Body::GetOrbiter( void )
{
    return m_orbiter;
}

World* DrawSpace::Planetoid::Body::GetWorld( void )
{
    return &m_world;
}

DrawSpace::Planetoid::Body::CameraEvtCb* DrawSpace::Planetoid::Body::GetCameraEvtCb( void )
{
    return m_camera_evt_cb;
}

void DrawSpace::Planetoid::Body::ApplyGravity( void )
{
    for( std::map<InertBody*, RegisteredBody>::iterator it = m_registered_bodies.begin(); it != m_registered_bodies.end(); ++it )
    {
        if( it->second.attached )
        {
            DrawSpace::Utils::Matrix local_pos;
            it->second.body->GetLastLocalWorldTrans( local_pos );

            Vector gravity;

            gravity[0] = -local_pos( 3, 0 );
            gravity[1] = -local_pos( 3, 1 );
            gravity[2] = -local_pos( 3, 2 );
            gravity[3] = 1.0;
            gravity.Normalize();

            InertBody::Parameters params;
            it->second.body->GetParameters( params );

            gravity.Scale( params.mass * 9.81 );

            it->second.body->ApplyForce( gravity );
        }
    }
}
