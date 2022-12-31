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

#include "luacontext.h"
#include "luaclass_renderstatesset.h"

const char LuaClass_RenderStatesSet::className[] = "RenderStatesSet";
const Luna<LuaClass_RenderStatesSet>::RegType LuaClass_RenderStatesSet::methods[] =
{
    { "add_renderstate_in", &LuaClass_RenderStatesSet::LUA_addrenderstatein },
    { "add_renderstate_out", &LuaClass_RenderStatesSet::LUA_addrenderstateout },
    { "update_renderstate_in", &LuaClass_RenderStatesSet::LUA_updaterenderstatein },
    { "update_renderstate_out", &LuaClass_RenderStatesSet::LUA_updaterenderstateout },
    { "clear_renderstate_in_extended_args", &LuaClass_RenderStatesSet::LUA_updaterenderstateout },
    { "clear_renderstate_in_extended_args", &LuaClass_RenderStatesSet::LUA_pushrenderstateinextendedargs },
    { "push_renderstate_in_extendedargs", &LuaClass_RenderStatesSet::LUA_pushrenderstateinextendedargs },
    { "push_renderstate_out_extendedargs", &LuaClass_RenderStatesSet::LUA_pushrenderstateoutextendedargs },

    

	{ 0, 0 }
};

LuaClass_RenderStatesSet::LuaClass_RenderStatesSet( lua_State* p_L )
{
}

LuaClass_RenderStatesSet::~LuaClass_RenderStatesSet( void )
{
}

int LuaClass_RenderStatesSet::LUA_addrenderstatein( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 2 )
	{		
        LUA_ERROR( "RenderStatesSet::add_renderstate_in : argument(s) missing" );
	}

    const auto ope{ static_cast<DrawSpace::Core::RenderState::Operation>(luaL_checkint(p_L, 1)) };
    const auto arg{ luaL_checkstring(p_L, 2) };

    m_renderstatesset.AddRenderStateIn( DrawSpace::Core::RenderState( ope, arg ) );
    return 0;
}

int LuaClass_RenderStatesSet::LUA_addrenderstateout( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 2 )
	{		
        LUA_ERROR( "RenderStatesSet::add_renderstate_out : argument(s) missing" );
	}

    const auto ope{ static_cast<DrawSpace::Core::RenderState::Operation>(luaL_checkint(p_L, 1)) };
    const auto arg{ luaL_checkstring(p_L, 2) };

    m_renderstatesset.AddRenderStateOut( DrawSpace::Core::RenderState( ope, arg ) );
    
    return 0;
}

int LuaClass_RenderStatesSet::LUA_updaterenderstatein( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 3 )
	{		
        LUA_ERROR( "RenderStatesSet::update_renderstate_in : argument(s) missing" );
	}

    const auto index{ luaL_checkint(p_L, 1) };
    const auto ope{ static_cast<DrawSpace::Core::RenderState::Operation>(luaL_checkint(p_L, 2)) };
    const auto arg{ luaL_checkstring(p_L, 3) };

    m_renderstatesset.UpdateRenderStateIn( index, DrawSpace::Core::RenderState( ope, arg ) );

    return 0;
}

int LuaClass_RenderStatesSet::LUA_updaterenderstateout( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 3 )
	{		
        LUA_ERROR( "RenderStatesSet::update_renderstate_out : argument(s) missing" );
	}

    const auto index{ luaL_checkint(p_L, 1) };
    const auto ope{ static_cast<DrawSpace::Core::RenderState::Operation>(luaL_checkint(p_L, 2)) };
    const auto arg{ luaL_checkstring(p_L, 3) };

    m_renderstatesset.UpdateRenderStateOut( index, DrawSpace::Core::RenderState( ope, arg ) );

    return 0;
}

int LuaClass_RenderStatesSet::LUA_clearrenderstateinextendedargs(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("RenderStatesSet::clear_renderstate_in_extended_args : argument(s) missing");
    }

    const auto index{ luaL_checkint(p_L, 1) };
    m_renderstatesset.ClearRenderStateInExtendedArgs(index);

    return 0;
}

int LuaClass_RenderStatesSet::LUA_clearrenderstateoutextendedargs(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("RenderStatesSet::clear_renderstate_out_extended_args : argument(s) missing");
    }

    const auto index{ luaL_checkint(p_L, 1) };
    m_renderstatesset.ClearRenderStateOutExtendedArgs(index);

    return 0;
}

int LuaClass_RenderStatesSet::LUA_pushrenderstateinextendedargs(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("RenderStatesSet::push_renderstate_in_extendedargs : argument(s) missing");
    }

    const auto index{ luaL_checkint(p_L, 1) };
    const auto arg{ luaL_checkstring(p_L, 2) };
    m_renderstatesset.PushRenderStateInExtendedArgs(index, arg);

    return 0;
}

int LuaClass_RenderStatesSet::LUA_pushrenderstateoutextendedargs(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("RenderStatesSet::push_renderstate_out_extendedargs : argument(s) missing");
    }

    const auto index{ luaL_checkint(p_L, 1) };
    const auto arg{ luaL_checkstring(p_L, 2) };
    m_renderstatesset.PushRenderStateOutExtendedArgs(index, arg);

    return 0;
}

DrawSpace::Core::RenderStatesSet LuaClass_RenderStatesSet::GetRenderStatesSet( void ) const
{
    return m_renderstatesset;
}
