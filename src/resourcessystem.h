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

#pragma once

#include <assimp/Importer.hpp>      // C++ importer interface

#include "systems.h"
#include "entitynodegraph.h"
#include "animationsaspect.h"
#include "resourcesaspect.h"
#include "task.h"
#include "file.h"
#include "runner.h"

struct aiNode;
struct aiScene;
struct aiMesh;


#include <sstream>
#include <algorithm>
#include <iterator>


namespace DrawSpace
{
namespace Core
{
class Meshe;
class Texture;
class Shader;
}

namespace Interface
{
class Renderer;
}

namespace Systems
{
class ResourcesSystem : public Interface::System
{
public:

    enum ResourceEvent
    {
        BLOB_LOAD,
        BLOB_LOADED,
        ASSET_SETLOADEDBLOB,
        SHADERCACHE_CREATION,
        SHADER_COMPILATION,
        SHADER_COMPILED,
        ALL_ASSETS_LOADED,
    };

    using ResourceEventHandler = DrawSpace::Core::BaseCallback2<void, ResourceEvent, const dsstring&>;

private:
    std::set<ResourceEventHandler*>					                    m_evt_handlers;

	static const dsstring bcCacheName;
	static const dsstring bcMd5FileName;
	static const dsstring bcCodeFileName;

    using Blob = struct
    {
        void*   data;
        int     size;
    };

    struct LoadBinaryFileTask : public Interface::ITask
    {
    private:
        // execution data
        long        m_size{ 0 };
        void*       m_data{ nullptr };
        bool        m_failure;

        dsstring    m_final_asset_path;
        
    public:

        LoadBinaryFileTask() :            
        ITask("LoadBinaryFileTask")
        {
        }

        void Execute(void)
        {
            long size;
            void* data = Utils::File::LoadAndAllocBinaryFile(m_final_asset_path, &size);
            if (data)
            {
                m_data = data;
                m_size = size;
                m_failure = false;
            }
            else
            {
                m_failure = true;
            }
        }
        
        inline void SetId(const dsstring& p_id)
        {
            m_id = p_id;
        }

        inline void SetFinalAssetPath(const dsstring& p_final_asset_path)
        {
            m_final_asset_path = p_final_asset_path;
        }

        inline bool Failed(void) const
        {
            return m_failure;
        }

        inline long GetSize(void) const
        {
            return m_size;
        }

        inline void* GetData(void) const
        {
            return m_data;
        }
    };


    static dsstring                                                     m_textures_rootpath;

    static dsstring                                                     m_shaders_rootpath;
    static bool                                                         m_addshaderspath;

    static dsstring                                                     m_meshes_rootpath;

    std::map<dsstring, Blob>                                            m_texturesCache;
    std::map<dsstring, Blob>                                            m_shadersCache;
    std::map<dsstring, std::pair<Assimp::Importer*, const aiScene*>>    m_meshesCache;

    bool                                                                m_new_asset;
    std::map<dsstring, bool>                                            m_asset_loading_state;

	DrawSpace::Interface::Renderer*										m_renderer;

    std::map<dsstring, LoadBinaryFileTask>                              m_currenttasks;
    std::set<dsstring>                                                  m_finishedtasks;

    void run(EntityGraph::EntityNodeGraph* p_entitygraph);

    dsstring compute_textures_final_path(const dsstring& p_path) const;
    dsstring compute_shaders_final_path(const dsstring& p_path) const;
    dsstring compute_meshes_final_path(const dsstring& p_path) const;

    void build_meshe(Core::Entity* p_entity, const dsstring& p_id, aiNode* p_ai_node, aiMesh** p_meshes, Core::Meshe* p_destination);

	// recusive
	void load_scene_nodes_hierachy(aiNode* p_ai_node, int depth, std::map<dsstring, Aspect::AnimationsAspect::Node>& p_node_table);

    // recursive
    void dump_assimp_scene_node(aiNode* p_ai_node, int depth, Aspect::ResourcesAspect::MeshesFileDescription& p_description, std::vector<dsstring>& p_meshes_node_owner_names);

    template<typename T>
    void launchAssetLoadingInRunner(const dsstring& p_final_asset_path)
    {
        const dsstring task_id{ p_final_asset_path };

        LoadBinaryFileTask task;
        task.SetId(task_id);
        task.SetFinalAssetPath(p_final_asset_path);

        m_currenttasks[p_final_asset_path] = task;

        Threading::Runner* runner{ Threading::Runner::GetInstance() };
        notify_event(BLOB_LOAD, p_final_asset_path);
        runner->m_mailbox_in.Push<ITask*>(&m_currenttasks.at(p_final_asset_path));


        /*
        if (p_blobs.find(p_final_asset_path) == p_blobs.end())
        {
            Blob blob;
            long size;
            void* data;

            // load it
            notify_event(BLOB_LOAD, p_final_asset_path);
            data = Utils::File::LoadAndAllocBinaryFile(p_final_asset_path, &size);
            if (!data)
            {
                _DSEXCEPTION("ResourcesSystem : failed to load " + p_final_asset_path);
            }
            blob.data = data;
            blob.size = size;
            notify_event(BLOB_LOADED, p_final_asset_path);

            p_blobs[p_final_asset_path] = blob;
            notify_event(ASSET_SETLOADEDBLOB, p_final_asset_path);
        }

        // update asset
        p_asset->SetData(p_blobs.at(p_final_asset_path).data, p_blobs.at(p_final_asset_path).size);
        */
    }

	void load_animations(const aiScene* p_scene, Aspect::AnimationsAspect* p_anims_aspect);

	void manage_shader_in_bccache(Core::Shader* p_shader, const dsstring& p_asset_path, const dsstring& p_final_asset_path, const dsstring& p_final_asset_dir, int p_shader_type);

	void check_bc_cache_presence(void) const;

	void update_bc_md5file(const dsstring& p_hash);

	void update_bc_codefile(void* p_bc, int p_size);

    void notify_event(ResourceEvent p_event, const dsstring& p_path) const;

    void check_all_assets_loaded(void);

    void check_finished_tasks(void);

public:

    ResourcesSystem( void );

    void RegisterEventHandler(ResourceEventHandler* p_handler);
    void UnregisterEventHandler(ResourceEventHandler* p_handler);

    dsstring GetSystemId(void) const { return "ResourcesSystem"; };

    void VisitEntity(Core::Entity* p_parent, Core::Entity* p_entity);

    void ReleaseAssets( void );
    void ReleaseShaderAsset( const dsstring& p_asset);

	static Utils::Matrix ResourcesSystem::ConvertFromAssimpMatrix(const aiMatrix4x4& p_in_mat);

    //////// direct API, useful for non-entity client //////

    void LoadTexture(Core::Texture* p_texture);
    void LoadShader(Core::Shader* p_shader, int p_shader_type);

    ////////////////////////////////////////////////////////

    static void SetTexturesRootPath(const dsstring& p_path);
    static void EnableShadersDescrInFinalPath(bool p_state);
    static void SetShadersRootPath(const dsstring& p_path);
    static void SetMeshesRootPath(const dsstring& p_path);


};
}
}

