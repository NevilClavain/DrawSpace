/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2016                              
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

#ifndef _LUA_CLOUDSNODE_H_
#define _LUA_CLOUDSNODE_H_

#include "drawspace_commons.h"
#include "callback.h"
#include "mediator.h"
#include "clouds.h"
#include "descriptors.h"

#include "luna.h"

class LuaCloudsNode
{
public:

    DrawSpace::Core::BaseCallback<void, DrawSpace::Core::PropertyPool&>*    m_scriptcalls_handler;

protected:

    DrawSpace::Utils::CloudsDescriptor                          m_descriptor;    
    DrawSpace::Core::SceneNode<DrawSpace::Clouds>               m_clouds_node;
    

public:
    LuaCloudsNode( lua_State* p_L );
    ~LuaCloudsNode( void );
    
    int Lua_EnableDetails( lua_State* p_L );
    int Lua_SetSeed( lua_State* p_L );
    int Lua_SetSortingDistance( lua_State* p_L );
    int Lua_SetProceduralFilePath( lua_State* p_L );


    int Lua_RegisterPassSlot( lua_State* p_L );
    int Lua_SetPassSlotFxName( lua_State* p_L );
    int Lua_SetPassSlotRenderingOrder( lua_State* p_L );
    int Lua_SetPassSlotTextureName( lua_State* p_L );
    int Lua_AddPassSlotShaderParam( lua_State* p_L );
    int Lua_UpdateShaderParam( lua_State* p_L );
    int Lua_LinkTo( lua_State* p_L );

    int Lua_UpdatePassSlotTexture( lua_State* p_L );
    

    
    static const char className[];
    static const Luna2<LuaCloudsNode>::RegType methods[];
};

#endif
