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


#include "shader.h"
#include "linemeshe.h"
#include "renderstate.h"

#include "logger_service.h"

#include "filesystem.h"
#include "logconf.h"

#include "entity.h"

#include "datacloud.h"


using namespace renderMe;
using namespace renderMe::core;
using namespace renderMe::rendering;


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
		
		if (true == m_quadEntity0_state_request)
		{
			m_quadEntity0_state_request = false;
		}
		else
		{
			m_quadEntity0_state_request = true;
		}		
	}
	else if (VK_F4 == p_key)
	{

		if (true == m_quadEntity1_state_request)
		{
			m_quadEntity1_state_request = false;
		}
		else
		{
			m_quadEntity1_state_request = true;
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

	renderMe::core::Json<> jsonParser;
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

	//////////////////////////////////////////////////////
	// 	
	// quadEntity0

	if (true == m_quadEntity0_state_request && false == m_quadEntity0_state)
	{
		// add quadEntity0
	
		const auto dataCloud{ renderMe::rendering::Datacloud::getInstance() };
		dataCloud->registerData<maths::Real4Vector>("quad0_color");

		Entitygraph::Node& screenRenderingPassNode{ m_entitygraph.node("screenRenderingEntity") };

		auto& quadNode{ m_entitygraph.add(screenRenderingPassNode, "quadEntity0") };
		const auto quadEntity{ quadNode.data() };

			
		auto& quad_resource_aspect{ quadEntity->makeAspect(core::resourcesAspect::id) };

		/////////// Add shaders

		quad_resource_aspect.addComponent<Shader>("vertexShader", Shader("color_vs", 0));
		quad_resource_aspect.addComponent<Shader>("pixelShader", Shader("color_ps", 1));

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

		quad_resource_aspect.addComponent<LineMeshe>("square", square);


	
		auto& quad_rendering_aspect{ quadEntity->makeAspect(core::renderingAspect::id) };

		/////////// Add renderstate

		RenderState rs_noculling(RenderState::Operation::SETCULLING, "none");
		const std::vector<RenderState> rs_list = { rs_noculling };

		quad_rendering_aspect.addComponent<std::vector<RenderState>>("renderStates", rs_list);

		/////////// Draw lines

		rendering::LineDrawingControl lineDrawingControl;

		lineDrawingControl.world.translation(0.0, 0.0, -5.0);
		lineDrawingControl.view.identity();
		lineDrawingControl.proj.perspective(1.0, 0.64285713434219360, 1.0, 100000.00000000000);

		lineDrawingControl.pshaders_map.push_back(std::make_pair("quad0_color", "color"));


		quad_rendering_aspect.addComponent<rendering::LineDrawingControl>("squareRendering", lineDrawingControl);




		auto& quad_time_aspect{ quadEntity->makeAspect(core::timeAspect::id) };
		quad_time_aspect.addComponent<TimeManager::Variable>("quad0_color", TimeManager::Variable(TimeManager::Variable::Type::POSITION, 1.0));





		m_quadEntity0_state = true;
	}
	else if (false == m_quadEntity0_state_request && true == m_quadEntity0_state)
	{
		// remove quadEntity0

		const auto dataCloud{ renderMe::rendering::Datacloud::getInstance() };
		dataCloud->removeData<maths::Real4Vector>("quad0_color");


		auto& quadNode{ m_entitygraph.node("quadEntity0") };
		const auto quadEntity{ quadNode.data() };


		////////////////////////////////////

		auto& quad_time_aspect{ quadEntity->aspectAccess(core::timeAspect::id) };

		quad_time_aspect.removeComponent<TimeManager::Variable>("quad0_color");
		quadEntity->removeAspect(core::timeAspect::id);

		////////////////////////////////////

		auto& quad_rendering_aspect{ quadEntity->aspectAccess(core::renderingAspect::id) };

		quad_rendering_aspect.removeComponent<rendering::LineDrawingControl>("squareRendering");
		quad_rendering_aspect.removeComponent<std::vector<RenderState>>("renderStates");

		quadEntity->removeAspect(core::renderingAspect::id);

		////////////////////////////////////

		auto& quad_resource_aspect{ quadEntity->aspectAccess(core::resourcesAspect::id) };

		quad_resource_aspect.removeComponent<LineMeshe>("square");
		quad_resource_aspect.removeComponent<Shader>("vertexShader");
		quad_resource_aspect.removeComponent<Shader>("pixelShader");




		m_entitygraph.remove(quadNode);


		m_quadEntity0_state = false;
	}



	//////////////////////////////////////////////////////
	// 	
	// quadEntity1

	if (true == m_quadEntity1_state_request && false == m_quadEntity1_state)
	{

		const auto dataCloud{ renderMe::rendering::Datacloud::getInstance() };
		dataCloud->registerData<maths::Real4Vector>("quad1_color");

		Entitygraph::Node& screenRenderingPassNode{ m_entitygraph.node("screenRenderingEntity") };

		auto& quadNode{ m_entitygraph.add(screenRenderingPassNode, "quadEntity1") };
		const auto quadEntity{ quadNode.data() };

		auto& quad_resource_aspect{ quadEntity->makeAspect(core::resourcesAspect::id) };

		/////////// Add shaders

		quad_resource_aspect.addComponent<Shader>("vertexShader", Shader("color_vs", 0));
		quad_resource_aspect.addComponent<Shader>("pixelShader", Shader("color_ps", 1));

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

		quad_resource_aspect.addComponent<LineMeshe>("square", square);



		auto& quad_rendering_aspect{ quadEntity->makeAspect(core::renderingAspect::id) };

		/////////// Add renderstate

		RenderState rs_noculling(RenderState::Operation::SETCULLING, "none");
		const std::vector<RenderState> rs_list = { rs_noculling };

		quad_rendering_aspect.addComponent<std::vector<RenderState>>("renderStates", rs_list);

		/////////// Draw lines

		rendering::LineDrawingControl lineDrawingControl;

		lineDrawingControl.world.translation(0.0, 0.0, -15.0);
		lineDrawingControl.view.identity();
		lineDrawingControl.proj.perspective(1.0, 0.64285713434219360, 1.0, 100000.00000000000);

		lineDrawingControl.pshaders_map.push_back(std::make_pair("quad1_color", "color"));


		quad_rendering_aspect.addComponent<rendering::LineDrawingControl>("squareRendering", lineDrawingControl);




		auto& quad_time_aspect{ quadEntity->makeAspect(core::timeAspect::id) };		
		quad_time_aspect.addComponent<TimeManager::Variable>("quad1_color", TimeManager::Variable(TimeManager::Variable::Type::POSITION, 0.43));




		m_quadEntity1_state = true;
	}
	else if (false == m_quadEntity1_state_request && true == m_quadEntity1_state)
	{
		// remove quadEntity1

		const auto dataCloud{ renderMe::rendering::Datacloud::getInstance() };
		dataCloud->removeData<maths::Real4Vector>("quad1_color");


		auto& quadNode{ m_entitygraph.node("quadEntity1") };
		const auto quadEntity{ quadNode.data() };


		////////////////////////////////////

		auto& quad_time_aspect{ quadEntity->aspectAccess(core::timeAspect::id) };

		quad_time_aspect.removeComponent<TimeManager::Variable>("quad1_color");
		quadEntity->removeAspect(core::timeAspect::id);

		////////////////////////////////////

		auto& quad_rendering_aspect{ quadEntity->aspectAccess(core::renderingAspect::id) };

		quad_rendering_aspect.removeComponent<rendering::LineDrawingControl>("squareRendering");
		quad_rendering_aspect.removeComponent<std::vector<RenderState>>("renderStates");

		quadEntity->removeAspect(core::renderingAspect::id);

		////////////////////////////////////

		auto& quad_resource_aspect{ quadEntity->aspectAccess(core::resourcesAspect::id) };

		quad_resource_aspect.removeComponent<LineMeshe>("square");
		quad_resource_aspect.removeComponent<Shader>("vertexShader");
		quad_resource_aspect.removeComponent<Shader>("pixelShader");




		m_entitygraph.remove(quadNode);


		m_quadEntity1_state = false;
	}



	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	if(m_quadEntity0_state)
	{
		auto& quadNode{ m_entitygraph.node("quadEntity0") };

		const auto quadEntity{ quadNode.data() };
		auto& quad_time_aspect{ quadEntity->aspectAccess(core::timeAspect::id) };

		renderMe::core::TimeManager::Variable& mycolor_r{ quad_time_aspect.getComponent<renderMe::core::TimeManager::Variable>("quad0_color")->getPurpose() };

		if (mycolor_r.value > 1.0)
		{
			mycolor_r.value = 1.0;
			mycolor_r.increment = false;
		}
		else if (mycolor_r.value < 0.0)
		{
			mycolor_r.value = 0.0;
			mycolor_r.increment = true;
		}

		const auto dataCloud{ renderMe::rendering::Datacloud::getInstance() };
		maths::Real4Vector mycolor;

		mycolor[0] = mycolor_r.value;
		mycolor[1] = mycolor_r.value;
		mycolor[2] = mycolor_r.value;
		mycolor[3] = 1.0;

		dataCloud->updateData("quad0_color", mycolor);
	}
	

	if (m_quadEntity1_state)
	{
		auto& quadNode{ m_entitygraph.node("quadEntity1") };

		const auto quadEntity{ quadNode.data() };
		auto& quad_time_aspect{ quadEntity->aspectAccess(core::timeAspect::id) };

		renderMe::core::TimeManager::Variable& mycolor_r{ quad_time_aspect.getComponent<renderMe::core::TimeManager::Variable>("quad1_color")->getPurpose() };

		if (mycolor_r.value > 1.0)
		{
			mycolor_r.value = 1.0;
			mycolor_r.increment = false;
		}
		else if (mycolor_r.value < 0.0)
		{
			mycolor_r.value = 0.0;
			mycolor_r.increment = true;
		}

		const auto dataCloud{ renderMe::rendering::Datacloud::getInstance() };
		maths::Real4Vector mycolor;

		mycolor[0] = mycolor_r.value;
		mycolor[1] = mycolor_r.value;
		mycolor[2] = mycolor_r.value;
		mycolor[3] = 1.0;

		dataCloud->updateData("quad1_color", mycolor);
	}
	
	
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
}
