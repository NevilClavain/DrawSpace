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

#include <wx/wx.h>
#include "BasicSceneMainFrame.h"

#include "ActionCloudsCreationDialog.h"

void ActionCloudsCreationDialog::Execute( void )
{
    DIALOG_ACTION_DECLARE( DIALOG_CLOUDS_CREATION_TITLE )

    DIALOG_APPENDROOT_STRING( "scene name", "" )
    DIALOG_APPENDROOT_BOOL( "details", true )
    DIALOG_APPENDROOT_FILE( "procedural rules", "" )

    DIALOG_APPLY
    DIALOG_SPECIFIC0( "New pass slot" )
    DIALOG_SPECIFIC1( "New shaders param slot" )

    DIALOG_SHOW
}