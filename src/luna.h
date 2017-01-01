/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
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

/*
this is luna a C++ class binder for lua
do not know who exactly is the author of this; for more details, visit http://lua-users.org/wiki/LunaWrapper
*/


#ifndef _LUNA_H_
#define _LUNA_H_


extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

/*
template<class T> 
class Luna 
{
public:
    static void Register(lua_State *L) 
    {
        lua_pushcfunction(L, &Luna<T>::constructor);
        lua_setglobal(L, T::className);

        luaL_newmetatable(L, T::className);
        lua_pushstring(L, "__gc");
        lua_pushcfunction(L, &Luna<T>::gc_obj);
        lua_settable(L, -3);
    }

    static int constructor(lua_State *L) 
    {
        T* obj = new T(L);

        lua_newtable(L);
        lua_pushnumber(L, 0);
        T** a = (T**)lua_newuserdata(L, sizeof(T*));
        *a = obj;
        luaL_getmetatable(L, T::className);
        lua_setmetatable(L, -2);
        lua_settable(L, -3); // table[0] = obj;

        for (int i = 0; T::Register[i].name; i++) 
        {
            lua_pushstring(L, T::Register[i].name);
            lua_pushnumber(L, i);
            lua_pushcclosure(L, &Luna<T>::thunk, 1);
            lua_settable(L, -3);
        }
        return 1;
    }

    static int thunk(lua_State *L) 
    {
        int i = (int)lua_tonumber(L, lua_upvalueindex(1));
        lua_pushnumber(L, 0);
        lua_gettable(L, 1);

        T** obj = static_cast<T**>(luaL_checkudata(L, -1, T::className));
        lua_remove(L, -1);
        return ((*obj)->*(T::Register[i].mfunc))(L);
    }

    static int gc_obj(lua_State *L)
    {
        T** obj = static_cast<T**>(luaL_checkudata(L, -1, T::className));
        delete (*obj);
        return 0;
    }

    struct RegType 
    {
        const char *name;
        int(T::*mfunc)(lua_State*);
    };
};
*/


template <typename T> class Luna2 
{
  typedef struct { T *pT; } userdataType;
public:
  typedef int (T::*mfp)(lua_State *L);
  typedef struct { const char *name; mfp mfunc; } RegType;

  static void Register(lua_State *L) 
  {
    lua_newtable(L);
    int methods = lua_gettop(L);

    luaL_newmetatable(L, T::className);
    int metatable = lua_gettop(L);

    // store method table in globals so that
    // scripts can add functions written in Lua.
    lua_pushstring(L, T::className);
    lua_pushvalue(L, methods);
    //lua_settable(L, LUA_GLOBALSINDEX);   // LUA_GLOBALSINDEX unsupported in lua 5.2
    lua_setglobal(L,T::className);

    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, methods);
    lua_settable(L, metatable);  // hide metatable from Lua getmetatable()

    lua_pushliteral(L, "__index");
    lua_pushvalue(L, methods);
    lua_settable(L, metatable);

    lua_pushliteral(L, "__tostring");
    lua_pushcfunction(L, tostring_T);
    lua_settable(L, metatable);

    lua_pushliteral(L, "__gc");
    lua_pushcfunction(L, gc_T);
    lua_settable(L, metatable);

    lua_newtable(L);                // mt for method table
    int mt = lua_gettop(L);
    lua_pushliteral(L, "__call");
    lua_pushcfunction(L, new_T);
    lua_pushliteral(L, "new");
    lua_pushvalue(L, -2);           // dup new_T function
    lua_settable(L, methods);       // add new_T to method table
    lua_settable(L, mt);            // mt.__call = new_T
    lua_setmetatable(L, methods);

    // fill method table with methods from class T
    for (RegType *l = (RegType *)T::methods; l->name; l++) 
    {
    /* edited by Snaily: shouldn't it be const RegType *l ... ? */
      lua_pushstring(L, l->name);
      lua_pushlightuserdata(L, (void*)l);
      lua_pushcclosure(L, thunk, 1);
      lua_settable(L, methods);
    }

    lua_pop(L, 2);  // drop metatable and method table
  }

  // get userdata from Lua stack and return pointer to T object
  static T *check(lua_State *L, int narg) 
  {
    userdataType *ud =
      static_cast<userdataType*>(luaL_checkudata(L, narg, T::className));
    //if(!ud) luaL_typerror(L, narg, T::className);
    if( !ud )
    {
        return NULL;
    }
    return ud->pT;  // pointer to T object
  }

private:
  Luna2();  // hide default constructor

  // member function dispatcher
  static int thunk(lua_State *L) 
  {
    // stack has userdata, followed by method args
    T *obj = check(L, 1);  // get 'self', or if you prefer, 'this'
    lua_remove(L, 1);  // remove self so member function args start at index 1
    // get member function from upvalue
    RegType *l = static_cast<RegType*>(lua_touserdata(L, lua_upvalueindex(1)));
    return (obj->*(l->mfunc))(L);  // call member function
  }

  // create a new T object and
  // push onto the Lua stack a userdata containing a pointer to T object
  static int new_T(lua_State *L) 
  {
    lua_remove(L, 1);   // use classname:new(), instead of classname.new()
    T *obj = new T(L);  // call constructor for T objects
    userdataType *ud =
      static_cast<userdataType*>(lua_newuserdata(L, sizeof(userdataType)));
    ud->pT = obj;  // store pointer to object in userdata
    luaL_getmetatable(L, T::className);  // lookup metatable in Lua registry
    lua_setmetatable(L, -2);
    return 1;  // userdata containing pointer to T object
  }

  // garbage collection metamethod
  static int gc_T(lua_State *L) 
  {
    userdataType *ud = static_cast<userdataType*>(lua_touserdata(L, 1));
    T *obj = ud->pT;
    delete obj;  // call destructor for T objects
    return 0;
  }

  static int tostring_T (lua_State *L) 
  {
    char buff[32];
    userdataType *ud = static_cast<userdataType*>(lua_touserdata(L, 1));
    T *obj = ud->pT;
    sprintf(buff, "%p", obj);
    lua_pushfstring(L, "%s (%s)", T::className, buff);
    return 1;
  }
};

#endif
