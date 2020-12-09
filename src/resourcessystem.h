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

#include "mesherenderingaspectimpl.h"


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
class Meshe;
class Texture;
class Shader;
}



namespace Systems
{

//fwd decl
class RunnerSystem;

class ResourcesSystem : public Interface::System
{
public:

    enum ResourceEvent
    {
        BLOB_LOAD,
        BLOB_LOADED,
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

    struct LoadFileTask : public Interface::ITask
    {
    private:
        // execution data
        long        m_size{ 0 };
        void*       m_data{ nullptr };
        bool        m_failure;

        dsstring    m_final_asset_path;
        
    public:

        LoadFileTask() : ITask("", "")
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


    struct LoadFileToAssimpTask : public Interface::ITask
    {
    private:

        bool                                m_failure;
        dsstring                            m_err_descr;

        dsstring                            m_final_asset_path;

        DrawSpace::Core::Meshe*             m_target_meshe;
        const aiScene*                      m_scene{ nullptr };
        Assimp::Importer*                   m_importer{ nullptr };

    public:

        LoadFileToAssimpTask() : ITask("LOADFILETOASSIMP", "")
        {
        }

        void Execute(void)
        {
            long size;
            void* data = Utils::File::LoadAndAllocBinaryFile(m_final_asset_path, &size);
            if (data)
            {
                m_failure = false;

                Assimp::Importer* importer = new Assimp::Importer();
                
                unsigned int flags = aiProcess_Triangulate |
                    aiProcess_JoinIdenticalVertices |
                    aiProcess_FlipUVs |
                    aiProcess_SortByPType;

                DrawSpace::Core::Meshe::NormalesGenerationMode normales_gen_mode = m_target_meshe->GetNGenerationMode();
                DrawSpace::Core::Meshe::TangentBinormalesGenerationMode tb_gen_mode = m_target_meshe->GetTBGenerationMode();
                    
                if (DrawSpace::Core::Meshe::NORMALES_AUTO == normales_gen_mode || DrawSpace::Core::Meshe::NORMALES_FROMLOADER == normales_gen_mode)
                {
                    flags |= aiProcess_GenNormals;
                }
                else if (DrawSpace::Core::Meshe::NORMALES_AUTO_SMOOTH == normales_gen_mode || DrawSpace::Core::Meshe::NORMALES_FROMLOADER_SMOOTH == normales_gen_mode)
                {
                    flags |= aiProcess_GenSmoothNormals;
                }

                if (DrawSpace::Core::Meshe::TB_AUTO == tb_gen_mode || DrawSpace::Core::Meshe::TB_FROMLOADER == tb_gen_mode)
                {
                    flags |= aiProcess_CalcTangentSpace;
                }

                const aiScene* scene = importer->ReadFileFromMemory(data, size, flags);
                if (scene)
                {
                    if (!scene->HasMeshes())
                    {
                        m_failure = true;
                        m_err_descr = dsstring("scene has no meshes : ") + m_final_asset_path;
                    }

                    aiMesh** meshes;

                    int nb_meshes;
                    aiNode* root;

                    meshes = scene->mMeshes;
                    nb_meshes = scene->mNumMeshes;
                    root = scene->mRootNode;

                    if (!root)
                    {
                        m_failure = true;
                        m_err_descr = dsstring("No root found in assimp scene : ") + m_final_asset_path;
                    }

                    m_scene = scene;
                }
                else
                {
                    m_failure = true;
                    m_err_descr = dsstring("No scene in file : ") + m_final_asset_path;
                }

                _DRAWSPACE_DELETE_N_(data);                
                m_importer = importer;

            }
            else
            {
                m_failure = true;
                m_err_descr = dsstring("LoadAndAllocBinaryFile failure : ") + m_final_asset_path;
            }
        }

        inline void SetFinalAssetPath(const dsstring& p_final_asset_path)
        {
            m_final_asset_path = p_final_asset_path;
        }

        inline void SetTargetMeshe(DrawSpace::Core::Meshe* p_meshe)
        {
            m_target_meshe = p_meshe;
        }

        inline bool Failed(void) const
        {
            return m_failure;
        }

        inline const aiScene* GetScene(void) const
        {
            return m_scene;
        }

        inline Assimp::Importer* GetImporter(void) const
        {
            return m_importer;
        }
    };



    struct ReadShaderMD5Task : public Interface::ITask
    {
    private:
        // execution data
        dsstring        m_shader_id;
        dsstring        m_filepath;

        bool            m_failure;

        dsstring        m_compare_md5;

        dsstring        m_loaded_md5;

        bool            m_md5_are_equals;

    public:

        ReadShaderMD5Task() : ITask("READMD5SHADERFILE", "")
        {
        }

        void Execute(void)
        {
            m_failure = false;
                        
            dsstring path{ bcCacheName + dsstring("/") + m_shader_id.c_str() };

            long md5filesize;
            unsigned char* md5Buf = { static_cast<unsigned char*>(Utils::FileSystem::LoadAndAllocFile(path + dsstring("\\") + bcMd5FileName, &md5filesize)) };

            if (md5Buf)
            {
                dsstring stored_md5((char*)md5Buf, md5filesize);
                _DRAWSPACE_DELETE_N_(md5Buf);
                m_loaded_md5 = stored_md5;

            }
            else
            {
                m_failure = true;
            }
        }

        inline void SetShaderId(const dsstring& p_shader_id)
        {
            m_shader_id = p_shader_id;
        }

        inline void SetFilePath(const dsstring& p_filepath)
        {
            m_filepath = p_filepath;
        }

        inline void SetCompareMD5(const dsstring& p_md5)
        {
            m_compare_md5 = p_md5;
        }

        inline bool MD5AreEquals(void) const
        {
            return (m_compare_md5 == m_loaded_md5);
        }

        inline bool Failed(void) const
        {
            return m_failure;
        }

    };


    struct CompileShaderTask : public Interface::ITask
    {
    private:

        bool                            m_failure;
        dsstring                        m_err_compil;

        DrawSpace::Interface::Renderer* m_renderer{ nullptr };

        long                            m_text_size;
        void*                           m_text{ nullptr };
        int                             m_shadertype;

        dsstring                        m_final_asset_dir;
        dsstring                        m_asset_path;

        // shader bytecode buffer infos...
        long                            m_bc_length;
        void*                           m_bc{ nullptr };

    public:

        CompileShaderTask() : ITask("COMPILESHADER", "")
        {
        }

        inline void Execute(void)
        {
            m_failure = false;

            void* bytecode_handle;

            bool comp_status{ m_renderer->CreateShaderBytes((char*)m_text, m_text_size, m_shadertype, m_asset_path, m_final_asset_dir, &bytecode_handle) };
            if (!comp_status)
            {
                dsstring err_compil{ m_renderer->GetShaderCompilationError(bytecode_handle) };

                m_failure = true;
                m_err_compil = err_compil;
            }
            else
            {
                m_bc_length = m_renderer->GetShaderBytesLength(bytecode_handle);

                m_bc = (void*)_DRAWSPACE_NEW_EXPLICIT_SIZE_WITH_COMMENT(unsigned char, unsigned char[m_bc_length], m_bc_length, m_final_asset_dir);
                memcpy(m_bc, m_renderer->GetShaderBytes(bytecode_handle), m_bc_length);

                m_renderer->ReleaseShaderBytes(bytecode_handle);
            }
        }

        inline void SetShaderText(void* p_text, long p_text_size, int p_shadertype)
        {
            m_text_size     = p_text_size;
            m_text          = p_text;
            m_shadertype    = p_shadertype;
        }

        inline void SetShaderDirs(const dsstring& p_final_asset_dir, const dsstring& p_asset_path)
        {
            m_final_asset_dir   = p_final_asset_dir;
            m_asset_path        = p_asset_path;
        }

        inline void SetRenderer(DrawSpace::Interface::Renderer* p_renderer)
        {
            m_renderer = p_renderer;
        }

        inline bool Failed(void) const
        {
            return m_failure;
        }

        inline dsstring GetCompilationError(void) const
        {
            return m_err_compil;
        }

        inline void* GetShaderByteCode(void) const
        {
            return m_bc;
        }

        inline long GetShaderByteCodeLength(void) const
        {
            return m_bc_length;
        }
    };


    struct UpdateBCFilesTask : public Interface::ITask
    {
    private:
        // execution data
        dsstring    m_shader_id;

        dsstring    m_hash;

        long        m_bc_length{ 0 };
        void*       m_bc{ nullptr };

    public:

        UpdateBCFilesTask() : ITask("UPDATEBCFILES", "")
        {
        }

        inline void Execute(void)
        {
            dsstring path{ bcCacheName + dsstring("/") + m_shader_id.c_str() };

            DrawSpace::Utils::FileSystem::WriteFile(path + dsstring("\\") + bcMd5FileName, (void*)m_hash.c_str(), m_hash.length());
            DrawSpace::Utils::FileSystem::WriteFile(path + dsstring("\\") + bcCodeFileName, m_bc, m_bc_length);
        }

        inline void SetBC(void* p_bc, long p_bc_length)
        {
            m_bc = p_bc;
            m_bc_length = p_bc_length;
        }

        inline void SetHash(const dsstring& p_hash)
        {
            m_hash = p_hash;
        }

        inline void SetShaderId(const dsstring& p_shader_id)
        {
            m_shader_id = p_shader_id;
        }

    };

    struct CreateDirectoryTask : public Interface::ITask
    {
    private:

        dsstring m_shader_id;

    public:

        CreateDirectoryTask() : ITask("CREATEDIRECTORY", "")
        {
        }

        inline void Execute(void)
        {
            dsstring path{ bcCacheName + dsstring("\\") + m_shader_id.c_str() };
            DrawSpace::Utils::FileSystem::CreateDirectory(path);
        }

        inline void SetShaderId(const dsstring& p_shader_id)
        {
            m_shader_id = p_shader_id;
        }
    };

    struct FillMeshesOwnerNamesTask : public Interface::ITask
    {
    private:

        aiNode*                 m_root{ nullptr };
        std::vector<dsstring>   m_meshes_node_owner_names;
        int                     m_nb_meshes;


        inline void fill_scene_node(aiNode* p_ai_node)
        {
            if (p_ai_node->mNumMeshes > 0)
            {
                for (unsigned int i = 0; i < p_ai_node->mNumMeshes; i++)
                {
                    int index = p_ai_node->mMeshes[i];
                    m_meshes_node_owner_names[index] = dsstring(p_ai_node->mName.C_Str());
                }
            }

            for (size_t i = 0; i < p_ai_node->mNumChildren; i++)
            {
                fill_scene_node(p_ai_node->mChildren[i]);
            }
        }
       
    public:

        FillMeshesOwnerNamesTask() : ITask("FILLMESHESOWNERNAMES", "")
        {
        }

        inline void Execute(void)
        {
            m_meshes_node_owner_names.resize(m_nb_meshes);
            fill_scene_node(m_root);
        }

        inline void SetNbMeshes(int p_nb_meshes)
        {
            m_nb_meshes = p_nb_meshes;
        }

        inline void SetRoot(aiNode* p_root)
        {
            m_root = p_root;
        }

        inline std::vector<dsstring> GetNodesNamesList(void) const
        {
            return m_meshes_node_owner_names;
        }
    };

    struct LoadAnimationsTask : public Interface::ITask
    {
    private:

        const aiScene*              m_scene{ nullptr };
        Aspect::AnimationsAspect*   m_anims_aspect{ nullptr };
        
        inline void load_scene_nodes_hierachy(aiNode* p_ai_node, std::map<dsstring, Aspect::AnimationsAspect::Node>& p_node_table)
        {
            Aspect::AnimationsAspect::Node node;

            node.id = p_ai_node->mName.C_Str();
            if (p_ai_node->mParent)
            {
                node.parent_id = p_ai_node->mParent->mName.C_Str();
            }

            node.locale_transform = ConvertFromAssimpMatrix(p_ai_node->mTransformation);

            for (size_t i = 0; i < p_ai_node->mNumChildren; i++)
            {
                node.children.push_back(p_ai_node->mChildren[i]->mName.C_Str());
                load_scene_nodes_hierachy(p_ai_node->mChildren[i], p_node_table);
            }

            /* theoriquement impossible qu'on tombe dans ce cas
            if (m_scene_nodes.count(node.id) > 0)
            {
                _DSEXCEPTION("assimp node with same name exists");
            }
            */

            p_node_table[node.id] = node;
        }

        inline void load_animations(const aiScene* p_scene, Aspect::AnimationsAspect* p_anims_aspect)
        {
            auto& animations = p_anims_aspect->GetComponent<std::map<dsstring, Aspect::AnimationsAspect::AnimationRoot>>("animations")->getPurpose();

            for (size_t i = 0; i < p_scene->mNumAnimations; i++)
            {
                aiAnimation* ai_animation = p_scene->mAnimations[i];

                Aspect::AnimationsAspect::AnimationRoot animation;

                animation.duration = ai_animation->mDuration;
                animation.ticksPerSeconds = ai_animation->mTicksPerSecond;

                animations[ai_animation->mName.C_Str()] = animation;

                for (size_t j = 0; j < ai_animation->mNumChannels; j++)
                {
                    aiNodeAnim* ai_node_anim = ai_animation->mChannels[j];
                    Aspect::AnimationsAspect::NodeAnimation node_animation;

                    node_animation.node_name = ai_node_anim->mNodeName.C_Str();

                    for (size_t k = 0; k < ai_node_anim->mNumPositionKeys; k++)
                    {
                        aiVectorKey ai_key = ai_node_anim->mPositionKeys[k];
                        Aspect::AnimationsAspect::VectorKey pos_key{ ai_key.mTime, { ai_key.mValue[0], ai_key.mValue[1], ai_key.mValue[2], 1.0 } };

                        node_animation.position_keys.push_back(pos_key);
                    }

                    for (size_t k = 0; k < ai_node_anim->mNumScalingKeys; k++)
                    {
                        aiVectorKey ai_key = ai_node_anim->mScalingKeys[k];
                        Aspect::AnimationsAspect::VectorKey pos_key{ ai_key.mTime, { ai_key.mValue[0], ai_key.mValue[1], ai_key.mValue[2], 1.0 } };

                        node_animation.scaling_keys.push_back(pos_key);
                    }

                    for (size_t k = 0; k < ai_node_anim->mNumRotationKeys; k++)
                    {
                        aiQuatKey ai_key = ai_node_anim->mRotationKeys[k];
                        Aspect::AnimationsAspect::QuaternionKey pos_key{ ai_key.mTime, { ai_key.mValue.x, ai_key.mValue.y, ai_key.mValue.z, ai_key.mValue.w } };

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

    public:

        LoadAnimationsTask() : ITask("LOADANIMATIONS", "")
        {
        }

        inline void Execute(void)
        {
            std::map<dsstring, Aspect::AnimationsAspect::Node> scene_nodes;
            aiNode* root{ m_scene->mRootNode };

            load_scene_nodes_hierachy(root, scene_nodes);

            m_anims_aspect->GetComponent<std::map<dsstring, Aspect::AnimationsAspect::Node>>("nodes")->getPurpose() = scene_nodes;

            /// Loading animations & keyframes infos
            load_animations(m_scene, m_anims_aspect);

        }

        void SetScene(const aiScene* p_scene)
        {
            m_scene = p_scene;
        }

        void SetAnimAspect(Aspect::AnimationsAspect* p_aspect)
        {
            m_anims_aspect = p_aspect;
        }
    };

    struct BuildMesheTask : public Interface::ITask
    {
    private:

        Core::Entity*   m_entity;
        dsstring        m_meshe_id;
        aiNode*         m_meshe_node;
        aiMesh**        m_meshes;
        Core::Meshe*    m_target_meshe;

        inline void build_meshe(Core::Entity* p_entity, const dsstring& p_id, aiNode* p_ai_node, aiMesh** p_meshes, Core::Meshe* p_destination)
        {
            Aspect::AnimationsAspect* anims_aspect = p_entity->GetAspect<Aspect::AnimationsAspect>();

            dsstring name = p_ai_node->mName.C_Str();

            Core::Meshe::NormalesGenerationMode normales_gen_mode = p_destination->GetNGenerationMode();
            Core::Meshe::TangentBinormalesGenerationMode tb_gen_mode = p_destination->GetTBGenerationMode();

            unsigned int nb_meshes = p_ai_node->mNumMeshes;
            int global_index = 0;

            unsigned int* indexes = p_ai_node->mMeshes;
            for (unsigned int i = 0; i < nb_meshes; i++)
            {
                aiMesh* meshe = p_meshes[indexes[i]];

                for (size_t j = 0; j < meshe->mNumFaces; j++)
                {
                    aiFace face = meshe->mFaces[j];

                    if (face.mNumIndices != 3)
                    {
                        _DSEXCEPTION("Face must have exactly 3 indices");
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

                    if (meshe->GetNumUVChannels() > 0)
                    {
                        aiVector3D texCoord = meshe->HasTextureCoords(0) ? meshe->mTextureCoords[0][j] : Zero3D;
                        v_out.tu[0] = texCoord[0];
                        v_out.tv[0] = texCoord[1];
                    }

                    if (hasN && (normales_gen_mode == Core::Meshe::NORMALES_AUTO || normales_gen_mode == Core::Meshe::NORMALES_FROMLOADER ||
                        normales_gen_mode == Core::Meshe::NORMALES_AUTO_SMOOTH || normales_gen_mode == Core::Meshe::NORMALES_FROMLOADER_SMOOTH))
                    {
                        // model has its own normales, so use it
                        v_out.nx = meshe->mNormals[j][0];
                        v_out.ny = meshe->mNormals[j][1];
                        v_out.nz = meshe->mNormals[j][2];
                    }

                    if (hasTB && (tb_gen_mode == Core::Meshe::TB_AUTO || tb_gen_mode == Core::Meshe::TB_FROMLOADER))
                    {
                        // model has its own tangent and binormales, so use it

                        v_out.tx = meshe->mTangents[j][0];
                        v_out.ty = meshe->mTangents[j][1];
                        v_out.tz = meshe->mTangents[j][2];

                        v_out.bx = meshe->mBitangents[j][0];
                        v_out.by = meshe->mBitangents[j][1];
                        v_out.bz = meshe->mBitangents[j][2];
                    }

                    p_destination->AddVertex(v_out);
                }

                if (normales_gen_mode == Core::Meshe::NORMALES_COMPUTED ||
                    ((normales_gen_mode == Core::Meshe::NORMALES_AUTO || normales_gen_mode == Core::Meshe::NORMALES_AUTO_SMOOTH) && !hasN))
                {
                    p_destination->ComputeNormales();
                }

                if (tb_gen_mode == Core::Meshe::TB_COMPUTED || tb_gen_mode == Core::Meshe::TB_AUTO && !hasTB)
                {
                    if (meshe->GetNumUVChannels() > 0)
                    {
                        p_destination->ComputeTBs();
                    }
                    else
                    {
                        _DSEXCEPTION("No UV channel in meshe, cannot compute T,B vectors")
                    }
                }

                //////////// transformation des normales
               Utils::Matrix n_transf = p_destination->GetNormalesTransf();

                for (long j = 0; j < p_destination->GetVertexListSize(); j++)
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
                    std::vector<Aspect::AnimationsAspect::BoneOutput> bones_outputs;
                    std::map<dsstring, int> bones_mapping;

                    for (size_t j = 0; j < meshe->mNumBones; j++)
                    {
                        aiBone* bone = meshe->mBones[j];

                        Aspect::AnimationsAspect::BoneOutput bone_output;
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
                    anims_aspect->GetComponent<std::vector<Aspect::AnimationsAspect::BoneOutput>>("bones_outputs")->getPurpose() = bones_outputs;
                    anims_aspect->GetComponent<std::map<dsstring, int>>("bones_mapping")->getPurpose() = bones_mapping;

                    Aspect::RenderingAspect* rendering_aspect = p_entity->GetAspect <Aspect::RenderingAspect>();
                    if (!rendering_aspect)
                    {
                        _DSEXCEPTION("an entity with AnimationsAspect must also have a RenderingAspect");
                    }

                    Core::ComponentList<AspectImplementations::MesheRenderingAspectImpl::PassSlot> passes;
                    rendering_aspect->GetComponentsByType<AspectImplementations::MesheRenderingAspectImpl::PassSlot>(passes);

                    for (auto e : passes)
                    {
                        Core::RenderingNode* rnode = e->getPurpose().GetRenderingNode();

                        rnode->AddShaderArrayParameter(0, Systems::AnimationsSystem::bonesBuffer0Id, Systems::AnimationsSystem::bonesBuffer0StartReg);
                        rnode->AddShaderArrayParameter(0, Systems::AnimationsSystem::bonesBuffer1Id, Systems::AnimationsSystem::bonesBuffer1StartReg);
                    }
                }

                //////////////////////////////////

                global_index += meshe->mNumVertices;
            }
        }



    public:

        BuildMesheTask() : ITask("BUILDMESHE", "")
        {
        }

        inline void Execute(void)
        {
            build_meshe(m_entity, m_meshe_id, m_meshe_node, m_meshes, m_target_meshe);
        }

        void SetEntity(Core::Entity* p_entity)
        {
            m_entity = p_entity;
        }

        void SetMeshesIOInfos(const dsstring& p_meshe_id, aiMesh** m_source_meshes, Core::Meshe* p_target_meshe, aiNode* p_meshe_node)
        {
            m_meshe_id = p_meshe_id;
            m_target_meshe = p_target_meshe;
            m_meshes = m_source_meshes;
            m_meshe_node = p_meshe_node;
        }

        

    };



    static dsstring                                                     m_textures_rootpath;

    static dsstring                                                     m_shaders_rootpath;
    static bool                                                         m_addshaderspath;

    static dsstring                                                     m_meshes_rootpath;

    std::map<dsstring, Blob>                                            m_texturesCache;
    std::map<dsstring, Blob>                                            m_shadersCache;
    std::map<dsstring, std::pair<Assimp::Importer*, const aiScene*>>    m_meshesCache;


    std::map<dsstring, bool>                                            m_asset_loading_state;

	DrawSpace::Interface::Renderer*										m_renderer;


    // list with all type of tasks
    std::set<dsstring>                                                  m_currenttasks;
    std::map<dsstring, LoadFileTask>                                    m_loadFile_tasks;

    RunnerSystem&                                                       m_runner_system;
    

    void run(EntityGraph::EntityNodeGraph* p_entitygraph);

    dsstring compute_textures_final_path(const dsstring& p_path) const;
    dsstring compute_shaders_final_path(const dsstring& p_path) const;
    dsstring compute_meshes_final_path(const dsstring& p_path) const;

    void build_meshe(Core::Entity* p_entity, const dsstring& p_id, aiNode* p_ai_node, aiMesh** p_meshes, Core::Meshe* p_destination);

	// recusive
	void load_scene_nodes_hierachy(aiNode* p_ai_node, int depth, std::map<dsstring, Aspect::AnimationsAspect::Node>& p_node_table);
    
    template<typename T>
    void launchAssetLoadingInRunner(const dsstring& p_final_asset_path)
    {
        const dsstring task_id{ p_final_asset_path };

        LoadFileTask task;
        task.SetTargetDescr(task_id);
        task.SetActionDescr("LOADASSETFILE");
        task.SetFinalAssetPath(p_final_asset_path);

        m_loadFile_tasks[p_final_asset_path] = task;
        m_currenttasks.insert(p_final_asset_path);

        Threading::Runner* runner{ Threading::Runner::GetInstance() };
        notify_event(BLOB_LOAD, p_final_asset_path);
        runner->m_mailbox_in.Push<ITask*>(&m_loadFile_tasks.at(p_final_asset_path));
    }
    

	void load_animations(const aiScene* p_scene, Aspect::AnimationsAspect* p_anims_aspect);

	void manage_shader_in_bccache(Core::Shader* p_shader, const dsstring& p_asset_path, const dsstring& p_final_asset_path, const dsstring& p_final_asset_dir, int p_shader_type);

	void check_bc_cache_presence(void) const;

	void update_bc_md5file(const dsstring& p_path, const dsstring& p_hash);

	void update_bc_codefile(const dsstring& p_path, void* p_bc, int p_size);

    void notify_event(ResourceEvent p_event, const dsstring& p_path) const;

    void check_all_assets_loaded(void);

    //void check_finished_tasks(void);

public:

    ResourcesSystem(RunnerSystem& p_runner);

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



};
}
}

