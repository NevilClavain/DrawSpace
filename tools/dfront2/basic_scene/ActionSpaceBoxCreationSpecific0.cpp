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

#include "ActionSpaceBoxCreationSpecific0.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

void ActionSpaceBoxCreationSpecific0::Execute( BasicSceneObjectPropertiesDialog* p_dialog )
{
    DIALOG_GETGRID

    DIALOG_SPECIFIC0_LABEL( "pass %d", pass_label )

    DIALOG_APPENDROOT_NODE( pass_label, pass_root )

    DIALOG_APPENDNODE_ENUM( pass_root, "pass", BasicSceneMainFrame::GetInstance()->get_intermediatepasses_list() )
    DIALOG_APPENDNODE_ENUM( pass_root, "fx", BasicSceneMainFrame::GetInstance()->get_fx_list() )
    DIALOG_APPENDNODE_INTEGER( pass_root, "rendering order", 200 )

    DIALOG_APPENDNODE_NODE( pass_root, "textures", textures_label )

    DIALOG_APPENDNODE_NODE( textures_label, "front", front_label )
    DIALOG_BUILD_LABELS( RenderingNode::NbMaxTextures, "stage %d", front_textures_stages_labels )
    DIALOG_APPENDNODE_ITERATE( front_label, BasicSceneMainFrame::GetInstance()->insert_void_choice( BasicSceneMainFrame::GetInstance()->get_textures_list() ), DIALOG_APPENDNODE_ENUM, front_textures_stages_labels )

    DIALOG_APPENDNODE_NODE( textures_label, "rear", rear_label )
    DIALOG_BUILD_LABELS( RenderingNode::NbMaxTextures, "stage %d", rear_textures_stages_labels )
    DIALOG_APPENDNODE_ITERATE( rear_label, BasicSceneMainFrame::GetInstance()->insert_void_choice( BasicSceneMainFrame::GetInstance()->get_textures_list() ), DIALOG_APPENDNODE_ENUM, rear_textures_stages_labels )

    DIALOG_APPENDNODE_NODE( textures_label, "left", left_label )
    DIALOG_BUILD_LABELS( RenderingNode::NbMaxTextures, "stage %d", left_textures_stages_labels )
    DIALOG_APPENDNODE_ITERATE( left_label, BasicSceneMainFrame::GetInstance()->insert_void_choice( BasicSceneMainFrame::GetInstance()->get_textures_list() ), DIALOG_APPENDNODE_ENUM, left_textures_stages_labels )

    DIALOG_APPENDNODE_NODE( textures_label, "right", right_label )
    DIALOG_BUILD_LABELS( RenderingNode::NbMaxTextures, "stage %d", right_textures_stages_labels )
    DIALOG_APPENDNODE_ITERATE( right_label, BasicSceneMainFrame::GetInstance()->insert_void_choice( BasicSceneMainFrame::GetInstance()->get_textures_list() ), DIALOG_APPENDNODE_ENUM, right_textures_stages_labels )

    DIALOG_APPENDNODE_NODE( textures_label, "top", top_label )
    DIALOG_BUILD_LABELS( RenderingNode::NbMaxTextures, "stage %d", top_textures_stages_labels )
    DIALOG_APPENDNODE_ITERATE( top_label, BasicSceneMainFrame::GetInstance()->insert_void_choice( BasicSceneMainFrame::GetInstance()->get_textures_list() ), DIALOG_APPENDNODE_ENUM, top_textures_stages_labels )

    DIALOG_APPENDNODE_NODE( textures_label, "bottom", bottom_label )
    DIALOG_BUILD_LABELS( RenderingNode::NbMaxTextures, "stage %d", bottom_textures_stages_labels )
    DIALOG_APPENDNODE_ITERATE( bottom_label, BasicSceneMainFrame::GetInstance()->insert_void_choice( BasicSceneMainFrame::GetInstance()->get_textures_list() ), DIALOG_APPENDNODE_ENUM, bottom_textures_stages_labels )

    DIALOG_FINALIZE
}
