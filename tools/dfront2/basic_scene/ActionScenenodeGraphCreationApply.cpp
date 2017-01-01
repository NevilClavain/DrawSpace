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

#include "ActionScenenodeGraphCreationApply.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

void ActionScenenodeGraphCreationApply::Execute( BasicSceneObjectPropertiesDialog* p_dialog )
{
    DIALOG_GETGRID
    DIALOG_PROPERTIES_VARS

    DIALOG_GET_STRING_PROPERTY( "name", scenegraph_name )

    DIALOG_WXSTRING_TO_DSSTRING( scenegraph_name, scenegraph_name2 )

    if( "" == scenegraph_name2 )
    {
        wxMessageBox( "'name' attribute cannot be void", "DrawFront error", wxICON_ERROR );
    }
    else
    {

        BasicSceneMainFrame::SceneNodeGraphEntry entry;

        entry.name = scenegraph_name2;
        entry.scenenodegraph = new SceneNodeGraph();
        
        //entry.treeitemid = BasicSceneMainFrame::GetInstance()->AppendItem( BasicSceneMainFrame::GetInstance()->m_scenegraphs_root_item, scenegraph_name, SCENEGRAPH_ICON_INDEX );
        entry.treeitemid = BasicSceneMainFrame::GetInstance()->AppendItem( p_dialog->GetTreeItem(), scenegraph_name, SCENEGRAPH_ICON_INDEX );
        entry.current_camera_set = false;

        BasicSceneMainFrame::GetInstance()->m_scenenodegraphs[entry.treeitemid.GetID()] = entry;
        BasicSceneMainFrame::GetInstance()->m_menubuild_table[entry.treeitemid.GetID()] = SCENEGRAPH_MASK;
       
        DIALOG_CLOSE
    }

}
