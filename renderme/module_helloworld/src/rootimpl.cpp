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

#include "rootimpl.h"
#include "aspects.h"
#include "sysengine.h"
#include "d3d11system.h"
#include "filesystem.h"
#include "logconf.h"

#include "renderingqueue.h"


using namespace renderMe;
using namespace renderMe::core;

std::string RootImpl::getModuleName() const
{
	return "Hello World";
}


std::string RootImpl::getModuleDescr() const
{
	return "Hello World module example";
}

renderMe::core::Entitygraph* RootImpl::entitygraph()
{
	return &m_entitygraph;
}

void RootImpl::onKeyPress(long p_key)
{
}

void RootImpl::onEndKeyPress(long p_key)
{
	if (VK_SPACE == p_key)
	{
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

		for (const auto& call : m_callbacks)
		{
			call(renderMe::interfaces::ModuleEvents::MOUSE_CIRCULARMODE_CHANGED, (int)m_mouse_circular_mode);
		}
	}
}

void RootImpl::onKeyPulse(long p_key)
{
}

void RootImpl::onChar(long p_char, long p_scan)
{
}

void RootImpl::onMouseMove(long p_xm, long p_ym, long p_dx, long p_dy)
{
}

void RootImpl::onMouseWheel(long p_delta)
{
}

void RootImpl::onMouseLeftButtonDown(long p_xm, long p_ym)
{
}

void RootImpl::onMouseLeftButtonUp(long p_xm, long p_ym)
{
}

void RootImpl::onMouseRightButtonDown(long p_xm, long p_ym)
{
}

void RootImpl::onMouseRightButtonUp(long p_xm, long p_ym)
{
}

void RootImpl::onAppEvent(WPARAM p_wParam, LPARAM p_lParam)
{
}

void RootImpl::init(const std::string p_appWindowsEntityName)
{
	/////////// logging conf

	renderMe::core::FileContent<char> logConfFileContent("./module_helloworld_config/logconf.json");
	logConfFileContent.load();

	const auto dataSize{ logConfFileContent.getDataSize() };
	const std::string data(logConfFileContent.getData(), dataSize);

	/////////// set static to spare some space on stack // compiler message
	static renderMe::core::Json jsonParser;
	jsonParser.registerSubscriber(logger::Configuration::getInstance()->getCallback());

	const auto logParseStatus{ jsonParser.parse(data) };

	if (logParseStatus < 0)
	{
		_EXCEPTION("Cannot parse logging configuration")
	}

	/////////// systems

	auto sysEngine{ SystemEngine::getInstance() };
	sysEngine->makeSystem<renderMe::D3D11System>(renderMe::d3d11SystemExecutionSlot, m_entitygraph);

	//////////////////////////

	createEntities(p_appWindowsEntityName);
}

void RootImpl::run(void)
{
	auto sysEngine{ SystemEngine::getInstance() };
	sysEngine->run();
}

void RootImpl::createEntities(const std::string p_appWindowsEntityName)
{
	/////////// add screen rendering pass entity

	auto& appwindow_node{ m_entitygraph.node(p_appWindowsEntityName) };
	auto screenRenderingPassNode{ m_entitygraph.add(appwindow_node, "screenRenderingPassNode") };

	const auto screenRenderingPassEntity{ screenRenderingPassNode.data() };
	auto& rendering_aspect{ screenRenderingPassEntity->makeAspect(core::renderingAspect::id) };

	rendering_aspect.addComponent<rendering::Queue>("renderingQueue", "final_pass");

	auto& rendering_queue{ rendering_aspect.getComponent<rendering::Queue>("renderingQueue")->getPurpose() };

	rendering_queue.setTargetClearColor({ 0, 0, 64, 255 });
	rendering_queue.enableTargetClearing(true);
}

void RootImpl::registerSubscriber(const Callback& p_callback)
{
	renderMe::property::EventSource<renderMe::interfaces::ModuleEvents, int>::registerSubscriber(p_callback);

	// send immediately m_show_mouse_cursor value
	for (const auto& call : m_callbacks)
	{
		call(renderMe::interfaces::ModuleEvents::MOUSE_DISPLAY_CHANGED, (int)m_show_mouse_cursor);
	}

	// send immediately m_mouse_circular_mode value
	for (const auto& call : m_callbacks)
	{
		call(renderMe::interfaces::ModuleEvents::MOUSE_CIRCULARMODE_CHANGED, (int)m_mouse_circular_mode);
	}

}