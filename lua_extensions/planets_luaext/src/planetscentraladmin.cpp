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

#include "planetscentraladmin.h"
#include "planetsrenderingaspectimpl.h"
#include "transformaspect.h"
#include "lod_layer.h"
#include "lod_binder.h"
#include "vector.h"


using namespace DrawSpace::AspectImplementations;
using namespace DrawSpace::Systems;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

extern DrawSpace::Logger::Sink planet_logger;
extern DrawSpace::Logger::Sink planetlayer_logger;
extern DrawSpace::Logger::Sink planetdrawing_logger;

PlanetsCentralAdmin::PlanetsCentralAdmin(void):
m_system_evt_cb(this, &PlanetsCentralAdmin::on_system_event)
{
}

void PlanetsCentralAdmin::Register(PlanetsRenderingAspectImpl* p_planet, DrawSpace::Systems::Hub* p_hub)
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
        const auto systems{ m_hub->GetSystems() };
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
                const auto reflexion_pass{ planet_renderer->GetReflectionPassId() };

                auto flatclouds_binder_2{ planet_renderer->GetPlanetFlatCloudsBinder2() };
                if (flatclouds_binder_2.count(reflexion_pass))
                {
                    auto clouds_binders_array{ flatclouds_binder_2.at(reflexion_pass) };
                    for (auto cloud_binder : clouds_binders_array)
                    {
                        auto mirror_flag{ cloud_binder->GetShaderFeederValue(ShaderType::VERTEX_SHADER, 61) };

                        // enable mirror flag for this pass
                        mirror_flag[0] = 1.0;
                        *cloud_binder << LOD::ShaderFeeder(ShaderType::VERTEX_SHADER, 61, mirror_flag);
                    }
                }

                auto atmos_binder_2{ planet_renderer->GetPlanetAtmoBinder2() };
                if (atmos_binder_2.count(reflexion_pass))
                {
                    auto atmo_binders_array{ atmos_binder_2.at(reflexion_pass) };
                    for (auto atmo_binder : atmo_binders_array)
                    {
                        auto mirror_flag{ atmo_binder->GetShaderFeederValue(ShaderType::VERTEX_SHADER, 61) };

                        // enable mirror flag for this pass
                        mirror_flag[0] = 1.0;
                        *atmo_binder << LOD::ShaderFeeder(ShaderType::VERTEX_SHADER, 61, mirror_flag);
                    }
                }
            }
        }
    }
}

void PlanetsCentralAdmin::SetLogconf(DrawSpace::Logger::Configuration* p_logconf)
{
    m_logconf = p_logconf;

    const auto logconf{ PlanetsCentralAdmin::getInstance()->GetLogconf() };

    logconf->registerSink(&planet_logger);
    planet_logger.SetConfiguration(logconf);

    logconf->registerSink(&planetlayer_logger);
    planetlayer_logger.SetConfiguration(logconf);

    logconf->registerSink(&planetdrawing_logger);
    planetdrawing_logger.SetConfiguration(logconf);
}

DrawSpace::Logger::Configuration* PlanetsCentralAdmin:: GetLogconf(void) const
{
    return m_logconf;
}
