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

#include "ActionWorldEditionApply.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

void ActionWorldEditionApply::Execute( BasicSceneObjectPropertiesDialog* p_dialog )
{
    DIALOG_GETGRID
    DIALOG_PROPERTIES_VARS

    DIALOG_GET_FLOAT_PROPERTY( "gravity.x", gx );
    DIALOG_GET_FLOAT_PROPERTY( "gravity.y", gy );
    DIALOG_GET_FLOAT_PROPERTY( "gravity.z", gz );

    BasicSceneMainFrame::WorldEntry world = BasicSceneMainFrame::GetInstance()->m_worlds[p_dialog->GetTreeItem().GetID()];

    world.world->SetGravity( Vector( gx, gy, gz, 1.0 ) );

    DIALOG_CLOSE
}
