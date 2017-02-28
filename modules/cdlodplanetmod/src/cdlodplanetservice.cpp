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

#include "cdlodplanetservice.h"
#include "file.h"
#include "exceptions.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface::Module;

_DECLARE_DS_LOGGER( logger, "cdlodplanetservice", NULL )

/////////////////////////////

SceneNodeGraphParam::SceneNodeGraphParam( const dsstring& p_id, CDLODPlanetService* p_owner ) : KeySink( p_id ), m_owner( p_owner )
{
}

void SceneNodeGraphParam::OnUpdated( DrawSpace::Core::SceneNodeGraph* p_val )
{
    m_owner->OnSceneNodeGraphUpdated( p_val );
}

/////////////////////////////

TexturePassParam::TexturePassParam( const dsstring& p_id, CDLODPlanetService* p_owner ) : KeySink( p_id ), m_owner( p_owner )
{
}

void TexturePassParam::OnUpdated( DrawSpace::IntermediatePass* p_val )
{
    m_owner->OnTexturePassUpdate( p_val );
}

/////////////////////////////

CDLODPlanetService::CDLODPlanetService( const dsstring& p_id )
{
    m_scparam = _DRAWSPACE_NEW_( SceneNodeGraphParam, SceneNodeGraphParam( p_id + dsstring( ".SceneNodeGraph" ), this ) );
    m_texturepassparam = _DRAWSPACE_NEW_( TexturePassParam, TexturePassParam( p_id + dsstring( ".TexturePass" ), this ) );
}

CDLODPlanetService::~CDLODPlanetService( void )
{
}

void CDLODPlanetService::GetKeys( std::vector<DrawSpace::Module::KeySinkBase*>& p_keys )
{
    p_keys.push_back( m_scparam );
    p_keys.push_back( m_texturepassparam );
}

void CDLODPlanetService::Init( DrawSpace::Logger::Configuration* p_logconf, 
                            DrawSpace::Core::BaseCallback<void, bool>* p_mousecircularmode_cb, 
                            DrawSpace::Core::BaseCallback<void, bool>* p_mousevisible_cb, 
                            DrawSpace::Core::BaseCallback<void, int>* p_closeapp_cb )
{
    p_logconf->RegisterSink( &logger );
    logger.SetConfiguration( p_logconf );

    p_logconf->RegisterSink( MemAlloc::GetLogSink() );
    MemAlloc::GetLogSink()->SetConfiguration( p_logconf );

    _DSDEBUG( logger, dsstring("CDLODPlanet service : startup...") );

    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    DrawSpace::SphericalLOD::Body::BuildMeshes();
}

void CDLODPlanetService::OnSceneNodeGraphUpdated( DrawSpace::Core::SceneNodeGraph* p_val )
{
    m_scenenodegraph = p_val;
}

void CDLODPlanetService::OnTexturePassUpdate( DrawSpace::IntermediatePass* p_val )
{
    m_texturepass = p_val;
}




void CDLODPlanetService::Run( void )
{

    dsstring camera_name;
    m_scenenodegraph->GetCurrentCameraName( camera_name );

    for( auto it = m_nodes.begin(); it != m_nodes.end(); ++it )
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        dsreal view_rel_alt = it->second.m_planet_root->GetAnyCameraRelativeAltitude( camera_name );
        if( view_rel_alt < ZBUFFER_ACTIVATION_REL_ALT )
        {
            it->second.m_details_fx->UpdateRenderStateIn( 0, DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
        }
        else
        {
            it->second.m_details_fx->UpdateRenderStateIn( 0, DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        it->second.m_planet_root->DrawSubPasses();

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		for (int i = 0; i < 6; i++)
		{
			it->second.m_planet_details_binder[i]->Update();
		}

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    }

	m_tm.Update();
}

void CDLODPlanetService::Release( void )
{
    _DSDEBUG( logger, dsstring("CDLODPlanet service : shutdown...") );
}

DrawSpace::Core::BaseSceneNode* CDLODPlanetService::InstanciateSceneNode( const dsstring& p_sceneNodeName, DrawSpace::Dynamics::Calendar* p_calendar )
{
    PlanetInstance pe;

	if( m_nodes_config.count( p_sceneNodeName ) )
	{
		// une config de node est associée
		pe.m_node_config = &m_nodes_config[p_sceneNodeName];

		// pour permettre la mise à jour de parametres "hot"
		m_nodes_config[p_sceneNodeName].SetOwner( &pe );
	}
	else
	{
		_DSEXCEPTION( dsstring( "Node configuration is mandatory for CDLODPlanet node creation" ) );
	}

	for (int i = 0; i < 6; i++)
	{
		//pe.simplebinder[i] = _DRAWSPACE_NEW_( SimpleColorBinder, SimpleColorBinder );

		pe.m_planet_details_binder[i] = _DRAWSPACE_NEW_( PlanetDetailsBinder, PlanetDetailsBinder( pe.m_node_config->m_planetRay.m_value * 1000.0, PLANET_ATMO_THICKNESS ) );
		pe.m_planet_climate_binder[i] = _DRAWSPACE_NEW_( PlanetClimateBinder, PlanetClimateBinder );
    }

	pe.m_planet_vshader = _DRAWSPACE_NEW_( Shader, Shader( pe.m_node_config->m_detailsVertexShader.m_value, true ) );
	pe.m_planet_pshader = _DRAWSPACE_NEW_( Shader, Shader( pe.m_node_config->m_detailsPixelShader.m_value, true ) );

	pe.m_texture_th_pixels = _DRAWSPACE_NEW_( Texture, Texture( "earth_th_pixels_16.jpg" ) );
	pe.m_texture_th_splatting = _DRAWSPACE_NEW_( Texture, Texture( "earth_th_splatting_16.jpg" ) );

	// chargement des medias; TODO : verifier le status retour de chaque LoadFromFile();
	pe.m_texture_th_pixels->LoadFromFile();
	pe.m_texture_th_splatting->LoadFromFile();
	pe.m_planet_vshader->LoadFromFile();
	pe.m_planet_pshader->LoadFromFile();


	pe.m_details_fx = _DRAWSPACE_NEW_( Fx, Fx );

    pe.m_details_fx->AddShader( pe.m_planet_vshader );
    pe.m_details_fx->AddShader( pe.m_planet_pshader );

    pe.m_details_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    pe.m_details_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    pe.m_details_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETVERTEXTEXTUREFILTERTYPE, "linear" ) );
    //pe.m_details_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "line" ) );

    pe.m_details_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    pe.m_details_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    pe.m_details_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETVERTEXTEXTUREFILTERTYPE, "none" ) );
    //pe.m_details_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "solid" ) );


    pe.m_details_fx->SetRenderStateUniqueQueueID( p_sceneNodeName ); // parce qu'on va updater le renderstate ENABLEZBUFFER pendant le rendu

    for( int i = 0; i < 6; i++ )
    {
		pe.m_planet_details_binder[i]->SetFx( pe.m_details_fx );

		pe.m_planet_details_binder[i]->SetTexture( pe.m_texture_th_pixels, 0 );
		pe.m_planet_details_binder[i]->SetTexture( pe.m_texture_th_splatting, 1 );
    }

	pe.m_climate_vshader = _DRAWSPACE_NEW_( Shader, Shader( "planet_ht.vso", true ) );
	pe.m_climate_pshader = _DRAWSPACE_NEW_( Shader, Shader( "planet_ht.pso", true ) );

	pe.m_climate_vshader->LoadFromFile();
	pe.m_climate_pshader->LoadFromFile();

	pe.m_climate_fx = _DRAWSPACE_NEW_( Fx, Fx );
	pe.m_climate_fx->AddShader( pe.m_climate_vshader );
	pe.m_climate_fx->AddShader( pe.m_climate_pshader );
    
	pe.m_climate_fx->AddRenderStateIn(DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETVERTEXTEXTUREFILTERTYPE, "linear" ) );
    //pe.m_climate_fx->AddRenderStateIn(DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    pe.m_climate_fx->AddRenderStateIn(DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );

	pe.m_climate_fx->AddRenderStateOut(DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETVERTEXTEXTUREFILTERTYPE, "none" ) );
    //pe.m_climate_fx->AddRenderStateOut(DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    pe.m_climate_fx->AddRenderStateOut(DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );
    

	for (int i = 0; i < 6; i++)
	{
		pe.m_planet_climate_binder[i]->SetFx( pe.m_climate_fx );
	}

    for( int i = 0; i < 6; i++ )
    {
		pe.m_planet_details_binder[i]->SetRenderer( m_renderer );
		pe.m_planet_climate_binder[i]->SetRenderer( m_renderer );
    }


    pe.m_config.m_lod0base = 19000.0;
    pe.m_config.m_ground_layer = 0;
    pe.m_config.m_nbLODRanges_inertBodies = NB_LOD_INERTBODIES;
    pe.m_config.m_nbLODRanges_freeCameras = NB_LOD_FREECAMERAS;



    SphericalLOD::Config::LayerDescriptor planet_surface;
    planet_surface.enable_collisions = false;
    planet_surface.enable_datatextures = true;
    planet_surface.enable_lod = true;
    planet_surface.min_lodlevel = 0;
	planet_surface.ray = pe.m_node_config->m_planetRay.m_value; //PLANET_RAY;
    for( int i = 0; i < 6; i++ )
    {
        planet_surface.groundCollisionsBinder[i] = NULL;
		planet_surface.patchTexturesBinder[i] = pe.m_planet_climate_binder[i];
    }

    pe.m_config.m_layers_descr.push_back( planet_surface );


	pe.m_planet_root = _DRAWSPACE_NEW_( DrawSpace::SphericalLOD::Root, DrawSpace::SphericalLOD::Root( p_sceneNodeName, /*PLANET_RAY*/pe.m_node_config->m_planetRay.m_value, &m_tm, pe.m_config ) );

    for( int i = 0; i < 6; i++ )
    {
		pe.m_planet_root->RegisterSinglePassSlot( m_texturepass, pe.m_planet_details_binder[i], i, DrawSpace::SphericalLOD::Body::LOWRES_SKIRT_MESHE, 0, 2000 );
    }


    // temp
    pe.m_planet_root->SetOrbitDuration( 0.333 );
    pe.m_planet_root->SetRevolutionTiltAngle( 0.0 );    
    pe.m_planet_root->SetRevolutionDuration( 1.0 );
    

    pe.m_planet_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::SphericalLOD::Root>, SceneNode<DrawSpace::SphericalLOD::Root>( p_sceneNodeName ) );
    pe.m_planet_node->SetContent( pe.m_planet_root );

	for (int i = 0; i < 6; i++)
	{
		pe.m_planet_details_binder[i]->SetPlanetNode( pe.m_planet_node );
	}

	p_calendar->RegisterWorld( pe.m_planet_root->GetWorld() );

	pe.m_planet_root->SetGravityState( pe.m_node_config->m_gravityEnabled.m_value );

    m_nodes[p_sceneNodeName] = pe;
    return pe.m_planet_node;
}

void CDLODPlanetService::RegisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph )
{
    for( auto it = m_nodes.begin(); it != m_nodes.end(); ++it )
    {
        it->second.m_planet_root->RegisterScenegraphCallbacks( *m_scenenodegraph );
    }
}

void CDLODPlanetService::UnregisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph )
{
    for( auto it = m_nodes.begin(); it != m_nodes.end(); ++it )
    {
        it->second.m_planet_root->UnregisterScenegraphCallbacks( *m_scenenodegraph );
    }
}

void CDLODPlanetService::ReleaseSceneNode( const dsstring& p_sceneNodeName, DrawSpace::Dynamics::Calendar* p_calendar )
{
    if( m_nodes.count( p_sceneNodeName ) )
    {	
		PlanetInstance pe = m_nodes[p_sceneNodeName];

		p_calendar->UnregisterWorld( pe.m_planet_root->GetWorld() );

		_DRAWSPACE_DELETE_( pe.m_planet_vshader );
		_DRAWSPACE_DELETE_( pe.m_planet_pshader );

		_DRAWSPACE_DELETE_( pe.m_climate_vshader );
		_DRAWSPACE_DELETE_( pe.m_climate_pshader );

		_DRAWSPACE_DELETE_( pe.m_texture_th_pixels );
		_DRAWSPACE_DELETE_( pe.m_texture_th_splatting );

		_DRAWSPACE_DELETE_( pe.m_details_fx );
		_DRAWSPACE_DELETE_( pe.m_climate_fx );

		_DRAWSPACE_DELETE_( pe.m_planet_root );
		_DRAWSPACE_DELETE_( pe.m_planet_node );

		for (int i = 0; i < 6; i++)
		{
			_DRAWSPACE_DELETE_( pe.m_planet_details_binder[i] );
			_DRAWSPACE_DELETE_( pe.m_planet_climate_binder[i] );
		}

        m_nodes.erase( p_sceneNodeName );
    }
}

DrawSpace::Module::KeysLinkTable* CDLODPlanetService::AddSceneNodeConfig( const dsstring& p_sceneNodeName )
{
	return &m_nodes_config[p_sceneNodeName].m_keylinksTable;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlanetInstance::OnGravityEnabledUpdate( bool p_value )
{
	m_planet_root->SetGravityState( p_value );
}

void GravityEnabledParam::OnUpdated( bool p_val )
{
	m_value = p_val;
	if( m_owner )
	{
		m_owner->OnGravityEnabledUpdate( m_value );
	}
}
