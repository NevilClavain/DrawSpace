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

#include "ActionTransformEditionDialog.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

void ActionTransformEditionDialog::Execute( void )
{
    void* id = BasicSceneMainFrame::GetInstance()->m_last_clicked_treeitem.GetID();

    if( BasicSceneMainFrame::GetInstance()->m_transformation_nodes.count( id ) > 0 )
    {

        DIALOG_ACTION_DECLARE( DIALOG_TRANSFORM_EDITION_TITLE )

        DIALOG_APPLY
        DIALOG_SPECIFIC0( "Add matrix" )
        DIALOG_SPECIFIC1( "Clear all" )

        std::vector<Matrix> mat_chain;
        BasicSceneMainFrame::GetInstance()->m_transformation_nodes[id].scene_node->GetContent()->GetMatrixChain( mat_chain );


        DIALOG_APPENDROOT_STRING( "scene name", BasicSceneMainFrame::GetInstance()->m_transformation_nodes[id].name );

        DIALOG_BUILD_LABELS( mat_chain.size(), "matrix %d", matrix_labels )

        DIALOG_APPENDROOT_ITERATE_NODE_BEGIN( i, matrix_labels, matrix_root )


            Matrix::ConfigurationInfo mci;
            mat_chain[i].GetConfigInfos( mci );

            wxArrayString matrix_type_labels;

            matrix_type_labels.Add( "identity" );
            matrix_type_labels.Add( "scaling" );
            matrix_type_labels.Add( "translation" );
            matrix_type_labels.Add( "rotation" );
            matrix_type_labels.Add( "zero" );
            matrix_type_labels.Add( "undetermined" );

            wxArrayInt arrIds;
            arrIds.Add( Matrix::CONFIG_IDENTITY );
            arrIds.Add( Matrix::CONFIG_SCALING );
            arrIds.Add( Matrix::CONFIG_TRANSLATION );
            arrIds.Add( Matrix::CONFIG_ROTATION );
            arrIds.Add( Matrix::CONFIG_ZERO );
            arrIds.Add( Matrix::CONFIG_UNDETERMINED );

            DIALOG_APPENDNODE_ENUM_PRESELECTED( matrix_root, "type", matrix_type_labels, arrIds, mci.type )

            DIALOG_APPENDNODE_FLOAT( matrix_root, "x", mci.values[0] )
            DIALOG_APPENDNODE_FLOAT( matrix_root, "y", mci.values[1] )
            DIALOG_APPENDNODE_FLOAT( matrix_root, "z", mci.values[2] )
            DIALOG_APPENDNODE_FLOAT( matrix_root, "angle", Maths::RadToDeg( mci.values[3] ) )

        DIALOG_APPENDROOT_ITERATE_NODE_END
        DIALOG_SHOW


        dialog->SetSpecific0Counter( mat_chain.size() );
    }
}
