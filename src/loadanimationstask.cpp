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

#include "loadanimationstask.h"
#include "resourcessystem.h"

using namespace DrawSpace;
using namespace DrawSpace::Systems;


LoadAnimationsTask::LoadAnimationsTask() : ITask("LOADANIMATIONS", "")
{
}

void LoadAnimationsTask::Execute(void)
{
    std::map<dsstring, Aspect::AnimationsAspect::Node> scene_nodes;
    aiNode* root{ m_scene->mRootNode };

    load_scene_nodes_hierachy(root, scene_nodes);

    m_anims_aspect->GetComponent<std::map<dsstring, Aspect::AnimationsAspect::Node>>("nodes")->getPurpose() = scene_nodes;
    m_anims_aspect->GetComponent<dsstring>("nodes_root_id")->getPurpose() = root->mName.C_Str();

    /// Loading animations & keyframes infos
    load_animations(m_scene, m_anims_aspect);
}

void LoadAnimationsTask::SetScene(const aiScene* p_scene)
{
    m_scene = p_scene;
}

void LoadAnimationsTask::SetAnimAspect(Aspect::AnimationsAspect* p_aspect)
{
    m_anims_aspect = p_aspect;
}

void LoadAnimationsTask::load_scene_nodes_hierachy(aiNode* p_ai_node, std::map<dsstring, Aspect::AnimationsAspect::Node>& p_node_table)
{
    Aspect::AnimationsAspect::Node node;

    node.id = p_ai_node->mName.C_Str();
    if (p_ai_node->mParent)
    {
        node.parent_id = p_ai_node->mParent->mName.C_Str();
    }

    node.locale_transform = ResourcesSystem::ConvertFromAssimpMatrix(p_ai_node->mTransformation);

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

void LoadAnimationsTask::load_animations(const aiScene* p_scene, Aspect::AnimationsAspect* p_anims_aspect)
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
