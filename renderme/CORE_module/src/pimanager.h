
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

#include <Windows.h>
#include <string>
#include <unordered_map>
#include "singleton.h"

namespace mage
{
	namespace core
	{
		template <typename base>
		class PlugInManager : public property::Singleton<PlugInManager<base>>
		{
		public:
			using Handle = HMODULE;

			enum class Status
			{
				PIM_OK,
				PIM_OK_PIALREADYLOADED,
				PIM_FAIL_PILOADING,
				PIM_FAIL_PIUNLOADING,
				PIM_FAIL_UNKNOWN,
				PIM_FAIL_ENTRYPOINTNOTFOUND
			};
			
			PlugInManager() = default;
			~PlugInManager() = default;

			Status loadPlugin(const std::string& p_path, Handle& p_handle);
			Status unloadPlugin(const std::string& p_path);
			Status instanciate(Handle p_handle, base** p_inst);
			Status trashInstance(const std::string& p_path, base* p_inst);

		private:

			static inline const std::string moduleFactorySymbol{ "moduleFactory" };
			static inline const std::string moduleTrashSymbol{ "moduleTrash" };

			struct PluginInfos
			{
				Handle          handle;
				std::string		path;
				long            refcount;
			};

			using Factory = base * (*)(void);
			using Trash = void  (*)(base*);

			using LibList = std::unordered_map<std::string, PluginInfos>;

			LibList m_libs;			
		};
	}
}

#include "pimanager_impl.hpp"