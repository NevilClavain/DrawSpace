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


#define PLANET_RAY                          500.0
#define PLANET_ATMO_THICKNESS               12000.0
#define ATMO_SCATTERING_SPACE_GROUND_LIMIT  70000.0
#define FOG_ALT_LIMIT                       10000.0
#define ATMO_SCATTERING_ALPHA_ALT_VIEWER    75000.0
#define FLAT_CLOUDS_ALT                     3300.0
#define VOLUMETRIC_CLOUDS_DISTANCE_CLIP     60000.0
#define VOLUMETRIC_CLOUDS_ALT               2400.0
#define VOLUMETRIC_CLOUDS_MIN_SPEED_DEG_S   0.003
#define VOLUMETRIC_CLOUDS_MAX_SPEED_DEG_S   0.004
#define PLAINS_AMPLITUDE                    50.0
#define MOUNTAINS_AMPLITUDE                 800.0
#define MOUNTAINS_OFFSET                    -120.0
#define VERTICAL_OFFSET                     0.0
#define TEMP_DEC_PER_KM                     66.0
#define BEACH_LIMIT                         25.0
#define CLOUDS_PROCEDURALRULES_FILE         "planet_clouds_small_small.rules"
#define CLOUDS_HEIGHT                       750.0
#define CLOUDS_FOG_DENSITY                  0.000045
#define FOG_DENSITY                         0.00050
#define ZBUFFER_ACTIVATION_REL_ALT          1.0099
#define TERRAIN_BUMP_FACTOR                 10.0
#define NB_LOD_FREECAMERAS                  14
#define NB_LOD_INERTBODIES                  15

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


SimpleColorBinder::SimpleColorBinder( void )
{
}

void SimpleColorBinder::Bind( void )
{
}

void SimpleColorBinder::Unbind( void )
{
}

////////////////////////////////////////////

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
}

void CDLODPlanetService::Release( void )
{
    _DSDEBUG( logger, dsstring("CDLODPlanet service : shutdown...") );
}

DrawSpace::Core::BaseSceneNode* CDLODPlanetService::InstanciateSceneNode( const dsstring& p_sceneNodeName )
{
    PlanetEntry pe;

    for( int i = 0; i < 6; i++ )
    {
        pe.simplebinder[i] = _DRAWSPACE_NEW_( SimpleColorBinder, SimpleColorBinder );
    }

    pe.planet_vshader = _DRAWSPACE_NEW_( Shader, Shader( "planet_surface.vso", true ) );
    pe.planet_pshader = _DRAWSPACE_NEW_( Shader, Shader( "planet_surface.pso", true ) );

    pe.planet_vshader->LoadFromFile();
    pe.planet_pshader->LoadFromFile();


	pe.details_fx = _DRAWSPACE_NEW_( Fx, Fx );

    pe.details_fx->AddShader( pe.planet_vshader );
    pe.details_fx->AddShader( pe.planet_pshader );

    //pe.details_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "none" ) );
    pe.details_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    pe.details_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    pe.details_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETVERTEXTEXTUREFILTERTYPE, "linear" ) );
    pe.details_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "line" ) );

    pe.details_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    pe.details_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    pe.details_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETVERTEXTEXTUREFILTERTYPE, "none" ) );
    pe.details_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "solid" ) );


    pe.details_fx->SetRenderStateUniqueQueueID( p_sceneNodeName ); // parce qu'on va updater le renderstate ENABLEZBUFFER pendant le rendu

    for( int i = 0; i < 6; i++ )
    {
        pe.simplebinder[i]->SetFx( pe.details_fx );
    }


    for( int i = 0; i < 6; i++ )
    {
        pe.simplebinder[i]->SetRenderer( m_renderer );
    }


    pe.config.m_lod0base = 19000.0;
    pe.config.m_ground_layer = 0;
    pe.config.m_nbLODRanges_inertBodies = NB_LOD_INERTBODIES;
    pe.config.m_nbLODRanges_freeCameras = NB_LOD_FREECAMERAS;



    SphericalLOD::Config::LayerDescriptor planet_surface;
    planet_surface.enable_collisions = false;
    planet_surface.enable_datatextures = false;
    planet_surface.enable_lod = true;
    planet_surface.min_lodlevel = 0;
    planet_surface.ray = PLANET_RAY;
    for( int i = 0; i < 6; i++ )
    {
        planet_surface.groundCollisionsBinder[i] = NULL;
        planet_surface.patchTexturesBinder[i] = NULL;
    }

    pe.config.m_layers_descr.push_back( planet_surface );


    pe.planet = _DRAWSPACE_NEW_( DrawSpace::SphericalLOD::Root, DrawSpace::SphericalLOD::Root( p_sceneNodeName, PLANET_RAY, &m_tm, pe.config ) );

    for( int i = 0; i < 6; i++ )
    {
        pe.planet->RegisterSinglePassSlot( m_texturepass, pe.simplebinder[i], i, DrawSpace::SphericalLOD::Body::LOWRES_SKIRT_MESHE, 0, 2000 );
    }

    // temp
    pe.planet->SetOrbitDuration( 0.333 );
    pe.planet->SetRevolutionTiltAngle( 0.0 );    
    pe.planet->SetRevolutionDuration( 1.0 );
    

    pe.planet_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::SphericalLOD::Root>, SceneNode<DrawSpace::SphericalLOD::Root>( p_sceneNodeName ) );
    pe.planet_node->SetContent( pe.planet );

    m_nodes[p_sceneNodeName] = pe;

    return pe.planet_node;
}

void CDLODPlanetService::RegisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph )
{
    for( auto it = m_nodes.begin(); it != m_nodes.end(); ++it )
    {
        it->second.planet->RegisterScenegraphCallbacks( *m_scenenodegraph );
    }
}

void CDLODPlanetService::UnregisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph )
{
    for( auto it = m_nodes.begin(); it != m_nodes.end(); ++it )
    {
        it->second.planet->UnregisterScenegraphCallbacks( *m_scenenodegraph );
    }
}

void CDLODPlanetService::ReleaseSceneNode( const dsstring& p_sceneNodeName )
{
    if( m_nodes.count( p_sceneNodeName ) )
    {
		PlanetEntry pe = m_nodes[p_sceneNodeName];

		_DRAWSPACE_DELETE_( pe.planet_vshader );
		_DRAWSPACE_DELETE_( pe.planet_pshader );

		_DRAWSPACE_DELETE_( pe.details_fx );
		_DRAWSPACE_DELETE_( pe.planet );
		_DRAWSPACE_DELETE_( pe.planet_node );

		for (int i = 0; i < 6; i++)
		{
			_DRAWSPACE_DELETE_( pe.simplebinder[i] );
		}

        m_nodes.erase( p_sceneNodeName );
    }
}

DrawSpace::Module::KeysLinkTable* CDLODPlanetService::AddSceneNodeConfig( const dsstring& p_sceneNodeName )
{
	m_nodes_config[p_sceneNodeName] = PlanetSceneNodeConfig();
	return &m_nodes_config[p_sceneNodeName].m_keylinksTable;
}