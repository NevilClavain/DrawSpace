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

#ifndef _LUACLASS_MESHERENDERING_H_
#define _LUACLASS_MESHERENDERING_H_

#include "luna.h"
#include "mesherenderingaspectimpl.h"
#include "renderingaspect.h"
#include "mesheimport.h"

class LuaClass_MesheRendering
{
private:
    DrawSpace::Core::Meshe                                          m_meshe;

    DrawSpace::AspectImplementations::MesheRenderingAspectImpl*     m_meshe_render;
    DrawSpace::Aspect::RenderingAspect*                             m_entity_rendering_aspect;
    DrawSpace::Core::Entity*                                        m_entity;

	// table de traduction RenderContext name -> Passes Name
	// permet de savoir a quelle passe est attribu�e un rendercontext
	std::map<dsstring, dsstring>									m_rcname_to_passes;

    std::map<dsstring, DrawSpace::Core::RenderingNode*>             m_renderingnodes; // classes par passes

    // ici stocker les ptr des textures associees a un des renderingnodes mais pas allouees
    // via LUA_configure() mais allouees et appartenant a des entitees exterieures
    // (typiquement : textures target d'une rendering passe)
    std::set<DrawSpace::Core::Texture*>                             m_external_textures;

    void cleanup_resources( lua_State* p_L );

public:

	LuaClass_MesheRendering( lua_State* p_L );
	~LuaClass_MesheRendering( void );

    int LUA_attachtoentity( lua_State* p_L );
    int LUA_detachfromentity( lua_State* p_L );
    int LUA_configure( lua_State* p_L );

    int LUA_setshaderreal( lua_State* p_L );
    int LUA_setshaderrealvector( lua_State* p_L );
	int LUA_setshaderrealinvector(lua_State* p_L);
    int LUA_setshaderrealmatrix( lua_State* p_L );
    int LUA_setshaderbool( lua_State* p_L );

    int LUA_setpassnodetexturefrompass( lua_State* p_L );


    int LUA_release( lua_State* p_L );
    int LUA_registertorendering( lua_State* p_L );
    int LUA_unregisterfromrendering( lua_State* p_L );
   
    int LUA_setNormaleGenerationMode(lua_State* p_L);
    int LUA_setTBGenerationMode(lua_State* p_L);
    int LUA_setNormaleTransformation(lua_State* p_L);

	int LUA_setPassForRenderContext(lua_State* p_L);
    
    static const char className[];
    static const Luna<LuaClass_MesheRendering>::RegType methods[];
    
};

#endif
