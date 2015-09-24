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

#include "ActionChunkCreationSpecific2.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

void ActionChunkCreationSpecific2::Execute( BasicSceneObjectPropertiesDialog* p_dialog )
{
    DIALOG_GETGRID

    DIALOG_SPECIFIC2_LABEL( "impostor %d", pass_label )

    DIALOG_APPENDROOT_NODE( pass_label, pass_root )

    DIALOG_APPENDNODE_NODE( pass_root, "scale", scale_label )

    DIALOG_APPENDNODE_FLOAT( scale_label, "width", 1.0 )
    DIALOG_APPENDNODE_FLOAT( scale_label, "height", 1.0 )

    DIALOG_APPENDNODE_NODE( pass_root, "position", position_label )
    DIALOG_APPENDNODE_FLOAT( position_label, "x", 0.0 )
    DIALOG_APPENDNODE_FLOAT( position_label, "y", 0.0 )
    DIALOG_APPENDNODE_FLOAT( position_label, "z", 0.0 )

    DIALOG_APPENDNODE_NODE( pass_root, "texture UV", uv_label )
    DIALOG_APPENDNODE_FLOAT( uv_label, "u1", 0.0 )
    DIALOG_APPENDNODE_FLOAT( uv_label, "v1", 0.0 )
    DIALOG_APPENDNODE_FLOAT( uv_label, "u2", 1.0 )
    DIALOG_APPENDNODE_FLOAT( uv_label, "v2", 0.0 )
    DIALOG_APPENDNODE_FLOAT( uv_label, "u3", 1.0 )
    DIALOG_APPENDNODE_FLOAT( uv_label, "v3", 1.0 )
    DIALOG_APPENDNODE_FLOAT( uv_label, "u4", 0.0 )
    DIALOG_APPENDNODE_FLOAT( uv_label, "v4", 1.0 )

    DIALOG_FINALIZE
}