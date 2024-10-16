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

#include "loadfiletoassimptask.h"

#include "meshe.h"
#include "memalloc.h"

using namespace DrawSpace;
using namespace DrawSpace::Systems;

LoadFileToAssimpTask::LoadFileToAssimpTask() : Task("LOADFILETOASSIMP", "")
{
}

void LoadFileToAssimpTask::execute(void)
{
    long size;

    auto data{ File::loadAndAllocBinaryFile(m_final_asset_path, &size) };
    if (data)
    {
        m_failure = false;

        const auto importer{ new Assimp::Importer() };

        auto flags{ aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_FlipUVs |
            aiProcess_SortByPType };

        DrawSpace::Core::Meshe::NormalesGenerationMode normales_gen_mode = m_target_meshe->GetNGenerationMode();
        DrawSpace::Core::Meshe::TangentBinormalesGenerationMode tb_gen_mode = m_target_meshe->GetTBGenerationMode();

        if (DrawSpace::Core::Meshe::NormalesGenerationMode::NORMALES_AUTO == normales_gen_mode || DrawSpace::Core::Meshe::NormalesGenerationMode::NORMALES_FROMLOADER == normales_gen_mode)
        {
            flags |= aiProcess_GenNormals;
        }
        else if (DrawSpace::Core::Meshe::NormalesGenerationMode::NORMALES_AUTO_SMOOTH == normales_gen_mode || DrawSpace::Core::Meshe::NormalesGenerationMode::NORMALES_FROMLOADER_SMOOTH == normales_gen_mode)
        {
            flags |= aiProcess_GenSmoothNormals;
        }

        if (DrawSpace::Core::Meshe::TangentBinormalesGenerationMode::TB_AUTO == tb_gen_mode || DrawSpace::Core::Meshe::TangentBinormalesGenerationMode::TB_FROMLOADER == tb_gen_mode)
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

            const auto meshes{ scene->mMeshes };
            const auto nb_meshes{ scene->mNumMeshes };

            const auto root{ scene->mRootNode };
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

void LoadFileToAssimpTask::SetFinalAssetPath(const dsstring& p_final_asset_path)
{
    m_final_asset_path = p_final_asset_path;
}

void LoadFileToAssimpTask::SetTargetMeshe(DrawSpace::Core::Meshe* p_meshe)
{
    m_target_meshe = p_meshe;
}

bool LoadFileToAssimpTask::Failed(void) const
{
    return m_failure;
}

const aiScene* LoadFileToAssimpTask::GetScene(void) const
{
    return m_scene;
}

Assimp::Importer* LoadFileToAssimpTask::GetImporter(void) const
{
    return m_importer;
}
