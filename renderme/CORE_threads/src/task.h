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

namespace renderMe
{
	struct Task
	{
	public:
		Task(const std::string& p_action_descr, const std::string& p_target_descr) :
		m_action_descr(p_action_descr),
		m_target_descr(p_target_descr)
		{
		};
		virtual void execute(void) = 0;

		std::string getTargetDescr(void) const
		{
			return m_target_descr;
		};

		std::string getActionDescr(void) const
		{
			return m_action_descr;
		};

		inline void setTargetDescr(const std::string& p_target_descr)
		{
			m_target_descr = p_target_descr;
		}

		inline void setActionDescr(const std::string& p_action_descr)
		{
			m_action_descr = p_action_descr;
		}

	private:
		std::string	m_action_descr;
		std::string	m_target_descr;

	};
}
