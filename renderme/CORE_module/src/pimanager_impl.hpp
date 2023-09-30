
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

using namespace renderMe::core;

template <typename base>
inline typename PlugInManager<base>::Status PlugInManager<base>::loadPlugin(const std::string& p_path, Handle& p_handle)
{
    
    const auto it{ m_libs.find(p_path) };

    if (m_libs.end() == it)
    {
        const auto hMod{ LoadLibraryA(p_path.c_str()) };
        if (nullptr == hMod)
        {
            return Status::PIM_FAIL_PILOADING;
        }
        else
        {
            PluginInfos pii;
            pii.refcount = 1;
            pii.handle = hMod;
            pii.path = p_path;

            m_libs[p_path] = pii;

            p_handle = hMod;
            return Status::PIM_OK;
        }
    }
    else
    {
        // plugin already loaded
        p_handle = it->second.handle;
        it->second.refcount++;

        return Status::PIM_OK_PIALREADYLOADED;
    }   
    return Status::PIM_OK;
}

template <typename base>
inline typename PlugInManager<base>::Status PlugInManager<base>::unloadPlugin(const std::string& p_path)
{    
    const auto it{ m_libs.find(p_path) };
    if (it == m_libs.end())
    {
        return Status::PIM_FAIL_UNKNOWN;
    }

    else
    {
        it->second.refcount--;
        if (0 == it->second.refcount) // si plus aucun hub ne fait reference a ce plugin
        {
            const auto pii{ it->second };
            m_libs.erase(it);
            FreeLibrary(pii.handle);
        }
    }    
    return Status::PIM_OK;
}

template <typename base>
inline typename PlugInManager<base>::Status PlugInManager<base>::instanciate(Handle p_handle, base** p_inst)
{
    
    // TODO  : verifier que le handle fourni est bien dans m_libs; sinon retour PIM_FAIL_UNKNOWN
    const auto proc{ GetProcAddress(p_handle, moduleFactorySymbol.c_str()) };

    if (nullptr == proc)
    {
        return Status::PIM_FAIL_ENTRYPOINTNOTFOUND;
    }

    const auto factory{ (Factory)proc };
    *p_inst = (*factory)();    
    return Status::PIM_OK;
}

template <typename base>
inline typename PlugInManager<base>::Status PlugInManager<base>::trashInstance(const std::string& p_path, base* p_inst)
{
    
    const auto it{ m_libs.find(p_path) };
    if (m_libs.end() == it)
    {
        return Status::PIM_FAIL_UNKNOWN;
    }

    // TODO  : verifier que le handle fourni est bien dans m_libs; sinon retour PIM_FAIL_UNKNOWN
    const auto proc{ GetProcAddress(it->second.handle, moduleTrashSymbol.c_str()) };
    if (nullptr == proc)
    {
        return Status::PIM_FAIL_ENTRYPOINTNOTFOUND;
    }

    const auto trash{ (Trash)proc };
    (*trash)(p_inst);
    return Status::PIM_OK;   
}



