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

#pragma once

#include "transformsystem.h"
#include "timesystem.h"
#include "physicssystem.h"
#include "rendersystem.h"
#include "tracesystem.h"
#include "resourcessystem.h"
#include "animationssystem.h"
#include "runnersystem.h"

#include "task.h"
#include "mailbox.h"

namespace DrawSpace
{
namespace Systems
{

class Hub : public Interface::System
{
public:
    enum SystemsUpdateEvent
    {
        SYSTEMS_UPDATE_BEGIN,
        SYSTEMS_UPDATE_END
    };

    using SystemsUpdateEventHandler = DrawSpace::Core::BaseCallback<void, SystemsUpdateEvent>;
    
private:
    RenderingSystem                        m_renderingSystem;
    TransformSystem                        m_transformSystem;
    PhysicsSystem                          m_physicsSystem;
    TimeSystem                             m_timeSystem;
    TraceSystem                            m_traceSystem;
    ResourcesSystem                        m_resourcesSystem;
    AnimationsSystem                       m_animationsSystem;
    RunnerSystem                           m_runnerSystem;

    std::vector<DrawSpace::Interface::System*>      m_systems;
    std::set<SystemsUpdateEventHandler*>            m_systems_update_evt_handlers;
   
protected:

    void run(EntityGraph::EntityNodeGraph* p_entitygraph);

public:
    Hub( void );

    void SetLogConf( DrawSpace::Logger::Configuration* p_logconf );

    dsstring GetSystemId(void) const { return "HubSystem"; };
    
	template<typename T>
	T& GetSystem(const dsstring& p_id) const
	{
		for (const auto& e : m_systems)
		{
			if (e->GetSystemId() == p_id)
			{
				try
				{
					return dynamic_cast<T&>(*e);
				}
				catch (std::bad_cast& p_bc)
				{
					_DSEXCEPTION("bad cast : " << dsstring( p_bc.what() ) << " " << p_id.c_str());
				}
			}
		}

        _DSEXCEPTION("Unknown systrm id ");
	}

    void EnableGUI( bool p_state );

    void RegisterSystemsUpdateEvtHandler( SystemsUpdateEventHandler* p_handler );
    void UnregisterSystemsUpdateEvtHandler( SystemsUpdateEventHandler* p_handler );

    std::vector<DrawSpace::Interface::System*> GetSystems( void ) const;

    void ReleaseAssets( void );  

    void StartupRunner(void);
    void ShutdownRunner(void);
};
}
}


