
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


#include "moduleload.h"
#include "pimanager.h"
#include "module_root.h"

bool mage::core::module::load(const std::string& p_file, const std::string& p_module_instance_id, mage::interfaces::ModuleRoot** p_module_root)
{
    auto complete_path{ p_file };
    complete_path += ".dll";

    PlugInManager<mage::interfaces::ModuleRoot>::Handle pihandle{ nullptr };
    auto piManager{ PlugInManager<mage::interfaces::ModuleRoot>::getInstance() };

    const auto pistatus{ piManager->loadPlugin(complete_path, pihandle) };
    if (pistatus != PlugInManager<mage::interfaces::ModuleRoot>::Status::PIM_OK && pistatus != PlugInManager<mage::interfaces::ModuleRoot>::Status::PIM_OK_PIALREADYLOADED)
    {
        return false;
    }

    mage::interfaces::ModuleRoot* module_root{ nullptr };
    if (piManager->instanciate(pihandle, &module_root) != PlugInManager<mage::interfaces::ModuleRoot>::Status::PIM_OK)
    {
        return false;
    }
    *p_module_root = module_root;

	return true;
}

bool mage::core::module::unload(const std::string& p_file, mage::interfaces::ModuleRoot* p_module_root)
{
    auto complete_path{ p_file };
    complete_path += ".dll";

    auto piManager{ PlugInManager<mage::interfaces::ModuleRoot>::getInstance() };

    if (piManager->trashInstance(complete_path, p_module_root) != PlugInManager<mage::interfaces::ModuleRoot>::Status::PIM_OK)
    {
        return false;
    }

    if (piManager->unloadPlugin(complete_path) != PlugInManager<mage::interfaces::ModuleRoot>::Status::PIM_OK)
    {
        return false;
    }
    return true;
}

