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

#include "dialog.h"

BEGIN_EVENT_TABLE( Dialog, wxDialog )
    EVT_IDLE( Dialog::OnIdle )
	EVT_CLOSE( Dialog::OnClose )
END_EVENT_TABLE()


Dialog::Dialog( wxWindow* p_parent, const wxString& p_title ) :
wxDialog( p_parent, wxID_ANY, p_title, wxPoint( 860, 50 ), wxSize( 200, 400 ), wxCAPTION )
{

}

Dialog::~Dialog( void )
{

}

void Dialog::OnIdle( wxIdleEvent& p_event )
{

}

void Dialog::OnClose( wxCloseEvent& p_event )
{
}
