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
#include <assimp/scene.h>           // Output data structure

namespace DrawSpace
{
namespace Systems
{
struct LoadAnimationsTask : public Interface::ITask
{
private:

    const aiScene*              m_scene{ nullptr };
    Aspect::AnimationsAspect*   m_anims_aspect{ nullptr };

    void load_scene_nodes_hierachy(aiNode* p_ai_node, std::map<dsstring, Aspect::AnimationsAspect::Node>& p_node_table);
    void load_animations(const aiScene* p_scene, Aspect::AnimationsAspect* p_anims_aspect);

public:

    LoadAnimationsTask();

    void Execute(void);
    void SetScene(const aiScene* p_scene);
    void SetAnimAspect(Aspect::AnimationsAspect* p_aspect);
};

}
}
