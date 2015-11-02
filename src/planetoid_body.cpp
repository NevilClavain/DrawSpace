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

    m_fractal = new Fractal( 3, 3345764, /*0.65*/ 0.75, 1.29 );
       
    m_drawable = _DRAWSPACE_NEW_( SphericalLOD::Drawing, SphericalLOD::Drawing );
    m_drawable->SetRenderer( SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface );
    
    m_camera_evt_cb = _DRAWSPACE_NEW_( CameraEvtCb, CameraEvtCb( this, &DrawSpace::Planetoid::Body::on_camera_event ) );
    m_nodes_evt_cb = _DRAWSPACE_NEW_( NodesEventCb, NodesEventCb( this, &DrawSpace::Planetoid::Body::on_nodes_event ) );
    m_scenegraph_evt_cb = _DRAWSPACE_NEW_( ScenegraphEventCb, ScenegraphEventCb( this, &DrawSpace::Planetoid::Body::on_scenegraph_event ) );

    for( size_t i = 0; i < 6; i++ )
    {
        m_proceduraltexture_runners[i] = _DRAWSPACE_NEW_( Runner, Runner );

        m_proceduraltexture_runnercb[i] = _DRAWSPACE_NEW_( RunnerMsgCb, RunnerMsgCb( this, &DrawSpace::Planetoid::Body::on_proceduraltexture_request ) );
        m_proceduraltexture_runners[i]->RegisterTaskMsgHandler( m_proceduraltexture_runnercb[i] );        
    }

    m_proceduraltexture_runnerevt[DrawSpace::SphericalLOD::Patch::FrontPlanetFace] = _DRAWSPACE_NEW_( RunnerEvtCb, RunnerEvtCb( this, &DrawSpace::Planetoid::Body::on_front_proceduraltexture_result ) );
    m_proceduraltexture_runners[DrawSpace::SphericalLOD::Patch::FrontPlanetFace]->RegisterEventHandler( m_proceduraltexture_runnerevt[DrawSpace::SphericalLOD::Patch::FrontPlanetFace] );

    m_proceduraltexture_runnerevt[DrawSpace::SphericalLOD::Patch::RearPlanetFace] = _DRAWSPACE_NEW_( RunnerEvtCb, RunnerEvtCb( this, &DrawSpace::Planetoid::Body::on_rear_proceduraltexture_result ) );
    m_proceduraltexture_runners[DrawSpace::SphericalLOD::Patch::RearPlanetFace]->RegisterEventHandler( m_proceduraltexture_runnerevt[DrawSpace::SphericalLOD::Patch::RearPlanetFace] );

    m_proceduraltexture_runnerevt[DrawSpace::SphericalLOD::Patch::LeftPlanetFace] = _DRAWSPACE_NEW_( RunnerEvtCb, RunnerEvtCb( this, &DrawSpace::Planetoid::Body::on_left_proceduraltexture_result ) );
    m_proceduraltexture_runners[DrawSpace::SphericalLOD::Patch::LeftPlanetFace]->RegisterEventHandler( m_proceduraltexture_runnerevt[DrawSpace::SphericalLOD::Patch::LeftPlanetFace] );

    m_proceduraltexture_runnerevt[DrawSpace::SphericalLOD::Patch::RightPlanetFace] = _DRAWSPACE_NEW_( RunnerEvtCb, RunnerEvtCb( this, &DrawSpace::Planetoid::Body::on_right_proceduraltexture_result ) );
    m_proceduraltexture_runners[DrawSpace::SphericalLOD::Patch::RightPlanetFace]->RegisterEventHandler( m_proceduraltexture_runnerevt[DrawSpace::SphericalLOD::Patch::RightPlanetFace] );

    m_proceduraltexture_runnerevt[DrawSpace::SphericalLOD::Patch::TopPlanetFace] = _DRAWSPACE_NEW_( RunnerEvtCb, RunnerEvtCb( this, &DrawSpace::Planetoid::Body::on_top_proceduraltexture_result ) );
    m_proceduraltexture_runners[DrawSpace::SphericalLOD::Patch::TopPlanetFace]->RegisterEventHandler( m_proceduraltexture_runnerevt[DrawSpace::SphericalLOD::Patch::TopPlanetFace] );

    m_proceduraltexture_runnerevt[DrawSpace::SphericalLOD::Patch::BottomPlanetFace] = _DRAWSPACE_NEW_( RunnerEvtCb, RunnerEvtCb( this, &DrawSpace::Planetoid::Body::on_bottom_proceduraltexture_result ) );
    m_proceduraltexture_runners[DrawSpace::SphericalLOD::Patch::BottomPlanetFace]->RegisterEventHandler( m_proceduraltexture_runnerevt[DrawSpace::SphericalLOD::Patch::BottomPlanetFace] );

    for( size_t i = 0; i < 6; i++ )
    {
        m_proceduraltexture_runners[i]->Startup();
    }

    m_front_done = false;
    m_rear_done = false;
    m_left_done = false;
    m_right_done = false;
    m_top_done = false;
    m_bottom_done = false;
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
                        reg_body.relative_alt_valid = false;
                        
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
                                              
                        m_registered_bodies[inertbody] = reg_body;
                    }

                }
                else
                {                    
                    reg_body.attached = false;
                    reg_body.body = inertbody;
                    reg_body.relative_alt_valid = false;

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

                            create_camera_collisions( camera_scenename, camera_node->GetContent(), reg_camera, true );

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
                create_camera_collisions( camera_scenename, camera_node->GetContent(), reg_camera, false );

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
        // PROVISOIRE +@+

        manage_bodies();
        manage_camerapoints();
        update_fragments();

        for( size_t i = 0; i < 6; i++ )
        {
            m_proceduraltexture_runners[i]->Check();
        }

        /*
        update_fragments();
        manage_bodies();
        */
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
        Fragment* bodyfragment = it->second.fragment;

        if( it->second.attached )
        {
            DrawSpace::Utils::Matrix bodypos;

            it->second.body->GetLastLocalWorldTrans( bodypos );

            DrawSpace::Utils::Vector bodypos2;
            bodypos2[0] = bodypos( 3, 0 );
            bodypos2[1] = bodypos( 3, 1 );
            bodypos2[2] = bodypos( 3, 2 );

            dsreal rel_alt = ( bodypos2.Length() / m_ray );
            it->second.relative_alt_valid = true;
            it->second.relative_alt = rel_alt;

            bodyfragment->UpdateRelativeAlt( rel_alt );

            if( rel_alt >= /*1.2*/ 2.1 )
            {

                detach_body( it->second.body );

                /*
                // inutile : le fragment associé à la camera associee au body est le même que le fragment associé au body

                // rechercher si une camera enregistree est associee a ce body
                std::vector<dsstring> cameras;
                body_find_attached_camera( it->second.body, cameras );

                for( size_t i = 0; i < cameras.size(); i++ )
                {
                    m_registered_camerapoints[cameras[i]].camera->SetRelativeOrbiter( NULL );

                    Fragment* fragment = m_registered_camerapoints[cameras[i]].fragment;
                    fragment->SetHotState( false );
                }
                */
                //////

                
                bodyfragment->RemoveColliderFromWorld();

                // ajout 11/2015 : un oubli je pense...
                bodyfragment->SetHotState( false );                

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

            it->second.relative_alt_valid = true;
            it->second.relative_alt = rel_alt;

            bodyfragment->UpdateRelativeAlt( rel_alt );

            if( rel_alt < /*1.1*/ 2.0 )
            {

                attach_body( it->second.body );

                /////
                /*
                // inutile : le fragment associé à la camera associee au body est le même que le fragment associé au body
                std::vector<dsstring> cameras;
                body_find_attached_camera( it->second.body, cameras );

                for( size_t i = 0; i < cameras.size(); i++ )
                {
                    //m_registered_camerapoints[cameras[i]].camera->SetRelativeOrbiter( m_orbiter );
                    m_registered_camerapoints[cameras[i]].camera->SetRelativeOrbiter( this );

                    Fragment* fragment = m_registered_camerapoints[cameras[i]].fragment;
                    fragment->SetHotState( true );                    
                }
                */

                // ajout 11/2015 : un oubli je pense...
                bodyfragment->SetHotState( true );
            }
        }
    }
}

void DrawSpace::Planetoid::Body::manage_camerapoints( void )
{
    for( auto it = m_registered_camerapoints.begin(); it != m_registered_camerapoints.end(); ++it )
    {
        if( it->second.type == FREE_ON_PLANET )
        {
            Matrix camera_pos;

            GetCameraHotpoint( it->first, camera_pos );


            DrawSpace::Utils::Vector camera_pos2;
            camera_pos2[0] = camera_pos( 3, 0 );
            camera_pos2[1] = camera_pos( 3, 1 );
            camera_pos2[2] = camera_pos( 3, 2 );

            dsreal rel_alt = ( camera_pos2.Length() / m_ray );

            it->second.fragment->UpdateRelativeAlt( rel_alt );
        }
        // les camera de type FREE ne sont jamais "hot", donc inutile de le fournir l'altitude relative
        // les cameras de type INERTBODY_LINKED : l'altitude relative est deja fournie au fragment via l'inertbody associe, dans manage_bodies()
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
            if( inertbody->GetAttachedBody() == this )
            {
                std::vector<dsstring> cameras;
                body_find_attached_camera( inertbody, cameras );

                for( size_t i = 0; i < cameras.size(); i++ )
                {
                    m_registered_camerapoints[cameras[i]].camera->SetRelativeAltitude( curr->GetPlanetBody()->GetHotPointAltitud() );
                }
            }
        }
        else if( camera )
        {
            CameraPoint::Infos cam_infos;
            camera->GetInfos( cam_infos );
            camera->SetRelativeAltitude( curr->GetPlanetBody()->GetHotPointAltitud() );
        }       
    }
}

void DrawSpace::Planetoid::Body::create_camera_collisions( const dsstring& p_cameraname, CameraPoint* p_camera, DrawSpace::Planetoid::Body::RegisteredCamera& p_cameradescr, bool p_hotstate )
{
    DrawSpace::SphericalLOD::Body* slod_body = _DRAWSPACE_NEW_( DrawSpace::SphericalLOD::Body, DrawSpace::SphericalLOD::Body( m_ray * 2.0 ) );
    Collider* collider = _DRAWSPACE_NEW_( Collider, Collider );

    dsstring final_name = m_scenename + dsstring( " " ) + p_cameraname;
    Fragment* planet_fragment = _DRAWSPACE_NEW_( Fragment, Fragment( final_name, slod_body, collider, m_ray, false ) );
   
    planet_fragment->SetHotState( p_hotstate );
   
    // si p_hotstate == false, il n'y a pas d'injection de hotpoint dans le spherelod body depuis Fragment::Update
    // donc inutile de faire le SetCamera sur le fragment
    if( p_hotstate )
    {
        planet_fragment->SetCamera( p_camera );
    }

    p_cameradescr.fragment = planet_fragment;    
    p_cameradescr.camera->SetRelativeOrbiter( this );

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
    }
}

void DrawSpace::Planetoid::Body::InitProceduralGlobalTextures( DrawSpace::Pass* p_pass, int p_r, int p_g, int p_b )
{
    if( m_procedural_global_textures.count( p_pass ) > 0 )
    {
        for( size_t i = 0; i < 6; i++ )
        {
            ProceduralTexture proc_texture = m_procedural_global_textures[p_pass][i];
            
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
           
            for( int y = 0; y < th; y++ )
            {
                for( int x = 0; x < tw; x++ )
                {                    
                    *color_ptr = p_b; color_ptr++;
                    *color_ptr = p_g; color_ptr++;
                    *color_ptr = p_r; color_ptr++;
                    *color_ptr = 255; color_ptr++;
                }
            }  

            if( false == proc_texture.texture->UpdateTextureContent() )
            {
                _DSEXCEPTION( "Texture content update FAILED" );
            }

            m_procedural_global_textures[p_pass][i] = proc_texture;
        }        
    }

    /*
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
           
            for( int y = 0; y < th; y++ )
            {
                for( int x = 0; x < tw; x++ )
                {
                    
                    Vector f_array;
                    Vector f_array2;

                    unsigned char color;

                    double fx = 2.0 * ( ( (double)x / (double)tw ) - 0.5 );
                    double fy = 2.0 * ( ( (double)( th - y ) / (double)th ) - 0.5 ); // le v des coords textures et le y du repere patch sont en sens opposés
                                  
                    DrawSpace::SphericalLOD::Patch::XYToXYZ( i, fx, fy, f_array );

                    DrawSpace::SphericalLOD::Patch::CubeToSphere( f_array, f_array2 );

                    f_array2[0] *= 2.0;
                    f_array2[1] *= 2.0;
                    f_array2[2] *= 2.0;

                    double res = m_fractal->fBm( f_array2.GetArray(), 15.0 );

                    if( res >= 0.20 && res < 0.65 )
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
    */
}


void DrawSpace::Planetoid::Body::fill_procedural_texture( int p_direction, const ProceduralTexture& p_procedural_texture, DrawSpace::Utils::Fractal* p_fractal )
{
    unsigned char* color_ptr = (unsigned char*)p_procedural_texture.texture_content;
    
    long tw, th, bpp;
    p_procedural_texture.texture->GetFormat( tw, th, bpp );
           
    for( int y = 0; y < th; y++ )
    {
        for( int x = 0; x < tw; x++ )
        {                    
            Vector f_array;
            Vector f_array2;

            unsigned char color;

            double fx = 2.0 * ( ( (double)x / (double)tw ) - 0.5 );
            double fy = 2.0 * ( ( (double)( th - y ) / (double)th ) - 0.5 ); // le v des coords textures et le y du repere patch sont en sens opposés
                                  
            DrawSpace::SphericalLOD::Patch::XYToXYZ( p_direction, fx, fy, f_array );

            DrawSpace::SphericalLOD::Patch::CubeToSphere( f_array, f_array2 );

            f_array2[0] *= 2.0;
            f_array2[1] *= 2.0;
            f_array2[2] *= 2.0;

            double res = p_fractal->fBm( f_array2.GetArray(), 15.0 );

            if( res >= 0.20 && res < 0.65 )
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
}

DrawSpace::Planetoid::Fragment* DrawSpace::Planetoid::Body::GetFragment( int p_index )
{
    return m_planetfragments_list[p_index];
}

void DrawSpace::Planetoid::Body::on_proceduraltexture_request( DrawSpace::Core::PropertyPool* p_args )
{
    int dir = p_args->GetPropValue<int>( "direction" );
    Fractal fractal( 3, 3345764, 0.75, 1.29 );
    DrawSpace::Pass* pass = p_args->GetPropValue<DrawSpace::Pass*>( "pass" );

    fill_procedural_texture( dir, m_procedural_global_textures[pass][dir], &fractal );
}

void DrawSpace::Planetoid::Body::on_front_proceduraltexture_result( DrawSpace::Core::Runner::State p_runnerstate )
{
    if( p_runnerstate == DrawSpace::Core::Runner::TASK_DONE )
    {
        m_front_done = true;
        m_procedural_global_textures[m_procedural_texture_currentpass][SphericalLOD::Patch::FrontPlanetFace].texture->UpdateTextureContent();
    }
}

void DrawSpace::Planetoid::Body::on_rear_proceduraltexture_result( DrawSpace::Core::Runner::State p_runnerstate )
{
    if( p_runnerstate == DrawSpace::Core::Runner::TASK_DONE )
    {
        m_rear_done = true;
        m_procedural_global_textures[m_procedural_texture_currentpass][SphericalLOD::Patch::RearPlanetFace].texture->UpdateTextureContent();
    }
}

void DrawSpace::Planetoid::Body::on_left_proceduraltexture_result( DrawSpace::Core::Runner::State p_runnerstate )
{
    if( p_runnerstate == DrawSpace::Core::Runner::TASK_DONE )
    {
        m_left_done = true;
        m_procedural_global_textures[m_procedural_texture_currentpass][SphericalLOD::Patch::LeftPlanetFace].texture->UpdateTextureContent();
    }
}

void DrawSpace::Planetoid::Body::on_right_proceduraltexture_result( DrawSpace::Core::Runner::State p_runnerstate )
{
    if( p_runnerstate == DrawSpace::Core::Runner::TASK_DONE )
    {
        m_right_done = true;
        m_procedural_global_textures[m_procedural_texture_currentpass][SphericalLOD::Patch::RightPlanetFace].texture->UpdateTextureContent();
    }
}

void DrawSpace::Planetoid::Body::on_top_proceduraltexture_result( DrawSpace::Core::Runner::State p_runnerstate )
{
    if( p_runnerstate == DrawSpace::Core::Runner::TASK_DONE )
    {
        m_top_done = true;
        m_procedural_global_textures[m_procedural_texture_currentpass][SphericalLOD::Patch::TopPlanetFace].texture->UpdateTextureContent();
    }
}

void DrawSpace::Planetoid::Body::on_bottom_proceduraltexture_result( DrawSpace::Core::Runner::State p_runnerstate )
{
    if( p_runnerstate == DrawSpace::Core::Runner::TASK_DONE )
    {
        m_bottom_done = true;
        m_procedural_global_textures[m_procedural_texture_currentpass][SphericalLOD::Patch::BottomPlanetFace].texture->UpdateTextureContent();
    }
}

void DrawSpace::Planetoid::Body::run_textures( DrawSpace::Pass* p_pass )
{
    m_procedural_texture_currentpass = p_pass;

    PropertyPool frontrunner_props;
    PropertyPool rearrunner_props;
    PropertyPool leftrunner_props;
    PropertyPool rightrunner_props;
    PropertyPool toprunner_props;
    PropertyPool bottomrunner_props;

    frontrunner_props.AddPropValue<int>( "direction", SphericalLOD::Patch::FrontPlanetFace );
    frontrunner_props.AddPropValue<DrawSpace::Pass*>( "pass", p_pass );
    m_proceduraltexture_runners[DrawSpace::SphericalLOD::Patch::FrontPlanetFace]->PushMessage( frontrunner_props );
    return;
    
    rearrunner_props.AddPropValue<int>( "direction", SphericalLOD::Patch::RearPlanetFace );
    rearrunner_props.AddPropValue<DrawSpace::Pass*>( "pass", p_pass );
    m_proceduraltexture_runners[DrawSpace::SphericalLOD::Patch::RearPlanetFace]->PushMessage( rearrunner_props );
        
    leftrunner_props.AddPropValue<int>( "direction", SphericalLOD::Patch::LeftPlanetFace );
    leftrunner_props.AddPropValue<DrawSpace::Pass*>( "pass", p_pass );
    m_proceduraltexture_runners[DrawSpace::SphericalLOD::Patch::LeftPlanetFace]->PushMessage( leftrunner_props );
        
    rightrunner_props.AddPropValue<int>( "direction", SphericalLOD::Patch::RightPlanetFace );
    rightrunner_props.AddPropValue<DrawSpace::Pass*>( "pass", p_pass );
    m_proceduraltexture_runners[DrawSpace::SphericalLOD::Patch::RightPlanetFace]->PushMessage( rightrunner_props );
        
    toprunner_props.AddPropValue<int>( "direction", SphericalLOD::Patch::TopPlanetFace );
    toprunner_props.AddPropValue<DrawSpace::Pass*>( "pass", p_pass );
    m_proceduraltexture_runners[DrawSpace::SphericalLOD::Patch::TopPlanetFace]->PushMessage( toprunner_props );
        
    bottomrunner_props.AddPropValue<int>( "direction", SphericalLOD::Patch::BottomPlanetFace );
    bottomrunner_props.AddPropValue<DrawSpace::Pass*>( "pass", p_pass );
    m_proceduraltexture_runners[DrawSpace::SphericalLOD::Patch::BottomPlanetFace]->PushMessage( bottomrunner_props );
}

bool DrawSpace::Planetoid::Body::GetInertBodyRelativeAltitude( DrawSpace::Dynamics::InertBody* p_body, dsreal& p_rel_altitude )
{
    if( m_registered_bodies.count( p_body ) > 0 )
    {
        RegisteredBody entry = m_registered_bodies[p_body];
        if( entry.relative_alt_valid )
        {
            p_rel_altitude = entry.relative_alt;
            return true;
        }
        return false;
    }
    return false;
}