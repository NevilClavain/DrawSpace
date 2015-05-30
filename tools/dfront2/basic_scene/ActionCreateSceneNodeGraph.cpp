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

#include "ActionCreateSceneNodeGraph.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;


void ActionCreateSceneNodeGraph::Execute( DrawSpace::Core::PropertyPool& p_propertypool )
{
    dsstring alias = p_propertypool.GetPropValue<dsstring>( "name" );
    SceneNodeGraph** newsc_ptr = p_propertypool.GetPropValue<SceneNodeGraph**>( "newsc_ptr" );
    dsstring* newsc_alias_ptr = p_propertypool.GetPropValue<dsstring*>( "newsc_alias_ptr" );

    BasicSceneMainFrame::SceneNodeGraphEntry entry;

    entry.name = alias;
    entry.scenenodegraph = new SceneNodeGraph();
    entry.treeitemid = BasicSceneMainFrame::GetInstance()->AppendItem( BasicSceneMainFrame::GetInstance()->m_scenegraphs_root_item, alias.c_str(), SCENEGRAPH_ICON_INDEX );
    entry.current_camera_set = false;
    BasicSceneMainFrame::GetInstance()->m_scenenodegraphs[entry.treeitemid.GetID()] = entry;

    BasicSceneMainFrame::GetInstance()->m_menubuild_table[entry.treeitemid.GetID()] = SCENEGRAPH_MASK;
    
    *newsc_ptr = entry.scenenodegraph;
    *newsc_alias_ptr = alias;
}