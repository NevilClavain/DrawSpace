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

#include "runner.h"
#include "logsink.h"
#include "logconf.h"
#include "logoutput.h"
#include "logging.h"

DrawSpace::Logger::Sink runner_logger("Runner", DrawSpace::Logger::Configuration::getInstance());

using namespace DrawSpace;


void Runner::mainloop(void)
{
	Runner::getInstance()->m_cont = true;

	_DSDEBUG(runner_logger, dsstring("<<< Runner begin >>>"));

	do
	{
		auto mb_in{ &Runner::getInstance()->m_mailbox_in };
		auto mb_out{ &Runner::getInstance()->m_mailbox_out };
		const auto mbsize{ mb_in->getBoxSize() };

		if (mbsize > 0)
		{
			Task* current{ nullptr };
			do
			{
				current = mb_in->popNext<Task*>(nullptr);
				if (current)
				{
					auto task_target{ current->getTargetDescr() };
					auto task_action{ current->getActionDescr() };

					const dsstring exec_trace{ dsstring("<<< Runner executing : ") + task_action + dsstring(" ") + task_target + dsstring(" >>>") };
					_DSDEBUG(runner_logger, exec_trace);
					current->execute();
					_DSDEBUG(runner_logger, dsstring("<<< Task execution done >>>"));
					mb_out->push<std::pair<dsstring, dsstring>>(std::make_pair(task_action, task_target));
				}

			} while (current);
		}
		else
		{
			// idle for a little time...
			::Sleep(idle_duration_ms);
		}

	} while (Runner::getInstance()->m_cont);

	_DSDEBUG(runner_logger, dsstring("<<< Runner end >>>"));
}

void Runner::startup(void)
{	
	m_thread = std::make_unique<std::thread>(Runner::mainloop);
};

void Runner::join(void) const
{
	if (m_thread.get())
	{
		m_thread->join();
	}	
}
