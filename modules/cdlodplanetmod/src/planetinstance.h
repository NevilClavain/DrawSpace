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

#ifndef _PLANETINSTANCE_H_
#define _PLANETINSTANCE_H_

#include "planetconfig.h"
#include "planetdetailsbinder.h"
#include "planetclimatebinder.h"

#include "planetloddependantnodeinfos.h"

class PlanetInstance
{
public:

    typedef DrawSpace::Core::BaseCallback2<void, const dsstring&, bool >  LODDependantNodeInfoStateHandler;


protected:

    DrawSpace::SphericalLOD::Config                                 m_config;

	PlanetDetailsBinder*                                            m_planet_details_binder[6];
	PlanetClimateBinder*											m_planet_climate_binder[6];
    PlanetDetailsBinder*                                            m_planet_atmosphere_binder[6];


    DrawSpace::Core::Fx*                                            m_details_fx;
	DrawSpace::Core::Fx*                                            m_climate_fx;
	DrawSpace::Core::Fx*                                            m_atmo_fx;

	DrawSpace::Core::Shader*                                        m_climate_vshader;
	DrawSpace::Core::Shader*                                        m_climate_pshader;

    DrawSpace::Core::Shader*                                        m_planet_vshader;
    DrawSpace::Core::Shader*                                        m_planet_pshader;

    DrawSpace::Core::Shader*                                        m_atmo_vshader;
    DrawSpace::Core::Shader*                                        m_atmo_pshader;

	DrawSpace::Core::Texture*										m_texture_th_pixels;
	DrawSpace::Core::Texture*										m_texture_th_splatting;

    DrawSpace::SphericalLOD::Root*                                  m_planet_root;
    DrawSpace::Core::SceneNode<DrawSpace::SphericalLOD::Root>*      m_planet_node;

	PlanetSceneNodeConfig*											m_node_config;

    DrawSpace::Dynamics::Calendar*                                  m_calendar;
    DrawSpace::Core::SceneNodeGraph*                                m_scenenodegraph;

    std::vector<LODDependantNodeInfoStateHandler*>                  m_LODDependantNodeInfosHandlers;

    std::map<dsstring, PlanetLODDependantNodeInfos>                 m_LODdependant_nodeinfos;

    bool                                                            m_renderAtmo;

    void zbuffer_control_from_viewer_alt( void );
    void update_binders( void );
    void update_lod_dep_nodes_infos( void );

    void update_atmolayout_node_visibility( void );

public:

    PlanetInstance( void );
    ~PlanetInstance( void );

    void Init( PlanetSceneNodeConfig* p_planet_config, DrawSpace::Interface::Renderer* p_renderer,
                    DrawSpace::Utils::TimeManager* p_tm, DrawSpace::IntermediatePass* p_ipass,
                    DrawSpace::Dynamics::Calendar* p_calendar, DrawSpace::Core::SceneNodeGraph* p_scenenodegraph,
                    LODDependantNodeInfoStateHandler* p_LODDepNodeInfoStateHandler );

    void Run( void );

    void Release( void );

    DrawSpace::Core::SceneNode<DrawSpace::SphericalLOD::Root>* GetSceneNode( void );

    void RegisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph );
    void UnregisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph );

	void OnGravityEnabledUpdate( bool p_value );
    void OnLandscapeBumpFactorUpdate( dsreal p_factor );
    void OnBeachLimitUpdate( dsreal p_limit );
    void OnAtmoRenderEnableUpdate( bool p_value );
    void OnAtmoKrUpdate( dsreal p_kr );
    void OnGroundFogAltLimit( dsreal p_groundFogAltLimit );
    void OnGroundFogDensity( dsreal p_groundFogDensity );

    void AddLODDependantNodeInfosKeyLinkTable( const dsstring& p_dependantNodeId, DrawSpace::Module::KeysLinkTable* p_keytable );
};

#endif