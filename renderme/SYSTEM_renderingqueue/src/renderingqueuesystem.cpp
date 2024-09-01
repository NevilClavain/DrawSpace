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
#include "texture.h"
#include "exceptions.h"

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

						const auto rendering_queues_list{ rendering_aspect.getComponentsByType<rendering::Queue>() };
						if (rendering_queues_list.size() > 0)
						{
							auto& renderingQueue{ rendering_queues_list.at(0)->getPurpose() };
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
		{ rendering::Queue::Purpose::BUFFER_RENDERING, "BUFFER_RENDERING" },
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

	if (!qnodes.size())
	{
		_RENDERME_DEBUG(m_localLogger, "Empty queue")
	}
	else
	{
		for (const auto& qnode : qnodes)
		{
			const int rendering_order{ qnode.first };

			_RENDERME_DEBUG(m_localLogger, "\t-> RENDERING ORDER CHANNEL: [" + std::to_string(rendering_order) + "]");

			const rendering::Queue::RenderingOrderChannel rendering_channel{ qnode.second };

			for (const auto& vshader : rendering_channel.list)
			{
				const auto vshader_id{ vshader.first };
				_RENDERME_DEBUG(m_localLogger, "\t\t-> vshader D3D resource id: " + vshader_id);

				for (const auto& pshader : vshader.second.list)
				{
					const auto pshader_id{ pshader.first };
					_RENDERME_DEBUG(m_localLogger, "\t\t\t-> pshader D3D resource id: " + pshader_id);

					for (const auto& rs : pshader.second.list)
					{
						_RENDERME_DEBUG(m_localLogger, "\t\t\t\t-> renderstate : " + rs.first);

						for (const auto& linemeshe : rs.second.linemeshes_list)
						{
							const auto linemeshe_id{ linemeshe.first };
							_RENDERME_DEBUG(m_localLogger, "\t\t\t\t\t-> line meshe D3D resource id: " + linemeshe_id);

							for (const auto& drawing : linemeshe.second.drawing_list)
							{
								const auto drawing_id{ drawing.first };
								_RENDERME_DEBUG(m_localLogger, "\t\t\t\t\t\t-> drawing : " + drawing_id);

								const auto drawing_body{ drawing.second };

								if (drawing_body.world)
								{
									_RENDERME_DEBUG(m_localLogger, "\t\t\t\t\t\t\t-> world :\n" + drawing_body.world->dump());
								}
								else
								{
									_RENDERME_WARN(m_localLogger, "\t\t\t\t\t\t\t-> world : nullptr\n");
								}

								_RENDERME_DEBUG(m_localLogger, "\t\t\t\t\t\t\t-> vshaders params datacloud connexions :");
								for (const auto& cnx : drawing_body.vshaders_map_cnx)
								{
									_RENDERME_DEBUG(m_localLogger, "\t\t\t\t\t\t\t\t-> datacloud var '" + cnx.first + "' mapped on shader input '"
										+ cnx.second.argument_id
										+ "' (" + cnx.second.argument_type
										+ ") for register " + std::to_string(cnx.second.shader_register));
								}

								_RENDERME_DEBUG(m_localLogger, "\t\t\t\t\t\t\t-> pshaders params datacloud connexions :");
								for (const auto& cnx : drawing_body.pshaders_map_cnx)
								{
									_RENDERME_DEBUG(m_localLogger, "\t\t\t\t\t\t\t\t-> datacloud var '" + cnx.first + "' mapped on shader input '"
										+ cnx.second.argument_id
										+ "' (" + cnx.second.argument_type
										+ ") for register " + std::to_string(cnx.second.shader_register));
								}

							}
						}

						for (const auto& trianglemeshe : rs.second.trianglemeshes_list)
						{
							const auto triangle_id{ trianglemeshe.first };
							_RENDERME_DEBUG(m_localLogger, "\t\t\t\t\t-> triangle meshe D3D resource id: " + triangle_id);

							for (const auto& drawing : trianglemeshe.second.drawing_list)
							{
								const auto drawing_id{ drawing.first };
								_RENDERME_DEBUG(m_localLogger, "\t\t\t\t\t\t-> drawing : " + drawing_id);

								const auto drawing_body{ drawing.second };

								if (drawing_body.world)
								{
									_RENDERME_DEBUG(m_localLogger, "\t\t\t\t\t\t\t-> world :\n" + drawing_body.world->dump());
								}
								else
								{
									_RENDERME_WARN(m_localLogger, "\t\t\t\t\t\t\t-> world : nullptr\n");
								}

								_RENDERME_DEBUG(m_localLogger, "\t\t\t\t\t\t\t-> vshaders params datacloud connexions :");
								for (const auto& cnx : drawing_body.vshaders_map_cnx)
								{
									_RENDERME_DEBUG(m_localLogger, "\t\t\t\t\t\t\t\t-> datacloud var '" + cnx.first + "' mapped on shader input '"
										+ cnx.second.argument_id
										+ "' (" + cnx.second.argument_type
										+ ") for register " + std::to_string(cnx.second.shader_register));
								}

								_RENDERME_DEBUG(m_localLogger, "\t\t\t\t\t\t\t-> pshaders params datacloud connexions :");
								for (const auto& cnx : drawing_body.pshaders_map_cnx)
								{
									_RENDERME_DEBUG(m_localLogger, "\t\t\t\t\t\t\t\t-> datacloud var '" + cnx.first + "' mapped on shader input '"
										+ cnx.second.argument_id
										+ "' (" + cnx.second.argument_type
										+ ") for register " + std::to_string(cnx.second.shader_register));
								}

							}

							for (const auto& texture_set_list : trianglemeshe.second.textures_set_list)
							{
								const rendering::Queue::TextureSetPayload textureSetPayload{ texture_set_list.second };

								for (const auto& staged_texture : textureSetPayload.textures)
								{
									const size_t	stage{ staged_texture.first };
									const Texture& texture{ staged_texture.second };

									_RENDERME_WARN(m_localLogger, "\t\t\t\t\t\t\t\t-> texture : stage " + std::to_string(stage) + " " + texture.getName() + " " +
										(Texture::Source::CONTENT_FROM_FILE == texture.getSource() ? "CONTENT_FROM_FILE" : "CONTENT_FROM_RENDERINGQUEUE"));



									for (const auto& drawing : textureSetPayload.drawing_list)
									{
										const auto drawing_id{ drawing.first };
										_RENDERME_DEBUG(m_localLogger, "\t\t\t\t\t\t\t-> drawing : " + drawing_id);

										const auto drawing_body{ drawing.second };

										if (drawing_body.world)
										{
											_RENDERME_DEBUG(m_localLogger, "\t\t\t\t\t\t\t\t-> world :\n" + drawing_body.world->dump());
										}
										else
										{
											_RENDERME_WARN(m_localLogger, "\t\t\t\t\t\t\t\t-> world : nullptr\n");
										}

										_RENDERME_DEBUG(m_localLogger, "\t\t\t\t\t\t\t\t-> vshaders params datacloud connexions :");
										for (const auto& cnx : drawing_body.vshaders_map_cnx)
										{
											_RENDERME_DEBUG(m_localLogger, "\t\t\t\t\t\t\t\t\t-> datacloud var '" + cnx.first + "' mapped on shader input '"
												+ cnx.second.argument_id
												+ "' (" + cnx.second.argument_type
												+ ") for register " + std::to_string(cnx.second.shader_register));
										}

										_RENDERME_DEBUG(m_localLogger, "\t\t\t\t\t\t\t\t-> pshaders params datacloud connexions :");
										for (const auto& cnx : drawing_body.pshaders_map_cnx)
										{
											_RENDERME_DEBUG(m_localLogger, "\t\t\t\t\t\t\t\t\t-> datacloud var '" + cnx.first + "' mapped on shader input '"
												+ cnx.second.argument_id
												+ "' (" + cnx.second.argument_type
												+ ") for register " + std::to_string(cnx.second.shader_register));
										}

									}
								}
							}
						}
					}
				}
			}
		}
	}

	_RENDERME_DEBUG(m_localLogger, ">>>>>>>>>>>>>>> QUEUE DUMP END <<<<<<<<<<<<<<<<<<<<<<<<<<");
}

static rendering::Queue* searchRenderingQueueInAncestors(core::Entity* p_entity)
{
	rendering::Queue* rqueue { nullptr };
	core::Entity* curr_parent{ p_entity->getParent() };

	while (curr_parent)
	{
		if (curr_parent->hasAspect(renderMe::core::renderingAspect::id))
		{
			const auto& rendering_aspect{ curr_parent->aspectAccess(renderMe::core::renderingAspect::id) };

			const auto rendering_queues_list{ rendering_aspect.getComponentsByType<rendering::Queue>() };
			if (rendering_queues_list.size() > 0)
			{
				auto& renderingQueue{ rendering_queues_list.at(0)->getPurpose() };
				rqueue = &renderingQueue;
				break;
			}
		}
		curr_parent = curr_parent->getParent();
	}
	return rqueue;
}

void RenderingQueueSystem::manageRenderingQueue()
{
	////////Queue states//////////////////////////////////////
	{
		const auto forEachRenderingAspect
		{
			[&](Entity* p_entity, const ComponentContainer& p_rendering_aspect)
			{
				const auto rendering_queues_list{ p_rendering_aspect.getComponentsByType<rendering::Queue>() };
				if (rendering_queues_list.size() > 0)
				{
					auto& renderingQueue{ rendering_queues_list.at(0)->getPurpose() };
					handleRenderingQueuesState(p_entity, renderingQueue);
				}
			}
		};
		renderMe::helpers::extractAspectsDownTop<renderMe::core::renderingAspect>(m_entitygraph, forEachRenderingAspect);
	}
	////////Queue build/updates/log//////////////////////////////////////
	{
		//rendering::Queue* current_queue{ nullptr };

		for (auto it = m_entitygraph.preBegin(); it != m_entitygraph.preEnd(); ++it)
		{
			const auto current_entity{ it->data() };
			const auto currEntityId{ current_entity->getId() };

			//////// check if request to log this queue
			if (current_entity->hasAspect(renderMe::core::renderingAspect::id))
			{
				const auto& rendering_aspect{ current_entity->aspectAccess(renderMe::core::renderingAspect::id) };

				const auto rendering_queues_list{ rendering_aspect.getComponentsByType<rendering::Queue>() };
				if (rendering_queues_list.size() > 0)
				{
					auto& renderingQueue{ rendering_queues_list.at(0)->getPurpose() };					

					if (m_queuesToLog.count(currEntityId))
					{
						logRenderingqueue(currEntityId, renderingQueue);
						m_queuesToLog.erase(currEntityId);
					}

					////////
				}
			}
			/////////////////////////////////////////

			// search for rendering queue over this current entity

			rendering::Queue* current_queue{ searchRenderingQueueInAncestors(current_entity) };

			if (current_entity->hasAspect(renderMe::core::resourcesAspect::id) &&
				current_entity->hasAspect(renderMe::core::renderingAspect::id) && current_queue)
			{
				const auto& resource_aspect{ current_entity->aspectAccess(renderMe::core::resourcesAspect::id) };
				const auto& rendering_aspect{ current_entity->aspectAccess(renderMe::core::renderingAspect::id) };

				if (current_queue)
				{
					checkEntityInsertion(currEntityId, resource_aspect, rendering_aspect, *current_queue);
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
							if (core::renderingAspect::renderingTarget::SCREEN_RENDERINGTARGET == parent_rendering_target_comp->getPurpose())
							{
								// SCREEN_RENDERINGTARGET
								// 
								// parent is a screen-target pass 
								// set queue purpose accordingly

								p_renderingQueue.setScreenRenderingPurpose();
								_RENDERME_DEBUG(m_localLogger, "rendering queue " + p_renderingQueue.getName() + " set to READY, SCREEN_RENDERING")
							}
							else
							{
								// BUFFER_RENDERINGTARGET
								// 
								// parent is a texture-target pass
								// set queue purpose accordingly

								// parent is a texture-target pass
								// search for a target texture in it

								// search in resource aspect

								const auto& parent_resource_aspect{ parent_entity->aspectAccess(core::resourcesAspect::id) };
								const ComponentList<std::pair<size_t, renderMe::Texture>> textures_list{ parent_resource_aspect.getComponentsByType<std::pair<size_t,renderMe::Texture>>() };

								p_renderingQueue.setBufferRenderingPurpose(textures_list);

								_RENDERME_DEBUG(m_localLogger, "rendering queue " + p_renderingQueue.getName() + " set to READY, BUFFER_RENDERING")
							}

							p_renderingQueue.setState(rendering::Queue::State::READY);
						}
						else
						{
							// parent rendering aspect has no renderingTarget component !
							p_renderingQueue.setState(rendering::Queue::State::ERROR_ORPHAN);

							_EXCEPTION("Rendering queue set to ERROR_ORPHAN : parent rendering aspect has no renderingTarget component : " + p_renderingQueue.getName() + ", parent is " + parent_entity->getId());
							// log it (WARN)
							//_RENDERME_WARN(m_localLogger, "Rendering queue set to ERROR_ORPHAN : parent rendering aspect has no renderingTarget component")							
						}
					}
					else
					{
						// parent has no rendering aspect 
						p_renderingQueue.setState(rendering::Queue::State::ERROR_ORPHAN);

						_EXCEPTION("Rendering queue set to ERROR_ORPHAN : parent rendering aspect has no rendering aspect : " + p_renderingQueue.getName() + ", parent is " + parent_entity->getId());

						// log it (WARN)
						//_RENDERME_WARN(m_localLogger, "Rendering queue set to ERROR_ORPHAN : parent has no rendering aspect")
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

static rendering::Queue::TriangleMeshePayload build_TriangleMesheAndTexturesPayload(
																const std::vector<RenderingQueueSystem::Callback>& p_cbs,
																renderMe::core::logger::Sink& p_localLogger,
																const renderMe::core::ComponentList<rendering::DrawingControl>& p_trianglesDrawingControls,
																const renderMe::core::ComponentList<std::pair<size_t, Texture>>& p_texturesSet,
																const renderMe::Shader& p_vshader, const renderMe::Shader& p_pshader)
{
	rendering::Queue::TriangleMeshePayload triangleMeshePayload;

	if (0 == p_texturesSet.size())
	{
		for (const auto& tdc : p_trianglesDrawingControls)
		{
			auto& trianglesDrawingControl{ tdc->getPurpose() };
			trianglesDrawingControl.ready = true;

			rendering::QueueDrawingControl trianglesQueueDrawingControl;
			trianglesQueueDrawingControl.owner_entity_id = trianglesDrawingControl.owner_entity_id;
			trianglesQueueDrawingControl.world = &trianglesDrawingControl.world;

			trianglesQueueDrawingControl.setup = &trianglesDrawingControl.setup;
			trianglesQueueDrawingControl.teardown = &trianglesDrawingControl.teardown;
			trianglesQueueDrawingControl.wvpFilter = &trianglesDrawingControl.wvpFilter;

			connect_shaders_args(p_localLogger, trianglesDrawingControl, trianglesQueueDrawingControl, p_vshader, p_pshader);

			triangleMeshePayload.drawing_list[trianglesDrawingControl.owner_entity_id] = trianglesQueueDrawingControl;

			_RENDERME_DEBUG(p_localLogger, "adding triangles DrawingControl of entity: " + trianglesDrawingControl.owner_entity_id)

			for (const auto& call : p_cbs)
			{
				call(RenderingQueueSystemEvent::TRIANGLEDRAWING_ADDED, trianglesDrawingControl.owner_entity_id);
			}
		}
	}
	else
	{
		// build textureSet payload
		rendering::Queue::TextureSetPayload textureSetPayload;
		std::string textureset_signature;

		for (const auto& e : p_texturesSet)
		{
			const auto& staged_texture { e->getPurpose() };

			const size_t stage{ staged_texture.first };
			const Texture& texture{ staged_texture.second };
			textureSetPayload.textures[stage] = texture.getName();

			textureset_signature += texture.getName() + "." + std::to_string(stage) + "/";
		}

		for (const auto& tdc : p_trianglesDrawingControls)
		{
			auto& trianglesDrawingControl{ tdc->getPurpose() };
			trianglesDrawingControl.ready = true;

			rendering::QueueDrawingControl trianglesQueueDrawingControl;
			trianglesQueueDrawingControl.owner_entity_id = trianglesDrawingControl.owner_entity_id;
			trianglesQueueDrawingControl.world = &trianglesDrawingControl.world;
			trianglesQueueDrawingControl.setup = &trianglesDrawingControl.setup;
			trianglesQueueDrawingControl.teardown = &trianglesDrawingControl.teardown;
			trianglesQueueDrawingControl.wvpFilter = &trianglesDrawingControl.wvpFilter;

			connect_shaders_args(p_localLogger, trianglesDrawingControl, trianglesQueueDrawingControl, p_vshader, p_pshader);

			textureSetPayload.drawing_list[trianglesDrawingControl.owner_entity_id] = trianglesQueueDrawingControl;

			_RENDERME_DEBUG(p_localLogger, "adding triangles DrawingControl of entity: " + trianglesDrawingControl.owner_entity_id)

			for (const auto& call : p_cbs)
			{
				call(RenderingQueueSystemEvent::TRIANGLEDRAWING_ADDED, trianglesDrawingControl.owner_entity_id);
			}
		}
	
		triangleMeshePayload.textures_set_list[textureset_signature] = textureSetPayload;

	}
	return triangleMeshePayload;
}

static rendering::Queue::LineMeshePayload build_LineMeshePayload(const std::vector<RenderingQueueSystem::Callback>& p_cbs,
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
		linesQueueDrawingControl.setup = &linesDrawingControl.setup;
		linesQueueDrawingControl.teardown = &linesDrawingControl.teardown;
		linesQueueDrawingControl.wvpFilter = &linesDrawingControl.wvpFilter;

		connect_shaders_args(p_localLogger, linesDrawingControl, linesQueueDrawingControl, p_vshader, p_pshader);

		lineMeshePayload.drawing_list[linesDrawingControl.owner_entity_id] = linesQueueDrawingControl;

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

void RenderingQueueSystem::checkEntityInsertion(const std::string& p_entity_id, const renderMe::core::ComponentContainer& p_resourceAspect,
												const renderMe::core::ComponentContainer& p_renderingAspect, 
												renderMe::rendering::Queue& p_renderingQueue)
{	
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

			// search for shaders
			const auto shaders{ p_resourceAspect.getComponentsByType<Shader>() };

			// search for textures set
			const auto texturesSet{ p_resourceAspect.getComponentsByType<std::pair<size_t,Texture>>() };

			// Trendering order channel : 0 by default
			int rendering_channel{ 0 };

			const auto rocs{ p_renderingAspect.getComponentsByType<int>() };
			if (rocs.size() > 0)
			{
				rendering_channel = rocs.at(0)->getPurpose();
			}


			if (1 < shaders.size())
			{
				const auto vshader{ shaders.at(0)->getPurpose() };
				const auto pshader{ shaders.at(1)->getPurpose() };

				if (0 == vshader.getType() && 1 == pshader.getType())
				{

					bool resources_D3D11ready{ true };

					//////////////////////////////// check shaders are D3D11 ready

					if (Shader::State::RENDERERLOADED != vshader.getState() || Shader::State::RENDERERLOADED != pshader.getState())
					{
						resources_D3D11ready = false;
					}

					//////////////////////////////// check meshes are D3D11 ready

					if (lineMeshes.size() > 0)
					{
						if (LineMeshe::State::RENDERERLOADED != lineMeshes.at(0)->getPurpose().getState())
						{
							resources_D3D11ready = false;
						}
					}

					if (triangleMeshes.size() > 0)
					{
						if (TriangleMeshe::State::RENDERERLOADED != triangleMeshes.at(0)->getPurpose().getState())
						{
							resources_D3D11ready = false;
						}
					}

					//////////////////////////////// check textures are D3D11 ready

					for (const auto& e : texturesSet)
					{
						const auto& staged_texture{ e->getPurpose() };

						if (Texture::State::RENDERERLOADED != staged_texture.second.getState())
						{
							resources_D3D11ready = false;
						}
					}

					if (resources_D3D11ready && rsStates.size() > 0 && (lineMeshes.size() > 0 || triangleMeshes.size() > 0))
					{
						// ok, can update queue
						
						if (!queueNodes.count(rendering_channel)) 
						{
							rendering::Queue::RenderingOrderChannel renderingOrderChannel;
							queueNodes[rendering_channel] = renderingOrderChannel;
						}

						if (queueNodes.at(rendering_channel).list.count(vshader.getName()))
						{
							// vshader entry exists

							_RENDERME_DEBUG(m_localLogger, "rendering queue " + p_renderingQueue.getName()
								+ " updated with new entity : " + p_entity_id
								+ " : adding under existing vshader branch : " + vshader.getName())

							auto& vertexShaderPayload{ queueNodes.at(rendering_channel).list.at(vshader.getName()) };

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
										if (renderStatePayload.linemeshes_list.count(lineMeshes.at(0)->getPurpose().getMd5()))
										{
											// linemeshe entry exists

											_RENDERME_DEBUG(m_localLogger, "rendering queue " + p_renderingQueue.getName()
												+ " updated with new entity : " + p_entity_id
												+ " : adding under existing linemeshe branch : " + lineMeshes.at(0)->getPurpose().getMd5())
									
											auto& lineMeshePayload{ renderStatePayload.linemeshes_list.at(lineMeshes.at(0)->getPurpose().getMd5())};

											for (const auto& dc : drawingControls)
											{
												auto& linesDrawingControl{ dc->getPurpose() };
												linesDrawingControl.ready = true;

												rendering::QueueDrawingControl linesQueueDrawingControl;
												linesQueueDrawingControl.owner_entity_id = linesDrawingControl.owner_entity_id;
												linesQueueDrawingControl.world = &linesDrawingControl.world;
												linesQueueDrawingControl.setup = &linesDrawingControl.setup;
												linesQueueDrawingControl.teardown = &linesDrawingControl.teardown;
												linesQueueDrawingControl.wvpFilter = &linesDrawingControl.wvpFilter;

												connect_shaders_args(m_localLogger, linesDrawingControl, linesQueueDrawingControl, vshader, pshader);

												lineMeshePayload.drawing_list[linesDrawingControl.owner_entity_id] = linesQueueDrawingControl;

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
												+ " : adding new linemeshe branch : " + lineMeshes.at(0)->getPurpose().getMd5())

											const auto lineMeshePayload{ build_LineMeshePayload(m_callbacks, m_localLogger, drawingControls, vshader, pshader) };
											renderStatePayload.linemeshes_list[lineMeshes.at(0)->getPurpose().getMd5()] = lineMeshePayload;
										}
									}
									else if (triangleMeshes.size() > 0)
									{
										if (renderStatePayload.trianglemeshes_list.count(triangleMeshes.at(0)->getPurpose().getMd5()))
										{
											// trianglemeshe entry exists
											auto& triangleMeshePayload{ renderStatePayload.trianglemeshes_list.at(triangleMeshes.at(0)->getPurpose().getMd5()) };

											if (0 == texturesSet.size())
											{
												// no textures associated, add directly new drawing control

												_RENDERME_DEBUG(m_localLogger, "rendering queue " + p_renderingQueue.getName()
													+ " updated with new entity : " + p_entity_id
													+ " : adding under existing trianglemeshe branch : " + triangleMeshes.at(0)->getPurpose().getMd5())
													
												for (const auto& dc : drawingControls)
												{
													auto& trianglesDrawingControl{ dc->getPurpose() };
													trianglesDrawingControl.ready = true;

													rendering::QueueDrawingControl trianglesQueueDrawingControl;
													trianglesQueueDrawingControl.owner_entity_id = trianglesDrawingControl.owner_entity_id;
													trianglesQueueDrawingControl.world = &trianglesDrawingControl.world;
													trianglesQueueDrawingControl.setup = &trianglesDrawingControl.setup;
													trianglesQueueDrawingControl.teardown = &trianglesDrawingControl.teardown;
													trianglesQueueDrawingControl.wvpFilter = &trianglesDrawingControl.wvpFilter;

													connect_shaders_args(m_localLogger, trianglesDrawingControl, trianglesQueueDrawingControl, vshader, pshader);

													triangleMeshePayload.drawing_list[trianglesDrawingControl.owner_entity_id] = trianglesQueueDrawingControl;

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
												// textureset signature
												std::string textureset_signature;
												for (const auto& e : texturesSet)
												{
													const auto& staged_texture{ e->getPurpose() };

													const size_t stage{ staged_texture.first };
													const Texture& texture{ staged_texture.second };
													textureset_signature += texture.getName() + "." + std::to_string(stage) + "/";
												}

												// does this textureSet signature exists ?
												if (triangleMeshePayload.textures_set_list.count(textureset_signature))
												{
													// add new drawing control under this textureSet

													auto& textureSetPayload{ triangleMeshePayload.textures_set_list.at(textureset_signature)};

													for (const auto& dc : drawingControls)
													{
														auto& trianglesDrawingControl{ dc->getPurpose() };
														trianglesDrawingControl.ready = true;

														rendering::QueueDrawingControl trianglesQueueDrawingControl;
														trianglesQueueDrawingControl.owner_entity_id = trianglesDrawingControl.owner_entity_id;
														trianglesQueueDrawingControl.world = &trianglesDrawingControl.world;
														trianglesQueueDrawingControl.setup = &trianglesDrawingControl.setup;
														trianglesQueueDrawingControl.teardown = &trianglesDrawingControl.teardown;
														trianglesQueueDrawingControl.wvpFilter = &trianglesDrawingControl.wvpFilter;

														connect_shaders_args(m_localLogger, trianglesDrawingControl, trianglesQueueDrawingControl, vshader, pshader);

														textureSetPayload.drawing_list[trianglesDrawingControl.owner_entity_id] = trianglesQueueDrawingControl;

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
													// add this new textureset + drawing control under new texturset

													rendering::Queue::TextureSetPayload textureSetPayload;

													for (const auto& e : texturesSet)
													{
														const auto& staged_texture{ e->getPurpose() };

														const size_t stage{ staged_texture.first };
														const Texture& texture{ staged_texture.second };
														textureSetPayload.textures[stage] = texture.getName();
													}

													for (const auto& dc : drawingControls)
													{
														auto& trianglesDrawingControl{ dc->getPurpose() };
														trianglesDrawingControl.ready = true;

														rendering::QueueDrawingControl trianglesQueueDrawingControl;
														trianglesQueueDrawingControl.owner_entity_id = trianglesDrawingControl.owner_entity_id;
														trianglesQueueDrawingControl.world = &trianglesDrawingControl.world;
														trianglesQueueDrawingControl.setup = &trianglesDrawingControl.setup;
														trianglesQueueDrawingControl.teardown = &trianglesDrawingControl.teardown;
														trianglesQueueDrawingControl.wvpFilter = &trianglesDrawingControl.wvpFilter;

														connect_shaders_args(m_localLogger, trianglesDrawingControl, trianglesQueueDrawingControl, vshader, pshader);

														textureSetPayload.drawing_list[trianglesDrawingControl.owner_entity_id] = trianglesQueueDrawingControl;

														_RENDERME_DEBUG(m_localLogger, "rendering queue " + p_renderingQueue.getName()
															+ " updated with new entity : " + p_entity_id
															+ " : adding trianglesDrawingControl of entity: " + trianglesDrawingControl.owner_entity_id)

														for (const auto& call : m_callbacks)
														{
															call(RenderingQueueSystemEvent::TRIANGLEDRAWING_ADDED, trianglesDrawingControl.owner_entity_id);
														}
													}

													triangleMeshePayload.textures_set_list[textureset_signature] = textureSetPayload;
												}
											}
										}
										else
										{
											// new trianglemeshe and below elements to add

											_RENDERME_DEBUG(m_localLogger, "rendering queue " + p_renderingQueue.getName()
												+ " updated with new entity : " + p_entity_id
												+ " : adding new trianglemeshe branch : " + triangleMeshes.at(0)->getPurpose().getMd5())

											const auto triangleMeshePayload{ build_TriangleMesheAndTexturesPayload(m_callbacks, m_localLogger, drawingControls, texturesSet, vshader, pshader) };
											renderStatePayload.trianglemeshes_list[triangleMeshes.at(0)->getPurpose().getMd5()] = triangleMeshePayload;
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
										renderStatePayload = build_RenderStatePayloadWithLineMeshePayload(m_localLogger, lineMeshes.at(0)->getPurpose().getMd5(), lineMeshePayload, rsStates.at(0)->getPurpose());

										renderStatePayloadSet = true;
									}
									else if (triangleMeshes.size() > 0)
									{
										const auto triangleMeshePayload{ build_TriangleMesheAndTexturesPayload(m_callbacks, m_localLogger, drawingControls, texturesSet, vshader, pshader) };
										renderStatePayload = build_RenderStatePayloadWithTriangleMeshePayload(m_localLogger, triangleMeshes.at(0)->getPurpose().getMd5(), triangleMeshePayload, rsStates.at(0)->getPurpose());

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
									renderStatePayload = build_RenderStatePayloadWithLineMeshePayload(m_localLogger, lineMeshes.at(0)->getPurpose().getMd5(), lineMeshePayload, rsStates.at(0)->getPurpose());

									renderStatePayloadSet = true;
								}
								else if (triangleMeshes.size() > 0)
								{
									const auto triangleMeshePayload{ build_TriangleMesheAndTexturesPayload(m_callbacks, m_localLogger, drawingControls, texturesSet, vshader, pshader) };

									// consider only one renderMe::LineMeshe per entity -> lineMeshes.at(0)
									// consider only one std::vector<RenderState> per entity -> rsStates.at(0)
									renderStatePayload = build_RenderStatePayloadWithTriangleMeshePayload(m_localLogger, triangleMeshes.at(0)->getPurpose().getMd5(), triangleMeshePayload, rsStates.at(0)->getPurpose());

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
								renderStatePayload = build_RenderStatePayloadWithLineMeshePayload(m_localLogger, lineMeshes.at(0)->getPurpose().getMd5(), lineMeshePayload, rsStates.at(0)->getPurpose());

								renderStatePayloadSet = true;
							}
							else if (triangleMeshes.size() > 0)
							{
								const auto triangleMeshePayload{ build_TriangleMesheAndTexturesPayload(m_callbacks, m_localLogger, drawingControls, texturesSet, vshader, pshader) };

								// consider only one renderMe::LineMeshe per entity -> lineMeshes.at(0)
								// consider only one std::vector<RenderState> per entity -> rsStates.at(0)
								renderStatePayload = build_RenderStatePayloadWithTriangleMeshePayload(m_localLogger, triangleMeshes.at(0)->getPurpose().getMd5(), triangleMeshePayload, rsStates.at(0)->getPurpose());

								renderStatePayloadSet = true;
							}
								
							if (renderStatePayloadSet)
							{
								const auto pixelShaderPayload{ build_pixelShaderPayload(m_localLogger, rsStates.at(0)->getPurpose(), renderStatePayload) };

								rendering::Queue::VertexShaderPayload vertexShaderPayload;
								vertexShaderPayload.list[pshader.getName()] = pixelShaderPayload;

								_RENDERME_DEBUG(m_localLogger, "build new vertexShaderPayload with pixel shader id " + pshader.getName())

								//queueNodes[vshader.getName()] = vertexShaderPayload;

								queueNodes.at(rendering_channel).list[vshader.getName()] = vertexShaderPayload;

							}
							else
							{
								_EXCEPTION("Cannot update queue : no linemeshe or trianglemeshe provided with entity : " + p_entity_id)
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

	std::vector<int> roc_to_remove;

	for (auto& qnode : queueNodes)
	{
		rendering::Queue::RenderingOrderChannel& rendering_channel{ qnode.second };

		std::vector<std::string> vs_to_remove;
		for (auto& vs : rendering_channel.list)
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

						for (const auto& ldc : lm.second.drawing_list)
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
							lm.second.drawing_list.erase(id);
						}

						if (0 == lm.second.drawing_list.size())
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

						for (const auto& tdc : tm.second.drawing_list)
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
							tm.second.drawing_list.erase(id);
						}

						/////////// about textures set

						std::vector<std::string> tsl_to_remove;

						for (auto& tsl : tm.second.textures_set_list)
						{
							std::vector<std::string> tdc_to_remove_2;

							for (const auto& tdc : tsl.second.drawing_list)
							{
								if (tdc.second.owner_entity_id == p_entity_id)
								{
									_RENDERME_DEBUG(m_localLogger, "remove triangles drawingControl of entity " + p_entity_id)
									// remove this triangle dc
									tdc_to_remove_2.push_back(p_entity_id);

									for (const auto& call : m_callbacks)
									{
										call(RenderingQueueSystemEvent::TRIANGLEDRAWING_REMOVED, p_entity_id);
									}
								}
							}

							for (const std::string& id : tdc_to_remove_2)
							{
								tsl.second.drawing_list.erase(id);
							}

							if (0 == tsl.second.drawing_list.size())
							{
								_RENDERME_DEBUG(m_localLogger, "textureSet payload is now empty, remove textureSet id : " + tsl.first)
								tsl_to_remove.push_back(tsl.first);
							}
						}

						for (const std::string& id : tsl_to_remove)
						{
							tm.second.textures_set_list.erase(id);
						}

						//////////////

						if (0 == tm.second.drawing_list.size() && 0 == tm.second.textures_set_list.size())
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
			rendering_channel.list.erase(id);
		}

		////////////////////////////////////////////////

		if (0 == rendering_channel.list.size())
		{
			_RENDERME_DEBUG(m_localLogger, "rendering order channel is now empty, remove : " + std::to_string(qnode.first))
			roc_to_remove.push_back(qnode.first);
		}
	}

	for (const int chan : roc_to_remove)
	{
		queueNodes.erase(chan);
	}


	/*
	
	std::vector<std::string> vs_to_remove;

	for (auto& vs : queueNodes)
	{

	}

	for (const std::string& id : vs_to_remove)
	{
		queueNodes.erase(id);
	}
	*/

	p_renderingQueue.setQueueNodes(queueNodes);
}
