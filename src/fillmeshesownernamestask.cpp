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

#include "fillmeshesownernamestask.h"

using namespace DrawSpace;
using namespace DrawSpace::Systems;

FillMeshesOwnerNamesTask::FillMeshesOwnerNamesTask() : ITask("FILLMESHESOWNERNAMES", "")
{
}

void FillMeshesOwnerNamesTask::Execute(void)
{
    m_meshes_node_owner_names.resize(m_nb_meshes);
    fill_scene_node(m_root);
}

void FillMeshesOwnerNamesTask::SetNbMeshes(int p_nb_meshes)
{
    m_nb_meshes = p_nb_meshes;
}

void FillMeshesOwnerNamesTask::SetRoot(aiNode* p_root)
{
    m_root = p_root;
}

std::vector<dsstring> FillMeshesOwnerNamesTask::GetNodesNamesList(void) const
{
    return m_meshes_node_owner_names;
}

void FillMeshesOwnerNamesTask::fill_scene_node(aiNode* p_ai_node)
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
