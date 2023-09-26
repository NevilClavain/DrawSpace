
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

#include <memory>
#include <vector>

namespace renderMe
{
	namespace core
	{
		class ComponentBase
		{
		public:
			~ComponentBase() = default;

			void setUID(int p_uid)
			{
				m_uid = p_uid;
			}

			int getUID() const
			{
				return m_uid;
			}
			
		protected:
			ComponentBase() = default;			
			int m_uid;
		};

		template<typename T>
		using ComponentPurpose = std::unique_ptr<T>;

		template<typename T>
		class Component : public ComponentBase
		{
		public:

			Component() = default;
			virtual ~Component() = default;

			template<class... Args>
			void makePurpose(Args&&... p_args)
			{
				m_purpose = std::make_unique<T>((std::forward<Args>(p_args))...);
			}

			virtual T& getPurpose(void) const
			{
				return *(m_purpose.get());
			}

		private:
			ComponentPurpose<T>       m_purpose;
		};

		template<typename T>
		using ComponentList = std::vector<Component<T>*>;
	}
}