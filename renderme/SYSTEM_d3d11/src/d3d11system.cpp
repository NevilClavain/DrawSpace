
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


using namespace renderMe;
using namespace renderMe::core;


D3D11System::D3D11System(Entitygraph& p_entitygraph) : System(p_entitygraph)
{
}

void D3D11System::checkD3D11SystemImplInit(core::Entity* p_entity)
{
	if (!m_initialized)
	{
		if (D3D11SystemImpl::getInstance()->init(p_entity))
		{
			m_initialized = true;
		}
		else
		{
			_EXCEPTION("D3D11 initialization failed")
		}
	}
}

void D3D11System::manageRenderingQueues(Entity* p_entity, rendering::Queue& p_renderingQueue)
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
					_RENDERME_WARN(D3D11SystemImpl::getInstance()->logger(), "Rendering queue set to ERROR_ORPHAN : no parent")
				}
				else
				{
					if (p_entity->hasAspect(core::renderingAspect::id))
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

								_RENDERME_DEBUG(D3D11SystemImpl::getInstance()->logger(), "rendering queue " + p_renderingQueue.getName() + " set to READY, SCREEN_RENDERING")
							}
							else
							{
								// BUFFER_TARGET
								// 
								// parent is a texture-target pass
								// set queue purpose accordingly

								p_renderingQueue.setPurpose(rendering::Queue::Purpose::INTERMEDIATE_RENDERING);
								p_renderingQueue.setState(rendering::Queue::State::READY);

								_RENDERME_DEBUG(D3D11SystemImpl::getInstance()->logger(), "rendering queue " + p_renderingQueue.getName() + " set to READY, INTERMEDIATE_RENDERING")
							}
						}
						else
						{
							// parent rendering aspect has no renderingTarget component !
							p_renderingQueue.setState(rendering::Queue::State::ERROR_ORPHAN);
							// log it (WARN)
							_RENDERME_WARN(D3D11SystemImpl::getInstance()->logger(), "Rendering queue set to ERROR_ORPHAN : parent rendering aspect has no renderingTarget component")
						}
					}
					else
					{
						// parent has no rendering aspect 
						p_renderingQueue.setState(rendering::Queue::State::ERROR_ORPHAN);
						// log it (WARN)
						_RENDERME_WARN(D3D11SystemImpl::getInstance()->logger(), "Rendering queue set to ERROR_ORPHAN : parent has no rendering aspect")
					}
				}
			}
		}
		break;

		case rendering::Queue::State::READY:

			// do queue rendering

			if (rendering::Queue::Purpose::SCREEN_RENDERING == p_renderingQueue.getPurpose())
			{
				D3D11SystemImpl::getInstance()->beginScreen();
				D3D11SystemImpl::getInstance()->clearScreen(p_renderingQueue.getTargetClearColor());
			}
			else //INTERMEDIATE_RENDERING
			{				
				// TODO
			}

			break;

		case rendering::Queue::State::ERROR_ORPHAN:

			// nothin' to do
			break;
	}
}

void D3D11System::run()
{
	for (auto it = m_entitygraph.preBegin(); it != m_entitygraph.preEnd(); ++it)
	{
		const auto current_entity{ it->data() };
		const auto currId{ current_entity->getId() };

		if (current_entity->hasAspect(core::renderingAspect::id))
		{
			const auto& rendering_aspect{ current_entity->aspectAccess(core::renderingAspect::id) };

			//////////////////////////////////////////////////////////////////////////////////////////////
			// manage D3D11 init

			auto rendering_target_comp{ rendering_aspect.getComponent<core::renderingAspect::renderingTarget>("renderingTarget") };
			const bool isWindowsRenderingTarget{ rendering_target_comp != nullptr && 
													core::renderingAspect::renderingTarget::WINDOW_TARGET == rendering_target_comp->getPurpose() };

			if (isWindowsRenderingTarget)
			{
				checkD3D11SystemImplInit(current_entity);
			}

			//////////////////////////////////////////////////////////////////////////////////////////////
			// manage rendering queues

			auto rendering_queue_comp{ rendering_aspect.getComponent<rendering::Queue>("renderingQueue") };
			if (rendering_queue_comp)
			{
				auto& renderingQueue{ rendering_queue_comp->getPurpose() };
				manageRenderingQueues(current_entity, renderingQueue);							
			}
		}
	}

	if (m_initialized)
	{
		D3D11SystemImpl::getInstance()->drawText({ 255, 0, 255, 255 }, { 10, 100 }, 10.0, "small text");
		D3D11SystemImpl::getInstance()->drawText({ 255, 0, 255, 255 }, { 10, 150 }, 20.0, "medium text");
		D3D11SystemImpl::getInstance()->drawText({ 255, 0, 255, 255 }, { 10, 200 }, 40.0, "large text");

		D3D11SystemImpl::getInstance()->flipScreen();
	}
}





