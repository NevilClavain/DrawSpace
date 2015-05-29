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

#include "ActionGetSceneCameraName.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;


void ActionGetSceneCameraName::Execute( DrawSpace::Core::PropertyPool& p_propertypool )
{
    dsstring scenegraphname = p_propertypool.GetPropValue<dsstring>( "scenegraphname" );
    dsstring* result = p_propertypool.GetPropValue<dsstring*>( "result" );

    bool sg_found = false;
    BasicSceneMainFrame::SceneNodeGraphEntry s_entry;
    
    for( std::map<void*, BasicSceneMainFrame::SceneNodeGraphEntry>::iterator it = BasicSceneMainFrame::GetInstance()->m_scenenodegraphs.begin(); it != BasicSceneMainFrame::GetInstance()->m_scenenodegraphs.end(); ++it )
    {
        if( it->second.name == scenegraphname )
        {
            s_entry = it->second;
            sg_found = true;
            break;
        }
    }
    if( !sg_found )
    {
        wxMessageBox( "DrawSpace:SetSceneNodeGraphCurrentCamera : unknown scenegraph name", "Script error", wxICON_ERROR );
        return;
    }

    dsstring camera_name;
    s_entry.scenenodegraph->GetCurrentCameraName( camera_name );
    *result = camera_name;
}