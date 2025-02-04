
/* -*-LIC_BEGIN-*- */
/*
*
* MaGE rendering framework
* Emmanuel Chaumont Copyright (c) 2013-2025
*
* This file is part of MaGE.
*
*    MaGE is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    MaGE is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with MaGE.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#include "module_impl.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>
#include <list>

#include <chrono>

#include "aspects.h"

#include "d3d11system.h"
#include "timesystem.h"
#include "resourcesystem.h"
#include "worldsystem.h"
#include "dataprintsystem.h"
#include "renderingqueuesystem.h"
#include "animationssystem.h"

#include "sysengine.h"
#include "filesystem.h"
#include "logconf.h"

#include "logger_service.h"

#include "worldposition.h"
#include "animatorfunc.h"

#include "trianglemeshe.h"
#include "renderstate.h"

#include "animationbone.h"
#include "scenenode.h"


#include "syncvariable.h"
#include "animators_helpers.h"

#include "datacloud.h"

#include "shaders_service.h"
#include "textures_service.h"

#include "entitygraph_helpers.h"
#include "graphicobjects_helpers.h"

using namespace mage;
using namespace mage::core;
using namespace mage::rendering;

void ModuleImpl::init(const std::string p_appWindowsEntityName)
{
	/////////// logging conf

	mage::core::FileContent<char> logConfFileContent("./module_anims_config/logconf.json");
	logConfFileContent.load();

	const auto dataSize{ logConfFileContent.getDataSize() };
	const std::string data(logConfFileContent.getData(), dataSize);

	mage::core::Json<> jsonParser;
	jsonParser.registerSubscriber(logger::Configuration::getInstance()->getCallback());

	const auto logParseStatus{ jsonParser.parse(data) };

	if (logParseStatus < 0)
	{
		_EXCEPTION("Cannot parse logging configuration")
	}

	///////////////////////////

	const auto dataCloud{ mage::rendering::Datacloud::getInstance() };

	dataCloud->registerData<std::string>("resources_event");
	dataCloud->updateDataValue<std::string>("resources_event", "...");


	dataCloud->registerData<std::string>("current_animation.id");
	dataCloud->registerData<double>("current_animation.ticks_progress");
	dataCloud->registerData<double>("current_animation.seconds_progress");
	dataCloud->registerData<double>("current_animation.ticks_duration");
	dataCloud->registerData<double>("current_animation.seconds_duration");


	/////////// systems

	auto sysEngine{ SystemEngine::getInstance() };

	sysEngine->makeSystem<mage::TimeSystem>(timeSystemSlot, m_entitygraph);
	sysEngine->makeSystem<mage::D3D11System>(d3d11SystemSlot, m_entitygraph);
	sysEngine->makeSystem<mage::ResourceSystem>(resourceSystemSlot, m_entitygraph);
	sysEngine->makeSystem<mage::WorldSystem>(worldSystemSlot, m_entitygraph);
	sysEngine->makeSystem<mage::RenderingQueueSystem>(renderingQueueSystemSlot, m_entitygraph);
	sysEngine->makeSystem<mage::DataPrintSystem>(dataPrintSystemSlot, m_entitygraph);
	sysEngine->makeSystem<mage::AnimationsSystem>(animationsSystemSlot, m_entitygraph);

	// D3D11 system provides compilation shader service : give access to this to resources sytem
	const auto d3d11System{ sysEngine->getSystem<mage::D3D11System>(d3d11SystemSlot) };
	services::ShadersCompilationService::getInstance()->registerSubscriber(d3d11System->getShaderCompilationInvocationCallback());
	services::TextureContentCopyService::getInstance()->registerSubscriber(d3d11System->getTextureContentCopyInvocationCallback());

	// dataprint system filters
	const auto dataPrintSystem{ sysEngine->getSystem<mage::DataPrintSystem>(dataPrintSystemSlot) };
	dataPrintSystem->addDatacloudFilter("resources_event");
	dataPrintSystem->addDatacloudFilter("current_animation");

	///////////////////////////

	auto now = std::chrono::system_clock::now();
	auto now_c = std::chrono::system_clock::to_time_t(now);
	const int time_based_seed{ static_cast<int>(now_c) };
	m_random_engine.seed(time_based_seed);

	///////////////////////////

	d3d11_system_events();
	resource_system_events();
	animation_system_events();

	//////////////////////////

	createEntities(p_appWindowsEntityName);

	//////////////////////////
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
	

	m_windowRenderingQueue = &rendering_queue;

	auto sysEngine{ SystemEngine::getInstance() };
	const auto dataPrintSystem{ sysEngine->getSystem<mage::DataPrintSystem>(dataPrintSystemSlot) };

	dataPrintSystem->setRenderingQueue(m_windowRenderingQueue);
}


void ModuleImpl::animation_system_events()
{
	// register to animation system events
	const AnimationsSystem::Callback cb
	{
		[&, this](AnimationSystemEvent p_event, const std::string& /*p_entityId*/, const std::string& /*p_animationName*/)
		{
			switch (p_event)
			{
				case AnimationSystemEvent::ANIMATION_START:
					break;

				case AnimationSystemEvent::ANIMATION_END:
					choose_animation();
					break;
			}
		}
	};

	const auto sysEngine{ SystemEngine::getInstance() };
	const auto animationsSystem{ sysEngine->getSystem<mage::AnimationsSystem>(animationsSystemSlot) };
	animationsSystem->registerSubscriber(cb);
}



void ModuleImpl::resource_system_events()
{	
	// register to resource system events
	const ResourceSystem::Callback rs_cb
	{
		[&, this](ResourceSystemEvent p_event, const std::string& p_resourceName)
		{
			auto& eventsLogger{ services::LoggerSharing::getInstance()->getLogger("Events") };

			const auto dataCloud{ mage::rendering::Datacloud::getInstance() };

			switch (p_event)
			{
				case ResourceSystemEvent::RESOURCE_SHADER_CACHE_CREATED:
					_MAGE_DEBUG(eventsLogger, "RECV EVENT -> RESOURCE_SHADER_CACHE_CREATED : " + p_resourceName);
					dataCloud->updateDataValue<std::string>("resources_event", "Shader cache creation : " + p_resourceName);
					break;

				case ResourceSystemEvent::RESOURCE_SHADER_COMPILATION_BEGIN:
					_MAGE_DEBUG(eventsLogger, "RECV EVENT -> RESOURCE_SHADER_COMPILATION_BEGIN : " + p_resourceName);
					dataCloud->updateDataValue<std::string>("resources_event", "Shader compilation: " + p_resourceName + " BEGIN");
					break;

				case ResourceSystemEvent::RESOURCE_SHADER_COMPILATION_SUCCESS:
					_MAGE_DEBUG(eventsLogger, "RECV EVENT -> RESOURCE_SHADER_COMPILATION_SUCCESS : " + p_resourceName);
					dataCloud->updateDataValue<std::string>("resources_event", "Shader compilation " + p_resourceName + " SUCCESS");
					break;

				case ResourceSystemEvent::RESOURCE_SHADER_COMPILATION_ERROR:
					_MAGE_DEBUG(eventsLogger, "RECV EVENT -> RESOURCE_SHADER_COMPILATION_ERROR : " + p_resourceName);
					dataCloud->updateDataValue<std::string>("resources_event", "Shader compilation " + p_resourceName + " ERROR");
					break;

				case ResourceSystemEvent::RESOURCE_TEXTURE_LOAD_SUCCESS:
					_MAGE_DEBUG(eventsLogger, "RECV EVENT -> RESOURCE_TEXTURE_LOAD_SUCCESS : " + p_resourceName);
					dataCloud->updateDataValue<std::string>("resources_event", "Texture loaded :" + p_resourceName);
					break;

				case ResourceSystemEvent::RESOURCE_MESHE_LOAD_SUCCESS:
					_MAGE_DEBUG(eventsLogger, "RECV EVENT -> RESOURCE_MESHE_LOAD_SUCCESS : " + p_resourceName);
					dataCloud->updateDataValue<std::string>("resources_event", "Meshe loaded :" + p_resourceName);

					if ("raptor.fbx" == p_resourceName)
					{
						const auto raptor_entity{ m_entitygraph.node("raptorEntity").data() };

						const auto& resources_aspect{ raptor_entity->aspectAccess(core::resourcesAspect::id) };

						const auto& meshe_comp{ resources_aspect.getComponent<std::pair<std::pair<std::string, std::string>, TriangleMeshe>>("meshe") };

						const auto& meshe_descr{ meshe_comp->getPurpose() };
						const TriangleMeshe& meshe{ meshe_descr.second };

						std::unordered_map<std::string, AnimationKeys> animations_list{ meshe.getAnimationsKeys() };

						m_raptor_animations = animations_list;
						m_distribution = new std::uniform_int_distribution<int>(0, m_raptor_animations.size() - 1);
						
						choose_animation();
					}
					break;
			}
		}
	};

	const auto sysEngine{ SystemEngine::getInstance() };
	const auto resourceSystem{ sysEngine->getSystem<mage::ResourceSystem>(resourceSystemSlot) };
	resourceSystem->registerSubscriber(rs_cb);
}

void ModuleImpl::choose_animation()
{
	int anim_index = (*m_distribution)(m_random_engine);
	std::vector<std::string> anims_names;

	for (const auto& e : m_raptor_animations)
	{
		anims_names.push_back(e.first);
	}

	const std::string choosen_anim{ anims_names.at(anim_index) };

	auto& raptorEntityNode{ m_entitygraph.node("raptorEntity") };
	const auto raptorEntity{ raptorEntityNode.data() };
	auto& anims_aspect{ raptorEntity->aspectAccess(core::animationsAspect::id) };
	auto& animationsIdList{ anims_aspect.getComponent<std::list<std::string>>("eg.std.animationsIdList")->getPurpose() };

	animationsIdList.push_back(choosen_anim);
}


void ModuleImpl::d3d11_system_events()
{
	const auto sysEngine{ SystemEngine::getInstance() };
	const auto d3d11System{ sysEngine->getSystem<mage::D3D11System>(d3d11SystemSlot) };

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

					const auto& mainwindows_rendering_aspect{ appwindow->aspectAccess(mage::core::renderingAspect::id) };

					const float characteristics_v_width{ mainwindows_rendering_aspect.getComponent<float>("eg.std.viewportWidth")->getPurpose()};
					const float characteristics_v_height{ mainwindows_rendering_aspect.getComponent<float>("eg.std.viewportHeight")->getPurpose()};


					const auto dataCloud{ mage::rendering::Datacloud::getInstance() };

					const auto window_dims{ dataCloud->readDataValue<mage::core::maths::IntCoords2D>("std.window_resol") };

					const int w_width{ window_dims.x() };
					const int w_height{ window_dims.y() };

					const auto rendering_quad_texture{ Texture(Texture::Format::TEXTURE_RGB, w_width, w_height) };

					mage::helpers::plugRenderingQuadView(m_entitygraph,
						characteristics_v_width, characteristics_v_height,
						"screenRenderingEntity",
						"screenRenderingQuadEntity",
						"ScreenRenderingViewEntity",
						m_windowRenderingQueue,
						"texture_vs",
						"texture_ps",
						{
							std::make_pair(Texture::STAGE_0, rendering_quad_texture)
						}
					);

					// buffer rendering queue
					rendering::Queue bufferRenderingQueue("buffer_pass_queue");
					bufferRenderingQueue.setTargetClearColor({ 0, 0, 0, 255 });
					bufferRenderingQueue.enableTargetClearing(true);
					bufferRenderingQueue.enableTargetDepthClearing(true);
					bufferRenderingQueue.setTargetStage(Texture::STAGE_0);

					mage::helpers::plugRenderingQueue(m_entitygraph, bufferRenderingQueue, "screenRenderingQuadEntity", "bufferRenderingEntity");


					auto& bufferRenderingNode{ m_entitygraph.node("bufferRenderingEntity") };


					/////////// commons shaders params

					dataCloud->registerData<maths::Real4Vector>("texture_keycolor_ps.key_color");
					dataCloud->updateDataValue<maths::Real4Vector>("texture_keycolor_ps.key_color", maths::Real4Vector(0, 0, 0, 1));


					dataCloud->registerData<maths::Real4Vector>("std.light0_dir");
					dataCloud->updateDataValue<maths::Real4Vector>("std.light0_dir", maths::Real4Vector(0, -0.58, 0.6, 1));

					dataCloud->registerData<maths::Real4Vector>("std.fog_color");
					dataCloud->updateDataValue<maths::Real4Vector>("std.fog_color", maths::Real4Vector(0.8, 0.9, 1, 1));

					dataCloud->registerData<maths::Real4Vector>("std.fog_density");
					dataCloud->updateDataValue<maths::Real4Vector>("std.fog_density", maths::Real4Vector(0.009, 0, 0, 0));


					constexpr double groundLevel{ 0 };

					constexpr double skydomeSkyfromspace_ESun{ 8.7 };
					constexpr double skydomeSkyfromatmo_ESun{ 70.0 };
					constexpr double skydomeGroundfromspace_ESun{ 24.0 };
					constexpr double skydomeGroundfromatmo_ESun{ 12.0 };

					constexpr double skydomeAtmoThickness{ 1600.0 };
					constexpr double skydomeOuterRadius{ 70000.0 };
					constexpr double skydomeInnerRadius{ skydomeOuterRadius - skydomeAtmoThickness };

					constexpr double skydomeWaveLength_x{ 0.650 };
					constexpr double skydomeWaveLength_y{ 0.570 };
					constexpr double skydomeWaveLength_z{ 0.475 };
					constexpr double skydomeKm{ 0.0010 };
					constexpr double skydomeKr{ 0.0033 };
					constexpr double skydomeScaleDepth{ 0.25 };


					dataCloud->registerData<maths::Real4Vector>("skydome_ps.atmo_scattering_flag_0");
					dataCloud->updateDataValue<maths::Real4Vector>("skydome_ps.atmo_scattering_flag_0", maths::Real4Vector(skydomeOuterRadius, skydomeInnerRadius, skydomeOuterRadius * skydomeOuterRadius, skydomeInnerRadius * skydomeInnerRadius));

					dataCloud->registerData<maths::Real4Vector>("skydome_ps.atmo_scattering_flag_1");
					dataCloud->updateDataValue<maths::Real4Vector>("skydome_ps.atmo_scattering_flag_1", maths::Real4Vector(skydomeScaleDepth, 1.0 / skydomeScaleDepth, 1.0 / (skydomeOuterRadius - skydomeInnerRadius), (1.0 / (skydomeOuterRadius - skydomeInnerRadius)) / skydomeScaleDepth));

					dataCloud->registerData<maths::Real4Vector>("skydome_ps.atmo_scattering_flag_2");
					dataCloud->updateDataValue<maths::Real4Vector>("skydome_ps.atmo_scattering_flag_2", maths::Real4Vector(1.0 / std::pow(skydomeWaveLength_x, 4.0), 1.0 / std::pow(skydomeWaveLength_y, 4.0), 1.0 / std::pow(skydomeWaveLength_z, 4.0), 0));

					dataCloud->registerData<maths::Real4Vector>("skydome_ps.atmo_scattering_flag_3");
					dataCloud->updateDataValue<maths::Real4Vector>("skydome_ps.atmo_scattering_flag_3", maths::Real4Vector(skydomeKr, skydomeKm, 4.0 * skydomeKr * 3.1415927, 4.0 * skydomeKm * 3.1415927));

					dataCloud->registerData<maths::Real4Vector>("skydome_ps.atmo_scattering_flag_4");
					dataCloud->updateDataValue<maths::Real4Vector>("skydome_ps.atmo_scattering_flag_4", maths::Real4Vector(skydomeSkyfromspace_ESun, skydomeSkyfromatmo_ESun, skydomeGroundfromspace_ESun, skydomeGroundfromatmo_ESun));

					dataCloud->registerData<maths::Real4Vector>("skydome_ps.atmo_scattering_flag_5");
					dataCloud->updateDataValue<maths::Real4Vector>("skydome_ps.atmo_scattering_flag_5", maths::Real4Vector(0.0, 0.0, 0.0, 1));



					///////////////	add ground


					{
						RenderState rs_noculling(RenderState::Operation::SETCULLING, "cw");
						RenderState rs_zbuffer(RenderState::Operation::ENABLEZBUFFER, "true");
						RenderState rs_fill(RenderState::Operation::SETFILLMODE, "solid");
						RenderState rs_texturepointsampling(RenderState::Operation::SETTEXTUREFILTERTYPE, "linear_uvwrap");

						RenderState rs_alphablend(RenderState::Operation::ALPHABLENDENABLE, "false");

						const std::vector<RenderState> ground_rs_list = { rs_noculling, rs_zbuffer, rs_fill, rs_texturepointsampling, rs_alphablend };


						const std::vector< std::pair<size_t, std::pair<std::string, Texture>>> ground_textures{ std::make_pair(Texture::STAGE_0, std::make_pair("grass08.jpg", Texture())) };




						const auto ground_entity{ helpers::plugMesheWithPosition(m_entitygraph, "bufferRenderingEntity", "groundEntity",
														"ground_vs", "ground_ps",
														"ground.ac", "rect",													
														ground_rs_list,
														1000,
														ground_textures
														) };
						
						auto& ground_world_aspect{ ground_entity->aspectAccess(core::worldAspect::id) };

						ground_world_aspect.addComponent<transform::Animator>("animator_positioning", transform::Animator
						(
							{},
							[=](const core::ComponentContainer& p_world_aspect,
								const core::ComponentContainer& p_time_aspect,
								const transform::WorldPosition&,
								const std::unordered_map<std::string, std::string>&)
							{

								maths::Matrix positionmat;
								positionmat.translation(0.0, skydomeInnerRadius + groundLevel, 0.0);

								transform::WorldPosition& wp{ p_world_aspect.getComponent<transform::WorldPosition>("position")->getPurpose() };
								wp.local_pos = wp.local_pos * positionmat;
							}
						));


						auto& ground_rendering_aspect{ ground_entity->aspectAccess(core::renderingAspect::id) };

						rendering::DrawingControl& drawingControl{ ground_rendering_aspect.getComponent<mage::rendering::DrawingControl>("drawingControl")->getPurpose() };

						drawingControl.pshaders_map.push_back(std::make_pair("std.fog_color", "fog_color"));
						drawingControl.pshaders_map.push_back(std::make_pair("std.fog_density", "fog_density"));

						

					}


					/////////////////// add clouds

					{
						RenderState rs_noculling(RenderState::Operation::SETCULLING, "none");
						RenderState rs_zbuffer(RenderState::Operation::ENABLEZBUFFER, "false");
						RenderState rs_fill(RenderState::Operation::SETFILLMODE, "solid");
						RenderState rs_texturepointsampling(RenderState::Operation::SETTEXTUREFILTERTYPE, "linear_uvwrap");

						RenderState rs_alphablend(RenderState::Operation::ALPHABLENDENABLE, "true");
						RenderState rs_alphablendop(RenderState::Operation::ALPHABLENDOP, "add");
						RenderState rs_alphablendfunc(RenderState::Operation::ALPHABLENDFUNC, "always");
						RenderState rs_alphablenddest(RenderState::Operation::ALPHABLENDDEST, "invsrcalpha");
						RenderState rs_alphablendsrc(RenderState::Operation::ALPHABLENDSRC, "srcalpha");

						const std::vector<RenderState> clouds_rs_list = { rs_noculling, rs_zbuffer, rs_fill, rs_texturepointsampling, 
																			rs_alphablend, rs_alphablendop, rs_alphablendfunc, rs_alphablenddest, rs_alphablendsrc
																		};


						const std::vector< std::pair<size_t, std::pair<std::string, Texture>>> ground_textures{ std::make_pair(Texture::STAGE_0, std::make_pair("flatclouds.jpg", Texture())) };




						const auto clouds_entity{ helpers::plugMesheWithPosition(m_entitygraph, "bufferRenderingEntity", "cloudsEntity",
														"flatclouds_vs", "flatclouds_ps",
														"flatclouds.ac", "rect",
														clouds_rs_list,
														999,
														ground_textures
														) };

						auto& clouds_world_aspect{ clouds_entity->aspectAccess(core::worldAspect::id) };

						clouds_world_aspect.addComponent<transform::Animator>("animator_positioning", transform::Animator
						(
							{},
							[=](const core::ComponentContainer& p_world_aspect,
								const core::ComponentContainer& p_time_aspect,
								const transform::WorldPosition&,
								const std::unordered_map<std::string, std::string>&)
							{

								maths::Matrix positionmat;
								positionmat.translation(0.0, skydomeInnerRadius + groundLevel + 400, 0.0);

								transform::WorldPosition& wp{ p_world_aspect.getComponent<transform::WorldPosition>("position")->getPurpose() };
								wp.local_pos = wp.local_pos * positionmat;
							}
						));


					}

					///// add tree

					{
						RenderState rs_noculling(RenderState::Operation::SETCULLING, "none");
						RenderState rs_zbuffer(RenderState::Operation::ENABLEZBUFFER, "true");
						RenderState rs_fill(RenderState::Operation::SETFILLMODE, "solid");
						RenderState rs_texturepointsampling(RenderState::Operation::SETTEXTUREFILTERTYPE, "linear");

						RenderState rs_alphablend(RenderState::Operation::ALPHABLENDENABLE, "false");

						const std::vector<RenderState> tree_rs_list = { rs_noculling, rs_zbuffer, rs_fill, rs_texturepointsampling, rs_alphablend };

						const std::vector< std::pair<size_t, std::pair<std::string, Texture>>> tree_textures{ std::make_pair(Texture::STAGE_0, std::make_pair("tree2_tex.bmp", Texture())) };


						const auto tree_entity{ helpers::plugMesheWithPosition(m_entitygraph, "bufferRenderingEntity", "treeEntity",
														"texture_fog_keycolor_vs", "texture_fog_keycolor_ps",
														"tree0.ac", "Plane.001",
														tree_rs_list,
														1000,
														tree_textures														
														) };

						auto& tree_world_aspect{ tree_entity->aspectAccess(core::worldAspect::id) };

						tree_world_aspect.addComponent<transform::Animator>("animator_positioning", transform::Animator
						(
							{},
							[=](const core::ComponentContainer& p_world_aspect,
								const core::ComponentContainer& p_time_aspect,
								const transform::WorldPosition&,
								const std::unordered_map<std::string, std::string>&)
							{

								maths::Matrix positionmat;
								positionmat.translation(0.0, skydomeInnerRadius + groundLevel, -30.0);

								transform::WorldPosition& wp{ p_world_aspect.getComponent<transform::WorldPosition>("position")->getPurpose() };
								wp.local_pos = wp.local_pos * positionmat;
							}
						));

						auto& tree_rendering_aspect{ tree_entity->aspectAccess(core::renderingAspect::id) };

						rendering::DrawingControl& drawingControl { tree_rendering_aspect.getComponent<mage::rendering::DrawingControl>("drawingControl")->getPurpose() };
						drawingControl.pshaders_map.push_back(std::make_pair("texture_keycolor_ps.key_color", "key_color"));
						drawingControl.pshaders_map.push_back(std::make_pair("std.fog_color", "fog_color"));
						drawingControl.pshaders_map.push_back(std::make_pair("std.fog_density", "fog_density"));


					}

					///// Raptor

					{
						RenderState rs_noculling(RenderState::Operation::SETCULLING, "none");
						RenderState rs_zbuffer(RenderState::Operation::ENABLEZBUFFER, "true");
						RenderState rs_fill(RenderState::Operation::SETFILLMODE, "solid");
						RenderState rs_texturepointsampling(RenderState::Operation::SETTEXTUREFILTERTYPE, "linear");

						RenderState rs_alphablend(RenderState::Operation::ALPHABLENDENABLE, "false");

						const std::vector<RenderState> raptor_rs_list = { rs_noculling, rs_zbuffer, rs_fill, rs_texturepointsampling, rs_alphablend };

						const std::vector< std::pair<size_t, std::pair<std::string, Texture>>> raptor_textures{ std::make_pair(Texture::STAGE_0, std::make_pair("raptorDif2.png", Texture())) };


						const auto raptor_entity{ helpers::plugMesheWithPosition(m_entitygraph, "bufferRenderingEntity", "raptorEntity",
														"texture_fog_keycolor_skanim_vs", "texture_fog_keycolor_skanim_ps",
														"raptor.fbx", "raptorMesh",
														raptor_rs_list,
														1000,
														raptor_textures
														) };

						auto& raptor_world_aspect{ raptor_entity->aspectAccess(core::worldAspect::id) };

						raptor_world_aspect.addComponent<transform::Animator>("animator_positioning", transform::Animator
						(
							{},
							[=](const core::ComponentContainer& p_world_aspect,
								const core::ComponentContainer& p_time_aspect,
								const transform::WorldPosition&,
								const std::unordered_map<std::string, std::string>&)
							{

								maths::Matrix positionmat;
								positionmat.translation(-40.0, skydomeInnerRadius + groundLevel, -30.0);

								maths::Matrix scalemat;
								scalemat.scale(0.03, 0.03, 0.03);


								transform::WorldPosition& wp{ p_world_aspect.getComponent<transform::WorldPosition>("position")->getPurpose() };
								wp.local_pos = /* wp.local_pos * */ scalemat * positionmat;
							}
						));

						auto& raptor_rendering_aspect{ raptor_entity->aspectAccess(core::renderingAspect::id) };

						rendering::DrawingControl& drawingControl{ raptor_rendering_aspect.getComponent<mage::rendering::DrawingControl>("drawingControl")->getPurpose() };
						drawingControl.pshaders_map.push_back(std::make_pair("texture_keycolor_ps.key_color", "key_color"));
						drawingControl.pshaders_map.push_back(std::make_pair("std.fog_color", "fog_color"));
						drawingControl.pshaders_map.push_back(std::make_pair("std.fog_density", "fog_density"));

						auto& raptor_animations_aspect{ raptor_entity->makeAspect(core::animationsAspect::id) };
						raptor_animations_aspect.addComponent<int>("eg.std.animationbonesArrayArgIndex", 0);


						raptor_animations_aspect.addComponent<std::list<std::string>>("eg.std.animationsIdList");
						raptor_animations_aspect.addComponent<std::list<std::pair<std::string, AnimationKeys>>>("eg.std.animationsList");

						raptor_animations_aspect.addComponent<core::TimeMark>("eg.std.animationsTimeMark", TimeControl::getInstance()->buildTimeMark());
						
						raptor_animations_aspect.addComponent<std::string>("eg.std.currentAnimationId");
						raptor_animations_aspect.addComponent<AnimationKeys>("eg.std.currentAnimation");
						

						raptor_animations_aspect.addComponent<double>("eg.std.currentAnimationTicksDuration");
						raptor_animations_aspect.addComponent<double>("eg.std.currentAnimationSecondsDuration");

						raptor_animations_aspect.addComponent<double>("eg.std.currentAnimationTicksProgress");
						raptor_animations_aspect.addComponent<double>("eg.std.currentAnimationSecondsProgress");
					}




					///// skydome

					{

						RenderState rs_noculling(RenderState::Operation::SETCULLING, "none");
						RenderState rs_zbuffer(RenderState::Operation::ENABLEZBUFFER, "false");
						RenderState rs_fill(RenderState::Operation::SETFILLMODE, "solid");
						RenderState rs_texturepointsampling(RenderState::Operation::SETTEXTUREFILTERTYPE, "linear");

						RenderState rs_alphablend(RenderState::Operation::ALPHABLENDENABLE, "true");
						RenderState rs_alphablendop(RenderState::Operation::ALPHABLENDOP, "add");
						RenderState rs_alphablendfunc(RenderState::Operation::ALPHABLENDFUNC, "always");
						RenderState rs_alphablenddest(RenderState::Operation::ALPHABLENDDEST, "invsrcalpha");
						RenderState rs_alphablendsrc(RenderState::Operation::ALPHABLENDSRC, "srcalpha");
						
						const std::vector<RenderState> skydome_rs_list = { rs_noculling, rs_zbuffer, rs_fill, rs_texturepointsampling,
																			rs_alphablend, rs_alphablendop, rs_alphablendfunc, rs_alphablenddest, rs_alphablendsrc
																		};
						

						const auto skydome_entity{ helpers::plugMesheWithPosition(m_entitygraph, "bufferRenderingEntity", "skydomeEntity",
														"skydome_vs", "skydome_ps",
														"skydome.ac", "sphere",
														skydome_rs_list, 900) };


						auto& skydome_world_aspect{ skydome_entity->aspectAccess(core::worldAspect::id) };

						skydome_world_aspect.addComponent<transform::Animator>("animator_positioning", transform::Animator
						(
							{},
							[=](const core::ComponentContainer& p_world_aspect,
								const core::ComponentContainer& p_time_aspect,
								const transform::WorldPosition&,
								const std::unordered_map<std::string, std::string>&)
							{

								maths::Matrix positionmat;
								positionmat.translation(0.0, 0.0, 0.0);

								maths::Matrix scalingmat;
								scalingmat.scale(skydomeOuterRadius, skydomeOuterRadius, skydomeOuterRadius);

								transform::WorldPosition& wp{ p_world_aspect.getComponent<transform::WorldPosition>("position")->getPurpose() };
								wp.local_pos = wp.local_pos * scalingmat * positionmat;
							}
						));

						auto& skydom_rendering_aspect{ skydome_entity->aspectAccess(core::renderingAspect::id) };

						rendering::DrawingControl& drawingControl{ skydom_rendering_aspect.getComponent<mage::rendering::DrawingControl>("drawingControl")->getPurpose() };
						
						drawingControl.pshaders_map.push_back(std::make_pair("std.light0_dir", "light0_dir"));
						drawingControl.pshaders_map.push_back(std::make_pair("skydome_ps.atmo_scattering_flag_0", "atmo_scattering_flag_0"));
						drawingControl.pshaders_map.push_back(std::make_pair("skydome_ps.atmo_scattering_flag_1", "atmo_scattering_flag_1"));
						drawingControl.pshaders_map.push_back(std::make_pair("skydome_ps.atmo_scattering_flag_2", "atmo_scattering_flag_2"));
						drawingControl.pshaders_map.push_back(std::make_pair("skydome_ps.atmo_scattering_flag_3", "atmo_scattering_flag_3"));
						drawingControl.pshaders_map.push_back(std::make_pair("skydome_ps.atmo_scattering_flag_4", "atmo_scattering_flag_4"));
						drawingControl.pshaders_map.push_back(std::make_pair("skydome_ps.atmo_scattering_flag_5", "atmo_scattering_flag_5"));




					}

					/////////////// add camera with gimbal lock jointure ////////////////

					

					auto& gblJointEntityNode{ m_entitygraph.add(bufferRenderingNode, "gblJointEntity") };

					const auto gblJointEntity{ gblJointEntityNode.data() };

					gblJointEntity->makeAspect(core::timeAspect::id);
					auto& gbl_world_aspect{ gblJointEntity->makeAspect(core::worldAspect::id) };

					gbl_world_aspect.addComponent<transform::WorldPosition>("gbl_output");

					gbl_world_aspect.addComponent<double>("gbl_theta", 0);
					gbl_world_aspect.addComponent<double>("gbl_phi", 0);
					gbl_world_aspect.addComponent<double>("gbl_speed", 0);
					gbl_world_aspect.addComponent<maths::Real3Vector>("gbl_pos", maths::Real3Vector(-50.0, skydomeInnerRadius + groundLevel + 5, 1.0));

					gbl_world_aspect.addComponent<transform::Animator>("animator", transform::Animator(
						{
							// input-output/components keys id mapping
							{"gimbalLockJointAnim.theta", "gbl_theta"},
							{"gimbalLockJointAnim.phi", "gbl_phi"},
							{"gimbalLockJointAnim.position", "gbl_pos"},
							{"gimbalLockJointAnim.speed", "gbl_speed"},
							{"gimbalLockJointAnim.output", "gbl_output"}

						}, helpers::animators::makeGimbalLockJointAnimator()));


					// add camera to scene
					maths::Matrix projection;
					projection.perspective(characteristics_v_width, characteristics_v_height, 1.0, 100000.00000000000);
					helpers::plugView(m_entitygraph, projection, "gblJointEntity", "cameraEntity");

					///////Select camera

					core::Entitygraph::Node& bufferRenderingQueueNode{ m_entitygraph.node("bufferRenderingEntity") };
					const auto bufferRenderingQueueEntity{ bufferRenderingQueueNode.data() };
					const auto& renderingAspect{ bufferRenderingQueueEntity->aspectAccess(core::renderingAspect::id) };

					m_bufferRenderingQueue = &renderingAspect.getComponent<rendering::Queue>("renderingQueue")->getPurpose();
					m_bufferRenderingQueue->setCurrentView("cameraEntity");
				}
				break;
			}
		}
	};
	d3d11System->registerSubscriber(d3d11_cb);
}
