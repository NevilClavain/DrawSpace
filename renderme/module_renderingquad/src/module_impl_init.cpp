
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

#include "aspects.h"

#include "d3d11system.h"
#include "timesystem.h"
#include "resourcesystem.h"
#include "worldsystem.h"
#include "dataprintsystem.h"
#include "renderingqueuesystem.h"

#include "sysengine.h"
#include "filesystem.h"
#include "logconf.h"

#include "trianglemeshe.h"
#include "renderstate.h"
#include "texture.h"


#include "logger_service.h"

#include "worldposition.h"
#include "animatorfunc.h"

#include "syncvariable.h"
#include "animators_helpers.h"

#include "datacloud.h"

#include "shaders_service.h"

#include "entitygraphlog.h"

using namespace renderMe;
using namespace renderMe::core;
using namespace renderMe::rendering;

void ModuleImpl::init(const std::string p_appWindowsEntityName)
{
	/////////// logging conf

	renderMe::core::FileContent<char> logConfFileContent("./module_renderingquad_config/logconf.json");
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
	sysEngine->makeSystem<renderMe::WorldSystem>(3, m_entitygraph);
	sysEngine->makeSystem<renderMe::RenderingQueueSystem>(4, m_entitygraph);
	sysEngine->makeSystem<renderMe::DataPrintSystem>(5, m_entitygraph);

	// D3D11 system provides compilation shader service : give access to this to resources sytem
	const auto d3d11System{ sysEngine->getSystem<renderMe::D3D11System>(d3d11SystemSlot) };
	services::ShadersCompilationService::getInstance()->registerSubscriber(d3d11System->getServiceInvocationCallback());


	d3d11_system_events();
	resource_system_events();

	//////////////////////////

	createEntities(p_appWindowsEntityName);
}


void ModuleImpl::createEntities(const std::string p_appWindowsEntityName)
{
	/////////// add screen rendering pass entity

	auto& appwindowNode{ m_entitygraph.node(p_appWindowsEntityName) };

	auto& screenRenderingNode{ m_entitygraph.add(appwindowNode, "screenRenderingEntity") };
	const auto screenRenderingPassEntity{ screenRenderingNode.data() };

	auto& screenRendering_rendering_aspect{ screenRenderingPassEntity->makeAspect(core::renderingAspect::id) };

	screenRendering_rendering_aspect.addComponent<rendering::Queue>("renderingQueue", "final_pass");

	auto& rendering_queue{ screenRendering_rendering_aspect.getComponent<rendering::Queue>("renderingQueue")->getPurpose() };
	rendering_queue.setTargetClearColor({ 0, 0, 64, 255 });
	rendering_queue.enableTargetClearing(true);

	//rendering_queue.setText(3, { "Hello world !", "Bahnschrift.16.spritefont", { 0, 255, 0, 255 }, { 400, 10 }, 0.0 });

	m_windowRenderingQueue = &rendering_queue;

	auto sysEngine{ SystemEngine::getInstance() };
	const auto dataPrintSystem{ sysEngine->getSystem<renderMe::DataPrintSystem>(dataPrintSystemSlot) };

	dataPrintSystem->setRenderingQueue(m_windowRenderingQueue);
}




void ModuleImpl::resource_system_events()
{
	const auto sysEngine{ SystemEngine::getInstance() };

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

				case ResourceSystemEvent::RESOURCE_TEXTURE_LOAD_SUCCESS:
					_RENDERME_DEBUG(eventsLogger, "RECV EVENT -> RESOURCE_TEXTURE_LOAD_SUCCESS : " + p_resourceName);
					m_resources_event = "Texture loaded :" + p_resourceName;
					break;
			}
		}
	};

	const auto resourceSystem{ sysEngine->getSystem<renderMe::ResourceSystem>(resourceSystemSlot) };
	resourceSystem->registerSubscriber(rs_cb);
}




void ModuleImpl::d3d11_system_events()
{
	const auto sysEngine{ SystemEngine::getInstance() };
	const auto d3d11System{ sysEngine->getSystem<renderMe::D3D11System>(d3d11SystemSlot) };

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

					m_characteristics_v_width = characteristics_v_width;
					m_characteristics_v_height = characteristics_v_height;



					{

						/////////////// add viewpoint ////////////////////////

						auto& screenRenderingNode{ m_entitygraph.node("screenRenderingEntity") };

					
						auto& viewPointNode{ m_entitygraph.add(screenRenderingNode, "ScreenRenderingViewEntity") };
						const auto cameraEntity{ viewPointNode.data() };

						auto& camera_aspect{ cameraEntity->makeAspect(core::cameraAspect::id) };

						maths::Matrix projection;

						projection.perspective(characteristics_v_width, characteristics_v_height, 1.0, 100000.00000000000);

						camera_aspect.addComponent<maths::Matrix>("projection", projection);

						auto& camera_world_aspect{ cameraEntity->makeAspect(core::worldAspect::id) };

						camera_world_aspect.addComponent<transform::WorldPosition>("camera_position", transform::WorldPosition());

					}

					//////////////////////////////////////////////////////////////

					m_windowRenderingQueue->setCurrentView("ScreenRenderingViewEntity");








					/////////////////////////////screenRenderingQuadEntity///////////////////////////////////////

					{

						Entitygraph::Node& screenRenderingNode{ m_entitygraph.node("screenRenderingEntity") };

						auto& screenRenderingQuadNode{ m_entitygraph.add(screenRenderingNode, "screenRenderingQuadEntity") };
						const auto screenRenderingQuadEntity{ screenRenderingQuadNode.data() };

						auto& quad_resource_aspect{ screenRenderingQuadEntity->makeAspect(core::resourcesAspect::id) };

						/////////// Add shaders


						quad_resource_aspect.addComponent<Shader>("vertexShader", Shader("texture_vs", 0));
						quad_resource_aspect.addComponent<Shader>("pixelShader", Shader("texture_ps", 1));



						/////////// Add trianglemeshe
						TriangleMeshe square("square", TriangleMeshe::State::BLOBLOADED);

						square.push(Vertex(-m_characteristics_v_width / 2, -m_characteristics_v_height / 2, 0.0));
						square.push(Vertex(m_characteristics_v_width / 2, -m_characteristics_v_height / 2, 0.0));
						square.push(Vertex(m_characteristics_v_width / 2, m_characteristics_v_height / 2, 0.0));
						square.push(Vertex(-m_characteristics_v_width / 2, m_characteristics_v_height / 2, 0.0));

						const TrianglePrimitive<unsigned int> t1{ 0, 1, 2 };
						square.push(t1);

						const TrianglePrimitive<unsigned int> t2{ 0, 2, 3 };
						square.push(t2);

						square.computeNormales();
						square.computeTB();

						quad_resource_aspect.addComponent<TriangleMeshe>("square", square);

						auto& quad_rendering_aspect{ screenRenderingQuadEntity->makeAspect(core::renderingAspect::id) };

						/////////// render target Texture

						const auto dataCloud{ renderMe::rendering::Datacloud::getInstance() };

						const auto window_dims{ dataCloud->readDataValue<renderMe::core::maths::IntCoords2D>("std.window_resol") };

						const int w_width{ window_dims.x() };
						const int w_height{ window_dims.y() };

						quad_resource_aspect.addComponent<std::pair<size_t, Texture>>("rendering_quad_texture", std::make_pair(Texture::STAGE_0, Texture("rendering_quad_texture", Texture::Format::TEXTURE_RGB, w_width, w_height)));


						/////////// Add renderstate

						RenderState rs_noculling(RenderState::Operation::SETCULLING, "none");
						RenderState rs_zbuffer(RenderState::Operation::ENABLEZBUFFER, "false");
						RenderState rs_fill(RenderState::Operation::SETFILLMODE, "solid");

						const std::vector<RenderState> rs_list = { rs_noculling, rs_zbuffer, rs_fill };

						quad_rendering_aspect.addComponent<std::vector<RenderState>>("renderStates", rs_list);

						/////////// Draw triangles

						rendering::DrawingControl drawingControl;



						quad_rendering_aspect.addComponent<rendering::DrawingControl>("squareRendering", drawingControl);


						/////////// time aspect
						// required for animator !

						auto& quad_time_aspect{ screenRenderingQuadEntity->makeAspect(core::timeAspect::id) };

						/////////// World position

						auto& world_aspect{ screenRenderingQuadEntity->makeAspect(core::worldAspect::id) };

						world_aspect.addComponent<transform::WorldPosition>("position");


						world_aspect.addComponent<transform::Animator>("animator_positioning", transform::Animator
						(
							{},
							[](const core::ComponentContainer& p_world_aspect,
								const core::ComponentContainer& p_time_aspect,
								const transform::WorldPosition&,
								const std::unordered_map<std::string, std::string>&)
							{

								maths::Matrix positionmat;
								positionmat.translation(0.0, 0.0, -1.05001);

								transform::WorldPosition& wp{ p_world_aspect.getComponent<transform::WorldPosition>("position")->getPurpose() };
								wp.local_pos = wp.local_pos * positionmat;
							}
						));

					}

					/////////////////////////////screenRenderingQuadEntity///////////////////////////////////////


					{
						Entitygraph::Node& screenRenderingQuadNode{ m_entitygraph.node("screenRenderingQuadEntity") };

						auto& bufferRenderingNode{ m_entitygraph.add(screenRenderingQuadNode, "bufferRenderingEntity") };
						const auto bufferRenderingQuadEntity{ bufferRenderingNode.data() };

						auto& bufferRendering_rendering_aspect{ bufferRenderingQuadEntity->makeAspect(core::renderingAspect::id) };

						// TO BE CONTINUED


					}
					
					helpers::logEntitygraph(m_entitygraph);
				}
				break;
			}
		}
	};
	d3d11System->registerSubscriber(d3d11_cb);
}