
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

#pragma warning( disable : 4005 4838 )

#include "d3d11system.h"

#include "logsink.h"
#include "logconf.h"
#include "logging.h"

#include "aspects.h"
#include "entity.h"
#include "entitygraph.h"

#include "exceptions.h"
#include "d3d11systemimpl.h"
#include "renderingqueue.h"

#include "ecshelpers.h"
#include "shader.h"


using namespace renderMe;
using namespace renderMe::core;

static const auto d3dimpl{ D3D11SystemImpl::getInstance() };

D3D11System::D3D11System(Entitygraph& p_entitygraph) : System(p_entitygraph)
{
	m_service_invocation_cb = [&, this](const std::string& p_includePath,
		const renderMe::core::FileContent<const char>& p_src,		
		int p_shaderType,
		std::unique_ptr<char[]>& p_shaderBytes,
		size_t& p_shaderBytesLength,
		bool& p_status)
	{
		p_status = d3dimpl->createShaderBytesOnFile(p_shaderType, p_includePath, p_src, p_shaderBytes, p_shaderBytesLength);
	};

	
	const Runner::Callback runner_cb
	{
		[&, this](renderMe::core::RunnerEvent p_event, const std::string& p_target_descr, const std::string& p_action_descr)
		{
			if (renderMe::core::RunnerEvent::TASK_ERROR == p_event)
			{
				if ("load_shader_d3d11" == p_action_descr)
				{
					// rethrow in current thread
					_EXCEPTION(std::string("failed action ") + p_action_descr + " on target " + p_target_descr);
				}
			}
			else if (renderMe::core::RunnerEvent::TASK_DONE == p_event)
			{
				_RENDERME_DEBUG(d3dimpl->logger(), std::string("TASK_DONE ") + p_target_descr + " " + p_action_descr);
			}
		}
	};
	
	m_runner.registerSubscriber(runner_cb);
	m_runner.startup();
}

void D3D11System::manageInitialization()
{
	const auto forEachRenderingAspect
	{
		[&](Entity* p_entity, const ComponentContainer& p_rendering_aspect)
		{
			//////////////////////////////////////////////////////////////////////////////////////////////
			// manage D3D11 init

			auto rendering_target_comp{ p_rendering_aspect.getComponent<core::renderingAspect::renderingTarget>("renderingTarget") };
			const bool isWindowsRenderingTarget{ rendering_target_comp != nullptr &&
													core::renderingAspect::renderingTarget::WINDOW_TARGET == rendering_target_comp->getPurpose() };

			if (isWindowsRenderingTarget)
			{
				if (d3dimpl->init(p_entity))
				{
					m_initialized = true;
				}
				else
				{
					_EXCEPTION("D3D11 initialization failed")
				}
			}
		}	
	};

	renderMe::helpers::extractAspectsTopDown<renderMe::core::renderingAspect>(m_entitygraph, forEachRenderingAspect);
}

void D3D11System::handleRenderingQueuesState(Entity* p_entity, rendering::Queue& p_renderingQueue)
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
					_RENDERME_WARN(d3dimpl->logger(), "Rendering queue set to ERROR_ORPHAN : no parent")
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

								_RENDERME_DEBUG(d3dimpl->logger(), "rendering queue " + p_renderingQueue.getName() + " set to READY, SCREEN_RENDERING")
							}
							else
							{
								// BUFFER_TARGET
								// 
								// parent is a texture-target pass
								// set queue purpose accordingly

								p_renderingQueue.setPurpose(rendering::Queue::Purpose::INTERMEDIATE_RENDERING);
								p_renderingQueue.setState(rendering::Queue::State::READY);

								_RENDERME_DEBUG(d3dimpl->logger(), "rendering queue " + p_renderingQueue.getName() + " set to READY, INTERMEDIATE_RENDERING")
							}
						}
						else
						{
							// parent rendering aspect has no renderingTarget component !
							p_renderingQueue.setState(rendering::Queue::State::ERROR_ORPHAN);
							// log it (WARN)
							_RENDERME_WARN(d3dimpl->logger(), "Rendering queue set to ERROR_ORPHAN : parent rendering aspect has no renderingTarget component")
						}
					}
					else
					{
						// parent has no rendering aspect 
						p_renderingQueue.setState(rendering::Queue::State::ERROR_ORPHAN);
						// log it (WARN)
						_RENDERME_WARN(d3dimpl->logger(), "Rendering queue set to ERROR_ORPHAN : parent has no rendering aspect")
					}
				}
			}
		}
		break;

		case rendering::Queue::State::READY:

			// do queue rendering
			renderQueue(p_renderingQueue);
			break;

		case rendering::Queue::State::ERROR_ORPHAN:

			// nothin' to do
			break;
	}
}

void D3D11System::manageRenderingQueue() const
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

void D3D11System::manageResources()
{
	const auto forEachResourcesAspect
	{
		[&](Entity* p_entity, const ComponentContainer& p_resource_aspect)
		{
			// search for vertex shaders			
			const auto vshaders_list { p_resource_aspect.getComponent<std::vector<Shader>>("vertexShaders") };
			if (vshaders_list)
			{
				for (auto& shaderDescr : vshaders_list->getPurpose())
				{
					const auto state{ shaderDescr.getState() };
					if (Shader::State::BLOBLOADED == state)
					{
						handleShader(shaderDescr, 0);
						shaderDescr.setState(Shader::State::RENDERERLOADING);
					}
				}
			}

			// search for pixel shaders
			const auto pshaders_list{ p_resource_aspect.getComponent<std::vector<Shader>>("pixelShaders") };
			if (pshaders_list)
			{
				for (auto& shaderDescr : pshaders_list->getPurpose())
				{
					const auto state{ shaderDescr.getState() };
					if (Shader::State::BLOBLOADED == state)
					{
						handleShader(shaderDescr, 1);
						shaderDescr.setState(Shader::State::RENDERERLOADING);
					}
				}
			}			
		}
	};
	renderMe::helpers::extractAspectsTopDown<renderMe::core::resourcesAspect>(m_entitygraph, forEachResourcesAspect);
}


void D3D11System::renderQueue(rendering::Queue& p_renderingQueue)
{
	if (rendering::Queue::Purpose::SCREEN_RENDERING == p_renderingQueue.getPurpose())
	{
		d3dimpl->beginScreen();

	}
	else //INTERMEDIATE_RENDERING
	{
		// TODO : beginIntermediate()
	}

	d3dimpl->clearTarget(p_renderingQueue.getTargetClearColor());

	// TODO : run queue

	// render texts
	for (auto& text : p_renderingQueue.texts())
	{
		d3dimpl->drawText(text.second.font, text.second.color, text.second.position, text.second.font_size, text.second.text);
	}
}

void D3D11System::run()
{
	if (!m_initialized)
	{
		manageInitialization();
	}

	manageResources();
	manageRenderingQueue();

	if (m_initialized)
	{
		d3dimpl->flipScreen();
	}

	m_runner.dispatchEvents();
}

void D3D11System::killRunner()
{
	renderMe::core::RunnerKiller runnerKiller;
	m_runner.m_mailbox_in.push(&runnerKiller);
	m_runner.join();
}

void D3D11System::handleShader(Shader& shaderInfos, int p_shaderType)
{
	const auto shaderType{ p_shaderType };

	_RENDERME_DEBUG(d3dimpl->logger(), std::string("Handle shader ") + shaderInfos.getName() + std::string(" shader type ") + std::to_string(shaderType));

	const std::string shaderAction{ "load_shader_d3d11" };

	const auto task{ new renderMe::core::SimpleAsyncTask<>(shaderInfos.getName(), shaderAction,
		[&,
			shaderType = shaderType,
			shaderAction = shaderAction
		]()
		{
			bool status { false };
			if (0 == shaderType)
			{
				status = d3dimpl->createVertexShader(shaderInfos.getName(), shaderInfos.getCode());
			}
			else if (1 == shaderType)
			{
				status = d3dimpl->createPixelShader(shaderInfos.getName(), shaderInfos.getCode());
			}

			if (!status)
			{
				_RENDERME_ERROR(d3dimpl->logger(), "Failed to load shader " + shaderInfos.getName() + " in D3D11 ");
				
				// send error status to main thread and let terminate
				const Runner::TaskReport report{ RunnerEvent::TASK_ERROR, shaderInfos.getName(), shaderAction };
				m_runner.m_mailbox_out.push(report);
			}
			else
			{
				_RENDERME_DEBUG(d3dimpl->logger(), "Successful creation of shader " + shaderInfos.getName() + " in D3D11 ");
				shaderInfos.setState(Shader::State::RENDERERLOADED);
			}			
		}
	)};

	m_runner.m_mailbox_in.push(task);
}

