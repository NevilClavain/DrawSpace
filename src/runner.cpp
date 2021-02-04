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


#include "runner.h"
#include "logsink.h"
#include "logconf.h"
#include "logoutput.h"
#include "tracedefs.h"

DrawSpace::Logger::Sink runner_logger("Runner", DrawSpace::Logger::Configuration::GetInstance());

using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Threading;

void Runner::mainloop(void)
{
	Runner::GetInstance()->m_cont = true;

	_DSDEBUG(runner_logger, dsstring("<<< Runner begin >>>"));

	do
	{
		Mailbox<ITask*>* mb_in{ &Runner::GetInstance()->m_mailbox_in };
		Mailbox<std::pair<dsstring, dsstring>>* mb_out{ &Runner::GetInstance()->m_mailbox_out };
		int mbsize{ mb_in->GetBoxSize() };

		if (mbsize > 0)
		{
			ITask* current{ nullptr };
			do
			{
				current = mb_in->PopNext<ITask*>(nullptr);
				if (current)
				{
					auto task_target{ current->GetTargetDescr() };
					auto task_action{ current->GetActionDescr() };

					dsstring exec_trace = dsstring("<<< Runner executing : ") + task_action + dsstring(" ") + task_target + dsstring(" >>>");
					_DSDEBUG(runner_logger, exec_trace);
					current->Execute();
					_DSDEBUG(runner_logger, dsstring("<<< Task execution done >>>"));
					mb_out->Push<std::pair<dsstring, dsstring>>(std::make_pair(task_action, task_target));
				}

			} while (current);
		}
		else
		{
			// idle for a little time...
			Sleep(idle_duration_ms);
		}

	} while (Runner::GetInstance()->m_cont);

	_DSDEBUG(runner_logger, dsstring("<<< Runner end >>>"));
}

void Runner::Startup(void)
{	
	m_thread = std::make_unique<std::thread>(Runner::mainloop);
};

void Runner::Join(void) const
{
	if (m_thread.get())
	{
		m_thread->join();
	}	
}
