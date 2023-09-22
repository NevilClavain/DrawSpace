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
#include <functional>

namespace renderMe
{
	// fwd decl
	namespace core
	{
		class Runner;
	}

	namespace property
	{
		struct AsyncTask
		{
		public:
			AsyncTask(const std::string& p_action_descr, const std::string& p_target_descr) :
				m_action_descr(p_action_descr),
				m_target_descr(p_target_descr)
			{
			};

			AsyncTask() = delete;
			~AsyncTask() = default;
			
			std::string getTargetDescr(void) const
			{
				return m_target_descr;
			};

			std::string getActionDescr(void) const
			{
				return m_action_descr;
			};

			void setTargetDescr(const std::string& p_target_descr)
			{
				m_target_descr = p_target_descr;
			}

			void setActionDescr(const std::string& p_action_descr)
			{
				m_action_descr = p_action_descr;
			}

		private:
			std::string	m_action_descr;
			std::string	m_target_descr;

			virtual void execute(core::Runner* p_runner) = 0;

			friend class core::Runner;

		};
	}

	namespace core
	{
		template <typename... Args>
		class SimpleAsyncTask : public renderMe::property::AsyncTask
		{
		public:

			SimpleAsyncTask() = default;
			~SimpleAsyncTask() = default;

			using bind_type = decltype(std::bind(std::declval<std::function<void(Args...)>>(), std::declval<Args>()...));

			template <typename... ConstrArgs>
			SimpleAsyncTask(const std::string& p_action_descr, const std::string& p_target_descr, std::function<void(Args...)> f, ConstrArgs&&... args) : AsyncTask(p_action_descr, p_target_descr),
				m_bind(std::forward<std::function<void(Args...)>>(f), std::forward<ConstrArgs>(args)...)
			{
			}

			void execute(renderMe::core::Runner* p_runner)
			{
				m_bind();
			}

		private:
			bind_type m_bind;
		};
	}
}
