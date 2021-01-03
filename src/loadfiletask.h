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
#include "file.h"
#include "filesystem.h"

namespace DrawSpace
{
namespace Systems
{
struct LoadFileTask : public Interface::ITask
{
private:
    // execution data
    long        m_size{ 0 };
    void*       m_data{ nullptr };
    bool        m_failure{ false };

    dsstring    m_final_asset_path;

public:

    LoadFileTask();

    void Execute(void);

    void SetFinalAssetPath(const dsstring& p_final_asset_path);
    bool Failed(void) const;
    long GetSize(void) const;
    void* GetData(void) const;
};
}
}
