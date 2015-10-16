/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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
#include "exceptions.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Planetoid;


DrawSpace::Planetoid::Body::Body( const dsstring& p_scenename, dsreal p_ray ) : Orbiter( &m_world ),
m_scenename( p_scenename ),
m_ray( p_ray * 1000.0 )
{
    m_world.Initialize();

    m_fractal = new Fractal( 3, 17029, 0.65, 1.29 );
       
    m_drawable = _DRAWSPACE_NEW_( SphericalLOD::Drawing, SphericalLOD::Drawing );
    m_drawable->SetRenderer( SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface );
    
    m_camera_evt_cb = _DRAWSPACE_NEW_( CameraEvtCb, CameraEvtCb( this, &DrawSpace::Planetoid::Body::on_camera_event ) );
    m_nodes_evt_cb = _DRAWSPACE_NEW_( NodesEventCb, NodesEventCb( this, &DrawSpace::Planetoid::Body::on_nodes_event ) );
    m_scenegraph_evt_cb = _DRAWSPACE_NEW_( ScenegraphEventCb, ScenegraphEventCb( this, &DrawSpace::Planetoid::Body::on_scenegraph_event ) );
}

DrawSpace::Planetoid::Body::~Body( void )
{

}

void DrawSpace::Planetoid::Body::GetCameraHotpoint( const dsstring& p_name, Matrix& p_outmat )
{
    if( INERTBODY_LINKED == m_registered_camerapoints[p_name].type )
    {
        m_registered_camerapoints[p_name].attached_body->GetLastLocalWorldTrans( p_outmat );
    }
    else if( FREE_ON_PLANET == m_registered_camerapoints[p_name].type )
    {
        SceneNode<CameraPoint>* camera_node = m_registered_camerapoints[p_name].camera->GetOwner();

        Matrix res;
        res.Identity();
        Orbiter* orbiter = static_cast<Orbiter*>( m_registered_camerapoints[p_name].camera->GetReferentBody() );
        camera_node->GetTransformationRelativeTo( orbiter->GetOwner(), res );
        p_outmat = res;
    }
}

void DrawSpace::Planetoid::Body::attach_body( InertBody* p_body )
{
    p_body->Attach( this );
    m_registered_bodies[p_body].attached = true;
}

void DrawSpace::Planetoid::Body::detach_body( InertBody* p_body )
{
    p_body->Detach();
    m_registered_bodies[p_body].attached = false;
}

void DrawSpace::Planetoid::Body::body_find_attached_camera( InertBody* p_body, std::vector<dsstring>& p_name )
{
    DrawSpace::Dynamics::Body* body = p_body;

    for( std::map<dsstring, RegisteredCamera>::iterator it = m_registered_camerapoints.begin(); it != m_registered_camerapoints.end(); ++it )
    {
        if( it->second.camera->GetReferentBody() == body )
        {
            p_name.push_back( it->first );
        }
    }
}

void DrawSpace::Planetoid::Body::on_camera_event( DrawSpace::Core::SceneNodeGraph::CameraEvent p_event, DrawSpace::Core::BaseSceneNode* p_node )
{
    if( DrawSpace::Core::SceneNodeGraph::ACTIVE == p_event )
    {
        if( !p_node )
        {
            return;
        }

        dsstring current_camera_scenename;
        p_node->GetSceneName( current_camera_scenename );

        if( m_registered_camerapoints.count( current_camera_scenename ) > 0 )
        {
            m_current_camerapoint = current_camera_scenename;

            Fragment* fragment = m_registered_camerapoints[current_camera_scenename].fragment;
            m_drawable->SetCurrentPlanetBody( fragment->GetPlanetBody() );
        }
        else
        {
            // camera non enregistree
            m_current_camerapoint = "";
        }
    }
}

void DrawSpace::Planetoid::Body::on_nodes_event( DrawSpace::Core::SceneNodeGraph::NodesEvent p_event, DrawSpace::Core::BaseSceneNode* p_node )
{
    if( SceneNodeGraph::NODE_ADDED == p_event )
    {
        SceneNode<InertBody>* inertbody_node = dynamic_cast<SceneNode<InertBody>*>( p_node );
        SceneNode<Rocket>* rocket_node = dynamic_cast<SceneNode<Rocket>*>( p_node );

        InertBody* inertbody = NULL;

        if( inertbody_node )
        {
            inertbody = inertbody_node->GetContent();
        }
        else if( rocket_node )
        {
            inertbody = rocket_node->GetContent();
        }

        if( inertbody )
        {            
            if( inertbody->IsDynamicLinkEnabled() )
            {
                RegisteredBody reg_body;

                if( inertbody->IsDynamicLinkInitState() )
                {
                    if( inertbody->GetReferentBody() == this )
                    {
                        reg_body.attached = true;
                        reg_body.body = inertbody;

                        
                        dsstring bodyname;
                        p_node->GetSceneName( bodyname );
                       

                        inertbody->IncludeTo( this );

                        DrawSpace::SphericalLOD::Body* slod_body = _DRAWSPACE_NEW_( DrawSpace::SphericalLOD::Body, DrawSpace::SphericalLOD::Body( m_ray * 2.0 ) );
                        Collider* collider = _DRAWSPACE_NEW_( Collider, Collider );


                        dsstring final_name = m_scenename + dsstring( " " ) + bodyname;
                        Fragment* planet_fragment = _DRAWSPACE_NEW_( Fragment, Fragment( final_name, slod_body, collider, m_ray, true ) );
                        planet_fragment->SetHotState( true );

                        m_planetfragments_list.push_back( planet_fragment );
                        reg_body.fragment = planet_fragment;

                        planet_fragment->SetInertBody( inertbody );

                        slod_body->Initialize();
                        
                        m_registered_bodies[inertbody] = reg_body;
                    }

                }
                else
                {                    
                    reg_body.attached = false;
                    reg_body.body = inertbody;

                    DrawSpace::SphericalLOD::Body* slod_body = _DRAWSPACE_NEW_( DrawSpace::SphericalLOD::Body, DrawSpace::SphericalLOD::Body( m_ray * 2.0 ) );
                    Collider* collider = _DRAWSPACE_NEW_( Collider, Collider );

                    dsstring bodyname;                   
                    p_node->GetSceneName( bodyname );
                   
                    dsstring final_name = m_scenename + dsstring( " " ) + bodyname;

                    Fragment* planet_fragment = _DRAWSPACE_NEW_( Fragment, Fragment( final_name, slod_body, collider, m_ray, true ) );
                    planet_fragment->SetHotState( false );

                    m_planetfragments_list.push_back( planet_fragment );
                    reg_body.fragment = planet_fragment;

                    planet_fragment->SetInertBody( inertbody );

                    slod_body->Initialize();

                    m_registered_bodies[inertbody] = reg_body;
                }
            }
            return;
        }

        SceneNode<Collider>* collider_node = dynamic_cast<SceneNode<Collider>*>( p_node );
        if( collider_node )
        {
            Orbiter* collider_ref_orbiter = collider_node->GetContent()->GetReferentOrbiter();
            if( collider_ref_orbiter == this )
            {
                collider_node->GetContent()->AddToWorld( &m_world );
            }
            return;
        }
        
        SceneNode<CameraPoint>* camera_node = dynamic_cast<SceneNode<CameraPoint>*>( p_node );
        if( camera_node )
        {
            RegisteredCamera reg_camera;
            dsstring camera_scenename;

            reg_camera.camera = camera_node->GetContent();

            p_node->GetSceneName( camera_scenename );

            DrawSpace::Dynamics::Body* camrefbody = camera_node->GetContent()->GetReferentBody();
            if( camrefbody )
            {
                InertBody* inert_body = dynamic_cast<InertBody*>( camrefbody );
                if( inert_body )
                {
                    if( m_registered_bodies.count( inert_body ) > 0 )
                    {
                        reg_camera.type = INERTBODY_LINKED;
                        reg_camera.attached_body = inert_body;
                        reg_camera.attached_collider = NULL;
                        reg_camera.fragment = m_registered_bodies[inert_body].fragment;

                        m_registered_camerapoints[camera_scenename] = reg_camera;
                    }
                    else
                    {
                        // camera liee a un inertbody ignore par la planete (dynamic link pas autorise) : ne rien faire
                        return;
                    }
                }
                else
                {
                    Orbiter* orbiter = dynamic_cast<Orbiter*>( camrefbody );
                    if( orbiter )
                    {
                        if( orbiter == this )
                        {
                            // la camera est attachee a notre planete !

                            reg_camera.type = FREE_ON_PLANET;
                            reg_camera.attached_body = NULL;
                            reg_camera.attached_collider = NULL;

                            create_camera_collisions( camera_scenename, camera_node->GetContent(), reg_camera );

                            m_registered_camerapoints[camera_scenename] = reg_camera;

                        }
                        else
                        {
                            // camera peut etre liee a une autre planete ou a un autre orbiter; ne rien faire ici...
                        }
                    }
                    else
                    {
                        // le body referent de la camera ne peut pas être autre chose qu'un inertbody ou un orbiter
                        _DSEXCEPTION( "camera referent body bad type" );
                    }
                }
            }
            else
            {
                reg_camera.type = FREE;
                reg_camera.attached_body = NULL;
                create_camera_collisions( camera_scenename, camera_node->GetContent(), reg_camera );

                m_registered_camerapoints[camera_scenename] = reg_camera;
            }            
        }
    }
}

void DrawSpace::Planetoid::Body::on_scenegraph_event( SceneNodeGraph::ScenegraphEvent p_event, SceneNodeGraph* p_scenegraph )
{
    if( DrawSpace::Core::SceneNodeGraph::TRANSFORMATIONS_BEGIN == p_event )
    {
        apply_gravity();
    }
    else if( DrawSpace::Core::SceneNodeGraph::TRANSFORMATIONS_DONE == p_event )
    {
        update_fragments();
        manage_bodies();
    }
}

void DrawSpace::Planetoid::Body::apply_gravity( void )
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

void DrawSpace::Planetoid::Body::manage_bodies( void )
{
    for( std::map<DrawSpace::Dynamics::InertBody*, RegisteredBody>::iterator it = m_registered_bodies.begin(); it != m_registered_bodies.end(); ++it )
    {
        if( it->second.attached )
        {
            DrawSpace::Utils::Matrix bodypos;

            it->second.body->GetLastLocalWorldTrans( bodypos );

            DrawSpace::Utils::Vector bodypos2;
            bodypos2[0] = bodypos( 3, 0 );
            bodypos2[1] = bodypos( 3, 1 );
            bodypos2[2] = bodypos( 3, 2 );

            dsreal rel_alt = ( bodypos2.Length() / m_ray );

            if( rel_alt >= 1.2 )
            {

                detach_body( it->second.body );

                // rechercher si une camera enregistree est associee a ce body
                std::vector<dsstring> cameras;
                body_find_attached_camera( it->second.body, cameras );

                for( size_t i = 0; i < cameras.size(); i++ )
                {
                    m_registered_camerapoints[cameras[i]].camera->SetRelativeOrbiter( NULL );

                    Fragment* fragment = m_registered_camerapoints[cameras[i]].fragment;
                    fragment->SetHotState( false );

                }

                //////

                Fragment* fragment = it->second.fragment;
                fragment->RemoveColliderFromWorld();

                //////
            }
        }
        else
        {
            DrawSpace::Utils::Matrix bodypos;

            it->second.body->GetLastLocalWorldTrans( bodypos );

            DrawSpace::Utils::Vector bodypos2;
            bodypos2[0] = bodypos( 3, 0 );
            bodypos2[1] = bodypos( 3, 1 );
            bodypos2[2] = bodypos( 3, 2 );

            DrawSpace::Utils::Matrix planetbodypos;
            //m_orbiter->GetLastWorldTransformation( planetbodypos );
            GetLastWorldTransformation( planetbodypos );

            DrawSpace::Utils::Vector planetbodypos2;
            planetbodypos2[0] = planetbodypos( 3, 0 );
            planetbodypos2[1] = planetbodypos( 3, 1 );
            planetbodypos2[2] = planetbodypos( 3, 2 );

            Vector delta;

            delta[0] = planetbodypos2[0] - bodypos2[0];
            delta[1] = planetbodypos2[1] - bodypos2[1];
            delta[2] = planetbodypos2[2] - bodypos2[2];
            delta[3] = 1.0;

            dsreal rel_alt = delta.Length() / m_ray;

            if( rel_alt < 1.1 )
            {

                attach_body( it->second.body );

                /////

                std::vector<dsstring> cameras;
                body_find_attached_camera( it->second.body, cameras );

                for( size_t i = 0; i < cameras.size(); i++ )
                {
                    //m_registered_camerapoints[cameras[i]].camera->SetRelativeOrbiter( m_orbiter );
                    m_registered_camerapoints[cameras[i]].camera->SetRelativeOrbiter( this );

                    Fragment* fragment = m_registered_camerapoints[cameras[i]].fragment;

                    fragment->SetHotState( true );
                }
            }
        }
    }
}

void DrawSpace::Planetoid::Body::update_fragments( void )
{
    for( size_t i = 0; i < m_planetfragments_list.size(); i++ )
    {
        Fragment* curr = m_planetfragments_list[i];

        curr->Update( &m_world, this );

        InertBody* inertbody = curr->GetInertBody();
        CameraPoint* camera = curr->GetCamera();

        if( inertbody )
        {
            //if( inertbody->GetRefBody() == m_orbiter )
            if( inertbody->GetAttachedBody() == this )
            {
                std::vector<dsstring> cameras;
                body_find_attached_camera( inertbody, cameras );

                for( size_t i = 0; i < cameras.size(); i++ )
                {
                    m_registered_camerapoints[cameras[i]].camera->SetRelativeAltitude( curr->GetPlanetBody()->GetAltitud() );
                }
            }
        }
        else if( camera )
        {
            CameraPoint::Infos cam_infos;
            camera->GetInfos( cam_infos );
            camera->SetRelativeAltitude( curr->GetPlanetBody()->GetAltitud() );
        }       
    }
}

void DrawSpace::Planetoid::Body::create_camera_collisions( const dsstring& p_cameraname, CameraPoint* p_camera, DrawSpace::Planetoid::Body::RegisteredCamera& p_cameradescr )
{
    DrawSpace::SphericalLOD::Body* slod_body = _DRAWSPACE_NEW_( DrawSpace::SphericalLOD::Body, DrawSpace::SphericalLOD::Body( m_ray * 2.0 ) );
    Collider* collider = _DRAWSPACE_NEW_( Collider, Collider );

    dsstring final_name = m_scenename + dsstring( " " ) + p_cameraname;
    Fragment* planet_fragment = _DRAWSPACE_NEW_( Fragment, Fragment( final_name, slod_body, collider, m_ray, false ) );

    slod_body->Initialize();

    planet_fragment->SetHotState( true );
    planet_fragment->SetCamera( p_camera );

    p_cameradescr.fragment = planet_fragment;

    //p_cameradescr.camera->SetRelativeOrbiter( m_orbiter );
    p_cameradescr.camera->SetRelativeOrbiter( this );

    //m_sphericallod_body_list[p_cameraname] = slod_body;
    m_planetfragments_list.push_back( planet_fragment );    
}

void DrawSpace::Planetoid::Body::GetSceneName( dsstring& p_name )
{
    p_name = m_scenename;
}

void DrawSpace::Planetoid::Body::RegisterPassSlot( Pass* p_pass )
{
    m_drawable->RegisterPassSlot( p_pass );
}

void DrawSpace::Planetoid::Body::SetFinalTransform( const DrawSpace::Utils::Matrix& p_mat )
{
    Orbiter::SetFinalTransform( p_mat );
    m_drawable->SetFinalTransform( p_mat );
}

void DrawSpace::Planetoid::Body::OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node )
{
    Orbiter::OnRegister( p_scenegraph, p_node );
    m_drawable->OnRegister( p_scenegraph, p_node );
}

void DrawSpace::Planetoid::Body::Update( DrawSpace::Utils::TimeManager& p_timemanager )
{
    Orbiter::Update( p_timemanager );
}

void DrawSpace::Planetoid::Body::Update2( DrawSpace::Utils::TimeManager& p_timemanager )
{
    Orbiter::Update2( p_timemanager );
}

void DrawSpace::Planetoid::Body::RegisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph )
{
    // enregistrer le cb evt nodes imperativement AVANT le cb evt cameras
    p_scenegraph.RegisterNodesEvtHandler( m_nodes_evt_cb );
    p_scenegraph.RegisterCameraEvtHandler( m_camera_evt_cb );
    p_scenegraph.RegisterScenegraphEvtHandler( m_scenegraph_evt_cb );
}

void DrawSpace::Planetoid::Body::BindExternalGlobalTexture( DrawSpace::Core::Texture* p_texture, DrawSpace::Pass* p_pass, int p_faceid )
{
    m_drawable->GetNodeFromPass( p_pass, p_faceid )->SetTexture( p_texture, 0 );
}

void DrawSpace::Planetoid::Body::AddShader( DrawSpace::Pass* p_pass, int p_faceid, DrawSpace::Core::Shader* p_shader )
{
    m_drawable->GetNodeFromPass( p_pass, p_faceid )->GetFx()->AddShader( p_shader );
}

DrawSpace::Core::Fx* DrawSpace::Planetoid::Body::CreateFx( DrawSpace::Pass* p_pass, int p_faceid )
{
    Fx* fx = _DRAWSPACE_NEW_( Fx, Fx );
    m_drawable->GetNodeFromPass( p_pass, p_faceid )->SetFx( fx );
    return fx;
}

void DrawSpace::Planetoid::Body::CreateProceduralGlobalTextures( DrawSpace::Pass* p_pass, int p_resol )
{
    for( size_t i = 0; i < 6; i++ )
    {
        ProceduralTexture proc_texture;

        Texture* globalproctexture = _DRAWSPACE_NEW_( Texture, Texture );
        
        globalproctexture->SetFormat( p_resol, p_resol, 4 );
        globalproctexture->SetPurpose( Texture::PURPOSE_COLOR );
   
        m_drawable->GetNodeFromPass( p_pass, i )->SetTexture( globalproctexture, 0 );

        proc_texture.texture = globalproctexture;
        proc_texture.texture_content = NULL;

        m_procedural_global_textures[p_pass].push_back( proc_texture );

        /*
        globalproctexture->AllocTextureContent();
        m_global_proc_textures_content = globalproctexture->GetTextureContentPtr();

        unsigned char* color_ptr = (unsigned char*)m_global_proc_textures_content;

        globalproctexture->UpdateTextureContent();
        */
    }
}

void DrawSpace::Planetoid::Body::InitProceduralGlobalTextures( void )
{
    for( auto it = m_procedural_global_textures.begin(); it != m_procedural_global_textures.end(); ++it )
    {
        for( size_t i = 0; i < it->second.size(); i++ )
        {
            ProceduralTexture proc_texture = it->second[i];
            
            if( false == proc_texture.texture->AllocTextureContent() )
            {
                _DSEXCEPTION( "AllocTextureContent FAILED" );
            }

            proc_texture.texture_content = proc_texture.texture->GetTextureContentPtr();
            if( NULL == proc_texture.texture_content )
            {
                _DSEXCEPTION( "Texture content Ptr is NULL" );
            }

            unsigned char* color_ptr = (unsigned char*)proc_texture.texture_content;
    
            long tw, th, bpp;
            proc_texture.texture->GetFormat( tw, th, bpp );

            double fbm_scale = 2.0;

            for( int y = 0; y < th; y++ )
            {
                for( int x = 0; x < tw; x++ )
                {
                    double f_array[3];
                    unsigned char color;

                    double fx = 2.0 * ( ( (double)x / (double)tw ) - 0.5 ) * fbm_scale;
                    double fy = 2.0 * ( ( (double)y / (double)th ) - 0.5 ) * fbm_scale;

                    switch( i )
                    {
                        case 0:
                            // front
                            f_array[0] = fx;
                            f_array[1] = fy;
                            f_array[2] = fbm_scale;
                            break;

                        case 1:
                            // rear
                            f_array[0] = -fx;
                            f_array[1] = fy;
                            f_array[2] = -fbm_scale;
                            break;

                        case 2:
                            //left
                            f_array[0] = -fbm_scale;
                            f_array[1] = fy;
                            f_array[2] = fx;
                            break;

                        case 3:
                            //right
                            f_array[0] = fbm_scale;
                            f_array[1] = fy;
                            f_array[2] = -fx;          
                            break;

                        case 4:
                            // top
                            f_array[0] = fx;
                            f_array[1] = fbm_scale;
                            f_array[2] = -fy;
                            break;

                        case 5:
                            // bottom
                            f_array[0] = fx;
                            f_array[1] = -fbm_scale;
                            f_array[2] = fy;          
                            break;
                    }

                    double res = m_fractal->fBm( f_array, 15.0 );

                    if( res >= 0.15 && res < 0.65 )
                    {
                        color = 255.0 * ( ( res * 0.5 ) + 0.5 );
                        *color_ptr = color * 0.6; color_ptr++;
                        *color_ptr = color * 0.8; color_ptr++;
                        *color_ptr = color * 0.6; color_ptr++;
                        *color_ptr = color; color_ptr++;

                    }
                    else if ( res >= 0.65 && res < 0.9 )
                    {
                        color = 255.0 * ( ( res * 0.5 ) + 0.5 );
                        *color_ptr = color * 0.6; color_ptr++;
                        *color_ptr = color * 0.6; color_ptr++;
                        *color_ptr = color * 0.6; color_ptr++;
                        *color_ptr = color; color_ptr++;
                    }            
                    else if( res >= 0.9 )
                    {
                        *color_ptr = 255; color_ptr++;
                        *color_ptr = 255; color_ptr++;
                        *color_ptr = 255; color_ptr++;
                        *color_ptr = 255; color_ptr++;            
                    }
                    else
                    {
                        *color_ptr = 75; color_ptr++;
                        *color_ptr = 0; color_ptr++;
                        *color_ptr = 0; color_ptr++;
                        *color_ptr = 0; color_ptr++;                
                    }


                }
            }  

            if( false == proc_texture.texture->UpdateTextureContent() )
            {
                _DSEXCEPTION( "Texture content update FAILED" );
            }
        }
    }
}


