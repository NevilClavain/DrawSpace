
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

#include "runner.h"

using namespace DrawSpace;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface;

void Runner::mainloop(void)
{
	Runner::GetInstance()->m_cont = true;
	do
	{
		Mailbox<ITask*>* mb_in = Runner::GetInstance()->m_mailbox_in;
		Mailbox<dsstring>* mb_out = Runner::GetInstance()->m_mailbox_out;
		int mbsize{ mb_in->GetBoxSize() };

		if (mbsize > 0)
		{
			ITask* current{ nullptr };
			do
			{
				current = mb_in->PopNext<ITask*>(nullptr);
				if (current)
				{
					auto task_id{ current->GetId() };
					current->Execute();
					mb_out->Push<dsstring>(task_id);
				}

			} while (current);
		}
		else
		{
			// idle for a little time...
			Sleep(50);
		}

	} while (Runner::GetInstance()->m_cont);

}

void Runner::Startup(Mailbox<ITask*>* p_mailbox_in, Mailbox<dsstring>* p_mailbox_out)
{
	m_mailbox_in = p_mailbox_in;
	m_mailbox_out = p_mailbox_out;
	m_thread = new std::thread(Runner::mainloop);
};

void Runner::Join(void) const
{
	if (m_thread)
	{
		m_thread->join();
	}
}
