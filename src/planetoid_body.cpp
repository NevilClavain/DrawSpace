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


DrawSpace::Planetoid::Body::Body( const dsstring& p_scenename, dsreal p_ray ) : Orbiter( &m_world, NULL ),
m_scenename( p_scenename ),
m_ray( p_ray * 1000.0 )
{
    m_world.Initialize();
       
    m_drawable = _DRAWSPACE_NEW_( SphericalLOD::Drawing, SphericalLOD::Drawing );
    m_drawable->SetSceneName( p_scenename );
    m_drawable->SetRenderer( SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface );
    
    //m_orbiter = _DRAWSPACE_NEW_( Orbiter, Orbiter( &m_world, m_drawable ) );

    m_camera_evt_cb = _DRAWSPACE_NEW_( CameraEvtCb, CameraEvtCb( this, &DrawSpace::Planetoid::Body::on_camera_event ) );
    m_nodes_evt_cb = _DRAWSPACE_NEW_( NodesEventCb, NodesEventCb( this, &DrawSpace::Planetoid::Body::on_nodes_event ) );
}

DrawSpace::Planetoid::Body::~Body( void )
{

}

void DrawSpace::Planetoid::Body::GetCameraHotpoint( const dsstring& p_name, Matrix& p_outmat )
{
    if( m_registered_camerapoints.count( p_name ) == 0 )
    {
        return;
    }

    if( INERTBODY_LINKED == m_registered_camerapoints[p_name].type )
    {
        m_registered_camerapoints[p_name].attached_body->GetLastLocalWorldTrans( p_outmat );
    }
    else if( COLLIDER_LINKED == m_registered_camerapoints[p_name].type )
    {
        m_registered_camerapoints[p_name].attached_collider->GetLastLocalWorldTrans( p_outmat );
    }
    else if( FREE_ON_PLANET == m_registered_camerapoints[p_name].type )
    {
        m_registered_camerapoints[p_name].camera->GetLocalTransform( p_outmat );
    }
}

void DrawSpace::Planetoid::Body::attach_body( InertBody* p_body )
{
    //p_body->Attach( m_orbiter );
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
    for( std::map<dsstring, RegisteredCamera>::iterator it = m_registered_camerapoints.begin(); it != m_registered_camerapoints.end(); ++it )
    {
        if( it->second.camera->GetAttachedBody() == p_body )
        {
            p_name.push_back( it->first );            
        }
    }
}

//void DrawSpace::Planetoid::Body::on_camera_event( Scenegraph::CameraEvent p_event, TransformNode* p_node )

void DrawSpace::Planetoid::Body::on_camera_event( DrawSpace::Core::SceneNodeGraph::CameraEvent p_event, DrawSpace::Core::BaseSceneNode* p_node )
{
    if( DrawSpace::Scenegraph::ACTIVE == p_event )
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

            /*
            for( std::map<dsstring, RegisteredCamera>::iterator it = m_registered_camerapoints.begin(); it != m_registered_camerapoints.end(); ++it )
            {
                if( it->second.camera == p_node )                
                {
                    Fragment* fragment = it->second.fragment;
                    m_drawable->SetCurrentPlanetBody( fragment->GetPlanetBody() );
                    
                    break;
                }
            }
            */
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
        if( inertbody_node )
        {          
            if( inertbody_node->CheckFlag( SCENENODE_FLAG_DLINKSCAN ) )
            {
                RegisteredBody reg_body;

                if( inertbody_node->CheckFlag( SCENENODE_FLAG_DLINKED ) )
                {
                    reg_body.attached = true;
                    reg_body.body = inertbody_node->GetContent();

                    Matrix body_initmat;
                    dsstring bodyname;                   
                    p_node->GetSceneName( bodyname );
                   
                    if( m_includedbody_initmat_table.count( bodyname ) > 0 )
                    {
                        body_initmat = m_includedbody_initmat_table[bodyname];
                    }
                    else
                    {
                        // this included inertbody does not belong to this planet...
                        return;
                    }

                    inertbody_node->GetContent()->IncludeTo( this, body_initmat );

                    DrawSpace::SphericalLOD::Body* slod_body = _DRAWSPACE_NEW_( DrawSpace::SphericalLOD::Body, DrawSpace::SphericalLOD::Body( m_ray * 2.0 ) );
                    Collider* collider = _DRAWSPACE_NEW_( Collider, Collider( NULL ) );


                    dsstring final_name = m_scenename + dsstring( " " ) + bodyname;
                    Fragment* planet_fragment = _DRAWSPACE_NEW_( Fragment, Fragment( final_name, slod_body, collider, m_ray, true ) );
                    planet_fragment->SetHotState( true );

                    m_planetfragments_list.push_back( planet_fragment );
                    reg_body.fragment = planet_fragment;

                    planet_fragment->SetInertBody( inertbody_node->GetContent() );

                    slod_body->Initialize();
                    
                    m_registered_bodies[inertbody_node->GetContent()] = reg_body;

                }
                else
                {                    
                    reg_body.attached = false;
                    reg_body.body = inertbody_node->GetContent();

                    DrawSpace::SphericalLOD::Body* slod_body = _DRAWSPACE_NEW_( DrawSpace::SphericalLOD::Body, DrawSpace::SphericalLOD::Body( m_ray * 2.0 ) );
                    Collider* collider = _DRAWSPACE_NEW_( Collider, Collider( NULL ) );

                    dsstring bodyname;                   
                    p_node->GetSceneName( bodyname );
                   
                    dsstring final_name = m_scenename + dsstring( " " ) + bodyname;

                    Fragment* planet_fragment = _DRAWSPACE_NEW_( Fragment, Fragment( final_name, slod_body, collider, m_ray, true ) );
                    planet_fragment->SetHotState( false );

                    m_planetfragments_list.push_back( planet_fragment );
                    reg_body.fragment = planet_fragment;

                    planet_fragment->SetInertBody( inertbody_node->GetContent() );

                    slod_body->Initialize();

                    m_registered_bodies[inertbody_node->GetContent()] = reg_body;
                }
            }
            return;
        }
        
        SceneNode<CameraPoint>* camera_node = dynamic_cast<SceneNode<CameraPoint>*>( p_node );
        if( camera_node )
        {
            RegisteredCamera reg_camera;
            dsstring camera_scenename;

            p_node->GetSceneName( camera_scenename );

            if( m_camerabodyassociation_table.count( camera_node->GetContent() ) > 0 )
            {
                DrawSpace::Dynamics::Body* attached_body = m_camerabodyassociation_table[camera_node->GetContent()];
                InertBody* inert_body = dynamic_cast<InertBody*>( attached_body );

                if( inert_body )
                {
                    if( m_registered_bodies.count( inert_body ) > 0 )
                    {
                        reg_camera.type = INERTBODY_LINKED;
                        reg_camera.attached_body = inert_body;
                        reg_camera.attached_collider = NULL;
                        reg_camera.fragment = m_registered_bodies[inert_body].fragment;
                    }
                    else
                    {
                        return;
                    }
                }
                else
                {
                    Orbiter* orbiter = dynamic_cast<Orbiter*>( attached_body );
                    if( orbiter )
                    {
                        if( orbiter == this )
                        {
                            // la camera est attachee a notre planete !

                            reg_camera.type = FREE_ON_PLANET;
                            reg_camera.attached_body = NULL;
                            reg_camera.attached_collider = NULL;

                            create_camera_collisions( camera_scenename, camera_node->GetContent(), reg_camera );

                            reg_camera.camera->SetRelativeOrbiter( this );

                        }
                        else
                        {
                            _DSEXCEPTION( "orbiter associated with camera " + camera_scenename + " is not THIS planet" );
                        }
                    }
                    else
                    {
                        Collider* collider = dynamic_cast<Collider*>( attached_body );

                        if( collider )
                        {
                            reg_camera.type = COLLIDER_LINKED;
                            reg_camera.attached_body = NULL;
                            reg_camera.attached_collider = collider;

                            create_camera_collisions( camera_scenename, camera_node->GetContent(), reg_camera );

                            reg_camera.camera->SetRelativeOrbiter( this );
                        }
                        else
                        {

                        }
                    }
                }
            }
            else
            {
                reg_camera.type = FREE;
                reg_camera.attached_body = NULL;
                create_camera_collisions( camera_scenename, camera_node->GetContent(), reg_camera );
            }

            return;
        }
    }
}

/*
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
*/

DrawSpace::Planetoid::Body::CameraEvtCb* DrawSpace::Planetoid::Body::GetCameraEvtCb( void )
{
    return m_camera_evt_cb;
}

DrawSpace::Planetoid::Body::NodesEventCb* DrawSpace::Planetoid::Body::GetNodesEvtCb( void )
{
    return m_nodes_evt_cb;
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

void DrawSpace::Planetoid::Body::ManageBodies( void )
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

                DrawSpace::Core::TransformNode* node;                
                node = it->second.body->GetDrawable();


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

void DrawSpace::Planetoid::Body::Update( void )
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
            if( inertbody->GetRefBody() == this )
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

void DrawSpace::Planetoid::Body::RegisterLinkedBodyInitMatrix( const dsstring& p_name, const DrawSpace::Utils::Matrix& p_initmat )
{
    m_includedbody_initmat_table[p_name] = p_initmat;
}

void DrawSpace::Planetoid::Body::RegisterCameraBodyAssociation( DrawSpace::Dynamics::CameraPoint* p_camera, DrawSpace::Dynamics::Body* p_body )
{
    m_camerabodyassociation_table[p_camera] = p_body;
}

/*
void DrawSpace::Planetoid::Body::RegisterInertBody( const dsstring& p_bodyname, InertBody* p_body )
{
    RegisteredBody reg_body;

    reg_body.attached = false;
    reg_body.body = p_body;

    DrawSpace::SphericalLOD::Body* slod_body = _DRAWSPACE_NEW_( DrawSpace::SphericalLOD::Body, DrawSpace::SphericalLOD::Body( m_ray * 2.0 ) );
    Collider* collider = _DRAWSPACE_NEW_( Collider, Collider( NULL ) );

    dsstring final_name = m_scenename + dsstring( " " ) + p_bodyname;
    Fragment* planet_fragment = _DRAWSPACE_NEW_( Fragment, Fragment( final_name, slod_body, collider, m_ray, true ) );
    planet_fragment->SetHotState( false );

    m_planetfragments_list.push_back( planet_fragment );
    reg_body.fragment = planet_fragment;

    planet_fragment->SetInertBody( p_body );

    slod_body->Initialize();

    m_sphericallod_body_list[p_bodyname] = slod_body;
    m_registered_bodies[p_body] = reg_body;
}

void DrawSpace::Planetoid::Body::RegisterIncludedInertBody( const dsstring& p_bodyname, InertBody* p_body, const Matrix& p_initmat )
{
    RegisteredBody reg_body;

    reg_body.attached = true;
    reg_body.body = p_body;

    p_body->IncludeTo( m_orbiter, p_initmat );

    DrawSpace::SphericalLOD::Body* slod_body = _DRAWSPACE_NEW_( DrawSpace::SphericalLOD::Body, DrawSpace::SphericalLOD::Body( m_ray * 2.0 ) );
    Collider* collider = _DRAWSPACE_NEW_( Collider, Collider( NULL ) );

    dsstring final_name = m_scenename + dsstring( " " ) + p_bodyname;
    Fragment* planet_fragment = _DRAWSPACE_NEW_( Fragment, Fragment( final_name, slod_body, collider, m_ray, true ) );
    planet_fragment->SetHotState( true );

    m_planetfragments_list.push_back( planet_fragment );
    reg_body.fragment = planet_fragment;

    planet_fragment->SetInertBody( p_body );

    slod_body->Initialize();

    m_sphericallod_body_list[p_bodyname] = slod_body;
    m_registered_bodies[p_body] = reg_body;
}
*/

void DrawSpace::Planetoid::Body::create_camera_collisions( const dsstring& p_cameraname, CameraPoint* p_camera, DrawSpace::Planetoid::Body::RegisteredCamera& p_cameradescr )
{
    DrawSpace::SphericalLOD::Body* slod_body = _DRAWSPACE_NEW_( DrawSpace::SphericalLOD::Body, DrawSpace::SphericalLOD::Body( m_ray * 2.0 ) );
    Collider* collider = _DRAWSPACE_NEW_( Collider, Collider( NULL ) );

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

/*
bool DrawSpace::Planetoid::Body::RegisterCameraPoint( CameraPoint* p_camera )
{
    RegisteredCamera reg_camera;

    dsstring camera_scenename;
    p_camera->GetSceneName( camera_scenename );

    reg_camera.camera = p_camera;

    ////

    Dynamics::Body* attached_body = p_camera->GetAttachedBody();

    if( attached_body )
    {
        InertBody* inert_body = dynamic_cast<InertBody*>( attached_body );
        if( inert_body )
        {
            if( m_registered_bodies.count( inert_body ) > 0 )
            {
                reg_camera.type = INERTBODY_LINKED;
                reg_camera.attached_body = inert_body;
                reg_camera.attached_collider = NULL;
                reg_camera.fragment = m_registered_bodies[inert_body].fragment;
            }
            else
            {
                // body attache n'est pas enregistre, erreur
                return false;
            }
        }
        else
        {
            Orbiter* orbiter = dynamic_cast<Orbiter*>( attached_body );
            if( orbiter )
            {
                // attachee a un orbiter
                if( orbiter == m_orbiter )
                {
                    // la camera est attachee a notre planete !

                    reg_camera.type = FREE_ON_PLANET;
                    reg_camera.attached_body = NULL;
                    reg_camera.attached_collider = NULL;

                    create_camera_collisions( camera_scenename, p_camera, reg_camera );

                    reg_camera.camera->SetRelativeOrbiter( m_orbiter );
                }
                else
                {
                    // la camera est attachee a un autre orbiter
                    return false;
                }
            }
            else
            {
                Collider* collider = dynamic_cast<Collider*>( attached_body );

                if( collider )
                {                    
                    reg_camera.type = COLLIDER_LINKED;
                    reg_camera.attached_body = NULL;
                    reg_camera.attached_collider = collider;

                    create_camera_collisions( camera_scenename, p_camera, reg_camera );

                    reg_camera.camera->SetRelativeOrbiter( m_orbiter );
                }
                else
                {
                    // camera attachee a autre chose qu'un orbiter ou un inertbody (???)
                    return false;
                }
            }
        }
    }
    else
    {
        // camera libre (attachee a aucun body)
        reg_camera.type = FREE;
        reg_camera.attached_body = NULL;

        create_camera_collisions( camera_scenename, p_camera, reg_camera );
    }

    ////

    m_registered_camerapoints[camera_scenename] = reg_camera;

    return true;
}
*/

void DrawSpace::Planetoid::Body::GetSceneName( dsstring& p_name )
{
    p_name = m_scenename;
}

/*
void DrawSpace::Planetoid::Body::RegisterCollider( DrawSpace::Dynamics::Collider* p_collider )
{
    p_collider->AddToWorld( &m_world );
    p_collider->SetRootOrbiter( m_orbiter );
}
*/

Fragment* DrawSpace::Planetoid::Body::GetRegisteredBodyPlanetFragment( DrawSpace::Dynamics::InertBody* p_body )
{
    if( 0 == m_registered_bodies.count( p_body ) )
    {
        return NULL;
    }    
    return m_registered_bodies[p_body].fragment;
}

void DrawSpace::Planetoid::Body::RegisterPassSlot( const dsstring& p_passname )
{
    m_drawable->RegisterPassSlot( p_passname );
}

DrawSpace::Core::RenderingNode* DrawSpace::Planetoid::Body::GetNodeFromPass( const dsstring& p_passname, int p_faceid )
{
    return m_drawable->GetNodeFromPass( p_passname, p_faceid );
}

void DrawSpace::Planetoid::Body::SetNodeFromPassSpecificFx( const dsstring& p_passname, int p_faceid, const dsstring& p_fxname )
{
    m_drawable->SetNodeFromPassSpecificFx( p_passname, p_faceid, p_fxname );
}

void DrawSpace::Planetoid::Body::SetFinalTransform( const DrawSpace::Utils::Matrix& p_mat )
{
    m_drawable->SetFinalTransform( p_mat );
}

void DrawSpace::Planetoid::Body::OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node )
{
    m_drawable->OnRegister( p_scenegraph, p_node );
}

void DrawSpace::Planetoid::Body::Update( DrawSpace::Utils::TimeManager& p_timemanager )
{
    Orbiter::Update( p_timemanager );
    ApplyGravity();
    Update();
    ManageBodies();
}