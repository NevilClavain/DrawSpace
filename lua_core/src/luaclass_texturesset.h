/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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

#ifndef _LUACLASS_TEXTURESSET_H_
#define _LUACLASS_TEXTURESSET_H_

#include "luna.h"
#include "renderingnode.h"

class LuaClass_TexturesSet
{
public:
    struct Data
    {
        dsstring textures[DrawSpace::Core::RenderingNode::NbMaxTextures];
    };

protected:
    Data m_data;

public:
	LuaClass_TexturesSet( lua_State* p_L );
	~LuaClass_TexturesSet( void );

    int LUA_settexturefiletostage( lua_State* p_L );

    inline Data GetData( void ) const
    {
        return m_data;
    }

    static const char className[];
    static const Luna<LuaClass_TexturesSet>::RegType methods[];    
};

#endif
