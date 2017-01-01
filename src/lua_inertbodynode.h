/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
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

#ifndef _LUA_INERTBODYNODE_H_
#define _LUA_INERTBODYNODE_H_

#include "drawspace_commons.h"
#include "callback.h"
#include "mediator.h"
#include "inertbody.h"
#include "transformation.h"

#include "luna.h"

class LuaInertBodyNode
{
public:

    DrawSpace::Core::BaseCallback<void, DrawSpace::Core::PropertyPool&>*    m_scriptcalls_handler;

    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::InertBody>              m_inertbody_node;
    DrawSpace::Dynamics::Body::Parameters                                   m_params;
    bool                                                                    m_initial_state;
    DrawSpace::Core::Transformation                                         m_initial_att_transform;

    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::InertBody>*             m_existing_inertbody_node;    


    LuaInertBodyNode( lua_State* p_L );
    ~LuaInertBodyNode( void );
     
    int Lua_IsValid( lua_State* p_L );

    int Lua_AddInitialAttitudeMatrix( lua_State* p_L );
    int Lua_SetShapeDescrSphere( lua_State* p_L );
    int Lua_SetShapeDescrBox( lua_State* p_L );
    int Lua_SetMass( lua_State* p_L );
    int Lua_SetInitialState( lua_State* p_L );


    int Lua_GetShapeType( lua_State* p_L );
    int Lua_GetShapeDescrBoxDimX( lua_State* p_L );
    int Lua_GetShapeDescrBoxDimY( lua_State* p_L );
    int Lua_GetShapeDescrBoxDimZ( lua_State* p_L );
    int Lua_GetShapeDescrSphereRadius( lua_State* p_L );

    int Lua_LinkTo( lua_State* p_L );

    int Lua_UpdateState( lua_State* p_L );
    
    static const char className[];
    static const Luna2<LuaInertBodyNode>::RegType methods[];
};

#endif
