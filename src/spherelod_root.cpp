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

#include "spherelod_root.h"
#include "plugin.h"
#include "exceptions.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::SphericalLOD;

Root::Root( const dsstring& p_scenename, dsreal p_ray, DrawSpace::Utils::TimeManager* p_time, const SphericalLOD::Config& p_config ) : Orbiter( &m_world ),
m_scenename( p_scenename ),
m_ray( p_ray * 1000.0 ),
m_timemanager( p_time ),
m_enable_gravity( true )
{
    m_world.Initialize();

    m_config = _DRAWSPACE_NEW_( SphericalLOD::Config, SphericalLOD::Config );
    *m_config = p_config;
       
    m_drawable = _DRAWSPACE_NEW_( SphericalLOD::Drawing, SphericalLOD::Drawing( m_config ) );
    m_drawable->SetRenderer( SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface );
    
    m_camera_evt_cb = _DRAWSPACE_NEW_( CameraEvtCb, CameraEvtCb( this, &DrawSpace::SphericalLOD::Root::on_camera_event ) );
    m_nodes_evt_cb = _DRAWSPACE_NEW_( NodesEventCb, NodesEventCb( this, &DrawSpace::SphericalLOD::Root::on_nodes_event ) );
    m_scenegraph_evt_cb = _DRAWSPACE_NEW_( ScenegraphEventCb, ScenegraphEventCb( this, &DrawSpace::SphericalLOD::Root::on_scenegraph_event ) );

    m_subpass_creation_cb = _DRAWSPACE_NEW_( SubPassCreationCb, SubPassCreationCb( this, &DrawSpace::SphericalLOD::Root::on_subpasscreation ) );

    m_timer_cb = _DRAWSPACE_NEW_( TimerCb, TimerCb( this, &DrawSpace::SphericalLOD::Root::on_timer ) );
    m_timer = _DRAWSPACE_NEW_( DrawSpace::Utils::Timer, DrawSpace::Utils::Timer );

    m_timer->SetHandler( m_timer_cb );
    m_timer->SetPeriod( /*100*/ /*60*/ 20 );
    m_timemanager->RegisterTimer( m_timer );

    m_timer->SetState( true );
}

Root::~Root( void )
{
}

void Root::attach_body( InertBody* p_body )
{
    p_body->Attach( this );
    m_registered_bodies[p_body].attached = true;
}

void Root::detach_body( InertBody* p_body )
{
    p_body->Detach();
    m_registered_bodies[p_body].attached = false;
}

void Root::body_find_attached_camera( InertBody* p_body, std::vector<dsstring>& p_name )
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

void Root::on_camera_event( DrawSpace::Core::SceneNodeGraph::CameraEvent p_event, DrawSpace::Core::BaseSceneNode* p_node )
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
            std::vector<SphericalLOD::Body*> planet_bodies;

            for( size_t i = 0; i < m_registered_camerapoints[current_camera_scenename].layers.size(); i++ )
            {
                SphericalLOD::Layer* layer = m_registered_camerapoints[current_camera_scenename].layers[i];
            
                planet_bodies.push_back( layer->GetBody() );
            }
            m_drawable->SetCurrentPlanetBodies( planet_bodies );
        }
    }
}

void Root::on_nodes_event( DrawSpace::Core::SceneNodeGraph::NodesEvent p_event, DrawSpace::Core::BaseSceneNode* p_node )
{
    if( SceneNodeGraph::NODE_REGISTERED == p_event )
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

                        for( size_t i = 0; i < m_config->m_layers_descr.size(); i++ )
                        {                                                                                                                
                            inertbody->IncludeTo( this );

                            DrawSpace::SphericalLOD::Body* slod_body = _DRAWSPACE_NEW_( DrawSpace::SphericalLOD::Body, 
                                DrawSpace::SphericalLOD::Body( m_timemanager, m_config, i, m_subpass_creation_cb, m_config->m_nbLODRanges_inertBodies ) );

                            Collider* collider = _DRAWSPACE_NEW_( Collider, Collider );
                        
                            SphericalLOD::Layer* layer = _DRAWSPACE_NEW_( SphericalLOD::Layer, SphericalLOD::Layer( this, m_config, &m_world, slod_body, 
                                                                            collider, m_subpass_creation_cb, i ) );
                            layer->SetHotState( true );

                            m_layers_list.push_back( layer );
                            reg_body.layers.push_back( layer );
                            layer->SetInertBody( inertbody );
                        }
                                              
                        m_registered_bodies[inertbody] = reg_body;
                    }
                }
                else
                {

                    reg_body.attached = false;
                    reg_body.body = inertbody;
                    reg_body.relative_alt_valid = false;

                    for( size_t i = 0; i < m_config->m_layers_descr.size(); i++ )
                    {                                                                                                                
                        DrawSpace::SphericalLOD::Body* slod_body = _DRAWSPACE_NEW_( DrawSpace::SphericalLOD::Body, 
                            DrawSpace::SphericalLOD::Body( m_timemanager, m_config, i, m_subpass_creation_cb, m_config->m_nbLODRanges_inertBodies ) );

                        Collider* collider = _DRAWSPACE_NEW_( Collider, Collider );
                   
                        SphericalLOD::Layer* layer = _DRAWSPACE_NEW_( SphericalLOD::Layer, SphericalLOD::Layer( this, m_config, &m_world, slod_body, 
                                                                        collider, m_subpass_creation_cb, i ) );
                        layer->SetHotState( false );                   
                        m_layers_list.push_back( layer );
                        reg_body.layers.push_back( layer );
                        layer->SetInertBody( inertbody );  
                    }

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

            reg_camera.relative_alt_valid = false;

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
                        
                        reg_camera.layers = m_registered_bodies[inert_body].layers;

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
                        // le body referent de la camera ne peut pas �tre autre chose qu'un inertbody ou un orbiter
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
    else if( SceneNodeGraph::NODE_UNREGISTERED == p_event )
    {
        _asm nop
    }
}

void Root::create_camera_collisions( const dsstring& p_cameraname, CameraPoint* p_camera, Root::RegisteredCamera& p_cameradescr, bool p_hotstate )
{
    for( size_t i = 0; i < m_config->m_layers_descr.size(); i++ )
    {
        DrawSpace::SphericalLOD::Body* slod_body = _DRAWSPACE_NEW_( DrawSpace::SphericalLOD::Body, DrawSpace::SphericalLOD::Body( m_timemanager, m_config, i, m_subpass_creation_cb, m_config->m_nbLODRanges_freeCameras ) );

        Collider* collider = _DRAWSPACE_NEW_( Collider, Collider );
    
        SphericalLOD::Layer* layer = _DRAWSPACE_NEW_( SphericalLOD::Layer, SphericalLOD::Layer( this, m_config, &m_world, slod_body, collider, m_subpass_creation_cb, i ) );
   
        layer->SetHotState( p_hotstate );   
        layer->SetCamera( p_camera );
        
        p_cameradescr.layers.push_back( layer );
        m_layers_list.push_back( layer );    
    }

    p_cameradescr.camera->SetRelativeOrbiter( this );   
}


void Root::on_scenegraph_event( SceneNodeGraph::ScenegraphEvent p_event, SceneNodeGraph* p_scenegraph )
{
    if( DrawSpace::Core::SceneNodeGraph::TRANSFORMATIONS_BEGIN == p_event )
    {
        if( m_enable_gravity )
        {
            apply_gravity();
        }
    }
    else if( DrawSpace::Core::SceneNodeGraph::TRANSFORMATIONS_DONE == p_event )
    {
        compute_layers();
        manage_bodies();
        manage_camerapoints();
        update_cameras_alt();
    }
}

void Root::on_timer( DrawSpace::Utils::Timer* p_timer )
{
    /*
    DrawSpace::SphericalLOD::SubPass* sp = pop_next_subpass();
    if( sp )
    {
        m_singleshot_subpasses.push_back( sp );
    }
    */

    if( m_singleshot_subpasses_stack.size() > 0 )
    {
        DrawSpace::SphericalLOD::SubPass* pass = m_singleshot_subpasses_stack.back();
        pass->SetTimerReadyFlag( true );
    }
}

void Root::apply_gravity( void )
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

void Root::manage_bodies( void )
{
    for( std::map<DrawSpace::Dynamics::InertBody*, RegisteredBody>::iterator it = m_registered_bodies.begin(); it != m_registered_bodies.end(); ++it )
    {

        for( size_t i = 0; i < it->second.layers.size(); i++ )
        {
            if( i == m_config->m_ground_layer )
            {
                SphericalLOD::Layer* layer = it->second.layers[i];

                ///////////////////////// calcul pos relative et passage/fin hot en se basant sur bullet

                if( it->second.attached )
                {

                    ///////////////////////// calcul pos relative en se basant sur bullet
                    DrawSpace::Utils::Matrix bodypos;

                    it->second.body->GetLastLocalWorldTrans( bodypos );

                    DrawSpace::Utils::Vector bodypos2;
                    bodypos2[0] = bodypos( 3, 0 );
                    bodypos2[1] = bodypos( 3, 1 );
                    bodypos2[2] = bodypos( 3, 2 );

                    dsreal rel_alt = ( bodypos2.Length() / m_ray );
                    it->second.relative_alt_valid = true;
                    it->second.relative_alt = rel_alt;

                    layer->UpdateRelativeAlt( rel_alt );
                    layer->UpdateRelativeHotViewerPos( bodypos2 );

                    if( rel_alt >= /*1.2*/ /*2.1*/ 4.5 )
                    {
                        detach_body( it->second.body );                
                        layer->RemoveColliderFromWorld();
                        layer->SetHotState( false );
                        layer->ResetBody();
                    }

                    ///////////////////////////////////////////////////////////////////////
                }
                else
                {

                    ///////////////////////// calcul pos relative en se basant sur bullet

                    DrawSpace::Utils::Matrix bodypos;

                    it->second.body->GetLastLocalWorldTrans( bodypos );

                    DrawSpace::Utils::Vector bodypos2;
                    bodypos2[0] = bodypos( 3, 0 );
                    bodypos2[1] = bodypos( 3, 1 );
                    bodypos2[2] = bodypos( 3, 2 );

                    DrawSpace::Utils::Matrix planetbodypos;            
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

                    layer->UpdateRelativeAlt( rel_alt );

                    if( rel_alt < /*1.1*/ /*2.0*/ 4.2 )
                    {
                        attach_body( it->second.body );
                        layer->SetHotState( true );
                    }

                    //////////////////////////////////////////////////////////////
                }

                /////////////////////////////////////////////////////////////////

                ///////////////////////// calcul pos relative en utilisant le scenegraph : cette pos relative ne change pas
                ///////////////////////// quand on passe ou quitte le mode "hot", contrairement a la pos relative calculee
                ///////////////////////// via bullet


            }
        }
    }
}

void Root::manage_camerapoints( void )
{
    for( auto it = m_registered_camerapoints.begin(); it != m_registered_camerapoints.end(); ++it )
    {
        if( it->second.type == FREE_ON_PLANET )
        {
            Matrix camera_pos;

            /*
            GetCameraHotpoint( it->first, camera_pos );
            */

            SceneNode<CameraPoint>* camera_node = m_registered_camerapoints[it->first].camera->GetOwner();

            Orbiter* orbiter = static_cast<Orbiter*>( m_registered_camerapoints[it->first].camera->GetReferentBody() );
            camera_pos.Identity();
            camera_node->GetTransformationRelativeTo( orbiter->GetOwner(), camera_pos );


            DrawSpace::Utils::Vector camera_pos2;
            camera_pos2[0] = camera_pos( 3, 0 );
            camera_pos2[1] = camera_pos( 3, 1 );
            camera_pos2[2] = camera_pos( 3, 2 );

            dsreal rel_alt = ( camera_pos2.Length() / m_ray );

            m_registered_camerapoints[it->first].relative_alt = rel_alt;
            m_registered_camerapoints[it->first].relative_alt_valid = true;
            
            for( size_t i = 0; i < it->second.layers.size(); i++ )
            {
                it->second.layers[i]->UpdateRelativeAlt( rel_alt );
            }
        }
        // les camera de type FREE ne sont jamais "hot", donc inutile de leur fournir l'altitude relative
        // les cameras de type INERTBODY_LINKED : l'altitude relative est deja fournie au layer via l'inertbody associe, dans manage_bodies()
    }
}

void Root::compute_layers( void )
{
    for( size_t i = 0; i < m_layers_list.size(); i++ )
    {
        SphericalLOD::Layer* curr = m_layers_list[i];
        curr->Compute( this );
    }
}

void Root::update_cameras_alt( void )
{
    // ECH 01/04/2016. Refonte - simplification de cet algo :
    // 
    // parcourir les registered cameras; pour chaque entree de m_registered_camerapoints faire :
    //      - si un referent body est associé ( GetReferentBody() non NULL ) alors
    //      -    recup du hotpoint altitude ( GetPlanetBody()->GetHotPointAltitud() )
    //      -    injecter valeur hotpoint altitude dans la camera


    for( auto it = m_registered_camerapoints.begin(); it != m_registered_camerapoints.end(); ++it )
    {
        if( it->second.camera->GetReferentBody() )
        {
            InertBody* inertbody = static_cast<InertBody*>( it->second.camera->GetReferentBody() );

            if( m_registered_bodies.count( inertbody ) )
            {                
                it->second.camera->SetRelativeAltitude( m_registered_bodies[inertbody].layers[m_config->m_ground_layer]->GetBody()->GetHotPointAltitud() );
            }
        }
    }
}

void Root::GetSceneName( dsstring& p_name )
{
    p_name = m_scenename;
}
/*
void DrawSpace::SphericalLOD::Body::RegisterPlanetBodyPassSlot( Pass* p_pass, SphericalLOD::Binder* p_binder )
{
    m_drawable->RegisterPlanetBodyPassSlot( p_pass, p_binder );
}
*/

/*
DrawSpace::SphericalLOD::FaceDrawingNode* Root::RegisterSinglePlanetBodyPassSlot( Pass* p_pass, SphericalLOD::Binder* p_binder, int p_orientation, 
                                                                    DrawSpace::SphericalLOD::Body::MesheType p_meshe_type, int p_layer_index )
{
    return m_drawable->RegisterSinglePlanetBodyPassSlot( p_pass, p_binder, p_orientation, p_meshe_type, p_layer_index );
}
*/

DrawSpace::Core::RenderingNode* Root::RegisterSinglePassSlot( Pass* p_pass, SphericalLOD::Binder* p_binder, int p_orientation, 
                                    DrawSpace::SphericalLOD::Body::MesheType p_meshe_type, int p_layer_index, int p_rendering_order )
{
    return m_drawable->RegisterSinglePassSlot( p_pass, p_binder, p_orientation, p_meshe_type, p_layer_index, p_rendering_order );
}


void Root::SetFinalTransform( const DrawSpace::Utils::Matrix& p_mat )
{
    Orbiter::SetFinalTransform( p_mat );
    m_drawable->SetFinalTransform( p_mat );
}

void Root::OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node )
{
    Orbiter::OnRegister( p_scenegraph, p_node );
    m_drawable->OnRegister( p_scenegraph, p_node );
}

void Root::OnUnregister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node )
{
	Orbiter::OnUnregister( p_scenegraph, p_node );
	m_drawable->OnUnregister( p_scenegraph, p_node );
}

void Root::Update( DrawSpace::Utils::TimeManager& p_timemanager )
{
    Orbiter::Update( p_timemanager );
}

void Root::Update2( DrawSpace::Utils::TimeManager& p_timemanager )
{
    Orbiter::Update2( p_timemanager );
}

void Root::RegisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph )
{
    // enregistrer le cb evt nodes imperativement AVANT le cb evt cameras
    p_scenegraph.RegisterNodesEvtHandler( m_nodes_evt_cb );
    p_scenegraph.RegisterCameraEvtHandler( m_camera_evt_cb );
    p_scenegraph.RegisterScenegraphEvtHandler( m_scenegraph_evt_cb );
}

void Root::UnregisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph )
{
    p_scenegraph.UnregisterNodesEvtHandler( m_nodes_evt_cb );
    p_scenegraph.UnregisterCameraEvtHandler( m_camera_evt_cb );
    p_scenegraph.UnregisterScenegraphEvtHandler( m_scenegraph_evt_cb );
}

DrawSpace::SphericalLOD::Layer* Root::GetLayerFromInertBody( DrawSpace::Dynamics::InertBody* p_body, int p_layer_index )
{
    if( m_registered_bodies.count( p_body ) )
    {
        return m_registered_bodies[p_body].layers[p_layer_index];
    }
    return NULL;
}

DrawSpace::SphericalLOD::Layer* Root::GetLayerFromCamera( const dsstring& p_cameraName, int p_layer_index )
{
    if( m_registered_camerapoints.count( p_cameraName ) )
    {    
        return m_registered_camerapoints[p_cameraName].layers[p_layer_index];
    }
    return NULL;
}

bool Root::GetInertBodyRelativeAltitude( DrawSpace::Dynamics::InertBody* p_body, dsreal& p_rel_altitude )
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

bool Root::GetCameraRelativeAltitude( const dsstring& p_cameraName, dsreal& p_rel_altitude )
{
    if( m_registered_camerapoints.count( p_cameraName ) > 0 )
    {
        RegisteredCamera entry = m_registered_camerapoints[p_cameraName];
        if( entry.relative_alt_valid )
        {
            p_rel_altitude = entry.relative_alt;
            return true;
        }
        return false;
    }
    return false;
}

void Root::ResetRegisteredBodyLayer( DrawSpace::Dynamics::InertBody* p_body, int p_layer_index )
{
    if( m_registered_bodies.count( p_body ) > 0 )
    {
        RegisteredBody entry = m_registered_bodies[p_body];        
        entry.layers[p_layer_index]->ResetBody();
    }
}

void Root::DrawSubPasses( void )
{
    if( m_singleshot_subpasses_stack.size() > 0 )
    {
        DrawSpace::SphericalLOD::SubPass* sp = m_singleshot_subpasses_stack.back();
        
        if( sp->GetTimerReadyFlag() )
        {
            m_singleshot_subpasses_stack.pop_back();
            sp->DrawSubPass();
            sp->SubPassDone();
        }
    }

    for( size_t i = 0; i < m_singleshot_subpasses.size(); i++ )
    {
        SphericalLOD::SubPass* sp = m_singleshot_subpasses[i];        
        sp->DrawSubPass();
        sp->SubPassDone();
    }
    m_singleshot_subpasses.clear();

    for( size_t i = 0; i < m_permanent_subpasses.size(); i++ )
    {
        SphericalLOD::SubPass* sp = m_permanent_subpasses[i];        
        sp->DrawSubPass();
        sp->SubPassDone();
    }
}

DrawSpace::SphericalLOD::SubPass::EntryInfos Root::on_subpasscreation( DrawSpace::SphericalLOD::SubPass* p_pass, int p_dest )
{
    // cb traitement d'une requete de creation d'une sub-pass
 
    SphericalLOD::FaceDrawingNode* node = static_cast<DrawSpace::SphericalLOD::FaceDrawingNode*>( p_pass->GetNode() );
    

    //m_drawable->SetSinglePassSlot( p_pass->GetPass(), node );

    node->RegisterHandler( m_drawable->GetSingleNodeDrawHandler() );

    // ces nodes ne sont pas destines a dependre d'un scenegraph
    // donc on ajoute le node a la queue directement ici
    p_pass->GetPass()->GetRenderingQueue()->Add( node );


    p_pass->GetPass()->GetRenderingQueue()->UpdateOutputQueueNoOpt();

   
    DrawSpace::SphericalLOD::SubPass::EntryInfos ei;

    ei.singleshot_subpasses_stack = &m_singleshot_subpasses_stack;
    ei.singleshot_subpasses = &m_singleshot_subpasses;
    ei.permanent_subpasses = &m_permanent_subpasses;

    switch( p_dest )
    {
        case 0:

            m_singleshot_subpasses_stack.push_front( p_pass );
            ei.singleshot_subpasses_stack_position = m_singleshot_subpasses_stack.begin();
            break;

        case 1:

            m_singleshot_subpasses.push_back( p_pass );
            ei.singleshot_subpasses_position = m_singleshot_subpasses.end() - 1;
            break;

        case 2:
            m_permanent_subpasses.push_back( p_pass );
            ei.permanent_subpasses_position = m_permanent_subpasses.end() - 1;
            break;

        default:

            _DSEXCEPTION( "unknow destination for subpass" );
            break;
    }

    ei.queue_id = p_dest;
    

    return ei;    
}

int Root::GetSingleShotSubPassesStackSize()
{
    return m_singleshot_subpasses_stack.size();
}

void Root::SetGravityState( bool p_state )
{
    m_enable_gravity = p_state;
}
