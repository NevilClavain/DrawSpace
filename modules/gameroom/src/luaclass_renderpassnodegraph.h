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

#ifndef _LUACLASS_RENDERPASSNODEGRAPH_H_
#define _LUACLASS_RENDERPASSNODEGRAPH_H_

#include "renderpassnodegraph.h"
#include "passesrenderingaspectimpl.h"
#include "fx.h"

#include "luna.h"

class LuaClass_RenderPassNodeGraph
{
protected:

    typedef struct 
    {
        DrawSpace::RenderGraph::RenderPassNode  m_renderpassnode;
        DrawSpace::Core::Fx                     m_fx;

    } Passe;
    
    DrawSpace::RenderGraph::RenderPassNodeGraph                             m_rendergraph;
    DrawSpace::AspectImplementations::PassesRenderingAspectImpl             m_passes_render; // le RenderingAspectImpl associe au m_rendergraph et permettant de lancer les rendus des passes stockees dans m_rendergraph

    std::unordered_map<dsstring, Passe>                                     m_passes;

public:
	LuaClass_RenderPassNodeGraph( lua_State* p_L );
	~LuaClass_RenderPassNodeGraph( void );

    DrawSpace::AspectImplementations::PassesRenderingAspectImpl& GetPassesRenderAspectImpl( void );
    DrawSpace::RenderGraph::RenderPassNodeGraph& GetRenderGraph( void );

    int LUA_createroot( lua_State* p_L );
    int LUA_createchild( lua_State* p_L );
    int LUA_removepass( lua_State* p_L );
    int LUA_setpasstargetclearcolor( lua_State* p_L );
    int LUA_setpasstargetclearstate( lua_State* p_L );
    int LUA_setpassdepthclearstate( lua_State* p_L );
    int LUA_createpassviewportquad( lua_State* p_L );
    int LUA_removepassviewportquad( lua_State* p_L );
    int LUA_loadpassviewportquadshader( lua_State* p_L );
    int LUA_unloadpassviewportquadshaders( lua_State* p_L );
    int LUA_loadpassviewportquadtexture( lua_State* p_L );
    int LUA_unloadpassviewportquadtextures( lua_State* p_L );
    int LUA_setpassviewportquadrenderstateset( lua_State* p_L );
    int LUA_updaterenderingqueues( lua_State* p_L );

    static const char className[];
    static const Luna<LuaClass_RenderPassNodeGraph>::RegType methods[];

};

#endif