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

#ifndef _CDLODPLANETSERVICE_H_
#define _CDLODPLANETSERVICE_H_

#include "module_service.h"
#include "drawspace.h"


class CDLODPlanetService;

class SceneNodeGraphParam : public DrawSpace::Module::KeySink<DrawSpace::Core::SceneNodeGraph*>
{
protected:
    CDLODPlanetService* m_owner;

public:
    SceneNodeGraphParam( const dsstring& p_id, CDLODPlanetService* p_owner );
    virtual void OnUpdated( DrawSpace::Core::SceneNodeGraph* p_val );
};

class TexturePassParam : public DrawSpace::Module::KeySink<DrawSpace::IntermediatePass*>
{
protected:
    CDLODPlanetService* m_owner;

public:
    TexturePassParam( const dsstring& p_id, CDLODPlanetService* p_owner );
    virtual void OnUpdated( DrawSpace::IntermediatePass* p_val );
};

///////////////////////////////////////////////////////////////////////////////////////////////

class PlanetInstance;

class RealParam : public DrawSpace::Module::KeySink<dsreal>
{
public:
	dsreal			m_value;

	RealParam( const dsstring& p_id ) : KeySink( p_id )
    {
    }

	virtual void OnUpdated( dsreal p_val )
    {
        m_value = p_val;
    }
};

class StringParam : public DrawSpace::Module::KeySink<dsstring>
{
public:
	dsstring		m_value;

	StringParam( const dsstring& p_id ) : KeySink( p_id )
	{
	}

	virtual void OnUpdated( dsstring p_val )
	{
		m_value = p_val;
	}
};

class GravityEnabledParam : public DrawSpace::Module::KeySink<bool>
{
protected:

	PlanetInstance*	m_owner;

public:
	bool			m_value;

	GravityEnabledParam( const dsstring& p_id ) : KeySink( p_id ),
	m_owner( NULL )
	{
	}

	virtual void OnUpdated( bool p_val );
	virtual void SetOwner( PlanetInstance* p_owner )
	{
		m_owner = p_owner;
	}
};


///////////////////////////////////////////////////////////////////////////////////////////////

class SimpleColorBinder : public DrawSpace::SphericalLOD::Binder
{
protected:
public:

    SimpleColorBinder( void );

    virtual void Bind( void );
    virtual void Unbind( void );
};

class MultiFractalBinder : public DrawSpace::SphericalLOD::Binder
{
public:

	dsreal                                  m_plains_amplitude;
	dsreal                                  m_mountains_amplitude;
	dsreal                                  m_vertical_offset;
	dsreal                                  m_mountains_offset;

	dsreal                                  m_plains_seed1;
	dsreal                                  m_plains_seed2;
	dsreal                                  m_mix_seed1;
	dsreal                                  m_mix_seed2;

public:

	MultiFractalBinder(void);

	virtual void Bind(void);
	virtual void Unbind(void);
};

class PlanetClimateBinder : public MultiFractalBinder
{
protected:

public:

	PlanetClimateBinder( void );

	virtual void Bind( void );
	virtual void Unbind( void );
};

class PlanetLight
{
public:

	PlanetLight(void) :
		m_enable(false)
	{
	};


	bool														m_enable;
	DrawSpace::Utils::Vector									m_color;
	DrawSpace::Utils::Vector									m_dir;
	DrawSpace::Utils::Vector									m_local_dir;

};

class PlanetDetailsBinder : public MultiFractalBinder
{
protected:

	dsreal														m_innerRadius;
	dsreal														m_outerRadius;
	DrawSpace::Utils::Vector									m_waveLength;
	dsreal														m_kr;
	dsreal														m_km;
	dsreal														m_scaleDepth;

	dsreal														m_skyfromspace_ESun;
	dsreal														m_skyfromatmo_ESun;
	dsreal														m_groundfromspace_ESun;
	dsreal														m_groundfromatmo_ESun;

	DrawSpace::Utils::Vector									m_atmo_scattering_flags0;
	DrawSpace::Utils::Vector									m_atmo_scattering_flags1;
	DrawSpace::Utils::Vector									m_atmo_scattering_flags2;
	DrawSpace::Utils::Vector									m_atmo_scattering_flags3;
	DrawSpace::Utils::Vector									m_atmo_scattering_flags4;
	DrawSpace::Utils::Vector									m_atmo_scattering_flags5;
	DrawSpace::Utils::Vector									m_atmo_scattering_flags6;


	DrawSpace::Core::SceneNode<DrawSpace::SphericalLOD::Root>* m_planet_node;

	bool														m_ambient;
	DrawSpace::Utils::Vector									m_ambient_color;

	PlanetLight													m_lights[3];

	DrawSpace::Utils::Matrix									m_planet_final_transform_rots;

	bool														m_mirror_mode;

	dsreal														m_ocean_details_alt;
	dsreal														m_terrain_bump_factor;

public:

	PlanetDetailsBinder( dsreal p_planetRay, dsreal p_atmoThickness );

	virtual void Bind( void );
	virtual void Unbind( void );

	void SetPlanetNode( DrawSpace::Core::SceneNode<DrawSpace::SphericalLOD::Root>* p_planet_node );

	void Update( void );

	void SetMirrorMode( bool p_state )
	{
		m_mirror_mode = p_state;
	}

	dsreal GetOceansDetailsAlt( void) { return m_ocean_details_alt; };

	PlanetLight GetLight( int p_index ) { return m_lights[p_index]; };
};


///////////////////////////////////////////////////////////////////////////////////////////////

class PlanetSceneNodeConfig
{
public:
	/////////////////--*- ENSEMBLE DES PARAMETRES PLANETE -*-- ////////////////////

	StringParam							m_planetName;

	RealParam                           m_planetRay;

	StringParam							m_detailsVertexShader;
	StringParam							m_detailsPixelShader;

	GravityEnabledParam					m_gravityEnabled;

	////////////////////////////////////////////////////////////////////////////////

	DrawSpace::Module::KeysLinkTable    m_keylinksTable;

public:

	PlanetSceneNodeConfig( void ) :
	m_planetName( "planetName" ),
	m_planetRay("planetRay"),
	m_detailsVertexShader( "detailsVertexShader" ),
	m_detailsPixelShader( "detailsPixelShader" ),
	m_gravityEnabled( "gravityEnabled" )
	{
		m_keylinksTable.RegisterModuleKey( &m_planetName );
		m_keylinksTable.RegisterModuleKey( &m_planetRay );
		m_keylinksTable.RegisterModuleKey( &m_detailsVertexShader );
		m_keylinksTable.RegisterModuleKey( &m_detailsPixelShader );
		m_keylinksTable.RegisterModuleKey( &m_gravityEnabled );
	}

	virtual void SetOwner( PlanetInstance* p_owner )
	{
		m_gravityEnabled.SetOwner( p_owner );
	}
};

class PlanetInstance
{

public:
    DrawSpace::SphericalLOD::Config                                 config;

	PlanetDetailsBinder*                                            planet_details_binder[6];
	PlanetClimateBinder*											planet_climate_binder[6];

    DrawSpace::Core::Fx*                                            details_fx;
	DrawSpace::Core::Fx*                                            climate_fx;

	DrawSpace::Core::Shader*                                        climate_vshader;
	DrawSpace::Core::Shader*                                        climate_pshader;

    DrawSpace::Core::Shader*                                        planet_vshader;
    DrawSpace::Core::Shader*                                        planet_pshader;

	DrawSpace::Core::Texture*										texture_th_pixels;
	DrawSpace::Core::Texture*										texture_th_splatting;

    DrawSpace::SphericalLOD::Root*                                  planet;
    DrawSpace::Core::SceneNode<DrawSpace::SphericalLOD::Root>*      planet_node;

	PlanetSceneNodeConfig*											node_config;

public:
	void OnGravityEnabledUpdate( bool p_value );
};



///////////////////////////////////////////////////////////////////////////////////////////////

class CDLODPlanetService : public DrawSpace::Interface::Module::Service
{
protected:

    DrawSpace::Interface::Renderer*                                     m_renderer;
    DrawSpace::Utils::TimeManager                                       m_tm;
    dsstring                                                            m_device;


    SceneNodeGraphParam*                                                m_scparam;
    TexturePassParam*                                                   m_texturepassparam;


    DrawSpace::Core::SceneNodeGraph*                                    m_scenenodegraph;
    DrawSpace::IntermediatePass*                                        m_texturepass;



	// liste des config possibles pour instancier des scenenodes planetes
	std::map<dsstring, PlanetSceneNodeConfig>							m_nodes_config;

	// liste des instances scenenodes planetes
    std::map<dsstring, PlanetInstance>									m_nodes;


public:
    CDLODPlanetService( const dsstring& p_id );
    ~CDLODPlanetService( void );

    virtual void								GetKeys( std::vector<DrawSpace::Module::KeySinkBase*>& p_keys );
    virtual void								Init( DrawSpace::Logger::Configuration* p_logconf, 
                                                    DrawSpace::Core::BaseCallback<void, bool>* p_mousecircularmode_cb, 
                                                    DrawSpace::Core::BaseCallback<void, bool>* p_mousevisible_cb, 
                                                    DrawSpace::Core::BaseCallback<void, int>* p_closeapp_cb );

    virtual void								Run( void );
    virtual void								Release( void );

    virtual DrawSpace::Core::BaseSceneNode*		InstanciateSceneNode( const dsstring& p_sceneNodeName, DrawSpace::Dynamics::Calendar* p_calendar );
    virtual void								RegisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph );
    virtual void								UnregisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph );
    virtual void								ReleaseSceneNode( const dsstring& p_sceneNodeName, DrawSpace::Dynamics::Calendar* p_calendar );

	virtual DrawSpace::Module::KeysLinkTable*	AddSceneNodeConfig( const dsstring& p_sceneNodeName );

    virtual void								OnSceneNodeGraphUpdated( DrawSpace::Core::SceneNodeGraph* p_val );
    virtual void								OnTexturePassUpdate( DrawSpace::IntermediatePass* p_val );
};

#endif
