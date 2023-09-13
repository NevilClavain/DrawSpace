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

#include <string>
#include <thread>
#include <utility> 
#include "mailbox.h"
#include "asynctask.h"
#include "eventsource.h"

namespace renderMe
{
	namespace core
	{
		enum class RunnerEvent
		{
			TASK_DONE
		};


		class Runner : public renderMe::property::EventSource<RunnerEvent, const std::string&, const std::string&>
		{
		public:

			Mailbox<property::AsyncTask*>					m_mailbox_in;
			Mailbox<std::pair<std::string, std::string>>	m_mailbox_out;

			void startup(void);
			void join(void);

		private:

			mutable std::unique_ptr<std::thread>			m_thread;
			bool											m_cont;

			static void mainloop(Runner* p_runner);

			static constexpr unsigned int idle_duration_ms{ 50 };
			friend struct RunnerKiller;
		};

		struct RunnerKiller : public property::AsyncTask
		{
			RunnerKiller(void) : AsyncTask("KILL", "Runner")
			{
			}

			void execute(core::Runner* p_runner)
			{
				p_runner->m_cont = false;
			}
		};
	}
}
