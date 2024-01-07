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
#include "logger_service.h"

#include "shaders_service.h"
#include "shader.h"

using namespace renderMe;
using namespace renderMe::core;



ResourceSystem::ResourceSystem(Entitygraph& p_entitygraph) : System(p_entitygraph),
m_localLogger("ResourceSystem", renderMe::core::logger::Configuration::getInstance()),
m_localLoggerRunner("ResourceSystemRunner", renderMe::core::logger::Configuration::getInstance())
{
	m_runner.reserve(nbRunners);

	for (int i = 0; i < nbRunners; i++)
	{
		m_runner.push_back(std::make_unique< renderMe::core::Runner>());
	}
	
	///////// check & create shader cache if needed

	if (!fileSystem::exists(m_shadersCachePath))
	{
		_RENDERME_DEBUG(m_localLogger, std::string("Shader cache missing, creating it..."));
		fileSystem::createDirectory(m_shadersCachePath);

		auto& eventsLogger{ services::LoggerSharing::getInstance()->getLogger("Events") };
		_RENDERME_DEBUG(eventsLogger, "EMIT EVENT -> RESOURCE_SHADER_CACHE_CREATED");

		for (const auto& call : m_callbacks)
		{
			call(ResourceSystemEvent::RESOURCE_SHADER_CACHE_CREATED, m_shadersCachePath);
		}
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
					_EXCEPTION(std::string("failed action ") + p_action_descr + " on target " + p_target_descr );
				}
			}
			else if (renderMe::core::RunnerEvent::TASK_DONE == p_event)
			{
				_RENDERME_DEBUG(m_localLoggerRunner, std::string("TASK_DONE ") + p_target_descr + " " + p_action_descr);
			}

		}
	};

	for (int i = 0; i < nbRunners; i++)
	{
		m_runner[i].get()->registerSubscriber(cb);
		m_runner[i].get()->startup();
	}
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
			const auto s_list{ p_resource_aspect.getComponentsByType<Shader>() };
			for (auto& e : s_list)
			{
				auto& shader{ e->getPurpose() };
				const auto state{ shader.getState() };

				if (Shader::State::INIT == state)
				{
					handleShader(shader, shader.getType());
					shader.setState(Shader::State::BLOBLOADING);
				}
			}
		}
	};

	renderMe::helpers::extractAspectsTopDown<renderMe::core::resourcesAspect>(m_entitygraph, forEachResourceAspect);

	for (int i = 0; i < nbRunners; i++)
	{
		m_runner[i].get()->dispatchEvents();
	}	
}

void ResourceSystem::handleShader(Shader& shaderInfos, int p_shaderType)
{
	const auto shaderType{ p_shaderType };

	_RENDERME_DEBUG(m_localLogger, std::string("Handle shader ") + shaderInfos.getName() + std::string(" shader type ") + std::to_string(shaderType));

	const std::string shaderAction{ "load_shader" };

	const auto task{ new renderMe::core::SimpleAsyncTask<>(shaderAction, shaderInfos.getName(),
		[&,
			shaderType=shaderType,
			shaderAction=shaderAction,
			currentIndex = m_runnerIndex
		]()
		{
			_RENDERME_DEBUG(m_localLoggerRunner, std::string("loading ") + shaderInfos.getName() + " shader type = " + std::to_string(shaderType));

			// build full path
			const auto shader_path{ m_shadersBasePath + "/" + shaderInfos.getName() };
			try
			{
				renderMe::core::FileContent<const char> shader_src_content(shader_path);
				shader_src_content.load();

				// no mutex needed here (only this thread access it)
				shaderInfos.setContentSize( shader_src_content.getDataSize() );
				shaderInfos.setContent( shader_src_content.getData() );

				MD5 md5;
				const std::string shaderMD5{ md5.digestMemory((BYTE*)shader_src_content.getData(), shader_src_content.getDataSize()) };
				shaderInfos.setContentMD5( shaderMD5 );

				const auto decomposed_shader_filename{ fileSystem::splitFilename(shaderInfos.getName()) };
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
					if (!fileSystem::exists(cacheDirectory + "/bc.md5") || !fileSystem::exists(cacheDirectory + "/bc.code"))
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

						if (std::string(cache_md5_content.getData(), cache_md5_content.getDataSize()) != shaderInfos.getContentMD5())
						{
							_RENDERME_TRACE(m_localLoggerRunner, std::string("MD5 not matching ! : ") + shaderInfos.getName());
							generate_cache_entry = true;
						}
						else
						{
							// load bc.code file
							_RENDERME_TRACE(m_localLoggerRunner, std::string("MD5 matches ! : ") + shaderInfos.getName());
						}
					}
				}

				if (generate_cache_entry)
				{
					_RENDERME_TRACE(m_localLoggerRunner, std::string("generating cache entry : ") + shaderInfos.getName());

					std::unique_ptr<char[]> shaderBytes;
					size_t shaderBytesLength;

					auto& eventsLogger{ services::LoggerSharing::getInstance()->getLogger("Events") };

					_RENDERME_DEBUG(eventsLogger, "EMIT EVENT -> RESOURCE_SHADER_COMPILATION_BEGIN : " + shaderInfos.getName());
					for (const auto& call : m_callbacks)
					{
						call(ResourceSystemEvent::RESOURCE_SHADER_COMPILATION_BEGIN, shaderInfos.getName());
					}

					bool compilationStatus;

					if (0 == shaderType)
					{
						services::ShadersCompilationService::getInstance()->requestVertexCompilationShader(fileSystem::splitPath(shader_path).first, shader_src_content, shaderBytes, shaderBytesLength, compilationStatus);
					}
					else if (1 == shaderType)
					{
						services::ShadersCompilationService::getInstance()->requestPixelCompilationShader(fileSystem::splitPath(shader_path).first, shader_src_content, shaderBytes, shaderBytesLength, compilationStatus);
					}

					if (compilationStatus)
					{
						_RENDERME_DEBUG(eventsLogger, "EMIT EVENT -> RESOURCE_SHADER_COMPILATION_SUCCESS : " + shaderInfos.getName());
						for (const auto& call : m_callbacks)
						{
							call(ResourceSystemEvent::RESOURCE_SHADER_COMPILATION_SUCCESS, shaderInfos.getName());
						}

						renderMe::core::FileContent<char> cache_code_content(cacheDirectory + "/bc.code");
						cache_code_content.save(shaderBytes.get(), shaderBytesLength);

						// create cache md5 file
						renderMe::core::FileContent<const char> shader_md5_content(cacheDirectory + "/bc.md5");
						const std::string shaderMD5{ shaderInfos.getContentMD5() };
						shader_md5_content.save(shaderMD5.c_str(), shaderMD5.length());

						// and transfer file content to shaderInfos 'code' buffer
						core::Buffer<char> shaderCode;
						shaderCode.fill(shaderBytes.get(), shaderBytesLength);
						shaderInfos.setCode(shaderCode);
					}
					else
					{

						_RENDERME_DEBUG(eventsLogger, "EMIT EVENT -> RESOURCE_SHADER_COMPILATION_ERROR : " + shaderInfos.getName());
						for (const auto& call : m_callbacks)
						{
							call(ResourceSystemEvent::RESOURCE_SHADER_COMPILATION_ERROR, shaderInfos.getName());
						}

						std::string compilErrorMessage(shaderBytes.get(), shaderBytesLength);
						_EXCEPTION("shader compilation error : " + compilErrorMessage)
					}
				}
				else
				{
					auto& eventsLogger{ services::LoggerSharing::getInstance()->getLogger("Events") };

					_RENDERME_DEBUG(eventsLogger, "EMIT EVENT -> RESOURCE_SHADER_LOAD_BEGIN : " + shaderInfos.getName());
					for (const auto& call : m_callbacks)
					{
						call(ResourceSystemEvent::RESOURCE_SHADER_LOAD_BEGIN, shaderInfos.getName());
					}

					// load bc.code file
					renderMe::core::FileContent<char> cache_code_content(cacheDirectory + "/bc.code");
					cache_code_content.load();

					// transfer file content to shaderInfos 'code' buffer
					core::Buffer<char> shaderCode;
					shaderCode.fill(cache_code_content.getData(), cache_code_content.getDataSize());
					shaderInfos.setCode(shaderCode);

					_RENDERME_DEBUG(eventsLogger, "EMIT EVENT -> RESOURCE_SHADER_LOAD_SUCCESS : " + shaderInfos.getName());
					for (const auto& call : m_callbacks)
					{
						call(ResourceSystemEvent::RESOURCE_SHADER_LOAD_SUCCESS, shaderInfos.getName());
					}

				}
				
				shaderInfos.setState(Shader::State::BLOBLOADED);

			}
			catch (const std::exception& e)
			{
				_RENDERME_ERROR(m_localLoggerRunner, std::string("failed to manage ") + shader_path + " : reason = " + e.what());

				// send error status to main thread and let terminate
				const Runner::TaskReport report{ RunnerEvent::TASK_ERROR, shaderInfos.getName(), shaderAction };
				m_runner[currentIndex].get()->m_mailbox_out.push(report);
			}

		}
	)};

	_RENDERME_DEBUG(m_localLogger, "Pushing to runner number : " + std::to_string(m_runnerIndex));

	m_runner[m_runnerIndex].get()->m_mailbox_in.push(task);

	m_runnerIndex++;
	if (m_runnerIndex == nbRunners)
	{
		m_runnerIndex = 0;
	}
}

void ResourceSystem::killRunner()
{
	renderMe::core::RunnerKiller runnerKiller;

	for (int i = 0; i < nbRunners; i++)
	{
		m_runner[i].get()->m_mailbox_in.push(&runnerKiller);
		m_runner[i].get()->join();
	}
}