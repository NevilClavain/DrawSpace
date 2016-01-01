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

#include <wx/wx.h>
#include "BasicSceneMainFrame.h"

#include "ActionCameraPointEditionDialog.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

void ActionCameraPointEditionDialog::Execute( void )
{
    void* id = BasicSceneMainFrame::GetInstance()->m_last_clicked_treeitem.GetID();

    BasicSceneMainFrame::SceneNodeEntry<DrawSpace::Dynamics::CameraPoint> camera_node = BasicSceneMainFrame::GetInstance()->m_camera_nodes[id];
    SceneNodeGraph* sc_owner = camera_node.scene_node->GetSceneNodeGraph();

    DIALOG_ACTION_DECLARE( DIALOG_CAMERA_EDIT_TITLE )

    DIALOG_APPENDROOT_STRING( "scene name", camera_node.name );
    DIALOG_APPENDROOT_FLOAT( "znear", camera_node.scene_node->GetContent()->GetZNear() );
    DIALOG_APPENDROOT_ENUM( "lock on", BasicSceneMainFrame::GetInstance()->insert_void_choice( BasicSceneMainFrame::GetInstance()->get_scenenodes_list( sc_owner ) ) );
    DIALOG_APPLY
    DIALOG_SHOW
}
