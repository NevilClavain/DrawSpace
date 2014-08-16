/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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

#include "scene.h"


const char Scene::className[] = "Scene";
const DrawSpace::Luna<Scene>::RegType Scene::Register[] =
{
  { "ShowFps", &Scene::Lua_ShowFps },
  { 0 }
};


Scene::Scene( lua_State* p_L ) : 
m_showfps( true )
{
}

Scene::~Scene( void )
{
}

void Scene::Draw( void )
{
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    renderer->ClearScreen( 0, 0, 0 );

    renderer->BeginScreen();
   
    if( m_showfps )
    {
        renderer->DrawText( 255, 0, 0, 10, 20, "%d fps", m_timer.GetFPS() );
    }

    renderer->DrawText( 255, 0, 0, 10, 40, "%s", m_name.c_str() );

    renderer->EndScreen();
    renderer->FlipScreen();

    m_timer.Update();
}

int Scene::Lua_ShowFps( lua_State* p_L )
{
	int argc = lua_gettop( p_L );
	if( argc != 2 )
	{
		lua_pushstring( p_L, "ShowFps : bad number of args" );
		lua_error( p_L );		
	}

    m_showfps = ( (int)luaL_checkinteger( p_L, 2 ) != 0 ? true : false );

    return 0;
}

void Scene::GetName( dsstring& p_name )
{
    p_name = m_name;
}

void Scene::SetName( const dsstring& p_name )
{
    m_name = p_name;
}