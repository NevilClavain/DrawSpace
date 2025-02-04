
/* -*-LIC_BEGIN-*- */
/*
*
* MaGE rendering framework
* Emmanuel Chaumont Copyright (c) 2013-2025
*
* This file is part of MaGE.
*
*    MaGE is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    MaGE is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with MaGE.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#pragma once

#include <map>
#include "componentcontainer.h"
#include "st_tree.h"

namespace mage
{
	namespace core
	{
		class Entity : public st_tree::tree<core::Entity*>::node_type
		{
		public:
			Entity(const std::string& p_id, Entity* p_parent = nullptr) :
			m_id(p_id),
			m_parent(p_parent)
			{
			}

			// give an "unique" aspect
			Entity(const Entity&) = delete;
			Entity(Entity&&) = delete;
			Entity& operator=(const Entity& t) = delete;

			~Entity() = default;

			std::string getId() const
			{
				return m_id;
			}

			ComponentContainer& makeAspect(int p_aspect)
			{
				if (m_aspects.count(p_aspect))
				{
					_EXCEPTION("Aspect already registered: " + std::to_string(p_aspect))
				}
				m_aspects[p_aspect]; // instantiate entry
				return m_aspects.at(p_aspect);
			}

			void removeAspect(int p_aspect)
			{
				if (m_aspects.count(p_aspect))
				{
					m_aspects.at(p_aspect).removeComponent<int>("aspect");
					m_aspects.erase(p_aspect);
				}
				else
				{
					_EXCEPTION("removeAspect: unknown aspect : " + std::to_string(p_aspect))
				}
			}

			bool hasAspect(int p_aspect) const
			{
				return (m_aspects.count(p_aspect) > 0);
			}

			ComponentContainer& aspectAccess(int p_aspect)
			{			
				if(0 == m_aspects.count(p_aspect))
				{ 
					_EXCEPTION("aspectAccess: unknown aspect : " + std::to_string(p_aspect))
				}
				return m_aspects.at(p_aspect);
			}

			const ComponentContainer& aspectAccess(int p_aspect) const
			{
				if (0 == m_aspects.count(p_aspect))
				{
					_EXCEPTION("aspectAccess: unknown aspect : " + std::to_string(p_aspect))
				}
				return m_aspects.at(p_aspect);
			}


			Entity* getParent() const
			{
				return m_parent;
			}

		private:
			std::map<int, ComponentContainer>	m_aspects;
			const std::string					m_id;

			Entity*								m_parent{ nullptr };
		};
	}
}
