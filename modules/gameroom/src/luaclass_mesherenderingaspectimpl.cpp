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

#include "luacontext.h"
#include "luaclass_mesherenderingaspectimpl.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;

const char LuaClass_MesheRenderingAspectImpl::className[] = "MesheRenderingAspectImpl";
const Luna<LuaClass_MesheRenderingAspectImpl>::RegType LuaClass_MesheRenderingAspectImpl::methods[] =
{
    { "configure", &LuaClass_MesheRenderingAspectImpl::LUA_configure },
    { "release", &LuaClass_MesheRenderingAspectImpl::LUA_release },
	{ 0, 0 }
};

LuaClass_MesheRenderingAspectImpl::LuaClass_MesheRenderingAspectImpl( lua_State* p_L ) :
m_entity_rendering_aspect( NULL ),
m_meshe_render( NULL )
{
}

LuaClass_MesheRenderingAspectImpl::~LuaClass_MesheRenderingAspectImpl( void )
{
}

int LuaClass_MesheRenderingAspectImpl::LUA_configure( lua_State* p_L )
{
    return 0;
}

int LuaClass_MesheRenderingAspectImpl::LUA_release( lua_State* p_L )
{
    if( m_meshe_render )
    {
        _DRAWSPACE_DELETE_( m_meshe_render );
    }
    return 0;
}
