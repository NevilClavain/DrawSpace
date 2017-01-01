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

#include "ActionTextureCtor.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;


void ActionTextureCtor::Execute( DrawSpace::Core::PropertyPool& p_propertypool )
{
    dsstring name = p_propertypool.GetPropValue<dsstring>( "name" );
    Texture** texture_ptr = p_propertypool.GetPropValue<Texture**>( "texture_ptr" );

    // chercher si un asset texture existe avec ce nom

    Texture* texture = dynamic_cast<Texture*>( AssetsBase::GetInstance()->GetAsset( name ) );
    if( texture )
    {
        *texture_ptr = texture;
    }
    else
    {
        IntermediatePass* pass = dynamic_cast<IntermediatePass*>( ConfigsBase::GetInstance()->GetConfigurableInstance( name ) );
        if( pass )
        {
            *texture_ptr = pass->GetTargetTexture();
        }
        else
        {
            wxMessageBox( wxString( "nor texture or pass with name : " ) + wxString( name.c_str() ) , "DrawFront error", wxICON_ERROR );
        }
    }
}
