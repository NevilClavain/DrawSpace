/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2021                     
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

#include "task.h"
#include "animationsaspect.h"
#include "meshe.h"
#include <assimp/scene.h>           // Output data structure

namespace DrawSpace
{
namespace Systems
{
struct BuildMesheTask : public Interface::ITask
{
private:

    Core::Entity*   m_entity;
    dsstring        m_meshe_id;
    aiNode*         m_meshe_node;
    aiMesh**        m_meshes;
    Core::Meshe*    m_target_meshe;

    void build_meshe(Core::Entity* p_entity, aiNode* p_ai_node, aiMesh** p_meshes, Core::Meshe* p_destination);

public:

    BuildMesheTask();

    void Execute(void);
    void SetEntity(Core::Entity* p_entity);
    void SetMeshesIOInfos(aiMesh** m_source_meshes, Core::Meshe* p_target_meshe, aiNode* p_meshe_node);

};

}
}

