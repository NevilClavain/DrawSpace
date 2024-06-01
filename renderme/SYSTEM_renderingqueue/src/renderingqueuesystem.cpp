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

#include "renderingqueuesystem.h"
#include "entity.h"
#include "entitygraph.h"
#include "aspects.h"
#include "ecshelpers.h"
#include "renderingqueue.h"
#include "shader.h"
#include "linemeshe.h"
#include "trianglemeshe.h"

using namespace renderMe;
using namespace renderMe::core;


RenderingQueueSystem::RenderingQueueSystem(Entitygraph& p_entitygraph) : System(p_entitygraph),
m_localLogger("RenderingQueueSystem", renderMe::core::logger::Configuration::getInstance())
{
	////// Register callback to entitygraph

	const Entitygraph::Callback eg_cb
	{
		[&, this](renderMe::core::EntitygraphEvents p_event, const core::Entity& p_removed_entity)
		{
			if (renderMe::core::EntitygraphEvents::ENTITYGRAPHNODE_REMOVED == p_event)
			{		
				rendering::Queue* current_queue{ nullptr };

				for (auto it = m_entitygraph.preBegin(); it != m_entitygraph.preEnd(); ++it)
				{
					const auto current_entity{ it->data() };
					const auto currEntityId{ current_entity->getId() };

					if (current_entity->hasAspect(renderMe::core::renderingAspect::id))
					{
						const auto& rendering_aspect{ current_entity->aspectAccess(renderMe::core::renderingAspect::id) };

						const auto rendering_queue_comp{ rendering_aspect.getComponent<rendering::Queue>("renderingQueue") };
						if (rendering_queue_comp)
						{
							auto& renderingQueue{ rendering_queue_comp->getPurpose() };
							current_queue = &renderingQueue;
						}
					}

					if (currEntityId == p_removed_entity.getId() && 
						current_queue)
					{
						// found the entity that will be removed...

						removeFromRenderingQueue(p_removed_entity.getId(), *current_queue);
					}
				}
			}
		}
	};
	p_entitygraph.registerSubscriber(eg_cb);
}

void RenderingQueueSystem::run()
{
	manageRenderingQueue();
}

void RenderingQueueSystem::requestRenderingqueueLogging(const std::string& p_entityid)
{
	m_queuesToLog.emplace(p_entityid);
}

void RenderingQueueSystem::logRenderingqueue(const std::string& p_entity_id, renderMe::rendering::Queue& p_renderingQueue) const
{
	_RENDERME_DEBUG(m_localLogger, ">>>>>>>>>>>>>>> QUEUE DUMP BEGIN <<<<<<<<<<<<<<<<<<<<<<<<")
	_RENDERME_DEBUG(m_localLogger, "for entity : " + p_entity_id);

	_RENDERME_DEBUG(m_localLogger, "name : " + p_renderingQueue.getName())

	const std::map<rendering::Queue::Purpose, std::string> purpose_translate
	{
		{ rendering::Queue::Purpose::UNDEFINED, "UNDEFINED" },
		{ rendering::Queue::Purpose::SCREEN_RENDERING, "SCREEN_RENDERING" },
		{ rendering::Queue::Purpose::INTERMEDIATE_RENDERING, "INTERMEDIATE_RENDERING" },
	};
	_RENDERME_DEBUG(m_localLogger, "purpose : " + purpose_translate.at(p_renderingQueue.getPurpose()))

	const std::map<rendering::Queue::State, std::string> state_translate
	{
		{ rendering::Queue::State::WAIT_INIT, "WAIT_INIT" },
		{ rendering::Queue::State::READY, "READY" },
		{ rendering::Queue::State::ERROR_ORPHAN, "ERROR_ORPHAN" },
	};
	_RENDERME_DEBUG(m_localLogger, "state : " + state_translate.at(p_renderingQueue.getState()))

	_RENDERME_DEBUG(m_localLogger, "clear_target : " + std::to_string(p_renderingQueue.getTargetClearing()))

	if (p_renderingQueue.getTargetClearing())
	{
		const auto clear_color{ p_renderingQueue.getTargetClearColor() };
		_RENDERME_DEBUG(m_localLogger, "clear_target_color : " + std::to_string(clear_color.r())
														+ " " + std::to_string(clear_color.g()) 
														+ " " + std::to_string(clear_color.b()) 
														+ " " + std::to_string(clear_color.a()))
	}

	// queue node dump
	const auto qnodes{ p_renderingQueue.getQueueNodes() };

	for (const auto& vshaders : qnodes)
	{
		const auto vshader_id{ vshaders.first };
		_RENDERME_DEBUG(m_localLogger, "	-> vshader D3D resource id: " + vshader_id);

		for (const auto& pshaders : vshaders.second.list)
		{
			const auto pshader_id{ pshaders.first };
			_RENDERME_DEBUG(m_localLogger, "		-> pshader D3D resource id: " + pshader_id);

			for (const auto& rs : pshaders.second.list)
			{
				_RENDERME_DEBUG(m_localLogger, "			-> renderstate : " + rs.first);

				for (const auto& linemeshe : rs.second.linemeshes_list)
				{
					const auto linemeshe_id{ linemeshe.first };
					_RENDERME_DEBUG(m_localLogger, "				-> line meshe D3D resource id: " + linemeshe_id);

					for (const auto& drawing : linemeshe.second.list)
					{
						const auto drawing_id{ drawing.first };
						_RENDERME_DEBUG(m_localLogger, "					-> drawing : " + drawing_id);

						const auto drawing_body{ drawing.second };

						if (drawing_body.world)
						{
							_RENDERME_DEBUG(m_localLogger, "						-> world :\n" + drawing_body.world->dump());
						}
						else
						{
							_RENDERME_WARN(m_localLogger, "						-> world : nullptr\n" );
						}

						_RENDERME_DEBUG(m_localLogger, "						-> vshaders params datacloud connexions :");
						for (const auto& cnx : drawing_body.vshaders_map_cnx)
						{
							_RENDERME_DEBUG(m_localLogger, "							-> datacloud var '" + cnx.first + "' mapped on shader input '"
																											+ cnx.second.argument_id
																											+ "' (" + cnx.second.argument_type
																											+ ") for register " + std::to_string(cnx.second.shader_register));
						}

						_RENDERME_DEBUG(m_localLogger, "						-> pshaders params datacloud connexions :");
						for (const auto& cnx : drawing_body.pshaders_map_cnx)
						{
							_RENDERME_DEBUG(m_localLogger, "							-> datacloud var '" + cnx.first + "' mapped on shader input '"
																											+ cnx.second.argument_id
																											+ "' (" + cnx.second.argument_type
																											+ ") for register " + std::to_string(cnx.second.shader_register));
						}

					}
				}

				for (const auto& trianglemeshe : rs.second.trianglemeshes_list)
				{
					const auto triangle_id{ trianglemeshe.first };
					_RENDERME_DEBUG(m_localLogger, "				-> triangle meshe D3D resource id: " + triangle_id);

					for (const auto& drawing : trianglemeshe.second.list)
					{
						const auto drawing_id{ drawing.first };
						_RENDERME_DEBUG(m_localLogger, "					-> drawing : " + drawing_id);

						const auto drawing_body{ drawing.second };

						if (drawing_body.world)
						{
							_RENDERME_DEBUG(m_localLogger, "						-> world :\n" + drawing_body.world->dump());
						}
						else
						{
							_RENDERME_WARN(m_localLogger, "						-> world : nullptr\n");
						}

						_RENDERME_DEBUG(m_localLogger, "						-> vshaders params datacloud connexions :");
						for (const auto& cnx : drawing_body.vshaders_map_cnx)
						{
							_RENDERME_DEBUG(m_localLogger, "							-> datacloud var '" + cnx.first + "' mapped on shader input '"
																											+ cnx.second.argument_id
																											+ "' (" + cnx.second.argument_type
																											+ ") for register " + std::to_string(cnx.second.shader_register));
						}

						_RENDERME_DEBUG(m_localLogger, "						-> pshaders params datacloud connexions :");
						for (const auto& cnx : drawing_body.pshaders_map_cnx)
						{
							_RENDERME_DEBUG(m_localLogger, "							-> datacloud var '" + cnx.first + "' mapped on shader input '"
																											+ cnx.second.argument_id
																											+ "' (" + cnx.second.argument_type
																											+ ") for register " + std::to_string(cnx.second.shader_register));
						}

					}
				}
			}
		}
	}

	_RENDERME_DEBUG(m_localLogger, ">>>>>>>>>>>>>>> QUEUE DUMP END <<<<<<<<<<<<<<<<<<<<<<<<<<");
}

void RenderingQueueSystem::manageRenderingQueue()
{
	////////Queue states//////////////////////////////////////
	{
		const auto forEachRenderingAspect
		{
			[&](Entity* p_entity, const ComponentContainer& p_rendering_aspect)
			{
				const auto rendering_queue_comp{ p_rendering_aspect.getComponent<rendering::Queue>("renderingQueue") };
				if (rendering_queue_comp)
				{
					auto& renderingQueue{ rendering_queue_comp->getPurpose() };
					handleRenderingQueuesState(p_entity, renderingQueue);
				}
			}
		};
		renderMe::helpers::extractAspectsDownTop<renderMe::core::renderingAspect>(m_entitygraph, forEachRenderingAspect);
	}
	////////Queue build/updates/log//////////////////////////////////////
	{
		rendering::Queue* current_queue{ nullptr };

		for (auto it = m_entitygraph.preBegin(); it != m_entitygraph.preEnd(); ++it)
		{
			const auto current_entity{ it->data() };
			const auto currEntityId{ current_entity->getId() };

			if (current_entity->hasAspect(renderMe::core::renderingAspect::id))
			{
				const auto& rendering_aspect{ current_entity->aspectAccess(renderMe::core::renderingAspect::id) };

				const auto rendering_queue_comp{ rendering_aspect.getComponent<rendering::Queue>("renderingQueue") };
				if (rendering_queue_comp)
				{
					auto& renderingQueue{ rendering_queue_comp->getPurpose() };
					current_queue = &renderingQueue;

					//////// check if anu request to log this queue

					if (m_queuesToLog.count(currEntityId))
					{
						logRenderingqueue(currEntityId, renderingQueue);
						m_queuesToLog.erase(currEntityId);
					}

					////////
				}
			}

			if (current_entity->hasAspect(renderMe::core::resourcesAspect::id) &&
				current_entity->hasAspect(renderMe::core::renderingAspect::id) && current_queue)
			{
				const auto& resource_aspect{ current_entity->aspectAccess(renderMe::core::resourcesAspect::id) };
				const auto& rendering_aspect{ current_entity->aspectAccess(renderMe::core::renderingAspect::id) };

				if (current_queue)
				{
					addToRenderingQueue(currEntityId, resource_aspect, rendering_aspect, *current_queue);
				}
			}
		}
	}
	
}

void RenderingQueueSystem::handleRenderingQueuesState(Entity* p_entity, rendering::Queue& p_renderingQueue)
{
	switch (p_renderingQueue.getState())
	{
		case rendering::Queue::State::WAIT_INIT:
		{
			const auto purpose{ p_renderingQueue.getPurpose() };

			if (rendering::Queue::Purpose::UNDEFINED == purpose)
			{
				const auto parent_entity{ p_entity->getParent() };

				if (nullptr == parent_entity)
				{
					p_renderingQueue.setState(rendering::Queue::State::ERROR_ORPHAN);
					// log it (WARN)
					_RENDERME_WARN(m_localLogger, "Rendering queue set to ERROR_ORPHAN : no parent")
				}
				else
				{
					if (parent_entity->hasAspect(core::renderingAspect::id))
					{
						const auto& parent_rendering_aspect{ parent_entity->aspectAccess(core::renderingAspect::id) };
						auto parent_rendering_target_comp{ parent_rendering_aspect.getComponent<core::renderingAspect::renderingTarget>("renderingTarget") };
						if (parent_rendering_target_comp)
						{
							if (core::renderingAspect::renderingTarget::WINDOW_TARGET == parent_rendering_target_comp->getPurpose())
							{
								// WINDOW_TARGET
								// 
								// parent is a screen-target pass 
								// set queue purpose accordingly

								p_renderingQueue.setPurpose(rendering::Queue::Purpose::SCREEN_RENDERING);
								p_renderingQueue.setState(rendering::Queue::State::READY);

								_RENDERME_DEBUG(m_localLogger, "rendering queue " + p_renderingQueue.getName() + " set to READY, SCREEN_RENDERING")
							}
							else
							{
								// BUFFER_TARGET
								// 
								// parent is a texture-target pass
								// set queue purpose accordingly

								p_renderingQueue.setPurpose(rendering::Queue::Purpose::INTERMEDIATE_RENDERING);
								p_renderingQueue.setState(rendering::Queue::State::READY);

								_RENDERME_DEBUG(m_localLogger, "rendering queue " + p_renderingQueue.getName() + " set to READY, INTERMEDIATE_RENDERING")
							}
						}
						else
						{
							// parent rendering aspect has no renderingTarget component !
							p_renderingQueue.setState(rendering::Queue::State::ERROR_ORPHAN);
							// log it (WARN)
							_RENDERME_WARN(m_localLogger, "Rendering queue set to ERROR_ORPHAN : parent rendering aspect has no renderingTarget component")
						}
					}
					else
					{
						// parent has no rendering aspect 
						p_renderingQueue.setState(rendering::Queue::State::ERROR_ORPHAN);
						// log it (WARN)
						_RENDERME_WARN(m_localLogger, "Rendering queue set to ERROR_ORPHAN : parent has no rendering aspect")
					}
				}
			}
		}
		break;
	}
}

static std::string build_rs_list_id(const std::vector<renderMe::rendering::RenderState>& p_rs_list)
{
	std::string rs_set_signature;
	for (const auto& e : p_rs_list)
	{
		rs_set_signature += e.toString() + "; ";
	}

	return rs_set_signature;
}

/*
static void const connect_shaders_args(renderMe::core::logger::Sink& p_localLogger, 
										const renderMe::core::ComponentList<rendering::DrawingControl>& p_linesDrawingControls,
										const renderMe::Shader& p_vshader, const renderMe::Shader& p_pshader)
{
	const auto vshaders_current_args{ p_vshader.getArguments() };
	const auto pshaders_current_args{ p_pshader.getArguments() };

	for (const auto& ldc : p_linesDrawingControls)
	{
		auto& linesDrawingControl{ ldc->getPurpose() };

		_RENDERME_DEBUG(p_localLogger, "connecting shaders argumants for linesDrawingControl of entity: " + linesDrawingControl.owner_entity_id)

		//vshader arguments id match loop
		for (const auto& current_arg : vshaders_current_args)
		{
			const auto argument_id{ current_arg.argument_id };
			for (const auto& connection_pair : linesDrawingControl.vshaders_map)
			{
				if (argument_id == connection_pair.second)
				{
					_RENDERME_DEBUG(p_localLogger, "connecting datacloud variable '" + connection_pair.first + "' on shader arg '" + argument_id
						+ "' of type '" + current_arg.argument_type + "' for register: " + std::to_string(current_arg.shader_register))
						linesDrawingControl.vshaders_map_cnx.push_back(std::make_pair(connection_pair.first, current_arg));
				}
			}
		}

		//pshader arguments id match loop
		for (const auto& current_arg : pshaders_current_args)
		{
			const auto argument_id{ current_arg.argument_id };
			for (const auto& connection_pair : linesDrawingControl.pshaders_map)
			{
				if (argument_id == connection_pair.second)
				{
					_RENDERME_DEBUG(p_localLogger, "connecting datacloud variable '" + connection_pair.first + "' on shader arg '" + argument_id
						+ "' of type '" + current_arg.argument_type + "' for register: " + std::to_string(current_arg.shader_register))
					linesDrawingControl.pshaders_map_cnx.push_back(std::make_pair(connection_pair.first, current_arg));
				}
			}
		}
	}
}
*/

static void const connect_shaders_args(renderMe::core::logger::Sink& p_localLogger,
	const rendering::DrawingControl& p_drawingControl, 
	rendering::QueueDrawingControl& p_queueDrawingControl,
	const renderMe::Shader& p_vshader, const renderMe::Shader& p_pshader)
{
	///////////////////// connect vertex shader args
	const auto vshaders_current_args{ p_vshader.getArguments() };

	//vshader arguments id match loop
	for (const auto& current_arg : vshaders_current_args)
	{
		const auto argument_id{ current_arg.argument_id };
		for (const auto& connection_pair : p_drawingControl.vshaders_map)
		{
			if (argument_id == connection_pair.second)
			{
				_RENDERME_DEBUG(p_localLogger, "connecting datacloud variable '" + connection_pair.first + "' on shader arg '" + argument_id
					+ "' of type '" + current_arg.argument_type + "' for register: " + std::to_string(current_arg.shader_register))

					p_queueDrawingControl.vshaders_map_cnx.push_back(std::make_pair(connection_pair.first, current_arg));
			}
		}
	}

	///////////////////// connect pixel shader args
	const auto pshaders_current_args{ p_pshader.getArguments() };

	//pshader arguments id match loop
	for (const auto& current_arg : pshaders_current_args)
	{
		const auto argument_id{ current_arg.argument_id };
		for (const auto& connection_pair : p_drawingControl.pshaders_map)
		{
			if (argument_id == connection_pair.second)
			{
				_RENDERME_DEBUG(p_localLogger, "connecting datacloud variable '" + connection_pair.first + "' on shader arg '" + argument_id
					+ "' of type '" + current_arg.argument_type + "' for register: " + std::to_string(current_arg.shader_register))

					p_queueDrawingControl.pshaders_map_cnx.push_back(std::make_pair(connection_pair.first, current_arg));
			}
		}
	}
	///////////////////////////////////
}

static rendering::Queue::TriangleMeshePayload build_TriangleMeshePayload(
																const std::vector<RenderingQueueSystem::Callback>& p_cbs,
																renderMe::core::logger::Sink& p_localLogger,
																const renderMe::core::ComponentList<rendering::DrawingControl>& p_trianglesDrawingControls,
																const renderMe::Shader& p_vshader, const renderMe::Shader& p_pshader)
{
	rendering::Queue::TriangleMeshePayload triangleMeshePayload;

	for (const auto& tdc : p_trianglesDrawingControls)
	{
		auto& trianglesDrawingControl{ tdc->getPurpose() };
		trianglesDrawingControl.ready = true;

		rendering::QueueDrawingControl trianglesQueueDrawingControl;
		trianglesQueueDrawingControl.owner_entity_id = trianglesDrawingControl.owner_entity_id;
		trianglesQueueDrawingControl.world = &trianglesDrawingControl.world;
		trianglesQueueDrawingControl.setup = trianglesDrawingControl.setup;
		trianglesQueueDrawingControl.teardown = trianglesDrawingControl.teardown;

		connect_shaders_args(p_localLogger, trianglesDrawingControl, trianglesQueueDrawingControl, p_vshader, p_pshader);

		triangleMeshePayload.list[trianglesDrawingControl.owner_entity_id] = trianglesQueueDrawingControl;


		_RENDERME_DEBUG(p_localLogger, "adding triangles DrawingControl of entity: " + trianglesDrawingControl.owner_entity_id)

		for (const auto& call : p_cbs)
		{
			call(RenderingQueueSystemEvent::TRIANGLEDRAWING_ADDED, trianglesDrawingControl.owner_entity_id);
		}
	}

	return triangleMeshePayload;
}

static rendering::Queue::LineMeshePayload build_LineMeshePayload(
																const std::vector<RenderingQueueSystem::Callback>& p_cbs,
																renderMe::core::logger::Sink& p_localLogger, 
																const renderMe::core::ComponentList<rendering::DrawingControl>& p_linesDrawingControls,
																const renderMe::Shader& p_vshader, const renderMe::Shader& p_pshader)
{
	rendering::Queue::LineMeshePayload lineMeshePayload;

	for (const auto& ldc : p_linesDrawingControls)
	{		
		auto& linesDrawingControl{ ldc->getPurpose() };
		linesDrawingControl.ready = true;

		rendering::QueueDrawingControl linesQueueDrawingControl;
		linesQueueDrawingControl.owner_entity_id = linesDrawingControl.owner_entity_id;
		linesQueueDrawingControl.world = &linesDrawingControl.world;
		linesQueueDrawingControl.setup = linesDrawingControl.setup;
		linesQueueDrawingControl.teardown = linesDrawingControl.teardown;

		connect_shaders_args(p_localLogger, linesDrawingControl, linesQueueDrawingControl, p_vshader, p_pshader);

		lineMeshePayload.list[linesDrawingControl.owner_entity_id] = linesQueueDrawingControl;

		_RENDERME_DEBUG(p_localLogger, "adding lines DrawingControl of entity: " + linesDrawingControl.owner_entity_id)

		for (const auto& call : p_cbs)
		{
			call(RenderingQueueSystemEvent::LINEDRAWING_ADDED, linesDrawingControl.owner_entity_id);
		}
	}

	return lineMeshePayload;
}

rendering::Queue::RenderStatePayload build_RenderStatePayloadWithLineMeshePayload(renderMe::core::logger::Sink& p_localLogger,
																const std::string& p_linemesheId, 
																const rendering::Queue::LineMeshePayload& p_lineMeshePayload, 
																const std::vector<renderMe::rendering::RenderState>& p_rs_list)
{
	rendering::Queue::RenderStatePayload renderStatePayload;

	renderStatePayload.linemeshes_list[p_linemesheId] = p_lineMeshePayload;
	renderStatePayload.description = p_rs_list;

	_RENDERME_DEBUG(p_localLogger, "build new RenderStatePayload with linemeshe id " + p_linemesheId)

	return renderStatePayload;
}

rendering::Queue::RenderStatePayload build_RenderStatePayloadWithTriangleMeshePayload(renderMe::core::logger::Sink& p_localLogger,
																						const std::string& p_trianglemesheId,
																						const rendering::Queue::TriangleMeshePayload& p_triangleMeshePayload,
																						const std::vector<renderMe::rendering::RenderState>& p_rs_list)
{
	rendering::Queue::RenderStatePayload renderStatePayload;

	renderStatePayload.trianglemeshes_list[p_trianglemesheId] = p_triangleMeshePayload;
	renderStatePayload.description = p_rs_list;

	_RENDERME_DEBUG(p_localLogger, "build new RenderStatePayload with trianglemeshe id " + p_trianglemesheId)

	return renderStatePayload;
}

static rendering::Queue::PixelShaderPayload build_pixelShaderPayload(renderMe::core::logger::Sink& p_localLogger, 
																		const std::vector<renderMe::rendering::RenderState>& p_rs_list, 
																		const rendering::Queue::RenderStatePayload& p_renderStatePayload)
{
	rendering::Queue::PixelShaderPayload pixelShaderPayload;

	const auto rs_id{ build_rs_list_id(p_rs_list) };
	pixelShaderPayload.list[rs_id] = p_renderStatePayload;

	_RENDERME_DEBUG(p_localLogger, "build new PixelShaderPayload with renderstate list id " + rs_id)

	return pixelShaderPayload;
}

void RenderingQueueSystem::addToRenderingQueue(const std::string& p_entity_id, const renderMe::core::ComponentContainer& p_resourceAspect,
												const renderMe::core::ComponentContainer& p_renderingAspect, 
												renderMe::rendering::Queue& p_renderingQueue)
{	
	//search for line drawing request
	const auto drawingControls{ p_renderingAspect.getComponentsByType<rendering::DrawingControl>() };

	if (drawingControls.size() > 0)
	{
		bool notAllReady{ false };

		for (const auto& dc : drawingControls)
		{
			auto& drawingControl{ dc->getPurpose() };

			if (!drawingControl.ready)
			{
				notAllReady = true;
				drawingControl.owner_entity_id = p_entity_id;
			}
		}

		if (notAllReady)
		{			
			auto queueNodes{ p_renderingQueue.getQueueNodes() };

			// search for lineMeshe
			const auto lineMeshes{ p_resourceAspect.getComponentsByType<LineMeshe>() };

			// search for triangleMeshe
			const auto triangleMeshes{ p_resourceAspect.getComponentsByType<TriangleMeshe>() };

			// search rendering states
			const auto rsStates{ p_renderingAspect.getComponentsByType<std::vector<renderMe::rendering::RenderState>>() };

			//search for shaders
			const auto shaders{ p_resourceAspect.getComponentsByType<Shader>() };

			if (1 < shaders.size())
			{
				const auto vshader{ shaders.at(0)->getPurpose() };
				const auto pshader{ shaders.at(1)->getPurpose() };

				if (0 == vshader.getType() && 1 == pshader.getType())
				{
					if (Shader::State::RENDERERLOADED == vshader.getState() && Shader::State::RENDERERLOADED == pshader.getState())
					{
						if (rsStates.size() > 0 && (lineMeshes.size() > 0 || triangleMeshes.size() > 0))
						{
							// ok, can update queue

							if (queueNodes.count(vshader.getName()))
							{
								// vshader entry exists

								_RENDERME_DEBUG(m_localLogger, "rendering queue " + p_renderingQueue.getName()
									+ " updated with new entity : " + p_entity_id
									+ " : adding under existing vshader branch : " + vshader.getName())

								auto& vertexShaderPayload{ queueNodes.at(vshader.getName()) };

								if (vertexShaderPayload.list.count(pshader.getName()))
								{
									// pshader entry exists

									_RENDERME_DEBUG(m_localLogger, "rendering queue " + p_renderingQueue.getName()
										+ " updated with new entity : " + p_entity_id
										+ " : adding under existing pshader branch : " + pshader.getName())

									auto& pixelShaderPayload{ vertexShaderPayload.list.at(pshader.getName())};

									const auto rs_list_id{ build_rs_list_id(rsStates.at(0)->getPurpose()) };
									if (pixelShaderPayload.list.count(rs_list_id))
									{
										// renderstates list entry exists

										_RENDERME_DEBUG(m_localLogger, "rendering queue " + p_renderingQueue.getName()
											+ " updated with new entity : " + p_entity_id
											+ " : adding under existing renderstates branch : " + rs_list_id)

										auto& renderStatePayload{ pixelShaderPayload.list.at(rs_list_id) };

										if (lineMeshes.size() > 0)
										{											
											if (renderStatePayload.linemeshes_list.count(lineMeshes.at(0)->getPurpose().getName()))
											{
												// linemeshe entry exists

												_RENDERME_DEBUG(m_localLogger, "rendering queue " + p_renderingQueue.getName()
													+ " updated with new entity : " + p_entity_id
													+ " : adding under existing linemeshe branch : " + lineMeshes.at(0)->getPurpose().getName())
									
												auto& lineMeshePayload{ renderStatePayload.linemeshes_list.at(lineMeshes.at(0)->getPurpose().getName())};

												for (const auto& dc : drawingControls)
												{
													auto& linesDrawingControl{ dc->getPurpose() };
													linesDrawingControl.ready = true;

													rendering::QueueDrawingControl linesQueueDrawingControl;
													linesQueueDrawingControl.owner_entity_id = linesDrawingControl.owner_entity_id;
													linesQueueDrawingControl.world = &linesDrawingControl.world;
													linesQueueDrawingControl.setup = linesDrawingControl.setup;
													linesQueueDrawingControl.teardown = linesDrawingControl.teardown;

													connect_shaders_args(m_localLogger, linesDrawingControl, linesQueueDrawingControl, vshader, pshader);

													lineMeshePayload.list[linesDrawingControl.owner_entity_id] = linesQueueDrawingControl;

													_RENDERME_DEBUG(m_localLogger, "rendering queue " + p_renderingQueue.getName()
														+ " updated with new entity : " + p_entity_id
														+ " : adding linesDrawingControl of entity: " + linesDrawingControl.owner_entity_id)

													for (const auto& call : m_callbacks)
													{
														call(RenderingQueueSystemEvent::LINEDRAWING_ADDED, linesDrawingControl.owner_entity_id);
													}
												}
											}
											else
											{
												// new linemeshe and below elements to add

												_RENDERME_DEBUG(m_localLogger, "rendering queue " + p_renderingQueue.getName()
													+ " updated with new entity : " + p_entity_id
													+ " : adding new linemeshe branch : " + lineMeshes.at(0)->getPurpose().getName())

												const auto lineMeshePayload{ build_LineMeshePayload(m_callbacks, m_localLogger, drawingControls, vshader, pshader) };
												renderStatePayload.linemeshes_list[lineMeshes.at(0)->getPurpose().getName()] = lineMeshePayload;
											}
										}
										else if (triangleMeshes.size() > 0)
										{
											if (renderStatePayload.trianglemeshes_list.count(triangleMeshes.at(0)->getPurpose().getName()))
											{
												// trianglemeshe entry exists

												_RENDERME_DEBUG(m_localLogger, "rendering queue " + p_renderingQueue.getName()
													+ " updated with new entity : " + p_entity_id
													+ " : adding under existing trianglemeshe branch : " + triangleMeshes.at(0)->getPurpose().getName())

												auto& triangleMeshePayload{ renderStatePayload.trianglemeshes_list.at(triangleMeshes.at(0)->getPurpose().getName()) };

												for (const auto& dc : drawingControls)
												{
													auto& trianglesDrawingControl{ dc->getPurpose() };
													trianglesDrawingControl.ready = true;

													rendering::QueueDrawingControl trianglesQueueDrawingControl;
													trianglesQueueDrawingControl.owner_entity_id = trianglesDrawingControl.owner_entity_id;
													trianglesQueueDrawingControl.world = &trianglesDrawingControl.world;
													trianglesQueueDrawingControl.setup = trianglesDrawingControl.setup;
													trianglesQueueDrawingControl.teardown = trianglesDrawingControl.teardown;

													connect_shaders_args(m_localLogger, trianglesDrawingControl, trianglesQueueDrawingControl, vshader, pshader);

													triangleMeshePayload.list[trianglesDrawingControl.owner_entity_id] = trianglesQueueDrawingControl;

													_RENDERME_DEBUG(m_localLogger, "rendering queue " + p_renderingQueue.getName()
														+ " updated with new entity : " + p_entity_id
														+ " : adding trianglesDrawingControl of entity: " + trianglesDrawingControl.owner_entity_id)

													for (const auto& call : m_callbacks)
													{
														call(RenderingQueueSystemEvent::TRIANGLEDRAWING_ADDED, trianglesDrawingControl.owner_entity_id);
													}
												}

											}
											else
											{
												// new trianglemeshe and below elements to add

												_RENDERME_DEBUG(m_localLogger, "rendering queue " + p_renderingQueue.getName()
													+ " updated with new entity : " + p_entity_id
													+ " : adding new trianglemeshe branch : " + triangleMeshes.at(0)->getPurpose().getName())

												const auto triangleMeshePayload{ build_TriangleMeshePayload(m_callbacks, m_localLogger, drawingControls, vshader, pshader) };
												renderStatePayload.trianglemeshes_list[triangleMeshes.at(0)->getPurpose().getName()] = triangleMeshePayload;
											}
										}
									}
									else
									{
										// new renderstate and below elements to add

										_RENDERME_DEBUG(m_localLogger, "rendering queue " + p_renderingQueue.getName()
											+ " updated with new entity : " + p_entity_id
											+ " : adding new renderstate branch : " + rs_list_id)

										rendering::Queue::RenderStatePayload renderStatePayload;
										bool renderStatePayloadSet{ false };

										if (lineMeshes.size() > 0)
										{											
											const auto lineMeshePayload{ build_LineMeshePayload(m_callbacks, m_localLogger, drawingControls, vshader, pshader) };
											renderStatePayload = build_RenderStatePayloadWithLineMeshePayload(m_localLogger, lineMeshes.at(0)->getPurpose().getName(), lineMeshePayload, rsStates.at(0)->getPurpose());

											renderStatePayloadSet = true;
										}
										else if (triangleMeshes.size() > 0)
										{
											const auto triangleMeshePayload{ build_TriangleMeshePayload(m_callbacks, m_localLogger, drawingControls, vshader, pshader) };
											renderStatePayload = build_RenderStatePayloadWithTriangleMeshePayload(m_localLogger, triangleMeshes.at(0)->getPurpose().getName(), triangleMeshePayload, rsStates.at(0)->getPurpose());

											renderStatePayloadSet = true;
										}

										if (renderStatePayloadSet)
										{
											pixelShaderPayload.list[rs_list_id] = renderStatePayload;
										}
										else
										{
											_EXCEPTION("Cannot update queue : no linemeshe or trianglemeshe provided with entity : " + p_entity_id)
										}
									}
								}
								else
								{
									// new pshader and below elements to add

									_RENDERME_DEBUG(m_localLogger, "rendering queue " + p_renderingQueue.getName()
										+ " updated with new entity : " + p_entity_id
										+ " : adding new pshader branch : " + pshader.getName())

									rendering::Queue::RenderStatePayload renderStatePayload;
									bool renderStatePayloadSet{ false };

									if (lineMeshes.size() > 0)
									{
										const auto lineMeshePayload{ build_LineMeshePayload(m_callbacks, m_localLogger, drawingControls, vshader, pshader) };

										// consider only one renderMe::LineMeshe per entity -> lineMeshes.at(0)
										// consider only one std::vector<RenderState> per entity -> rsStates.at(0)
										renderStatePayload = build_RenderStatePayloadWithLineMeshePayload(m_localLogger, lineMeshes.at(0)->getPurpose().getName(), lineMeshePayload, rsStates.at(0)->getPurpose());

										renderStatePayloadSet = true;
									}
									else if (triangleMeshes.size() > 0)
									{
										const auto triangleMeshePayload{ build_TriangleMeshePayload(m_callbacks, m_localLogger, drawingControls, vshader, pshader) };

										// consider only one renderMe::LineMeshe per entity -> lineMeshes.at(0)
										// consider only one std::vector<RenderState> per entity -> rsStates.at(0)
										renderStatePayload = build_RenderStatePayloadWithTriangleMeshePayload(m_localLogger, triangleMeshes.at(0)->getPurpose().getName(), triangleMeshePayload, rsStates.at(0)->getPurpose());

										renderStatePayloadSet = true;
									}

									if (renderStatePayloadSet)
									{
										const auto pixelShaderPayload{ build_pixelShaderPayload(m_localLogger, rsStates.at(0)->getPurpose(), renderStatePayload) };
										vertexShaderPayload.list[pshader.getName()] = pixelShaderPayload;
									}
									else
									{
										_EXCEPTION("Cannot update queue : no linemeshe or trianglemeshe provided with entity : " + p_entity_id)
									}

								}
							}
							else
							{
								// new vshader and below elements to add

								_RENDERME_DEBUG(m_localLogger, "rendering queue " + p_renderingQueue.getName()
									+ " updated with new entity : " + p_entity_id
									+ " : adding new vshader branch : " + vshader.getName())

								rendering::Queue::RenderStatePayload renderStatePayload;
								bool renderStatePayloadSet{ false };

								if (lineMeshes.size() > 0)
								{
									const auto lineMeshePayload{ build_LineMeshePayload(m_callbacks, m_localLogger, drawingControls, vshader, pshader) };

									// consider only one renderMe::LineMeshe per entity -> lineMeshes.at(0)
									// consider only one std::vector<RenderState> per entity -> rsStates.at(0)
									renderStatePayload = build_RenderStatePayloadWithLineMeshePayload(m_localLogger, lineMeshes.at(0)->getPurpose().getName(), lineMeshePayload, rsStates.at(0)->getPurpose());

									renderStatePayloadSet = true;
								}
								else if (triangleMeshes.size() > 0)
								{
									const auto triangleMeshePayload{ build_TriangleMeshePayload(m_callbacks, m_localLogger, drawingControls, vshader, pshader) };

									// consider only one renderMe::LineMeshe per entity -> lineMeshes.at(0)
									// consider only one std::vector<RenderState> per entity -> rsStates.at(0)
									renderStatePayload = build_RenderStatePayloadWithTriangleMeshePayload(m_localLogger, triangleMeshes.at(0)->getPurpose().getName(), triangleMeshePayload, rsStates.at(0)->getPurpose());

									renderStatePayloadSet = true;
								}
								
								if (renderStatePayloadSet)
								{
									const auto pixelShaderPayload{ build_pixelShaderPayload(m_localLogger, rsStates.at(0)->getPurpose(), renderStatePayload) };

									rendering::Queue::VertexShaderPayload vertexShaderPayload;
									vertexShaderPayload.list[pshader.getName()] = pixelShaderPayload;

									_RENDERME_DEBUG(m_localLogger, "build new vertexShaderPayload with pixel shader id " + pshader.getName())

									queueNodes[vshader.getName()] = vertexShaderPayload;

								}
								else
								{
									_EXCEPTION("Cannot update queue : no linemeshe or trianglemeshe provided with entity : " + p_entity_id)
								}
							}							
						}
					}
				}
			}
			p_renderingQueue.setQueueNodes(queueNodes);
		}
	}	
}

void RenderingQueueSystem::removeFromRenderingQueue(const std::string& p_entity_id, renderMe::rendering::Queue& p_renderingQueue)
{
	auto queueNodes{ p_renderingQueue.getQueueNodes() };

	std::vector<std::string> vs_to_remove;

	for (auto& vs : queueNodes)
	{
		std::vector<std::string> ps_to_remove;

		for (auto& ps : vs.second.list)
		{
			std::vector<std::string> rs_to_remove;

			for (auto& rs : ps.second.list)
			{
				//// line meshes
				std::vector<std::string> lm_to_remove;

				for (auto& lm : rs.second.linemeshes_list)
				{
					std::vector<std::string> ldc_to_remove;

					for (const auto& ldc : lm.second.list)
					{						
						if (ldc.second.owner_entity_id == p_entity_id)
						{
							_RENDERME_DEBUG(m_localLogger, "remove lines drawingControl of entity " + p_entity_id)
							// remove this ldc
							ldc_to_remove.push_back(p_entity_id);

							for (const auto& call : m_callbacks)
							{
								call(RenderingQueueSystemEvent::LINEDRAWING_REMOVED, p_entity_id);
							}
						}						
					}

					for (const std::string& id : ldc_to_remove)
					{
						lm.second.list.erase(id);
					}

					if (0 == lm.second.list.size())
					{
						_RENDERME_DEBUG(m_localLogger, "linemeshe payload is now empty, remove linemeshe id : " + lm.first)
						lm_to_remove.push_back(lm.first);
					}
				}

				for (const std::string& id : lm_to_remove)
				{
					rs.second.linemeshes_list.erase(id);
				}
				////////////////////

				//// triangle meshes
				std::vector<std::string> tm_to_remove;

				for (auto& tm : rs.second.trianglemeshes_list)
				{
					std::vector<std::string> tdc_to_remove;

					for (const auto& tdc : tm.second.list)
					{
						if (tdc.second.owner_entity_id == p_entity_id)
						{
							_RENDERME_DEBUG(m_localLogger, "remove triangles drawingControl of entity " + p_entity_id)
							// remove this triangle dc
							tdc_to_remove.push_back(p_entity_id);

							for (const auto& call : m_callbacks)
							{
								call(RenderingQueueSystemEvent::TRIANGLEDRAWING_REMOVED, p_entity_id);
							}
						}
					}

					for (const std::string& id : tdc_to_remove)
					{
						tm.second.list.erase(id);
					}

					if (0 == tm.second.list.size())
					{
						_RENDERME_DEBUG(m_localLogger, "trianglemeshe payload is now empty, remove trianglemeshe id : " + tm.first)
						tm_to_remove.push_back(tm.first);
					}
				}

				for (const std::string& id : tm_to_remove)
				{
					rs.second.trianglemeshes_list.erase(id);
				}

				////////////////////

				if (0 == rs.second.linemeshes_list.size() && 0 == rs.second.trianglemeshes_list.size())
				{
					_RENDERME_DEBUG(m_localLogger, "renderstate payload is now empty, remove renderstate id : " + rs.first)
					rs_to_remove.push_back(rs.first);
				}
			}

			for (const std::string& id : rs_to_remove)
			{
				ps.second.list.erase(id);
			}

			if (0 == ps.second.list.size())
			{
				_RENDERME_DEBUG(m_localLogger, "pixelshader payload is now empty, remove pixelshader id : " + ps.first)
				ps_to_remove.push_back(ps.first);
			}
		}

		for (const std::string& id : ps_to_remove)
		{
			vs.second.list.erase(id);
		}

		if (0 == vs.second.list.size())
		{
			_RENDERME_DEBUG(m_localLogger, "vertexshader payload is now empty, remove vertexshader id : " + vs.first)
			vs_to_remove.push_back(vs.first);
		}
	}

	for (const std::string& id : vs_to_remove)
	{
		queueNodes.erase(id);
	}

	p_renderingQueue.setQueueNodes(queueNodes);
}
