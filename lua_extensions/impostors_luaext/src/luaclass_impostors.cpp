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


#include "luacontext.h"
#include "luaclass_impostors.h"
#include "luaclass_entity.h"
#include "renderingaspect.h"
#include "impostorsrenderingaspectimpl.h"
#include "luaclass_renderpassnodegraph.h"

using namespace DrawSpace;
using namespace DrawSpace::Aspect;


const char LuaClass_Impostors::className[] = "Impostors";
const Luna<LuaClass_Impostors>::RegType LuaClass_Impostors::methods[] =
{
	{ "attach_toentity", &LuaClass_Impostors::LUA_attachtoentity },
    { "detach_fromentity", &LuaClass_Impostors::LUA_detachfromentity },
    { "set_passforrenderid", &LuaClass_Impostors::LUA_setPassForRenderId },
    { "register_to_rendering", &LuaClass_Impostors::LUA_registertorendering },
    { "unregister_from_rendering", &LuaClass_Impostors::LUA_unregisterfromrendering },

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
    
    if (argc < 1)
    {
        LUA_ERROR("Impostors::attach_toentity : argument(s) missing");
    }
    
    LuaClass_Entity* lua_ent = Luna<LuaClass_Entity>::check(p_L, 1);
    
    DrawSpace::Core::Entity& entity = lua_ent->GetEntity();
    RenderingAspect* rendering_aspect = entity.GetAspect<RenderingAspect>();

    if (NULL == rendering_aspect)
    {
        LUA_ERROR("Impostors::attach_toentity : entity has no rendering aspect!");
    }
    
    m_entity_rendering_aspect = rendering_aspect;
    m_entity = &entity;
    
    m_impostors_render = _DRAWSPACE_NEW_(ImpostorsRenderingAspectImpl, ImpostorsRenderingAspectImpl);
    m_entity_rendering_aspect->AddImplementation(m_impostors_render, nullptr);
    
    return 0;
}

int LuaClass_Impostors::LUA_detachfromentity(lua_State* p_L)
{
    if (!m_entity)
    {
        LUA_ERROR("Impostors::detach_fromentity : argument(s) missing");
    }
    if (m_impostors_render)
    {
        _DRAWSPACE_DELETE_(m_impostors_render);
    }
    LUA_TRY
    {
        m_entity_rendering_aspect->RemoveImplementation(m_impostors_render);

    } LUA_CATCH;

    m_entity_rendering_aspect = nullptr;
    m_entity = nullptr;
    return 0;
}

int LuaClass_Impostors::LUA_setPassForRenderId(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("Impostors::set_passforrenderid : argument(s) missing");
    }

    dsstring rc_id = luaL_checkstring(p_L, 1);
    dsstring pass_id = luaL_checkstring(p_L, 2);

    m_rcname_to_passes[rc_id].push_back(pass_id);

    return 0;
}

int LuaClass_Impostors::LUA_registertorendering(lua_State* p_L)
{
    if (NULL == m_impostors_render)
    {
        LUA_ERROR("Impostors::register_to_rendering : no impostors rendering aspect impl created");
    }

    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("Impostors::register_to_rendering : argument(s) missing");
    }

    LuaClass_RenderPassNodeGraph* lua_rg = Luna<LuaClass_RenderPassNodeGraph>::check(p_L, 1);

    m_impostors_render->RegisterToRendering(lua_rg->GetRenderGraph());
    return 0;
}

int LuaClass_Impostors::LUA_unregisterfromrendering(lua_State* p_L)
{
    if (NULL == m_impostors_render)
    {
        LUA_ERROR("Impostors::unregister_from_rendering : no impostors rendering aspect impl created");
    }

    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("Impostors::unregister_from_rendering : argument(s) missing");
    }

    LuaClass_RenderPassNodeGraph* lua_rg = Luna<LuaClass_RenderPassNodeGraph>::check(p_L, 1);

    m_impostors_render->UnregisterFromRendering(lua_rg->GetRenderGraph());
    return 0;
}