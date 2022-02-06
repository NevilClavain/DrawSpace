/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2022
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

#include "luaext_load.h"
#include "renderer.h"
#include "pimanager.h"
#include "plugin.h"
#include "exceptions.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;


LuaExtension* LuaExtLoad::RegisterLuaExtension(const dsstring& p_file, lua_State* p_L, dsstring& p_description)
{
    dsstring complete_path = p_file;

#ifdef _FROMCMAKE
    complete_path += "_luaext.dll";
#else
#ifdef _DEBUG
    complete_path += "_luaext.dll";
#else
    complete_path += "_luaext_r.dll";
#endif
#endif

    DrawSpace::Utils::PlugInManager<LuaExtension>::Handle pihandle{ nullptr };
    LuaExtension* lua_extension{ nullptr };

    PluginManagerStatus pistatus = DrawSpace::Utils::PlugInManager<LuaExtension>::LoadPlugin(complete_path.c_str(), pihandle);
    if (pistatus != PluginManagerStatus::PIM_OK)
    {
        _DSEXCEPTION("Error while loading plugin " + complete_path)
    }

    if (DrawSpace::Utils::PlugInManager<LuaExtension>::Instanciate(pihandle, &lua_extension) != PluginManagerStatus::PIM_OK)
    {
        _DSEXCEPTION("Error while instanciating lua ext of plugin " + complete_path)
    }

    lua_extension->UpdateRenderer(DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface);
    lua_extension->Register(p_L);

    dsstring description{ lua_extension->Description() };
    p_description = description;

    return lua_extension;
}
