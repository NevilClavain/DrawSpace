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

#include "ActionAddShaderParam.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;


void ActionAddShaderParam::Execute( BasicSceneObjectPropertiesDialog* p_dialog )
{
    DIALOG_GETGRID

    DIALOG_SPECIFIC1_LABEL( "shader params %d", param_label )

    DIALOG_APPENDROOT_NODE( param_label, param_root )
    DIALOG_APPENDNODE_ENUM( param_root, "pass", BasicSceneMainFrame::GetInstance()->get_intermediatepasses_list() )
    DIALOG_APPENDNODE_INTEGER( param_root, "shader index", 0 )
    DIALOG_APPENDNODE_STRING( param_root, "param id", "" )
    DIALOG_APPENDNODE_INTEGER( param_root, "register", 0 )
    DIALOG_APPENDNODE_NODE( param_root, "values", param_values )
    DIALOG_APPENDNODE_FLOAT( param_values, "x", 0.0 )
    DIALOG_APPENDNODE_FLOAT( param_values, "y", 0.0 )
    DIALOG_APPENDNODE_FLOAT( param_values, "z", 0.0 )
    DIALOG_APPENDNODE_FLOAT( param_values, "w", 0.0 )
    DIALOG_FINALIZE
}
