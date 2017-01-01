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

#include "ActionCameraPointSelection.h"

void ActionCameraPointSelection::Execute( void )
{    
    void* id = BasicSceneMainFrame::GetInstance()->find_scenenodegraph_id( BasicSceneMainFrame::GetInstance()->m_last_clicked_treeitem );

    if( BasicSceneMainFrame::GetInstance()->m_scenenodegraphs[id].current_camera_set )
    {
        //m_scenegraphs_treeCtrl->SetItemImage( BasicSceneMainFrame::GetInstance()->m_scenenodegraphs[id].current_camera, CAMERA_ICON_INDEX );
        BasicSceneMainFrame::GetInstance()->SetItemImage( BasicSceneMainFrame::GetInstance()->m_scenenodegraphs[id].current_camera, CAMERA_ICON_INDEX );
    }

    //m_scenegraphs_treeCtrl->SetItemImage( BasicSceneMainFrame::GetInstance()->m_last_clicked_treeitem, CAMERASEL_ICON_INDEX );

    BasicSceneMainFrame::GetInstance()->SetItemImage( BasicSceneMainFrame::GetInstance()->m_last_clicked_treeitem, CAMERASEL_ICON_INDEX );
    BasicSceneMainFrame::GetInstance()->m_scenenodegraphs[id].current_camera_set = true;
    BasicSceneMainFrame::GetInstance()->m_scenenodegraphs[id].current_camera = BasicSceneMainFrame::GetInstance()->m_last_clicked_treeitem;

    BasicSceneMainFrame::GetInstance()->m_scenenodegraphs[id].scenenodegraph->SetCurrentCamera( BasicSceneMainFrame::GetInstance()->m_camera_nodes[BasicSceneMainFrame::GetInstance()->m_last_clicked_treeitem.GetID()].name );
}
