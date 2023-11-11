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
#include <memory>

#include "eventsource.h"
#include "singleton.h"
#include "system.h"
#include "exceptions.h"

namespace renderMe
{
	namespace core
	{
		enum class SystemEngineEvents
		{
			SYSTEM_ADDED,
		};

		//singleton and events source
		class SystemEngine : public property::EventSource<SystemEngineEvents, const System&>, public property::Singleton<SystemEngine>
		{
		public:
			SystemEngine() = default;
			~SystemEngine() = default;

			template<typename T, class... Args>
			void makeSystem(int p_executionslot, Args&&... p_args);
			
			void run();

			core::System* getSystem(int p_executionslot) const;

		private:
			std::map<int, std::unique_ptr<core::System>> m_systems;
		};


		template<typename T, class... Args>
		inline void SystemEngine::makeSystem(int p_executionslot, Args&&... p_args)
		{
			const auto place{ m_systems.emplace(p_executionslot, std::make_unique<T>((std::forward<Args>(p_args))...)) };

			if (!place.second) {
				_EXCEPTION("system already registered for this slot : " + std::to_string(p_executionslot))
			}

			for (const auto& call : m_callbacks)
			{
				call(SystemEngineEvents::SYSTEM_ADDED, *m_systems.at(p_executionslot).get());
			}
		}
	}
}
