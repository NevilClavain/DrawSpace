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

#include "ActionCreateWorld.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;


void ActionCreateWorld::Execute( DrawSpace::Core::PropertyPool& p_propertypool )
{
    dsstring alias = p_propertypool.GetPropValue<dsstring>( "name" );
    Vector gravity = p_propertypool.GetPropValue<Vector>( "gravity" );

    BasicSceneMainFrame::WorldEntry entry;

    World* world = new World;
    world->Initialize();
    world->SetGravity( Vector( gravity[0], gravity[1], gravity[2], 0.0 ) );

    entry.name = alias;
    entry.world = world;
    entry.treeitemid = BasicSceneMainFrame::GetInstance()->AppendItem( BasicSceneMainFrame::GetInstance()->m_scenegraphs_root_item, alias.c_str(), WORLD_ICON_INDEX );

    BasicSceneMainFrame::GetInstance()->m_worlds[entry.treeitemid.GetID()] = entry;
    BasicSceneMainFrame::GetInstance()->m_menubuild_table[entry.treeitemid.GetID()] = WORLD_MASK;

}