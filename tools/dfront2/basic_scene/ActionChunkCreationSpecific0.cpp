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

#include "ActionChunkCreationSpecific0.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

void ActionChunkCreationSpecific0::Execute( BasicSceneObjectPropertiesDialog* p_dialog )
{
    DIALOG_GETGRID

    DIALOG_SPECIFIC0_LABEL( "pass %d", pass_label )

    DIALOG_APPENDROOT_NODE( pass_label, pass_root )

    DIALOG_APPENDNODE_ENUM( pass_root, "pass", BasicSceneMainFrame::GetInstance()->get_intermediatepasses_list() )
    DIALOG_APPENDNODE_ENUM( pass_root, "fx", BasicSceneMainFrame::GetInstance()->get_fx_list() )
    DIALOG_APPENDNODE_INTEGER( pass_root, "rendering order", 10000 )
  
    DIALOG_APPENDNODE_NODE( pass_root, "textures", textures_label )

    DIALOG_BUILD_LABELS( RenderingNode::NbMaxTextures, "stage %d", textures_stages_labels )
    DIALOG_APPENDNODE_ITERATE( textures_label, BasicSceneMainFrame::GetInstance()->insert_void_choice( BasicSceneMainFrame::GetInstance()->get_textures_list() ), DIALOG_APPENDNODE_ENUM, textures_stages_labels )

    DIALOG_FINALIZE
}
