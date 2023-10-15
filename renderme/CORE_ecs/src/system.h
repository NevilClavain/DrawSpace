
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

namespace renderMe
{
	namespace core
	{
		// fwd decl
		class Entitygraph;

		class System
		{
		public:

			System(Entitygraph& p_entitygraph, int p_executionSlot);
			~System() = default;

			virtual void run() = 0;

			int getExecutionSlot() const;
		
		protected:
			Entitygraph&	m_entitygraph;

		private:
			const int		m_executionSlot{ 0 };

		};
	}
}
