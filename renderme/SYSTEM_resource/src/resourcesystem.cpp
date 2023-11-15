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
#include "filesystem.h"
#include "md5.h"

using namespace renderMe;
using namespace renderMe::core;

ResourceSystem::ResourceSystem(Entitygraph& p_entitygraph) : System(p_entitygraph),
m_localLogger("ResourceSystem", renderMe::core::logger::Configuration::getInstance()),
m_localLoggerRunner("ResourceSystemRunner", renderMe::core::logger::Configuration::getInstance())
{
	///////// check & create shader cache if needed

	if (!fileSystem::exists(m_shadersCachePath))
	{
		_RENDERME_DEBUG(m_localLogger, std::string("Shader cache missing, creating it..."));
		fileSystem::createDirectory(m_shadersCachePath);
	}

	/////////////////////////////////////////////

	const Runner::Callback cb
	{
		[&, this](renderMe::core::RunnerEvent p_event, const std::string& p_target_descr, const std::string& p_action_descr)
		{
			if (renderMe::core::RunnerEvent::TASK_ERROR == p_event)
			{
				if ("load_shader" == p_action_descr)
				{
					// rethrow in current thread
					_EXCEPTION(std::string("failed action ") + p_action_descr + "on target " + p_target_descr );
				}
			}
			else if (renderMe::core::RunnerEvent::TASK_DONE == p_event)
			{
				_RENDERME_DEBUG(m_localLoggerRunner, std::string("TASK_DONE ") + p_target_descr + " " + p_action_descr);
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

	const std::string shader_action{ "load_shader" };
	
	static renderMe::core::SimpleAsyncTask<> loadShader(shaderInfos.name, shader_action,
		[&, shader_action = shader_action]()
		{
			_RENDERME_DEBUG(m_localLoggerRunner, std::string("loading ") + shaderInfos.name);

			// build full path
			const auto shader_path{ m_shadersBasePath + "/" + shaderInfos.name };
			try
			{				
				renderMe::core::FileContent<char> shader_src_content(shader_path);
				shader_src_content.load();

				// no mutex needed here (only this thread access it)
				shaderInfos.contentSize = shader_src_content.getDataSize();
				shaderInfos.content = shader_src_content.getData();

				MD5 md5;
				const std::string shaderMD5{ md5.digestMemory((BYTE*)shader_src_content.getData(), shader_src_content.getDataSize()) };
				shaderInfos.contentMD5 = shaderMD5;

				const auto decomposed_shader_filename{ fileSystem::splitFilename(shaderInfos.name) };
				const auto cacheDirectory{ m_shadersCachePath + "/" + decomposed_shader_filename.first };

				bool generate_cache_entry{ false };
				
				//check if shader exists in cache...
				if (!fileSystem::exists(cacheDirectory))
				{
					_RENDERME_TRACE(m_localLoggerRunner, std::string("cache directory missing : ") + cacheDirectory);

					// create all
					fileSystem::createDirectory(cacheDirectory);
					generate_cache_entry = true;
				}
				else
				{
					_RENDERME_TRACE(m_localLoggerRunner, std::string("cache directory exists : ") + cacheDirectory);

					// check if cache md5 file exists AND compiled shader exists

					if (!fileSystem::exists(cacheDirectory + "/bc.md5")/* || !fileSystem::exists(cacheDirectory + "/bc.code") */)
					{
						_RENDERME_TRACE(m_localLoggerRunner, std::string("cache file missing !"));
						generate_cache_entry = true;
					}
					else
					{
						_RENDERME_TRACE(m_localLoggerRunner, std::string("cache md5 file exists : ") + cacheDirectory + "/bc.md5");

						// load cache md5 file
						renderMe::core::FileContent<char> cache_md5_content(cacheDirectory + "/bc.md5");
						cache_md5_content.load();

						// check if md5 are equals

						if (std::string(cache_md5_content.getData(), cache_md5_content.getDataSize()) != shaderInfos.contentMD5)
						{
							_RENDERME_TRACE(m_localLoggerRunner, std::string("MD5 not matching ! : ") + shaderInfos.name);
							generate_cache_entry = true;
						}
						else
						{
							// load bc.code file
							_RENDERME_TRACE(m_localLoggerRunner, std::string("MD5 matches ! : ") + shaderInfos.name);
						}

					}
				}

				if (generate_cache_entry)
				{
					_RENDERME_TRACE(m_localLoggerRunner, std::string("generating cache entry : ") + shaderInfos.name);

					// create cache md5 file
					renderMe::core::FileContent<const char> shader_md5_content(cacheDirectory + "/bc.md5");
					shader_md5_content.save(shaderInfos.contentMD5.c_str(), shaderInfos.contentMD5.size());

					// compile shader and create bc.code file					
				}
				

			}
			catch (const std::exception& e)
			{
				_RENDERME_ERROR(m_localLoggerRunner, std::string("failed to manage ") + shader_path + " : reason = " + e.what());

				// send error status to main thread and let terminate
				const Runner::TaskReport report{ RunnerEvent::TASK_ERROR, shaderInfos.name, shader_action };
				m_runner.m_mailbox_out.push<Runner::TaskReport>(report);
			}
		}
	);

	m_runner.m_mailbox_in.push<renderMe::property::AsyncTask*>(&loadShader);
}

void ResourceSystem::killRunner()
{
	renderMe::core::RunnerKiller runnerKiller;
	m_runner.m_mailbox_in.push<renderMe::property::AsyncTask*>(&runnerKiller);
	m_runner.join();
}