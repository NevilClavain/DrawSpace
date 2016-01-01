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

#include "ActionSetWorldGravity.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

void ActionSetWorldGravity::Execute( DrawSpace::Core::PropertyPool& p_propertypool )
{
    dsstring worldname = p_propertypool.GetPropValue<dsstring>( "worldname" );
    Vector gravity = p_propertypool.GetPropValue<Vector>( "gravity" );

    bool w_found = false;
    BasicSceneMainFrame::WorldEntry w_entry;

    for( std::map<void*, BasicSceneMainFrame::WorldEntry>::iterator it = BasicSceneMainFrame::GetInstance()->m_worlds.begin(); it != BasicSceneMainFrame::GetInstance()->m_worlds.end(); ++it )
    {
        if( it->second.name == worldname )
        {
            w_entry = it->second;
            w_found = true;
            break;
        }
    }
    if( !w_found )
    {
        wxMessageBox( "DrawSpace:SetWorldGravity : unknown world name", "Script error", wxICON_ERROR );
        return;
    }

    w_entry.world->SetGravity( gravity );

}
