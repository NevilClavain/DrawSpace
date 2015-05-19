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

#include "ActionInertBodyCreationDialog.h"

void ActionInertBodyCreationDialog::Execute( void )
{
    DIALOG_ACTION_DECLARE( DIALOG_INERTBODY_CREATION_TITLE )

    DIALOG_APPENDROOT_STRING( "scene name", "" )

    DIALOG_APPENDROOT_FLOAT( "mass", 0.0 );
    DIALOG_APPENDROOT_NODE( "shape description", shape_descr )
    wxArrayString shape_types;
    shape_types.Add( "box" );
    shape_types.Add( "sphere" );
    shape_types.Add( "meshe" );

    DIALOG_APPENDNODE_ENUM( shape_descr, "shape type", shape_types )
    DIALOG_APPENDNODE_FLOAT( shape_descr, "sphere radius", 0.5 )
    DIALOG_APPENDNODE_NODE( shape_descr, "box dims", box_dims )
    DIALOG_APPENDNODE_FLOAT( box_dims, "x", 0.5 )
    DIALOG_APPENDNODE_FLOAT( box_dims, "y", 0.5 )
    DIALOG_APPENDNODE_FLOAT( box_dims, "z", 0.5 )
    DIALOG_APPENDNODE_ENUM( shape_descr, "meshe", BasicSceneMainFrame::GetInstance()->get_meshes_list() )

    DIALOG_APPENDROOT_BOOL( "active at creation", false );

    DIALOG_SPECIFIC0( "Add matrix" )
    DIALOG_APPLY
    DIALOG_SHOW
}