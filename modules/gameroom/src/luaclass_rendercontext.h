/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2019                     
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

#ifndef _LUACLASS_RENDERCONTEXT_H_
#define _LUACLASS_RENDERCONTEXT_H_

#include "luna.h"
#include "renderingnode.h"

#include "luaclass_fxparams.h"
#include "luaclass_texturesset.h"


class LuaClass_RenderContext
{
public:
    using NamedShaderParam = std::pair<dsstring, DrawSpace::Core::RenderingNode::ShadersParams>;

    struct Data
    {
        Data( void ) : rendering_order(10000)
        {
        }

        int                                     rendering_order;
        dsstring                                passname;
        std::vector<LuaClass_FxParams::Data>    fxparams;
        std::vector<LuaClass_TexturesSet::Data> textures_sets;
        std::vector<LuaClass_TexturesSet::Data> vertex_textures_sets;
        std::vector<NamedShaderParam>           shaders_params;
    };

private:
    Data m_data;

public:
	LuaClass_RenderContext( lua_State* p_L );
	~LuaClass_RenderContext( void );

    int LUA_addfxparams( lua_State* p_L );
    int LUA_addtexturesset( lua_State* p_L );
    int LUA_addvertextexturesset( lua_State* p_L );
    int LUA_setrenderingorder( lua_State* p_L );
    int LUA_addshaderparam( lua_State* p_L );
   
    inline Data GetData(void) const
    {
        return m_data;
    }

    //dsstring GetPassName( void ) const;

    static const char className[];
    static const Luna<LuaClass_RenderContext>::RegType methods[];
};

#endif
