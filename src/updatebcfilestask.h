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

#include "task.h"

namespace DrawSpace
{
namespace Systems
{
struct UpdateBCFilesTask : public Interface::ITask
{
private:

    static const dsstring bcCacheName;
    static const dsstring bcMd5FileName;
    static const dsstring bcCodeFileName;
    static const dsstring sourceFileNameExt;

    // shader complete source text
    long        m_text_size;
    void*       m_text{ nullptr };
    
    // execution data
    dsstring    m_shader_id;
    dsstring    m_hash;

    long        m_bc_length{ 0 };
    void*       m_bc{ nullptr };

public:

    UpdateBCFilesTask();

    void SetShaderText(void* p_text, long p_text_size);

    void Execute(void);
    void SetBC(void* p_bc, long p_bc_length);
    void SetHash(const dsstring& p_hash);
    void SetShaderId(const dsstring& p_shader_id);
};
}
}
