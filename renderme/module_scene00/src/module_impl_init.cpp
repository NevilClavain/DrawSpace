
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

#include "logger_service.h"

#include "worldposition.h"
#include "animatorfunc.h"

#include "syncvariable.h"
#include "animators_helpers.h"

#include "datacloud.h"

#include "shaders_service.h"

using namespace renderMe;
using namespace renderMe::core;

void ModuleImpl::init(const std::string p_appWindowsEntityName)
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
	sysEngine->makeSystem<renderMe::WorldSystem>(3, m_entitygraph);
	sysEngine->makeSystem<renderMe::RenderingQueueSystem>(4, m_entitygraph);
	sysEngine->makeSystem<renderMe::DataPrintSystem>(5, m_entitygraph);

	// D3D11 system provides compilation shader service : give access to this to resources sytem
	const auto d3d11System{ sysEngine->getSystem<renderMe::D3D11System>(1) };
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

	auto& screenRenderingPassNode{ m_entitygraph.add(appwindowNode, "screenRenderingEntity") };
	const auto screenRenderingPassEntity{ screenRenderingPassNode.data() };

	auto& screenRendering_rendering_aspect{ screenRenderingPassEntity->makeAspect(core::renderingAspect::id) };

	screenRendering_rendering_aspect.addComponent<rendering::Queue>("renderingQueue", "final_pass");

	auto& rendering_queue{ screenRendering_rendering_aspect.getComponent<rendering::Queue>("renderingQueue")->getPurpose() };
	rendering_queue.setTargetClearColor({ 0, 0, 64, 255 });
	rendering_queue.enableTargetClearing(true);

	//rendering_queue.setText(3, { "Hello world !", "Bahnschrift.16.spritefont", { 0, 255, 0, 255 }, { 400, 10 }, 0.0 });

	m_windowRenderingQueue = &rendering_queue;

	auto sysEngine{ SystemEngine::getInstance() };
	const auto dataPrintSystem{ sysEngine->getSystem<renderMe::DataPrintSystem>(5) };

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
			}
		}
	};

	const auto resourceSystem{ sysEngine->getSystem<renderMe::ResourceSystem>(2) };
	resourceSystem->registerSubscriber(rs_cb);
}




void ModuleImpl::d3d11_system_events()
{
	const auto sysEngine{ SystemEngine::getInstance() };
	const auto d3d11System{ sysEngine->getSystem<renderMe::D3D11System>(1) };

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

					{
						/////////////// add viewpoint with gimbal lock jointure ////////////////

						auto& screenRenderingNode{ m_entitygraph.node("screenRenderingEntity") };
						auto& gblJointEntityNode{ m_entitygraph.add(screenRenderingNode, "gblJointEntity") };
						const auto gblJointEntity{ gblJointEntityNode.data() };

						gblJointEntity->makeAspect(core::timeAspect::id);
						auto& gbl_world_aspect{ gblJointEntity->makeAspect(core::worldAspect::id) };

						gbl_world_aspect.addComponent<transform::WorldPosition>("gbl_output");

						gbl_world_aspect.addComponent<double>("gbl_theta", 0);
						gbl_world_aspect.addComponent<double>("gbl_phi", 0);
						gbl_world_aspect.addComponent<double>("gbl_speed", 0);
						gbl_world_aspect.addComponent<maths::Real3Vector>("gbl_pos", maths::Real3Vector(12.0, -4.0, 7.0));

						gbl_world_aspect.addComponent<transform::Animator>("animator", transform::Animator(
							{
								// input-output/components keys id mapping
								{"gimbalLockJointAnim.theta", "gbl_theta"},
								{"gimbalLockJointAnim.phi", "gbl_phi"},
								{"gimbalLockJointAnim.position", "gbl_pos"},
								{"gimbalLockJointAnim.speed", "gbl_speed"},
								{"gimbalLockJointAnim.output", "gbl_output"}

							}, helpers::animators::makeGimbalLockJointAnimator()));

						/////////////// add viewpoint ////////////////////////

						auto& viewPointNode{ m_entitygraph.add(gblJointEntityNode, "Camera01Entity") };
						const auto cameraEntity{ viewPointNode.data() };

						auto& camera_aspect{ cameraEntity->makeAspect(core::cameraAspect::id) };

						maths::Matrix projection;
						projection.perspective(characteristics_v_width, characteristics_v_height, 1.0, 100000.00000000000);

						camera_aspect.addComponent<maths::Matrix>("projection", projection);

						auto& camera_world_aspect{ cameraEntity->makeAspect(core::worldAspect::id) };

						maths::Matrix cam_positionmat;
						cam_positionmat.translation(0.0, 0.0, 0.0);

						camera_world_aspect.addComponent<transform::WorldPosition>("camera_position", transform::WorldPosition(cam_positionmat));


						//////////////////////////////////////////////////////

					}

					//////////////////////////////////////////////////////////////

					{
						/////////////// add viewpoint with full gimbal jointure ////////////////

						auto& screenRenderingNode{ m_entitygraph.node("screenRenderingEntity") };
						auto& fullGblJointEntityNode{ m_entitygraph.add(screenRenderingNode, "fullGblJointEntity") };
						const auto fullGblJointEntity{ fullGblJointEntityNode.data() };

						fullGblJointEntity->makeAspect(core::timeAspect::id);
						auto& fullgbl_world_aspect{ fullGblJointEntity->makeAspect(core::worldAspect::id) };

						fullgbl_world_aspect.addComponent<transform::WorldPosition>("fullgbl_output");

						fullgbl_world_aspect.addComponent<double>("rspeed_x", 0.0);
						fullgbl_world_aspect.addComponent<double>("rspeed_y", 0.0);
						fullgbl_world_aspect.addComponent<double>("rspeed_z", 0.0);
						fullgbl_world_aspect.addComponent<core::maths::Real3Vector>("fullgbl_speed");

						// add 3 axis here
						fullgbl_world_aspect.addComponent<maths::Real3Vector>("rot_axis_x", maths::XAxisVector);
						fullgbl_world_aspect.addComponent<maths::Real3Vector>("rot_axis_y", maths::YAxisVector);
						fullgbl_world_aspect.addComponent<maths::Real3Vector>("rot_axis_z", maths::ZAxisVector);

						fullgbl_world_aspect.addComponent<maths::Real3Vector>("fullgbl_pos", maths::Real3Vector(0.0, -1.0, 9.0));
						fullgbl_world_aspect.addComponent<maths::Quaternion>("fullgbl_quat");

						fullgbl_world_aspect.addComponent<transform::Animator>("animator", transform::Animator(
							{
								// input-output/components keys id mapping
								{"fullGimbalJointAnim.position", "fullgbl_pos"},
								{"fullGimbalJointAnim.quat", "fullgbl_quat"},
								{"fullGimbalJointAnim.speed", "fullgbl_speed"},
								{"fullGimbalJointAnim.rot_axis_x", "rot_axis_x"},
								{"fullGimbalJointAnim.rot_axis_y", "rot_axis_y"},
								{"fullGimbalJointAnim.rot_axis_z", "rot_axis_z"},
								{"fullGimbalJointAnim.rot_speed_x", "rspeed_x"},
								{"fullGimbalJointAnim.rot_speed_y", "rspeed_y"},
								{"fullGimbalJointAnim.rot_speed_z", "rspeed_z"},
								{"fullGimbalJointAnim.output", "fullgbl_output"}

							}, helpers::animators::makeFullGimbalJointAnimator()));


						/////////////// add viewpoint ////////////////////////

						auto& viewPointNode{ m_entitygraph.add(fullGblJointEntityNode, "Camera02Entity") };
						const auto cameraEntity{ viewPointNode.data() };

						auto& camera_aspect{ cameraEntity->makeAspect(core::cameraAspect::id) };

						maths::Matrix projection;
						projection.perspective(characteristics_v_width, characteristics_v_height, 1.0, 100000.00000000000);

						camera_aspect.addComponent<maths::Matrix>("projection", projection);

						auto& camera_world_aspect{ cameraEntity->makeAspect(core::worldAspect::id) };

						maths::Matrix cam_positionmat;
						cam_positionmat.translation(0.0, 0.0, 0.0);

						camera_world_aspect.addComponent<transform::WorldPosition>("camera_position", transform::WorldPosition(cam_positionmat));


						//////////////////////////////////////////////////////

					}

					{
						/////////////// add viewpoint of lookat jointure ////////////////

						auto& screenRenderingNode{ m_entitygraph.node("screenRenderingEntity") };
						auto& sliderJointEntityNode{ m_entitygraph.add(screenRenderingNode, "sliderJointEntity") };
						const auto sliderJointEntity{ sliderJointEntityNode.data() };

						auto& slider_time_aspect{ sliderJointEntity->makeAspect(core::timeAspect::id) };
						auto& slider_world_aspect{ sliderJointEntity->makeAspect(core::worldAspect::id) };


						slider_world_aspect.addComponent<transform::WorldPosition>("slider_output");

						SyncVariable x_slide_pos(SyncVariable::Type::POSITION, 5.0, SyncVariable::Direction::INC, 3.0);
						x_slide_pos.state = SyncVariable::State::OFF;

						SyncVariable y_slide_pos(SyncVariable::Type::POSITION, 0.0, SyncVariable::Direction::INC, 4.0);
						y_slide_pos.state = SyncVariable::State::OFF;

						SyncVariable z_slide_pos(SyncVariable::Type::POSITION, 5.0, SyncVariable::Direction::INC, 0.0);
						z_slide_pos.state = SyncVariable::State::OFF;

						slider_time_aspect.addComponent<SyncVariable>("x_slide_pos", x_slide_pos);
						slider_time_aspect.addComponent<SyncVariable>("y_slide_pos", y_slide_pos);
						slider_time_aspect.addComponent<SyncVariable>("z_slide_pos", z_slide_pos);

						slider_world_aspect.addComponent<transform::Animator>("animator", transform::Animator(
							{
								{"xyzSliderJointAnim.x_pos", "x_slide_pos"},
								{"xyzSliderJointAnim.y_pos", "y_slide_pos"},
								{"xyzSliderJointAnim.z_pos", "z_slide_pos"},
								{"xyzSliderJointAnim.output", "slider_output"}

							}, helpers::animators::makeXYZSliderJointAnimator()));








						auto& lookatJointEntityNode{ m_entitygraph.add(sliderJointEntityNode, "lookatJointEntity") };



						const auto lookatJointEntity{ lookatJointEntityNode.data() };

						auto& lookat_time_aspect{ lookatJointEntity->makeAspect(core::timeAspect::id) };
						auto& lookat_world_aspect{ lookatJointEntity->makeAspect(core::worldAspect::id) };

						lookat_world_aspect.addComponent<transform::WorldPosition>("lookat_output");


						lookat_world_aspect.addComponent<core::maths::Real3Vector>("lookat_dest", core::maths::Real3Vector(0.0, 0.0, -20.0));

						lookat_world_aspect.addComponent<transform::Animator>("animator", transform::Animator(
							{
								{"lookatJointAnim.output", "lookat_output"},
								{"lookatJointAnim.dest", "lookat_dest"},

							},
							helpers::animators::makeLookatJointAnimator())
						);

						/////////////// add viewpoint ////////////////////////

						auto& viewPointNode{ m_entitygraph.add(lookatJointEntityNode, "Camera03Entity") };
						const auto cameraEntity{ viewPointNode.data() };

						auto& camera_aspect{ cameraEntity->makeAspect(core::cameraAspect::id) };

						maths::Matrix projection;
						projection.perspective(characteristics_v_width, characteristics_v_height, 1.0, 100000.00000000000);

						camera_aspect.addComponent<maths::Matrix>("projection", projection);

						auto& camera_world_aspect{ cameraEntity->makeAspect(core::worldAspect::id) };

						maths::Matrix cam_positionmat;
						cam_positionmat.translation(0.0, 0.0, 0.0);

						camera_world_aspect.addComponent<transform::WorldPosition>("camera_position", transform::WorldPosition(cam_positionmat));
					}

					//////////////////////////////////////////////////////////////

					const auto dataCloud{ renderMe::rendering::Datacloud::getInstance() };


					//m_windowRenderingQueue->setCurrentView("Camera01Entity");
					//m_windowRenderingQueue->setCurrentView("Camera02Entity");
					m_windowRenderingQueue->setCurrentView("Camera03Entity");
				}
				break;
			}
		}
	};
	d3d11System->registerSubscriber(d3d11_cb);
}