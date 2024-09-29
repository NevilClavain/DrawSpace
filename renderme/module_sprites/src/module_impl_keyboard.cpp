
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
#include <vector>

#include "logger_service.h"
#include "logging.h"
#include "renderingqueuesystem.h"
#include "sysengine.h"
#include "datacloud.h"
#include "aspects.h"
#include "syncvariable.h"
#include "entitygraph_helpers.h"
#include "graphicobjects_helpers.h"

using namespace renderMe;
using namespace renderMe::core;

void ModuleImpl::onKeyPress(long p_key)
{
}

void ModuleImpl::onEndKeyPress(long p_key)
{
	auto& eventsLogger{ services::LoggerSharing::getInstance()->getLogger("Events") };

	if (VK_ESCAPE == p_key)
	{
		_RENDERME_DEBUG(eventsLogger, "EMIT EVENT -> CLOSE_APP");
		for (const auto& call : m_callbacks)
		{
			call(renderMe::interfaces::ModuleEvents::CLOSE_APP, 0);
		}
	}

	else if (VK_F8 == p_key)
	{
		auto renderingQueueSystem{ SystemEngine::getInstance()->getSystem(renderingQueueSystemSlot) };
		auto renderingQueueSystemInstance{ dynamic_cast<renderMe::RenderingQueueSystem*>(renderingQueueSystem) };

		renderingQueueSystemInstance->requestRenderingqueueLogging("screenRenderingEntity");
		renderingQueueSystemInstance->requestRenderingqueueLogging("bufferRenderingEntity");		
	}

	else if (VK_SPACE)
	{
		rendering::RenderState rs_noculling(rendering::RenderState::Operation::SETCULLING, "cw");
		rendering::RenderState rs_zbuffer(rendering::RenderState::Operation::ENABLEZBUFFER, "false");
		rendering::RenderState rs_fill(rendering::RenderState::Operation::SETFILLMODE, "solid");
		rendering::RenderState rs_texturepointsampling(rendering::RenderState::Operation::SETTEXTUREFILTERTYPE, "point");

		const std::vector<rendering::RenderState> rs_list = { rs_noculling, rs_zbuffer, rs_fill, rs_texturepointsampling };

		auto sprite{ helpers::plug2DSpriteWithSyncVariables(m_entitygraph, "bufferRenderingEntity", "sprite#" + std::to_string(m_sprites.size()), 0.05, 0.05, "sprite_vs", "sprite_ps", "tennis_ball.bmp", rs_list, 1000) };
		m_sprites.push_back(sprite);
	}
}

void ModuleImpl::onKeyPulse(long p_key)
{
}

void ModuleImpl::onChar(long p_char, long p_scan)
{
}