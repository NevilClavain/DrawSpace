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
#include "task.h"
#include "mailbox.h"
#include "crtp_singleton.h"
#include "task.h"

namespace DrawSpace
{
namespace Threading
{
struct Runner : public DrawSpace::Singleton<Runner>
{
public:	

	Mailbox<Interface::ITask*>						m_mailbox_in;
	Mailbox<std::pair<dsstring, dsstring>>			m_mailbox_out;
private:

	mutable std::unique_ptr<std::thread>			m_thread;
	bool											m_cont;

	static void mainloop(void);

	static const unsigned int idle_duration_ms{ 50 };

public:

	void Startup(void);
	void Join(void) const;


	friend struct RunnerKiller;
};

struct RunnerKiller : public Interface::ITask
{
	RunnerKiller(void) : Interface::ITask("KILL", "Runner")
	{
	}

	inline void Execute(void)
	{
		Runner::getInstance()->m_cont = false;
	}
};
}
}
