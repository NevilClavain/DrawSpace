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


DrawSpace::Planetoid::Body::Body( const dsstring& p_scenename, dsreal p_ray, DrawSpace::Utils::TimeManager* p_time, const SphericalLOD::Config& p_config ) : Orbiter( &m_world ),
m_scenename( p_scenename ),
m_ray( p_ray * 1000.0 ),
m_timemanager( p_time )
{
    m_world.Initialize();

    m_config = _DRAWSPACE_NEW_( SphericalLOD::Config, SphericalLOD::Config );
    *m_config = p_config;
       
    m_drawable = _DRAWSPACE_NEW_( SphericalLOD::Drawing, SphericalLOD::Drawing( m_config ) );
    m_drawable->SetRenderer( SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface );
    
    m_camera_evt_cb = _DRAWSPACE_NEW_( CameraEvtCb, CameraEvtCb( this, &DrawSpace::Planetoid::Body::on_camera_event ) );
    m_nodes_evt_cb = _DRAWSPACE_NEW_( NodesEventCb, NodesEventCb( this, &DrawSpace::Planetoid::Body::on_nodes_event ) );
    m_scenegraph_evt_cb = _DRAWSPACE_NEW_( ScenegraphEventCb, ScenegraphEventCb( this, &DrawSpace::Planetoid::Body::on_scenegraph_event ) );

    m_patchsdraw_request_cb = _DRAWSPACE_NEW_( PatchsDrawRequestCb, PatchsDrawRequestCb( this, &DrawSpace::Planetoid::Body::on_patchsdraw_request ) );

    m_fractal = new Fractal( 3, 3345764, m_config->m_fbmRoughness, m_config->m_fbmLacunarity );

    for( long i = 0; i < 6; i++ )
    {
        m_faces[i] = _DRAWSPACE_NEW_( DrawSpace::SphericalLOD::Face, DrawSpace::SphericalLOD::Face( m_ray * 2.0, m_config ) );
        m_faces[i]->Init( i );
    }
    
    m_colortextures_subpasses[0] = create_colortexture();

    m_subpasses[m_colortextures_subpasses[0]].need_redraw = true;

    std::vector<DrawSpace::SphericalLOD::Patch*> dl;
    dl.push_back( m_faces[0]->GetRootPatch() );

    m_subpasses[m_colortextures_subpasses[0]].renderingpatches_node->SetDisplayList( dl );
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

DrawSpace::IntermediatePass* DrawSpace::Planetoid::Body::create_colliding_heightmap_pass( const dsstring& p_inertbody_scenename )
{
    dsstring complete_name = m_scenename + dsstring( "_" ) + p_inertbody_scenename + dsstring( "_collisionheightmap_pass" );
    IntermediatePass* ipass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( complete_name ) );

    ipass->SetTargetDimsFromRenderer( false );    
    ipass->SetTargetDims( PATCH_RESOLUTION, PATCH_RESOLUTION );
    ipass->SetRenderPurpose( Texture::RENDERPURPOSE_FLOAT32 );

    ipass->Initialize();
    ipass->GetRenderingQueue()->EnableDepthClearing( true );
    ipass->GetRenderingQueue()->EnableTargetClearing( true );
    ipass->GetRenderingQueue()->SetTargetClearingColor( 0, 0, 0, 255 );

    return ipass;
}

DrawSpace::IntermediatePass* DrawSpace::Planetoid::Body::create_color_texture_pass( void )
{
    dsstring complete_name = m_scenename + dsstring( "_colortexture_pass" );
    IntermediatePass* ipass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( complete_name ) );

    ipass->SetTargetDimsFromRenderer( false );    
    ipass->SetTargetDims( 256, 256 );
    
    ipass->Initialize();
    ipass->GetRenderingQueue()->EnableDepthClearing( true );
    ipass->GetRenderingQueue()->EnableTargetClearing( true );
    ipass->GetRenderingQueue()->SetTargetClearingColor( 0, 0, 0, 255 );

    return ipass;
}

void DrawSpace::Planetoid::Body::create_colliding_heightmap( const dsstring& p_inertbody_scenename, DrawSpace::IntermediatePass** p_pass, DrawSpace::SphericalLOD::FaceDrawingNode** p_renderingnode )
{
    SphericalLOD::FaceDrawingNode* node;

    IntermediatePass* ipass = create_colliding_heightmap_pass( p_inertbody_scenename );

    RegisterSinglePassSlot( ipass );
    node = static_cast<SphericalLOD::FaceDrawingNode*>( GetSingleNodeFromPass( ipass ) );

    Shader* patch_vshader = _DRAWSPACE_NEW_( Shader, Shader( "planethm.vsh", false ) );
    Shader* patch_pshader = _DRAWSPACE_NEW_( Shader, Shader( "planethm.psh", false ) );
    patch_vshader->LoadFromFile();
    patch_pshader->LoadFromFile();

    Fx* patch_fx = CreateSingleNodeFx( ipass );

    patch_fx->AddShader( patch_vshader );
    patch_fx->AddShader( patch_pshader );

    ipass->GetRenderingQueue()->UpdateOutputQueue();

    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    void* tx_data;
    if( false == renderer->CreateTexture( ipass->GetTargetTexture(), &tx_data ) )
    {
        _DSEXCEPTION( "failed to create colliding heightmap texture in renderer" );
    }

    ipass->GetTargetTexture()->AllocTextureContent();
    
    SubPass sp;
    sp.need_redraw = false;
    sp.pass = ipass;
    sp.renderingpatches_node = node;
    m_subpasses.push_back( sp );

    *p_pass = ipass;
    *p_renderingnode = node;    
}

int DrawSpace::Planetoid::Body::create_colortexture( void )
{
    SphericalLOD::FaceDrawingNode* node;
    IntermediatePass* ipass = create_color_texture_pass();

    RegisterSinglePassSlot( ipass );
    node = static_cast<SphericalLOD::FaceDrawingNode*>( GetSingleNodeFromPass( ipass ) );

    Shader* patch_vshader = _DRAWSPACE_NEW_( Shader, Shader( "planetcolors.vsh", false ) );
    Shader* patch_pshader = _DRAWSPACE_NEW_( Shader, Shader( "planetcolors.psh", false ) );
    patch_vshader->LoadFromFile();
    patch_pshader->LoadFromFile();

    Fx* patch_fx = CreateSingleNodeFx( ipass );

    patch_fx->AddShader( patch_vshader );
    patch_fx->AddShader( patch_pshader );

    //patch_fx->AddRenderStateIn(  DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "line" ) );
    //patch_fx->AddRenderStateOut(  DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "solid" ) );

    ipass->GetRenderingQueue()->UpdateOutputQueue();

    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    void* tx_data;
    if( false == renderer->CreateTexture( ipass->GetTargetTexture(), &tx_data ) )
    {
        _DSEXCEPTION( "failed to create colliding heightmap texture in renderer" );
    }

    ipass->GetTargetTexture()->AllocTextureContent();

    int index = m_subpasses.size();

    SubPass sp;
    sp.need_redraw = false;
    sp.pass = ipass;
    sp.renderingpatches_node = node;
    m_subpasses.push_back( sp );
   
    return index;
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
                        dsstring bodyname;
                        p_node->GetSceneName( bodyname );

                        reg_body.attached = true;
                        reg_body.body = inertbody;                        
                        reg_body.relative_alt_valid = false;
                        
                        create_colliding_heightmap( bodyname, &reg_body.collidingheightmap_pass, &reg_body.collidingheightmap_node );
                                                                                        
                        inertbody->IncludeTo( this );

                        DrawSpace::SphericalLOD::Body* slod_body = _DRAWSPACE_NEW_( DrawSpace::SphericalLOD::Body, 
                            DrawSpace::SphericalLOD::Body( m_ray * 2.0, m_timemanager, m_config ) );
                        Collider* collider = _DRAWSPACE_NEW_( Collider, Collider );
                        
                        Fragment* planet_fragment = _DRAWSPACE_NEW_( Fragment, Fragment( m_config, &m_world, slod_body, collider, m_ray, true ) );
                        planet_fragment->SetHotState( true );
                        planet_fragment->RegisterPatchsDrawRequestHandler( m_patchsdraw_request_cb );
                        planet_fragment->SetCollidingHMSubPassIndex( m_subpasses.size() - 1 );
                        planet_fragment->SetCollidingHeightMapTexture( reg_body.collidingheightmap_pass->GetTargetTexture() );
                        m_subpassdone_handlers.push_back( planet_fragment->GetSubPassDoneCb() );

                        m_planetfragments_list.push_back( planet_fragment );
                        reg_body.fragment = planet_fragment;

                        planet_fragment->SetInertBody( inertbody );
                                              
                        m_registered_bodies[inertbody] = reg_body;
                    }
                }
                else
                {
                    dsstring bodyname;
                    p_node->GetSceneName( bodyname );

                    reg_body.attached = false;
                    reg_body.body = inertbody;
                    reg_body.relative_alt_valid = false;

                    create_colliding_heightmap( bodyname, &reg_body.collidingheightmap_pass, &reg_body.collidingheightmap_node );

                    DrawSpace::SphericalLOD::Body* slod_body = _DRAWSPACE_NEW_( DrawSpace::SphericalLOD::Body, 
                        DrawSpace::SphericalLOD::Body( m_ray * 2.0, m_timemanager, m_config ) );

                    Collider* collider = _DRAWSPACE_NEW_( Collider, Collider );
                   
                    Fragment* planet_fragment = _DRAWSPACE_NEW_( Fragment, Fragment( m_config, &m_world, slod_body, collider, m_ray, true ) );
                    planet_fragment->SetHotState( false );
                    planet_fragment->RegisterPatchsDrawRequestHandler( m_patchsdraw_request_cb );
                    planet_fragment->SetCollidingHMSubPassIndex( m_subpasses.size() - 1 );
                    planet_fragment->SetCollidingHeightMapTexture( reg_body.collidingheightmap_pass->GetTargetTexture() );
                    m_subpassdone_handlers.push_back( planet_fragment->GetSubPassDoneCb() );

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
        manage_bodies();
        manage_camerapoints();
        update_fragments();
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
                bodyfragment->RemoveColliderFromWorld();
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

        curr->Update( this );

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
    DrawSpace::SphericalLOD::Body* slod_body = _DRAWSPACE_NEW_( DrawSpace::SphericalLOD::Body, 
        DrawSpace::SphericalLOD::Body( m_ray * 2.0, m_timemanager, m_config ) );
    Collider* collider = _DRAWSPACE_NEW_( Collider, Collider );
    
    Fragment* planet_fragment = _DRAWSPACE_NEW_( Fragment, Fragment( m_config, &m_world, slod_body, collider, m_ray, false ) );
   
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

void DrawSpace::Planetoid::Body::RegisterPlanetBodyPassSlot( Pass* p_pass )
{
    m_drawable->RegisterPlanetBodyPassSlot( p_pass );
}

void DrawSpace::Planetoid::Body::RegisterSinglePassSlot( Pass* p_pass )
{
    m_drawable->RegisterSinglePassSlot( p_pass );
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

void DrawSpace::Planetoid::Body::BindPlanetBodyExternalGlobalTexture( DrawSpace::Core::Texture* p_texture, DrawSpace::Pass* p_pass, int p_faceid )
{
    m_drawable->GetPlanetBodyNodeFromPass( p_pass, p_faceid )->SetTexture( p_texture, 0 );
}

DrawSpace::Core::Fx* DrawSpace::Planetoid::Body::CreatePlanetBodyFx( DrawSpace::Pass* p_pass, int p_faceid )
{
    Fx* fx = _DRAWSPACE_NEW_( Fx, Fx );
    m_drawable->GetPlanetBodyNodeFromPass( p_pass, p_faceid )->SetFx( fx );
    return fx;
}

DrawSpace::Core::Fx* DrawSpace::Planetoid::Body::CreateSingleNodeFx( DrawSpace::Pass* p_pass )
{
    Fx* fx = _DRAWSPACE_NEW_( Fx, Fx );
    m_drawable->GetSingleNodeFromPass( p_pass )->SetFx( fx );
    return fx;
}



DrawSpace::Planetoid::Fragment* DrawSpace::Planetoid::Body::GetFragment( int p_index )
{
    return m_planetfragments_list[p_index];
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

void DrawSpace::Planetoid::Body::InitNoisingTextures( void )
{
    m_drawable->InitNoisingTextures( m_fractal );
}

DrawSpace::Core::RenderingNode* DrawSpace::Planetoid::Body::GetPlanetBodyNodeFromPass( Pass* p_pass, int p_faceid )
{
    return m_drawable->GetPlanetBodyNodeFromPass( p_pass, p_faceid );
}

DrawSpace::Core::RenderingNode* DrawSpace::Planetoid::Body::GetSingleNodeFromPass( Pass* p_pass )
{
    return m_drawable->GetSingleNodeFromPass( p_pass );
}

void DrawSpace::Planetoid::Body::DrawSubPasses( void )
{  
    for( size_t i = 0; i < m_subpasses.size(); i++ )
    {
        if( m_subpasses[i].need_redraw )
        {
            m_subpasses[i].pass->GetRenderingQueue()->Draw();
            m_subpasses[i].need_redraw = false;

            for( size_t j = 0; j < m_subpassdone_handlers.size(); j++ )
            {
                (*m_subpassdone_handlers[j])( i );
            }
        }
    }
}

void DrawSpace::Planetoid::Body::on_patchsdraw_request( const std::vector<DrawSpace::SphericalLOD::Patch*>& p_displaylist, int p_subpassindex )
{    
    m_subpasses[p_subpassindex].need_redraw = true;
    m_subpasses[p_subpassindex].renderingpatches_node->SetDisplayList( p_displaylist );
}

DrawSpace::Core::Texture* DrawSpace::Planetoid::Body::GetColorTexture( int p_index )
{
    return m_subpasses[m_colortextures_subpasses[p_index]].pass->GetTargetTexture();
}

void DrawSpace::Planetoid::Body::BindGlobalTexture( DrawSpace::Pass* p_pass )
{
    RenderingNode* node = m_drawable->GetPlanetBodyNodeFromPass( p_pass, 0 );
    node->SetTexture( m_subpasses[m_colortextures_subpasses[0]].pass->GetTargetTexture(), 0 );
}