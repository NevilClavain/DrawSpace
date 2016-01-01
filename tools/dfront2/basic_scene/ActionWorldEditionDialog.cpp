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

#include "ActionWorldEditionDialog.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

void ActionWorldEditionDialog::Execute( void )
{
    void* id = BasicSceneMainFrame::GetInstance()->m_last_clicked_treeitem.GetID();

    if( BasicSceneMainFrame::GetInstance()->m_worlds.count( id ) > 0 )
    {
        DIALOG_ACTION_DECLARE( DIALOG_WORLD_EDITION_TITLE )

        DIALOG_APPENDROOT_STRING( "name", BasicSceneMainFrame::GetInstance()->m_worlds[id].name );

        Vector gravity;
        BasicSceneMainFrame::GetInstance()->m_worlds[id].world->GetGravity( gravity );

        DIALOG_APPENDROOT_NODE( "gravity", gravity_root );
        DIALOG_APPENDNODE_FLOAT( gravity_root, "x", gravity[0] );
        DIALOG_APPENDNODE_FLOAT( gravity_root, "y", gravity[1] );
        DIALOG_APPENDNODE_FLOAT( gravity_root, "z", gravity[2] );    
        DIALOG_APPLY
        DIALOG_SHOW
    }
}
