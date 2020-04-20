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

#ifndef _LUACLASS_RENDERPASSNODEGRAPH_H_
#define _LUACLASS_RENDERPASSNODEGRAPH_H_

#include "renderpassnodegraph.h"
#include "passesrenderingaspectimpl.h"
#include "fx.h"

#include "luna.h"

class LuaClass_RenderPassNodeGraph
{
protected:

    using Passe = struct
    {
        DrawSpace::RenderGraph::RenderPassNode  m_renderpassnode;
        DrawSpace::Core::Fx                     m_fx;
    };

	using RenderPassEventCb           = DrawSpace::Core::CallBack<LuaClass_RenderPassNodeGraph, void, DrawSpace::RenderGraph::RenderPassNodeGraph::RenderPassEvent>;
    
    DrawSpace::RenderGraph::RenderPassNodeGraph                             m_rendergraph;
    DrawSpace::AspectImplementations::PassesRenderingAspectImpl             m_passes_render; // le RenderingAspectImpl associe au m_rendergraph et permettant de lancer les rendus des passes stockees dans m_rendergraph

    std::unordered_map<dsstring, Passe>                                     m_passes;
    dsstring                                                                m_id;

	std::map<dsstring, int>                                                 m_renderpassevent_lua_callbacks;

	RenderPassEventCb														m_renderpass_event_cb;

    void cleanup_resources( lua_State* p_L, const dsstring& p_passid );

	void on_renderpass_event(DrawSpace::RenderGraph::RenderPassNodeGraph::RenderPassEvent p_event);

public:
	LuaClass_RenderPassNodeGraph( lua_State* p_L );
	~LuaClass_RenderPassNodeGraph( void );

    DrawSpace::AspectImplementations::PassesRenderingAspectImpl& GetPassesRenderAspectImpl( void );
    DrawSpace::RenderGraph::RenderPassNodeGraph& GetRenderGraph( void );

    int LUA_createroot( lua_State* p_L );
    int LUA_createchild( lua_State* p_L );
    int LUA_removepass( lua_State* p_L );
    int LUA_setpasstargetslice(lua_State* p_L);
    int LUA_setpasstargetclearcolor( lua_State* p_L );
    int LUA_setpasstargetclearstate( lua_State* p_L );
    int LUA_setpassdepthclearstate( lua_State* p_L );
    int LUA_createpassviewportquad( lua_State* p_L );
    int LUA_removepassviewportquad( lua_State* p_L );

    int LUA_configurepassviewportquadresources( lua_State* p_L );
    int LUA_releasepassviewportquadresources( lua_State* p_L );

    int LUA_setviewportquadshaderrealvector( lua_State* p_L );
	int LUA_setviewportquadshaderrealmatrix(lua_State* p_L);

    int LUA_updaterenderingqueues( lua_State* p_L );

	int LUA_addrenderpasseventcb(lua_State* p_L);
	int LUA_removerenderpasseventcb(lua_State* p_L);

	void RegisterAnimationEventCallback(const dsstring& p_id, int p_regindex);
	int UnregisterAnimationEventCallback(const dsstring& p_id);

    DrawSpace::RenderGraph::RenderPassNode& GetNode( const dsstring& p_pass_id );
    
    static const char className[];
    static const Luna<LuaClass_RenderPassNodeGraph>::RegType methods[];

};

#endif
