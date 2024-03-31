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
#include "syncvariable.h"
#include "timemanager.h"
#include "resourcesystem.h"
#include "renderingqueuesystem.h"
#include "worldsystem.h"
#include "worldposition.h"
#include "shader.h"
#include "linemeshe.h"
#include "trianglemeshe.h"
#include "renderstate.h"

#include "logger_service.h"

#include "filesystem.h"
#include "logconf.h"

#include "entity.h"

#include "datacloud.h"

#include "animatorfunc.h"


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
	else if (VK_F5 == p_key)
	{
		if (true == m_quadEntity2_state_request)
		{
			m_quadEntity2_state_request = false;
		}
		else
		{
			m_quadEntity2_state_request = true;
		}
	}

	else if (VK_F8 == p_key)
	{
		auto renderingQueueSystem{ SystemEngine::getInstance()->getSystem(5) };
		auto renderingQueueSystemInstance{ dynamic_cast<renderMe::RenderingQueueSystem*>(renderingQueueSystem) };

		renderingQueueSystemInstance->requestRenderingqueueLogging("screenRenderingEntity");
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
	const auto tm{ TimeManager::getInstance() };

	if (tm->isReady())
	{
		auto& fpsMvtNode{ m_entitygraph.node("CameraFPSMvtEntity") };
		const auto cameraFPSMvtEntity{ fpsMvtNode.data() };

		auto& fps_world_aspect{ cameraFPSMvtEntity->aspectAccess(core::worldAspect::id) };

		double& fps_theta{ fps_world_aspect.getComponent<double>("fps_theta")->getPurpose() };
		double& fps_phi{ fps_world_aspect.getComponent<double>("fps_phi")->getPurpose() };

		tm->angleSpeedInc(&fps_theta, -p_dx);
		tm->angleSpeedInc(&fps_phi, -p_dy);

	}	
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
	sysEngine->makeSystem<renderMe::WorldSystem> (3, m_entitygraph);
	sysEngine->makeSystem<renderMe::RenderingQueueSystem>(4, m_entitygraph);

	// D3D11 system provides compilation shader service

	const auto d3d11System{ sysEngine->getSystem<renderMe::D3D11System>(1) };
	services::ShadersCompilationService::getInstance()->registerSubscriber(d3d11System->getServiceInvocationCallback());

	const D3D11System::Callback d3d11_cb
	{
		[&, this](D3D11SystemEvent p_event, const std::string& p_id)
		{
			switch (p_event)
			{
				case D3D11SystemEvent::D3D11_WINDOW_READY:
				{
					auto& appwindowNode{ m_entitygraph.node(p_id) };
					const auto appwindow{ appwindowNode.data() };

					const auto& mainwindows_rendering_aspect{ appwindow->aspectAccess(renderMe::core::renderingAspect::id) };

					const float characteristics_v_width{ mainwindows_rendering_aspect.getComponent<float>("viewportWidth")->getPurpose()};
					const float characteristics_v_height{ mainwindows_rendering_aspect.getComponent<float>("viewportHeight")->getPurpose()};


					/////////////// add viewpoint FPS mvt ////////////////

					auto& fpsMvtNode{ m_entitygraph.add(appwindowNode, "CameraFPSMvtEntity") };
					const auto cameraFPSMvtEntity{ fpsMvtNode.data() };

					auto& camera_time_aspect{ cameraFPSMvtEntity->makeAspect(core::timeAspect::id) };

					auto& fps_world_aspect{ cameraFPSMvtEntity->makeAspect(core::worldAspect::id) };

					fps_world_aspect.addComponent<transform::WorldPosition>("fpsmvt_position");

					fps_world_aspect.addComponent<double>("fps_theta", 0);
					fps_world_aspect.addComponent<double>("fps_phi", 0);
					fps_world_aspect.addComponent<maths::Real4Vector>("fps_pos", maths::Real4Vector( 0.0, 4.0, 7.0 ));

					fps_world_aspect.addComponent<transform::AnimatorFunc>("animator", [](const core::ComponentContainer& p_world_aspect, const core::ComponentContainer& p_time_aspect)
						{
							const double fps_theta{ p_world_aspect.getComponent<double>("fps_theta")->getPurpose() };
							const double fps_phi{ p_world_aspect.getComponent<double>("fps_phi")->getPurpose() };

							maths::Matrix fps_thetarotnmat;
							fps_thetarotnmat.rotation(maths::Real4Vector(0.0, 1.0, 0.0), fps_theta);

							maths::Matrix fps_phirotnmat;
							fps_phirotnmat.rotation(maths::Real4Vector(1.0, 0.0, 0.0), fps_phi);

							const auto fps_pos { p_world_aspect.getComponent<maths::Real4Vector>("fps_pos")->getPurpose() };

							maths::Matrix fps_positionmat;
							fps_positionmat.translation(fps_pos);

							const auto final_local_mat{ fps_phirotnmat * fps_thetarotnmat * fps_positionmat };

							// store result
							transform::WorldPosition& wp{ p_world_aspect.getComponent<transform::WorldPosition>("fpsmvt_position")->getPurpose() };
							wp.local_pos = final_local_mat;
						}
					);




					/////////////// add viewpoint ////////////////////////

					auto& viewPointNode{ m_entitygraph.add(fpsMvtNode, "Camera01Entity")};
					const auto cameraEntity{ viewPointNode.data() };

					auto& camera_aspect{ cameraEntity->makeAspect(core::cameraAspect::id) };

					maths::Matrix projection;
					projection.projection(characteristics_v_width, characteristics_v_height, 1.0, 100000.00000000000);

					camera_aspect.addComponent<maths::Matrix>("projection", projection);

					auto& camera_world_aspect{ cameraEntity->makeAspect(core::worldAspect::id) };

					maths::Matrix cam_positionmat;
					cam_positionmat.translation(0.0, 0.0, 0.0);

					camera_world_aspect.addComponent<transform::WorldPosition>("camera_position", transform::WorldPosition(cam_positionmat));

					//////////////////////////////////////////////////////

					const auto dataCloud{ renderMe::rendering::Datacloud::getInstance() };
					dataCloud->updateDataValue<std::string>("(@std)current_view", "Camera01Entity");
				}
				break;
			}
		}
	};
	d3d11System->registerSubscriber(d3d11_cb);



	// register to resource system events

	const ResourceSystem::Callback rs_cb
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

	const auto resourceSystem{ sysEngine->getSystem<renderMe::ResourceSystem>(2) };
	resourceSystem->registerSubscriber(rs_cb);



	
	//////////////////////////

	createEntities(p_appWindowsEntityName);

	const auto dataCloud{ renderMe::rendering::Datacloud::getInstance() };
	dataCloud->registerData<std::string>("(@std)current_view");

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

		rendering::DrawingControl lineDrawingControl;
		lineDrawingControl.pshaders_map.push_back(std::make_pair("quad0_color", "color"));


		quad_rendering_aspect.addComponent<rendering::DrawingControl>("squareRendering", lineDrawingControl);


		auto& quad_time_aspect{ quadEntity->makeAspect(core::timeAspect::id) };
		quad_time_aspect.addComponent<SyncVariable>("quad0_color", SyncVariable(SyncVariable::Type::POSITION, 1.0));

		quad_time_aspect.addComponent<SyncVariable>("z_rotation_angle", SyncVariable(SyncVariable::Type::ANGLE, 1.0));

		/////////// World position

		auto& world_aspect{ quadEntity->makeAspect(core::worldAspect::id) };

		world_aspect.addComponent<transform::WorldPosition>("position");
	
		
		world_aspect.addComponent<transform::AnimatorFunc>("animator", [](const core::ComponentContainer& p_world_aspect, const core::ComponentContainer& p_time_aspect)
			{
				const auto& z_rotation_angle{ p_time_aspect.getComponent<SyncVariable>("z_rotation_angle")->getPurpose()};

				maths::Matrix rotation_mat;
				rotation_mat.rotation(maths::Real4Vector(0.0, 0.0, 1.0), z_rotation_angle.value);

				transform::WorldPosition& wp{ p_world_aspect.getComponent<transform::WorldPosition>("position")->getPurpose() };
				wp.local_pos = rotation_mat;
			}		
		);




		m_quadEntity0_state = true;
	}
	else if (false == m_quadEntity0_state_request && true == m_quadEntity0_state)
	{
		// remove quadEntity0

		const auto dataCloud{ renderMe::rendering::Datacloud::getInstance() };
		dataCloud->removeData<maths::Real4Vector>("quad0_color");


		auto& quadNode{ m_entitygraph.node("quadEntity0") };
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

		rendering::DrawingControl lineDrawingControl;
		lineDrawingControl.pshaders_map.push_back(std::make_pair("quad1_color", "color"));


		quad_rendering_aspect.addComponent<rendering::DrawingControl>("squareRendering", lineDrawingControl);


		auto& quad_time_aspect{ quadEntity->makeAspect(core::timeAspect::id) };		
		quad_time_aspect.addComponent<SyncVariable>("quad1_color", SyncVariable(SyncVariable::Type::POSITION, 0.43));

		/////////// World position

		auto& world_aspect{ quadEntity->makeAspect(core::worldAspect::id) };

		maths::Matrix positionmat;
		positionmat.translation(0.0, 0.0, -15.0);

		world_aspect.addComponent<transform::WorldPosition>("position", transform::WorldPosition(positionmat));


		m_quadEntity1_state = true;
	}
	else if (false == m_quadEntity1_state_request && true == m_quadEntity1_state)
	{
		// remove quadEntity1

		const auto dataCloud{ renderMe::rendering::Datacloud::getInstance() };
		dataCloud->removeData<maths::Real4Vector>("quad1_color");

		auto& quadNode{ m_entitygraph.node("quadEntity1") };
		m_entitygraph.remove(quadNode);


		m_quadEntity1_state = false;
	}

	//////////////////////////////////////////////////////
	// 	
	// quadEntity2

	if (true == m_quadEntity2_state_request && false == m_quadEntity2_state)
	{

		const auto dataCloud{ renderMe::rendering::Datacloud::getInstance() };
		dataCloud->registerData<maths::Real4Vector>("quad2_color");
		dataCloud->updateDataValue< maths::Real4Vector>("quad2_color", maths::Real4Vector(1.0f, 1.0f, 1.0f, 1.0f));

		Entitygraph::Node& screenRenderingPassNode{ m_entitygraph.node("screenRenderingEntity") };

		auto& quadNode{ m_entitygraph.add(screenRenderingPassNode, "quadEntity2") };
		const auto quadEntity{ quadNode.data() };

		auto& quad_resource_aspect{ quadEntity->makeAspect(core::resourcesAspect::id) };

		/////////// Add shaders

		quad_resource_aspect.addComponent<Shader>("vertexShader", Shader("color_vs", 0));
		quad_resource_aspect.addComponent<Shader>("pixelShader", Shader("color_ps", 1));


		/////////// Add trianglemeshe
		TriangleMeshe square("square", TriangleMeshe::State::BLOBLOADED);

		square.push(Vertex(-0.5, -0.5, 0.0));
		square.push(Vertex(0.5, -0.5, 0.0));
		square.push(Vertex(0.5, 0.5, 0.0));
		square.push(Vertex(-0.5, 0.5, 0.0));

		const TrianglePrimitive<unsigned int> t1 { 0, 1, 2 };
		square.push(t1);

		const TrianglePrimitive<unsigned int> t2 { 0, 2, 3 };
		square.push(t2);

		square.computeNormales();
		square.computeTB();

		quad_resource_aspect.addComponent<TriangleMeshe>("square", square);

		auto& quad_rendering_aspect{ quadEntity->makeAspect(core::renderingAspect::id) };

		/////////// Add renderstate

		RenderState rs_noculling(RenderState::Operation::SETCULLING, "none");
		RenderState rs_zbuffer(RenderState::Operation::ENABLEZBUFFER, "false");
		RenderState rs_fill(RenderState::Operation::SETFILLMODE, "line");

		const std::vector<RenderState> rs_list = { rs_noculling, rs_zbuffer, rs_fill };

		quad_rendering_aspect.addComponent<std::vector<RenderState>>("renderStates", rs_list);

		/////////// Draw triangles

		rendering::DrawingControl drawingControl;
		drawingControl.pshaders_map.push_back(std::make_pair("quad2_color", "color"));


		quad_rendering_aspect.addComponent<rendering::DrawingControl>("squareRendering", drawingControl);



		/////////// World position

		auto& world_aspect{ quadEntity->makeAspect(core::worldAspect::id) };

		maths::Matrix positionmat;
		positionmat.translation(0.0, 0.0, -20.0);

		world_aspect.addComponent<transform::WorldPosition>("position", transform::WorldPosition(positionmat));


		m_quadEntity2_state = true;
	}
	else if (false == m_quadEntity2_state_request && true == m_quadEntity2_state)
	{
		// remove quadEntity2

		const auto dataCloud{ renderMe::rendering::Datacloud::getInstance() };
		dataCloud->removeData<maths::Real4Vector>("quad2_color");

		auto& quadNode{ m_entitygraph.node("quadEntity2") };
		m_entitygraph.remove(quadNode);


		m_quadEntity2_state = false;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	if(m_quadEntity0_state)
	{
		auto& quadNode{ m_entitygraph.node("quadEntity0") };

		const auto quadEntity{ quadNode.data() };
		auto& quad_time_aspect{ quadEntity->aspectAccess(core::timeAspect::id) };

		renderMe::core::SyncVariable& mycolor_r{ quad_time_aspect.getComponent<renderMe::core::SyncVariable>("quad0_color")->getPurpose() };

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

		dataCloud->updateDataValue("quad0_color", mycolor);
	}
	

	if (m_quadEntity1_state)
	{
		auto& quadNode{ m_entitygraph.node("quadEntity1") };

		const auto quadEntity{ quadNode.data() };
		auto& quad_time_aspect{ quadEntity->aspectAccess(core::timeAspect::id) };

		renderMe::core::SyncVariable& mycolor_r{ quad_time_aspect.getComponent<renderMe::core::SyncVariable>("quad1_color")->getPurpose() };

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

		dataCloud->updateDataValue("quad1_color", mycolor);
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

}
