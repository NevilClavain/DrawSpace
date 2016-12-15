/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2016                              
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
 /*
    for( int i = 0; i < 6; i++ )
    {
        m_simplebinder[i] = _DRAWSPACE_NEW_( SimpleColorBinder, SimpleColorBinder );
    }

    Shader* planet_vshader = _DRAWSPACE_NEW_( Shader, Shader( "planet_surface_flat.vso", true ) );
    Shader* planet_pshader = _DRAWSPACE_NEW_( Shader, Shader( "planet_surface_flat.pso", true ) );

    planet_vshader->LoadFromFile();
    planet_pshader->LoadFromFile();


    m_config.m_lod0base = 19000.0;
    m_config.m_ground_layer = 0;
    m_config.m_nbLODRanges_inertBodies = NB_LOD_INERTBODIES;
    m_config.m_nbLODRanges_freeCameras = NB_LOD_FREECAMERAS;


    m_details_fx = new Fx;

    m_details_fx->AddShader( planet_vshader );
    m_details_fx->AddShader( planet_pshader );


    m_details_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_details_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_details_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETVERTEXTEXTUREFILTERTYPE, "linear" ) );
    m_details_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "line" ) );

    m_details_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_details_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    m_details_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETVERTEXTEXTUREFILTERTYPE, "none" ) );
    m_details_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "solid" ) );


    m_details_fx->SetRenderStateUniqueQueueID( "planet01" ); // parce qu'on va updater le renderstate ENABLEZBUFFER pendant le rendu

    for( int i = 0; i < 6; i++ )
    {
        m_simplebinder[i]->SetFx( m_details_fx );
    }


    for( int i = 0; i < 6; i++ )
    {
        m_simplebinder[i]->SetRenderer( m_renderer );
    }


    SphericalLOD::Config::LayerDescriptor planet_surface;
    planet_surface.enable_collisions = true;
    planet_surface.enable_datatextures = true;
    planet_surface.enable_lod = true;
    planet_surface.min_lodlevel = 0;
    planet_surface.ray = PLANET_RAY;
    for( int i = 0; i < 6; i++ )
    {
        planet_surface.groundCollisionsBinder[i] = NULL;
        planet_surface.patchTexturesBinder[i] = NULL;
    }

    m_config.m_layers_descr.push_back( planet_surface );


    m_planet = _DRAWSPACE_NEW_( DrawSpace::SphericalLOD::Root, DrawSpace::SphericalLOD::Root( p_sceneNodeName, PLANET_RAY, &m_tm, m_config ) );


    */


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
}

DrawSpace::Core::BaseSceneNode* CDLODPlanetService::InstanciateSceneNode( const dsstring& p_sceneNodeName )
{







    return NULL;
}

void CDLODPlanetService::ReleaseSceneNode( const dsstring& p_sceneNodeName )
{
}