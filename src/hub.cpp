/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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
/* -*-LIC_END-*- */

#include "hub.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Systems;
using namespace DrawSpace::Aspect;


Hub::Hub( void ) :
m_resourcesSystem(m_runnerSystem)
{
    // attention ! l'ordre est important ! ( par ex. time system doit etre execute avant tt les autres!)
    
    m_systems.push_back( &m_timeSystem );
    m_systems.push_back( &m_resourcesSystem );
    m_systems.push_back( &m_physicsSystem );
    m_systems.push_back( &m_transformSystem );
    m_systems.push_back( &m_animationsSystem );
    m_systems.push_back( &m_renderingSystem );
    m_systems.push_back( &m_runnerSystem );
    m_systems.push_back( &m_traceSystem );    
}

void Hub::run( EntityGraph::EntityNodeGraph* p_entitygraph )
{
    /// signifier debut run des systemes
    for( auto it = m_systems_update_evt_handlers.begin(); it != m_systems_update_evt_handlers.end(); ++it )
    {
        (**it)( SYSTEMS_UPDATE_BEGIN );
    }
    ///////

    p_entitygraph->ProcessSignals();

    for( size_t i = 0; i < m_systems.size(); i++ )
    {
        m_systems[i]->Run( p_entitygraph );
    }

    /// signifier fin run des systemes
    for( auto it = m_systems_update_evt_handlers.begin(); it != m_systems_update_evt_handlers.end(); ++it )
    {
        (**it)( SYSTEMS_UPDATE_END );
    }
    ///////
}

void Hub::EnableGUI( bool p_state )
{
    m_renderingSystem.EnableGUI( p_state );
}


void Hub::RegisterSystemsUpdateEvtHandler( SystemsUpdateEventHandler* p_handler )
{
    m_systems_update_evt_handlers.insert( p_handler );
}

void Hub::UnregisterSystemsUpdateEvtHandler( SystemsUpdateEventHandler* p_handler )
{
    m_systems_update_evt_handlers.erase( p_handler );
}

void Hub::SetLogConf( DrawSpace::Logger::Configuration* p_logconf )
{
    m_traceSystem.SetLogConf( p_logconf );
}

std::vector<DrawSpace::Interface::System*> Hub::GetSystems(void) const
{
    return m_systems;
}

void Hub::ReleaseAssets(void)
{
    m_resourcesSystem.ReleaseAssets();
}

void Hub::StartupRunner(void)
{
    m_runnerSystem.StartupRunner();
}

void Hub::ShutdownRunner(void)
{
    m_runnerSystem.ShutdownRunner();
}

