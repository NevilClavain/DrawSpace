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
#include "task.h"

namespace DrawSpace
{
namespace Core
{
class RenderingQueue; // fwd class declaration

struct UpdateQueueTask : public Interface::ITask
{
private:
    // execution data
    std::vector<DrawSpace::Core::RenderingQueue*> m_renderingqueues;
    bool                                          m_disable_opt{ false };

public:

    UpdateQueueTask( void );
    void Execute(void);

    void SetRenderingQueue(const std::vector<DrawSpace::Core::RenderingQueue*> p_renderingqueues);
    void DisableOpt(bool p_disable);
};

}
}
