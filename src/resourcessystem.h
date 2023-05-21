/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags


#include "systems.h"
#include "entitynodegraph.h"
#include "animationsaspect.h"
#include "resourcesaspect.h"
#include "renderingaspect.h"
#include "task.h"
#include "file.h"
#include "filesystem.h"
#include "componentcontainer.h"
#include "runner.h"

#include "renderer.h"

#include "animationssystem.h"

#include "primitives.h"


struct aiNode;
struct aiScene;
struct aiMesh;


#include <sstream>
#include <algorithm>
#include <iterator>


#define CHECK_PHYSFS( __call__ ) \
{ \
	int status { __call__ }; \
	if (status == 0) \
	{ \
		dsstring perr{ PHYSFS_getLastError() }; \
		_DSEXCEPTION(perr); \
	} \
}

namespace DrawSpace
{
namespace Core
{
class   Meshe;
class   Texture;
class   Shader;
}



namespace Systems
{

//fwd decl
class RunnerSystem;

class ResourcesSystem : public Interface::System
{
public:

    enum class ResourceEvent
    {
        BLOB_LOAD,
        BLOB_LOADED,
        SHADERCACHE_CREATION,
        SHADER_COMPILATION,
        SHADER_COMPILED,
        ALL_ASSETS_LOADED,
    };

    using ResourceEventHandler = DrawSpace::Core::BaseCallback3<void, ResourceEvent, const dsstring&, const dsstring&>;

    struct MesheCacheEntry
    {
        MesheCacheEntry(void) {};
        MesheCacheEntry(Assimp::Importer* p_importer, const aiScene* p_scene) :
        m_assimp_importer(p_importer),
        m_assimp_scene(p_scene)
        {};

        Assimp::Importer*   m_assimp_importer{ nullptr };
        const aiScene*      m_assimp_scene{ nullptr };

        struct MesheData
        {
            std::vector<Core::Vertex>                               m_vertices;
            std::vector<Core::TrianglePrimitive<unsigned int>>      m_triangles;
        };

        std::map<dsstring, MesheData> m_meshes_data;
    };

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

    static dsstring                                                     m_textures_rootpath;

    static dsstring                                                     m_shaders_rootpath;
    static bool                                                         m_addshaderspath;

    static dsstring                                                     m_meshes_rootpath;

    std::map<dsstring, Blob>                                            m_texturesCache;
    std::map<dsstring, Blob>                                            m_shadersCache;
    std::map<dsstring, MesheCacheEntry>                                 m_meshesCache;


    DrawSpace::Interface::Renderer*                                     m_renderer{ nullptr };
    RunnerSystem&                                                       m_runner_system;

    bool                                                                m_all_asset_loaded{ false };

    bool                                                                m_active{ false };

    dsstring                                                            m_current_context;
 
    void run(EntityGraph::EntityNodeGraph* p_entitygraph);

    dsstring compute_textures_final_path(const dsstring& p_path) const;
    dsstring compute_shaders_final_path(const dsstring& p_path) const;
    dsstring compute_meshes_final_path(const dsstring& p_path) const;

    template<typename T>
    void updateAssetFromCache(T* p_asset, std::map<dsstring, Blob>& p_blobs, dsstring p_final_asset_path) const
    {
        if (p_blobs.find(p_final_asset_path) == p_blobs.end())
        {
            Blob blob;
            long size;
            void* data;

            // load it
            data = Utils::File::LoadAndAllocBinaryFile(p_final_asset_path, &size);
            if (!data)
            {
                _DSEXCEPTION("ResourcesSystem : failed to load " + p_final_asset_path);
            }
            blob.data = data;
            blob.size = size;

            p_blobs[p_final_asset_path] = blob;
        }

        // update asset
        p_asset->SetData(p_blobs.at(p_final_asset_path).data, p_blobs.at(p_final_asset_path).size);
    }
    
	void manage_shader_in_bccache(Core::Shader* p_shader, const dsstring& p_asset_path, const dsstring& p_final_asset_path, const dsstring& p_final_asset_dir, int p_shader_type);
	void check_bc_cache_presence(void) const;
	void update_bc_md5file(const dsstring& p_path, const dsstring& p_hash);
	void update_bc_codefile(const dsstring& p_path, void* p_bc, int p_size);
    void notify_event(ResourceEvent p_event, const dsstring& p_path) const;


public:

    ResourcesSystem(RunnerSystem& p_runner);

    void Activate(const dsstring& p_context);
    void Deactivate(void);

    void RegisterEventHandler(ResourceEventHandler* p_handler);
    void UnregisterEventHandler(ResourceEventHandler* p_handler);

    dsstring GetSystemId(void) const { return "ResourcesSystem"; };

    void VisitEntity(Core::Entity* p_parent, Core::Entity* p_entity);

    void ReleaseAssets( void );
    void ReleaseShaderAsset( const dsstring& p_asset);

    void NotifyEvent(ResourceEvent p_event, const dsstring& p_path) const;

	static Utils::Matrix ResourcesSystem::ConvertFromAssimpMatrix(const aiMatrix4x4& p_in_mat);

    //////// direct API, useful for non-entity client //////

    void LoadTexture(Core::Texture* p_texture);
    void LoadShader(Core::Shader* p_shader, int p_shader_type);

    ////////////////////////////////////////////////////////

    static void SetTexturesRootPath(const dsstring& p_path);
    static void EnableShadersDescrInFinalPath(bool p_state);
    static void SetShadersRootPath(const dsstring& p_path);
    static void SetMeshesRootPath(const dsstring& p_path);

    // recursive
    static void DumpAssimpSceneNode(aiNode* p_ai_node, int depth, DrawSpace::Logger::Sink* p_rs_logger);
    static void DumpMeshe(aiNode* p_ai_node, aiMesh** p_meshes, DrawSpace::Logger::Sink* p_rs_logger);
};
}
}

