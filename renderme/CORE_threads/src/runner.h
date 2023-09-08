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

#include <thread>
#include "task.h"
#include "mailbox.h"
#include "singleton.h"
#include "task.h"

namespace renderMe
{
	namespace core
	{
		struct Runner : public property::Singleton<Runner>
		{
		public:

			Mailbox<Task*>									m_mailbox_in;
			Mailbox<std::pair<std::string, std::string>>	m_mailbox_out;

			void startup(void);
			void join(void) const;

		private:

			mutable std::unique_ptr<std::thread>			m_thread;
			bool											m_cont;

			static void mainloop(void);

			static constexpr unsigned int idle_duration_ms{ 50 };
			friend struct RunnerKiller;
		};

		struct RunnerKiller : public Task
		{
			RunnerKiller(void) : Task("KILL", "Runner")
			{
			}

			void execute(void)
			{
				Runner::getInstance()->m_cont = false;
			}
		};
	}
}
