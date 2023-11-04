
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

#pragma once

#include <string>
#include "tvector.h"

namespace renderMe
{
	namespace rendering
	{
		class Queue
		{
		public:

			enum class Purpose
			{
				UNDEFINED,
				SCREEN_RENDERING,
				INTERMEDIATE_RENDERING
			};

			enum class State
			{
				WAIT_INIT,
				READY,
				ERROR_ORPHAN
			};

			Queue(const std::string& p_name);
			~Queue() = default;

			std::string			getName() const;
			Purpose				getPurpose() const;
			State				getState() const;

			void				setState(State p_newstate);
			void				setPurpose(Purpose p_purpose);

			void				enableTargetClearing(bool p_enable);
			void				setTargetClearColor(const core::RGBAColor& p_color);

			bool				getTargetClearing() const;
			core::RGBAColor		getTargetClearColor() const;

		private:
			std::string						m_name;
			Purpose							m_purpose{ Purpose::UNDEFINED };
			State							m_state{ State::WAIT_INIT };

			bool							m_clear_target{ false };

			core::RGBAColor					m_target_clear_color;
			
		};
	}
}
