/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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
/* -*-LIC_END-*- */

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

#include <algorithm>
#include "clouds.h"
#include "memalloc.h"
#include "transformation.h"
#include "maths.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

Clouds::Clouds( void ) : 
m_clouds_sort_request( false ),
m_current_camera( NULL ),
m_previous_camera_pos_avail( false ),
m_owner( NULL ),
m_sorting_distance( 1000.0 ),
m_details( true ),
m_nbmax_clouds_impostors( -1 ),
m_spherical_pos_ray( 0.0 )
{
    m_proceduralcb = _DRAWSPACE_NEW_( ProceduralCb, ProceduralCb( this, &Clouds::on_procedural ) );
    //m_cameracb = _DRAWSPACE_NEW_( CameraEventCb, CameraEventCb( this, &Clouds::on_camera_event ) );
}

Clouds::~Clouds( void )
{
}

/*
void Clouds::on_camera_event( SceneNodeGraph::CameraEvent p_event, BaseSceneNode* p_node )
{
    if( DrawSpace::Core::SceneNodeGraph::ACTIVE == p_event )
    {
        DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>* prec_camera = m_current_camera;        
        m_current_camera = static_cast< DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>* >( p_node );

        if( prec_camera != m_current_camera )
        {
            m_clouds_sort_request = true;
        }
    }
}
*/

void Clouds::on_procedural( Procedural::Atomic* p_atom )
{
    // decode procedural rules messages
    Procedural::Array* message = static_cast<Procedural::Array*>( p_atom );
    Procedural::String* opcode = static_cast<Procedural::String*>( message->GetValueAt( 0 ) );

    if( "declare_nbmax_impostors" == opcode->GetValue() )
    {
        Procedural::Integer* nbmax_impostors = static_cast<Procedural::Integer*>( message->GetValueAt( 1 ) );

        m_nbmax_clouds_impostors = nbmax_impostors->GetValue();
    }
    else if( "declare_cloud" == opcode->GetValue() )
    {
        Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_cloudposx = 
            static_cast<Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>*>( message->GetValueAt( 1 ) );

        Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_cloudposz = 
            static_cast<Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>*>( message->GetValueAt( 2 ) );

        Procedural::Real* posx = static_cast<Procedural::Real*>( rand_cloudposx->GetResultValue() );
        Procedural::Real* posz = static_cast<Procedural::Real*>( rand_cloudposz->GetResultValue() );


        m_new_cloud = new CloudUnitDescriptor;

        m_new_cloud->spherical = false;

        m_new_cloud->position[0] = posx->GetValue();
        m_new_cloud->position[1] = 0.0;
        m_new_cloud->position[2] = posz->GetValue();
        m_new_cloud->position[3] = 1.0;

    }
    else if( "declare_cloud_spherical_pos" == opcode->GetValue() )
    {
        Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_cloudpostheta = 
            static_cast<Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>*>( message->GetValueAt( 1 ) );

        Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_cloudposphi = 
            static_cast<Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>*>( message->GetValueAt( 2 ) );

        Procedural::Real* postheta = static_cast<Procedural::Real*>( rand_cloudpostheta->GetResultValue() );
        Procedural::Real* posphi = static_cast<Procedural::Real*>( rand_cloudposphi->GetResultValue() );

        m_new_cloud = new CloudUnitDescriptor;

        m_new_cloud->spherical = true; 

        m_new_cloud->position_spherical[0] = m_spherical_pos_ray;
        m_new_cloud->position_spherical[1] = postheta->GetValue();
        m_new_cloud->position_spherical[2] = posphi->GetValue();
        m_new_cloud->position_spherical[3] = 1.0;
    }
    else if( "add_core_impostor" == opcode->GetValue() )
    {
        ImpostorsDisplayListEntry idle;

        Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_impostor_posx = 
            static_cast<Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>*>( message->GetValueAt( 1 ) );

        Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_impostor_posz = 
            static_cast<Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>*>( message->GetValueAt( 2 ) );

        //Procedural::Vector* impostor_uv = static_cast<Procedural::Vector*>( message->GetValueAt( 3 ) );

        Procedural::Index* impostor_uvindex = static_cast<Procedural::Index*>( message->GetValueAt( 3 ) );
        Procedural::Vector* impostor_uv = static_cast<Procedural::Vector*>( impostor_uvindex->GetResultValue() );


        Procedural::Real* posx = static_cast<Procedural::Real*>( rand_impostor_posx->GetResultValue() );
        Procedural::Real* posz = static_cast<Procedural::Real*>( rand_impostor_posz->GetResultValue() );
        

        Procedural::Real* scale = static_cast<Procedural::Real*>( message->GetValueAt( 4 ) );

        idle.width_scale = scale->GetValue();
        idle.height_scale = scale->GetValue();

        idle.u1 = impostor_uv->GetValue()[0];
        idle.v1 = impostor_uv->GetValue()[1];
        idle.u2 = impostor_uv->GetValue()[2];
        idle.v2 = impostor_uv->GetValue()[1];
        idle.u3 = impostor_uv->GetValue()[2];
        idle.v3 = impostor_uv->GetValue()[3];
        idle.u4 = impostor_uv->GetValue()[0];
        idle.v4 = impostor_uv->GetValue()[3];


        if( !m_new_cloud->spherical )
        {
            idle.localpos[0] = m_new_cloud->position[0] + posx->GetValue();
            idle.localpos[1] = m_new_cloud->position[1];
            idle.localpos[2] = m_new_cloud->position[2] + posz->GetValue();
        }
        else
        {
            idle.localpos[0] = posx->GetValue();
            idle.localpos[1] = 0.0;
            idle.localpos[2] = posz->GetValue();

            idle.spherical_longitud = m_new_cloud->position_spherical[1];
            idle.spherical_latitud = m_new_cloud->position_spherical[2];
            idle.spherical_ray = m_new_cloud->position_spherical[0];
        }

        m_new_cloud->impostors.push_back( idle );
    }
    else if( "add_bottom_impostor" == opcode->GetValue() )
    {
        ImpostorsDisplayListEntry idle;

        Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_impostor_posx = 
            static_cast<Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>*>( message->GetValueAt( 1 ) );

        Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_impostor_posz = 
            static_cast<Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>*>( message->GetValueAt( 3 ) );

        Procedural::Vector* impostor_uv = static_cast<Procedural::Vector*>( message->GetValueAt( 4 ) );

        Procedural::Real* posx = static_cast<Procedural::Real*>( rand_impostor_posx->GetResultValue() );
        Procedural::Real* posz = static_cast<Procedural::Real*>( rand_impostor_posz->GetResultValue() );
        Procedural::Real* posy = static_cast<Procedural::Real*>( message->GetValueAt( 2 ) );

        Procedural::Real* scale = static_cast<Procedural::Real*>( message->GetValueAt( 5 ) );

        idle.width_scale = scale->GetValue();
        idle.height_scale = scale->GetValue();

        idle.u1 = impostor_uv->GetValue()[0];
        idle.v1 = impostor_uv->GetValue()[1];
        idle.u2 = impostor_uv->GetValue()[2];
        idle.v2 = impostor_uv->GetValue()[1];
        idle.u3 = impostor_uv->GetValue()[2];
        idle.v3 = impostor_uv->GetValue()[3];
        idle.u4 = impostor_uv->GetValue()[0];
        idle.v4 = impostor_uv->GetValue()[3];


        if( !m_new_cloud->spherical )
        {
            idle.localpos[0] = m_new_cloud->position[0] + posx->GetValue();
            idle.localpos[1] = m_new_cloud->position[1] + posy->GetValue();
            idle.localpos[2] = m_new_cloud->position[2] + posz->GetValue();
        }
        else
        {
            idle.localpos[0] = posx->GetValue();
            idle.localpos[1] = posy->GetValue();
            idle.localpos[2] = posz->GetValue();

            idle.spherical_longitud = m_new_cloud->position_spherical[1];
            idle.spherical_latitud = m_new_cloud->position_spherical[2];
            idle.spherical_ray = m_new_cloud->position_spherical[0];
        }
    
        m_new_cloud->impostors.push_back( idle );
    
    }
    else if( "add_impostor" == opcode->GetValue() && m_details )
    {
        ImpostorsDisplayListEntry idle;

        
        Procedural::Index* impostor_uvindex = static_cast<Procedural::Index*>( message->GetValueAt( 1 ) );
        Procedural::Vector* impostor_uv = static_cast<Procedural::Vector*>( impostor_uvindex->GetResultValue() );

        Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_impostorscale = 
            static_cast<Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>*>( message->GetValueAt( 2 ) );

        Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_impostor_posx = 
            static_cast<Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>*>( message->GetValueAt( 3 ) );

        Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_impostor_posy = 
            static_cast<Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>*>( message->GetValueAt( 4 ) );

        Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_impostor_posz = 
            static_cast<Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>*>( message->GetValueAt( 5 ) );



        Procedural::Real* impostor_scale = static_cast<Procedural::Real*>( rand_impostorscale->GetResultValue() );

        idle.width_scale = impostor_scale->GetValue();
        idle.height_scale = impostor_scale->GetValue();

        Procedural::Real* posx = static_cast<Procedural::Real*>( rand_impostor_posx->GetResultValue() );
        Procedural::Real* posy = static_cast<Procedural::Real*>( rand_impostor_posy->GetResultValue() );
        Procedural::Real* posz = static_cast<Procedural::Real*>( rand_impostor_posz->GetResultValue() );

        
        idle.u1 = impostor_uv->GetValue()[0];
        idle.v1 = impostor_uv->GetValue()[1];
        idle.u2 = impostor_uv->GetValue()[2];
        idle.v2 = impostor_uv->GetValue()[1];
        idle.u3 = impostor_uv->GetValue()[2];
        idle.v3 = impostor_uv->GetValue()[3];
        idle.u4 = impostor_uv->GetValue()[0];
        idle.v4 = impostor_uv->GetValue()[3];
        

        if( !m_new_cloud->spherical )
        {
            idle.localpos[0] = m_new_cloud->position[0] + posx->GetValue();
            idle.localpos[1] = m_new_cloud->position[1] + posy->GetValue();
            idle.localpos[2] = m_new_cloud->position[2] + posz->GetValue();
        }
        else
        {
            idle.localpos[0] = posx->GetValue();
            idle.localpos[1] = posy->GetValue();
            idle.localpos[2] = posz->GetValue();

            idle.spherical_longitud = m_new_cloud->position_spherical[1];
            idle.spherical_latitud = m_new_cloud->position_spherical[2];
            idle.spherical_ray = m_new_cloud->position_spherical[0];
        }

        m_new_cloud->impostors.push_back( idle );    
    }
    else if( "push_cloud" == opcode->GetValue() )
    {
        m_clouds.push_back( m_new_cloud );
    }
}

void Clouds::execsortz( const DrawSpace::Utils::Matrix& p_impostor_mat, const DrawSpace::Utils::Matrix& p_cam_mat )
{
    // compute all camera-space z-depth

    for( size_t i = 0; i < m_clouds.size(); i++ )
    {        
        Matrix finalm;

        if( m_clouds[i]->spherical )
        {
            Matrix pos;
            Matrix roty, rotx;

            dsreal ray = m_clouds[i]->position_spherical[0];
            dsreal theta = m_clouds[i]->position_spherical[1];
            dsreal phi = m_clouds[i]->position_spherical[2];

            pos.Translation( 0.0, ray, 0.0 );

            
            //roty.Rotation( Vector( 0.0, 1.0, 0.0, 1.0 ), Maths::DegToRad( theta ) );
            //rotx.Rotation( Vector( 1.0, 0.0, 0.0, 1.0 ), Maths::DegToRad( phi ) );
            
            
            roty.Identity();
            rotx.Identity();

            roty(0, 0) = cos(Maths::DegToRad( theta ));
            roty(2, 2) = cos(Maths::DegToRad( theta ));
            roty(2, 0) = -sin(Maths::DegToRad( theta ));
            roty(0, 2) = sin(Maths::DegToRad( theta ));

            rotx(1, 1) = cos(Maths::DegToRad( phi ));
            rotx(2, 2) = cos(Maths::DegToRad( phi ));
            rotx(2, 1) = sin(Maths::DegToRad( phi ));
            rotx(1, 2) = -sin(Maths::DegToRad( phi ));
            
            finalm = pos * rotx * roty * p_impostor_mat;
        }
        else
        {
            Matrix local_trans;
            local_trans.Translation( m_clouds[i]->position );
            finalm = local_trans * p_impostor_mat;
        }

        Matrix cam_mat = p_cam_mat;

        Vector point_imp( 0.0, 0.0, 0.0, 1.0 );

        
        Vector point_cam( 0.0, 0.0, 0.0, 1.0 );

        Vector t_point_imp, t_point_cam;

        finalm.Transform( &point_imp, &t_point_imp );
        cam_mat.Transform( &point_cam, &t_point_cam );

        // la distance entre l'impostor et le point camera;
        Vector dist_imp;
        
        dist_imp[0] = t_point_imp[0] - t_point_cam[0];
        dist_imp[1] = t_point_imp[1] - t_point_cam[1];
        dist_imp[2] = t_point_imp[2] - t_point_cam[2];
        dist_imp[3] = 1.0;

        m_clouds[i]->distToView = dist_imp.Length();
        
    }

    std::sort( m_clouds.begin(), m_clouds.end(), Clouds::nodes_comp );
}


bool Clouds::nodes_comp( CloudUnitDescriptor* p_n1, CloudUnitDescriptor* p_n2 )
{
    return ( p_n1->distToView > p_n2->distToView );
}

void Clouds::impostors_init( void )
{
    m_idl.clear();

    for( size_t i = 0; i < m_clouds.size(); i++ )
    {
        for( size_t j = 0; j < m_clouds[i]->impostors.size(); j++ )
        {     
            m_idl.push_back( m_clouds[i]->impostors[j] );
        }
    }    
}

void Clouds::ImpostorsInit( void )
{
    m_clouds_sort_request = true;
    impostors_init();
    Chunk::ImpostorsInit( m_nbmax_clouds_impostors );
}

Clouds::ProceduralCb* Clouds::GetProceduralCallback( void )
{
    return m_proceduralcb;
}

void Clouds::OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node )
{
    Chunk::OnRegister( p_scenegraph, p_node );
    //m_scenenodegraph->RegisterCameraEvtHandler( m_cameracb );
    m_owner = p_node;
}

void Clouds::OnUnregister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node )
{
    Chunk::OnUnregister( p_scenegraph, p_node );
}

void Clouds::Update2( DrawSpace::Utils::TimeManager& p_timemanager )
{
    if( !m_owner )
    {
        return;
    }
   
    Chunk::Update2( p_timemanager );

    if( m_current_camera )
    {
        Matrix CamMat;
        
        m_current_camera->GetFinalTransform( CamMat );
       
        Vector current_camera_pos;

        current_camera_pos[0] = CamMat( 3, 0 );
        current_camera_pos[1] = CamMat( 3, 1 );
        current_camera_pos[2] = CamMat( 3, 2 );
        current_camera_pos[3] = 1.0;

        if( m_previous_camera_pos_avail )
        {
            Vector delta_cam_pos;

            delta_cam_pos[0] = current_camera_pos[0] - m_previous_camera_pos[0];
            delta_cam_pos[0] = current_camera_pos[1] - m_previous_camera_pos[1];
            delta_cam_pos[0] = current_camera_pos[2] - m_previous_camera_pos[2];
            delta_cam_pos[3] = 1.0;

            if( delta_cam_pos.Length() > m_sorting_distance )
            {
                m_clouds_sort_request = true;
                m_previous_camera_pos = current_camera_pos;
            }
        }
        else
        {
            m_previous_camera_pos = current_camera_pos;
            m_previous_camera_pos_avail = true;
        }
    }

    if( m_clouds_sort_request && m_current_camera )
    {
        
        // get clouds node global transform
        Matrix ImpostorMat;
        m_owner->GetFinalTransform( ImpostorMat );

        Matrix CamMat;
        m_current_camera->GetFinalTransform( CamMat );
        
        execsortz( ImpostorMat, CamMat );
        impostors_init();
        ImpostorsUpdate();
        
        m_clouds_sort_request = false;
    }    
}

void Clouds::EnableDetails( bool p_details )
{
    m_details = p_details;
}

void Clouds::SetSortingDistance( dsreal p_distance )
{
    m_sorting_distance = p_distance;
}

dsreal Clouds::GetSortingDistance( void )
{
    return m_sorting_distance;
}

void Clouds::CloudsReset( void )
{
    for( size_t i = 0; i < m_clouds.size(); i++ )
    {
        delete m_clouds[i];
    }

    m_clouds.clear();
}

void Clouds::CloudsUpdateRequest( void )
{
    if( m_current_camera )
    {
        Matrix ImpostorMat;
        m_owner->GetFinalTransform( ImpostorMat );

        Matrix CamMat;
        m_current_camera->GetFinalTransform( CamMat );

        execsortz( ImpostorMat, CamMat );
        impostors_init();
        ImpostorsUpdate();
    }
}

void Clouds::SetSphericalPosRay( dsreal p_ray )
{
    m_spherical_pos_ray = p_ray;
}
