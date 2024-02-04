
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

#include "logger_service.h"

#include "linemeshe.h"
#include "datacloud.h"

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

	////// Register callback to runner
	
	const Runner::Callback runner_cb
	{
		[&, this](renderMe::core::RunnerEvent p_event, const std::string& p_target_descr, const std::string& p_action_descr)
		{		
			if (renderMe::core::RunnerEvent::TASK_ERROR == p_event)
			{
				// rethrow in current thread
				_EXCEPTION(std::string("failed action ") + p_action_descr + " on target " + p_target_descr);
			}
			else if (renderMe::core::RunnerEvent::TASK_DONE == p_event)
			{
				auto& eventsLogger{ services::LoggerSharing::getInstance()->getLogger("Events") };

				_RENDERME_DEBUG(d3dimpl->logger(), std::string("TASK_DONE ") + p_target_descr + " " + p_action_descr);

				if ("load_shader_d3d11" == p_action_descr)
				{
					_RENDERME_DEBUG(eventsLogger, "EMIT EVENT -> D3D11_SHADER_CREATION_SUCCESS : " + p_target_descr);
					for (const auto& call : m_callbacks)
					{
						call(D3D11SystemEvent::D3D11_SHADER_CREATION_SUCCESS, p_target_descr);
					}
				}
				else if ("release_shader_d3d11" == p_action_descr)
				{
					_RENDERME_DEBUG(eventsLogger, "EMIT EVENT -> D3D11_SHADER_RELEASE_SUCCESS : " + p_target_descr);
					for (const auto& call : m_callbacks)
					{
						call(D3D11SystemEvent::D3D11_SHADER_RELEASE_SUCCESS, p_target_descr);
					}
				}
				else if ("load_linemeshe_d3d11" == p_action_descr)
				{
					_RENDERME_DEBUG(eventsLogger, "EMIT EVENT -> D3D11_LINEMESHE_CREATION_SUCCESS : " + p_target_descr);
					for (const auto& call : m_callbacks)
					{
						call(D3D11SystemEvent::D3D11_LINEMESHE_CREATION_SUCCESS, p_target_descr);
					}
				}
				else if ("release_linemeshe_d3d11" == p_action_descr)
				{
					_RENDERME_DEBUG(eventsLogger, "EMIT EVENT -> D3D11_LINEMESHE_RELEASE_SUCCESS : " + p_target_descr);
					for (const auto& call : m_callbacks)
					{
						call(D3D11SystemEvent::D3D11_LINEMESHE_RELEASE_SUCCESS, p_target_descr);
					}
				}
			}
		}
	};
	
	m_runner.registerSubscriber(runner_cb);
	m_runner.startup();

	////// Register callback to entitygraph

	const Entitygraph::Callback eg_cb
	{
		[&, this](renderMe::core::EntitygraphEvents p_event, const core::Entity& p_entity)
		{
			auto& eventsLogger{ services::LoggerSharing::getInstance()->getLogger("Events") };

			if (renderMe::core::EntitygraphEvents::ENTITYGRAPHNODE_REMOVED == p_event)
			{
				_RENDERME_DEBUG(eventsLogger, "RECV EVENT -> ENTITYGRAPHNODE_REMOVED : " + p_entity.getId());

				if (p_entity.hasAspect(core::resourcesAspect::id))
				{
					auto& eventsLogger{ services::LoggerSharing::getInstance()->getLogger("Events") };
				
					const auto& resources{ p_entity.aspectAccess(core::resourcesAspect::id) };


					const auto s_list{ resources.getComponentsByType<Shader>() };
					for (auto& e : s_list)
					{
						auto& shader{ e->getPurpose() };

						const auto state{ shader.getState() };
						if (Shader::State::RENDERERLOADED == state)
						{
							_RENDERME_DEBUG(eventsLogger, "EMIT EVENT -> D3D11_SHADER_RELEASE_BEGIN : " + shader.getName());
							for (const auto& call : m_callbacks)
							{
								call(D3D11SystemEvent::D3D11_SHADER_RELEASE_BEGIN, shader.getName());
							}
							handleShaderRelease(shader, shader.getType());
						}
					}

					//search for linemeshe
					const auto lm_list{ resources.getComponent<std::vector<LineMeshe>>("lineMeshes") };
					if (lm_list)
					{
						for (auto& lm : lm_list->getPurpose())
						{
							const auto state{ lm.getState() };
							if (LineMeshe::State::RENDERERLOADED == state)
							{
								_RENDERME_DEBUG(eventsLogger, "EMIT EVENT -> D3D11_LINEMESHE_RELEASE_BEGIN : " + lm.getName());
								for (const auto& call : m_callbacks)
								{
									call(D3D11SystemEvent::D3D11_LINEMESHE_RELEASE_BEGIN, lm.getName());
								}

								handleLinemesheRelease(lm);
							}
						}
					}
				}				
			}
		}
	};

	p_entitygraph.registerSubscriber(eg_cb);
}

void D3D11System::manageInitialization()
{
	const auto forEachRenderingAspect
	{
		[&](Entity* p_entity, const ComponentContainer& p_rendering_aspect)
		{
			//////////////////////////////////////////////////////////////////////////////////////////////
			// manage D3D11 init

			const auto rendering_target_comp{ p_rendering_aspect.getComponent<core::renderingAspect::renderingTarget>("renderingTarget") };
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
			auto& eventsLogger{ services::LoggerSharing::getInstance()->getLogger("Events") };

			{			
				const auto s_list{ p_resource_aspect.getComponentsByType<Shader>() };
				for (auto& e : s_list)
				{
					auto& shader{ e->getPurpose() };
					const auto state{ shader.getState() };
					if (Shader::State::BLOBLOADED == state)
					{

						_RENDERME_DEBUG(eventsLogger, "EMIT EVENT -> D3D11_SHADER_CREATION_BEGIN : " + shader.getName());
						for (const auto& call : m_callbacks)
						{
							call(D3D11SystemEvent::D3D11_SHADER_CREATION_BEGIN, shader.getName());
						}

						handleShaderCreation(shader, shader.getType());
						shader.setState(Shader::State::RENDERERLOADING);
					}
				}
			}
			
			//search for line Meshes
			const auto lmeshes_list{ p_resource_aspect.getComponentsByType<LineMeshe>()};
			for (auto& e : lmeshes_list)
			{
				auto& lm{ e->getPurpose() };
				const auto state{ lm.getState()};

				if (LineMeshe::State::BLOBLOADED == state)
				{				
					_RENDERME_DEBUG(eventsLogger, "EMIT EVENT -> D3D11_LINEMESHE_CREATION_BEGIN : " + lm.getName());
					for (const auto& call : m_callbacks)
					{
						call(D3D11SystemEvent::D3D11_LINEMESHE_CREATION_BEGIN, lm.getName());
					}

					handleLinemesheCreation(lm);
					lm.setState(LineMeshe::State::RENDERERLOADING);
				}			
			}
		}
	};
	renderMe::helpers::extractAspectsTopDown<renderMe::core::resourcesAspect>(m_entitygraph, forEachResourcesAspect);
}


void D3D11System::renderQueue(rendering::Queue& p_renderingQueue)
{
	const auto dataCloud{ renderMe::rendering::Datacloud::getInstance() };

	if (rendering::Queue::Purpose::SCREEN_RENDERING == p_renderingQueue.getPurpose())
	{
		d3dimpl->beginScreen();

	}
	else //INTERMEDIATE_RENDERING
	{
		// TODO : beginIntermediate()
	}

	d3dimpl->clearTarget(p_renderingQueue.getTargetClearColor());

	{
		auto queueNodes{ p_renderingQueue.getQueueNodes() };

		for (const auto& vertexShaderInfo : queueNodes)
		{
			const auto& vertexShaderId{ vertexShaderInfo.first };
			const auto& vertexShaderPayload{ vertexShaderInfo.second };

			//set vertex shader
			d3dimpl->setVertexShader(vertexShaderId);
			for (const auto& pixelShaderInfo : vertexShaderPayload.list)
			{
				const auto& pixelShaderId{ pixelShaderInfo.first };
				const auto& pixelShaderPayload{ pixelShaderInfo.second };

				//set pixel shader
				d3dimpl->setPixelShader(pixelShaderId);
				for (const auto& renderStatesInfo : pixelShaderPayload.list)
				{
					const auto renderStates { renderStatesInfo.second.description };
					for (const auto& renderState : renderStates)
					{
						d3dimpl->setDepthStenciState(renderState);
						d3dimpl->setPSSamplers(renderState);
						d3dimpl->setVSSamplers(renderState);

						// prepare updates
						d3dimpl->prepareRenderState(renderState);
						d3dimpl->prepareBlendState(renderState);
					}

					// apply updates
					d3dimpl->setCacheRS();
					d3dimpl->setCacheBlendstate();

					if (renderStatesInfo.second.list.size() > 0)
					{
						d3dimpl->setLineListTopology();
					}
					
					for (const auto& lineMesheInfo : renderStatesInfo.second.list)
					{
						const auto& lineMesheId{ lineMesheInfo.first };
						d3dimpl->setLineMeshe(lineMesheId);

						std::vector<renderMe::rendering::LineDrawingControl> lineDrawingControls{ lineMesheInfo.second.list };
						for (const auto& lc : lineDrawingControls)
						{
							//////
							lc.setup();

							////// Apply shaders params

							for (const auto& e : lc.vshaders_map_cnx)
							{
								const auto& datacloud_data_id{ e.first };
								const auto& shader_param{ e.second };

								if ("Real4Vector" == shader_param.argument_type)
								{
									const maths::Real4Vector rvector{ { dataCloud->readDataValue<maths::Real4Vector>(datacloud_data_id) } };
									d3dimpl->setVertexshaderConstantsVec(0, rvector);
								}
							}

							for (const auto& e : lc.pshaders_map_cnx)
							{
								const auto& datacloud_data_id{ e.first };
								const auto& shader_param{ e.second };

								if ("Real4Vector" == shader_param.argument_type)
								{
									const maths::Real4Vector rvector{ { dataCloud->readDataValue<maths::Real4Vector>(datacloud_data_id) } };
									d3dimpl->setPixelshaderConstantsVec(0, rvector);
								}
							}

							//////

							d3dimpl->drawLineMeshe(lc.world, lc.view, lc.proj);

							//////
							lc.teardown();
						}
					}
				}
			}
		}
	}
	
	// render texts
	for (auto& text : p_renderingQueue.texts())
	{
		d3dimpl->drawText(text.second.font, text.second.color, text.second.position, text.second.rotation_rad, text.second.text);
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

void D3D11System::handleShaderCreation(Shader& p_shaderInfos, int p_shaderType)
{
	const auto shaderType{ p_shaderType };

	_RENDERME_DEBUG(d3dimpl->logger(), std::string("Handle shader creation ") + p_shaderInfos.getName() + std::string(" shader type ") + std::to_string(shaderType));

	const std::string shaderAction{ "load_shader_d3d11" };

	const auto task{ new renderMe::core::SimpleAsyncTask<>(shaderAction, p_shaderInfos.getName(),
		[&,
			shaderType = shaderType,
			shaderAction = shaderAction
		]()
		{
			bool status { false };
			if (0 == shaderType)
			{
				status = d3dimpl->createVertexShader(p_shaderInfos.getName(), p_shaderInfos.getCode());
			}
			else if (1 == shaderType)
			{
				status = d3dimpl->createPixelShader(p_shaderInfos.getName(), p_shaderInfos.getCode());
			}

			if (!status)
			{
				_RENDERME_ERROR(d3dimpl->logger(), "Failed to load shader " + p_shaderInfos.getName() + " in D3D11 ");
				
				// send error status to main thread and let terminate
				const Runner::TaskReport report{ RunnerEvent::TASK_ERROR, p_shaderInfos.getName(), shaderAction };
				m_runner.m_mailbox_out.push(report);
			}
			else
			{
				_RENDERME_DEBUG(d3dimpl->logger(), "Successful creation of shader " + p_shaderInfos.getName() + " in D3D11 ");
				p_shaderInfos.setState(Shader::State::RENDERERLOADED);
			}			
		}
	)};

	m_runner.m_mailbox_in.push(task);
}

void D3D11System::handleShaderRelease(Shader& p_shaderInfos, int p_shaderType)
{
	const auto shaderType{ p_shaderType };

	_RENDERME_DEBUG(d3dimpl->logger(), std::string("Handle shader release ") + p_shaderInfos.getName() + std::string(" shader type ") + std::to_string(shaderType));

	const std::string shaderAction{ "release_shader_d3d11" };

	const auto task{ new renderMe::core::SimpleAsyncTask<>(shaderAction, p_shaderInfos.getName(),
		[&,
			shaderType = shaderType,
			shaderAction = shaderAction
		]()
		{
			try
			{
				if (0 == shaderType)
				{
					d3dimpl->destroyVertexShader(p_shaderInfos.getName());
				}
				else if (1 == shaderType)
				{
					d3dimpl->destroyPixelShader(p_shaderInfos.getName());
				}

				_RENDERME_DEBUG(d3dimpl->logger(), "Successful release of shader " + p_shaderInfos.getName() + " in D3D11 ");

			}
			catch (const std::exception& e)
			{
				_RENDERME_ERROR(d3dimpl->logger(), std::string("failed to release ") + p_shaderInfos.getName() + " : reason = " + e.what());

				// send error status to main thread and let terminate
				const Runner::TaskReport report{ RunnerEvent::TASK_ERROR, p_shaderInfos.getName(), shaderAction };
				m_runner.m_mailbox_out.push(report);
			}
		}
	) };

	m_runner.m_mailbox_in.push(task);
}

void D3D11System::handleLinemesheCreation(LineMeshe& p_lm)
{
	_RENDERME_DEBUG(d3dimpl->logger(), std::string("Handle line meshe creation ") + p_lm.getName());

	const std::string action{ "load_linemeshe_d3d11" };

	const auto task{ new renderMe::core::SimpleAsyncTask<>(action, p_lm.getName(),
		[&,
			action = action
		]()
		{
			bool status { false };
			status = d3dimpl->createLineMeshe(p_lm);

			if (!status)
			{
				_RENDERME_ERROR(d3dimpl->logger(), "Failed to load linemeshe " + p_lm.getName() + " in D3D11 ");

				// send error status to main thread and let terminate
				const Runner::TaskReport report{ RunnerEvent::TASK_ERROR, p_lm.getName(), action };
				m_runner.m_mailbox_out.push(report);
			}
			else
			{
				_RENDERME_DEBUG(d3dimpl->logger(), "Successful creation of linemeshe " + p_lm.getName() + " in D3D11 ");
				p_lm.setState(LineMeshe::State::RENDERERLOADED);
			}
		}
	) };

	m_runner.m_mailbox_in.push(task);
}

void D3D11System::handleLinemesheRelease(LineMeshe& p_lm)
{
	_RENDERME_DEBUG(d3dimpl->logger(), std::string("Handle line meshe release ") + p_lm.getName());

	const std::string action{ "release_linemeshe_d3d11" };

	const auto task{ new renderMe::core::SimpleAsyncTask<>(action, p_lm.getName(),
		[&,
			action = action
		]()
		{
			try
			{
				d3dimpl->destroyLineMeshe(p_lm.getName());
				_RENDERME_DEBUG(d3dimpl->logger(), "Successful release of linemeshe " + p_lm.getName() + " in D3D11 ");

			}
			catch (const std::exception& e)
			{
				_RENDERME_ERROR(d3dimpl->logger(), std::string("failed to release ") + p_lm.getName() + " : reason = " + e.what());

				// send error status to main thread and let terminate
				const Runner::TaskReport report{ RunnerEvent::TASK_ERROR, p_lm.getName(), action };
				m_runner.m_mailbox_out.push(report);
			}
		}
	) };

	m_runner.m_mailbox_in.push(task);
}