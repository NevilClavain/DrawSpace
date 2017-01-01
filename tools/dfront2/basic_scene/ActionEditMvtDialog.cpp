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

#include "ActionEditMvtDialog.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;


void ActionEditMvtDialog::Execute( void )
{
    void* id = BasicSceneMainFrame::GetInstance()->m_last_clicked_treeitem.GetID();
    if( BasicSceneMainFrame::GetInstance()->m_lin_nodes.count( id ) > 0 )
    {
        BasicSceneMainFrame::SceneNodeEntry<LinearMovement> lin_node = BasicSceneMainFrame::GetInstance()->m_lin_nodes[id];                    
        LinearMovement* linmvt = lin_node.scene_node->GetContent();

        dsreal curr_theta = linmvt->GetCurrentTheta();
        dsreal curr_phi = linmvt->GetCurrentPhi();
        dsreal curr_speed = linmvt->GetCurrentSpeed();

        DIALOG_ACTION_DECLARE( DIALOG_LINMVT_EDITION_TITLE )

        DIALOG_APPENDROOT_STRING( "scene name", lin_node.name );
        DIALOG_APPENDROOT_FLOAT( "current speed", curr_speed );
        DIALOG_APPENDROOT_FLOAT( "current theta", curr_theta );
        DIALOG_APPENDROOT_FLOAT( "current phi", curr_phi );

        DIALOG_APPLY
        DIALOG_SHOW
    }
    else if( BasicSceneMainFrame::GetInstance()->m_circ_nodes.count( id ) > 0 )
    {
        BasicSceneMainFrame::SceneNodeEntry<CircularMovement> circ_node = BasicSceneMainFrame::GetInstance()->m_circ_nodes[id];                    
        CircularMovement* circmvt = circ_node.scene_node->GetContent();

        dsreal curr_theta = circmvt->GetCurrentTheta();
        dsreal curr_phi = circmvt->GetCurrentPhi();
        dsreal curr_speed = circmvt->GetAngularSpeed();

        DIALOG_ACTION_DECLARE( DIALOG_CIRCMVT_EDITION_TITLE )

        DIALOG_APPENDROOT_STRING( "scene name", circ_node.name );
        DIALOG_APPENDROOT_FLOAT( "current angular speed", curr_speed );
        DIALOG_APPENDROOT_FLOAT( "current theta", curr_theta );
        DIALOG_APPENDROOT_FLOAT( "current phi", curr_phi );

        DIALOG_APPLY
        DIALOG_SHOW
    }
    else if( BasicSceneMainFrame::GetInstance()->m_ll_nodes.count( id ) > 0 )
    {
        BasicSceneMainFrame::SceneNodeEntry<LongLatMovement> ll_node = BasicSceneMainFrame::GetInstance()->m_ll_nodes[id];                    
        LongLatMovement* llmvt = ll_node.scene_node->GetContent();

        dsreal curr_theta = llmvt->GetCurrentTheta();
        dsreal curr_phi = llmvt->GetCurrentPhi();
        dsreal curr_alt = llmvt->GetCurrentAltitud();

        dsreal curr_longitud = llmvt->GetCurrentLongitud();
        dsreal curr_latitud = llmvt->GetCurrentLatitud();

        DIALOG_ACTION_DECLARE( DIALOG_LONGLATMVT_EDITION_TITLE )

        DIALOG_APPENDROOT_STRING( "scene name", ll_node.name );
        DIALOG_APPENDROOT_FLOAT( "current theta", curr_theta );
        DIALOG_APPENDROOT_FLOAT( "current phi", curr_phi );                   
        DIALOG_APPENDROOT_FLOAT( "current longitud", curr_longitud );
        DIALOG_APPENDROOT_FLOAT( "current latitud", curr_latitud );
        DIALOG_APPENDROOT_FLOAT( "current altitud", curr_alt );

        DIALOG_APPLY
        DIALOG_SHOW
    }

}
