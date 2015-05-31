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

#include "ActionWorldCreationApply.h"
#include "buildobjects.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

void ActionWorldCreationApply::Execute( BasicSceneObjectPropertiesDialog* p_dialog )
{
    DIALOG_GETGRID
    DIALOG_PROPERTIES_VARS

    DIALOG_GET_STRING_PROPERTY( "name", alias2 )
    DIALOG_WXSTRING_TO_DSSTRING( alias2, alias )

    if( "" == alias )
    {
        wxMessageBox( "'name' attribute cannot be void", "DrawFront error", wxICON_ERROR );
        return;
    }

    DIALOG_GET_FLOAT_PROPERTY( "gravity vector.x", g_x )
    DIALOG_GET_FLOAT_PROPERTY( "gravity vector.y", g_y )
    DIALOG_GET_FLOAT_PROPERTY( "gravity vector.z", g_z )

    World* world = new World;

    world->Initialize();
    world->SetGravity( Vector( g_x, g_y, g_z, 0.0 ) );


    BasicSceneMainFrame::WorldEntry entry;
    entry.name = alias;
    entry.world = world;
    entry.treeitemid = BasicSceneMainFrame::GetInstance()->AppendItem( p_dialog->GetTreeItem(), alias2, WORLD_ICON_INDEX );

    BasicSceneMainFrame::GetInstance()->m_worlds[entry.treeitemid.GetID()] = entry;
    BasicSceneMainFrame::GetInstance()->m_menubuild_table[entry.treeitemid.GetID()] = WORLD_MASK;
       
    DIALOG_CLOSE

}