/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2020                     
*                                                                          
* This file is part of DrawSpace.                                          
*                                                                          
*    DrawSpace is free software: you can redistribute it and/or modify     
*    it under the terms of the GNU General Public License as published by  
*    the Free Software Foundation, either version 3 of the License, or     
*    (at your option) any later version.                                   
*                                                                          
*    DrawSpace is distributed in the hope that it will be useful,          
*    but WITHOUT ANY WARRANTY; without even the implied warranty of        
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         
*    GNU General Public License for more details.                          
*                                                                          
*    You should have received a copy of the GNU General Public License     
*    along with DrawSpace.  If not, see <http://www.gnu.org/licenses/>.    
*
*/
/* -*-LIC_END-*- */

#include "renderer.h"
#include "resourcessystem.h"


#include "animationsaspect.h"

#include "texture.h"
#include "shader.h"
#include "meshe.h"


#include "renderer.h"
#include "plugin.h"
#include "misc_utils.h"
#include "md5.h"

#include "loadfiletask.h"
#include "loadfiletoassimptask.h"
#include "readshadermd5task.h"
#include "compileshadertask.h"
#include "createdirectorytask.h"
#include "updatebcfilestask.h"
#include "fillmeshesownernamestask.h"
#include "loadanimationstask.h"

#include "runnersystem.h"

DrawSpace::Logger::Sink rs_logger("ResourcesSystem", DrawSpace::Logger::Configuration::GetInstance());

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::EntityGraph;
using namespace DrawSpace::Systems;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface;
using namespace DrawSpace::AspectImplementations;

dsstring ResourcesSystem::m_textures_rootpath { "." };
dsstring ResourcesSystem::m_meshes_rootpath { "." };
dsstring ResourcesSystem::m_shaders_rootpath { "." };
bool ResourcesSystem::m_addshaderspath { false };

const dsstring ResourcesSystem::bcCacheName{ "bc_cache" };
const dsstring ResourcesSystem::bcMd5FileName{ "bc.md5" };
const dsstring ResourcesSystem::bcCodeFileName{ "bc.code" };


ResourcesSystem::ResourcesSystem(RunnerSystem& p_runner) :
m_runner_system(p_runner)
{
	m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

	// create bc_cache dir if not exists
	check_bc_cache_presence();

}

void ResourcesSystem::RegisterEventHandler(ResourceEventHandler* p_handler)
{
	m_evt_handlers.insert(p_handler);
}

void ResourcesSystem::UnregisterEventHandler(ResourceEventHandler* p_handler)
{
	m_evt_handlers.erase(p_handler);
}

void ResourcesSystem::notify_event(ResourceEvent p_event, const dsstring& p_path) const
{
	static const std::map<ResourceEvent, dsstring> event_to_string 
	{
		{ BLOB_LOAD, "BLOB_LOAD"},
		{ BLOB_LOADED, "BLOB_LOADED"},
		{ SHADERCACHE_CREATION, "SHADERCACHE_CREATION"},
		{ SHADER_COMPILATION, "SHADER_COMPILATION"},
		{ SHADER_COMPILED, "SHADER_COMPILED"},
		{ ALL_ASSETS_LOADED, "ALL_ASSETS_LOADED"},
	};

	dsstring event_str{ event_to_string.at(p_event) };
	dsstring resource{ p_path };

	_DSDEBUG(rs_logger, dsstring("resources => ") << event_str << dsstring( " " ) << resource);

	for (auto& e : m_evt_handlers)
	{
		(*e)(p_event, p_path);
	}
}

void ResourcesSystem::NotifyEvent(ResourceEvent p_event, const dsstring& p_path) const
{
	notify_event(p_event, p_path);
}


void ResourcesSystem::EnableShadersDescrInFinalPath(bool p_state)
{
    m_addshaderspath = p_state;
}

void ResourcesSystem::SetShadersRootPath(const dsstring& p_path)
{
    m_shaders_rootpath = p_path;
}

void ResourcesSystem::SetTexturesRootPath(const dsstring& p_path)
{
    m_textures_rootpath = p_path;
}

void ResourcesSystem::SetMeshesRootPath(const dsstring& p_path)
{
    m_meshes_rootpath = p_path;
}

void ResourcesSystem::run(EntityGraph::EntityNodeGraph* p_entitygraph)
{
	//check_finished_tasks();

    p_entitygraph->AcceptSystemRootToLeaf( this );

	check_all_assets_loaded();
}

void ResourcesSystem::VisitEntity(Entity* p_parent, Entity* p_entity)
{
	//Threading::Runner* runner{ Threading::Runner::GetInstance() };

    ResourcesAspect* resources_aspect = p_entity->GetAspect<ResourcesAspect>();
    if (resources_aspect)
    {
        ComponentList<std::tuple<Texture*, bool>> textures_assets;
        resources_aspect->GetComponentsByType<std::tuple<Texture*, bool>>(textures_assets);

        for(auto& e : textures_assets)
        {
            bool& loaded = std::get<1>( e->getPurpose() );
            if( !loaded )
            {				
				dsstring asset_path;
				std::get<0>(e->getPurpose())->GetBasePath(asset_path);
				dsstring final_asset_path = compute_textures_final_path(asset_path);

				if (!m_runner_system.HasSequence(final_asset_path))
				{
					// check if already in cache
					if (m_texturesCache.find(final_asset_path) == m_texturesCache.end())
					{

						// if not, setup a new sequence

						RunnerSequenceStep load_texture_step;

						std::map<dsstring, bool>* asset_loading_state{ &m_asset_loading_state };

						load_texture_step.AddComponent< std::map<dsstring, bool>* >("&m_asset_loading_state", asset_loading_state);
						load_texture_step.AddComponent<dsstring>("final_asset_path", final_asset_path);
						load_texture_step.AddComponent<ResourcesSystem*>("ResourcesSystem", this);
						load_texture_step.AddComponent<std::map<dsstring, Blob>*>("&m_texturesCache", &m_texturesCache);

						load_texture_step.SetRunHandler([](RunnerSequenceStep& p_step, RunnerSequence& p_seq)
						{
							auto final_asset_path{ p_step.GetComponent<dsstring>("final_asset_path")->getPurpose() };
							auto asset_loading_state{ p_step.GetComponent<std::map<dsstring, bool>*>("&m_asset_loading_state")->getPurpose() };
							auto resource_system{ p_step.GetComponent<ResourcesSystem*>("ResourcesSystem")->getPurpose() };

							(*asset_loading_state)[final_asset_path] = false;

							const dsstring task_id{ final_asset_path };

							LoadFileTask* task = _DRAWSPACE_NEW_(LoadFileTask, LoadFileTask);
							task->SetTargetDescr(task_id);
							task->SetActionDescr("LOADASSETFILE");
							task->SetFinalAssetPath(final_asset_path);
							p_step.SetTask(task);

							resource_system->NotifyEvent(BLOB_LOAD, final_asset_path);
						});

						load_texture_step.SetStepCompletedHandler([](RunnerSequenceStep& p_step, RunnerSequence& p_seq)
						{
							auto final_asset_path{ p_step.GetComponent<dsstring>("final_asset_path")->getPurpose() };
							auto texturesCache{ p_step.GetComponent < std::map<dsstring, Blob>*>("&m_texturesCache")->getPurpose() };
							LoadFileTask* task{ static_cast<LoadFileTask*>(p_step.GetTask()) };

							if (task->Failed())
							{
								_DSEXCEPTION("LoadFileTask Failed for texture : " + final_asset_path);
							}

							Blob blob;
							long size = task->GetSize();
							void* data = task->GetData();
							blob.data = data;
							blob.size = size;

							(*texturesCache)[final_asset_path] = blob;

							_DRAWSPACE_DELETE_(task);

							p_seq.DeclareCompleted();
						});

						RunnerSequence sequence;
						sequence.RegisterStep(dsstring("loadTextureStep ") + final_asset_path, load_texture_step);
						sequence.SetCurrentStep(dsstring("loadTextureStep ") + final_asset_path);

						m_runner_system.RegisterSequence(final_asset_path, sequence);
					}
					else
					{
						std::get<0>(e->getPurpose())->SetData(m_texturesCache.at(final_asset_path).data, m_texturesCache.at(final_asset_path).size);
						loaded = true;
					}

				}
				else
				{
					// Check if sequence completed

					if (m_runner_system.IsSequenceCompleted(final_asset_path))
					{
						loaded = true;
						notify_event(BLOB_LOADED, final_asset_path);
						m_asset_loading_state.at(final_asset_path) = true;
						
						std::get<0>(e->getPurpose())->SetData(m_texturesCache.at(final_asset_path).data, m_texturesCache.at(final_asset_path).size);

						m_runner_system.RemoveSequence(final_asset_path);
					}
				}
            }
        }

        ComponentList<std::tuple<Shader*, bool, int>> shaders_assets;
        resources_aspect->GetComponentsByType<std::tuple<Shader*, bool, int>>(shaders_assets);

        for (auto& e : shaders_assets)
        {
            bool& loaded = std::get<1>(e->getPurpose());
            if (!loaded)
            {

				dsstring asset_path;
				std::get<0>(e->getPurpose())->GetBasePath(asset_path);
				dsstring final_asset_path = compute_shaders_final_path(asset_path);
				dsstring final_asset_dir = compute_shaders_final_path("");
				Shader* shader{ std::get<0>(e->getPurpose()) };
				int shader_type{ std::get<2>(e->getPurpose()) };

				// shader key id
				std::vector<dsstring> items;
				SplitString(asset_path, items, '.');
				dsstring shader_id{ items[0] };

				if (!m_runner_system.HasSequence(final_asset_path))
				{
					// check if already in cache

					if (m_shadersCache.find(final_asset_path) == m_shadersCache.end())
					{
						// if not, setup a new sequence

						RunnerSequenceStep load_shader_step;
						RunnerSequenceStep read_shader_md5_step;
						RunnerSequenceStep load_shaderbc_step;
						RunnerSequenceStep compile_shader_step;
						RunnerSequenceStep update_shaderbc_step;
						RunnerSequenceStep create_directory_step;

						std::map<dsstring, bool>* asset_loading_state{ &m_asset_loading_state };


						load_shader_step.AddComponent< std::map<dsstring, bool>* >("&m_asset_loading_state", asset_loading_state);
						load_shader_step.AddComponent<dsstring>("final_asset_path", final_asset_path);
						load_shader_step.AddComponent<dsstring>("shader_id", shader_id);
						load_shader_step.AddComponent<ResourcesSystem*>("ResourcesSystem", this);
						load_shader_step.AddComponent<std::map<dsstring, Blob>*>("&m_shadersCache", &m_shadersCache);
						load_shader_step.AddComponent<Shader*>("shader", shader);

						load_shader_step.SetRunHandler([](RunnerSequenceStep& p_step, RunnerSequence& p_seq)
						{
							auto final_asset_path{ p_step.GetComponent<dsstring>("final_asset_path")->getPurpose() };
							auto asset_loading_state{ p_step.GetComponent<std::map<dsstring, bool>*>("&m_asset_loading_state")->getPurpose() };
							auto resource_system{ p_step.GetComponent<ResourcesSystem*>("ResourcesSystem")->getPurpose() };

							(*asset_loading_state)[final_asset_path] = false;

							const dsstring task_id{ final_asset_path };

							LoadFileTask* task = _DRAWSPACE_NEW_(LoadFileTask, LoadFileTask);
							task->SetTargetDescr(task_id);
							task->SetActionDescr("LOADASSETFILE");
							task->SetFinalAssetPath(final_asset_path);
							p_step.SetTask(task);

							resource_system->NotifyEvent(BLOB_LOAD, final_asset_path);
						});

						load_shader_step.SetStepCompletedHandler([](RunnerSequenceStep& p_step, RunnerSequence& p_seq)
						{
							Shader* shader{ p_step.GetComponent<Shader*>("shader")->getPurpose() };
							dsstring shader_id{ p_step.GetComponent<dsstring>("shader_id")->getPurpose() };
							auto final_asset_path{ p_step.GetComponent<dsstring>("final_asset_path")->getPurpose() };

							LoadFileTask* task{ static_cast<LoadFileTask*>(p_step.GetTask()) };

							if (task->Failed())
							{
								_DSEXCEPTION("LoadFileTask Failed for shader : " + final_asset_path);
							}

							long size = task->GetSize();
							void* data = task->GetData();

							_DRAWSPACE_DELETE_(task);

							
							if (shader->IsCompiled())
							{
								auto shadersCache{ p_step.GetComponent<std::map<dsstring, Blob>*>("&m_shadersCache")->getPurpose() };

								Blob blob;
								blob.data = data;
								blob.size = size;

								(*shadersCache)[final_asset_path] = blob;

								p_seq.DeclareCompleted();
							}
							else
							{
								MD5 md5;
								dsstring hash_shader{ md5.digestMemory((BYTE*)data, size) };
								dsstring shaderInfos_path{ bcCacheName + dsstring("\\") + shader_id.c_str() };

								if (FileSystem::Exists(shaderInfos_path))
								{
									// read md5 file content...

									// provide loaded shader hash to next step...
									p_seq.GetStep("readShaderMD5Step").AddComponent<dsstring>("hash_shader", hash_shader);

									// provide shader source text to next step...
									p_seq.GetStep("readShaderMD5Step").AddComponent<void*>("text", data);
									p_seq.GetStep("readShaderMD5Step").AddComponent<long>("text_size", size);

									p_seq.SetCurrentStep("readShaderMD5Step");
								}
								else
								{
									// shader entry does not exists, create all...


									// provide loaded shader hash to next step...
									p_seq.GetStep("createDirectoryStep").AddComponent<dsstring>("hash_shader", hash_shader);

									// provide shader source text to next step...
									p_seq.GetStep("createDirectoryStep").AddComponent<void*>("text", data);
									p_seq.GetStep("createDirectoryStep").AddComponent<long>("text_size", size);

									p_seq.SetCurrentStep("createDirectoryStep");
								}
							}

						});

						dsstring bcMd5FileName_path{ bcCacheName + dsstring("\\") + shader_id.c_str() + dsstring("\\") + bcMd5FileName };
						dsstring bcCodeFileName_path{ bcCacheName + dsstring("\\") + shader_id.c_str() + dsstring("\\") + bcCodeFileName };

						read_shader_md5_step.AddComponent<dsstring>("shader_id", shader_id);
						read_shader_md5_step.AddComponent<dsstring>("bcMd5FileName_path", bcMd5FileName_path);
						read_shader_md5_step.AddComponent<dsstring>("final_asset_path", final_asset_path);
						read_shader_md5_step.AddComponent<std::map<dsstring, Blob>*>("&m_shadersCache", &m_shadersCache);
						read_shader_md5_step.AddComponent<Shader*>("shader", shader);

						read_shader_md5_step.SetRunHandler([](RunnerSequenceStep& p_step, RunnerSequence& p_seq)
						{
							dsstring shader_id{ p_step.GetComponent<dsstring>("shader_id")->getPurpose() };
							dsstring bcMd5FileName_path{ p_step.GetComponent<dsstring>("bcMd5FileName_path")->getPurpose() };
							dsstring hash_shader{ p_step.GetComponent<dsstring>("hash_shader")->getPurpose() };

							ReadShaderMD5Task* task = _DRAWSPACE_NEW_(ReadShaderMD5Task, ReadShaderMD5Task);

							task->SetShaderId(shader_id);
							task->SetFilePath(bcMd5FileName_path);
							task->SetTargetDescr(bcMd5FileName_path);
							task->SetCompareMD5(hash_shader);

							p_step.SetTask(task);

						});

						read_shader_md5_step.SetStepCompletedHandler([](RunnerSequenceStep& p_step, RunnerSequence& p_seq)
						{
							dsstring shader_id{ p_step.GetComponent<dsstring>("shader_id")->getPurpose() };
							auto final_asset_path{ p_step.GetComponent<dsstring>("final_asset_path")->getPurpose() };
							dsstring hash_shader{ p_step.GetComponent<dsstring>("hash_shader")->getPurpose() };
							Shader* shader{ p_step.GetComponent<Shader*>("shader")->getPurpose() };

							ReadShaderMD5Task* task{ static_cast<ReadShaderMD5Task*>(p_step.GetTask()) };

							if (task->Failed())
							{
								_DSEXCEPTION("ReadShaderMD5Task Failed for shader : " + final_asset_path);
							}

							bool md5_equals{ task->MD5AreEquals() };
							_DRAWSPACE_DELETE_(task);

							if (md5_equals)
							{
								// crc identical, can directly load bc code file...

								p_seq.SetCurrentStep("loadShaderbcStep");
							}
							else
							{
								// crc changed, update all,

								// forward shader source text to next step....

								void* text{ p_step.GetComponent<void*>("text")->getPurpose() };
								long text_size{ p_step.GetComponent<long>("text_size")->getPurpose() };

								p_seq.GetStep("compileShaderStep").AddComponent<void*>("text", text);
								p_seq.GetStep("compileShaderStep").AddComponent<long>("text_size", text_size);
								p_seq.GetStep("compileShaderStep").AddComponent<dsstring>("hash_shader", hash_shader);

								p_seq.SetCurrentStep("compileShaderStep");
							}
						});


						load_shaderbc_step.AddComponent<dsstring>("final_asset_path", final_asset_path);
						load_shaderbc_step.AddComponent<dsstring>("bcCodeFileName_path", bcCodeFileName_path);
						load_shaderbc_step.AddComponent<std::map<dsstring, Blob>*>("&m_shadersCache", &m_shadersCache);

						load_shaderbc_step.SetRunHandler([](RunnerSequenceStep& p_step, RunnerSequence& p_seq)
						{
							dsstring bcCodeFileName_path{ p_step.GetComponent<dsstring>("bcCodeFileName_path")->getPurpose() };

							LoadFileTask* task = _DRAWSPACE_NEW_(LoadFileTask, LoadFileTask);
							task->SetTargetDescr(bcCodeFileName_path);
							task->SetActionDescr("LOADBCSHADERFILE");
							task->SetFinalAssetPath(bcCodeFileName_path);

							p_step.SetTask(task);

						});

						load_shaderbc_step.SetStepCompletedHandler([](RunnerSequenceStep& p_step, RunnerSequence& p_seq)
						{
							dsstring final_asset_path{ p_step.GetComponent<dsstring>("final_asset_path")->getPurpose() };
							auto shadersCache{ p_step.GetComponent<std::map<dsstring, Blob>*>("&m_shadersCache")->getPurpose() };

							LoadFileTask* task{ static_cast<LoadFileTask*>(p_step.GetTask()) };

							if (task->Failed())
							{
								_DSEXCEPTION("load_shaderbc_step Failed for shader : " + final_asset_path);
							}

							Blob blob;
							long size{ task->GetSize() };
							void* data{ task->GetData() };
							blob.data = data;
							blob.size = size;

							// update cache
							(*shadersCache)[final_asset_path] = blob;

							_DRAWSPACE_DELETE_(task);
							p_seq.DeclareCompleted(); // end of sequence :)

						});



						compile_shader_step.AddComponent<ResourcesSystem*>("ResourcesSystem", this);
						compile_shader_step.AddComponent<dsstring>("final_asset_path", final_asset_path);
						compile_shader_step.AddComponent<dsstring>("final_asset_dir", final_asset_dir);
						compile_shader_step.AddComponent<dsstring>("asset_path", asset_path);
						compile_shader_step.AddComponent<int>("shader_type", shader_type);
						compile_shader_step.AddComponent<DrawSpace::Interface::Renderer*>("Renderer", m_renderer);
						

						compile_shader_step.SetRunHandler([](RunnerSequenceStep& p_step, RunnerSequence& p_seq)
						{
							auto resource_system{ p_step.GetComponent<ResourcesSystem*>("ResourcesSystem")->getPurpose() };
							dsstring final_asset_path{ p_step.GetComponent<dsstring>("final_asset_path")->getPurpose() };

							void* text{ p_step.GetComponent<void*>("text")->getPurpose() };
							long text_size{ p_step.GetComponent<long>("text_size")->getPurpose() };
							int shader_type{ p_step.GetComponent<int>("shader_type")->getPurpose() };

							DrawSpace::Interface::Renderer* renderer{ p_step.GetComponent<DrawSpace::Interface::Renderer*>("Renderer")->getPurpose() };

							dsstring final_asset_dir{ p_step.GetComponent<dsstring>("final_asset_dir")->getPurpose() };
							dsstring asset_path{ p_step.GetComponent<dsstring>("asset_path")->getPurpose() };

							resource_system->NotifyEvent(SHADER_COMPILATION, final_asset_path);

							const dsstring task_id{ final_asset_path };

							CompileShaderTask* task = _DRAWSPACE_NEW_(CompileShaderTask, CompileShaderTask);

							task->SetTargetDescr(task_id);
							task->SetShaderText(text, text_size, shader_type);
							task->SetShaderDirs(final_asset_dir, asset_path);
							task->SetRenderer(renderer);

							p_step.SetTask(task);
						});

						compile_shader_step.SetStepCompletedHandler([](RunnerSequenceStep& p_step, RunnerSequence& p_seq)
						{
							dsstring final_asset_path{ p_step.GetComponent<dsstring>("final_asset_path")->getPurpose() };
							auto resource_system{ p_step.GetComponent<ResourcesSystem*>("ResourcesSystem")->getPurpose() };

							CompileShaderTask* task{ static_cast<CompileShaderTask*>(p_step.GetTask()) };
							if (task->Failed())
							{
								_DSEXCEPTION("compile_shader_step Failed for shader : " + final_asset_path + dsstring(" reason : ") + task->GetCompilationError());
							}

							// get blob
							long bc_length{ task->GetShaderByteCodeLength() };
							void* bc{ task->GetShaderByteCode() };

							_DRAWSPACE_DELETE_(task);

							resource_system->NotifyEvent(SHADER_COMPILED, final_asset_path);

							p_seq.GetStep("updateShaderStep").AddComponent<void*>("bc", bc);
							p_seq.GetStep("updateShaderStep").AddComponent<long>("bc_length", bc_length);

							dsstring hash_shader { p_step.GetComponent<dsstring>("hash_shader")->getPurpose() };


							p_seq.GetStep("updateShaderStep").AddComponent<dsstring>("hash_shader", hash_shader);

							p_seq.SetCurrentStep("updateShaderStep");
							
						});


						update_shaderbc_step.AddComponent<dsstring>("final_asset_path", final_asset_path);
						update_shaderbc_step.AddComponent<dsstring>("shader_id", shader_id);
						update_shaderbc_step.AddComponent<std::map<dsstring, Blob>*>("&m_shadersCache", &m_shadersCache);

						update_shaderbc_step.SetRunHandler([](RunnerSequenceStep& p_step, RunnerSequence& p_seq)
						{
							dsstring final_asset_path{ p_step.GetComponent<dsstring>("final_asset_path")->getPurpose() };
							dsstring shader_id{ p_step.GetComponent<dsstring>("shader_id")->getPurpose() };
							dsstring hash_shader{ p_step.GetComponent<dsstring>("hash_shader")->getPurpose() };

							long bc_length{ p_step.GetComponent<long>("bc_length")->getPurpose() };
							void* bc{ p_step.GetComponent<void*>("bc")->getPurpose() };

							const dsstring task_id{ final_asset_path };

							UpdateBCFilesTask* task = _DRAWSPACE_NEW_(UpdateBCFilesTask, UpdateBCFilesTask);

							task->SetTargetDescr(task_id);
							task->SetShaderId(shader_id);
							task->SetHash(hash_shader);
							task->SetBC(bc, bc_length);

							p_step.SetTask(task);
						});

						update_shaderbc_step.SetStepCompletedHandler([](RunnerSequenceStep& p_step, RunnerSequence& p_seq)
						{
							dsstring final_asset_path{ p_step.GetComponent<dsstring>("final_asset_path")->getPurpose() };
							auto shadersCache{ p_step.GetComponent<std::map<dsstring, Blob>*>("&m_shadersCache")->getPurpose() };
							UpdateBCFilesTask* task{ static_cast<UpdateBCFilesTask*>(p_step.GetTask()) };

							Blob blob;
							long size = { p_step.GetComponent<long>("bc_length")->getPurpose() };
							void* data = { p_step.GetComponent<void*>("bc")->getPurpose() };
							blob.data = data;
							blob.size = size;

							// update cache
							(*shadersCache)[final_asset_path] = blob;

							_DRAWSPACE_DELETE_(task);
							p_seq.DeclareCompleted(); // end of sequence :)
						});


						create_directory_step.AddComponent<dsstring>("final_asset_path", final_asset_path);
						create_directory_step.AddComponent<dsstring>("shader_id", shader_id);

						create_directory_step.SetRunHandler([](RunnerSequenceStep& p_step, RunnerSequence& p_seq)
						{
							dsstring final_asset_path{ p_step.GetComponent<dsstring>("final_asset_path")->getPurpose() };
							dsstring shader_id{ p_step.GetComponent<dsstring>("shader_id")->getPurpose() };

							const dsstring task_id{ final_asset_path };
							CreateDirectoryTask* task = _DRAWSPACE_NEW_(CreateDirectoryTask, CreateDirectoryTask);
							task->SetTargetDescr(task_id);
							task->SetShaderId(shader_id);
							p_step.SetTask(task);
						});

						create_directory_step.SetStepCompletedHandler([](RunnerSequenceStep& p_step, RunnerSequence& p_seq)
						{
							dsstring shader_id{ p_step.GetComponent<dsstring>("shader_id")->getPurpose() };
							CreateDirectoryTask* task{ static_cast<CreateDirectoryTask*>(p_step.GetTask()) };
							_DRAWSPACE_DELETE_(task);

							void* text{ p_step.GetComponent<void*>("text")->getPurpose() };
							long text_size{ p_step.GetComponent<long>("text_size")->getPurpose() };
							dsstring hash_shader{ p_step.GetComponent<dsstring>("hash_shader")->getPurpose() };

							p_seq.GetStep("compileShaderStep").AddComponent<void*>("text", text);
							p_seq.GetStep("compileShaderStep").AddComponent<long>("text_size", text_size);
							p_seq.GetStep("compileShaderStep").AddComponent<dsstring>("hash_shader", hash_shader);

							p_seq.SetCurrentStep("compileShaderStep");
						});


						RunnerSequence sequence;

						sequence.RegisterStep(dsstring("loadShaderStep"), load_shader_step);
						sequence.RegisterStep(dsstring("readShaderMD5Step"), read_shader_md5_step);
						sequence.RegisterStep(dsstring("loadShaderbcStep"), load_shaderbc_step);
						sequence.RegisterStep(dsstring("compileShaderStep"), compile_shader_step);
						sequence.RegisterStep(dsstring("updateShaderStep"), update_shaderbc_step);
						sequence.RegisterStep(dsstring("createDirectoryStep"), create_directory_step);

						sequence.SetCurrentStep(dsstring("loadShaderStep"));

						m_runner_system.RegisterSequence(final_asset_path, sequence);

					}
					else
					{
						shader->SetData(m_shadersCache.at(final_asset_path).data, m_shadersCache.at(final_asset_path).size);
						loaded = true;

						shader->SetCompilationFlag(true); //if shader was already in cache, its mandatory compiled
					}
				}
				else
				{
					// sequence 'final_asset_path' is running

					// Check if sequence completed

					if (m_runner_system.IsSequenceCompleted(final_asset_path))
					{
						loaded = true;
						notify_event(BLOB_LOADED, final_asset_path);
						m_asset_loading_state.at(final_asset_path) = true;

						shader->SetData(m_shadersCache.at(final_asset_path).data, m_shadersCache.at(final_asset_path).size);
						shader->SetCompilationFlag(true); //shader now contains compiled shader

						m_runner_system.RemoveSequence(final_asset_path);
					}
				}
            }
        }

        ComponentList<std::tuple<Meshe*, dsstring, dsstring, bool>> meshes_assets;
        resources_aspect->GetComponentsByType<std::tuple<Meshe*, dsstring, dsstring, bool>>(meshes_assets);
        
        for (auto& e : meshes_assets)
        {
            Meshe* target_meshe = std::get<0>(e->getPurpose());

            bool& loaded = std::get<3>(e->getPurpose());
            if (!loaded)
            {				
				dsstring final_asset_path = compute_meshes_final_path(std::get<1>(e->getPurpose()));
				dsstring meshe_id = std::get<2>(e->getPurpose());
						
				if (!m_runner_system.HasSequence(final_asset_path))
				{
					RunnerSequence sequence;

					// check if already in cache
					if (m_meshesCache.find(final_asset_path) == m_meshesCache.end())
					{
						// if not, setup a new sequence

						RunnerSequenceStep load_meshes_step;

						std::map<dsstring, bool>* asset_loading_state{ &m_asset_loading_state };

						load_meshes_step.AddComponent<std::map<dsstring, bool>*>("&m_asset_loading_state", asset_loading_state);
						load_meshes_step.AddComponent<dsstring>("final_asset_path", final_asset_path);
						load_meshes_step.AddComponent<Meshe*>("target_meshe", target_meshe);
						load_meshes_step.AddComponent<ResourcesSystem*>("ResourcesSystem", this);
						load_meshes_step.AddComponent<std::map<dsstring, std::pair<Assimp::Importer*, const aiScene*>>*>("&m_meshesCache", &m_meshesCache);

						load_meshes_step.SetRunHandler([](RunnerSequenceStep& p_step, RunnerSequence& p_seq)
						{
							auto final_asset_path{ p_step.GetComponent<dsstring>("final_asset_path")->getPurpose() };
							auto asset_loading_state{ p_step.GetComponent<std::map<dsstring, bool>*>("&m_asset_loading_state")->getPurpose() };
							auto target_meshe{ p_step.GetComponent<Meshe*>("target_meshe")->getPurpose() };
							auto resource_system{ p_step.GetComponent<ResourcesSystem*>("ResourcesSystem")->getPurpose() };

							(*asset_loading_state)[final_asset_path] = false;

							const dsstring task_id{ final_asset_path };

							LoadFileToAssimpTask* task =_DRAWSPACE_NEW_(LoadFileToAssimpTask, LoadFileToAssimpTask);
							task->SetTargetDescr(task_id);
							task->SetFinalAssetPath(final_asset_path);
							task->SetTargetMeshe(target_meshe);
							p_step.SetTask(task);

							resource_system->NotifyEvent(BLOB_LOAD, final_asset_path);
							
						});

						load_meshes_step.SetStepCompletedHandler([](RunnerSequenceStep& p_step, RunnerSequence& p_seq)
						{
							auto final_asset_path{ p_step.GetComponent<dsstring>("final_asset_path")->getPurpose() };
							auto resource_system{ p_step.GetComponent<ResourcesSystem*>("ResourcesSystem")->getPurpose() };

							auto meshesCache{ p_step.GetComponent<std::map<dsstring, std::pair<Assimp::Importer*, const aiScene*>>*>("&m_meshesCache")->getPurpose() };
							
							LoadFileToAssimpTask* task{ static_cast<LoadFileToAssimpTask*>(p_step.GetTask()) };

							(*meshesCache)[final_asset_path] = std::make_pair(task->GetImporter(), task->GetScene());
							
							_DRAWSPACE_DELETE_(task);

							resource_system->NotifyEvent(BLOB_LOADED, final_asset_path);

							p_seq.SetCurrentStep("fillMeshesDescriptionStep");
						});

						
						sequence.RegisterStep(dsstring("loadMeshesStep"), load_meshes_step);
						sequence.SetCurrentStep(dsstring("loadMeshesStep"));
						
					}
					else
					{
						sequence.SetCurrentStep(dsstring("fillMeshesDescriptionStep"));
					}


					AnimationsAspect* anims_aspect = p_entity->GetAspect<AnimationsAspect>();

					//register next sequence

					RunnerSequenceStep fill_meshes_description_step;

					fill_meshes_description_step.AddComponent<dsstring>("filename", std::get<1>(e->getPurpose()));
					fill_meshes_description_step.AddComponent<DrawSpace::Logger::Sink*>("&rs_logger", &rs_logger);
					fill_meshes_description_step.AddComponent<dsstring>("final_asset_path", final_asset_path);
					fill_meshes_description_step.AddComponent<std::map<dsstring, std::pair<Assimp::Importer*, const aiScene*>>*>("&m_meshesCache", &m_meshesCache);
					fill_meshes_description_step.AddComponent<AnimationsAspect*>("anims_aspect", anims_aspect);
					
					fill_meshes_description_step.SetRunHandler([](RunnerSequenceStep& p_step, RunnerSequence& p_seq)
					{
						ResourcesAspect::MeshesFileDescription mesheFileDescription;

						DrawSpace::Logger::Sink* rs_logger{ p_step.GetComponent<DrawSpace::Logger::Sink*>("&rs_logger")->getPurpose() };
						auto final_asset_path{ p_step.GetComponent<dsstring>("final_asset_path")->getPurpose() };
						auto meshesCache{ p_step.GetComponent<std::map<dsstring, std::pair<Assimp::Importer*, const aiScene*>>*>("&m_meshesCache")->getPurpose() };

						const aiScene* scene{ (*meshesCache).at(final_asset_path).second };
						

						_DSTRACE((*rs_logger), dsstring("************************************SCENE INFOS***********************************"));
						_DSTRACE((*rs_logger), dsstring("resources = ") << final_asset_path);
						
						_DSTRACE((*rs_logger), dsstring("scene HasMeshes ") << scene->HasMeshes());
						_DSTRACE((*rs_logger), dsstring("scene num Meshes ") << scene->mNumMeshes);

						_DSTRACE((*rs_logger), dsstring("scene HasTextures ") << scene->HasTextures());
						_DSTRACE((*rs_logger), dsstring("scene num Textures ") << scene->mNumTextures);

						_DSTRACE((*rs_logger), dsstring("scene HasMaterials ") << scene->HasMaterials());
						_DSTRACE((*rs_logger), dsstring("scene num Materials ") << scene->mNumMaterials);

						_DSTRACE((*rs_logger), dsstring("scene HasLights ") << scene->HasLights());
						_DSTRACE((*rs_logger), dsstring("scene num Lights ") << scene->mNumLights);

						_DSTRACE((*rs_logger), dsstring("scene HasCameras ") << scene->HasCameras());
						_DSTRACE((*rs_logger), dsstring("scene num Cameras ") << scene->mNumCameras);

						_DSTRACE((*rs_logger), dsstring("scene HasAnimations ") << scene->HasAnimations());
						_DSTRACE((*rs_logger), dsstring("scene num Animations ") << scene->mNumAnimations);
						
						mesheFileDescription.file = p_step.GetComponent<dsstring>("filename")->getPurpose();
						mesheFileDescription.has_meshes = scene->HasMeshes();
						mesheFileDescription.num_meshes = scene->mNumMeshes;

						mesheFileDescription.has_animations = scene->HasAnimations();
						mesheFileDescription.num_animations = scene->mNumAnimations;

						p_seq.AddComponent<ResourcesAspect::MeshesFileDescription>("mesheFileDescription", mesheFileDescription);

						aiNode* root{ scene->mRootNode };

						_DSTRACE((*rs_logger), dsstring("************************************NODE HIERARCHY BEGIN***********************************"));

						ResourcesSystem::DumpAssimpSceneNode(root, 1, rs_logger);

						_DSTRACE((*rs_logger), dsstring("************************************NODE HIERARCHY END*************************************"));

						const dsstring task_id{ final_asset_path };

						FillMeshesOwnerNamesTask* task = _DRAWSPACE_NEW_(FillMeshesOwnerNamesTask, FillMeshesOwnerNamesTask);

						task->SetTargetDescr(task_id);
						task->SetNbMeshes(scene->mNumMeshes);
						task->SetRoot(root);

						p_step.SetTask(task);

					});

					fill_meshes_description_step.SetStepCompletedHandler([](RunnerSequenceStep& p_step, RunnerSequence& p_seq)
					{
						auto final_asset_path{ p_step.GetComponent<dsstring>("final_asset_path")->getPurpose() };
						auto meshesCache{ p_step.GetComponent<std::map<dsstring, std::pair<Assimp::Importer*, const aiScene*>>*>("&m_meshesCache")->getPurpose() };
						AnimationsAspect* anims_aspect{ p_step.GetComponent<AnimationsAspect*>("anims_aspect")->getPurpose() };

						FillMeshesOwnerNamesTask* task{ static_cast<FillMeshesOwnerNamesTask*>(p_step.GetTask()) };

						const aiScene* scene{ (*meshesCache).at(final_asset_path).second };

						// associe a aiMesh** meshes -> meshes_node_owner_names[i] -> contient le nom du node proprietaire du meshes[i]
						std::vector<dsstring> meshes_node_owner_names{ task->GetNodesNamesList() };

						//p_seq.GetComponent< ResourcesAspect::MeshesFileDescription>("mesheFileDescription").getPurpose().

						aiMesh** meshes{ scene->mMeshes };

						// ICI : remplir les descriptions meshes a partir du tableau aiMesh** meshes;
						for (unsigned int i = 0; i < scene->mNumMeshes; i++)
						{
							ResourcesAspect::MesheDescription mesheDescription;
							mesheDescription.node_id = meshes_node_owner_names[i];
							mesheDescription.name = meshes[i]->mName.C_Str();
							mesheDescription.has_positions = meshes[i]->HasPositions();
							mesheDescription.has_faces = meshes[i]->HasFaces();
							mesheDescription.has_normales = meshes[i]->HasNormals();
							mesheDescription.has_tbn = meshes[i]->HasTangentsAndBitangents();
							mesheDescription.num_bones = meshes[i]->mNumBones;
							mesheDescription.num_vertices = meshes[i]->mNumVertices;
							mesheDescription.num_faces = meshes[i]->mNumFaces;
							mesheDescription.num_uvchannels = meshes[i]->GetNumUVChannels();

							p_seq.GetComponent< ResourcesAspect::MeshesFileDescription>("mesheFileDescription")->getPurpose().meshes_descriptions.push_back(mesheDescription);
						}
						
						_DRAWSPACE_DELETE_(task);
					
						if (anims_aspect)
						{
							p_seq.SetCurrentStep("fillMeshesAnimationsStep");
						}
						else
						{
							p_seq.SetCurrentStep("buildMesheStep");
						}						
					});


					RunnerSequenceStep fill_meshes_animations_step;

					fill_meshes_animations_step.AddComponent<DrawSpace::Logger::Sink*>("&rs_logger", &rs_logger);
					fill_meshes_animations_step.AddComponent<std::map<dsstring, std::pair<Assimp::Importer*, const aiScene*>>*>("&m_meshesCache", &m_meshesCache);
					fill_meshes_animations_step.AddComponent<dsstring>("final_asset_path", final_asset_path);
					
					fill_meshes_animations_step.AddComponent<AnimationsAspect*>("anims_aspect", anims_aspect);

					fill_meshes_animations_step.SetRunHandler([](RunnerSequenceStep& p_step, RunnerSequence& p_seq)
					{
						DrawSpace::Logger::Sink* rs_logger{ p_step.GetComponent<DrawSpace::Logger::Sink*>("&rs_logger")->getPurpose() };
						auto meshesCache{ p_step.GetComponent<std::map<dsstring, std::pair<Assimp::Importer*, const aiScene*>>*>("&m_meshesCache")->getPurpose() };
						auto final_asset_path{ p_step.GetComponent<dsstring>("final_asset_path")->getPurpose() };
						const aiScene* scene{ (*meshesCache).at(final_asset_path).second };
						AnimationsAspect* anims_aspect{ p_step.GetComponent<AnimationsAspect*>("anims_aspect")->getPurpose() };


						_DSTRACE((*rs_logger), dsstring("************************************Animations list BEGIN***********************************"));
						_DSTRACE((*rs_logger), final_asset_path);

						for (size_t i = 0; i < scene->mNumAnimations; i++)
						{
							_DSTRACE((*rs_logger), dsstring("Animation ") << i);

							aiAnimation* animation = scene->mAnimations[i];

							_DSTRACE((*rs_logger), dsstring("Name = ") << animation->mName.C_Str());
							_DSTRACE((*rs_logger), dsstring("TicksPerSeconds = ") << animation->mTicksPerSecond);
							_DSTRACE((*rs_logger), dsstring("Duration (ticks) = ") << animation->mDuration);
							_DSTRACE((*rs_logger), dsstring("Num Channels = ") << animation->mNumChannels);
							_DSTRACE((*rs_logger), dsstring("Num Mesh Channels = ") << animation->mMeshChannels);
							_DSTRACE((*rs_logger), dsstring("Num Morph Mesh Channels = ") << animation->mNumMorphMeshChannels);

							ResourcesAspect::AnimationDescription animation_description;

							animation_description.name = animation->mName.C_Str();
							animation_description.ticks_per_seconds = animation->mTicksPerSecond;
							animation_description.num_channels = animation->mNumChannels;
							animation_description.duration_seconds = animation->mDuration / animation->mTicksPerSecond;

							p_seq.GetComponent< ResourcesAspect::MeshesFileDescription>("mesheFileDescription")->getPurpose().anims_descriptions.push_back(animation_description);
						}

						_DSTRACE((*rs_logger), dsstring("************************************Animations list END*************************************"));

						const dsstring task_id{ final_asset_path };

						LoadAnimationsTask* task = _DRAWSPACE_NEW_(LoadAnimationsTask, LoadAnimationsTask);

						task->SetTargetDescr(task_id);
						task->SetAnimAspect(anims_aspect);
						task->SetScene(scene);

						p_step.SetTask(task);
					});

					fill_meshes_animations_step.SetStepCompletedHandler([](RunnerSequenceStep& p_step, RunnerSequence& p_seq)
					{
						LoadAnimationsTask* task{ static_cast<LoadAnimationsTask*>(p_step.GetTask()) };

						_DRAWSPACE_DELETE_(task);

						p_seq.SetCurrentStep("buildMesheStep");
					});


					RunnerSequenceStep build_meshe_step;


					build_meshe_step.AddComponent<Entity*>("entity", p_entity);
					build_meshe_step.AddComponent<dsstring>("meshe_id", meshe_id);					
					build_meshe_step.AddComponent<Core::Meshe*>("target_meshe", target_meshe);
					build_meshe_step.AddComponent<dsstring>("final_asset_path", final_asset_path);
					build_meshe_step.AddComponent<ResourcesAspect*>("resources_aspect", resources_aspect);

					build_meshe_step.AddComponent<std::map<dsstring, std::pair<Assimp::Importer*, const aiScene*>>*>("&m_meshesCache", &m_meshesCache);
					build_meshe_step.AddComponent<DrawSpace::Logger::Sink*>("&rs_logger", &rs_logger);

					build_meshe_step.SetRunHandler([](RunnerSequenceStep& p_step, RunnerSequence& p_seq)
					{
						DrawSpace::Logger::Sink* rs_logger{ p_step.GetComponent<DrawSpace::Logger::Sink*>("&rs_logger")->getPurpose() };
						Entity* entity{ p_step.GetComponent<Entity*>("entity")->getPurpose() };
						auto final_asset_path{ p_step.GetComponent<dsstring>("final_asset_path")->getPurpose() };
						auto meshesCache{ p_step.GetComponent<std::map<dsstring, std::pair<Assimp::Importer*, const aiScene*>>*>("&m_meshesCache")->getPurpose() };
						dsstring meshe_id{ p_step.GetComponent<dsstring>("meshe_id")->getPurpose() };
						Core::Meshe* target_meshe{ p_step.GetComponent<Core::Meshe*>("target_meshe")->getPurpose() };
						
						const aiScene* scene{ (*meshesCache).at(final_asset_path).second };
						aiNode* root{ scene->mRootNode };

						aiMesh** meshes{ scene->mMeshes };

						aiNode* meshe_node = root->FindNode(meshe_id.c_str());
						if (!meshe_node)
						{
							_DSEXCEPTION("cannot locate meshe objet " + meshe_id);
						}

						ResourcesSystem::DumpMeshe(meshe_node, meshes, rs_logger);

						const dsstring task_id{ final_asset_path };

						BuildMesheTask* task = _DRAWSPACE_NEW_(BuildMesheTask, BuildMesheTask);
						task->SetTargetDescr(task_id);

						task->SetEntity(entity);
						task->SetMeshesIOInfos(meshe_id, meshes, target_meshe, meshe_node);

						p_step.SetTask(task);

					});

					build_meshe_step.SetStepCompletedHandler([](RunnerSequenceStep& p_step, RunnerSequence& p_seq)
					{
						ResourcesAspect* resource_aspect{ p_step.GetComponent<ResourcesAspect*>("resources_aspect")->getPurpose() };

						BuildMesheTask* task{ static_cast<BuildMesheTask*>(p_step.GetTask()) };
						_DRAWSPACE_DELETE_(task);

						ResourcesAspect::MeshesFileDescription mesheFileDescription{ p_seq.GetComponent< ResourcesAspect::MeshesFileDescription>("mesheFileDescription")->getPurpose() };
						resource_aspect->AddMeshesFileDescription(mesheFileDescription);

						p_seq.DeclareCompleted(); // end of sequence :)
					});



					sequence.RegisterStep(dsstring("fillMeshesDescriptionStep"), fill_meshes_description_step);
					sequence.RegisterStep(dsstring("fillMeshesAnimationsStep"), fill_meshes_animations_step);
					sequence.RegisterStep(dsstring("buildMesheStep"), build_meshe_step);

					m_runner_system.RegisterSequence(final_asset_path, sequence);
				}
				else
				{
					// Check if sequence completed

					if (m_runner_system.IsSequenceCompleted(final_asset_path))
					{
						loaded = true;
						m_asset_loading_state.at(final_asset_path) = true;

						m_runner_system.RemoveSequence(final_asset_path);
					}
				}
            }
        }   
    }	
}


void ResourcesSystem::load_animations(const aiScene* p_scene, AnimationsAspect* p_anims_aspect)
{
	auto& animations = p_anims_aspect->GetComponent<std::map<dsstring, AnimationsAspect::AnimationRoot>>("animations")->getPurpose();

	for (size_t i = 0; i < p_scene->mNumAnimations; i++)
	{
		aiAnimation* ai_animation = p_scene->mAnimations[i];

		AnimationsAspect::AnimationRoot animation;

		animation.duration = ai_animation->mDuration;
		animation.ticksPerSeconds = ai_animation->mTicksPerSecond;

		animations[ai_animation->mName.C_Str()] = animation;

		for (size_t j = 0; j < ai_animation->mNumChannels; j++)
		{
			aiNodeAnim* ai_node_anim = ai_animation->mChannels[j];
			AnimationsAspect::NodeAnimation node_animation;

			node_animation.node_name = ai_node_anim->mNodeName.C_Str();

			for (size_t k = 0; k < ai_node_anim->mNumPositionKeys; k++)
			{			
				aiVectorKey ai_key = ai_node_anim->mPositionKeys[k];
				AnimationsAspect::VectorKey pos_key { ai_key.mTime, { ai_key.mValue[0], ai_key.mValue[1], ai_key.mValue[2], 1.0 } };

				node_animation.position_keys.push_back(pos_key);
			}

			for (size_t k = 0; k < ai_node_anim->mNumScalingKeys; k++)
			{
				aiVectorKey ai_key = ai_node_anim->mScalingKeys[k];
				AnimationsAspect::VectorKey pos_key { ai_key.mTime, { ai_key.mValue[0], ai_key.mValue[1], ai_key.mValue[2], 1.0 } };

				node_animation.scaling_keys.push_back(pos_key);
			}

			for (size_t k = 0; k < ai_node_anim->mNumRotationKeys; k++)
			{
				aiQuatKey ai_key = ai_node_anim->mRotationKeys[k];
				AnimationsAspect::QuaternionKey pos_key { ai_key.mTime, { ai_key.mValue.x, ai_key.mValue.y, ai_key.mValue.z, ai_key.mValue.w } };

				node_animation.rotations_keys.push_back(pos_key);
			}

			if (animation.channels.count(node_animation.node_name) > 0)
			{
				_DSEXCEPTION("animation data load : a node is referenced more than once in animation channels");
			}
			else
			{
				animation.channels[node_animation.node_name] = node_animation;
			}
		}

		animations[ai_animation->mName.C_Str()] = animation;
	}
}

void ResourcesSystem::DumpAssimpSceneNode(aiNode* p_ai_node, int depth, DrawSpace::Logger::Sink* p_rs_logger)
{
    dsstring spacing(depth, ' ');
    _DSTRACE((*p_rs_logger), spacing + dsstring("node : ") << p_ai_node->mName.C_Str() << " nb children : " << p_ai_node->mNumChildren);
    _DSTRACE((*p_rs_logger), spacing + dsstring("nb meshes : ") << p_ai_node->mNumMeshes);

    _DSTRACE((*p_rs_logger), spacing + dsstring("  -> ") << p_ai_node->mTransformation.a1 << " " << p_ai_node->mTransformation.b1 << " " << p_ai_node->mTransformation.c1 << " " << p_ai_node->mTransformation.d1);
    _DSTRACE((*p_rs_logger), spacing + dsstring("  -> ") << p_ai_node->mTransformation.a2 << " " << p_ai_node->mTransformation.b2 << " " << p_ai_node->mTransformation.c2 << " " << p_ai_node->mTransformation.d2);
    _DSTRACE((*p_rs_logger), spacing + dsstring("  -> ") << p_ai_node->mTransformation.a3 << " " << p_ai_node->mTransformation.b3 << " " << p_ai_node->mTransformation.c3 << " " << p_ai_node->mTransformation.d3);
    _DSTRACE((*p_rs_logger), spacing + dsstring("  -> ") << p_ai_node->mTransformation.a4 << " " << p_ai_node->mTransformation.b4 << " " << p_ai_node->mTransformation.c4 << " " << p_ai_node->mTransformation.d4);


    for( size_t i = 0; i < p_ai_node->mNumChildren; i++)
    {
		DumpAssimpSceneNode(p_ai_node->mChildren[i], depth+1, p_rs_logger);
    }
}

void ResourcesSystem::load_scene_nodes_hierachy(aiNode* p_ai_node, int depth, std::map<dsstring, AnimationsAspect::Node>& p_node_table)
{
	AnimationsAspect::Node node;

	node.id = p_ai_node->mName.C_Str();
	if (p_ai_node->mParent)
	{
		node.parent_id = p_ai_node->mParent->mName.C_Str();
	}
	
	node.locale_transform = ConvertFromAssimpMatrix(p_ai_node->mTransformation);

	for (size_t i = 0; i < p_ai_node->mNumChildren; i++)
	{
		node.children.push_back(p_ai_node->mChildren[i]->mName.C_Str());
		load_scene_nodes_hierachy(p_ai_node->mChildren[i], depth + 1, p_node_table);
	}

	if (p_node_table.count(node.id) > 0)
	{
		_DSEXCEPTION("assimp node with same name exists");
	}

	p_node_table[node.id] = node;
}

Utils::Matrix ResourcesSystem::ConvertFromAssimpMatrix(const aiMatrix4x4& p_in_mat)
{
	Utils::Matrix mat;

	mat(0, 0) = p_in_mat.a1;
	mat(0, 1) = p_in_mat.b1;
	mat(0, 2) = p_in_mat.c1;
	mat(0, 3) = p_in_mat.d1;

	mat(1, 0) = p_in_mat.a2;
	mat(1, 1) = p_in_mat.b2;
	mat(1, 2) = p_in_mat.c2;
	mat(1, 3) = p_in_mat.d2;

	mat(2, 0) = p_in_mat.a3;
	mat(2, 1) = p_in_mat.b3;
	mat(2, 2) = p_in_mat.c3;
	mat(2, 3) = p_in_mat.d3;

	mat(3, 0) = p_in_mat.a4;
	mat(3, 1) = p_in_mat.b4;
	mat(3, 2) = p_in_mat.c4;
	mat(3, 3) = p_in_mat.d4;

	return mat;
}

void ResourcesSystem::DumpMeshe(aiNode* p_ai_node, aiMesh** p_meshes, DrawSpace::Logger::Sink* p_rs_logger)
{
	unsigned int nb_meshes = p_ai_node->mNumMeshes;

	_DSTRACE((*p_rs_logger), dsstring("************************************MESHE INFOS***********************************"));
	dsstring name = p_ai_node->mName.C_Str();
	_DSTRACE((*p_rs_logger), dsstring("owner node = ") + name);
	_DSTRACE((*p_rs_logger), dsstring("nb_meshes = ") << nb_meshes);
	
	unsigned int* indexes = p_ai_node->mMeshes;
	for (unsigned int i = 0; i < nb_meshes; i++)
	{
		aiMesh* meshe = p_meshes[indexes[i]];

		_DSTRACE(rs_logger, dsstring(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>MESHE ") << i );		
		_DSTRACE(rs_logger, dsstring("name = ") << dsstring(meshe->mName.C_Str()));
		_DSTRACE(rs_logger, dsstring("meshe HasPositions ") << meshe->HasPositions());
		_DSTRACE(rs_logger, dsstring("meshe HasFaces ") << meshe->HasFaces());
		_DSTRACE(rs_logger, dsstring("meshe HasNormals ") << meshe->HasNormals());
		_DSTRACE(rs_logger, dsstring("meshe HasTangentsAndBitangents ") << meshe->HasTangentsAndBitangents());
		_DSTRACE(rs_logger, dsstring("meshe NumUVChannels ") << meshe->GetNumUVChannels());
		_DSTRACE(rs_logger, dsstring("meshe HasBones ") << meshe->HasBones());
		_DSTRACE(rs_logger, dsstring("meshe NumBones ") << meshe->mNumBones);
		_DSTRACE(rs_logger, dsstring("meshe NumFaces ") << meshe->mNumFaces);
		_DSTRACE(rs_logger, dsstring("meshe NumVertices ") << meshe->mNumVertices);

		for (size_t j = 0; j < meshe->mNumBones; j++)
		{
			aiBone* bone = meshe->mBones[j];

			_DSTRACE(rs_logger, dsstring("Bone ") << j);
			_DSTRACE(rs_logger, dsstring("  -> name = ") << bone->mName.C_Str());
			_DSTRACE(rs_logger, dsstring("  -> offsetMatrx"));
			_DSTRACE(rs_logger, dsstring("  -> ") << bone->mOffsetMatrix.a1 << " " << bone->mOffsetMatrix.b1 << " " << bone->mOffsetMatrix.c1 << " " << bone->mOffsetMatrix.d1);
			_DSTRACE(rs_logger, dsstring("  -> ") << bone->mOffsetMatrix.a2 << " " << bone->mOffsetMatrix.b2 << " " << bone->mOffsetMatrix.c2 << " " << bone->mOffsetMatrix.d2);
			_DSTRACE(rs_logger, dsstring("  -> ") << bone->mOffsetMatrix.a3 << " " << bone->mOffsetMatrix.b3 << " " << bone->mOffsetMatrix.c3 << " " << bone->mOffsetMatrix.d3);
			_DSTRACE(rs_logger, dsstring("  -> ") << bone->mOffsetMatrix.a4 << " " << bone->mOffsetMatrix.b4 << " " << bone->mOffsetMatrix.c4 << " " << bone->mOffsetMatrix.d4);

		}
	}

	_DSTRACE((*p_rs_logger), dsstring("************************************MESHE INFOS END*******************************"));
}

void ResourcesSystem::build_meshe(Entity* p_entity, const dsstring& p_id, aiNode* p_ai_node, aiMesh** p_meshes, Meshe* p_destination)
{
	AnimationsAspect* anims_aspect = p_entity->GetAspect<AnimationsAspect>();

    dsstring name = p_ai_node->mName.C_Str();

    Meshe::NormalesGenerationMode normales_gen_mode = p_destination->GetNGenerationMode();
    Meshe::TangentBinormalesGenerationMode tb_gen_mode = p_destination->GetTBGenerationMode();

    unsigned int nb_meshes = p_ai_node->mNumMeshes;
    int global_index = 0;

    unsigned int* indexes = p_ai_node->mMeshes;
    for( unsigned int i = 0; i < nb_meshes; i++ )
    {
        aiMesh* meshe = p_meshes[indexes[i]];

        _DSTRACE(rs_logger, dsstring("************************************MESHE INFOS***********************************"));
        _DSTRACE(rs_logger, dsstring("owner node id = ") + p_id);
        _DSTRACE(rs_logger, dsstring("name = ") << dsstring( meshe->mName.C_Str() ));
        _DSTRACE(rs_logger, dsstring("meshe HasPositions ") << meshe->HasPositions());
        _DSTRACE(rs_logger, dsstring("meshe HasFaces ") << meshe->HasFaces());
        _DSTRACE(rs_logger, dsstring("meshe HasNormals ") << meshe->HasNormals());        
        _DSTRACE(rs_logger, dsstring("meshe HasTangentsAndBitangents ") << meshe->HasTangentsAndBitangents());
        _DSTRACE(rs_logger, dsstring("meshe NumUVChannels ") << meshe->GetNumUVChannels());
        _DSTRACE(rs_logger, dsstring("meshe HasBones ") << meshe->HasBones());
        _DSTRACE(rs_logger, dsstring("meshe NumBones ") << meshe->mNumBones);
        
        for(size_t j = 0; j < meshe->mNumBones; j++)
        {
            aiBone* bone = meshe->mBones[j];

            _DSTRACE(rs_logger, dsstring("Bone ") << j);
            _DSTRACE(rs_logger, dsstring("  -> name = ") << bone->mName.C_Str());
            _DSTRACE(rs_logger, dsstring("  -> offsetMatrx"));
            _DSTRACE(rs_logger, dsstring("  -> ") << bone->mOffsetMatrix.a1 << " " << bone->mOffsetMatrix.b1 << " " << bone->mOffsetMatrix.c1 << " " << bone->mOffsetMatrix.d1);
            _DSTRACE(rs_logger, dsstring("  -> ") << bone->mOffsetMatrix.a2 << " " << bone->mOffsetMatrix.b2 << " " << bone->mOffsetMatrix.c2 << " " << bone->mOffsetMatrix.d2);
            _DSTRACE(rs_logger, dsstring("  -> ") << bone->mOffsetMatrix.a3 << " " << bone->mOffsetMatrix.b3 << " " << bone->mOffsetMatrix.c3 << " " << bone->mOffsetMatrix.d3);
            _DSTRACE(rs_logger, dsstring("  -> ") << bone->mOffsetMatrix.a4 << " " << bone->mOffsetMatrix.b4 << " " << bone->mOffsetMatrix.c4 << " " << bone->mOffsetMatrix.d4);

        }


        _DSTRACE(rs_logger, dsstring("************************************MESHE INFOS END*******************************"));
        
        for( size_t j = 0; j < meshe->mNumFaces; j++)
        {
            aiFace face = meshe->mFaces[j];

            if(face.mNumIndices != 3)
            {
                _DSEXCEPTION( "Face must have exactly 3 indices");
            }

            int i1 = face.mIndices[0];
            int i2 = face.mIndices[1];
            int i3 = face.mIndices[2];

            p_destination->AddTriangle(Core::Triangle(i1 + global_index, i2 + global_index, i3 + global_index));
        }

        const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
        bool hasN = meshe->HasNormals();
        bool hasTB = meshe->HasTangentsAndBitangents();

        for (size_t j = 0; j < meshe->mNumVertices; j++)
        {
            aiVector3D v_in = meshe->mVertices[j];

            DrawSpace::Core::Vertex v_out(v_in[0], v_in[1], v_in[2]);
			
			if (anims_aspect)
			{				
				v_out.tu[4] = -1.0;
				v_out.tv[4] = -1.0;
				v_out.tw[4] = -1.0;
				v_out.ta[4] = -1.0;
				v_out.tu[5] = 0.0;
				v_out.tv[5] = 0.0;
				v_out.tw[5] = 0.0;
				v_out.ta[5] = 0.0;

				v_out.tu[6] = -1.0;
				v_out.tv[6] = -1.0;
				v_out.tw[6] = -1.0;
				v_out.ta[6] = -1.0;
				v_out.tu[7] = 0.0;
				v_out.tv[7] = 0.0;
				v_out.tw[7] = 0.0;
				v_out.ta[7] = 0.0;				
			}
			
            if(meshe->GetNumUVChannels() > 0)
            {
                aiVector3D texCoord = meshe->HasTextureCoords(0) ? meshe->mTextureCoords[0][j] : Zero3D;            
                v_out.tu[0] = texCoord[0];
                v_out.tv[0] = texCoord[1];
            }
            
            if (hasN && (normales_gen_mode == Meshe::NORMALES_AUTO || normales_gen_mode == Meshe::NORMALES_FROMLOADER ||
						 normales_gen_mode == Meshe::NORMALES_AUTO_SMOOTH || normales_gen_mode == Meshe::NORMALES_FROMLOADER_SMOOTH))
            {
                // model has its own normales, so use it
                v_out.nx = meshe->mNormals[j][0];
                v_out.ny = meshe->mNormals[j][1];
                v_out.nz = meshe->mNormals[j][2];
            }
            
            if (hasTB && (tb_gen_mode == Meshe::TB_AUTO || tb_gen_mode == Meshe::TB_FROMLOADER))
            {
                // model has its own tangent and binormales, so use it

                v_out.tx = meshe->mTangents[j][0];
                v_out.ty = meshe->mTangents[j][1];
                v_out.tz = meshe->mTangents[j][2];

                v_out.bx = meshe->mBitangents[j][0];
                v_out.by = meshe->mBitangents[j][1];
                v_out.bz = meshe->mBitangents[j][2];
            }
            
            p_destination->AddVertex( v_out );
        }

        if(normales_gen_mode==Meshe::NORMALES_COMPUTED || 
			((normales_gen_mode == Meshe::NORMALES_AUTO || normales_gen_mode == Meshe::NORMALES_AUTO_SMOOTH) && !hasN) )
        {
            p_destination->ComputeNormales();
        }

        if(tb_gen_mode == Meshe::TB_COMPUTED || tb_gen_mode == Meshe::TB_AUTO && !hasTB)
        {
            if(meshe->GetNumUVChannels() > 0)
            {
                p_destination->ComputeTBs();
            }
            else
            {
                _DSEXCEPTION( "No UV channel in meshe, cannot compute T,B vectors")
            }
        }

        //////////// transformation des normales
        Matrix n_transf = p_destination->GetNormalesTransf();

        for(long j = 0; j < p_destination->GetVertexListSize(); j++)
        {
            DrawSpace::Core::Vertex vertex;
            p_destination->GetVertex(j, vertex);

            Utils::Vector n(vertex.nx, vertex.ny, vertex.nz, 1.0);
            Utils::Vector nt;

            n_transf.Transform(&n, &nt);

            vertex.nx = nt[0];
            vertex.ny = nt[1];
            vertex.nz = nt[2];

            p_destination->SetVertex(j, vertex);
        }


        ////// ANIMATION BONES ////////////////////////////
		
		if (anims_aspect)
		{
			std::vector<AnimationsAspect::BoneOutput> bones_outputs;
			std::map<dsstring, int> bones_mapping;
					
			for (size_t j = 0; j < meshe->mNumBones; j++)
			{
				aiBone* bone = meshe->mBones[j];

				AnimationsAspect::BoneOutput bone_output;
				bone_output.offset_matrix = ConvertFromAssimpMatrix(bone->mOffsetMatrix);

				bones_outputs.push_back(bone_output);

				bones_mapping[bone->mName.C_Str()] = j;

				for (size_t k = 0; k < bone->mNumWeights; k++)
				{
					float weight = bone->mWeights[k].mWeight;
					int vert_index = bone->mWeights[k].mVertexId;

					DrawSpace::Core::Vertex vertex;
					p_destination->GetVertex(vert_index, vertex);
					
					if (vertex.tu[4] == -1.0)
					{
						vertex.tu[4] = j;       // j = bone index
						vertex.tu[5] = weight;
					}
					else if (vertex.tv[4] == -1.0)
					{
						vertex.tv[4] = j;       // j = bone index
						vertex.tv[5] = weight;

					}					
					else if (vertex.tw[4] == -1.0)
					{
						vertex.tw[4] = j;       // j = bone index
						vertex.tw[5] = weight;
					}
					else if (vertex.ta[4] == -1.0)
					{
						vertex.ta[4] = j;       // j = bone index
						vertex.ta[5] = weight;
					}
					
					else if (vertex.tu[6] == -1.0)
					{
						vertex.tu[6] = j;       // j = bone index
						vertex.tu[7] = weight;
					}
					else if (vertex.tv[6] == -1.0)
					{
						vertex.tv[6] = j;       // j = bone index
						vertex.tv[7] = weight;
					}
					else if (vertex.tw[6] == -1.0)
					{
						vertex.tw[6] = j;       // j = bone index
						vertex.tw[7] = weight;
					}
					else if (vertex.ta[6] == -1.0)
					{
						vertex.ta[6] = j;       // j = bone index
						vertex.ta[7] = weight;
					}
					else
					{
						_DSEXCEPTION("A vertex cannot reference more than 8 bones");
					}
					
					// update vertex
					p_destination->SetVertex(vert_index, vertex);
				}
			}
			
			//////

			for (long j = 0; j < p_destination->GetVertexListSize(); j++)
			{
				DrawSpace::Core::Vertex vertex;
				p_destination->GetVertex(j, vertex);

				float sum = vertex.tu[5] + vertex.tv[5] + vertex.tw[5] + vertex.ta[5] +
							vertex.tu[7] + vertex.tv[7] + vertex.tw[7] + vertex.ta[7];

				if (sum < 0.95)
				{
					_DSEXCEPTION("Vertex weights sum must be 1.0");
				}

				p_destination->SetVertex(j, vertex);
			}

			//////
			anims_aspect->GetComponent<std::vector<AnimationsAspect::BoneOutput>>("bones_outputs")->getPurpose() = bones_outputs;
			anims_aspect->GetComponent<std::map<dsstring, int>>("bones_mapping")->getPurpose() = bones_mapping;

			RenderingAspect* rendering_aspect = p_entity->GetAspect <RenderingAspect>();
			if (!rendering_aspect)
			{
				_DSEXCEPTION("an entity with AnimationsAspect must also have a RenderingAspect");
			}

			ComponentList<MesheRenderingAspectImpl::PassSlot> passes;
			rendering_aspect->GetComponentsByType<MesheRenderingAspectImpl::PassSlot>(passes);

			for (auto e : passes)
			{
				RenderingNode* rnode = e->getPurpose().GetRenderingNode();

				rnode->AddShaderArrayParameter(0, AnimationsSystem::bonesBuffer0Id, AnimationsSystem::bonesBuffer0StartReg);
				rnode->AddShaderArrayParameter(0, AnimationsSystem::bonesBuffer1Id, AnimationsSystem::bonesBuffer1StartReg);
			}
		}

		//////////////////////////////////

        global_index += meshe->mNumVertices;
    }
}

dsstring ResourcesSystem::compute_textures_final_path(const dsstring& p_path) const
{
    dsstring final_path = m_textures_rootpath + "/";
    final_path += p_path;
    return final_path;
}

dsstring ResourcesSystem::compute_shaders_final_path(const dsstring& p_path) const
{
    dsstring final_path = m_shaders_rootpath + "/";
    if (m_addshaderspath)
    {
        Renderer* renderer = SingletonPlugin<Renderer>::GetInstance()->m_interface;
        dsstring m_shader_descr;
        renderer->GetShadersDescr(m_shader_descr);
        final_path += m_shader_descr + "/";
    }
    final_path += p_path;

    return final_path;
}

dsstring ResourcesSystem::compute_meshes_final_path(const dsstring& p_path) const
{
    dsstring final_path = m_meshes_rootpath + "/";
    final_path += p_path;
    return final_path;
}

void ResourcesSystem::ReleaseAssets(void)
{
    for(auto& e : m_texturesCache)
    {
        _DRAWSPACE_DELETE_N_(e.second.data);
    }
    for (auto& e : m_shadersCache)
    {
        //_DRAWSPACE_DELETE_(e.second.data);
		_DRAWSPACE_DELETE_N_(e.second.data);
    }
    for (auto& e : m_meshesCache)
    {
        e.second.first->FreeScene();
        //_DRAWSPACE_DELETE_(e.second.first);
		delete e.second.first;
    }

	// reset all
    m_shadersCache.clear();
    m_meshesCache.clear();
    m_texturesCache.clear();
	m_asset_loading_state.clear();
}

void ResourcesSystem::ReleaseShaderAsset(const dsstring& p_asset)
{
    dsstring final_asset_path = compute_shaders_final_path(p_asset);

    if( m_shadersCache.count(final_asset_path) )
    {
        _DRAWSPACE_DELETE_N_(m_shadersCache.at(final_asset_path).data);
        m_shadersCache.erase(final_asset_path);
    }
}

void ResourcesSystem::LoadTexture(DrawSpace::Core::Texture* p_texture)
{
    dsstring asset_path;
    p_texture->GetBasePath(asset_path);
    dsstring final_asset_path = compute_textures_final_path(asset_path);

	updateAssetFromCache<Texture>(p_texture, m_texturesCache, final_asset_path);
}

void ResourcesSystem::LoadShader(Core::Shader* p_shader, int p_shader_type)
{
	dsstring asset_path;
	p_shader->GetBasePath(asset_path);
	dsstring final_asset_path = compute_shaders_final_path(asset_path);
	dsstring final_asset_dir = compute_shaders_final_path("");

	if (p_shader->IsCompiled())
	{
		updateAssetFromCache<Shader>(p_shader, m_shadersCache, final_asset_path);
	}
	else
	{
		manage_shader_in_bccache(p_shader, asset_path, final_asset_path, final_asset_dir, p_shader_type);
	}
}

void ResourcesSystem::check_bc_cache_presence(void) const
{
	if (FileSystem::Exists(bcCacheName))
	{
		if (!FileSystem::IsDirectory(bcCacheName))
		{
			_DSEXCEPTION("unexpected bc_cache element");
		}
	}
	else
	{
		notify_event(SHADERCACHE_CREATION, "");
		FileSystem::CreateDirectory(bcCacheName);
	}
}

void ResourcesSystem::update_bc_md5file(const dsstring& p_path, const dsstring& p_hash)
{
	FileSystem::WriteFile(p_path + dsstring("\\") + bcMd5FileName, (void*)p_hash.c_str(), p_hash.length());
}

void ResourcesSystem::update_bc_codefile(const dsstring& p_path, void* p_bc, int p_size)
{
	FileSystem::WriteFile(p_path + dsstring("\\") + bcCodeFileName, p_bc, p_size);
}

void ResourcesSystem::manage_shader_in_bccache(Shader* p_shader, const dsstring& p_asset_path, const dsstring& p_final_asset_path, const dsstring& p_final_asset_dir, int p_shader_type)
{
	// shader bytecode buffer infos...
	long bc_length;
	void* bc;

	// load shader hlsl source text
	long text_size;
	void* text;

	text = Utils::File::LoadAndAllocBinaryFile(p_final_asset_path, &text_size);
	if (!text)
	{
		_DSEXCEPTION("ResourcesSystem : failed to load " + p_final_asset_path);
	}
	//compute source text checksum
	MD5 md5;
	dsstring hash_shader{ md5.digestMemory((BYTE*)text, text_size) };

	// shader key id
	std::vector<dsstring> items;
	SplitString(p_asset_path, items, '.');
	dsstring shader_id{ items[0] };

	dsstring path{ bcCacheName + dsstring("\\") + shader_id.c_str() };

	if (FileSystem::Exists(path))
	{
		if (!FileSystem::IsDirectory(path))
		{
			_DSEXCEPTION("ResourcesSystem : expecting directory for bytecode " + shader_id);
		}

		// check bc code presence...
		if (!FileSystem::Exists(path + dsstring("\\") + bcCodeFileName))
		{
			_DSEXCEPTION("ResourcesSystem : cannot find bytecode for " + shader_id);
		}

		// check bc md5 presence...
		if (!FileSystem::Exists(path + dsstring("\\") + bcMd5FileName))
		{
			_DSEXCEPTION("ResourcesSystem : cannot find md5 for " + shader_id);
		}

		///////////////////////////////////
		long md5filesize;
		unsigned char* md5Buf = { static_cast<unsigned char*>( FileSystem::LoadAndAllocFile(path + dsstring("\\") + bcMd5FileName, &md5filesize) ) };

		dsstring stored_md5((char*)md5Buf, md5filesize);

		if (stored_md5 == hash_shader)
		{
			if (m_shadersCache.find(p_final_asset_path) == m_shadersCache.end())
			{
				// OK, can load bc.code file or get from m_shadersCache

				notify_event(BLOB_LOAD, p_final_asset_path);

				bc = FileSystem::LoadAndAllocFile(path + dsstring("\\") + bcCodeFileName, &bc_length);
				if (NULL == bc)
				{
					_DSEXCEPTION(dsstring("unexpected error while reading code file : ") + shader_id);
				}

				notify_event(BLOB_LOADED, p_final_asset_path);

				Blob blob{ bc, bc_length };
				m_shadersCache[p_final_asset_path] = blob;
			
			}

			// update shader & m_shadersCache with bytecode

			p_shader->SetData(m_shadersCache.at(p_final_asset_path).data, m_shadersCache.at(p_final_asset_path).size);
		}
		else
		{
			// crc changed, update all,

			notify_event(SHADER_COMPILATION, p_final_asset_path);

			// try to compile...
			void* bytecode_handle;

			bool comp_status{ m_renderer->CreateShaderBytes((char*)text, text_size, p_shader_type, p_asset_path, p_final_asset_dir, &bytecode_handle) };
			if (!comp_status)
			{
				dsstring err_compil{ m_renderer->GetShaderCompilationError(bytecode_handle) };
				_DSEXCEPTION("ResourcesSystem : failed to compile " + p_final_asset_path + dsstring(" : ") + err_compil);
			}

			bc_length = m_renderer->GetShaderBytesLength(bytecode_handle);

			bc = (void*)_DRAWSPACE_NEW_EXPLICIT_SIZE_WITH_COMMENT(unsigned char, unsigned char[bc_length], bc_length, p_final_asset_dir);
			memcpy(bc, m_renderer->GetShaderBytes(bytecode_handle), bc_length);

			m_renderer->ReleaseShaderBytes(bytecode_handle);

			update_bc_md5file(path, hash_shader);
			update_bc_codefile(path, bc, bc_length);

			notify_event(SHADER_COMPILED, p_final_asset_path);

			// update shader & m_shadersCache with bytecode

			Blob blob{ bc, bc_length };
			m_shadersCache[p_final_asset_path] = blob;

			p_shader->SetData(bc, bc_length);

		}

		_DRAWSPACE_DELETE_N_(md5Buf);
	}
	else
	{
		// shader entry does not exists, create all...

		notify_event(SHADER_COMPILATION, p_final_asset_path);

		// try to compile...
		void* bytecode_handle;

		bool comp_status{ m_renderer->CreateShaderBytes((char*)text, text_size, p_shader_type, p_asset_path, p_final_asset_dir, &bytecode_handle) };
		if (!comp_status)
		{
			dsstring err_compil{ m_renderer->GetShaderCompilationError(bytecode_handle) };
			_DSEXCEPTION("ResourcesSystem : failed to compile " + p_final_asset_path + dsstring(" : ") + err_compil);
		}

		bc_length = m_renderer->GetShaderBytesLength(bytecode_handle);

		bc = (void*)_DRAWSPACE_NEW_EXPLICIT_SIZE_WITH_COMMENT(unsigned char, unsigned char[bc_length], bc_length, p_final_asset_dir);
		memcpy(bc, m_renderer->GetShaderBytes(bytecode_handle), bc_length);

		m_renderer->ReleaseShaderBytes(bytecode_handle);

		FileSystem::CreateDirectory(path);

		update_bc_md5file(path, hash_shader);
		update_bc_codefile(path, bc, bc_length);

		notify_event(SHADER_COMPILED, p_final_asset_path);

		// update shader & m_shadersCache with bytecode

		Blob blob{ bc, bc_length };
		m_shadersCache[p_final_asset_path] = blob;

		p_shader->SetData(bc, bc_length);
	}

	_DRAWSPACE_DELETE_N_(text);
	p_shader->SetCompilationFlag(true); //shader now contains compiled shader
}

void ResourcesSystem::check_all_assets_loaded(void)
{
	if (0 == m_asset_loading_state.size())
	{
		return;
	}

	// check if all is loaded
	bool all_asset_loaded{ true };

	for (auto& e : m_asset_loading_state)
	{
		const auto id{ e.first };
		const bool status{ e.second };

		if (!status)
		{
			all_asset_loaded = false;
		}
	}

	if (all_asset_loaded)
	{
		// send event -> all asset properly loaded

		notify_event(ALL_ASSETS_LOADED, "");

		// reset flag

		m_asset_loading_state.clear();
	}
}