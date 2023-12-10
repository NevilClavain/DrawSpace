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
#include "timesystem.h"
#include "resourcesystem.h"
#include "shader.h"

#include "filesystem.h"
#include "logconf.h"

#include "entity.h"


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

	renderMe::core::Json jsonParser;
	jsonParser.registerSubscriber(logger::Configuration::getInstance()->getCallback());

	const auto logParseStatus{ jsonParser.parse(data) };

	if (logParseStatus < 0)
	{
		_EXCEPTION("Cannot parse logging configuration")
	}

	/////////// systems

	auto sysEngine{ SystemEngine::getInstance() };

	sysEngine->makeSystem<renderMe::TimeSystem>(0, m_entitygraph);
	sysEngine->makeSystem<renderMe::D3D11System>(1, m_entitygraph);
	sysEngine->makeSystem<renderMe::ResourceSystem>(2, m_entitygraph);

	// D3D11 system provides compilation shader service

	renderMe::D3D11System* d3d11System{ sysEngine->getSystem<renderMe::D3D11System>(1) };
	services::ShadersCompilationService::getInstance()->registerSubscriber(d3d11System->getCallback());

	// register to resource system events

	const ResourceSystem::Callback cb
	{
		[&, this](ResourceSystemEvent p_event, const std::string& p_resourceName)
		{
			switch (p_event)
			{
				case ResourceSystemEvent::RESOURCE_SHADER_CACHE_CREATED:
					m_resources_event = "Shader cache creation : " + p_resourceName;
					break;

				case ResourceSystemEvent::RESOURCE_SHADER_COMPILATION_BEGIN:
					m_resources_event = "Shader compilation: " + p_resourceName + " BEGIN";
					break;

				case ResourceSystemEvent::RESOURCE_SHADER_COMPILATION_SUCCESS:
					m_resources_event = "Shader compilation " + p_resourceName + " SUCCESS";
					break;

				case ResourceSystemEvent::RESOURCE_SHADER_COMPILATION_ERROR:
					m_resources_event = "Shader compilation " + p_resourceName + " ERROR";
					break;
			}
		}
	};

	renderMe::ResourceSystem* resourceSystem{ sysEngine->getSystem<renderMe::ResourceSystem>(2) };
	resourceSystem->registerSubscriber(cb);

	
	//////////////////////////

	createEntities(p_appWindowsEntityName);
}

void RootImpl::run(void)
{
	/////////////////////////////////////////////////////

	auto sysEngine{ SystemEngine::getInstance() };
	sysEngine->run();

	/////////////////////////////////////////////////////

	const auto currentFPS { m_timeInfos_time_aspect->getComponent<int>("framePerSeconds")->getPurpose() };
	const std::string fpsText { std::string("fps = ") + std::to_string(currentFPS) };;
	m_windowRenderingQueue->setText(4, { fpsText, "Courier New", { 255, 255, 255, 255 }, { 5, 5 }, 12.0 });

	// resources system event
	m_windowRenderingQueue->setText(5, { m_resources_event, "Courier New", {255, 255, 255, 255}, {5, 25}, 12.0});
}

void RootImpl::close(void)
{
	auto resourceSystem{ SystemEngine::getInstance()->getSystem(2) };
	auto resourceSystemInstance{ dynamic_cast<renderMe::ResourceSystem*>(resourceSystem) };

	resourceSystemInstance->killRunner();
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RootImpl::createEntities(const std::string p_appWindowsEntityName)
{
	/////////// add screen rendering pass entity

	auto& appwindowNode{ m_entitygraph.node(p_appWindowsEntityName) };

	auto& screenRenderingPassNode{ m_entitygraph.add(appwindowNode, "screenRenderingEntity") };
	const auto screenRenderingPassEntity{ screenRenderingPassNode.data() };

	auto& screenRendering_rendering_aspect{ screenRenderingPassEntity->makeAspect(core::renderingAspect::id) };

	screenRendering_rendering_aspect.addComponent<rendering::Queue>("renderingQueue", "final_pass");
	
	auto& rendering_queue{ screenRendering_rendering_aspect.getComponent<rendering::Queue>("renderingQueue")->getPurpose() };
	rendering_queue.setTargetClearColor({ 0, 0, 64, 255 });
	rendering_queue.enableTargetClearing(true);

	rendering_queue.setText(0, { "Small text", "Bahnschrift", { 255, 0, 255, 255 }, { 10, 100 }, 10.0 });
	rendering_queue.setText(1, { "Medium text", "Bahnschrift", { 255, 0, 255, 255 }, { 10, 150 }, 20.0 });
	rendering_queue.setText(2, { "Large text", "Bahnschrift", { 255, 0, 255, 255 }, { 10, 200 }, 40.0 });


	rendering_queue.setText(3, { "Hello world !", "Bahnschrift", { 255, 255, 255, 255 }, { 400, 10 }, 20.0 });

	m_windowRenderingQueue = &rendering_queue;
	
	/////////////// add time management infos entity

	auto& timeInfosNode{ m_entitygraph.add(appwindowNode, "timeInfosEntity") };

	const auto timeInfosEntity{ timeInfosNode.data() };

	auto& timeInfos_time_aspect{ timeInfosEntity->makeAspect(core::timeAspect::id) };
	timeInfos_time_aspect.addComponent<int>("framePerSeconds", -1); // will be updated by time system

	m_timeInfos_time_aspect = &timeInfos_time_aspect;

	//////////////////////////////////////////////////////
	
	auto& circleNode{ m_entitygraph.add(screenRenderingPassNode, "circleEntity") };
	const auto circleEntity{ circleNode.data() };
	auto& circle_resource_aspect{ circleEntity->makeAspect(core::resourcesAspect::id) };

	const std::vector<Shader> vertex_shaders =
	{
		Shader("color_vs.hlsl")
	};

	const std::vector<Shader> pixel_shaders =
	{
		Shader("color_ps.hlsl")
	};

	circle_resource_aspect.addComponent<std::vector<Shader>>("vertexShaders", vertex_shaders);
	circle_resource_aspect.addComponent<std::vector<Shader>>("pixelShaders", pixel_shaders);

}
