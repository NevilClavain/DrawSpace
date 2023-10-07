
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

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

#include "exceptions.h"
#include "component.h"
#include "allocator.h"

namespace renderMe
{
	namespace core
	{
		class ComponentContainer
		{
		public:

			ComponentContainer() = default;

			// give component container an "unique" aspect
			ComponentContainer(const ComponentContainer&) = delete;
			ComponentContainer(ComponentContainer&&) = delete;
			ComponentContainer& operator=(const ComponentContainer& t) = delete;

			~ComponentContainer() = default;

			static int getUIDCount() { return m_uid_count; };

			template<typename T, class... Args>
			void addComponent(const std::string& p_id, Args&&... p_args)
			{
				if (m_components.count(p_id) > 0)
				{
					_EXCEPTION("Component with same id already exists : " + p_id);
				}

				const auto newcomp{ _NEW_CHUNK_WITH_COMMENT(Component<T>, Component<T>, p_id) };
				newcomp->makePurpose((std::forward<Args>(p_args))...);
				m_components[p_id] = newcomp;

				// ajout dans m_components_by_type

				const auto tid{ typeid(T).hash_code() };
				m_components_by_type[tid].push_back(newcomp);

				newcomp->setUID( m_uid_count++ );
			}

			template<typename T>
			void removeComponent(const std::string& p_id)
			{
				if (0 == m_components.count(p_id))
				{
					_EXCEPTION("Component id not registered in this aspect : " + p_id);
				}

				auto comp{ static_cast<Component<T>*>(m_components.at(p_id)) };
				// suppression dans m_components_by_type
				const auto tid{ typeid(T).hash_code() };
				for (auto it = m_components_by_type.at(tid).begin(); it != m_components_by_type.at(tid).end(); ++it)
				{
					if (m_components.at(p_id) == *it)
					{
						// on a trouve le composant en question ! suppression...
						m_components_by_type.at(tid).erase(it);
						break;
					}
				}
				m_components.erase(p_id);
				//
				_DELETE_CHUNK_(comp);

				m_uid_count--;
			}

			
			template<typename T>
			Component<T>* getComponent(const std::string& p_id) const
			{
				if (0 == m_components.count(p_id))
				{
					return nullptr;
				}
				const auto comp{ static_cast<Component<T>*>(m_components.at(p_id)) };
				return comp;
			}

			
			template<typename T>
			void getComponentsByType(ComponentList<T>& p_outlist) const
			{
				const auto tid{ typeid(T).hash_code() };
				if (m_components_by_type.count(tid) > 0)
				{
					const auto& list{ m_components_by_type.at(tid) };
					for (auto& e : list)
					{
						p_outlist.push_back(static_cast<Component<T>*>(e));
					}
				}
			}
			
		protected:
			static int													m_uid_count;

			// map globale, regroupant les composants par id...
			std::unordered_map<std::string, ComponentBase*>				m_components;

			// 2eme map pour regrouper les composants en fct de leur type
			std::unordered_map<size_t, std::vector<ComponentBase*>>		m_components_by_type;
		};
	}
}