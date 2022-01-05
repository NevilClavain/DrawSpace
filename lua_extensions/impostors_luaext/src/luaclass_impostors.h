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

#include "drawspace_commons.h"
#include "luna.h"

// fwd decls
namespace DrawSpace
{
namespace Core
{
class Entity;
class RenderingNode;
}
namespace Aspect
{
class RenderingAspect;
}
};

class ImpostorsRenderingAspectImpl;
// fwd decls

class LuaClass_Impostors
{
private:

    ImpostorsRenderingAspectImpl*                                   m_impostors_render          { nullptr };
    DrawSpace::Aspect::RenderingAspect*                             m_entity_rendering_aspect   { nullptr };
    DrawSpace::Core::Entity*                                        m_entity                    { nullptr };

    std::map<dsstring, DrawSpace::Core::RenderingNode*>             m_renderingnodes; // classes par passes

    // table de traduction RenderContext name -> Passes Name
    // permet de savoir a quelle passe est attribue un rendercontext
    std::map<dsstring, std::vector<dsstring>>                       m_rcname_to_passes;

    void cleanup_resources(lua_State* p_L);

public:
    LuaClass_Impostors( lua_State* p_L );
	~LuaClass_Impostors( void );

    int LUA_attachtoentity(lua_State* p_L);
    int LUA_detachfromentity(lua_State* p_L);

    int LUA_setPassForRenderId(lua_State* p_L);

    int LUA_registertorendering(lua_State* p_L);
    int LUA_unregisterfromrendering(lua_State* p_L);

    int LUA_configure(lua_State* p_L);
    int LUA_release(lua_State* p_L);

    int LUA_setshaderreal(lua_State* p_L);
    int LUA_setshaderrealvector(lua_State* p_L);
    int LUA_setshaderrealinvector(lua_State* p_L);
    int LUA_setshaderrealmatrix(lua_State* p_L);
    int LUA_setshaderbool(lua_State* p_L);



    static const char className[];
    static const Luna<LuaClass_Impostors>::RegType methods[];

};
