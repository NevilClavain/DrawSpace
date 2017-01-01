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

#include "ActionTransformEditionApply.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

void ActionTransformEditionApply::Execute( BasicSceneObjectPropertiesDialog* p_dialog )
{
    DIALOG_GETGRID
    DIALOG_PROPERTIES_VARS

    std::vector<Matrix> new_chain;
    bool ok = true;
    Vector values;


    DIALOG_EXPLORE_NODES_BEGIN( "", "matrix %d", i, matrix_label )

        DIALOG_GET_ENUM_PROPERTY( DIALOG_INCREMENT_STRING( matrix_label, "type" ), matrix_type )
        DIALOG_GET_FLOAT_PROPERTY( DIALOG_INCREMENT_STRING( matrix_label, "x" ), x )
        DIALOG_GET_FLOAT_PROPERTY( DIALOG_INCREMENT_STRING( matrix_label, "y" ), y )
        DIALOG_GET_FLOAT_PROPERTY( DIALOG_INCREMENT_STRING( matrix_label, "z" ), z )
        DIALOG_GET_FLOAT_PROPERTY( DIALOG_INCREMENT_STRING( matrix_label, "angle" ), angle )

        DIALOG_WXSTRING_TO_DSSTRING( matrix_type, matrix_type2 )

        values[0] = x;
        values[1] = y;
        values[2] = z;
        values[3] = angle;
       
        Matrix mat;

        if( "identity" == matrix_type2 )
        {
            mat.Identity();            
        }
        else if( "translation" == matrix_type2 )
        {
            values[3] = 1.0;
            mat.Translation( values );            
        }
        else if( "rotation" == matrix_type2 )
        {
            Vector axis = values;
            axis[3] = 1.0;
            mat.Rotation( axis, Maths::DegToRad( values[3] ) );
        }
        else if( "scaling" == matrix_type2 )
        {
            values[3] = 1.0;
            mat.Scale( values );
        }
        else if( "zero" == matrix_type2 )
        {
            mat.Zero();            
        }
        else if( "undetermined" == matrix_type2 )
        {
            ok = false;
            break;
        }

        new_chain.push_back( mat );      


    DIALOG_EXPLORE_NODES_END( i )

    if( ok )
    {
        Transformation* tdet = BasicSceneMainFrame::GetInstance()->m_transformation_nodes[p_dialog->GetTreeItem().GetID()].scene_node->GetContent();

        tdet->ClearAll();
        for( size_t i = 0; i < new_chain.size(); i++ )
        {
            tdet->PushMatrix( new_chain[i] );
        }            
    }
    else
    {
        wxMessageBox( "Edited matrix chain has a undetermined matrix", "DrawFront error", wxICON_ERROR );
    }
}
