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

#include "CEGUIWrapper.h"
#include "exceptions.h"

CEGUIWrapper::CEGUIWrapper( void ) :
m_ready( false )
{
}


CEGUIWrapper::~CEGUIWrapper( void )
{
}

void CEGUIWrapper::SetReady( void )
{
    m_ready = true;
}

void CEGUIWrapper::RenderGUI( void )
{
    if( !m_ready )
    {
        _DSEXCEPTION( "GUI sub system is currently not initialized, cannot render" )
    }

    CEGUI::System::getSingleton().renderAllGUIContexts();
}
