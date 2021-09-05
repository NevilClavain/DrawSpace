/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2021
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

#include "planetscentraladmin.h"
#include "planetsrenderingaspectimpl.h"


using namespace DrawSpace::AspectImplementations;
using namespace DrawSpace::Systems;

PlanetsCentralAdmin::PlanetsCentralAdmin(void):
m_system_evt_cb(this, &PlanetsCentralAdmin::on_system_event)
{

}

void PlanetsCentralAdmin::Register(PlanetsRenderingAspectImpl* p_planet, Systems::Hub* p_hub)
{
    m_planet_renderers.insert(p_planet);

    if (!m_hub)
    {
        m_hub = p_hub;

        std::vector<DrawSpace::Interface::System*> systems = m_hub->GetSystems();
        for (auto& e : systems)
        {
            e->RegisterSystemEvtHandler(&m_system_evt_cb);
        }
    }
}

void PlanetsCentralAdmin::Unregister(PlanetsRenderingAspectImpl* p_planet)
{
    if (m_planet_renderers.count(p_planet))
    {
        m_planet_renderers.erase(p_planet);
    }

    if (m_hub && 0 == m_planet_renderers.size())
    {
        std::vector<DrawSpace::Interface::System*> systems = m_hub->GetSystems();
        for (auto& e : systems)
        {
            e->UnregisterSystemEvtHandler(&m_system_evt_cb);
        }

        m_hub = nullptr;
    }
}


void PlanetsCentralAdmin::on_system_event(DrawSpace::Interface::System::Event p_event, dsstring p_id)
{
    if ("RenderingSystem" == p_id)
    {
        _asm nop
    }
}
