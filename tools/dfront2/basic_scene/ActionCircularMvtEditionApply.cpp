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

#include "ActionCircularMvtEditionApply.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

void ActionCircularMvtEditionApply::Execute( BasicSceneObjectPropertiesDialog* p_dialog )
{
    DIALOG_GETGRID
    DIALOG_PROPERTIES_VARS

    DIALOG_GET_FLOAT_PROPERTY( "current theta", curr_theta );
    DIALOG_GET_FLOAT_PROPERTY( "current phi", curr_phi );
    DIALOG_GET_FLOAT_PROPERTY( "current angular speed", curr_speed );
    BasicSceneMainFrame::SceneNodeEntry<DrawSpace::Core::CircularMovement> circ_node = BasicSceneMainFrame::GetInstance()->m_circ_nodes[p_dialog->GetTreeItem().GetID()];
    circ_node.scene_node->GetContent()->SetAngularSpeed( curr_speed );
    circ_node.scene_node->GetContent()->SetTheta( curr_theta );
    circ_node.scene_node->GetContent()->SetPhi( curr_phi );
}
