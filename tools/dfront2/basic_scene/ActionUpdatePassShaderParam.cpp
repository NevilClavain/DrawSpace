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

#include "ActionUpdatePassShaderParam.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;


void ActionUpdatePassShaderParam::Execute( DrawSpace::Core::PropertyPool& p_propertypool )
{
    dsstring passname = p_propertypool.GetPropValue<dsstring>( "passname" );
    dsstring paramid = p_propertypool.GetPropValue<dsstring>( "paramid" );
    Vector values = p_propertypool.GetPropValue<Vector>( "values" );

    Pass* current_pass = dynamic_cast<Pass*>( ConfigsBase::GetInstance()->GetConfigurableInstance( passname ) );

    if( !current_pass )
    {
        wxMessageBox( "DrawSpace:UpdatePassShaderParam : bad pass name (dont exists or is not a pass) : " + passname, "Script error", wxICON_ERROR );
        return;
    }

    Configurable::PropertiesMap props;
    current_pass->GetPropertiesMap( props );

    std::map<dsstring, RenderingNode::ShadersParams> viewportquad_shaderparams = props["viewportquad_shaderparams"].GetPropValue<std::map<dsstring, RenderingNode::ShadersParams>>();

    if( viewportquad_shaderparams.count( paramid ) == 0 )
    {
        wxMessageBox( "DrawSpace:UpdatePassShaderParam : unknown pass shader param id " + passname + " " + paramid, "Script error", wxICON_ERROR );
    }

    // apply pass viewportquad shader values modifications
    current_pass->GetViewportQuad()->SetShaderRealVector( paramid, values );

    // update pass properties   
    viewportquad_shaderparams[paramid].param_values = values;

    props["viewportquad_shaderparams"].SetPropValue<std::map<dsstring, RenderingNode::ShadersParams>>( viewportquad_shaderparams );

    current_pass->SetPropertiesMap( props );

    // update mainframe list ctrl
    BasicSceneMainFrame::GetInstance()->AdaptPassesShaderParamsList( current_pass );

}