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

#include "ActionSetSceneNodeGraphCurrentCamera.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;


void ActionSetSceneNodeGraphCurrentCamera::Execute( DrawSpace::Core::PropertyPool& p_propertypool )
{
    dsstring scenegraphname = p_propertypool.GetPropValue<dsstring>( "scenegraphname" );
    dsstring cameraname = p_propertypool.GetPropValue<dsstring>( "cameraname" );

    bool sg_found = false;
    BasicSceneMainFrame::SceneNodeGraphEntry s_entry;
    
    for( std::map<void*, BasicSceneMainFrame::SceneNodeGraphEntry>::iterator it = BasicSceneMainFrame::GetInstance()->m_scenenodegraphs.begin(); it != BasicSceneMainFrame::GetInstance()->m_scenenodegraphs.end(); ++it )
    {
        if( it->second.name == scenegraphname )
        {
            s_entry = it->second;
            sg_found = true;
            break;
        }
    }
    if( !sg_found )
    {
        wxMessageBox( "DrawSpace:SetSceneNodeGraphCurrentCamera : unknown scenegraph name", "Script error", wxICON_ERROR );
        return;
    }

    bool cam_found = false;
    BasicSceneMainFrame::SceneNodeEntry<DrawSpace::Dynamics::CameraPoint> c_entry;

    for( std::map<void*, BasicSceneMainFrame::SceneNodeEntry<DrawSpace::Dynamics::CameraPoint>>::iterator it = BasicSceneMainFrame::GetInstance()->m_camera_nodes.begin(); it != BasicSceneMainFrame::GetInstance()->m_camera_nodes.end(); ++it )
    {
        if( it->second.name == cameraname )
        {
            c_entry = it->second;
            cam_found = true;
            break;
        }
    }
    if( !cam_found )
    {
        wxMessageBox( "DrawSpace:SetSceneNodeGraphCurrentCamera : unknown camera name", "Script error", wxICON_ERROR );
        return;
    }
    
    void* idsg;
    for( std::map<void*, BasicSceneMainFrame::SceneNodeGraphEntry>::iterator it = BasicSceneMainFrame::GetInstance()->m_scenenodegraphs.begin(); it != BasicSceneMainFrame::GetInstance()->m_scenenodegraphs.end(); ++it )
    {
        if( it->second.name == scenegraphname )
        {
            if( it->second.current_camera_set )
            {
                BasicSceneMainFrame::GetInstance()->SetItemImage( it->second.current_camera, CAMERA_ICON_INDEX );
            }
            idsg = it->first;
            break;
        }
    }

    void* idcam = BasicSceneMainFrame::GetInstance()->m_inv_tree_nodes[c_entry.scene_node];


    BasicSceneMainFrame::GetInstance()->SetItemImage( c_entry.treeitemid, CAMERASEL_ICON_INDEX );
    BasicSceneMainFrame::GetInstance()->m_scenenodegraphs[idsg].current_camera_set = true;
    BasicSceneMainFrame::GetInstance()->m_scenenodegraphs[idsg].current_camera = c_entry.treeitemid;

    BasicSceneMainFrame::GetInstance()->m_scenenodegraphs[idsg].scenenodegraph->SetCurrentCamera( c_entry.name );
    
}