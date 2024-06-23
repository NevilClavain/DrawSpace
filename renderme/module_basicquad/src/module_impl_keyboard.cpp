
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

#include "logger_service.h"
#include "logging.h"
#include "renderingqueuesystem.h"
#include "sysengine.h"
#include "datacloud.h"
#include "aspects.h"
#include "syncvariable.h"

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

		_RENDERME_DEBUG(eventsLogger, "EMIT EVENT -> MOUSE_DISPLAY_CHANGED");
		for (const auto& call : m_callbacks)
		{
			call(renderMe::interfaces::ModuleEvents::MOUSE_DISPLAY_CHANGED, (int)m_show_mouse_cursor);
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

		_RENDERME_DEBUG(eventsLogger, "EMIT EVENT -> MOUSE_CIRCULARMODE_CHANGED");
		for (const auto& call : m_callbacks)
		{
			call(renderMe::interfaces::ModuleEvents::MOUSE_CIRCULARMODE_CHANGED, (int)m_mouse_circular_mode);
		}
	}
	else if (VK_F3 == p_key)
	{
		if (true == m_quadEntity_state_request)
		{
			m_quadEntity_state_request = false;
		}
		else
		{
			m_quadEntity_state_request = true;
		}
	}

	else if (VK_F8 == p_key)
	{
		auto renderingQueueSystem{ SystemEngine::getInstance()->getSystem(renderingQueueSystemSlot) };
		auto renderingQueueSystemInstance{ dynamic_cast<renderMe::RenderingQueueSystem*>(renderingQueueSystem) };

		renderingQueueSystemInstance->requestRenderingqueueLogging("screenRenderingEntity");
	}
}

void ModuleImpl::onKeyPulse(long p_key)
{
}

void ModuleImpl::onChar(long p_char, long p_scan)
{
}