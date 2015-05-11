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


#ifndef _LUA_DRAWSPACE_H_
#define _LUA_DRAWSPACE_H_

#include "drawspace_commons.h"
#include "callback.h"
#include "mediator.h"
#include "timemanager.h"
#include "scenenodegraph.h"
#include "luna.h"

class LuaDrawSpace
{
public:

    typedef DrawSpace::Core::CallBack2<LuaDrawSpace, void, DrawSpace::Core::SceneNodeGraph::NodesEvent, DrawSpace::Core::BaseSceneNode*>    NodesEventCallback;

    typedef struct
    {
        dsstring                            alias;
        DrawSpace::Core::SceneNodeGraph*    scenenodegraph;
        NodesEventCallback*                 cb;

    } ScenegraphDescr;

    lua_State*                                                              m_L;

    DrawSpace::Core::BaseCallback<void, DrawSpace::Core::PropertyPool&>*    m_scriptcalls_handler;
    DrawSpace::Utils::TimeManager*                                          m_timer;
    
    dsstring                                                                m_scenegrapheventcbname;
    std::vector<ScenegraphDescr>                                            m_nodesevent_callbacks;
    

    void on_scenenodegraph_evt( DrawSpace::Core::SceneNodeGraph::NodesEvent p_evt, DrawSpace::Core::BaseSceneNode* p_node );


public:
    LuaDrawSpace( lua_State* p_L );
    ~LuaDrawSpace( void );
     
    int Lua_DisplayFramerate( lua_State* p_L );
    int Lua_DisplayCurrentCamera( lua_State* p_L );
    int Lua_CreateSceneNodeGraph( lua_State* p_L );

    int Lua_SetSceneNodeGraphCurrentCamera( lua_State* p_L );

    int Lua_LoadKeyUpScript( lua_State* p_L );
    int Lua_LoadKeyDownScript( lua_State* p_L );
    int Lua_LoadMouseScript( lua_State* p_L );

    int Lua_AngleSpeedInc( lua_State* p_L );
    int Lua_AngleSpeedDec( lua_State* p_L );

    int Lua_TranslationSpeedInc( lua_State* p_L );
    int Lua_TranslationSpeedDec( lua_State* p_L );

    int Lua_GetSceneCameraName( lua_State* p_L );

    int Lua_IsCurrentCamera( lua_State* p_L );
    int Lua_SetScenegraphEventCallback( lua_State* p_L );

    static const char className[];
    static const Luna2<LuaDrawSpace>::RegType methods[];
};

#endif
