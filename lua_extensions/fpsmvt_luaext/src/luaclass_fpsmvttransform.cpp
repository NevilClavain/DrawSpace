/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2022
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
#include "luaclass_fpsmvttransform.h"
#include "luaclass_entity.h"
#include "transformaspect.h"
#include "quaternion.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Aspect;

const char LuaClass_FPSMovementTransform::className[] = "FPSMovementTransform";
const Luna<LuaClass_FPSMovementTransform>::RegType LuaClass_FPSMovementTransform::methods[] =
{
    { "configure", &LuaClass_FPSMovementTransform::LUA_configure },
    { "release", &LuaClass_FPSMovementTransform::LUA_release },
	{ 0, 0 }
};

LuaClass_FPSMovementTransform::LuaClass_FPSMovementTransform( lua_State* p_L )
{
}

LuaClass_FPSMovementTransform::~LuaClass_FPSMovementTransform( void )
{
}


int LuaClass_FPSMovementTransform::LUA_configure( lua_State* p_L )
{
    int argc = lua_gettop(p_L);
    if (argc < 2)
    {
        LUA_ERROR("FPSMovementTransform::configure : argument(s) missing");
    }

    LuaClass_Entity* lua_ent{ Luna<LuaClass_Entity>::check(p_L, 1) };
	int transfoimpl_order{ luaL_checkint(p_L, 2) };
	
	// here get specific parameters...
	// ...
	
    DrawSpace::Core::Entity& entity{ lua_ent->GetEntity() };

    // recupere l'aspect transfo s'il existe pour cette entitee
    TransformAspect* transform_aspect{ entity.GetAspect<TransformAspect>() };
    if (transform_aspect)
    {
        transform_aspect->AddImplementation(transfoimpl_order, this);
        m_entity_transform_aspect = transform_aspect;

		// here set specific parameters in m_entity_transform_aspect with AddComponent<type>(...) calls
    }
    else
    {
        m_entity_transform_aspect = NULL;
        LUA_ERROR("FPSMovementTransform::configure : entity passed on arg has no transform aspect");
    }
    return 0;
}

int LuaClass_FPSMovementTransform::LUA_release( lua_State* p_L )
{
    if( !m_entity_transform_aspect )
    {
        LUA_ERROR( "FPSMovementTransform::update : no transform aspect" );
    }
   
    // here unset specific parameters in m_entity_transform_aspect with RemoveComponent<type>(...) calls
	// ...

    m_entity_transform_aspect->RemoveAllImplementations();
    m_entity_transform_aspect = NULL;
		
    return 0;
}

void LuaClass_FPSMovementTransform::GetLocaleTransform(DrawSpace::Aspect::TransformAspect* p_transformaspect, DrawSpace::Utils::Matrix& p_out_base_transform)
{

}