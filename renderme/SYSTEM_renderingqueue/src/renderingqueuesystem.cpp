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

using namespace renderMe;
using namespace renderMe::core;


RenderingQueueSystem::RenderingQueueSystem(Entitygraph& p_entitygraph) : System(p_entitygraph),
m_localLogger("RenderingQueueSystem", renderMe::core::logger::Configuration::getInstance())
{
}

void RenderingQueueSystem::run()
{
	manageRenderingQueue();
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
	////////Queue build/updates//////////////////////////////////////
	{
		rendering::Queue* current_queue{ nullptr };

		for (auto it = m_entitygraph.preBegin(); it != m_entitygraph.preEnd(); ++it)
		{
			const auto current_entity{ it->data() };
			const auto currId{ current_entity->getId() };

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

			if (current_entity->hasAspect(renderMe::core::resourcesAspect::id) &&
				current_entity->hasAspect(renderMe::core::renderingAspect::id))
			{
				const auto& resource_aspect{ current_entity->aspectAccess(renderMe::core::resourcesAspect::id) };
				const auto& rendering_aspect{ current_entity->aspectAccess(renderMe::core::renderingAspect::id) };

				if (current_queue)
				{
					updateRenderingQueue(resource_aspect, rendering_aspect, *current_queue);
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

static rendering::Queue::LineMeshePayload build_LineMeshePayload(const renderMe::core::ComponentList<rendering::LineDrawingControl>& p_linesDrawingControls,
																const renderMe::Shader& p_vshader, const renderMe::Shader& p_pshader)
{
	rendering::Queue::LineMeshePayload lineMeshePayload;

	const auto vshaders_current_args{ p_vshader.getArguments() };
	const auto pshaders_current_args{ p_pshader.getArguments() };


	for (const auto& ldc : p_linesDrawingControls)
	{
		auto& linesDrawingControl{ ldc->getPurpose() };
		linesDrawingControl.ready = true;

		//vshader arguments id match loop
		for (const auto& e : vshaders_current_args)
		{
			const auto argument_id{ e.argument_id };
			for (const auto& e2 : linesDrawingControl.vshaders_map)
			{
				if (argument_id == e2.second)
				{
					linesDrawingControl.vshaders_map_cnx.push_back(std::make_pair(e2.first, e));
				}
			}
		}

		//pshader arguments id match loop
		for (const auto& e : pshaders_current_args)
		{
			const auto argument_id{ e.argument_id };
			for (const auto& e2 : linesDrawingControl.pshaders_map)
			{
				if (argument_id == e2.second)
				{
					linesDrawingControl.pshaders_map_cnx.push_back(std::make_pair(e2.first, e));
				}
			}
		}

		lineMeshePayload.list.push_back(linesDrawingControl);
	}

	return lineMeshePayload;
}

void RenderingQueueSystem::updateRenderingQueue(const renderMe::core::ComponentContainer& p_resourceAspect, 
												const renderMe::core::ComponentContainer& p_renderingAspect, 
												renderMe::rendering::Queue& p_renderingQueue)
{
	auto queueNodes{ p_renderingQueue.getQueueNodes() };

	//search for line drawing request
	const auto linesDrawingControls{ p_renderingAspect.getComponentsByType<rendering::LineDrawingControl>() };

	if (linesDrawingControls.size() > 0)
	{
		bool notAllReady{ false };

		for (const auto& e : linesDrawingControls)
		{
			const auto& linesDrawingControl{ e->getPurpose() };

			if (!linesDrawingControl.ready)
			{
				notAllReady = true;
			}
		}

		if (notAllReady)
		{	
			// search for lineMeshe

			const auto lineMeshes{ p_resourceAspect.getComponentsByType<LineMeshe>() };

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
						if (rsStates.size() > 0 && lineMeshes.size() > 0)
						{
							// ok, can update queue

							if (queueNodes.count(vshader.getName()))
							{
								// vshader entry exists

								const auto& pshaders{ queueNodes.at(vshader.getName()) };

								if (queueNodes.count(pshader.getName()))
								{
									// pshader entry exists

								}
								else
								{
									// insert new branch
								}


								// TODO
							}
							else
							{
								// insert new branch
								const auto lineMeshePayload { build_LineMeshePayload(linesDrawingControls, vshader, pshader) };

								rendering::Queue::RenderStatePayload renderStatePayload;
								renderStatePayload.list[lineMeshes.at(0)->getPurpose().getName()] = lineMeshePayload;
								renderStatePayload.description = rsStates.at(0)->getPurpose();

								rendering::Queue::PixelShaderPayload pixelShaderPayload;
								std::string rs_set_signature;
								const auto rs_set{ rsStates.at(0)->getPurpose() };
								for (const auto& e : rs_set)
								{
									rs_set_signature += e.toString() + "; ";
								}
								pixelShaderPayload.list[rs_set_signature] = renderStatePayload;

								rendering::Queue::VertexShaderPayload vertexShaderPayload;
								vertexShaderPayload.list[pshader.getName()] = pixelShaderPayload;
								
								queueNodes[vshader.getName()] = vertexShaderPayload;
							}							
						}
					}
				}
			}
		}
	}

	


	p_renderingQueue.setQueueNodes(queueNodes);
}