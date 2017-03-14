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
    for( auto it = m_nodes.begin(); it != m_nodes.end(); ++it )
    {
        it->second->Run();
    }

	m_tm.Update();
}

void CDLODPlanetService::Release( void )
{
    _DSDEBUG( logger, dsstring("CDLODPlanet service : shutdown...") );
}

DrawSpace::Core::BaseSceneNode* CDLODPlanetService::InstanciateSceneNode( const dsstring& p_sceneNodeName, DrawSpace::Dynamics::Calendar* p_calendar, LODDependantNodeInfoStateHandler* p_handler )
{
    PlanetSceneNodeConfig* pconfig;

	if( m_nodes_config.count( p_sceneNodeName ) )
	{
		// une config de node est associée
		pconfig = &m_nodes_config[p_sceneNodeName];
	}
	else
	{
		_DSEXCEPTION( dsstring( "Node configuration is mandatory for CDLODPlanet node creation" ) );
	}

    PlanetInstance* pe = _DRAWSPACE_NEW_( PlanetInstance, PlanetInstance );

	// pour permettre la mise à jour de parametres "hot"
	m_nodes_config[p_sceneNodeName].SetOwner( pe );


    pe->Init( pconfig, m_renderer, &m_tm, m_texturepass, p_calendar, m_scenenodegraph, p_handler );

    m_nodes[p_sceneNodeName] = pe;
    return pe->GetSceneNode();

}

void CDLODPlanetService::RegisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph )
{
    for( auto it = m_nodes.begin(); it != m_nodes.end(); ++it )
    {
        it->second->RegisterScenegraphCallbacks( *m_scenenodegraph );
    }   
}

void CDLODPlanetService::UnregisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph )
{
    for( auto it = m_nodes.begin(); it != m_nodes.end(); ++it )
    {
        it->second->UnregisterScenegraphCallbacks( *m_scenenodegraph );
    }    
}

void CDLODPlanetService::ReleaseSceneNode( const dsstring& p_sceneNodeName, DrawSpace::Dynamics::Calendar* p_calendar )
{
    if( m_nodes.count( p_sceneNodeName ) )
    {
        PlanetInstance* pe = m_nodes[p_sceneNodeName];

        pe->Release();
        _DRAWSPACE_DELETE_( pe );

        m_nodes.erase( p_sceneNodeName );
    }
}

DrawSpace::Module::KeysLinkTable* CDLODPlanetService::AddSceneNodeConfig( const dsstring& p_sceneNodeName )
{
	return &m_nodes_config[p_sceneNodeName].m_keylinksTable;
}


