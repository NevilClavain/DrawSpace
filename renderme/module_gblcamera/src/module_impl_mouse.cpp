
/* -*-LIC_BEGIN-*- */
/*
*
* renderMe grafx framework
* Emmanuel Chaumont Copyright (c) 2013-2023
*
* This file is part of renderMe.
*
*    renderMe is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    renderMe is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with renderMe.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#include "module_impl.h"
#include <string>

#include "datacloud.h"
#include "aspects.h"
#include "timemanager.h"

using namespace renderMe;
using namespace renderMe::core;


void ModuleImpl::onMouseMove(long p_xm, long p_ym, long p_dx, long p_dy)
{
	const auto dataCloud{ renderMe::rendering::Datacloud::getInstance() };
	const auto current_view_entity_id{ dataCloud->readDataValue<std::string>("std.current_view") };

	if ("Camera01Entity" == current_view_entity_id)
	{
		const auto tm{ TimeManager::getInstance() };
		if (tm->isReady())
		{
			auto& gblJointEntityNode{ m_entitygraph.node("gblJointEntity") };
			const auto gblJointEntity{ gblJointEntityNode.data() };

			auto& world_aspect{ gblJointEntity->aspectAccess(core::worldAspect::id) };

			double& fps_theta{ world_aspect.getComponent<double>("gbl_theta")->getPurpose() };
			double& fps_phi{ world_aspect.getComponent<double>("gbl_phi")->getPurpose() };

			tm->angleSpeedInc(&fps_theta, -p_dx);
			tm->angleSpeedInc(&fps_phi, -p_dy);

		}
	}
}

void ModuleImpl::onMouseWheel(long p_delta)
{
}

void ModuleImpl::onMouseLeftButtonDown(long p_xm, long p_ym)
{
}

void ModuleImpl::onMouseLeftButtonUp(long p_xm, long p_ym)
{
}

void ModuleImpl::onMouseRightButtonDown(long p_xm, long p_ym)
{
}

void ModuleImpl::onMouseRightButtonUp(long p_xm, long p_ym)
{
}