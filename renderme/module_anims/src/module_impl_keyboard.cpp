
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

#include "logger_service.h"
#include "logging.h"
#include "renderingqueuesystem.h"
#include "sysengine.h"
#include "datacloud.h"
#include "aspects.h"
#include "syncvariable.h"
#include "entitygraph_helpers.h"
#include "timecontrol.h"

using namespace mage;
using namespace mage::core;

void ModuleImpl::onKeyPress(long p_key)
{
	if (m_bufferRenderingQueue)
	{
		const auto current_view_entity_id{ m_bufferRenderingQueue->getCurrentView() };

		if ('Q' == p_key)
		{
			if ("cameraEntity" == current_view_entity_id)
			{
				auto& gblJointEntityNode{ m_entitygraph.node("gblJointEntity") };
				const auto gblJointEntity{ gblJointEntityNode.data() };

				auto& world_aspect{ gblJointEntity->aspectAccess(core::worldAspect::id) };

				double& speed{ world_aspect.getComponent<double>("gbl_speed")->getPurpose() };

				speed = 0.1;
			}
		}
		else if ('W' == p_key)
		{
			if ("cameraEntity" == current_view_entity_id)
			{
				auto& gblJointEntityNode{ m_entitygraph.node("gblJointEntity") };
				const auto gblJointEntity{ gblJointEntityNode.data() };

				auto& world_aspect{ gblJointEntity->aspectAccess(core::worldAspect::id) };

				double& speed{ world_aspect.getComponent<double>("gbl_speed")->getPurpose() };

				speed = -0.1;
			}
		}
	}
}

void ModuleImpl::onEndKeyPress(long p_key)
{
	auto& eventsLogger{ services::LoggerSharing::getInstance()->getLogger("Events") };

	if (m_bufferRenderingQueue)
	{

		const auto current_view_entity_id{ m_bufferRenderingQueue->getCurrentView() };

		if (VK_ESCAPE == p_key)
		{
			_MAGE_DEBUG(eventsLogger, "EMIT EVENT -> CLOSE_APP");
			for (const auto& call : m_callbacks)
			{
				call(mage::interfaces::ModuleEvents::CLOSE_APP, 0);
			}
		}
		else if (VK_F1 == p_key)
		{
			if (m_show_mouse_cursor)
			{
				m_show_mouse_cursor = false;
			}
			else
			{
				m_show_mouse_cursor = true;
			}

			_MAGE_DEBUG(eventsLogger, "EMIT EVENT -> MOUSE_DISPLAY_CHANGED");
			for (const auto& call : m_callbacks)
			{
				call(mage::interfaces::ModuleEvents::MOUSE_DISPLAY_CHANGED, (int)m_show_mouse_cursor);
			}
		}
		else if (VK_F2 == p_key)
		{
			if (m_mouse_circular_mode)
			{
				m_mouse_circular_mode = false;
			}
			else
			{
				m_mouse_circular_mode = true;
			}

			_MAGE_DEBUG(eventsLogger, "EMIT EVENT -> MOUSE_CIRCULARMODE_CHANGED");
			for (const auto& call : m_callbacks)
			{
				call(mage::interfaces::ModuleEvents::MOUSE_CIRCULARMODE_CHANGED, (int)m_mouse_circular_mode);
			}
		}

		else if (VK_F3 == p_key)
		{
			// play animation

			auto& raptorEntityNode{ m_entitygraph.node("raptorEntity") };
			const auto raptorEntity{ raptorEntityNode.data() };
			auto& anims_aspect{ raptorEntity->aspectAccess(core::animationsAspect::id) };
			auto& animationsIdList{ anims_aspect.getComponent<std::list<std::string>>("eg.std.animationsIdList")->getPurpose()};

			animationsIdList.push_back("eat");			
		}

		else if (VK_F4 == p_key)
		{
			// play animation

			auto& raptorEntityNode{ m_entitygraph.node("raptorEntity") };
			const auto raptorEntity{ raptorEntityNode.data() };
			auto& anims_aspect{ raptorEntity->aspectAccess(core::animationsAspect::id) };
			auto& animationsIdList{ anims_aspect.getComponent<std::list<std::string>>("eg.std.animationsIdList")->getPurpose() };

			animationsIdList.push_back("walk");
		}

		else if (VK_F5 == p_key)
		{
			// play animation

			auto& raptorEntityNode{ m_entitygraph.node("raptorEntity") };
			const auto raptorEntity{ raptorEntityNode.data() };
			auto& anims_aspect{ raptorEntity->aspectAccess(core::animationsAspect::id) };
			auto& animationsIdList{ anims_aspect.getComponent<std::list<std::string>>("eg.std.animationsIdList")->getPurpose() };

			animationsIdList.push_back("idle");
		}

		else if (VK_F6 == p_key)
		{
			// play animation

			auto& raptorEntityNode{ m_entitygraph.node("raptorEntity") };
			const auto raptorEntity{ raptorEntityNode.data() };
			auto& anims_aspect{ raptorEntity->aspectAccess(core::animationsAspect::id) };
			auto& animationsIdList{ anims_aspect.getComponent<std::list<std::string>>("eg.std.animationsIdList")->getPurpose() };

			animationsIdList.push_back("Alert");
		}

		else if (VK_F7 == p_key)
		{
			auto tc{ TimeControl::getInstance() };

			auto mode{ tc->getTimeFactor() };
			if (TimeControl::TimeScale::DIV4_TIME == mode)
			{
				tc->setTimeFactor(TimeControl::TimeScale::NORMAL_TIME);
			}
			else if (TimeControl::TimeScale::NORMAL_TIME == mode)
			{
				tc->setTimeFactor(TimeControl::TimeScale::DIV4_TIME);
			}
		}

		else if (VK_F8 == p_key)
		{
			auto renderingQueueSystem{ SystemEngine::getInstance()->getSystem(renderingQueueSystemSlot) };
			auto renderingQueueSystemInstance{ dynamic_cast<mage::RenderingQueueSystem*>(renderingQueueSystem) };

			renderingQueueSystemInstance->requestRenderingqueueLogging("bufferRenderingEntity");
		}

		else if ('Q' == p_key)
		{
			if ("cameraEntity" == current_view_entity_id)
			{
				auto& gblJointEntityNode{ m_entitygraph.node("gblJointEntity") };
				const auto gblJointEntity{ gblJointEntityNode.data() };

				auto& world_aspect{ gblJointEntity->aspectAccess(core::worldAspect::id) };

				double& speed{ world_aspect.getComponent<double>("gbl_speed")->getPurpose() };

				speed = 0.0;
			}
		}

		else if ('W' == p_key)
		{
			if ("cameraEntity" == current_view_entity_id)
			{
				auto& gblJointEntityNode{ m_entitygraph.node("gblJointEntity") };
				const auto gblJointEntity{ gblJointEntityNode.data() };

				auto& world_aspect{ gblJointEntity->aspectAccess(core::worldAspect::id) };

				double& speed{ world_aspect.getComponent<double>("gbl_speed")->getPurpose() };

				speed = 0.0;
			}
		}
	}
}

void ModuleImpl::onKeyPulse(long p_key)
{
}

void ModuleImpl::onChar(long p_char, long p_scan)
{
}