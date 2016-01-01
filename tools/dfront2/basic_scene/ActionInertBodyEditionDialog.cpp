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

#include "ActionInertBodyEditionDialog.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

void ActionInertBodyEditionDialog::Execute( void )
{
    void* id = BasicSceneMainFrame::GetInstance()->m_last_clicked_treeitem.GetID();

    if( BasicSceneMainFrame::GetInstance()->m_inertbody_descriptors.count( id ) > 0 )
    {
        Body::Parameters descr = BasicSceneMainFrame::GetInstance()->m_inertbody_descriptors[id];
        BasicSceneMainFrame::SceneNodeEntry<DrawSpace::Dynamics::InertBody> inertbody_node = BasicSceneMainFrame::GetInstance()->m_inertbody_nodes[id];

        DIALOG_ACTION_DECLARE( DIALOG_INERTBODY_EDITION_TITLE )       
        DIALOG_APPENDROOT_STRING( "scene name", inertbody_node.name )
        DIALOG_APPENDROOT_BOOL( "active", inertbody_node.scene_node->GetContent()->IsEnabled() );
        DIALOG_APPLY
        DIALOG_SHOW
    }
}
