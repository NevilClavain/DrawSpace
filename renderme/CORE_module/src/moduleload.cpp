
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


#include "moduleload.h"
#include "pimanager.h"
#include "module_root.h"

bool renderMe::core::module::load(const std::string& p_file, const std::string& p_module_instance_id, renderMe::interfaces::ModuleRoot** p_module_root)
{
    auto complete_path{ p_file };
    complete_path += ".dll";

    PlugInManager<renderMe::interfaces::ModuleRoot>::Handle pihandle{ nullptr };
    auto piManager{ PlugInManager<renderMe::interfaces::ModuleRoot>::getInstance() };

    const auto pistatus{ piManager->loadPlugin(complete_path, pihandle) };
    if (pistatus != PlugInManager<renderMe::interfaces::ModuleRoot>::Status::PIM_OK && pistatus != PlugInManager<renderMe::interfaces::ModuleRoot>::Status::PIM_OK_PIALREADYLOADED)
    {
        return false;
    }

    renderMe::interfaces::ModuleRoot* module_root{ nullptr };
    if (piManager->instanciate(pihandle, &module_root) != PlugInManager<renderMe::interfaces::ModuleRoot>::Status::PIM_OK)
    {
        return false;
    }
    *p_module_root = module_root;

	return true;
}

bool renderMe::core::module::unload(const std::string& p_file, renderMe::interfaces::ModuleRoot* p_module_root)
{
    auto complete_path{ p_file };
    complete_path += ".dll";

    auto piManager{ PlugInManager<renderMe::interfaces::ModuleRoot>::getInstance() };

    if (piManager->trashInstance(complete_path, p_module_root) != PlugInManager<renderMe::interfaces::ModuleRoot>::Status::PIM_OK)
    {
        return false;
    }

    if (piManager->unloadPlugin(complete_path) != PlugInManager<renderMe::interfaces::ModuleRoot>::Status::PIM_OK)
    {
        return false;
    }
    return true;
}

