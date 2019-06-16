#pragma once

/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2019
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

#include <assimp/Importer.hpp>      // C++ importer interface

#include "systems.h"
#include "entitynodegraph.h"

#include "resourcesiosystem.h"

struct aiNode;
struct aiScene;
struct aiMesh;

namespace DrawSpace
{
namespace Core
{
class Meshe;
}

namespace Systems
{
class ResourcesSystem : public Interface::System
{
protected:

    using Blob = struct
    {
        void*   data;
        int     size;
    };

    static dsstring                         m_textures_rootpath;

    static dsstring                         m_shaders_rootpath;
    static bool                             m_addshaderspath;

    static dsstring                         m_meshes_rootpath;

    std::map<dsstring, Blob>                m_texturesCache;
    std::map<dsstring, Blob>                m_shadersCache;
    std::map<dsstring, const aiScene*>      m_meshesCache;

    ResourcesIOSystem                       m_iosystem;
    Assimp::Importer                        m_importer;

    void run(EntityGraph::EntityNodeGraph* p_entitygraph);

    dsstring compute_textures_final_path(const dsstring& p_path) const;
    dsstring compute_shaders_final_path(const dsstring& p_path) const;
    dsstring compute_meshes_final_path(const dsstring& p_path) const;

    // recursive
    void build_meshe(aiNode* p_ai_node, aiMesh** p_meshes, Core::Meshe* p_destination);

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


public:

    ResourcesSystem( void );

    dsstring GetSystemId(void) const { return "ResourcesSystem"; };

    void VisitEntity(Core::Entity* p_parent, Core::Entity* p_entity);

    void ReleaseAssets( void );


    static void SetTexturesRootPath(const dsstring& p_path);
    static void EnableShadersDescrInFinalPath(bool p_state);
    static void SetShadersRootPath(const dsstring& p_path);
    static void SetMeshesRootPath(const dsstring& p_path);



};
}
}

