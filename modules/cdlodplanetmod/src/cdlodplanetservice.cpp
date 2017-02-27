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


//#define PLANET_RAY                          500.0
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

MultiFractalBinder::MultiFractalBinder(void) :
m_plains_amplitude(PLAINS_AMPLITUDE),
m_mountains_amplitude(MOUNTAINS_AMPLITUDE),
m_vertical_offset(VERTICAL_OFFSET),
m_mountains_offset(MOUNTAINS_OFFSET),
m_plains_seed1(234.4),
m_plains_seed2(9334.1),
m_mix_seed1(823.4),
m_mix_seed2(509.0)
{
}

void MultiFractalBinder::Bind(void)
{
	Vector landscape_control;
	Vector seeds;

	landscape_control[0] = m_plains_amplitude;
	landscape_control[1] = m_mountains_amplitude;
	landscape_control[2] = m_vertical_offset;
	landscape_control[3] = m_mountains_offset;

	seeds[0] = m_plains_seed1;
	seeds[1] = m_plains_seed2;
	seeds[2] = m_mix_seed1;
	seeds[3] = m_mix_seed2;

	m_renderer->SetFxShaderParams(0, 30, landscape_control);
	m_renderer->SetFxShaderParams(0, 31, seeds);
}

void MultiFractalBinder::Unbind(void)
{
}



PlanetClimateBinder::PlanetClimateBinder(void) : MultiFractalBinder()
{
}

void PlanetClimateBinder::Bind(void)
{
	// planete temperee


	Vector thparams(40.0, 20.0, TEMP_DEC_PER_KM, BEACH_LIMIT);
	Vector thparams2(0.48, 0.87, 0.45, 0.75);



	// planete chaude et peu humide (aride) : desertique
	//Vector thparams( 0.0, 10.0, TEMP_DEC_PER_KM, BEACH_LIMIT );
	//Vector thparams2( 0.0, 0.0, 0.78, 0.78 );

	// planete chaude et tres humide : monde tropical
	//Vector thparams( 2500.0, 1800.0, TEMP_DEC_PER_KM, BEACH_LIMIT );
	//Vector thparams2( 0.05, 0.1, 0.45, 0.48 );


	// monde glacé et plutot sec
	//Vector thparams( 50.0, 80.0, TEMP_DEC_PER_KM, BEACH_LIMIT );
	//Vector thparams2( 1.4, 1.5, 0.28, 0.99 );

	// monde froid et plutot humide
	//Vector thparams( 1400.0, 1900.0, TEMP_DEC_PER_KM, BEACH_LIMIT );
	//Vector thparams2( 0.92, 1.5, 0.37, 0.99 );


	m_renderer->SetFxShaderParams( 0, 32, thparams );
	m_renderer->SetFxShaderParams( 0, 33, thparams2 );

	MultiFractalBinder::Bind();
}

void PlanetClimateBinder::Unbind( void )
{
	MultiFractalBinder::Unbind();
}

PlanetDetailsBinder::PlanetDetailsBinder( dsreal p_planetRay, dsreal p_atmoThickness ) :
m_planet_node( NULL),
m_ocean_details_alt( 1.0010 ),
m_terrain_bump_factor( TERRAIN_BUMP_FACTOR )
{

	m_mirror_mode = false;

	m_skyfromspace_ESun = 8.7; //9.0;
	m_skyfromatmo_ESun = 70.0; //50.0;
	m_groundfromspace_ESun = 24.0;
	m_groundfromatmo_ESun = 12.0;

	m_innerRadius = p_planetRay;
	m_outerRadius = p_planetRay + p_atmoThickness;

	m_waveLength[0] = 0.650;
	m_waveLength[1] = 0.570;
	m_waveLength[2] = 0.475;
	m_kr = 0.0025;
	m_km = 0.0010;
	m_scaleDepth = 0.25;

	m_atmo_scattering_flags0[0] = m_outerRadius;
	m_atmo_scattering_flags0[1] = m_innerRadius;
	m_atmo_scattering_flags0[2] = m_outerRadius * m_outerRadius;
	m_atmo_scattering_flags0[3] = m_innerRadius * m_innerRadius;

	m_atmo_scattering_flags1[0] = m_scaleDepth;
	m_atmo_scattering_flags1[1] = 1.0 / m_scaleDepth;
	m_atmo_scattering_flags1[2] = 1.0 / (m_outerRadius - m_innerRadius);
	m_atmo_scattering_flags1[3] = m_atmo_scattering_flags1[2] / m_scaleDepth;

	m_atmo_scattering_flags2[0] = 1.0 / pow(m_waveLength[0], 4.0);
	m_atmo_scattering_flags2[1] = 1.0 / pow(m_waveLength[1], 4.0);
	m_atmo_scattering_flags2[2] = 1.0 / pow(m_waveLength[2], 4.0);

	m_atmo_scattering_flags3[0] = m_kr;
	m_atmo_scattering_flags3[1] = m_km;
	m_atmo_scattering_flags3[2] = 4.0 * m_kr * 3.1415927;
	m_atmo_scattering_flags3[3] = 4.0 * m_km * 3.1415927;

	m_atmo_scattering_flags4[0] = m_skyfromspace_ESun;
	m_atmo_scattering_flags4[1] = m_skyfromatmo_ESun;
	m_atmo_scattering_flags4[2] = m_groundfromspace_ESun;
	m_atmo_scattering_flags4[3] = m_groundfromatmo_ESun;

	m_atmo_scattering_flags5[0] = ATMO_SCATTERING_SPACE_GROUND_LIMIT; // altitude limite de transition entre xxxfromspace_atmo_scattering et xxxfromatmo_atmo_scattering
	m_atmo_scattering_flags5[1] = FOG_ALT_LIMIT; // altitude debut d'apparition du fog "sol"
	m_atmo_scattering_flags5[2] = FOG_DENSITY; // intensite fog "sol"
	m_atmo_scattering_flags5[3] = ATMO_SCATTERING_ALPHA_ALT_VIEWER; // altitude limite pour prise en compte facteur altitude camera pour le calcul de l'alpha

	// couleurs fog "sol"    
	m_atmo_scattering_flags6[0] = 0.45;
	m_atmo_scattering_flags6[1] = 0.63;
	m_atmo_scattering_flags6[2] = 0.78;
	m_atmo_scattering_flags6[3] = 1.0;


	m_lights[0].m_enable = true;
	m_lights[0].m_color[0] = 1.0;
	m_lights[0].m_color[1] = 0.99;
	m_lights[0].m_color[2] = 0.99;
	m_lights[0].m_color[3] = 1.0;
	m_lights[0].m_dir[0] = -0.6;
	m_lights[0].m_dir[1] = 0.0;
	m_lights[0].m_dir[2] = 0.6; // 1.0
	m_lights[0].m_dir[3] = 1.0;

	m_lights[0].m_dir.Normalize();

	m_lights[1].m_enable = false;
	m_lights[1].m_color[0] = 1.0;
	m_lights[1].m_color[1] = 0.9;
	m_lights[1].m_color[2] = 0.9;
	m_lights[1].m_color[3] = 1.0;
	m_lights[1].m_dir[0] = -1.0;
	m_lights[1].m_dir[1] = 0.2;
	m_lights[1].m_dir[2] = 0.0;
	m_lights[1].m_dir[3] = 1.0;

	m_lights[1].m_dir.Normalize();

	m_lights[2].m_enable = false;
	m_lights[2].m_color[0] = 1.0;
	m_lights[2].m_color[1] = 0.5;
	m_lights[2].m_color[2] = 0.5;
	m_lights[2].m_color[3] = 1.0;
	m_lights[2].m_dir[0] = 0.0;
	m_lights[2].m_dir[1] = -1.0;
	m_lights[2].m_dir[2] = 0.0;
	m_lights[2].m_dir[3] = 1.0;

	m_lights[2].m_dir.Normalize();

	m_ambient = false;
	m_ambient_color[0] = 0.1;
	m_ambient_color[1] = 0.1;
	m_ambient_color[2] = 0.1;
	m_ambient_color[3] = 1.0;

}

void PlanetDetailsBinder::Bind( void )
{

	Vector flags6(16.0, 1.095, 1.0040, m_ocean_details_alt);

	Vector flags_lights;
	flags_lights[0] = (m_ambient ? 1.0 : 0.0);
	flags_lights[1] = (m_lights[0].m_enable ? 1.0 : 0.0);
	flags_lights[2] = (m_lights[1].m_enable ? 1.0 : 0.0);
	flags_lights[3] = (m_lights[2].m_enable ? 1.0 : 0.0);


	m_renderer->SetFxShaderParams(0, 32, m_atmo_scattering_flags0);
	m_renderer->SetFxShaderParams(0, 33, m_atmo_scattering_flags1);
	m_renderer->SetFxShaderParams(0, 34, m_atmo_scattering_flags2);
	m_renderer->SetFxShaderParams(0, 35, m_atmo_scattering_flags3);
	m_renderer->SetFxShaderParams(0, 36, m_atmo_scattering_flags4);
	m_renderer->SetFxShaderParams(0, 37, m_atmo_scattering_flags5);
	m_renderer->SetFxShaderParams(0, 38, m_atmo_scattering_flags6);

	m_renderer->SetFxShaderParams(0, 40, flags_lights);
	m_renderer->SetFxShaderParams(0, 41, m_ambient_color);

	m_renderer->SetFxShaderParams(0, 42, m_lights[0].m_local_dir);
	m_renderer->SetFxShaderParams(0, 43, m_lights[0].m_dir);
	m_renderer->SetFxShaderParams(0, 44, m_lights[0].m_color);

	m_renderer->SetFxShaderParams(0, 45, m_lights[1].m_local_dir);
	m_renderer->SetFxShaderParams(0, 46, m_lights[1].m_dir);
	m_renderer->SetFxShaderParams(0, 47, m_lights[1].m_color);

	m_renderer->SetFxShaderParams(0, 48, m_lights[2].m_local_dir);
	m_renderer->SetFxShaderParams(0, 49, m_lights[2].m_dir);
	m_renderer->SetFxShaderParams(0, 50, m_lights[2].m_color);

	Vector mirror_flag;

	mirror_flag[0] = (m_mirror_mode ? 1.0 : 0.0);
	mirror_flag[1] = m_innerRadius;

	m_renderer->SetFxShaderParams(0, 51, mirror_flag);

	m_renderer->SetFxShaderParams(1, 6, flags6);

	m_renderer->SetFxShaderParams(1, 7, flags_lights);
	m_renderer->SetFxShaderParams(1, 8, m_ambient_color);

	m_renderer->SetFxShaderParams(1, 9, m_lights[0].m_local_dir);
	m_renderer->SetFxShaderParams(1, 10, m_lights[0].m_dir);
	m_renderer->SetFxShaderParams(1, 11, m_lights[0].m_color);

	m_renderer->SetFxShaderParams(1, 12, m_lights[1].m_local_dir);
	m_renderer->SetFxShaderParams(1, 13, m_lights[1].m_dir);
	m_renderer->SetFxShaderParams(1, 14, m_lights[1].m_color);

	m_renderer->SetFxShaderParams(1, 15, m_lights[2].m_local_dir);
	m_renderer->SetFxShaderParams(1, 16, m_lights[2].m_dir);
	m_renderer->SetFxShaderParams(1, 17, m_lights[2].m_color);

	m_renderer->SetFxShaderParams(1, 18, m_atmo_scattering_flags0);
	m_renderer->SetFxShaderParams(1, 19, m_atmo_scattering_flags1);
	m_renderer->SetFxShaderParams(1, 20, m_atmo_scattering_flags2);
	m_renderer->SetFxShaderParams(1, 21, m_atmo_scattering_flags3);
	m_renderer->SetFxShaderParams(1, 22, m_atmo_scattering_flags4);
	m_renderer->SetFxShaderParams(1, 23, m_atmo_scattering_flags5);
	m_renderer->SetFxShaderParams(1, 24, m_atmo_scattering_flags6);

	m_planet_final_transform_rots.Transpose(); // faire comme dans le plugin
	m_renderer->SetFxShaderMatrix(1, 25, m_planet_final_transform_rots);


	Vector terrain_bump_flag(m_terrain_bump_factor, 0.0, 0.0, 0.0);
	m_renderer->SetFxShaderParams(1, 31, terrain_bump_flag);

	MultiFractalBinder::Bind();
}

void PlanetDetailsBinder::Unbind( void )
{
	MultiFractalBinder::Unbind();
}

void PlanetDetailsBinder::SetPlanetNode( DrawSpace::Core::SceneNode<DrawSpace::SphericalLOD::Root>* p_planet_node )
{
	m_planet_node = p_planet_node;
}

void PlanetDetailsBinder::Update( void )
{
	Matrix planet_final_transform;

	m_planet_node->GetFinalTransform(planet_final_transform);

	Vector planet_pos;
	planet_pos[0] = - planet_final_transform(3, 0);
	planet_pos[1] = - planet_final_transform(3, 1);
	planet_pos[2] = - planet_final_transform(3, 2);
	planet_pos[3] = 1.0;

	planet_pos.Normalize();


	m_lights[0].m_dir.Normalize();

	planet_final_transform.ClearTranslation();
	m_planet_final_transform_rots = planet_final_transform;



	planet_final_transform.Transpose();

	for (long i = 0; i < 3; i++)
	{
		if (m_lights[i].m_enable)
		{
			planet_final_transform.Transform( &m_lights[i].m_dir, &m_lights[i].m_local_dir );

			m_lights[i].m_local_dir.Normalize();
		}
	}
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
    for( auto it = m_nodes.begin(); it != m_nodes.end(); ++it )
    {
        it->second.planet->DrawSubPasses();

		for (int i = 0; i < 6; i++)
		{
			it->second.planet_details_binder[i]->Update();
		}
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
		pe.node_config = &m_nodes_config[p_sceneNodeName];

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

		pe.planet_details_binder[i] = _DRAWSPACE_NEW_( PlanetDetailsBinder, PlanetDetailsBinder( pe.node_config->m_planetRay.m_value * 1000.0, PLANET_ATMO_THICKNESS ) );
		pe.planet_climate_binder[i] = _DRAWSPACE_NEW_( PlanetClimateBinder, PlanetClimateBinder );
    }

	pe.planet_vshader = _DRAWSPACE_NEW_( Shader, Shader( pe.node_config->m_detailsVertexShader.m_value, true ) );
	pe.planet_pshader = _DRAWSPACE_NEW_( Shader, Shader( pe.node_config->m_detailsPixelShader.m_value, true ) );

	pe.texture_th_pixels = _DRAWSPACE_NEW_( Texture, Texture( "earth_th_pixels_16.jpg" ) );
	pe.texture_th_splatting = _DRAWSPACE_NEW_( Texture, Texture( "earth_th_splatting_16.jpg" ) );

	// chargement des medias; TODO : verifier le status retour de chaque LoadFromFile();
	pe.texture_th_pixels->LoadFromFile();
	pe.texture_th_splatting->LoadFromFile();
	pe.planet_vshader->LoadFromFile();
	pe.planet_pshader->LoadFromFile();


	pe.details_fx = _DRAWSPACE_NEW_( Fx, Fx );

    pe.details_fx->AddShader( pe.planet_vshader );
    pe.details_fx->AddShader( pe.planet_pshader );

    pe.details_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    pe.details_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    pe.details_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETVERTEXTEXTUREFILTERTYPE, "linear" ) );
    //pe.details_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "line" ) );

    pe.details_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    pe.details_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    pe.details_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETVERTEXTEXTUREFILTERTYPE, "none" ) );
    //pe.details_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "solid" ) );


    pe.details_fx->SetRenderStateUniqueQueueID( p_sceneNodeName ); // parce qu'on va updater le renderstate ENABLEZBUFFER pendant le rendu

    for( int i = 0; i < 6; i++ )
    {
		pe.planet_details_binder[i]->SetFx( pe.details_fx );

		pe.planet_details_binder[i]->SetTexture( pe.texture_th_pixels, 0 );
		pe.planet_details_binder[i]->SetTexture( pe.texture_th_splatting, 1 );
    }

	pe.climate_vshader = _DRAWSPACE_NEW_( Shader, Shader( "planet_ht.vso", true ) );
	pe.climate_pshader = _DRAWSPACE_NEW_( Shader, Shader( "planet_ht.pso", true ) );

	pe.climate_vshader->LoadFromFile();
	pe.climate_pshader->LoadFromFile();

	pe.climate_fx = _DRAWSPACE_NEW_( Fx, Fx );
	pe.climate_fx->AddShader( pe.climate_vshader );
	pe.climate_fx->AddShader( pe.climate_pshader );
    
	pe.climate_fx->AddRenderStateIn(DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETVERTEXTEXTUREFILTERTYPE, "linear" ) );
    //pe.climate_fx->AddRenderStateIn(DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    pe.climate_fx->AddRenderStateIn(DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );

	pe.climate_fx->AddRenderStateOut(DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETVERTEXTEXTUREFILTERTYPE, "none" ) );
    //pe.climate_fx->AddRenderStateOut(DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    pe.climate_fx->AddRenderStateOut(DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );
    

	for (int i = 0; i < 6; i++)
	{
		pe.planet_climate_binder[i]->SetFx( pe.climate_fx );
	}

    for( int i = 0; i < 6; i++ )
    {
		pe.planet_details_binder[i]->SetRenderer( m_renderer );
		pe.planet_climate_binder[i]->SetRenderer( m_renderer );
    }


    pe.config.m_lod0base = 19000.0;
    pe.config.m_ground_layer = 0;
    pe.config.m_nbLODRanges_inertBodies = NB_LOD_INERTBODIES;
    pe.config.m_nbLODRanges_freeCameras = NB_LOD_FREECAMERAS;



    SphericalLOD::Config::LayerDescriptor planet_surface;
    planet_surface.enable_collisions = false;
    planet_surface.enable_datatextures = true;
    planet_surface.enable_lod = true;
    planet_surface.min_lodlevel = 0;
	planet_surface.ray = pe.node_config->m_planetRay.m_value; //PLANET_RAY;
    for( int i = 0; i < 6; i++ )
    {
        planet_surface.groundCollisionsBinder[i] = NULL;
		planet_surface.patchTexturesBinder[i] = pe.planet_climate_binder[i];
    }

    pe.config.m_layers_descr.push_back( planet_surface );


	pe.planet = _DRAWSPACE_NEW_( DrawSpace::SphericalLOD::Root, DrawSpace::SphericalLOD::Root( p_sceneNodeName, /*PLANET_RAY*/pe.node_config->m_planetRay.m_value, &m_tm, pe.config ) );

    for( int i = 0; i < 6; i++ )
    {
		pe.planet->RegisterSinglePassSlot( m_texturepass, pe.planet_details_binder[i], i, DrawSpace::SphericalLOD::Body::LOWRES_SKIRT_MESHE, 0, 2000 );
    }


    // temp
    pe.planet->SetOrbitDuration( 0.333 );
    pe.planet->SetRevolutionTiltAngle( 0.0 );    
    pe.planet->SetRevolutionDuration( 1.0 );
    

    pe.planet_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::SphericalLOD::Root>, SceneNode<DrawSpace::SphericalLOD::Root>( p_sceneNodeName ) );
    pe.planet_node->SetContent( pe.planet );

	for (int i = 0; i < 6; i++)
	{
		pe.planet_details_binder[i]->SetPlanetNode( pe.planet_node );
	}

	p_calendar->RegisterWorld( pe.planet->GetWorld() );

	pe.planet->SetGravityState( pe.node_config->m_gravityEnabled.m_value );

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

void CDLODPlanetService::ReleaseSceneNode( const dsstring& p_sceneNodeName, DrawSpace::Dynamics::Calendar* p_calendar )
{
    if( m_nodes.count( p_sceneNodeName ) )
    {	
		PlanetInstance pe = m_nodes[p_sceneNodeName];

		p_calendar->UnregisterWorld( pe.planet->GetWorld() );

		_DRAWSPACE_DELETE_( pe.planet_vshader );
		_DRAWSPACE_DELETE_( pe.planet_pshader );

		_DRAWSPACE_DELETE_( pe.climate_vshader );
		_DRAWSPACE_DELETE_( pe.climate_pshader );

		_DRAWSPACE_DELETE_( pe.texture_th_pixels );
		_DRAWSPACE_DELETE_( pe.texture_th_splatting );

		_DRAWSPACE_DELETE_( pe.details_fx );
		_DRAWSPACE_DELETE_( pe.climate_fx );

		_DRAWSPACE_DELETE_( pe.planet );
		_DRAWSPACE_DELETE_( pe.planet_node );

		for (int i = 0; i < 6; i++)
		{
			_DRAWSPACE_DELETE_( pe.planet_details_binder[i] );
			_DRAWSPACE_DELETE_( pe.planet_climate_binder[i] );
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
	planet->SetGravityState( p_value );
}

void GravityEnabledParam::OnUpdated( bool p_val )
{
	m_value = p_val;
	if( m_owner )
	{
		m_owner->OnGravityEnabledUpdate( m_value );
	}
}
