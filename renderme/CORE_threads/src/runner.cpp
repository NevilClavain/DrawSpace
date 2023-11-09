/* -*-LIC_BEGIN-*- */
/*
*
* renderMe grafx framework
* Emmanuel Chaumont Copyright (c) 2013-2023
*
* This file is part of renderMe.
*
*    renderMe is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    renderMe is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with renderMe.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#include <iostream>

#include <string>
#include <chrono>

#include "runner.h"

using namespace renderMe;
using namespace renderMe::core;
using namespace renderMe::property;

void Runner::mainloop(Runner* p_runner)
{
	const auto runnerInstance{ p_runner };

	runnerInstance->m_cont = true;

	do
	{
		auto mb_in{ &runnerInstance->m_mailbox_in };
		auto mb_out{ &runnerInstance->m_mailbox_out };
		const auto mbsize{ mb_in->getBoxSize() };

		if (mbsize > 0)
		{
			AsyncTask* current{ nullptr };
			do
			{
				current = mb_in->popNext<AsyncTask*>(nullptr);
				if (current)
				{	
					auto task_target{ current->getTargetDescr() };
					auto task_action{ current->getActionDescr() };

					current->execute(runnerInstance);
					mb_out->push<std::pair<std::string, std::string>>(std::make_pair(task_action, task_target));
				}

			} while (current);
		}
		else
		{
			// idle for a little time...

			std::this_thread::sleep_for(std::chrono::milliseconds(idle_duration_ms));
		}

	} while (runnerInstance->m_cont);
	
}

void Runner::startup(void)
{	
	m_thread = std::make_unique<std::thread>(Runner::mainloop, this);
};

void Runner::join(void)
{
	if (m_thread.get())
	{
		m_thread->join();

		const auto mb_size{ m_mailbox_out.getBoxSize() };

		for (int i = 0; i < mb_size; i++)
		{
			const auto task_descr{ m_mailbox_out.popNext<std::pair<std::string, std::string>>(std::make_pair<std::string, std::string>("","")) };
			for (const auto& call : m_callbacks)
			{
				call(RunnerEvent::TASK_DONE, task_descr.first, task_descr.second);
			}
		}
	}	
}
