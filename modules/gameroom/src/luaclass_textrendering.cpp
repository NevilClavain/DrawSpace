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

#include "luacontext.h"
#include "luaclass_textrendering.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;

const char LuaClass_TextRendering::className[] = "TextRendering";
const Luna<LuaClass_TextRendering>::RegType LuaClass_TextRendering::methods[] =
{
    { "configure", &LuaClass_TextRendering::LUA_configure },
    { "update", &LuaClass_TextRendering::LUA_update },
    { "release", &LuaClass_TextRendering::LUA_release },
	{ 0, 0 }
};

LuaClass_TextRendering::LuaClass_TextRendering( lua_State* p_L ) :
m_entity_rendering_aspect( NULL )
{
}

LuaClass_TextRendering::~LuaClass_TextRendering( void )
{
}


int LuaClass_TextRendering::LUA_configure( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc < 8 )
	{
        LUA_ERROR( "TextRendering::configure : argument(s) missing" );
	}

    LuaClass_Entity* lua_ent = Luna<LuaClass_Entity>::check( p_L, 1 );

    dsstring id = luaL_checkstring( p_L, 2 );
    int posx = luaL_checkint( p_L, 3 );
    int posy = luaL_checkint( p_L, 4 );
    int r = luaL_checkint( p_L, 5 );
    int g = luaL_checkint( p_L, 6 );
    int b = luaL_checkint( p_L, 7 );
    dsstring text = luaL_checkstring( p_L, 8 );
   
    DrawSpace::Core::Entity& entity = lua_ent->GetEntity();

    // recupere l'aspect rendu s'il existe pour cette entitee
    RenderingAspect* rendering_aspect = entity.GetAspect<RenderingAspect>();
    if( rendering_aspect )
    {
        rendering_aspect->AddImplementation( &m_text_render );
        m_entity_rendering_aspect = rendering_aspect;

        LUA_TRY
        {
            rendering_aspect->AddComponent<TextRenderingAspectImpl::TextDisplay>( id, posx, posy, r, g, b, text );

        } LUA_CATCH;
        
        m_id = id;
    }
    else
    {
        m_entity_rendering_aspect = NULL;
        LUA_ERROR( "TextRendering::configure : entity passed on arg has no rendering aspect" );
    }
    return 0;
}

int LuaClass_TextRendering::LUA_release( lua_State* p_L )
{
    if( m_entity_rendering_aspect )
    {
        LUA_TRY
        {
            m_entity_rendering_aspect->RemoveComponent<TextRenderingAspectImpl::TextDisplay>( m_id );
            m_entity_rendering_aspect->RemoveImplementation( &m_text_render );

        } LUA_CATCH;

        m_id = "";
        m_entity_rendering_aspect = NULL;
    }
    else
    {
        LUA_ERROR( "TextRendering::release : no rendering aspect" );
    }
    return 0;
}

int LuaClass_TextRendering::LUA_update( lua_State* p_L )
{
    if( !m_entity_rendering_aspect )
    {
        LUA_ERROR( "TextRendering::update : no rendering aspect" );
    }

	int argc = lua_gettop( p_L );
	if( argc < 6 )
	{
        LUA_ERROR( "TextRendering::update : argument(s) missing" );
	}

    LUA_TRY
    {
        int posx = luaL_checkint( p_L, 1 );
        int posy = luaL_checkint( p_L, 2 );
        int r = luaL_checkint( p_L, 3 );
        int g = luaL_checkint( p_L, 4 );
        int b = luaL_checkint( p_L, 5 );
        dsstring text = luaL_checkstring( p_L, 6 );

        TextRenderingAspectImpl::TextDisplay td( posx, posy, r, g, b, text );
        m_entity_rendering_aspect->GetComponent<TextRenderingAspectImpl::TextDisplay>( m_id )->getPurpose() = td;

    } LUA_CATCH;

    return 0;
}
