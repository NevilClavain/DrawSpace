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

#include "logger_service.h"

#include "shaders_service.h"
#include "shader.h"
#include "texture.h"
#include <utility>

#include "datacloud.h"

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
				if ("load_texture" == p_action_descr)
				{
					// rethrow in current thread
					_EXCEPTION(std::string("failed action ") + p_action_descr + " on target " + p_target_descr);
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

	//// for shaders json metadata parsing

	m_jsonparser_cb = [&, this](JSONEvent p_event, const std::string& p_id, int p_index, const std::string& p_value, const std::optional<Shader*>& p_shader_opt)
	{
		static			std::string			section_name;
		thread_local	Shader::Argument	s_argument;

		Shader* shader_dest { p_shader_opt.value()};

		switch (p_event)
		{
			case renderMe::core::JSONEvent::ARRAY_BEGIN:

				section_name = p_id;
				_RENDERME_DEBUG(m_localLoggerRunner, "shaders json metadata parsing : ARRAY_BEGIN : " + p_id);
				break;

			case renderMe::core::JSONEvent::ARRAY_END:

				_RENDERME_DEBUG(m_localLoggerRunner, "shaders json metadata parsing : ARRAY_END : " + p_id);

				if ("inputs" == section_name)
				{
					_RENDERME_DEBUG(m_localLoggerRunner, "shaders json metadata parsing : ARRAY_END on inputs section ");
					section_name = "";
				
					if (s_argument.shader_register > -1 && s_argument.argument_id != "" && s_argument.argument_type != "")
					{
						_RENDERME_DEBUG(m_localLoggerRunner, "shaders json metadata parsing : SUCCESS, addArgument");
						shader_dest->addArgument(s_argument);
					}
					else
					{
						_RENDERME_WARN(m_localLoggerRunner, "shaders json metadata parsing : cannot add argument, incomplete s_argument");
					}
				}
				break;

			case renderMe::core::JSONEvent::STRING:

				if ("inputs" == section_name)
				{
					if ("type" == p_id)
					{
						_RENDERME_DEBUG(m_localLoggerRunner, "shaders json metadata parsing : found type : " + p_value);
						s_argument.argument_type = p_value;
					}
					else if ("argument_id" == p_id)
					{
						_RENDERME_DEBUG(m_localLoggerRunner, "shaders json metadata parsing : found argument_id : " + p_value);
						s_argument.argument_id = p_value;
					}
				}
				break;
			
			case renderMe::core::JSONEvent::PRIMITIVE:

				if ("inputs" == section_name)
				{
					if ("register" == p_id)
					{
						_RENDERME_DEBUG(m_localLoggerRunner, "shaders json metadata parsing : found register : " + p_value);
						s_argument.shader_register = std::atoi(p_value.c_str());
					}
				}
				break;			
		}
	};
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

			////// Handle shaders ///////////
			//const auto s_list{ p_resource_aspect.getComponentsByType<Shader>() };

			const auto s_list{ p_resource_aspect.getComponentsByType<std::pair<std::string, Shader>>() };
			for (auto& e : s_list)
			{
				auto& shader{ e->getPurpose().second };
				const auto filename{ e->getPurpose().first};

				const auto state{ shader.getState() };
				if (Shader::State::INIT == state)
				{
					handleShader(shader, shader.getType(), filename);
					shader.setState(Shader::State::BLOBLOADING);
				}
			}
			////// Handle textures ///////////
			const auto t_list{ p_resource_aspect.getComponentsByType<std::pair<size_t, std::pair<std::string, Texture>>>() };
			for (auto& e : t_list)
			{
				auto& staged_texture{ e->getPurpose() };
				Texture& texture{ staged_texture.second.second };
				const auto filename{ staged_texture.second.first };
					
				const auto state{ texture.getState() };
				if (Texture::State::INIT == state)
				{
					handleTexture(texture, filename);
					texture.setState(Texture::State::BLOBLOADING);
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

void ResourceSystem::handleShader(Shader& shaderInfos, int p_shaderType, const std::string& p_filename)
{
	const auto shaderType{ p_shaderType };

	_RENDERME_DEBUG(m_localLogger, std::string("Handle shader ") + p_filename + std::string(" shader type ") + std::to_string(shaderType));

	const std::string shaderAction{ "load_shader" };

	const auto task{ new renderMe::core::SimpleAsyncTask<>(shaderAction, p_filename,
		[&,
			shaderType=shaderType,
			shaderAction=shaderAction,
			currentIndex = m_runnerIndex,
			filename = p_filename
		]()
		{
			_RENDERME_DEBUG(m_localLoggerRunner, std::string("loading ") + filename + " shader type = " + std::to_string(shaderType));

			// build full path
			const auto shader_path{ m_shadersBasePath + "/" + filename + ".hlsl"};
			const auto shader_metadata_path{ m_shadersBasePath + "/" + filename + ".json" };
			try
			{
				renderMe::core::FileContent<const char> shader_src_content(shader_path);
				shader_src_content.load();

				// no mutex needed here (only this thread access it)
				shaderInfos.setContentSize( shader_src_content.getDataSize() );
				shaderInfos.setContent( shader_src_content.getData() );
				shaderInfos.m_source_id = filename;
				shaderInfos.compute_resource_uid();

				/*
				MD5 md5;
				const std::string shaderMD5{ md5.digestMemory((BYTE*)shader_src_content.getData(), shader_src_content.getDataSize()) };
				shaderInfos.setContentMD5( shaderMD5 );
				*/

				/*
				const auto decomposed_shader_filename{ fileSystem::splitFilename(p_filename) };
				const auto shaderCacheDirectory{ m_shadersCachePath + "/" + decomposed_shader_filename.first };
				*/

				const auto shaderCacheDirectory{ m_shadersCachePath + "/" + filename };

				bool generate_cache_entry{ false };

				///////// check driver version change...

				// get current driver version
				const auto dataCloud{ renderMe::rendering::Datacloud::getInstance() };
				const auto current_driver{ dataCloud->readDataValue<std::string>("std.gpu_driver") };

				bool update_driver_text{ false };

				if (fileSystem::exists(m_shadersCachePath + "/driverversion.text"))
				{
					// file exists
					renderMe::core::FileContent<const char> driverversion_content(m_shadersCachePath + "/driverversion.text");
					driverversion_content.load();

					const std::string last_driverversion(driverversion_content.getData(), driverversion_content.getDataSize());

					if (current_driver != last_driverversion)
					{
						update_driver_text = true;
					}
				}
				else
				{
					update_driver_text = true;
				}
				
				if(update_driver_text) // update driver text and so rebuild shaders
				{
					renderMe::core::FileContent<const char> driverversion_content(m_shadersCachePath + "/driverversion.text");
					driverversion_content.save(current_driver.c_str(), current_driver.length());

					generate_cache_entry = true;
				}
				
				///////// check if shader exists in cache...

				if (!fileSystem::exists(shaderCacheDirectory))
				{
					_RENDERME_TRACE(m_localLoggerRunner, std::string("cache directory missing : ") + shaderCacheDirectory);

					// create all
					fileSystem::createDirectory(shaderCacheDirectory);
					generate_cache_entry = true;
				}
				else
				{
					_RENDERME_TRACE(m_localLoggerRunner, std::string("cache directory exists : ") + shaderCacheDirectory);

					// check if cache md5 file exists AND compiled shader exists
					if (!fileSystem::exists(shaderCacheDirectory + "/bc.md5") || !fileSystem::exists(shaderCacheDirectory + "/bc.code"))
					{
						_RENDERME_TRACE(m_localLoggerRunner, std::string("cache file missing !"));
						generate_cache_entry = true;
					}
					else
					{
						_RENDERME_TRACE(m_localLoggerRunner, std::string("cache md5 file exists : ") + shaderCacheDirectory + "/bc.md5");

						// load cache md5 file
						renderMe::core::FileContent<char> cache_md5_content(shaderCacheDirectory + "/bc.md5");
						cache_md5_content.load();

						// check if md5 are equals

						if (std::string(cache_md5_content.getData(), cache_md5_content.getDataSize()) != shaderInfos.m_resource_uid)
						{
							_RENDERME_TRACE(m_localLoggerRunner, std::string("MD5 not matching ! : ") + filename);
							generate_cache_entry = true;
						}
						else
						{
							// load bc.code file
							_RENDERME_TRACE(m_localLoggerRunner, std::string("MD5 matches ! : ") + filename);
						}
					}
				}

				if (generate_cache_entry)
				{
					_RENDERME_TRACE(m_localLoggerRunner, std::string("generating cache entry : ") + filename);

					std::unique_ptr<char[]> shaderBytes;
					size_t shaderBytesLength;

					auto& eventsLogger{ services::LoggerSharing::getInstance()->getLogger("Events") };

					_RENDERME_DEBUG(eventsLogger, "EMIT EVENT -> RESOURCE_SHADER_COMPILATION_BEGIN : " + filename);
					for (const auto& call : m_callbacks)
					{
						call(ResourceSystemEvent::RESOURCE_SHADER_COMPILATION_BEGIN, filename);
					}

					bool compilationStatus;

					if (vertexShader == shaderType)
					{
						services::ShadersCompilationService::getInstance()->requestVertexCompilationShader(fileSystem::splitPath(shader_path).first, shader_src_content, shaderBytes, shaderBytesLength, compilationStatus);
					}
					else if (pixelShader == shaderType)
					{
						services::ShadersCompilationService::getInstance()->requestPixelCompilationShader(fileSystem::splitPath(shader_path).first, shader_src_content, shaderBytes, shaderBytesLength, compilationStatus);
					}

					if (compilationStatus)
					{
						_RENDERME_DEBUG(eventsLogger, "EMIT EVENT -> RESOURCE_SHADER_COMPILATION_SUCCESS : " + filename);
						for (const auto& call : m_callbacks)
						{
							call(ResourceSystemEvent::RESOURCE_SHADER_COMPILATION_SUCCESS, filename);
						}

						renderMe::core::FileContent<char> cache_code_content(shaderCacheDirectory + "/bc.code");
						cache_code_content.save(shaderBytes.get(), shaderBytesLength);

						// create cache md5 file
						renderMe::core::FileContent<const char> shader_md5_content(shaderCacheDirectory + "/bc.md5");
						const std::string shaderMD5{ shaderInfos.m_resource_uid };
						shader_md5_content.save(shaderMD5.c_str(), shaderMD5.length());

						// and transfer file content to shaderInfos 'code' buffer
						core::Buffer<char> shaderCode;
						shaderCode.fill(shaderBytes.get(), shaderBytesLength);
						shaderInfos.setCode(shaderCode);
					}
					else
					{

						_RENDERME_DEBUG(eventsLogger, "EMIT EVENT -> RESOURCE_SHADER_COMPILATION_ERROR : " + filename);
						for (const auto& call : m_callbacks)
						{
							call(ResourceSystemEvent::RESOURCE_SHADER_COMPILATION_ERROR, filename);
						}

						std::string compilErrorMessage(shaderBytes.get(), shaderBytesLength);
						_EXCEPTION("shader compilation error : " + compilErrorMessage)
					}
				}
				else
				{
					auto& eventsLogger{ services::LoggerSharing::getInstance()->getLogger("Events") };

					_RENDERME_DEBUG(eventsLogger, "EMIT EVENT -> RESOURCE_SHADER_LOAD_BEGIN : " + filename);
					for (const auto& call : m_callbacks)
					{
						call(ResourceSystemEvent::RESOURCE_SHADER_LOAD_BEGIN, filename);
					}

					// load bc.code file
					renderMe::core::FileContent<char> cache_code_content(shaderCacheDirectory + "/bc.code");
					cache_code_content.load();

					// transfer file content to shaderInfos 'code' buffer
					core::Buffer<char> shaderCode;
					shaderCode.fill(cache_code_content.getData(), cache_code_content.getDataSize());
					shaderInfos.setCode(shaderCode);

					_RENDERME_DEBUG(eventsLogger, "EMIT EVENT -> RESOURCE_SHADER_LOAD_SUCCESS : " + filename);
					for (const auto& call : m_callbacks)
					{
						call(ResourceSystemEvent::RESOURCE_SHADER_LOAD_SUCCESS, filename);
					}
				}

				///// manage metadata json file

				renderMe::core::FileContent<const char> shadermetadata_src_content(shader_metadata_path);
				shadermetadata_src_content.load();

				const auto metadataSize{ shadermetadata_src_content.getDataSize() };
				const std::string metadata(shadermetadata_src_content.getData(), metadataSize);

				// json parser seem to be not thread-safe -> enter critical section
				m_jsonparser_mutex.lock();
				renderMe::core::Json<Shader> jsonParser;
				jsonParser.registerSubscriber(m_jsonparser_cb);			
				const auto logParseStatus{ jsonParser.parse(metadata, &shaderInfos) };
				m_jsonparser_mutex.unlock();

				if (logParseStatus < 0)
				{
					_EXCEPTION("JSON parse error on " + shader_metadata_path);
				}

				////////////////////////////////
				
				shaderInfos.setState(Shader::State::BLOBLOADED);
			}
			catch (const std::exception& e)
			{
				_RENDERME_ERROR(m_localLoggerRunner, std::string("failed to manage ") + shader_path + " : reason = " + e.what());

				// send error status to main thread and let terminate
				const Runner::TaskReport report{ RunnerEvent::TASK_ERROR, filename, shaderAction };
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

void ResourceSystem::handleTexture(Texture& textureInfos, const std::string& p_filename)
{
	_RENDERME_DEBUG(m_localLogger, std::string("Handle Texture ") + p_filename);

	const std::string textureAction{ "load_texture" };

	const auto task{ new renderMe::core::SimpleAsyncTask<>(textureAction, p_filename,
		[&,
			textureAction = textureAction,
			currentIndex = m_runnerIndex,
			filename = p_filename
		]()
		{
			_RENDERME_DEBUG(m_localLoggerRunner, std::string("loading ") + filename);

			// build full path
			const auto texture_path{ m_texturesBasePath + "/" + filename };

			try
			{
				auto& eventsLogger{ services::LoggerSharing::getInstance()->getLogger("Events") };

				_RENDERME_DEBUG(eventsLogger, "EMIT EVENT -> RESOURCE_TEXTURE_LOAD_BEGIN : " + filename);
				for (const auto& call : m_callbacks)
				{
					call(ResourceSystemEvent::RESOURCE_TEXTURE_LOAD_BEGIN, filename);
				}


				renderMe::core::FileContent<unsigned char> texture_content(texture_path);
				texture_content.load();

				textureInfos.setState(Texture::State::BLOBLOADED);

				// transfer file content to textureInfos buffer

				core::Buffer<unsigned char> textureBytes;
				textureBytes.fill(texture_content.getData(), texture_content.getDataSize());
				textureInfos.m_file_content = textureBytes;

				_RENDERME_DEBUG(eventsLogger, "EMIT EVENT -> RESOURCE_TEXTURE_LOAD_SUCCESS : " + filename);
				for (const auto& call : m_callbacks)
				{
					call(ResourceSystemEvent::RESOURCE_TEXTURE_LOAD_SUCCESS, filename);
				}

				textureInfos.m_source = Texture::Source::CONTENT_FROM_FILE;
				textureInfos.m_source_id = filename;
				textureInfos.compute_resource_uid();
			}
			catch (const std::exception& e)
			{
				_RENDERME_ERROR(m_localLoggerRunner, std::string("failed to manage ") + texture_path + " : reason = " + e.what());

				// send error status to main thread and let terminate
				const Runner::TaskReport report{ RunnerEvent::TASK_ERROR, filename, textureAction };
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