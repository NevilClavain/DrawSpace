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
#include "renderingqueuesystem.h"
#include "linemeshelistdrawing.h"

#include "shader.h"
#include "linemeshe.h"
#include "renderstate.h"

#include "logger_service.h"

#include "filesystem.h"
#include "logconf.h"

#include "entity.h"


using namespace renderMe;
using namespace renderMe::core;


RootImpl::RootImpl()
{
	/////////// create common specific logger for events
	services::LoggerSharing::getInstance()->createLogger("Events");

}

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
	auto& eventsLogger{ services::LoggerSharing::getInstance()->getLogger("Events") };

	if (VK_SPACE == p_key)
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
		if (m_draw_circle)
		{
			m_draw_circle = false;

			m_entitygraph.remove(m_entitygraph.node("circleEntity"));
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
	sysEngine->makeSystem<renderMe::RenderingQueueSystem>(3, m_entitygraph);

	// D3D11 system provides compilation shader service

	renderMe::D3D11System* d3d11System{ sysEngine->getSystem<renderMe::D3D11System>(1) };
	services::ShadersCompilationService::getInstance()->registerSubscriber(d3d11System->getServiceInvocationCallback());

	// register to resource system events

	const ResourceSystem::Callback cb
	{
		[&, this](ResourceSystemEvent p_event, const std::string& p_resourceName)
		{
			auto& eventsLogger{ services::LoggerSharing::getInstance()->getLogger("Events") };

			switch (p_event)
			{
				case ResourceSystemEvent::RESOURCE_SHADER_CACHE_CREATED:
					_RENDERME_DEBUG(eventsLogger, "RECV EVENT -> RESOURCE_SHADER_CACHE_CREATED : " + p_resourceName);
					m_resources_event = "Shader cache creation : " + p_resourceName;
					break;

				case ResourceSystemEvent::RESOURCE_SHADER_COMPILATION_BEGIN:
					_RENDERME_DEBUG(eventsLogger, "RECV EVENT -> RESOURCE_SHADER_COMPILATION_BEGIN : " + p_resourceName);
					m_resources_event = "Shader compilation: " + p_resourceName + " BEGIN";
					break;

				case ResourceSystemEvent::RESOURCE_SHADER_COMPILATION_SUCCESS:
					_RENDERME_DEBUG(eventsLogger, "RECV EVENT -> RESOURCE_SHADER_COMPILATION_SUCCESS : " + p_resourceName);
					m_resources_event = "Shader compilation " + p_resourceName + " SUCCESS";
					break;

				case ResourceSystemEvent::RESOURCE_SHADER_COMPILATION_ERROR:
					_RENDERME_DEBUG(eventsLogger, "RECV EVENT -> RESOURCE_SHADER_COMPILATION_ERROR : " + p_resourceName);
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

	m_windowRenderingQueue->setText(4, { fpsText, "CourierNew.10.spritefont", { 255, 0, 0, 255 }, { 0, 0 }, 0.0f });

	// resources system event
	m_windowRenderingQueue->setText(5, { m_resources_event, "CourierNew.10.spritefont", {255, 255, 255, 255}, {0, 25}, 0.0});
}

void RootImpl::close(void)
{
	auto resourceSystem{ SystemEngine::getInstance()->getSystem(2) };
	auto resourceSystemInstance{ dynamic_cast<renderMe::ResourceSystem*>(resourceSystem) };

	resourceSystemInstance->killRunner();

	auto d3d11System{ SystemEngine::getInstance()->getSystem(1) };
	auto d3d11SystemInstance{ dynamic_cast<renderMe::D3D11System*>(d3d11System) };

	d3d11SystemInstance->killRunner();
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

	rendering_queue.setText(3, { "Hello world !", "Bahnschrift.16.spritefont", { 0, 255, 0, 255 }, { 400, 10 }, 0.0 });
	

	m_windowRenderingQueue = &rendering_queue;
	
	/////////////// add time management infos entity

	auto& timeInfosNode{ m_entitygraph.add(appwindowNode, "timeInfosEntity") };

	const auto timeInfosEntity{ timeInfosNode.data() };

	auto& timeInfos_time_aspect{ timeInfosEntity->makeAspect(core::timeAspect::id) };
	timeInfos_time_aspect.addComponent<int>("framePerSeconds", -1); // will be updated by time system

	m_timeInfos_time_aspect = &timeInfos_time_aspect;

	//////////////////////////////////////////////////////

	if (m_draw_circle)
	{
		auto& circleNode{ m_entitygraph.add(screenRenderingPassNode, "circleEntity") };
		const auto circleEntity{ circleNode.data() };
		auto& circle_resource_aspect{ circleEntity->makeAspect(core::resourcesAspect::id) };

		/////////// Add shaders

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

		/////////// Add linemeshe

		LineMeshe square("square", LineMeshe::State::BLOBLOADED);

		square.push(Vertex(-0.5, -0.5, 0.0));
		square.push(Vertex(0.5, -0.5, 0.0));
		square.push(Vertex(0.5, 0.5, 0.0));
		square.push(Vertex(-0.5, 0.5, 0.0));

		square.push({ 0, 1 });
		square.push({ 1, 2 });
		square.push({ 2, 3 });
		square.push({ 3, 0 });

		const std::vector<LineMeshe> line_meshes = { square };

		circle_resource_aspect.addComponent<LineMeshe>("lineMeshe", square);

		/////////// Add renderstate

		RenderState rs_nocullling(RenderState::Operation::SETCULLING, "none");		
		const std::vector<RenderState> rs_list = { rs_nocullling };

		circle_resource_aspect.addComponent<std::vector<RenderState>>("renderStates", rs_list);

		/////////// Add rendering method : simple line Meshes list here
		circle_resource_aspect.addComponent<rendering::LineMesheListDrawing>("lineMesheDrawing", rendering::LineMesheListDrawing({ "square" }));
	}
	
}
