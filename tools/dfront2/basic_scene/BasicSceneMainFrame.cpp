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

#include "BasicSceneMainFrame.h"
#include "drawspace.h"
#include "adapters.h"

BasicSceneMainFrame::BasicSceneMainFrame( wxWindow* parent ) : MainFrame( parent ),
m_glready( false )
{

}

void BasicSceneMainFrame::OnClose( wxCloseEvent& event )
{
    Destroy();
}

void BasicSceneMainFrame::OnIdle( wxIdleEvent& event )
{
    if( m_glready )
    {

        DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
        renderer->ClearScreen( 0, 0, 120 );


        renderer->BeginScreen();
                 
        renderer->DrawText( 255, 0, 0, 10, 20, "%d fps", m_timer.GetFPS() );

        renderer->EndScreen();

        renderer->FlipScreen();

        m_timer.Update();

        event.RequestMore( true );
    }
}

void BasicSceneMainFrame::SetGLReady( void )
{
    m_glready = true;
}

void BasicSceneMainFrame::Update( void )
{
    AdaptAssetsList( m_assets_listCtrl );
}