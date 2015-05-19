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

#include "ActionAddMatrix.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;


void ActionAddMatrix::Execute( BasicSceneObjectPropertiesDialog* p_dialog )
{
    DIALOG_GETGRID

    DIALOG_SPECIFIC0_LABEL( "matrix %d", matrix_label )

    DIALOG_APPENDROOT_NODE( matrix_label, matrix_root )

    wxArrayString matrix_type_labels;

    matrix_type_labels.Add( "identity" );
    matrix_type_labels.Add( "scaling" );
    matrix_type_labels.Add( "translation" );
    matrix_type_labels.Add( "rotation" );
    matrix_type_labels.Add( "zero" );
    matrix_type_labels.Add( "undetermined" );

    DIALOG_APPENDNODE_ENUM( matrix_root, "type", matrix_type_labels )

    DIALOG_APPENDNODE_FLOAT( matrix_root, "x", 0.0 )
    DIALOG_APPENDNODE_FLOAT( matrix_root, "y", 0.0 )
    DIALOG_APPENDNODE_FLOAT( matrix_root, "z", 0.0 )
    DIALOG_APPENDNODE_FLOAT( matrix_root, "angle", 0.0 )

    DIALOG_FINALIZE
}