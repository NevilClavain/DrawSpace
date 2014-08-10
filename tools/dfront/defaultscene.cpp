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

#include "defaultscene.h"

DefaultScene::DefaultScene( void )
{
}

DefaultScene::~DefaultScene( void )
{
}

void DefaultScene::Draw( void )
{
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    renderer->ClearScreen( 0, 0, 0 );

    renderer->BeginScreen();

    renderer->DrawText( 0, 255, 0, 10, 10, "Default scene" );

    renderer->DrawText( 255, 0, 0, 10, 40, "%d fps", m_timer.GetFPS() );

    renderer->EndScreen();
    renderer->FlipScreen();

    m_timer.Update();
}

bool DefaultScene::IsBrowsable( void )
{
    return false;
}

bool DefaultScene::IsLuaScriptRecipient( void )
{
    return false;
}

