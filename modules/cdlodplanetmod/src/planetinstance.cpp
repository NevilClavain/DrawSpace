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

#include "planetinstance.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

PlanetInstance::PlanetInstance( void )
{
}

PlanetInstance::~PlanetInstance( void )
{
}

void PlanetInstance::Init( PlanetSceneNodeConfig* p_planet_config, DrawSpace::Interface::Renderer* p_renderer,
                DrawSpace::Utils::TimeManager* p_tm, DrawSpace::IntermediatePass* p_ipass,
                DrawSpace::Dynamics::Calendar* p_calendar,  DrawSpace::Core::SceneNodeGraph* p_scenenodegraph,
                LODDependantNodeInfoStateHandler* p_LODDepNodeInfoStateHandler )
{
    m_node_config = p_planet_config;
    m_scenenodegraph = p_scenenodegraph;

	for (int i = 0; i < 6; i++)
	{
		m_planet_details_binder[i] = _DRAWSPACE_NEW_( PlanetDetailsBinder, PlanetDetailsBinder( m_node_config->m_planetRay.m_value * 1000.0, PLANET_ATMO_THICKNESS, 
                                                                                                m_node_config->m_plainsAmplitude.m_value, m_node_config->m_moutainsAmplitude.m_value, 
                                                                                                m_node_config->m_verticalOffset.m_value, m_node_config->m_moutainsOffset.m_value,
                                                                                                m_node_config->m_landscapeBumpFactor.m_value ) );

		m_planet_climate_binder[i] = _DRAWSPACE_NEW_( PlanetClimateBinder, PlanetClimateBinder( m_node_config->m_plainsAmplitude.m_value, m_node_config->m_moutainsAmplitude.m_value, 
                                                                                                m_node_config->m_verticalOffset.m_value, m_node_config->m_moutainsOffset.m_value,
                                                                                                m_node_config->m_beachLimit.m_value ) );
    }

	m_planet_vshader = _DRAWSPACE_NEW_( Shader, Shader( m_node_config->m_detailsVertexShader.m_value, true ) );
	m_planet_pshader = _DRAWSPACE_NEW_( Shader, Shader( m_node_config->m_detailsPixelShader.m_value, true ) );

	m_texture_th_pixels = _DRAWSPACE_NEW_( Texture, Texture( "earth_th_pixels_16.jpg" ) );
	m_texture_th_splatting = _DRAWSPACE_NEW_( Texture, Texture( "earth_th_splatting_16.jpg" ) );

	// chargement des medias; TODO : verifier le status retour de chaque LoadFromFile();
	m_texture_th_pixels->LoadFromFile();
	m_texture_th_splatting->LoadFromFile();
	m_planet_vshader->LoadFromFile();
	m_planet_pshader->LoadFromFile();


	m_details_fx = _DRAWSPACE_NEW_( Fx, Fx );

    m_details_fx->AddShader( m_planet_vshader );
    m_details_fx->AddShader( m_planet_pshader );

    m_details_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_details_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_details_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETVERTEXTEXTUREFILTERTYPE, "linear" ) );
    //m_details_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "line" ) );

    m_details_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_details_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    m_details_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETVERTEXTEXTUREFILTERTYPE, "none" ) );
    //m_details_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "solid" ) );

    m_details_fx->SetRenderStateUniqueQueueID( m_node_config->m_planetName.m_value ); // parce qu'on va updater le renderstate ENABLEZBUFFER pendant le rendu

    for( int i = 0; i < 6; i++ )
    {
		m_planet_details_binder[i]->SetFx( m_details_fx );

		m_planet_details_binder[i]->SetTexture( m_texture_th_pixels, 0 );
		m_planet_details_binder[i]->SetTexture( m_texture_th_splatting, 1 );
    }

	m_climate_vshader = _DRAWSPACE_NEW_( Shader, Shader( "planet_ht.vso", true ) );
	m_climate_pshader = _DRAWSPACE_NEW_( Shader, Shader( "planet_ht.pso", true ) );

    m_climate_vshader->LoadFromFile();
	m_climate_pshader->LoadFromFile();

    m_climate_fx = _DRAWSPACE_NEW_( Fx, Fx );
    m_climate_fx->AddShader( m_climate_vshader );
    m_climate_fx->AddShader( m_climate_pshader );

	m_climate_fx->AddRenderStateIn(DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETVERTEXTEXTUREFILTERTYPE, "linear" ) );
    //pe.m_climate_fx->AddRenderStateIn(DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_climate_fx->AddRenderStateIn(DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );

	m_climate_fx->AddRenderStateOut(DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETVERTEXTEXTUREFILTERTYPE, "none" ) );
    //pe.m_climate_fx->AddRenderStateOut(DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_climate_fx->AddRenderStateOut(DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );

	for( int i = 0; i < 6; i++ )
	{
		m_planet_climate_binder[i]->SetFx( m_climate_fx );
	}

    for( int i = 0; i < 6; i++ )
    {
		m_planet_details_binder[i]->SetRenderer( p_renderer );
		m_planet_climate_binder[i]->SetRenderer( p_renderer );
    }

    m_config.m_lod0base = 19000.0;
    m_config.m_ground_layer = 0;
    m_config.m_nbLODRanges_inertBodies = NB_LOD_INERTBODIES;
    m_config.m_nbLODRanges_freeCameras = NB_LOD_FREECAMERAS;

    SphericalLOD::Config::LayerDescriptor planet_surface;
    planet_surface.enable_collisions = false;
    planet_surface.enable_datatextures = true;
    planet_surface.enable_lod = true;
    planet_surface.min_lodlevel = 0;
	planet_surface.ray = m_node_config->m_planetRay.m_value;
    for( int i = 0; i < 6; i++ )
    {
        planet_surface.groundCollisionsBinder[i] = NULL;
		planet_surface.patchTexturesBinder[i] = m_planet_climate_binder[i];
    }

    m_config.m_layers_descr.push_back( planet_surface );

	m_planet_root = _DRAWSPACE_NEW_( DrawSpace::SphericalLOD::Root, DrawSpace::SphericalLOD::Root( m_node_config->m_planetName.m_value, m_node_config->m_planetRay.m_value, p_tm, m_config ) );

    for( int i = 0; i < 6; i++ )
    {
		m_planet_root->RegisterSinglePassSlot( p_ipass, m_planet_details_binder[i], i, DrawSpace::SphericalLOD::Body::LOWRES_SKIRT_MESHE, 0, 2000 );
    }

    m_LODDependantNodeInfosHandlers.push_back( p_LODDepNodeInfoStateHandler );

    // temp
    m_planet_root->SetOrbitDuration( 0.333 );
    m_planet_root->SetRevolutionTiltAngle( 0.0 );    
    m_planet_root->SetRevolutionDuration( 1.0 );

    m_planet_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::SphericalLOD::Root>, SceneNode<DrawSpace::SphericalLOD::Root>( m_node_config->m_planetName.m_value ) );
    m_planet_node->SetContent( m_planet_root );

	for (int i = 0; i < 6; i++)
	{
		m_planet_details_binder[i]->SetPlanetNode( m_planet_node );
	}

	p_calendar->RegisterWorld( m_planet_root->GetWorld() );

	m_planet_root->SetGravityState( m_node_config->m_gravityEnabled.m_value );

    m_calendar = p_calendar;
}

void PlanetInstance::Run( void )
{
    zbuffer_control_from_viewer_alt();
    m_planet_root->DrawSubPasses();
    update_details_binders();
    update_lod_dep_nodes_infos();
}

void PlanetInstance::Release( void )
{
    m_calendar->UnregisterWorld( m_planet_root->GetWorld() );

	_DRAWSPACE_DELETE_( m_planet_node );
	_DRAWSPACE_DELETE_( m_planet_root );

	_DRAWSPACE_DELETE_( m_climate_fx );

	_DRAWSPACE_DELETE_( m_climate_vshader );
	_DRAWSPACE_DELETE_( m_climate_pshader );
    
	_DRAWSPACE_DELETE_( m_details_fx );

    _DRAWSPACE_DELETE_( m_texture_th_pixels );
	_DRAWSPACE_DELETE_( m_texture_th_splatting );

	_DRAWSPACE_DELETE_( m_planet_vshader );
	_DRAWSPACE_DELETE_( m_planet_pshader );

	for (int i = 0; i < 6; i++)
	{
		_DRAWSPACE_DELETE_( m_planet_details_binder[i] );
		_DRAWSPACE_DELETE_( m_planet_climate_binder[i] );
	}
}

DrawSpace::Core::SceneNode<DrawSpace::SphericalLOD::Root>* PlanetInstance::GetSceneNode( void )
{
    return m_planet_node;
}

void PlanetInstance::RegisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph )
{
    m_planet_root->RegisterScenegraphCallbacks( p_scenegraph );
}

void PlanetInstance::UnregisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph )
{
    m_planet_root->UnregisterScenegraphCallbacks( p_scenegraph );
}

void PlanetInstance::OnGravityEnabledUpdate( bool p_value )
{
	m_planet_root->SetGravityState( p_value );
}

void PlanetInstance::AddLODDependantNodeInfosKeyLinkTable( const dsstring& p_dependantNodeId, DrawSpace::Module::KeysLinkTable* p_keytable )
{
    if( m_LODdependant_nodeinfos.count( p_dependantNodeId ) )
    {
        m_LODdependant_nodeinfos[p_dependantNodeId].RegisterKeysLinkTable( p_keytable );
    }
    else
    {
        _DSEXCEPTION( "Unknown LODdependant node : " + p_dependantNodeId );
    }
}

void PlanetInstance::zbuffer_control_from_viewer_alt( void )
{
    dsstring camera_name;
    m_scenenodegraph->GetCurrentCameraName( camera_name );

    dsreal view_rel_alt = m_planet_root->GetAnyCameraRelativeAltitude( camera_name );
    if( view_rel_alt < ZBUFFER_ACTIVATION_REL_ALT )
    {
        m_details_fx->UpdateRenderStateIn( 0, DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    }
    else
    {
        m_details_fx->UpdateRenderStateIn( 0, DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    }
}

void PlanetInstance::update_details_binders( void )
{
	for( int i = 0; i < 6; i++ )
	{
		m_planet_details_binder[i]->Update();
	}
}

void PlanetInstance::update_lod_dep_nodes_infos( void )
{
    std::vector<DrawSpace::SphericalLOD::Root::RegisteredBody> bodies_infos;
    m_planet_root->GetRegisteredBodyInfosList( bodies_infos );

    int nbsubpasses = m_planet_root->GetNbSingleShotSubpasses();
    nbsubpasses += m_planet_root->GetSingleShotSubPassesStackSize();

    for( size_t i = 0; i < bodies_infos.size(); i++ )
    {
        dsstring node_sceneid = bodies_infos[i].scenename;

        bool notify_new_entry = false;
        if( !m_LODdependant_nodeinfos.count( node_sceneid ) )
        {
            notify_new_entry = true;
        }

        m_LODdependant_nodeinfos[node_sceneid].m_isNodeHot = bodies_infos[i].attached;
        m_LODdependant_nodeinfos[node_sceneid].m_isCamera = false;
        m_LODdependant_nodeinfos[node_sceneid].m_cameraType = -1;

        m_LODdependant_nodeinfos[node_sceneid].m_nodeRelativeAltitudeValid = bodies_infos[i].relative_alt_valid;
        m_LODdependant_nodeinfos[node_sceneid].m_nodeRelativeAltitude = bodies_infos[i].relative_alt;
        m_LODdependant_nodeinfos[node_sceneid].m_nodeAltitude = bodies_infos[i].layers[0]->GetBody()->GetHotPointAltitud();
        m_LODdependant_nodeinfos[node_sceneid].m_groundAlt = bodies_infos[i].layers[0]->GetCurrentHeight();
        m_LODdependant_nodeinfos[node_sceneid].m_groundAltValid = bodies_infos[i].layers[0]->HasCollisions();

        m_LODdependant_nodeinfos[node_sceneid].m_nbSubPasses = nbsubpasses;

        if( notify_new_entry )
        {
            for( size_t i = 0; i < m_LODDependantNodeInfosHandlers.size(); i++ )
            {
                (* m_LODDependantNodeInfosHandlers[i] )( node_sceneid, true );
            }        
        }
    }

    std::vector<DrawSpace::SphericalLOD::Root::RegisteredCamera> cameras_infos;
    m_planet_root->GetRegisteredCameraInfosList( cameras_infos );
    
    for( size_t i = 0; i < cameras_infos.size(); i++ )
    {
        dsstring node_sceneid = cameras_infos[i].scenename;

        bool notify_new_entry = false;

        if( !m_LODdependant_nodeinfos.count( node_sceneid ) )
        {
            notify_new_entry = true;
        }

        m_LODdependant_nodeinfos[node_sceneid].m_isCamera = true;
        m_LODdependant_nodeinfos[node_sceneid].m_cameraType = (int)cameras_infos[i].type;

        switch( cameras_infos[i].type )
        {
            case SphericalLOD::Root::FREE:

                m_LODdependant_nodeinfos[node_sceneid].m_isNodeHot = false;
                break;

            case SphericalLOD::Root::FREE_ON_PLANET:

                m_LODdependant_nodeinfos[node_sceneid].m_isNodeHot = true;
                break;

            case SphericalLOD::Root::INERTBODY_LINKED:
                {
                    DrawSpace::Dynamics::InertBody* attached_body = cameras_infos[i].attached_body;

                    for( size_t j = 0; j < bodies_infos.size(); j++ )
                    {
                        if( bodies_infos[j].body == attached_body )
                        {
                            m_LODdependant_nodeinfos[node_sceneid].m_isNodeHot = bodies_infos[j].attached;
                            break;
                        }
                    }
                }
                break;
        }

        m_LODdependant_nodeinfos[node_sceneid].m_nodeRelativeAltitudeValid = cameras_infos[i].relative_alt_valid;
        m_LODdependant_nodeinfos[node_sceneid].m_nodeRelativeAltitude = cameras_infos[i].relative_alt;
        m_LODdependant_nodeinfos[node_sceneid].m_nodeAltitude = cameras_infos[i].layers[0]->GetBody()->GetHotPointAltitud();
        m_LODdependant_nodeinfos[node_sceneid].m_groundAlt = cameras_infos[i].layers[0]->GetCurrentHeight();
        m_LODdependant_nodeinfos[node_sceneid].m_groundAltValid = cameras_infos[i].layers[0]->HasCollisions();

        if( notify_new_entry )
        {
            for( size_t i = 0; i < m_LODDependantNodeInfosHandlers.size(); i++ )
            {
                (* m_LODDependantNodeInfosHandlers[i] )( node_sceneid, true );
            }
        }
    }
}