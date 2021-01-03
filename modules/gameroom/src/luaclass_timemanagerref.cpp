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
#include "luaclass_entity.h"
#include "luaclass_timemanagerref.h"
#include "timemanager.h"
#include "timeaspect.h"

using namespace DrawSpace::Aspect;
using namespace DrawSpace::Utils;

const char LuaClass_TimeManagerRef::className[] = "TimeManagerRef";
const Luna<LuaClass_TimeManagerRef>::RegType LuaClass_TimeManagerRef::methods[] =
{
    { 0, 0 }
};

LuaClass_TimeManagerRef::LuaClass_TimeManagerRef(lua_State* p_L)
{
    int argc = lua_gettop(p_L);
    if (argc == 1)
    {
        LuaClass_Entity* lua_ent = Luna<LuaClass_Entity>::check(p_L, 1);

        DrawSpace::Core::Entity& entity = lua_ent->GetEntity();

        TimeAspect* time_aspect = entity.GetAspect<TimeAspect>();
        if (NULL == time_aspect)
        {
            LUA_ERROR("TimeManagerRef::TimeManagerRef : time aspect doesnt exists in specified entity!");
        }

        m_tm = &time_aspect->GetComponent<TimeManager>("time_manager")->getPurpose();

    }
    else
    {
        LUA_ERROR("TimeManagerRef::TimeManagerRef : argument(s) missing");
    }
}

LuaClass_TimeManagerRef::~LuaClass_TimeManagerRef(void)
{
}

DrawSpace::Utils::TimeManager* LuaClass_TimeManagerRef::GetTimeManager() const
{
    return m_tm;
}
