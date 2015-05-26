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

#include "ActionCameraPointEditionApply.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

void ActionCameraPointEditionApply::Execute( BasicSceneObjectPropertiesDialog* p_dialog )
{
    DIALOG_GETGRID
    DIALOG_PROPERTIES_VARS

    DIALOG_GET_FLOAT_PROPERTY( "znear", znear );
    BasicSceneMainFrame::SceneNodeEntry<DrawSpace::Dynamics::CameraPoint> camera_node = BasicSceneMainFrame::GetInstance()->m_camera_nodes[p_dialog->GetTreeItem().GetID()];
    camera_node.scene_node->GetContent()->UpdateProjectionZNear( znear );

    DIALOG_GET_ENUM_PROPERTY( "lock on", locked_node );
    if( locked_node != "..." )
    {
        DIALOG_WXSTRING_TO_DSSTRING( locked_node, locked_node2 )

        BasicSceneMainFrame::SceneNodeEntry<DrawSpace::Dynamics::CameraPoint> camera_node = BasicSceneMainFrame::GetInstance()->m_camera_nodes[p_dialog->GetTreeItem().GetID()];
        SceneNodeGraph* sc_owner = camera_node.scene_node->GetSceneNodeGraph();

        std::vector<BaseSceneNode*> n_list = sc_owner->GetAllNodesList();
        for( size_t i = 0; i < n_list.size(); i++ )
        {
            dsstring node_name;
            n_list[i]->GetSceneName( node_name );

            if( node_name == locked_node2 )
            {
                camera_node.scene_node->GetContent()->Lock( n_list[i] );
            }
        }        
    }
    else
    {
        camera_node.scene_node->GetContent()->Unlock();
    }
}