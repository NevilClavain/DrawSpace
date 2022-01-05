/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2022
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

#include "updatequeuetask.h"
#include "renderingqueue.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;


UpdateQueueTask::UpdateQueueTask() : ITask("", "")
{
}

void UpdateQueueTask::Execute(void)
{
	for (auto& queue : m_renderingqueues)
	{
		if (m_disable_opt)
		{
			queue->UpdateOutputQueueNoOpt();
		}
		else
		{
			queue->UpdateOutputQueue();
		}
		
	}
}

void UpdateQueueTask::SetRenderingQueue(const std::vector<DrawSpace::Core::RenderingQueue*> p_renderingqueues)
{
	m_renderingqueues = p_renderingqueues;
}

void UpdateQueueTask::DisableOpt(bool p_disable)
{
	m_disable_opt = p_disable;
}
