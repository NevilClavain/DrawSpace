
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
#include "texture.h"
#include "renderstate.h"
#include "shader.h"



#include "logger_service.h"

#include "worldposition.h"
#include "animatorfunc.h"

#include "syncvariable.h"
#include "animators_helpers.h"

#include "datacloud.h"

#include "shaders_service.h"
#include "textures_service.h"

#include "entitygraph_helpers.h"

#include "graphicobjects_helpers.h"


using namespace renderMe;
using namespace renderMe::core;
using namespace renderMe::rendering;

void ModuleImpl::init(const std::string p_appWindowsEntityName)
{
	/////////// logging conf

	renderMe::core::FileContent<char> logConfFileContent("./module_sprites_config/logconf.json");
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
	services::ShadersCompilationService::getInstance()->registerSubscriber(d3d11System->getShaderCompilationInvocationCallback());
	services::TextureContentCopyService::getInstance()->registerSubscriber(d3d11System->getTextureContentCopyInvocationCallback());


	d3d11_system_events();
	resource_system_events();

	//////////////////////////

	createEntities(p_appWindowsEntityName);

	//////////////////////////

	const auto seed{ ::GetTickCount() };

	m_generator = new std::default_random_engine(seed);

	m_speed_distribution = new std::uniform_real_distribution<double>(0.1, 0.2);
	m_rotation_speed_distribution = new std::uniform_real_distribution<double>(0.25 * core::maths::pi, 2 * core::maths::pi);
	m_speed_sign_distribution = new std::uniform_int_distribution<int>(0, 1);
	m_rotation_speed_sign_distribution = new std::uniform_int_distribution<int>(0, 1);
}


void ModuleImpl::createEntities(const std::string& p_appWindowsEntityName)
{
	/////////// add screen rendering pass entity

	auto& appwindowNode{ m_entitygraph.node(p_appWindowsEntityName) };

	auto& screenRenderingNode{ m_entitygraph.add(appwindowNode, "screenRenderingEntity") };
	const auto screenRenderingPassEntity{ screenRenderingNode.data() };

	auto& screenRendering_rendering_aspect{ screenRenderingPassEntity->makeAspect(core::renderingAspect::id) };

	screenRendering_rendering_aspect.addComponent<rendering::Queue>("screenRenderingQueue", "screen_pass_queue");

	auto& rendering_queue{ screenRendering_rendering_aspect.getComponent<rendering::Queue>("screenRenderingQueue")->getPurpose() };
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


					const auto dataCloud{ renderMe::rendering::Datacloud::getInstance() };

					const auto window_dims{ dataCloud->readDataValue<renderMe::core::maths::IntCoords2D>("std.window_resol") };

					const int w_width{ window_dims.x() };
					const int w_height{ window_dims.y() };

					m_rendering_quad_texture = new Texture(Texture::Format::TEXTURE_RGB, w_width, w_height, Texture::ContentAccessMode::CONTENT_ACCESS);

					renderMe::helpers::plugRenderingQuadView( m_entitygraph,
																	characteristics_v_width, characteristics_v_height,																	
																	"screenRenderingEntity",
																	"screenRenderingQuadEntity",
																	"ScreenRenderingViewEntity",
																	m_windowRenderingQueue,
																	"texture_vs",
																	"texture_ps",
																	{	
																		std::make_pair(Texture::STAGE_0, *m_rendering_quad_texture)
																	}																																		
																);
						
					// buffer rendering queue
					rendering::Queue bufferRenderingQueue("buffer_pass_queue");
					bufferRenderingQueue.setTargetClearColor({ 50, 0, 20, 255 });
					bufferRenderingQueue.enableTargetClearing(true);
					bufferRenderingQueue.setTargetStage(Texture::STAGE_0);

					renderMe::helpers::plugRenderingQueue(m_entitygraph, bufferRenderingQueue, "screenRenderingQuadEntity", "bufferRenderingEntity");

							
					// add camera to scene
					maths::Matrix projection;
					projection.perspective(characteristics_v_width, characteristics_v_height, 1.0, 100000.00000000000);
					helpers::plugView(m_entitygraph, projection, "bufferRenderingEntity", "cameraEntity");

					// attach animator/positionner to camera
					core::Entitygraph::Node& cameraNode{ m_entitygraph.node("cameraEntity") };
					const auto cameraEntity{ cameraNode.data() };
					auto& camera_world_aspect{ cameraEntity->aspectAccess(core::worldAspect::id) };

					cameraEntity->makeAspect(core::timeAspect::id);
					camera_world_aspect.addComponent<transform::Animator>("animator_positioning", transform::Animator
					(
						{},
						[](const core::ComponentContainer& p_world_aspect,
							const core::ComponentContainer& p_time_aspect,
							const transform::WorldPosition&,
							const std::unordered_map<std::string, std::string>&)
						{

							core::maths::Matrix positionmat;
							positionmat.translation(0.0, 0.0, 5.000);

							transform::WorldPosition& wp{ p_world_aspect.getComponent<transform::WorldPosition>("camera_position")->getPurpose() };
							wp.local_pos = wp.local_pos * positionmat;
						}
					));

					///////Select camera

					core::Entitygraph::Node& bufferRenderingQueueNode{ m_entitygraph.node("bufferRenderingEntity") };
					const auto bufferRenderingQueueEntity { bufferRenderingQueueNode.data() };
					const auto& renderingAspect{ bufferRenderingQueueEntity->aspectAccess(core::renderingAspect::id) };

					renderingAspect.getComponent<rendering::Queue>("renderingQueue")->getPurpose().setCurrentView("cameraEntity");


					/////////////

					rendering::RenderState rs_noculling(rendering::RenderState::Operation::SETCULLING, "cw");
					rendering::RenderState rs_zbuffer(rendering::RenderState::Operation::ENABLEZBUFFER, "false");
					rendering::RenderState rs_fill(rendering::RenderState::Operation::SETFILLMODE, "solid");
					rendering::RenderState rs_texturepointsampling(rendering::RenderState::Operation::SETTEXTUREFILTERTYPE, "point");

					const std::vector<rendering::RenderState> rs_list = { rs_noculling, rs_zbuffer, rs_fill, rs_texturepointsampling };

					auto sprite{ helpers::plug2DSprite(m_entitygraph, "bufferRenderingEntity", "sprite#0", 0.05, 0.05, "sprite_vs", "sprite_ps", "tennis_ball.bmp", rs_list, 1000) };
					m_sprites.push_back(sprite);

				}
				break;
			}
		}
	};
	d3d11System->registerSubscriber(d3d11_cb);
}