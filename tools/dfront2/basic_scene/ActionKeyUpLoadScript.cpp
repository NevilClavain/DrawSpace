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

#include "ActionKeyUpLoadScript.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;


void ActionKeyUpLoadScript::Execute( DrawSpace::Core::PropertyPool& p_propertypool )
{
    dsstring filepath = p_propertypool.GetPropValue<dsstring>( "filepath" );

    long size;
    void* data = File::LoadAndAllocBinaryFile( filepath, &size );
    if( data )
    {
        char* script_text = new char[size + 1];
        memcpy( script_text, data, size );
        script_text[size] = 0;
        BasicSceneMainFrame::GetInstance()->m_keyup_script = script_text;
    }
    else
    {
        wxMessageBox( "DrawSpace:LoadKeyUpScript : file not found", "Script error", wxICON_ERROR );
    }
}
