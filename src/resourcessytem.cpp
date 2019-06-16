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

#include "resourcessystem.h"
#include "resourcesaspect.h"

#include "texture.h"
#include "shader.h"
#include "meshe.h"

#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "renderer.h"
#include "plugin.h"
#include "file.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::EntityGraph;
using namespace DrawSpace::Systems;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface;

dsstring ResourcesSystem::m_textures_rootpath = ".";
dsstring ResourcesSystem::m_meshes_rootpath = ".";
dsstring ResourcesSystem::m_shaders_rootpath = ".";
bool ResourcesSystem::m_addshaderspath = false;

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
    p_entitygraph->AcceptSystemRootToLeaf( this );
}

void ResourcesSystem::VisitEntity(Entity* p_parent, Entity* p_entity)
{
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

                updateAssetFromCache<Texture>(std::get<0>(e->getPurpose()), m_texturesCache, final_asset_path);
                loaded = true;
            }
        }

        ComponentList<std::tuple<Shader*, bool>> shaders_assets;
        resources_aspect->GetComponentsByType<std::tuple<Shader*, bool>>(shaders_assets);

        for (auto& e : shaders_assets)
        {
            bool& loaded = std::get<1>(e->getPurpose());
            if (!loaded)
            {
                dsstring asset_path;
                std::get<0>(e->getPurpose())->GetBasePath(asset_path);
                dsstring final_asset_path = compute_shaders_final_path(asset_path);

                updateAssetFromCache<Shader>(std::get<0>(e->getPurpose()), m_shadersCache, final_asset_path);
                loaded = true;
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
                

                if( m_meshesCache.find(final_asset_path) == m_meshesCache.end() )
                {
                    const aiScene* scene = m_importer.ReadFile(final_asset_path,
                        aiProcess_CalcTangentSpace |
                        aiProcess_Triangulate |
                        aiProcess_JoinIdenticalVertices |
                        aiProcess_FlipUVs |
                        aiProcess_SortByPType);

                    if (scene)
                    {
                        if( !scene->HasMeshes() )
                        {
                            _DSEXCEPTION("No meshes in file : " + final_asset_path);
                        }
                        aiMesh** meshes = scene->mMeshes;

                        aiNode* root = scene->mRootNode;
                        if (root)
                        {
                            aiNode* meshe_node = root->FindNode(meshe_id.c_str());
                            if (meshe_node)
                            {
                                build_meshe(meshe_node, meshes, target_meshe);
                                m_meshesCache[final_asset_path] = scene;
                            }
                            else
                            {
                                _DSEXCEPTION("cannot locate meshe objet " + meshe_id);
                            }
                        }
                        else
                        {
                            _DSEXCEPTION("No root found in assimp scene description");
                        }
                    }
                    else
                    {
                        _DSEXCEPTION("Assimp importer cannot load meshe " + final_asset_path);
                    }
                }
                else
                {
                    const aiScene* scene = m_meshesCache[final_asset_path];
                    aiMesh** meshes = scene->mMeshes;
                    aiNode* root = scene->mRootNode;
                    aiNode* meshe_node = root->FindNode(meshe_id.c_str());                
                    build_meshe(meshe_node, meshes, target_meshe);
                }                             
                loaded = true;            
            }
        }
    }
}

void ResourcesSystem::build_meshe(aiNode* p_ai_node, aiMesh** p_meshes, Core::Meshe* p_destination)
{
    dsstring name = p_ai_node->mName.C_Str();

    unsigned int nb_meshes = p_ai_node->mNumMeshes;
    int global_index = 0;

    unsigned int* indexes = p_ai_node->mMeshes;
    for( unsigned int i = 0; i < nb_meshes; i++ )
    {
        aiMesh* meshe = p_meshes[indexes[i]];
        
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
        for (size_t j = 0; j < meshe->mNumVertices; j++)
        {
            aiVector3D v_in = meshe->mVertices[j];

            DrawSpace::Core::Vertex v_out(v_in[0], v_in[1], v_in[2]);
            aiVector3D texCoord = meshe->HasTextureCoords(0) ? meshe->mTextureCoords[0][j] : Zero3D;
            
            v_out.tu[0] = texCoord[0];
            v_out.tv[0] = texCoord[1];

            p_destination->AddVertex( v_out );
        }

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
        _DRAWSPACE_DELETE_(e.second.data);
    }
    for (auto& e : m_shadersCache)
    {
        _DRAWSPACE_DELETE_(e.second.data);
    }

    m_importer.FreeScene();
}
