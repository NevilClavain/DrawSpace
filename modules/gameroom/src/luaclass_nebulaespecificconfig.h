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

#pragma once



#include "luna.h"
#include "vector.h"

class LuaClass_NebulaeSpecificConfig
{
private:

    // data model
    using UVPairList    = std::vector<std::pair<int,int>>;
    using Bloc          = std::tuple<DrawSpace::Utils::Vector, DrawSpace::Utils::Vector, dsreal, UVPairList, UVPairList>;
    using DataModel     = std::vector<Bloc>;

    DataModel   m_dataModel;
    int         m_texture_atlas_resolution;
    int         m_mask_atlas_resolution;

public:
    LuaClass_NebulaeSpecificConfig(lua_State* p_L);
    ~LuaClass_NebulaeSpecificConfig(void);

    int LUA_apply(lua_State* p_L);
    int LUA_cleanup(lua_State* p_L);

    int LUA_createbloc(lua_State* p_L);
    int LUA_setbloccolor(lua_State* p_L);
    int LUA_setblocposition(lua_State* p_L);
    int LUA_setblocscale(lua_State* p_L);
    int LUA_addbloctextureuvpair(lua_State* p_L);
    int LUA_addblocmaskuvpair(lua_State* p_L);

    int LUA_settexturesresolutions(lua_State* p_L);
   
    static const char className[];
    static const Luna<LuaClass_NebulaeSpecificConfig>::RegType methods[];
};

