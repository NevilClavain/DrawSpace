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


#ifndef _LUACLASS_RENDERCONTEXT_H_
#define _LUACLASS_RENDERCONTEXT_H_

#include "luna.h"
#include "renderingnode.h"

class LuaClass_FxParams;
class LuaClass_TexturesSet;

class LuaClass_RenderContext
{
public:
    using NamedShaderParam = std::pair<dsstring, DrawSpace::Core::RenderingNode::ShadersParams>;

private:
    int                                 m_rendering_order;
    dsstring                            m_passname;
    std::vector<LuaClass_FxParams*>     m_fxparams;
    std::vector<LuaClass_TexturesSet*>  m_textures_sets;
    std::vector<NamedShaderParam>       m_shaders_params;

public:
	LuaClass_RenderContext( lua_State* p_L );
	~LuaClass_RenderContext( void );

    int LUA_addfxparams( lua_State* p_L );
    int LUA_addtexturesset( lua_State* p_L );
    int LUA_setrenderingorder( lua_State* p_L );
    int LUA_addshaderparam( lua_State* p_L );
    

    int GetFxParamsListSize( void ) const;
    LuaClass_FxParams* GetFxParams( int p_index ) const;

    int GetTexturesSetListSize( void ) const;
    LuaClass_TexturesSet* GetTexturesSet( int p_index ) const;

    int GetShadersParamsListSize( void ) const;
    NamedShaderParam GetNamedShaderParam( int p_index ) const;

    int GetRenderingOrder( void ) const;

    static const char className[];
    static const Luna<LuaClass_RenderContext>::RegType methods[];
};

#endif
