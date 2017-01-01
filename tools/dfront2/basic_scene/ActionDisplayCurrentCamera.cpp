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

#include "ActionDisplayCurrentCamera.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;


void ActionDisplayCurrentCamera::Execute( DrawSpace::Core::PropertyPool& p_propertypool )
{
    bool state = p_propertypool.GetPropValue<bool>( "state" );
    dsstring scenegraphname = p_propertypool.GetPropValue<dsstring>( "scenegraphname" );

    bool found = false;
    for( std::map<void*, BasicSceneMainFrame::SceneNodeGraphEntry>::iterator it = BasicSceneMainFrame::GetInstance()->m_scenenodegraphs.begin(); it != BasicSceneMainFrame::GetInstance()->m_scenenodegraphs.end(); ++it )
    {
        if( it->second.name == scenegraphname )
        {
            BasicSceneMainFrame::GetInstance()->m_display_currentcamera = state;
            BasicSceneMainFrame::GetInstance()->m_scenegraph_currentcameradisplay = it->second;
            found = true;
            break;
        }
    }
    if( !found )
    {
        wxMessageBox( "DrawSpace:DisplayCurrentCamera, unknown scenegraph name : " + scenegraphname, "Script error", wxICON_ERROR );
    }
}
