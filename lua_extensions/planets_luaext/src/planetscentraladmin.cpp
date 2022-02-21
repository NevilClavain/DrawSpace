/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2022
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
#include "planetdetailsbinder.h"
#include "planetsrenderingaspectimpl.h"
#include "transformaspect.h"
#include "lod_layer.h"


using namespace DrawSpace::AspectImplementations;
using namespace DrawSpace::Systems;
using namespace DrawSpace::Aspect;

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
        if (DrawSpace::Interface::System::Event::SYSTEM_RUN_BEGIN == p_event)
        {
            for (auto planet_renderer : m_planet_renderers)
            {
                dsstring reflexion_pass{ planet_renderer->GetReflectionPassId() };

                auto flatclouds_binder{ planet_renderer->GetPlanetFlatCloudsBinder() };
                if (flatclouds_binder.count(reflexion_pass))
                {
                    auto clouds_binders_array{ flatclouds_binder.at(reflexion_pass) };
                    for (auto cloud_binder : clouds_binders_array)
                    {
                        cloud_binder->SetMirrorMode(true);
                    }
                }

                auto atmos_binder{ planet_renderer->GetPlanetAtmoBinder() };
                if (atmos_binder.count(reflexion_pass))
                {
                    auto atmo_binders_array{ atmos_binder.at(reflexion_pass) };
                    for (auto atmo_binder : atmo_binders_array)
                    {
                        atmo_binder->SetMirrorMode(true);
                    }
                }
            }
        }
    }
}
