/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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

#ifndef _LUA_CHUNKNODE_H_
#define _LUA_CHUNKNODE_H_

#include "drawspace_commons.h"
#include "callback.h"
#include "mediator.h"
#include "chunk.h"
#include "descriptors.h"

#include "luna.h"

class LuaChunkNode
{
public:

    DrawSpace::Core::BaseCallback<void, DrawSpace::Core::PropertyPool&>*    m_scriptcalls_handler;

protected:

    DrawSpace::Utils::ChunkDescriptor                           m_descriptor;    
    DrawSpace::Core::SceneNode<DrawSpace::Chunk>                m_chunk_node;

public:
    LuaChunkNode( lua_State* p_L );
    ~LuaChunkNode( void );
     
    int Lua_SetMesheName( lua_State* p_L );
    int Lua_RegisterPassSlot( lua_State* p_L );
    int Lua_SetPassSlotFxName( lua_State* p_L );
    int Lua_SetPassSlotRenderingOrder( lua_State* p_L );
    int Lua_SetPassSlotTextureName( lua_State* p_L );
    int Lua_AddPassSlotShaderParam( lua_State* p_L );
    int Lua_UpdateShaderParam( lua_State* p_L );
    int Lua_LinkTo( lua_State* p_L );

    int Lua_UpdatePassSlotTexture( lua_State* p_L );
    

    
    static const char className[];
    static const Luna2<LuaChunkNode>::RegType methods[];
};

#endif