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

#include <vector>
#include <assimp/scene.h>           // Output data structure
#include "task.h"

namespace DrawSpace
{
namespace Systems
{
struct FillMeshesOwnerNamesTask : public Task
{
private:

    aiNode*                 m_root{ nullptr };
    std::vector<dsstring>   m_meshes_node_owner_names;
    int                     m_nb_meshes;

    void fill_scene_node(aiNode* p_ai_node);

public:

    FillMeshesOwnerNamesTask();

    void                    execute(void);
    void                    SetNbMeshes(int p_nb_meshes);
    void                    SetRoot(aiNode* p_root);
    std::vector<dsstring>   GetNodesNamesList(void) const;
};

}
}

