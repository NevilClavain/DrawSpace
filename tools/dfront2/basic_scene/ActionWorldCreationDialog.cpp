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

#include <wx/wx.h>
#include "BasicSceneMainFrame.h"

#include "ActionWorldCreationDialog.h"

void ActionWorldCreationDialog::Execute( void )
{
    DIALOG_ACTION_DECLARE( DIALOG_WORLD_CREATION_TITLE )
    DIALOG_APPENDROOT_STRING( "name", "" )

    DIALOG_APPENDROOT_NODE( "gravity vector", gravity_vector )
    DIALOG_APPENDNODE_FLOAT( gravity_vector, "x", 0.0 )
    DIALOG_APPENDNODE_FLOAT( gravity_vector, "y", -9.81 )
    DIALOG_APPENDNODE_FLOAT( gravity_vector, "z", 0.0 )

    DIALOG_APPLY
    DIALOG_SHOW
}
