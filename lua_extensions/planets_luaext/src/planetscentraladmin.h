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

#pragma once

#include "drawspace_commons.h"
#include "crtp_singleton.h"
#include "hub.h"

class PlanetsRenderingAspectImpl;

class PlanetsCentralAdmin : public DrawSpace::Utils::BaseSingleton<PlanetsCentralAdmin>
{
private:

	using SystemsEvtCb = DrawSpace::Core::CallBack2<PlanetsCentralAdmin, void, DrawSpace::Interface::System::Event, dsstring>;

	DrawSpace::Systems::Hub*					m_hub{ nullptr };
	SystemsEvtCb								m_system_evt_cb;
	std::set<PlanetsRenderingAspectImpl*>		m_planet_renderers;

	void on_system_event(DrawSpace::Interface::System::Event p_event, dsstring p_id);
	void check_is_planet_relative(PlanetsRenderingAspectImpl* planet_renderer);

public:
	PlanetsCentralAdmin(void);

	void Register(PlanetsRenderingAspectImpl* p_planet, DrawSpace::Systems::Hub* p_hub);
	void Unregister(PlanetsRenderingAspectImpl* p_planet);

};

