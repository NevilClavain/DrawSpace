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
#include "animationssystem.h"

#include "animationsaspect.h"
#include "renderingaspect.h"
#include "mesherenderingaspectimpl.h"

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
using namespace DrawSpace::AspectImplementations;

dsstring ResourcesSystem::m_textures_rootpath = ".";
dsstring ResourcesSystem::m_meshes_rootpath = ".";
dsstring ResourcesSystem::m_shaders_rootpath = ".";
bool ResourcesSystem::m_addshaderspath = false;

ResourcesSystem::ResourcesSystem(void)
{
	m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
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

        ComponentList<std::tuple<Shader*, bool, int>> shaders_assets;
        resources_aspect->GetComponentsByType<std::tuple<Shader*, bool, int>>(shaders_assets);

        for (auto& e : shaders_assets)
        {
            bool& loaded = std::get<1>(e->getPurpose());
            if (!loaded)
            {
				Shader* shader { std::get<0>(e->getPurpose()) };

				dsstring asset_path;
				std::get<0>(e->getPurpose())->GetBasePath(asset_path);
				dsstring final_asset_path = compute_shaders_final_path(asset_path);

				updateAssetFromCache<Shader>(std::get<0>(e->getPurpose()), m_shadersCache, final_asset_path);
				loaded = true;

				/*
				if(!shader->IsCompiled())
				{
					int shader_type{ std::get<2>(e->getPurpose()) };

					void* bytecode_handle;

					Blob shader_blob{ m_shadersCache.at(final_asset_path) };

					bool comp_status{ m_renderer->CreateShaderBytes((char* )shader_blob.data, shader_blob.size, shader_type, asset_path, &bytecode_handle) };

					if (!comp_status)
					{
						dsstring err_text = m_renderer->GetShaderCompilationError(bytecode_handle);
					}
					else
					{
						size_t bc_length { m_renderer->GetShaderBytesLength(bytecode_handle) };

						void* bc { m_renderer->GetShaderBytes(bytecode_handle) };
					}

					m_renderer->ReleaseShaderBytes(bytecode_handle);
					
				}
				*/
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
				ResourcesAspect::MeshesFileDescription mesheFileDescription;

                dsstring final_asset_path = compute_meshes_final_path(std::get<1>(e->getPurpose()));
                dsstring meshe_id = std::get<2>(e->getPurpose());
			      
				aiMesh** meshes;
				std::vector<dsstring> meshes_node_owner_names; // associ� a aiMesh** meshes -> meshes_node_owner_names[i] -> contient le nom du node propri�taire du meshes[i]

				int nb_meshes;
				aiNode* root;
				const aiScene* scene;

				if (m_meshesCache.find(final_asset_path) == m_meshesCache.end())
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

					if (Meshe::NORMALES_AUTO == normales_gen_mode || Meshe::NORMALES_FROMLOADER == normales_gen_mode)
					{
						flags |= aiProcess_GenNormals;
					}
					else if (Meshe::NORMALES_AUTO_SMOOTH == normales_gen_mode || Meshe::NORMALES_FROMLOADER_SMOOTH == normales_gen_mode)
					{
						flags |= aiProcess_GenSmoothNormals;
					}

					if (Meshe::TB_AUTO == tb_gen_mode || Meshe::TB_FROMLOADER == tb_gen_mode)
					{
						flags |= aiProcess_CalcTangentSpace;
					}

					scene = importer->ReadFileFromMemory(data, size, flags);
					if (scene)
					{
						if (!scene->HasMeshes())
						{
							_DSEXCEPTION("No meshes in file : " + final_asset_path);
						}
						meshes = scene->mMeshes;
						nb_meshes = scene->mNumMeshes;
						root = scene->mRootNode;	
						if(!root)
						{
							_DSEXCEPTION("No root found in assimp scene");
						}
					}
					else
					{
						_DSEXCEPTION("No scene in file : " + final_asset_path);
					}

					_DRAWSPACE_DELETE_N_(data);

					m_meshesCache[final_asset_path] = std::make_pair(importer, scene);
				}
				else
				{
					std::pair<Assimp::Importer*, const aiScene*> entry = m_meshesCache.at(final_asset_path);

					meshes = entry.second->mMeshes;
					nb_meshes = entry.second->mNumMeshes;
					root = entry.second->mRootNode;
					scene = entry.second;
				}

				mesheFileDescription.file = std::get<1>(e->getPurpose());

				_DSDEBUG(rs_logger, dsstring("************************************SCENE INFOS***********************************"));
				_DSDEBUG(rs_logger, dsstring("resources = ") << final_asset_path);
				_DSDEBUG(rs_logger, dsstring("scene HasMeshes ") << scene->HasMeshes());
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

				mesheFileDescription.has_meshes = scene->HasMeshes();
				mesheFileDescription.num_meshes = scene->mNumMeshes;

				mesheFileDescription.has_animations = scene->HasAnimations();
				mesheFileDescription.num_animations = scene->mNumAnimations;


				meshes_node_owner_names.resize(scene->mNumMeshes);

				_DSDEBUG(rs_logger, dsstring("************************************NODE HIERARCHY BEGIN***********************************"));

				dump_assimp_scene_node(root, 1, mesheFileDescription, meshes_node_owner_names);

				_DSDEBUG(rs_logger, dsstring("************************************NODE HIERARCHY END*************************************"));

				// ICI : remplir les descriptions meshes a partir du tableau aiMesh** meshes;
				for (int i = 0; i < nb_meshes; i++)
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

					mesheFileDescription.meshes_descriptions.push_back(mesheDescription);
				}
				//////



				_DSDEBUG(rs_logger, dsstring("************************************Animations list BEGIN***********************************"));

				for (size_t i = 0; i < scene->mNumAnimations; i++)
				{
					_DSDEBUG(rs_logger, dsstring("Animation ") << i);

					aiAnimation* animation = scene->mAnimations[i];

					_DSDEBUG(rs_logger, dsstring("Name = ") << animation->mName.C_Str());
					_DSDEBUG(rs_logger, dsstring("TicksPerSeconds = ") << animation->mTicksPerSecond);
					_DSDEBUG(rs_logger, dsstring("Duration (ticks) = ") << animation->mDuration);
					_DSDEBUG(rs_logger, dsstring("Num Channels = ") << animation->mNumChannels);
					_DSDEBUG(rs_logger, dsstring("Num Mesh Channels = ") << animation->mMeshChannels);
					_DSDEBUG(rs_logger, dsstring("Num Morph Mesh Channels = ") << animation->mNumMorphMeshChannels);

					ResourcesAspect::AnimationDescription animation_description;

					animation_description.name = animation->mName.C_Str();
					animation_description.ticks_per_seconds = animation->mTicksPerSecond;
					animation_description.num_channels = animation->mNumChannels;
					animation_description.duration_seconds = animation->mDuration / animation->mTicksPerSecond;

					mesheFileDescription.anims_descriptions.push_back(animation_description);
				}

				_DSDEBUG(rs_logger, dsstring("************************************Animations list END*************************************"));

				AnimationsAspect* anims_aspect = p_entity->GetAspect<AnimationsAspect>();
				if (anims_aspect)
				{
					std::map<dsstring, AnimationsAspect::Node> scene_nodes;
					load_scene_nodes_hierachy(root, 1, scene_nodes);
					anims_aspect->GetComponent<std::map<dsstring, AnimationsAspect::Node>>("nodes")->getPurpose() = scene_nodes;

					anims_aspect->GetComponent<dsstring>("nodes_root_id")->getPurpose() = root->mName.C_Str();

					/// Loading animations & keyframes infos
					load_animations(scene, anims_aspect);
				}

				if (meshe_id != "")
				{
					aiNode* meshe_node = root->FindNode(meshe_id.c_str());
					if (meshe_node)
					{
						build_meshe(p_entity, meshe_id, meshe_node, meshes, target_meshe);
					}
					else
					{
						_DSEXCEPTION("cannot locate meshe objet " + meshe_id);
					}
				}

				_DSDEBUG(rs_logger, dsstring("************************************SCENE INFOS END*******************************"));

				resources_aspect->AddMeshesFileDescription(mesheFileDescription);

                loaded = true;
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

void ResourcesSystem::dump_assimp_scene_node(aiNode* p_ai_node, int depth, ResourcesAspect::MeshesFileDescription& p_description, std::vector<dsstring>& p_meshes_node_owner_names)
{
    dsstring spacing(depth, ' ');
    _DSDEBUG(rs_logger, spacing + dsstring("node : ") << p_ai_node->mName.C_Str() << " nb children : " << p_ai_node->mNumChildren);
    _DSDEBUG(rs_logger, spacing + dsstring("nb meshes : ") << p_ai_node->mNumMeshes);

    _DSDEBUG(rs_logger, spacing + dsstring("  -> ") << p_ai_node->mTransformation.a1 << " " << p_ai_node->mTransformation.b1 << " " << p_ai_node->mTransformation.c1 << " " << p_ai_node->mTransformation.d1);
    _DSDEBUG(rs_logger, spacing + dsstring("  -> ") << p_ai_node->mTransformation.a2 << " " << p_ai_node->mTransformation.b2 << " " << p_ai_node->mTransformation.c2 << " " << p_ai_node->mTransformation.d2);
    _DSDEBUG(rs_logger, spacing + dsstring("  -> ") << p_ai_node->mTransformation.a3 << " " << p_ai_node->mTransformation.b3 << " " << p_ai_node->mTransformation.c3 << " " << p_ai_node->mTransformation.d3);
    _DSDEBUG(rs_logger, spacing + dsstring("  -> ") << p_ai_node->mTransformation.a4 << " " << p_ai_node->mTransformation.b4 << " " << p_ai_node->mTransformation.c4 << " " << p_ai_node->mTransformation.d4);

	if (p_ai_node->mNumMeshes > 0)
	{
		for (unsigned int i = 0; i < p_ai_node->mNumMeshes; i++)
		{
			int index = p_ai_node->mMeshes[i];

			p_meshes_node_owner_names[index] = dsstring( p_ai_node->mName.C_Str() );
		}
	}

    for( size_t i = 0; i < p_ai_node->mNumChildren; i++)
    {
        dump_assimp_scene_node(p_ai_node->mChildren[i], depth+1, p_description, p_meshes_node_owner_names);
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

        _DSDEBUG(rs_logger, dsstring("************************************MESHE INFOS***********************************"));
        _DSDEBUG(rs_logger, dsstring("owner node id = ") + p_id);
        _DSDEBUG(rs_logger, dsstring("name = ") << dsstring( meshe->mName.C_Str() ));
        _DSDEBUG(rs_logger, dsstring("meshe HasPositions ") << meshe->HasPositions());
        _DSDEBUG(rs_logger, dsstring("meshe HasFaces ") << meshe->HasFaces());
        _DSDEBUG(rs_logger, dsstring("meshe HasNormals ") << meshe->HasNormals());        
        _DSDEBUG(rs_logger, dsstring("meshe HasTangentsAndBitangents ") << meshe->HasTangentsAndBitangents());
        _DSDEBUG(rs_logger, dsstring("meshe NumUVChannels ") << meshe->GetNumUVChannels());
        _DSDEBUG(rs_logger, dsstring("meshe HasBones ") << meshe->HasBones());
        _DSDEBUG(rs_logger, dsstring("meshe NumBones ") << meshe->mNumBones);
        
        for(size_t j = 0; j < meshe->mNumBones; j++)
        {
            aiBone* bone = meshe->mBones[j];

            _DSDEBUG(rs_logger, dsstring("Bone ") << j);
            _DSDEBUG(rs_logger, dsstring("  -> name = ") << bone->mName.C_Str());
            _DSDEBUG(rs_logger, dsstring("  -> offsetMatrx"));
            _DSDEBUG(rs_logger, dsstring("  -> ") << bone->mOffsetMatrix.a1 << " " << bone->mOffsetMatrix.b1 << " " << bone->mOffsetMatrix.c1 << " " << bone->mOffsetMatrix.d1);
            _DSDEBUG(rs_logger, dsstring("  -> ") << bone->mOffsetMatrix.a2 << " " << bone->mOffsetMatrix.b2 << " " << bone->mOffsetMatrix.c2 << " " << bone->mOffsetMatrix.d2);
            _DSDEBUG(rs_logger, dsstring("  -> ") << bone->mOffsetMatrix.a3 << " " << bone->mOffsetMatrix.b3 << " " << bone->mOffsetMatrix.c3 << " " << bone->mOffsetMatrix.d3);
            _DSDEBUG(rs_logger, dsstring("  -> ") << bone->mOffsetMatrix.a4 << " " << bone->mOffsetMatrix.b4 << " " << bone->mOffsetMatrix.c4 << " " << bone->mOffsetMatrix.d4);

        }


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
