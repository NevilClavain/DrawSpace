/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2021                     
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


#include "luaclass_impostors.h"
#include "luacontext.h"
#include "luaclass_entity.h"

const char LuaClass_Impostors::className[] = "Impostors";
const Luna<LuaClass_Impostors>::RegType LuaClass_Impostors::methods[] =
{
	{ "attach_toentity", &LuaClass_Impostors::LUA_attachtoentity },
	{ 0, 0 }
};

LuaClass_Impostors::LuaClass_Impostors( lua_State* p_L )
{
}

LuaClass_Impostors::~LuaClass_Impostors( void )
{
}

int LuaClass_Impostors::LUA_attachtoentity(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    /*
    if (argc < 1)
    {
        LUA_ERROR("Impostors::attach_toentity : argument(s) missing");
    }
    */

    //LuaClass_Entity* lua_ent = Luna<LuaClass_Entity>::check(p_L, 1);

    /*
    DrawSpace::Core::Entity& entity = lua_ent->GetEntity();
    RenderingAspect* rendering_aspect = entity.GetAspect<RenderingAspect>();

    if (NULL == rendering_aspect)
    {
        LUA_ERROR("MesheRendering::attach_toentity : entity has no rendering aspect!");
    }

    m_entity_rendering_aspect = rendering_aspect;
    m_entity = &entity;

    m_meshe_render = _DRAWSPACE_NEW_(DrawSpace::AspectImplementations::MesheRenderingAspectImpl, DrawSpace::AspectImplementations::MesheRenderingAspectImpl);
    m_entity_rendering_aspect->AddImplementation(m_meshe_render, NULL);
    */
    return 0;
}
