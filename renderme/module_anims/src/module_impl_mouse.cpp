
/* -*-LIC_BEGIN-*- */
/*
*
* MaGE rendering framework
* Emmanuel Chaumont Copyright (c) 2013-2025
*
* This file is part of MaGE.
*
*    MaGE is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    MaGE is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with MaGE.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#include "module_impl.h"
#include <string>

#include "datacloud.h"
#include "aspects.h"
//#include "timemanager.h"
#include "timecontrol.h"

using namespace mage;
using namespace mage::core;


void ModuleImpl::onMouseMove(long p_xm, long p_ym, long p_dx, long p_dy)
{
	if (m_bufferRenderingQueue)
	{
		const auto current_view_entity_id{ m_bufferRenderingQueue->getCurrentView() };

		if ("cameraEntity" == current_view_entity_id)
		{
			const auto tc{ TimeControl::getInstance() };
			if (tc->isReady())
			{

				auto& gblJointEntityNode{ m_entitygraph.node("gblJointEntity") };
				const auto gblJointEntity{ gblJointEntityNode.data() };

				auto& world_aspect{ gblJointEntity->aspectAccess(core::worldAspect::id) };

				double& fps_theta{ world_aspect.getComponent<double>("gbl_theta")->getPurpose() };
				double& fps_phi{ world_aspect.getComponent<double>("gbl_phi")->getPurpose() };

				tc->angleSpeedInc(&fps_theta, -p_dx);
				tc->angleSpeedInc(&fps_phi, -p_dy);
			}
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