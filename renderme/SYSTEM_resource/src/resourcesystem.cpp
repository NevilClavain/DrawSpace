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

#include "resourcesystem.h"
#include "entity.h"
#include "entitygraph.h"
#include "aspects.h"
#include "ecshelpers.h"

using namespace renderMe;
using namespace renderMe::core;

ResourceSystem::ResourceSystem(Entitygraph& p_entitygraph) : System(p_entitygraph),
m_localLogger("ResourceSystem", renderMe::core::logger::Configuration::getInstance()),
m_localLoggerRunner("ResourceSystemRunner", renderMe::core::logger::Configuration::getInstance())
{
	const Runner::Callback cb
	{
		[&, this](renderMe::core::RunnerEvent p_event, const std::string& p_target_descr, const std::string& p_action_descr)
		{
			if (renderMe::core::RunnerEvent::TASK_DONE == p_event)
			{
				_RENDERME_DEBUG(m_localLoggerRunner, std::string("TASK_DONE ") + p_target_descr + " " + p_action_descr);
			}
			else if (renderMe::core::RunnerEvent::TASK_UPDATE == p_event)
			{
				_RENDERME_DEBUG(m_localLoggerRunner, std::string("TASK_UPDATE ") + p_target_descr + " " + p_action_descr);
			}
		}
	};

	m_runner.registerSubscriber(cb);
	m_runner.startup();
}

ResourceSystem::~ResourceSystem()
{
	_RENDERME_DEBUG(m_localLogger, std::string("Exiting..."));
}

void ResourceSystem::run()
{
	const auto forEachResourceAspect
	{
		[&](Entity* p_entity, const ComponentContainer& p_resource_aspect)
		{
			// search for vertex shaders

			const auto vshaders_list { p_resource_aspect.getComponent<std::vector<ResourceSystem::ShaderInfos>>("vertexShaders") };
			if (vshaders_list)
			{
				for (auto& shaderDescr : vshaders_list->getPurpose())
				{
					if (!shaderDescr.readyToUse)
					{					
						handleShader(shaderDescr);
						shaderDescr.readyToUse = true;
					}
				}
			}
		}
	};

	renderMe::helpers::extractAspectsTopDown<renderMe::core::resourcesAspect>(m_entitygraph, forEachResourceAspect);
	m_runner.dispatchEvents();
}

void ResourceSystem::handleShader(ShaderInfos& shaderInfos)
{
	_RENDERME_DEBUG(m_localLogger, std::string("Handle vertex shader ") + shaderInfos.name);
	
	static renderMe::core::SimpleAsyncTask<> loadShaderSource(shaderInfos.name, "load_shader_source",
		[&](void)
		{
			_RENDERME_DEBUG(m_localLoggerRunner, std::string("loading ") + shaderInfos.name);

			Sleep(100);

			_RENDERME_DEBUG(m_localLoggerRunner, std::string("part 1 done ") + shaderInfos.name);

			const Runner::TaskReport report{ RunnerEvent::TASK_UPDATE, shaderInfos.name, "load_shader_source" };
			m_runner.m_mailbox_out.push<Runner::TaskReport>(report);

			Sleep(150);

			_RENDERME_DEBUG(m_localLoggerRunner, std::string("part 2 done ") + shaderInfos.name);

			const Runner::TaskReport report2{ RunnerEvent::TASK_UPDATE, shaderInfos.name, "load_shader_source" };
			m_runner.m_mailbox_out.push<Runner::TaskReport>(report2);
		}
	);

	m_runner.m_mailbox_in.push<renderMe::property::AsyncTask*>(&loadShaderSource);	
}

void ResourceSystem::killRunner()
{
	renderMe::core::RunnerKiller runnerKiller;
	m_runner.m_mailbox_in.push<renderMe::property::AsyncTask*>(&runnerKiller);
	m_runner.join();
}