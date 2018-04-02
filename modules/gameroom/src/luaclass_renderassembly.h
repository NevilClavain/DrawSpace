/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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

#ifndef _LUACLASS_RENDERASSEMBLY_H_
#define _LUACLASS_RENDERASSEMBLY_H_

#include "luna.h"

#include "renderstatesset.h"
#include "renderingnode.h"

class LuaClass_RenderAssembly
{
protected:
    DrawSpace::Core::RenderStatesSet            m_rss;
    std::vector<std::pair<dsstring,bool>>       m_shaders;
    dsstring                                    m_textures[DrawSpace::Core::RenderingNode::NbMaxTextures];

public:
	LuaClass_RenderAssembly( lua_State* p_L );
	~LuaClass_RenderAssembly( void );

    int LUA_setrenderstatesset( lua_State* p_L );
    int LUA_addshaderfile( lua_State* p_L );
    int LUA_settexturefile( lua_State* p_L );

    DrawSpace::Core::RenderStatesSet GetRenderStatesSet( void ) const;
    dsstring GetTextureFile( int p_stage ) const;
    size_t GetNbShaderFiles( void ) const;
    std::pair<dsstring,bool> GetShaderFile( int p_index ) const;
    
    static const char className[];
    static const Luna<LuaClass_RenderAssembly>::RegType methods[];
};

#endif
