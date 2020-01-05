/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2020                     
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
#include "luaclass_vector.h"
#include "maths.h"

using namespace DrawSpace::Utils;

const char LuaClass_Vector::className[] = "Vector";
const Luna<LuaClass_Vector>::RegType LuaClass_Vector::methods[] =
{
    { "get_x", &LuaClass_Vector::LUA_getx },
    { "get_y", &LuaClass_Vector::LUA_gety },
    { "get_z", &LuaClass_Vector::LUA_getz },
    { "get_w", &LuaClass_Vector::LUA_getw },
    { "set_x", &LuaClass_Vector::LUA_setx },
    { "set_y", &LuaClass_Vector::LUA_sety },
    { "set_z", &LuaClass_Vector::LUA_setz },
    { "set_w", &LuaClass_Vector::LUA_setw },
    { "lengthpow2", &LuaClass_Vector::LUA_lengthpow2 },
    { "length", &LuaClass_Vector::LUA_length },
    { "normalize", &LuaClass_Vector::LUA_normalize },
    { "scale", &LuaClass_Vector::LUA_scale },
    { "dotproduct_with", &LuaClass_Vector::LUA_dotproductwith },
    { "crossproduct_with", &LuaClass_Vector::LUA_crossproductwith },
    { "copy", &LuaClass_Vector::LUA_copy },
    { "add_with", &LuaClass_Vector::LUA_addwith },
    { "sub_with", &LuaClass_Vector::LUA_subwith },
    { 0, 0 }
};

LuaClass_Vector::LuaClass_Vector(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc == 4)
    {
        dsreal x = luaL_checknumber(p_L, 1);
        dsreal y = luaL_checknumber(p_L, 2);
        dsreal z = luaL_checknumber(p_L, 3);
        dsreal w = luaL_checknumber(p_L, 4);
        m_vector = std::make_unique<DrawSpace::Utils::Vector>( x, y, z, w );
    }
    else
    {
        m_vector = std::make_unique<DrawSpace::Utils::Vector>();
    }    
}

LuaClass_Vector::~LuaClass_Vector(void)
{
}

int LuaClass_Vector::LUA_getx(lua_State* p_L)
{
    lua_pushnumber(p_L, (*m_vector)[0]);
    return 1;
}

int LuaClass_Vector::LUA_gety(lua_State* p_L)
{
    lua_pushnumber(p_L, (*m_vector)[1]);
    return 1;
}

int LuaClass_Vector::LUA_getz(lua_State* p_L)
{
    lua_pushnumber(p_L, (*m_vector)[2]);
    return 1;
}

int LuaClass_Vector::LUA_getw(lua_State* p_L)
{
    lua_pushnumber(p_L, (*m_vector)[3]);
    return 1;
}

int LuaClass_Vector::LUA_setx(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("Vector::set_x : argument(s) missing");
    }
    (*m_vector)[0] = luaL_checknumber(p_L, 1);

    return 0;
}

int LuaClass_Vector::LUA_sety(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("Vector::set_y : argument(s) missing");
    }
    (*m_vector)[1] = luaL_checknumber(p_L, 1);

    return 0;
}

int LuaClass_Vector::LUA_setz(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("Vector::set_z : argument(s) missing");
    }
    (*m_vector)[2] = luaL_checknumber(p_L, 1);

    return 0;
}

int LuaClass_Vector::LUA_setw(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("Vector::set_w : argument(s) missing");
    }
    (*m_vector)[3] = luaL_checknumber(p_L, 1);

    return 0;
}

int LuaClass_Vector::LUA_lengthpow2(lua_State* p_L)
{
    lua_pushnumber(p_L, m_vector->LengthPow2());
    return 1;
}

int LuaClass_Vector::LUA_length(lua_State* p_L)
{
    lua_pushnumber(p_L, m_vector->Length());
    return 1;
}

int LuaClass_Vector::LUA_normalize(lua_State* p_L)
{
    m_vector->Normalize();
    return 0;
}

int LuaClass_Vector::LUA_scale(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("Vector::scale : argument(s) missing");
    }

    m_vector->Scale(luaL_checknumber(p_L, 1));
    return 0;
}

int LuaClass_Vector::LUA_dotproductwith(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("Vector::dotproduct_with : argument(s) missing");
    }

    LuaClass_Vector* vectorB = Luna<LuaClass_Vector>::check(p_L, 1);

    dsreal dotproduct = *m_vector * vectorB->getVector();
    lua_pushnumber(p_L, dotproduct);
    return 1;
}

int LuaClass_Vector::LUA_crossproductwith(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("Vector::crossproduct_with : argument(s) missing");
    }

    LuaClass_Vector* vectorA = Luna<LuaClass_Vector>::check(p_L, 1);
    LuaClass_Vector* vectorB = Luna<LuaClass_Vector>::check(p_L, 2);

    *m_vector = ProdVec(vectorA->getVector(), vectorB->getVector());
    return 0;
}

int LuaClass_Vector::LUA_copy(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("Vector::copy : argument(s) missing");
    }

    LuaClass_Vector* vectorA = Luna<LuaClass_Vector>::check(p_L, 1);
    *m_vector = vectorA->getVector();

    return 0;
}

int LuaClass_Vector::LUA_addwith(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("Vector::add_with : argument(s) missing");
    }
    LuaClass_Vector* vectorA = Luna<LuaClass_Vector>::check(p_L, 1);

    (*m_vector)[0] = (*m_vector)[0] + (vectorA->getVector())[0];
    (*m_vector)[1] = (*m_vector)[1] + (vectorA->getVector())[1];
    (*m_vector)[2] = (*m_vector)[2] + (vectorA->getVector())[2];

    return 0;
}

int LuaClass_Vector::LUA_subwith(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc < 1)
    {
        LUA_ERROR("Vector::sub_with : argument(s) missing");
    }
    LuaClass_Vector* vectorA = Luna<LuaClass_Vector>::check(p_L, 1);

    (*m_vector)[0] = (*m_vector)[0] - (vectorA->getVector())[0];
    (*m_vector)[1] = (*m_vector)[1] - (vectorA->getVector())[1];
    (*m_vector)[2] = (*m_vector)[2] - (vectorA->getVector())[2];

    return 0;
}


/////////////////////////////////////////////////////////////////////

DrawSpace::Utils::Vector LuaClass_Vector::getVector(void) const
{
    return *m_vector;
}
