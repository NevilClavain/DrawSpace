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

#include <array>
#include "buildemeshetask.h"
#include "resourcessystem.h"
#include "passslot.h"
#include "logging.h"

using namespace DrawSpace::Core;

DrawSpace::Logger::Sink bmt_logger("BuildeMesheTask", DrawSpace::Logger::Configuration::getInstance());

using namespace DrawSpace;
using namespace DrawSpace::Systems;

BuildMesheTask::BuildMesheTask() : ITask("BUILDMESHE", "")
{
}

void BuildMesheTask::Execute(void)
{
    build_meshe(m_entity, m_meshe_node, m_meshes, m_target_meshe);
}

void BuildMesheTask::SetEntity(Core::Entity* p_entity)
{
    m_entity = p_entity;
}

void BuildMesheTask::SetMeshesIOInfos(aiMesh** m_source_meshes, Core::Meshe* p_target_meshe, aiNode* p_meshe_node)
{
    m_target_meshe = p_target_meshe;
    m_meshes = m_source_meshes;
    m_meshe_node = p_meshe_node;
}

void BuildMesheTask::build_meshe(Core::Entity* p_entity, aiNode* p_ai_node, aiMesh** p_meshes, Core::Meshe* p_destination)
{   
    const auto anims_aspect { p_entity->GetAspect<Aspect::AnimationsAspect>() };
    const dsstring name { p_ai_node->mName.C_Str() };

    _DSDEBUG(bmt_logger, dsstring("name = ") + name);

    const auto normales_gen_mode { p_destination->GetNGenerationMode() };
  
    static const std::map<Meshe::NormalesGenerationMode, dsstring> normales_gen_mode_text
    {
        { Meshe::NormalesGenerationMode::NORMALES_DISCARDED, "NORMALES_DISCARDED" },
        { Meshe::NormalesGenerationMode::NORMALES_AUTO, "NORMALES_AUTO" },
        { Meshe::NormalesGenerationMode::NORMALES_AUTO_SMOOTH, "NORMALES_AUTO_SMOOTH" },
        { Meshe::NormalesGenerationMode::NORMALES_FROMLOADER, "NORMALES_FROMLOADER" },
        { Meshe::NormalesGenerationMode::NORMALES_FROMLOADER_SMOOTH, "NORMALES_FROMLOADER_SMOOTH" },
        { Meshe::NormalesGenerationMode::NORMALES_COMPUTED, "NORMALES_COMPUTED" },
    };
    _DSDEBUG(bmt_logger, dsstring("normales_gen_mode = ") + normales_gen_mode_text.at(normales_gen_mode));

    const auto tb_gen_mode { p_destination->GetTBGenerationMode() };

    static const std::map<Meshe::TangentBinormalesGenerationMode, dsstring> tbn_gen_mode_text
    {
        { Meshe::TangentBinormalesGenerationMode::TB_DISCARDED, "TB_DISCARDED" },
        { Meshe::TangentBinormalesGenerationMode::TB_AUTO, "TB_AUTO" },
        { Meshe::TangentBinormalesGenerationMode::TB_FROMLOADER, "TB_FROMLOADER" },
        { Meshe::TangentBinormalesGenerationMode::TB_COMPUTED, "TB_COMPUTED" }
    };
    _DSDEBUG(bmt_logger, dsstring("tbn_gen_mode = ") + tbn_gen_mode_text.at(tb_gen_mode));


    const auto nb_meshes{ p_ai_node->mNumMeshes };
    int global_index = 0;

    const auto indexes{ p_ai_node->mMeshes };
    for (unsigned int i = 0; i < nb_meshes; i++)
    {
        const auto meshe { p_meshes[indexes[i]] };
        for (size_t j = 0; j < meshe->mNumFaces; j++)
        {
            const auto face{ meshe->mFaces[j] };

            if (face.mNumIndices != 3)
            {
                _DSEXCEPTION("Face must have exactly 3 indices");
            }

            const auto i1 { face.mIndices[0] };
            const auto i2 { face.mIndices[1] };
            const auto i3 { face.mIndices[2] };

            p_destination->AddTriangle(Commons::TrianglePrimitive<unsigned int>({ i1 + global_index, i2 + global_index, i3 + global_index }));
        }

        const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
        const auto hasN { meshe->HasNormals() };
        const auto hasTB { meshe->HasTangentsAndBitangents() };

        for (size_t j = 0; j < meshe->mNumVertices; j++)
        {
            const auto v_in { meshe->mVertices[j] };
            DrawSpace::Commons::Vertex v_out(v_in[0], v_in[1], v_in[2]);

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
                const auto texCoord{ meshe->HasTextureCoords(0) ? meshe->mTextureCoords[0][j] : Zero3D };
                v_out.tu[0] = texCoord[0];
                v_out.tv[0] = texCoord[1];
            }

            if (hasN && (normales_gen_mode == Core::Meshe::NormalesGenerationMode::NORMALES_AUTO || normales_gen_mode == Core::Meshe::NormalesGenerationMode::NORMALES_FROMLOADER ||
                normales_gen_mode == Core::Meshe::NormalesGenerationMode::NORMALES_AUTO_SMOOTH || normales_gen_mode == Core::Meshe::NormalesGenerationMode::NORMALES_FROMLOADER_SMOOTH))
            {
                // model has its own normales, so use it
                v_out.nx = meshe->mNormals[j][0];
                v_out.ny = meshe->mNormals[j][1];
                v_out.nz = meshe->mNormals[j][2];
            }

            if (hasTB && (tb_gen_mode == Core::Meshe::TangentBinormalesGenerationMode::TB_AUTO || tb_gen_mode == Core::Meshe::TangentBinormalesGenerationMode::TB_FROMLOADER))
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

        if (normales_gen_mode == Core::Meshe::NormalesGenerationMode::NORMALES_COMPUTED ||
            ((normales_gen_mode == Core::Meshe::NormalesGenerationMode::NORMALES_AUTO || normales_gen_mode == Core::Meshe::NormalesGenerationMode::NORMALES_AUTO_SMOOTH) && !hasN))
        {
            _DSDEBUG(bmt_logger, dsstring("Computing normales..."));
            p_destination->ComputeNormales();
        }

        if (tb_gen_mode == Core::Meshe::TangentBinormalesGenerationMode::TB_COMPUTED || tb_gen_mode == Core::Meshe::TangentBinormalesGenerationMode::TB_AUTO && !hasTB)
        {
            const auto numUVChannels{ meshe->GetNumUVChannels() };
            if (numUVChannels > 0)
            {
                p_destination->ComputeTBs();
            }
            else
            {
                _DSEXCEPTION("No UV channel in meshe, cannot compute T,B vectors")
            }
        }

        //////////// transformation des normales
        auto n_transf { p_destination->GetNormalesTransf() };

        for (size_t j = 0; j < p_destination->GetVertexListSize(); j++)
        {
            DrawSpace::Commons::Vertex vertex;
            p_destination->GetVertex(j, vertex);

            Maths::Vector n(vertex.nx, vertex.ny, vertex.nz, 1.0);
            Maths::Vector nt;

            n_transf.transform(&n, &nt);

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
                const auto bone { meshe->mBones[j] };

                Aspect::AnimationsAspect::BoneOutput bone_output;
                bone_output.offset_matrix = ResourcesSystem::ConvertFromAssimpMatrix(bone->mOffsetMatrix);

                bones_outputs.push_back(bone_output);

                bones_mapping[bone->mName.C_Str()] = j;

                for (size_t k = 0; k < bone->mNumWeights; k++)
                {
                    const auto weight { bone->mWeights[k].mWeight };
                    const auto vert_index { bone->mWeights[k].mVertexId };

                    DrawSpace::Commons::Vertex vertex;
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

            for (size_t j = 0; j < p_destination->GetVertexListSize(); j++)
            {
                DrawSpace::Commons::Vertex vertex;
                p_destination->GetVertex(j, vertex);

                const auto sum{ vertex.tu[5] + vertex.tv[5] + vertex.tw[5] + vertex.ta[5] +
                    vertex.tu[7] + vertex.tv[7] + vertex.tw[7] + vertex.ta[7] };

                if (sum < 0.95)
                {
                    _DSEXCEPTION("Vertex weights sum must be 1.0");
                }

                p_destination->SetVertex(j, vertex);
            }

            //////
            anims_aspect->GetComponent<std::vector<Aspect::AnimationsAspect::BoneOutput>>("bones_outputs")->getPurpose() = bones_outputs;
            anims_aspect->GetComponent<std::map<dsstring, int>>("bones_mapping")->getPurpose() = bones_mapping;

            const auto rendering_aspect{ p_entity->GetAspect <Aspect::RenderingAspect>() };
            if (!rendering_aspect)
            {
                _DSEXCEPTION("an entity with AnimationsAspect must also have a RenderingAspect");
            }

            Core::ComponentList<Core::PassSlot> passes;
            rendering_aspect->GetComponentsByType<Core::PassSlot>(passes);

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
