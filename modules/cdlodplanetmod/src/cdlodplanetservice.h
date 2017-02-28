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
#include "multifractalbinder.h"
#include "planetclimatebinder.h"
#include "planetdetailsbinder.h"

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
    DrawSpace::SphericalLOD::Config                                 m_config;

	PlanetDetailsBinder*                                            m_planet_details_binder[6];
	PlanetClimateBinder*											m_planet_climate_binder[6];

    DrawSpace::Core::Fx*                                            m_details_fx;
	DrawSpace::Core::Fx*                                            m_climate_fx;

	DrawSpace::Core::Shader*                                        m_climate_vshader;
	DrawSpace::Core::Shader*                                        m_climate_pshader;

    DrawSpace::Core::Shader*                                        m_planet_vshader;
    DrawSpace::Core::Shader*                                        m_planet_pshader;

	DrawSpace::Core::Texture*										m_texture_th_pixels;
	DrawSpace::Core::Texture*										m_texture_th_splatting;

    DrawSpace::SphericalLOD::Root*                                  m_planet_root;
    DrawSpace::Core::SceneNode<DrawSpace::SphericalLOD::Root>*      m_planet_node;

	PlanetSceneNodeConfig*											m_node_config;

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
