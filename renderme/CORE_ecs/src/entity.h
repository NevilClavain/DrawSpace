
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

#include <map>
#include "componentcontainer.h"

namespace renderMe
{
	namespace core
	{
		class entity
		{
		public:
			entity(const std::string& p_id) :
			m_id(p_id)
			{

			}

			~entity() = default;

			std::string getId() const
			{
				return m_id;
			}

			void makeAspect(int p_aspect)
			{
				if (m_aspects.count(p_aspect))
				{
					_EXCEPTION("Aspect already registered: " + std::to_string(p_aspect))
				}

				// mark aspect id in the component container
				m_aspects[p_aspect].addComponent<int>("aspect", p_aspect);
			}

			void removeAspect(int p_aspect)
			{
				if (m_aspects.count(p_aspect))
				{
					m_aspects.erase(p_aspect);
				}
				else
				{
					_EXCEPTION("removeAspect: unknown aspect : " + std::to_string(p_aspect))
				}
			}

			ComponentContainer& aspectAccess(int p_aspect)
			{			
				if(0 == m_aspects.count(p_aspect))
				{ 
					_EXCEPTION("aspectAccess: unknown aspect : " + std::to_string(p_aspect))
				}
				return m_aspects.at(p_aspect);
			}

		private:
			std::map<int, ComponentContainer> m_aspects;
			const std::string				  m_id;
		};
	}
}
