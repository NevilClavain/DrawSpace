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

DrawSpace::Logger::Sink rs_logger("ResourcesSystem", DrawSpace::Logger::Configuration::GetInstance());

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

ResourcesSystem::ResourcesSystem(void)
{
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
                    Meshe::NormalesGenerationMode normales_gen_mode = target_meshe->GetNGenerationMode();
                    Meshe::TangentBinormalesGenerationMode tb_gen_mode = target_meshe->GetTBGenerationMode();

                    void* data;
                    long size;
                    data = Utils::File::LoadAndAllocBinaryFile(final_asset_path, &size);
                    if (!data)
                    {
                        _DSEXCEPTION("ResourcesSystem : failed to load " + final_asset_path);
                    }

                    Assimp::Importer* importer = new Assimp::Importer();

                    unsigned int flags = aiProcess_Triangulate |
                        aiProcess_JoinIdenticalVertices |
                        aiProcess_FlipUVs |
                        aiProcess_SortByPType;

                    if(Meshe::NORMALES_AUTO == normales_gen_mode || Meshe::NORMALES_FROMLOADER == normales_gen_mode)
                    {
                        flags |= aiProcess_GenSmoothNormals;
                    }

                    if (Meshe::TB_AUTO == tb_gen_mode || Meshe::TB_FROMLOADER == tb_gen_mode)
                    {
                        flags |= aiProcess_CalcTangentSpace;
                    }

                    const aiScene* scene = importer->ReadFileFromMemory(data, size,flags);
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
                            _DSDEBUG(rs_logger, dsstring("************************************SCENE INFOS***********************************"));
                            _DSDEBUG(rs_logger, dsstring("resources = ") << final_asset_path);
                            _DSDEBUG(rs_logger, dsstring("scene HasMeshes ") << scene->HasMeshes() );
                            _DSDEBUG(rs_logger, dsstring("scene num Meshes ") << scene->mNumMeshes);

                            _DSDEBUG(rs_logger, dsstring("scene HasTextures ") << scene->HasTextures());
                            _DSDEBUG(rs_logger, dsstring("scene num Textures ") << scene->mNumTextures);

                            _DSDEBUG(rs_logger, dsstring("scene HasMaterials ") << scene->HasMaterials());
                            _DSDEBUG(rs_logger, dsstring("scene num Materials ") << scene->mNumMaterials);

                            _DSDEBUG(rs_logger, dsstring("scene HasLights ") << scene->HasLights());
                            _DSDEBUG(rs_logger, dsstring("scene num Lights ") << scene->mNumLights);

                            _DSDEBUG(rs_logger, dsstring("scene HasCameras ") << scene->HasCameras());
                            _DSDEBUG(rs_logger, dsstring("scene num Cameras ") << scene->mNumCameras);

                            _DSDEBUG(rs_logger, dsstring("scene HasAnimations ") << scene->HasAnimations());
                            _DSDEBUG(rs_logger, dsstring("scene num Animations ") << scene->mNumAnimations);

                            aiNode* meshe_node = root->FindNode(meshe_id.c_str());
                            if (meshe_node)
                            {
                                build_meshe(meshe_id, meshe_node, meshes, target_meshe);
                                m_meshesCache[final_asset_path] = std::make_pair(importer, scene );

                            }
                            else
                            {
                                _DSEXCEPTION("cannot locate meshe objet " + meshe_id);
                            }
                            _DSDEBUG(rs_logger, dsstring("************************************SCENE INFOS END*******************************"));
                        }
                        else
                        {
                            _DSEXCEPTION("No root found in assimp scene description");
                        }

                        _DRAWSPACE_DELETE_N_(data);
                    }
                    else
                    {
                        _DSEXCEPTION("Assimp importer cannot load meshe " + final_asset_path);
                    }
                }
                else
                {
                    std::pair<Assimp::Importer*, const aiScene*> entry = m_meshesCache.at(final_asset_path);

                    aiMesh** meshes = entry.second->mMeshes;
                    aiNode* root = entry.second->mRootNode;

                    aiNode* meshe_node = root->FindNode(meshe_id.c_str());

                    if(meshe_node)
                    {
                        build_meshe(meshe_id, meshe_node, meshes, target_meshe);
                    }
                    else
                    {
                        _DSEXCEPTION("cannot locate meshe objet " + meshe_id);
                    }
                }

                loaded = true;            
            }
        }   
    }
}

void ResourcesSystem::build_meshe(const dsstring& p_id, aiNode* p_ai_node, aiMesh** p_meshes, Core::Meshe* p_destination)
{
    dsstring name = p_ai_node->mName.C_Str();

    Meshe::NormalesGenerationMode normales_gen_mode = p_destination->GetNGenerationMode();
    Meshe::TangentBinormalesGenerationMode tb_gen_mode = p_destination->GetTBGenerationMode();


    unsigned int nb_meshes = p_ai_node->mNumMeshes;
    int global_index = 0;

    unsigned int* indexes = p_ai_node->mMeshes;
    for( unsigned int i = 0; i < nb_meshes; i++ )
    {
        aiMesh* meshe = p_meshes[indexes[i]];

        _DSDEBUG(rs_logger, dsstring("************************************MESHE INFOS***********************************"));
        _DSDEBUG(rs_logger, dsstring("meshe id = ") + p_id);
        _DSDEBUG(rs_logger, dsstring("name = ") << dsstring( meshe->mName.C_Str() ));
        _DSDEBUG(rs_logger, dsstring("meshe HasPositions ") << meshe->HasPositions());
        _DSDEBUG(rs_logger, dsstring("meshe HasFaces ") << meshe->HasFaces());
        _DSDEBUG(rs_logger, dsstring("meshe HasNormals ") << meshe->HasNormals());        
        _DSDEBUG(rs_logger, dsstring("meshe HasTangentsAndBitangents ") << meshe->HasTangentsAndBitangents());
        _DSDEBUG(rs_logger, dsstring("meshe HasBones ") << meshe->HasBones());
        _DSDEBUG(rs_logger, dsstring("meshe NumUVChannels ") << meshe->GetNumUVChannels());

        _DSDEBUG(rs_logger, dsstring("************************************MESHE INFOS END*******************************"));
        
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

            if(meshe->GetNumUVChannels() > 0)
            {
                aiVector3D texCoord = meshe->HasTextureCoords(0) ? meshe->mTextureCoords[0][j] : Zero3D;            
                v_out.tu[0] = texCoord[0];
                v_out.tv[0] = texCoord[1];
            }
            
            if (hasN && (normales_gen_mode == Meshe::NORMALES_AUTO || normales_gen_mode == Meshe::TB_FROMLOADER))
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

        if(normales_gen_mode==Meshe::NORMALES_COMPUTED || (normales_gen_mode == Meshe::NORMALES_AUTO && !hasN) )
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
        _DRAWSPACE_DELETE_(e.second.data);
    }
    for (auto& e : m_meshesCache)
    {
        e.second.first->FreeScene();
        _DRAWSPACE_DELETE_(e.second.first);
    }

    m_shadersCache.clear();
    m_meshesCache.clear();
    m_texturesCache.clear();
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

void ResourcesSystem::LoadShader(Core::Shader* p_shader)
{
    dsstring asset_path;
    p_shader->GetBasePath(asset_path);
    dsstring final_asset_path = compute_shaders_final_path(asset_path);
    updateAssetFromCache<Shader>(p_shader, m_shadersCache, final_asset_path);
}