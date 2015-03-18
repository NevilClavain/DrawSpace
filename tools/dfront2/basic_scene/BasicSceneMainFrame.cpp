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
#include "drawspace.h"
#include "adapters.h"
//#include "BasicSceneObjectPropertiesDialog.h"
#include "buildobjects.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

BasicSceneMainFrame::BasicSceneMainFrame( wxWindow* parent ) : MainFrame( parent, wxID_ANY, "DFront Basic Scene", wxDefaultPosition, wxSize( 1264,600 ), wxCAPTION | wxCLOSE_BOX ),
m_glready( false ),
m_scenegraphlistctrl_currentindex( -1 ),
m_mvtslistctrl_currentindex( -1 ),
m_cameraslistctrl_currentindex( -1 ),
m_regslistctrl_currentindex( -1 ),
m_last_xmouse( 0 ),
m_last_ymouse( 0 ),
m_current_camera( NULL ),
m_display_framerate( false ),
m_display_currentcamera( false ),
m_console_font( 8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false )
{

   
    m_consoleinput_textCtrl->SetFont( m_console_font );
    m_consoleoutput_textCtrl->SetFont( m_console_font );


    m_timercb = new TimerCallback( this, &BasicSceneMainFrame::on_timer );

    m_timer.AddTimer( "timer", 100, m_timercb );
    m_timer.SetTimerState( "timer", true );

    m_scripting = DrawSpace::Core::SingletonPlugin<Scripting>::GetInstance()->m_interface;

    m_scripting->Initialize();

    m_scripting_error_cb = new ScriptingErrorCallback( this, &BasicSceneMainFrame::on_scripting_error );
    m_scripting->RegisterScriptErrorHandler( m_scripting_error_cb );

    m_scripting_calls_cb = new ScriptingCallsCallback( this, &BasicSceneMainFrame::on_scripting_calls );
    m_scripting->RegisterScriptCallsHandler( m_scripting_calls_cb );

    ////////////////////////////////////

    // root; seul la creation de scenenodegraph est possible
    //m_scenegraphs_masks[0].push_back( CONTEXTMENU_NEWSCENENODEGRAPH );

    PopupMenuEntry pme_separator = { CONTEXTMENU_SEPARATOR, "" };
    PopupMenuEntry pme_edittransformnode = { CONTEXTMENU_EDIT_TRANSFORMNODE, "Edit transformation..." };

    PopupMenuEntry pme_newscenenodegraph = { CONTEXTMENU_NEWSCENENODEGRAPH, "New scenenodegraph..." };
    PopupMenuEntry pme_newspacebox = { CONTEXTMENU_NEWSPACEBOX, "New spacebox..." };
    PopupMenuEntry pme_newchunk = { CONTEXTMENU_NEWCHUNK, "New chunk..." };
    PopupMenuEntry pme_newinertbody = { CONTEXTMENU_NEWINERTBODY, "New inertbody..." };
    PopupMenuEntry pme_newcollider = { CONTEXTMENU_NEWCOLLIDER, "New collider..." };
    PopupMenuEntry pme_newrocket = { CONTEXTMENU_NEWROCKET, "New rocket..." };
    PopupMenuEntry pme_neworbit = { CONTEXTMENU_NEWORBIT, "New orbit..." };
    PopupMenuEntry pme_neworbiter = { CONTEXTMENU_NEWORBITER, "New orbiter..." };
    PopupMenuEntry pme_newplanet = { CONTEXTMENU_NEWPLANET, "New planet..." };
    PopupMenuEntry pme_newtransfo = { CONTEXTMENU_NEWTRANSFO, "New transformation..." };
    PopupMenuEntry pme_newcamera = { CONTEXTMENU_NEWCAMERA, "New camera point..." };
    PopupMenuEntry pme_newlinearmvt = { CONTEXTMENU_NEWLINEARMVT, "New linear movement..." };
    PopupMenuEntry pme_newcircularmvt = { CONTEXTMENU_NEWCIRCULARMVT, "New circular movement..." };
    PopupMenuEntry pme_newfpsmvt = { CONTEXTMENU_NEWFPSMVT, "New fps movement..." };
    PopupMenuEntry pme_newfreemvt = { CONTEXTMENU_NEWFREEMVT, "New free movement..." };
    PopupMenuEntry pme_newheadmvt = { CONTEXTMENU_NEWHEADMVT, "New head movement..." };
    PopupMenuEntry pme_newlonglatmvt = { CONTEXTMENU_NEWLONGLATMVT, "New longlat movement..." };
    PopupMenuEntry pme_newspectatormvt = { CONTEXTMENU_NEWSPECTATORMVT, "New spectator movement..." };


    ///////////////////////////////////////////////////////////////////

    m_scenegraphs_masks[DRAWSPACE_MASK].push_back( pme_newscenenodegraph );

    ///////////////////////////////////////////////////////////////////

    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_newspacebox );
    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_newchunk );

    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_separator );

    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_newinertbody );
    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_newcollider );
    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_newrocket );
    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_neworbit );
    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_newplanet );

    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_separator );

    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_newtransfo );
    
    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_separator );

    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_newlinearmvt );
    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_newcircularmvt );
    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_newfpsmvt );
    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_newfreemvt );
    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_newspectatormvt );

    ///////////////////////////////////////////////////////////////////

    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newspacebox );
    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newchunk );
    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newcollider );
    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_neworbit );
    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newplanet );

    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_separator );

    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newtransfo );

    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_separator );

    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newlinearmvt );
    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newcircularmvt );
    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newlonglatmvt );
    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newheadmvt );

    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_separator );

    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_edittransformnode );


    m_applybutton_clicked_cb = new CallBack<BasicSceneMainFrame, void, BasicSceneObjectPropertiesDialog*>( this, &BasicSceneMainFrame::on_applybutton_clicked );
    m_specificbutton0_clicked_cb = new CallBack<BasicSceneMainFrame, void, BasicSceneObjectPropertiesDialog*>( this, &BasicSceneMainFrame::on_specificbutton0_clicked );
    m_specificbutton1_clicked_cb = new CallBack<BasicSceneMainFrame, void, BasicSceneObjectPropertiesDialog*>( this, &BasicSceneMainFrame::on_specificbutton1_clicked );

}

void BasicSceneMainFrame::on_scripting_error( const dsstring& p_error )
{
    PrintOutputConsole( p_error );
    wxMessageBox( "Scripting syntax error", "Script error", wxICON_ERROR );
}


void BasicSceneMainFrame::build_popupmenu( int p_level, wxMenu& p_menu )
{   
    for( size_t i = 0; i < m_scenegraphs_masks[p_level].size(); i++ )
    {
        if( CONTEXTMENU_SEPARATOR == m_scenegraphs_masks[p_level][i].id )
        {
            p_menu.AppendSeparator();
        }
        else
        {
            p_menu.Append( m_scenegraphs_masks[p_level][i].id, m_scenegraphs_masks[p_level][i].text.c_str() );
        }
    }
}


void BasicSceneMainFrame::on_scripting_calls( DrawSpace::Core::PropertyPool& p_propertypool )
{
    dsstring script_call_id = p_propertypool.GetPropValue<dsstring>( "script_call_id" );

    if( "global:print" == script_call_id )
    {
        dsstring text = p_propertypool.GetPropValue<dsstring>( "text" );
        PrintOutputConsole( text );
    }
    else if( "DrawSpace:DisplayFramerate" == script_call_id )
    {
        bool state = p_propertypool.GetPropValue<bool>( "state" );
        m_display_framerate = state;
    }
    else if( "DrawSpace:DisplayCurrentCamera" == script_call_id )
    {
        bool state = p_propertypool.GetPropValue<bool>( "state" );
        m_display_currentcamera = state;
    }
    else if( "DrawSpace:CreateSceneNodeGraph" == script_call_id )
    {
        dsstring alias = p_propertypool.GetPropValue<dsstring>( "name" );

        SceneNodeGraphEntry entry;

        entry.name = alias;
        entry.scenenodegraph = new SceneNodeGraph();
        entry.treeitemid = m_scenegraphs_treeCtrl->AppendItem( m_scenegraphs_root_item, alias.c_str(), SCENEGRAPH_ICON_INDEX );
        m_scenenodegraphs[entry.treeitemid.GetID()] = entry;

        m_scenegraphs_treeCtrl->ExpandAllChildren( m_scenegraphs_root_item );
    }
    else if( "SpaceBoxBuilder:BuildIt" == script_call_id )
    {
        Spacebox::Descriptor descriptor = p_propertypool.GetPropValue<Spacebox::Descriptor>( "descr" );

        MetadataScenegraphEntry entry;
        dsstring error_msg;
        bool status = BuildSpaceBox( descriptor, entry, error_msg );

        if( status )
        {
            m_scenegraph.RegisterNode( entry.node );

            //m_metada_scenegraph[descriptor.scene_name] = entry;

            // call UpdateOutputQueue() for all passes
            
            for( size_t i = 0; i < m_ordered_configs.size(); i++ )
            {
                Pass* pass = dynamic_cast<Pass*>( m_ordered_configs[i] );
                if( pass )
                {
                    pass->GetRenderingQueue()->UpdateOutputQueue();
                }
            }

            //wxWidgetAdapter::GetInstance()->AdaptScenegraphList( &m_scenegraph, m_scenegraph_listCtrl );
        }
        else
        {
            wxMessageBox( error_msg, "Script error", wxICON_ERROR );
        }
    }
    else if( "SpaceBoxBuilder:ClearMatrixStack" == script_call_id )
    {
        dsstring scene_name = p_propertypool.GetPropValue<dsstring>( "name" );

    }
    else if( "SpaceBoxBuilder:AddMatrix" == script_call_id )
    {
        dsstring mat_type = p_propertypool.GetPropValue<dsstring>( "mat_type" );

        dsreal x, y, z, angle;
        dsstring xreg, yreg, zreg, anglereg;
        TransformationMatrixDescriptor mat_descr;
        
        dsstring scene_name = p_propertypool.GetPropValue<dsstring>( "name" );


    }
    else if( "DrawSpace:CreateConstRegister" == script_call_id )
    {
        /*
        dsstring reg_name = p_propertypool.GetPropValue<dsstring>( "reg_name" );
        dsreal reg_val = p_propertypool.GetPropValue<dsreal>( "reg_val" );

        BasicSceneMainFrame::RegisterEntry register_entry;
        memset( &register_entry, 0, sizeof( BasicSceneMainFrame::RegisterEntry ) );

        register_entry.mode = BasicSceneMainFrame::REGISTER_CONSTANT;
        register_entry.const_value = reg_val;
        register_entry.id = wxWidgetAdapter::GetInstance()->m_register_index++;
        wxWidgetAdapter::GetInstance()->InitializeRegister( &register_entry );

        m_registers[reg_name] = register_entry;
        wxWidgetAdapter::GetInstance()->AdaptRegistersList( &m_registers, m_registers_listCtrl );
        */
    }
    else if( "DrawSpace:ModifyConstRegisterValue" == script_call_id )
    {
        /*
        dsstring reg_name = p_propertypool.GetPropValue<dsstring>( "reg_name" );
        dsreal reg_val = p_propertypool.GetPropValue<dsreal>( "reg_val" );

        if( m_registers.count( reg_name ) > 0 )
        {
            if( REGISTER_CONSTANT == m_registers[reg_name].mode )
            {
                m_registers[reg_name].const_value = reg_val;
            }
            else
            {
                wxMessageBox( "Specified register is not a constant type : " + reg_name, "Script error", wxICON_ERROR );
            }
        }
        else
        {
            wxMessageBox( "Unknown register name : " + reg_name, "Script error", wxICON_ERROR );
        }
        */
    }
    else if( "DrawSpace:CreateVarRegister" == script_call_id )
    {
        /*
        dsstring reg_name = p_propertypool.GetPropValue<dsstring>( "reg_name" );
        dsstring var_mode = p_propertypool.GetPropValue<dsstring>( "var_mode" );
        dsreal init_val = p_propertypool.GetPropValue<dsreal>( "init_val" );
        dsreal speed = p_propertypool.GetPropValue<dsreal>( "speed" );
        dsreal range_inf = p_propertypool.GetPropValue<dsreal>( "range_inf" );
        dsreal range_sup = p_propertypool.GetPropValue<dsreal>( "range_sup" );

        BasicSceneMainFrame::RegisterEntry register_entry;
        memset( &register_entry, 0, sizeof( BasicSceneMainFrame::RegisterEntry ) );

        register_entry.mode = BasicSceneMainFrame::REGISTER_VARIABLE;

        if( "translation_simple" == var_mode )
        {
            register_entry.variable_mode = BasicSceneMainFrame::REGISTER_VARIABLE_TRANSLATION_SIMPLE;
        }
        else if( "translation_roundtrip" == var_mode )
        {
            register_entry.variable_mode = BasicSceneMainFrame::REGISTER_VARIABLE_TRANSLATION_ROUNDTRIP;
        }
        else if( "angular_simple" == var_mode )
        {
            register_entry.variable_mode = BasicSceneMainFrame::REGISTER_VARIABLE_ANGULAR_SIMPLE;
        }
        else if( "angular_roundtrip" == var_mode )
        {
            register_entry.variable_mode = BasicSceneMainFrame::REGISTER_VARIABLE_ANGULAR_ROUNDTRIP;
        }

        register_entry.variable_initial_value = init_val;
        register_entry.variable_speed = speed;
        register_entry.variable_range_inf = range_inf;
        register_entry.variable_range_sup = range_sup;

        register_entry.id = wxWidgetAdapter::GetInstance()->m_register_index++;
        wxWidgetAdapter::GetInstance()->InitializeRegister( &register_entry );
        m_registers[reg_name] = register_entry;
        wxWidgetAdapter::GetInstance()->AdaptRegistersList( &m_registers, m_registers_listCtrl );

        */
    }
    else if( "DrawSpace:ResetVarRegister" == script_call_id )
    {
        /*
        dsstring reg_name = p_propertypool.GetPropValue<dsstring>( "reg_name" );

        if( m_registers.count( reg_name ) > 0 )
        {
            if( REGISTER_VARIABLE == m_registers[reg_name].mode )
            {
                wxWidgetAdapter::GetInstance()->InitializeRegister( &m_registers[reg_name] );      
            }
            else
            {
                wxMessageBox( "Specified register is not a variable type : " + reg_name, "Script error", wxICON_ERROR );
            }
        }
        else
        {
            wxMessageBox( "Unknown register name : " + reg_name, "Script error", wxICON_ERROR );
        }
        */
    }
    else if( "DrawSpace:SetVarRegisterState" == script_call_id )
    {
        /*
        dsstring reg_name = p_propertypool.GetPropValue<dsstring>( "reg_name" );
        bool state = p_propertypool.GetPropValue<bool>( "state" );

        if( m_registers.count( reg_name ) > 0 )
        {
            if( REGISTER_VARIABLE == m_registers[reg_name].mode )
            {
                m_registers[reg_name].state = state;
            }
            else
            {
                wxMessageBox( "Specified register is not a variable type : " + reg_name, "Script error", wxICON_ERROR );
            }
        }
        else
        {
            wxMessageBox( "Unknown register name : " + reg_name, "Script error", wxICON_ERROR );
        }
        */
    }
    else if( "DrawSpace:SetVarRegisterInitVal" == script_call_id )
    {
        /*
        dsstring reg_name = p_propertypool.GetPropValue<dsstring>( "reg_name" );
        dsreal init_val = p_propertypool.GetPropValue<dsreal>( "init_val" );

        if( m_registers.count( reg_name ) > 0 )
        {
            if( REGISTER_VARIABLE == m_registers[reg_name].mode )
            {
                m_registers[reg_name].variable_initial_value = init_val;
            }
            else
            {
                wxMessageBox( "Specified register is not a variable type : " + reg_name, "Script error", wxICON_ERROR );
            }
        }
        else
        {
            wxMessageBox( "Unknown register name : " + reg_name, "Script error", wxICON_ERROR );
        }
        */
    }
    else if( "DrawSpace:SetVarRegisterSpeed" == script_call_id )
    {
        /*
        dsstring reg_name = p_propertypool.GetPropValue<dsstring>( "reg_name" );
        dsreal speed = p_propertypool.GetPropValue<dsreal>( "speed" );

        if( m_registers.count( reg_name ) > 0 )
        {
            if( REGISTER_VARIABLE == m_registers[reg_name].mode )
            {
                m_registers[reg_name].variable_speed = speed;
            }
            else
            {
                wxMessageBox( "Specified register is not a variable type : " + reg_name, "Script error", wxICON_ERROR );
            }
        }
        else
        {
            wxMessageBox( "Unknown register name : " + reg_name, "Script error", wxICON_ERROR );
        }
        */
    }
    else if( "DrawSpace:SetVarRegisterRange" == script_call_id )
    {
        /*
        dsstring reg_name = p_propertypool.GetPropValue<dsstring>( "reg_name" );
        dsreal range_min = p_propertypool.GetPropValue<dsreal>( "range_min" );
        dsreal range_max = p_propertypool.GetPropValue<dsreal>( "range_max" );

        if( m_registers.count( reg_name ) > 0 )
        {
            if( REGISTER_VARIABLE == m_registers[reg_name].mode )
            {
                m_registers[reg_name].variable_range_inf = range_min;
                m_registers[reg_name].variable_range_sup = range_max;
            }
            else
            {
                wxMessageBox( "Specified register is not a variable type : " + reg_name, "Script error", wxICON_ERROR );
            }
        }
        else
        {
            wxMessageBox( "Unknown register name : " + reg_name, "Script error", wxICON_ERROR );
        }
        */
    }
    else if( "MovementBuilder:Lua_BuildIt" == script_call_id )
    {


        //RegisterMovement( name, movement_entry );
    }
    else if( "TransformationNodeBuilder:LinkTo" == script_call_id )
    {
        dsstring scene_name = p_propertypool.GetPropValue<dsstring>( "scene_name" );
        dsstring scenegraph_name = p_propertypool.GetPropValue<dsstring>( "scenegraph_name" );
        dsstring parent_name = p_propertypool.GetPropValue<dsstring>( "parent_name" );
        BaseSceneNode* node = p_propertypool.GetPropValue<BaseSceneNode*>( "node" );

        wxTreeItemId parent_tree_item;
        void* parent_id = NULL;

        // search for scenenodegraph

        bool scene_found = false;
        SceneNodeGraphEntry scenenodegraph_entry;

        for( std::map<void*, SceneNodeGraphEntry>::iterator it = m_scenenodegraphs.begin(); it != m_scenenodegraphs.end(); ++it )
        {
            if( it->second.name == scenegraph_name )
            {
                scenenodegraph_entry = it->second;
                scene_found = true;                
                break;
            }
        }

        bool parent_found = false;
        BaseSceneNode* parent = NULL;


        for( std::map<void*, DrawSpace::Core::BaseSceneNode*>::iterator it = m_tree_nodes.begin(); it != m_tree_nodes.end(); ++it )
        {
            dsstring node_scenename;
            it->second->GetSceneName( node_scenename );

            if( node_scenename == parent_name )
            {
                parent_found = true;
                parent = it->second;
                parent_id = it->first;
                break;
            }
        }

        if( !parent_found )
        {
            for( std::map<void*, SceneNodeGraphEntry>::iterator it = m_scenenodegraphs.begin(); it != m_scenenodegraphs.end(); ++it )
            {
                if( it->second.name == parent_name )
                {
                    parent_found = true;
                    parent_id = it->first;
                    break;
                }
            }
        }

        if( !scene_found )
        {
            wxMessageBox( "Transformation node, unknown scenegraph name : " + scenegraph_name, "Script error", wxICON_ERROR );
            return;           
        }

        else if( !parent_found )
        {
            wxMessageBox( "Transformation node, unknown parent name : " + parent_name, "Script error", wxICON_ERROR );
            return;
        }

        SceneNode<Transformation>* transfo_node = static_cast<SceneNode<Transformation>*>( node );


        scenenodegraph_entry.scenenodegraph->RegisterNode( node );

        if( parent )
        {            
            transfo_node->LinkTo( parent );
            parent_tree_item = m_transformation_nodes[parent_id].treeitemid;
        }
        else
        {
            scenenodegraph_entry.scenenodegraph->AddNode( node );
            parent_tree_item = scenenodegraph_entry.treeitemid;
        }

        
        // GUI : add item in the tree
        wxTreeItemId treeitemid = m_scenegraphs_treeCtrl->AppendItem( parent_tree_item, scene_name.c_str(), TRANSFO_ICON_INDEX );
        m_scenegraphs_treeCtrl->ExpandAllChildren( parent_tree_item );
        
        // record the new transformation node and associated metadata

        BasicSceneMainFrame::TransformationNodeEntry t_entry;

        t_entry.name = scene_name;
        t_entry.transformation = transfo_node;
        t_entry.treeitemid = treeitemid;

        /*
        BasicSceneMainFrame::TransformationMatrixDescriptor transfdescr;
        transfdescr.ope = BasicSceneMainFrame::TRANSFORMATIONMATRIX_IDENTITY;

        t_entry.matrix_stack_descr.push_back( transfdescr );
        */

        m_transformation_nodes[t_entry.treeitemid.GetID()] = t_entry;
        m_tree_nodes[t_entry.treeitemid.GetID()] = transfo_node;
    }
}

void BasicSceneMainFrame::ExecStartupScript( const dsstring& p_scriptfilepath )
{
    m_scripting->ExecFile( p_scriptfilepath.c_str() );
}

void BasicSceneMainFrame::PrintOutputConsole( const dsstring& p_text )
{
    m_consoleoutput_textCtrl->AppendText( wxString( p_text.c_str() ) );
    m_consoleoutput_textCtrl->AppendText( wxString( "\n" ) );
}

wxNotebook* BasicSceneMainFrame::GetNoteBook( void )
{
    return m_notebook2;
}

void BasicSceneMainFrame::SetWindowDims( long p_w_width, long p_w_height )
{
    m_w_width = p_w_width;
    m_w_height = p_w_height;
}


void BasicSceneMainFrame::on_timer( const dsstring& p_timername )
{
    
}

void BasicSceneMainFrame::OnClose( wxCloseEvent& p_event )
{
    DrawSpace::Core::SingletonPlugin<Scripting>::GetInstance()->m_interface->Shutdown();
    Destroy();
}


void BasicSceneMainFrame::OnKeyDown( wxKeyEvent& p_event )
{

}
void BasicSceneMainFrame::OnKeyUp( wxKeyEvent& p_event )
{

}

void BasicSceneMainFrame::OnMouseMotion( wxMouseEvent& p_event )
{
    if( !p_event.Dragging() )
    {
        return;
    }

    wxCoord curr_xmouse = p_event.GetX();
    wxCoord curr_ymouse = p_event.GetY();

    if( curr_xmouse < m_w_width / 4 )
    {
        curr_xmouse = 3 * m_w_width / 4;
        WarpPointer( curr_xmouse, curr_ymouse );
        m_last_xmouse = curr_xmouse;
        m_last_ymouse = curr_ymouse;
        return;
    }
    else if( curr_xmouse > 3 * m_w_width / 4 )
    {
        curr_xmouse = m_w_width / 4;
        WarpPointer( curr_xmouse, curr_ymouse );
        m_last_xmouse = curr_xmouse;
        m_last_ymouse = curr_ymouse;
        return;
    }

    if( curr_ymouse < m_w_height / 4 )
    {
        curr_ymouse = 3 * m_w_height / 4;
        WarpPointer( curr_xmouse, curr_ymouse );
        m_last_xmouse = curr_xmouse;
        m_last_ymouse = curr_ymouse;
        return;
    }
    else if( curr_ymouse > 3 * m_w_height / 4 )
    {
        curr_ymouse = m_w_height / 4;
        WarpPointer( curr_xmouse, curr_ymouse );
        m_last_xmouse = curr_xmouse;
        m_last_ymouse = curr_ymouse;
        return;
    }


    int delta_x = curr_xmouse - m_last_xmouse;
    int delta_y = curr_ymouse - m_last_ymouse;
    

    m_last_xmouse = curr_xmouse;
    m_last_ymouse = curr_ymouse;
}


void BasicSceneMainFrame::OnIdle( wxIdleEvent& p_event )
{
    if( m_glready )
    {
        int index = m_cameraslist_comboBox->GetSelection();
        CameraPoint* camera = (CameraPoint*)m_cameraslist_comboBox->GetClientData( index );

        if( camera != m_current_camera )
        {
            if( NULL == camera )
            {
                m_scenegraph.SetCurrentCamera( "" );
            }
            else
            {
                dsstring camera_name;
                camera->GetSceneName( camera_name );
                m_scenegraph.SetCurrentCamera( camera_name );
            }
            m_current_camera = camera;
        }

        
        
        DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
       

        for( std::map<void*, SceneNodeGraphEntry>::iterator it = m_scenenodegraphs.begin(); it != m_scenenodegraphs.end(); ++it )
        {
            it->second.scenenodegraph->ComputeTransformations( m_timer );
        }

        for( size_t i = 0; i < m_ordered_configs.size(); i++ )
        {
            Pass* pass = dynamic_cast<Pass*>( m_ordered_configs[i] );
            if( pass )
            {
                pass->GetRenderingQueue()->Draw();
            }
        }

        if( m_display_framerate )
        {
            renderer->DrawText( 255, 0, 0, 10, 20, "%d fps", m_timer.GetFPS() );
        }

        if( m_display_currentcamera )
        {            
            dsstring camera_name;
            m_scenegraph.GetCurrentCameraName( camera_name );
            if( "" == camera_name )
            {
                camera_name = "...";
            }

            renderer->DrawText( 255, 0, 0, 10, 40, "current camera : %s", camera_name.c_str() );
        }
        


        renderer->FlipScreen();

        m_timer.Update();
        if( m_timer.IsReady() )
        {

        }

        // wxWidget framework specific !
        p_event.RequestMore( true );
    }
}

void BasicSceneMainFrame::SetGLReady( void )
{
    m_glready = true;
}

void BasicSceneMainFrame::Update( void )
{
    wxWidgetAdapter::GetInstance()->AdaptAssetsList( m_assets_listCtrl );
    wxWidgetAdapter::GetInstance()->AdaptConfigsList( m_configs_listCtrl );
    wxWidgetAdapter::GetInstance()->AdaptPassesList( m_passes_listCtrl );        
    wxWidgetAdapter::GetInstance()->AdaptCameraListComboBox( &m_scenegraph, m_cameraslist_comboBox );

    

    ConfigsBase::GetInstance()->GetOrderedConfigsInstancesList( m_ordered_configs );
    for( size_t i = 0; i < m_ordered_configs.size(); i++ )
    {
        Pass* pass = dynamic_cast<Pass*>( m_ordered_configs[i] );
        if( pass )
        {
            m_scenegraph.RegisterPass( pass );
            pass->GetRenderingQueue()->UpdateOutputQueue();
        }
    }

    

    
    wxImageList* pImageList = new wxImageList( DFRONT_ICON_DIM, DFRONT_ICON_DIM, TRUE );


    wxBitmap bmp_camera( "icon_camera.bmp", wxBITMAP_TYPE_BMP );
    wxBitmap bmp_chunk( "icon_chunk.bmp", wxBITMAP_TYPE_BMP );
    wxBitmap bmp_collider( "icon_collider.bmp", wxBITMAP_TYPE_BMP );
    wxBitmap bmp_inertbody( "icon_inertbody.bmp", wxBITMAP_TYPE_BMP );
    wxBitmap bmp_movement( "icon_movement.bmp", wxBITMAP_TYPE_BMP );
    wxBitmap bmp_orbit( "icon_orbit.bmp", wxBITMAP_TYPE_BMP );
    wxBitmap bmp_orbiter( "icon_orbiter.bmp", wxBITMAP_TYPE_BMP );
    wxBitmap bmp_planet( "icon_planet.bmp", wxBITMAP_TYPE_BMP );
    wxBitmap bmp_rocket( "icon_rocket.bmp", wxBITMAP_TYPE_BMP );
    wxBitmap bmp_spacebox( "icon_spacebox.bmp", wxBITMAP_TYPE_BMP );
    wxBitmap bmp_transfo( "icon_transfo.bmp", wxBITMAP_TYPE_BMP );
    wxBitmap bmp_scenegraph( "icon_scenegraph.bmp", wxBITMAP_TYPE_BMP );
    wxBitmap bmp_drawspace( "icon_drawspace.bmp", wxBITMAP_TYPE_BMP );


    
    pImageList->Add( bmp_camera );
    pImageList->Add( bmp_chunk );
    pImageList->Add( bmp_collider );
    pImageList->Add( bmp_inertbody );
    pImageList->Add( bmp_movement );
    pImageList->Add( bmp_orbit );
    pImageList->Add( bmp_orbiter );
    pImageList->Add( bmp_planet );
    pImageList->Add( bmp_rocket );
    pImageList->Add( bmp_spacebox );
    pImageList->Add( bmp_transfo );
    pImageList->Add( bmp_scenegraph );
    pImageList->Add( bmp_drawspace );
    

    m_scenegraphs_treeCtrl->AssignImageList( pImageList );
    
    m_scenegraphs_root_item = m_scenegraphs_treeCtrl->AddRoot( "DrawSpace", DRAWSPACE_ICON_INDEX );
}

void BasicSceneMainFrame::OnAssetsListItemActivated( wxListEvent& p_event )
{
    long sel_index = p_event.GetIndex();

    Asset* asset = (Asset*)m_assets_listCtrl->GetItemData( sel_index );

    Texture* texture = dynamic_cast<Texture*>( asset );
    if( texture )
    {
        Asset::PropertiesMap props;
        texture->GetPropertiesMap( props );

        dsstring assetname = props["assetname"].GetPropValue<dsstring>();
        dsstring filepath = props["filepath"].GetPropValue<dsstring>();
        bool rendertarget = props["rendertarget"].GetPropValue<bool>();
        unsigned long rendetarget_w = props["rendertarget_size"].GetPropValue<unsigned long>( "width" );
        unsigned long rendetarget_h = props["rendertarget_size"].GetPropValue<unsigned long>( "height" );

        
        DIALOG_DECLARE( DIALOG_TEXTURE_PROPS_TITLE )
        DIALOG_APPENDROOT_STRING( "assetname", assetname )
        DIALOG_APPENDROOT_STRING( "filepath", filepath )
        DIALOG_APPENDROOT_BOOL( "rendertarget", rendertarget )

        if( rendertarget )
        {
            DIALOG_APPENDROOT_NODE( "rendertarget dims", rendertarget_dimsnode )

            DIALOG_APPENDNODE_INTEGER( rendertarget_dimsnode, "width", rendetarget_w )
            DIALOG_APPENDNODE_INTEGER( rendertarget_dimsnode, "height", rendetarget_h )
        }

        DIALOG_SHOW

    }

    Shader* shader = dynamic_cast<Shader*>( asset );
    if( shader )
    {
        Asset::PropertiesMap props;
        shader->GetPropertiesMap( props );

        dsstring assetname = props["assetname"].GetPropValue<dsstring>();
        dsstring filepath = props["filepath"].GetPropValue<dsstring>();
        bool compiled = props["compiled"].GetPropValue<bool>();
        
        DIALOG_DECLARE( DIALOG_SHADER_PROPS_TITLE )
        DIALOG_APPENDROOT_STRING( "assetname", assetname )
        DIALOG_APPENDROOT_STRING( "filepath", filepath )
        DIALOG_APPENDROOT_BOOL( "compiled", compiled )

        DIALOG_SHOW
    }

    Font* font = dynamic_cast<Font*>( asset );
    if( font )
    {
        Asset::PropertiesMap props;
        font->GetPropertiesMap( props );

        dsstring assetname = props["assetname"].GetPropValue<dsstring>();
        dsstring plugin = props["plugin"].GetPropValue<dsstring>();
        dsstring texturefilepath = props["filespath"].GetPropValue<dsstring>( "texturefilepath" );
        dsstring metricsfilepath = props["filespath"].GetPropValue<dsstring>( "metricsfilepath" );

        DIALOG_DECLARE( DIALOG_FONT_PROPS_TITLE )

        DIALOG_APPENDROOT_STRING( "assetname", assetname )
        DIALOG_APPENDROOT_STRING( "plugin", plugin )
        DIALOG_APPENDROOT_STRING( "texturefilepath", texturefilepath )
        DIALOG_APPENDROOT_STRING( "metricsfilepath", metricsfilepath )

        DIALOG_SHOW
    }

    Meshe* meshe = dynamic_cast<Meshe*>( asset );
    if( meshe )
    {
        Asset::PropertiesMap props;
        meshe->GetPropertiesMap( props );

        dsstring assetname = props["assetname"].GetPropValue<dsstring>();
        dsstring filepath = props["filepath"].GetPropValue<dsstring>();
        long index = props["index"].GetPropValue<long>();
        dsstring plugin = props["plugin"].GetPropValue<dsstring>();

        DIALOG_DECLARE( DIALOG_MESHE_PROPS_TITLE )

        DIALOG_APPENDROOT_STRING( "assetname", assetname )
        DIALOG_APPENDROOT_STRING( "filepath", filepath )
        DIALOG_APPENDROOT_INTEGER( "index", index )
        DIALOG_APPENDROOT_STRING( "plugin", plugin )

        DIALOG_SHOW
    }
}

void BasicSceneMainFrame::OnConfigsListItemActivated( wxListEvent& p_event )
{
    long sel_index = p_event.GetIndex();

    Configurable* config = (Configurable*)m_configs_listCtrl->GetItemData( sel_index );

    Fx* fx = dynamic_cast<Fx*>( config );
    if( fx )
    {               
        Configurable::PropertiesMap props;
        fx->GetPropertiesMap( props );


        DIALOG_DECLARE( DIALOG_FX_PROPS_TITLE )

        std::vector<dsstring> shaders_list = props["shaders"].GetPropValue<std::vector<dsstring>>();

        if( shaders_list.size() > 0 )
        {
            DIALOG_APPENDROOT_NODE( "shaders", shaders_root )

            DIALOG_BUILD_LABELS( shaders_list.size(), "shader %d", shader_labels )
            DIALOG_APPENDNODE_ITERATE( shaders_root, shaders_list[i], DIALOG_APPENDNODE_STRING, shader_labels )
        }

        std::vector<RenderState> rsin_list = props["renderstates_in"].GetPropValue<std::vector<RenderState>>();

        if( rsin_list.size() > 0 )
        {
            DIALOG_APPENDROOT_NODE( "renderstates in", rsin_root )

            DIALOG_BUILD_LABELS( rsin_list.size(), "%d", rsin_labels )


            DIALOG_APPENDNODE_ITERATE_NODE_BEGIN( rsin_root, i, rsin_labels, rsin )

                RenderState rs = rsin_list[i];
                dsstring renderstate_op;
                dsstring renderstate_arg;

                rs.GetOperationToString( renderstate_op );
                rs.GetArg( renderstate_arg );

                DIALOG_APPENDNODE_STRING( rsin, "operation", renderstate_op )
                DIALOG_APPENDNODE_STRING( rsin, "argument", renderstate_arg )

            DIALOG_APPENDNODE_ITERATE_NODE_END
        }

        std::vector<RenderState> rsout_list = props["renderstates_out"].GetPropValue<std::vector<RenderState>>();
        if( rsout_list.size() > 0 )
        {
            DIALOG_APPENDROOT_NODE( "renderstates out", rsout_root )

            DIALOG_BUILD_LABELS( rsout_list.size(), "%d", rsout_labels )

            DIALOG_APPENDNODE_ITERATE_NODE_BEGIN( rsout_root, i, rsout_labels, rsout )

                RenderState rs = rsout_list[i];
                dsstring renderstate_op;
                dsstring renderstate_arg;

                rs.GetOperationToString( renderstate_op );
                rs.GetArg( renderstate_arg );

                DIALOG_APPENDNODE_STRING( rsout, "operation", renderstate_op )
                DIALOG_APPENDNODE_STRING( rsout, "argument", renderstate_arg )

            DIALOG_APPENDNODE_ITERATE_NODE_END
        }

        DIALOG_SHOW            
    }

    build_passes_infos_dialog( config );
}

void BasicSceneMainFrame::build_passes_infos_dialog( DrawSpace::Core::Configurable* p_config )
{ 
    IntermediatePass* ipass = dynamic_cast<IntermediatePass*>( p_config );
    FinalPass* fpass = dynamic_cast<FinalPass*>( p_config );

    if( ipass || fpass )
    {
        Configurable::PropertiesMap props;
        dsstring title;

        if( ipass )
        {            
            ipass->GetPropertiesMap( props );
            title = DIALOG_IPASS_PROPS_TITLE;
        }
        else
        {            
            fpass->GetPropertiesMap( props );
            title = DIALOG_FPASS_PROPS_TITLE;
        }

        DIALOG_DECLARE( title )

        dsstring passname = props["passname"].GetPropValue<dsstring>();
        DIALOG_APPENDROOT_STRING( "name", passname )

        if( ipass )
        {
            bool targetdimsfromrenderer = props["targetdimsfromrenderer"].GetPropValue<bool>();

            DIALOG_APPENDROOT_BOOL( "target dims from renderer", targetdimsfromrenderer );

            if( !targetdimsfromrenderer )
            {
                long targetdims_width = props["targetdims"].GetPropValue<long>( "width" );
                long targetdims_height = props["targetdims"].GetPropValue<long>( "height" );

                DIALOG_APPENDROOT_NODE( "target dims", targetdims_root )
                DIALOG_APPENDNODE_INTEGER( targetdims_root, "width", targetdims_width )
                DIALOG_APPENDNODE_INTEGER( targetdims_root, "height", targetdims_height )
            }
        }


        DIALOG_APPENDROOT_BOOL( "enable depth clear", props["enabledepthclear"].GetPropValue<bool>() );
        DIALOG_APPENDROOT_BOOL( "enable target clear", props["enabletargetclear"].GetPropValue<bool>() );

        if( props["enabletargetclear"].GetPropValue<bool>() )
        {
            DIALOG_APPENDROOT_NODE( "target clear color", targetclearcolor_root )

            DIALOG_APPENDNODE_INTEGER( targetclearcolor_root, "r", props["targetclearcolor"].GetPropValue<unsigned char>( "r" ) )
            DIALOG_APPENDNODE_INTEGER( targetclearcolor_root, "g", props["targetclearcolor"].GetPropValue<unsigned char>( "g" ) )
            DIALOG_APPENDNODE_INTEGER( targetclearcolor_root, "b", props["targetclearcolor"].GetPropValue<unsigned char>( "b" ) )
        }

        bool viewportquad = props["viewportquad"].GetPropValue<bool>();
        DIALOG_APPENDROOT_BOOL( "viewport quad", viewportquad );

        if( viewportquad )
        {
            DIALOG_APPENDROOT_STRING( "viewport quad fx", props["viewportquad_fx"].GetPropValue<dsstring>() );

            std::vector<std::pair<long, Pass::TextureSourceName>> viewportquad_textures;
            viewportquad_textures = props["viewportquad_textures"].GetPropValue<std::vector<std::pair<long, Pass::TextureSourceName>>>();

            if( viewportquad_textures.size() > 0 )
            {
                DIALOG_APPENDROOT_NODE( "viewport quad textures", viewportquadtextures_root )

                DIALOG_BUILD_LABELS( viewportquad_textures.size(), "stage %d", labels )

                DIALOG_APPENDNODE_ITERATE_NODE_BEGIN( viewportquadtextures_root, i, labels, t_out )

                    Pass::TextureSourceName texture_source_name = viewportquad_textures[i].second;
                    long stage = viewportquad_textures[i].first;

                    if( Pass::PASS_NAME == texture_source_name.source )
                    {
                        DIALOG_APPENDNODE_STRING( t_out, "source type", "pass" )
                    }
                    else
                    {
                        DIALOG_APPENDNODE_STRING( t_out, "source type", "texture" )
                    }
                    DIALOG_APPENDNODE_STRING( t_out, "source name", texture_source_name.name )

                DIALOG_APPENDNODE_ITERATE_NODE_END
            }
        }

        std::map<dsstring, RenderingNode::ShadersParams> viewportquad_shaderparams = props["viewportquad_shaderparams"].GetPropValue<std::map<dsstring, RenderingNode::ShadersParams>>();

        if( viewportquad_shaderparams.size() )
        {
            //wxPGProperty* vpqshaderparams_prop = p_propertygrid->Append( new wxStringProperty( "viewportquad_shaderparams", wxPG_LABEL, "<composed>" ) );

            DIALOG_APPENDROOT_NODE( "viewport quad shader params", viewportquadshaderparams_root )
                           
            for( std::map<dsstring, RenderingNode::ShadersParams>::iterator it = viewportquad_shaderparams.begin(); it != viewportquad_shaderparams.end(); ++ it )
            {

                //wxPGProperty* vpqshaderparamname_prop = p_propertygrid->AppendIn( vpqshaderparams_prop, new wxStringProperty( it->first.c_str(), wxPG_LABEL, "<composed>" ) );

                DIALOG_APPENDNODE_NODE( viewportquadshaderparams_root, it->first, shaderparam )

                DIALOG_APPENDNODE_INTEGER( shaderparam, "shader index", it->second.shader_index )
                DIALOG_APPENDNODE_INTEGER( shaderparam, "register", it->second.param_register )

                DIALOG_APPENDNODE_NODE( shaderparam, "values", shaderparamvalues )

                DIALOG_APPENDNODE_FLOAT( shaderparamvalues, "x", it->second.param_values[0] )
                DIALOG_APPENDNODE_FLOAT( shaderparamvalues, "y", it->second.param_values[1] )
                DIALOG_APPENDNODE_FLOAT( shaderparamvalues, "z", it->second.param_values[2] )
                DIALOG_APPENDNODE_FLOAT( shaderparamvalues, "w", it->second.param_values[3] )

            }
            
        }

        DIALOG_SHOW
    }

}

void BasicSceneMainFrame::OnPassesListItemActivated( wxListEvent& p_event )
{
    long sel_index = p_event.GetIndex();

    Configurable* config = (Configurable*)m_passes_listCtrl->GetItemData( sel_index );

    build_passes_infos_dialog( config );
}

void BasicSceneMainFrame::OnPassesListItemSelected( wxListEvent& p_event )
{
    long sel_index = p_event.GetIndex();
    Configurable* config = (Configurable*)m_passes_listCtrl->GetItemData( sel_index );

    wxWidgetAdapter::GetInstance()->AdaptPassesShaderParamsList( static_cast<Pass*>( config ), m_shadersparams_listCtrl );
}

void BasicSceneMainFrame::OnShadersListItemActivated( wxListEvent& p_event )
{
    long sel_index = p_event.GetIndex();
    Pass* pass = (Pass*)m_shadersparams_listCtrl->GetItemData( sel_index );

    wxString shader_name = m_shadersparams_listCtrl->GetItemText( sel_index );

    wxCharBuffer buffer = shader_name.ToAscii();

    BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Shader parameters modification" );

    dialog->SetData( "ctrl", m_shadersparams_listCtrl );
    wxWidgetAdapter::GetInstance()->AdaptPassShaderValuesPropsModification( pass, buffer.data(), dialog );
    dialog->EnableApplyButton();
    dialog->Show();
}






void BasicSceneMainFrame::OnPopupClick(wxCommandEvent& p_evt)
{   
 	switch( p_evt.GetId() ) 
    {
 		case CONTEXTMENU_NEWSCENENODEGRAPH:
            {
                DIALOG_DECLARE( DIALOG_SCENEGRAPH_CREATION_TITLE )
                DIALOG_APPENDROOT_STRING( "name", "" )
                DIALOG_APPLY
                DIALOG_SHOW
            }
 			break;

        case CONTEXTMENU_NEWTRANSFO:
            {

                DIALOG_DECLARE( DIALOG_TRANSFORM_CREATION_TITLE )

                DIALOG_APPENDROOT_STRING( "scene name", "" )
                DIALOG_APPLY
                DIALOG_SHOW

            }
            break;

        case CONTEXTMENU_EDIT_TRANSFORMNODE:
            {
                void* id = m_last_clicked_treeitem.GetID();

                if( m_transformation_nodes.count( id ) > 0 )
                {

                    DIALOG_DECLARE( DIALOG_TRANSFORM_EDITION_TITLE )

                    DIALOG_APPLY
                    DIALOG_SPECIFIC0( "Add matrix" )
                    DIALOG_SPECIFIC1( "Clear all" )

                    std::vector<Matrix> mat_chain;
                    m_transformation_nodes[id].transformation->GetContent()->GetMatrixChain( mat_chain );


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
            break;

        case CONTEXTMENU_NEWSPACEBOX:
            {

                DIALOG_DECLARE( DIALOG_SPACEBOX_CREATION_TITLE )

                DIALOG_APPENDROOT_STRING( "scene name", "" )

                DIALOG_APPLY
                DIALOG_SPECIFIC0( "New pass slot" )
                DIALOG_SPECIFIC1( "New shaders param slot" )

                DIALOG_SHOW
            }
            break;
 	}
 }

void BasicSceneMainFrame::OnSceneNodeGraphsListRightClick( wxTreeEvent& p_event )
{	
 	wxMenu mnu;
 	
    wxTreeItemId item = p_event.GetItem();

    m_last_clicked_treeitem = item;

    if( item.GetID() == m_scenegraphs_root_item.GetID() )
    {
        build_popupmenu( DRAWSPACE_MASK, mnu );
    }
    else
    {
        
        if( m_scenenodegraphs.count( item.GetID() ) > 0 )
        {
            // clicked item belong to a registered scenenodegraph...
            build_popupmenu( SCENEGRAPH_MASK, mnu );
        }
        else if( m_transformation_nodes.count( item.GetID() ) > 0 )
        {
            build_popupmenu( TRANSFO_MASK, mnu );
        }
    }

 	mnu.Connect(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&BasicSceneMainFrame::OnPopupClick, NULL, this );
 	PopupMenu(&mnu);
}


void BasicSceneMainFrame::OnMouseKeyboardOutputCombobox( wxCommandEvent& p_event )
{
}










void BasicSceneMainFrame::OnConsoleInSendButtonClicked( wxCommandEvent& p_event )
{
    m_scripting->ExecChunk( m_consoleinput_textCtrl->GetValue().c_str() );
}

void BasicSceneMainFrame::OnConsoleInClearButtonClicked( wxCommandEvent& p_event )
{
    m_consoleinput_textCtrl->Clear();
}

void BasicSceneMainFrame::OnConsoleInLoadButtonClicked( wxCommandEvent& p_event )
{
}

wxArrayString BasicSceneMainFrame::get_passes_list( void )
{
    wxArrayString availables_passes_labels;
    std::map<dsstring, DrawSpace::Core::Configurable*> configs;

    ConfigsBase::GetInstance()->GetConfigsInstancesList( configs );
    for( std::map<dsstring, DrawSpace::Core::Configurable*>::iterator it = configs.begin(); it != configs.end(); ++it )
    {
        Pass* pass = dynamic_cast<Pass*>( it->second );
        if( pass )
        {
            dsstring pass_name;            
            availables_passes_labels.Add( it->first.c_str() );
        }
    }
    return availables_passes_labels;
}

wxArrayString BasicSceneMainFrame::get_finalpasses_list( void )
{
    wxArrayString availables_passes_labels;
    std::map<dsstring, DrawSpace::Core::Configurable*> configs;

    ConfigsBase::GetInstance()->GetConfigsInstancesList( configs );
    for( std::map<dsstring, DrawSpace::Core::Configurable*>::iterator it = configs.begin(); it != configs.end(); ++it )
    {
        FinalPass* pass = dynamic_cast<FinalPass*>( it->second );
        if( pass )
        {
            dsstring pass_name;            
            availables_passes_labels.Add( it->first.c_str() );
        }
    }
    return availables_passes_labels;
}

wxArrayString BasicSceneMainFrame::get_intermediatepasses_list( void )
{
    wxArrayString availables_passes_labels;
    std::map<dsstring, DrawSpace::Core::Configurable*> configs;

    ConfigsBase::GetInstance()->GetConfigsInstancesList( configs );
    for( std::map<dsstring, DrawSpace::Core::Configurable*>::iterator it = configs.begin(); it != configs.end(); ++it )
    {
        IntermediatePass* pass = dynamic_cast<IntermediatePass*>( it->second );
        if( pass )
        {
            dsstring pass_name;            
            availables_passes_labels.Add( it->first.c_str() );
        }
    }
    return availables_passes_labels;
}

wxArrayString BasicSceneMainFrame::get_fx_list( void )
{
    wxArrayString availables_fx_labels;
    std::map<dsstring, DrawSpace::Core::Configurable*> configs;

    ConfigsBase::GetInstance()->GetConfigsInstancesList( configs );
    for( std::map<dsstring, DrawSpace::Core::Configurable*>::iterator it = configs.begin(); it != configs.end(); ++it )
    {
        Fx* fx = dynamic_cast<Fx*>( it->second );
        if( fx )
        {
            dsstring pass_name;            
            availables_fx_labels.Add( it->first.c_str() );
        }
    }
    return availables_fx_labels;
}

wxArrayString BasicSceneMainFrame::get_textures_list( void )
{
    wxArrayString availables_textures_labels;

    std::map<dsstring, Asset*> assets_list;

    AssetsBase::GetInstance()->GetAssetsList( assets_list );    
    for( std::map<dsstring, Asset*>::iterator it = assets_list.begin(); it != assets_list.end(); ++it )
    {
        if( dynamic_cast<Texture*>( it->second ) )
        {
            availables_textures_labels.Add( it->first.c_str() );
        }
    }
    return availables_textures_labels;
}

wxArrayString BasicSceneMainFrame::get_shaders_list( void )
{
    wxArrayString availables_shaders_labels;

    std::map<dsstring, Asset*> assets_list;

    AssetsBase::GetInstance()->GetAssetsList( assets_list );    
    for( std::map<dsstring, Asset*>::iterator it = assets_list.begin(); it != assets_list.end(); ++it )
    {
        if( dynamic_cast<Shader*>( it->second ) )
        {
            availables_shaders_labels.Add( it->first.c_str() );
        }
    }

    return availables_shaders_labels;
}

wxArrayString BasicSceneMainFrame::get_meshes_list( void )
{
    wxArrayString availables_meshes_labels;

    std::map<dsstring, Asset*> assets_list;

    AssetsBase::GetInstance()->GetAssetsList( assets_list );    
    for( std::map<dsstring, Asset*>::iterator it = assets_list.begin(); it != assets_list.end(); ++it )
    {
        if( dynamic_cast<Meshe*>( it->second ) )
        {
            availables_meshes_labels.Add( it->first.c_str() );
        }
    }

    return availables_meshes_labels;
}

wxArrayString BasicSceneMainFrame::get_fonts_list( void )
{
    wxArrayString availables_fonts_labels;

    std::map<dsstring, Asset*> assets_list;

    AssetsBase::GetInstance()->GetAssetsList( assets_list );    
    for( std::map<dsstring, Asset*>::iterator it = assets_list.begin(); it != assets_list.end(); ++it )
    {
        if( dynamic_cast<Font*>( it->second ) )
        {
            availables_fonts_labels.Add( it->first.c_str() );
        }
    }

    return availables_fonts_labels;
}

wxArrayString BasicSceneMainFrame::insert_void_choice( const wxArrayString& p_array )
{
    wxArrayString completed_array = p_array;
    completed_array.Insert( "...", 0 );

    return completed_array;
}

void BasicSceneMainFrame::on_applybutton_clicked( BasicSceneObjectPropertiesDialog* p_dialog )
{
    DIALOG_GETGRID
    DIALOG_PROPERTIES_VARS


    if( DIALOG_SCENEGRAPH_CREATION_TITLE == DIALOG_TITLE )
    {
        DIALOG_GET_STRING_PROPERTY( "name", scenegraph_name )

        DIALOG_WXSTRING_TO_DSSTRING( scenegraph_name, scenegraph_name2 )

        if( "" == scenegraph_name2 )
        {
            wxMessageBox( "'name' attribute cannot be void", "DrawFront error", wxICON_ERROR );
        }
        else
        {

            BasicSceneMainFrame::SceneNodeGraphEntry entry;

            entry.name = scenegraph_name2;
            entry.scenenodegraph = new SceneNodeGraph();
            entry.treeitemid = m_scenegraphs_treeCtrl->AppendItem( m_scenegraphs_root_item, scenegraph_name, SCENEGRAPH_ICON_INDEX );

            m_scenenodegraphs[entry.treeitemid.GetID()] = entry;

            m_scenegraphs_treeCtrl->ExpandAllChildren( m_scenegraphs_root_item );

            DIALOG_CLOSE
        }
    }

    else if( DIALOG_TRANSFORM_CREATION_TITLE == DIALOG_TITLE )
    {
        DIALOG_GET_STRING_PROPERTY( "scene name", alias2 )

        DIALOG_WXSTRING_TO_DSSTRING( alias2, alias )

        if( "" == alias )
        {
            wxMessageBox( "'scene name' attribute cannot be void", "DrawFront error", wxICON_ERROR );
            return;
        }

        /////////////////////////////////////////////////////////////////////////////////

        // create the transformation node

        SceneNode<Transformation>* transfo_node;
        transfo_node = new SceneNode<Transformation>( alias );
        transfo_node->SetContent( new Transformation );

        /////////////////////////////////////////////////////////////////////////////////

        // now we must found the scenenodegraph we belong to make the RegisterNode() call

        wxTreeItemId current = m_last_clicked_treeitem;

        void* id;

        while( 1 )
        {
            id = current.GetID();

            if( m_scenenodegraphs.count( id ) > 0 )
            {
                break;
            }

            current = m_scenegraphs_treeCtrl->GetItemParent( current );
        }

        BasicSceneMainFrame::SceneNodeGraphEntry entry;

        entry = m_scenenodegraphs[id];
        entry.scenenodegraph->RegisterNode( transfo_node );

        ///////////////////////////////////////////////////////////////////////////////////////////////

        // link to the scenegraph hierarchy

        current = m_last_clicked_treeitem;
        id = current.GetID();

        if( m_scenenodegraphs.count( id ) > 0 )
        {
            // parent is a scenegraph : use SceneNodeGraph::Add() method
            entry.scenenodegraph->AddNode( transfo_node );
        }
        else
        {
            BaseSceneNode* parent_node = m_tree_nodes[id];
            transfo_node->LinkTo( parent_node );
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////

        // GUI : add item in the tree

        wxTreeItemId treeitemid = m_scenegraphs_treeCtrl->AppendItem( m_last_clicked_treeitem, alias2, TRANSFO_ICON_INDEX );
        m_scenegraphs_treeCtrl->ExpandAllChildren( m_last_clicked_treeitem );


        /////////////////////////////////////////////////////////////////////////////////

        // record the new transformation node and associated metadata

        BasicSceneMainFrame::TransformationNodeEntry t_entry;

        t_entry.name = alias;
        t_entry.transformation = transfo_node;
        t_entry.treeitemid = treeitemid;


        m_transformation_nodes[t_entry.treeitemid.GetID()] = t_entry;

        m_tree_nodes[t_entry.treeitemid.GetID()] = transfo_node;



        DIALOG_CLOSE
    }

    else if( DIALOG_TRANSFORM_EDITION_TITLE == DIALOG_TITLE )
    {               
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


        DIALOG_EXPLORE_NODES_END

        if( ok )
        {
            Transformation* tdet = m_transformation_nodes[m_last_clicked_treeitem.GetID()].transformation->GetContent();
            //(*tdet) = new_chain;

            tdet->ClearAll();
            for( size_t i = 0; i < new_chain.size(); i++ )
            {
                tdet->PushMatrix( new_chain[i] );
            }

            p_dialog->Close();
        }
        else
        {
            wxMessageBox( "Edited matrix chain has a undetermined matrix", "DrawFront error", wxICON_ERROR );
        }


    }

    else if( DIALOG_SPACEBOX_CREATION_TITLE == DIALOG_TITLE )
    {
        DIALOG_GET_STRING_PROPERTY( "scene name", alias2 )

        DIALOG_WXSTRING_TO_DSSTRING( alias2, alias )

        if( "" == alias )
        {
            wxMessageBox( "'scene name' attribute cannot be void", "DrawFront error", wxICON_ERROR );
            return;
        }

        DIALOG_EXPLORE_NODES_BEGIN( "", "pass %d", i, pass_slot )

            DIALOG_GET_ENUM_PROPERTY( DIALOG_INCREMENT_STRING( pass_slot, "pass" ), pass_name )
            DIALOG_GET_ENUM_PROPERTY( DIALOG_INCREMENT_STRING( pass_slot, "fx" ), fx_name )

            _asm nop

        DIALOG_EXPLORE_NODES_END
    }


    /*
    DIALOG_GETGRID
    DIALOG_PROPERTIES_VARS

    if( "Spacebox node creation" == DIALOG_TITLE )
    {
        DIALOG_GET_STRING_PROPERTY( "scene name", scene_name )

        DIALOG_GET_INT_PROPERTY( "the beast", var_int )

        DIALOG_GET_FLOAT_PROPERTY( "pi", pi )

        DIALOG_GET_ENUM_PROPERTY( "list", sel_text )

        DIALOG_WXSTRING_TO_DSSTRING( scene_name, scene_name2 )
        DIALOG_WXSTRING_TO_DSSTRING( sel_text, sel_text2 )

        

        DIALOG_EXPLORE_NODES_BEGIN( "", "PASS_%d", i, pass_slot )
            

            DIALOG_GET_BOOL_PROPERTY( DIALOG_INCREMENT_STRING( pass_slot, "enable" ), enable_it )

            
            DIALOG_EXPLORE_NODES_BEGIN( pass_slot, "texture_stage_%d", j, texture_stage )

                
                DIALOG_GET_INT_PROPERTY( DIALOG_INCREMENT_STRING( texture_stage, "order" ), order )

                DIALOG_GET_ENUM_PROPERTY( DIALOG_INCREMENT_STRING( texture_stage, "source" ), sel_texture )

                _asm nop
                

            DIALOG_EXPLORE_NODES_END

            
        DIALOG_EXPLORE_NODES_END
    }
    */
}

void BasicSceneMainFrame::on_specificbutton0_clicked( BasicSceneObjectPropertiesDialog* p_dialog )
{
    DIALOG_GETGRID

    if( DIALOG_SPACEBOX_CREATION_TITLE == DIALOG_TITLE )
    {
        DIALOG_SPECIFIC0_LABEL( "pass %d", pass_label )

        DIALOG_APPENDROOT_NODE( pass_label, pass_root )

        DIALOG_APPENDNODE_ENUM( pass_root, "pass", get_intermediatepasses_list() )
        DIALOG_APPENDNODE_ENUM( pass_root, "fx", get_fx_list() )
        DIALOG_APPENDNODE_INTEGER( pass_root, "rendering order", 200 )

        DIALOG_APPENDNODE_NODE( pass_root, "textures", textures_label )

        DIALOG_APPENDNODE_NODE( textures_label, "front", front_label )
        DIALOG_BUILD_LABELS( RenderingNode::NbMaxTextures, "stage %d", front_textures_stages_labels )
        DIALOG_APPENDNODE_ITERATE( front_label, insert_void_choice( get_textures_list() ), DIALOG_APPENDNODE_ENUM, front_textures_stages_labels )

        DIALOG_APPENDNODE_NODE( textures_label, "rear", rear_label )
        DIALOG_BUILD_LABELS( RenderingNode::NbMaxTextures, "stage %d", rear_textures_stages_labels )
        DIALOG_APPENDNODE_ITERATE( rear_label, insert_void_choice( get_textures_list() ), DIALOG_APPENDNODE_ENUM, rear_textures_stages_labels )

        DIALOG_APPENDNODE_NODE( textures_label, "left", left_label )
        DIALOG_BUILD_LABELS( RenderingNode::NbMaxTextures, "stage %d", left_textures_stages_labels )
        DIALOG_APPENDNODE_ITERATE( left_label, insert_void_choice( get_textures_list() ), DIALOG_APPENDNODE_ENUM, left_textures_stages_labels )

        DIALOG_APPENDNODE_NODE( textures_label, "right", right_label )
        DIALOG_BUILD_LABELS( RenderingNode::NbMaxTextures, "stage %d", right_textures_stages_labels )
        DIALOG_APPENDNODE_ITERATE( right_label, insert_void_choice( get_textures_list() ), DIALOG_APPENDNODE_ENUM, right_textures_stages_labels )

        DIALOG_APPENDNODE_NODE( textures_label, "bottom", bottom_label )
        DIALOG_BUILD_LABELS( RenderingNode::NbMaxTextures, "stage %d", bottom_textures_stages_labels )
        DIALOG_APPENDNODE_ITERATE( bottom_label, insert_void_choice( get_textures_list() ), DIALOG_APPENDNODE_ENUM, bottom_textures_stages_labels )

        DIALOG_APPENDNODE_NODE( textures_label, "top", top_label )
        DIALOG_BUILD_LABELS( RenderingNode::NbMaxTextures, "stage %d", top_textures_stages_labels )
        DIALOG_APPENDNODE_ITERATE( top_label, insert_void_choice( get_textures_list() ), DIALOG_APPENDNODE_ENUM, top_textures_stages_labels )


        DIALOG_FINALIZE
    }

    else if( DIALOG_TRANSFORM_EDITION_TITLE == DIALOG_TITLE )
    {
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



    /*
    DIALOG_GETGRID

    if( "Spacebox node creation" == DIALOG_TITLE )
    {
        DIALOG_SPECIFIC0_LABEL( "PASS_%d", pass_label )

        DIALOG_APPENDROOT_NODE( pass_label, pass_root )        
        DIALOG_APPENDNODE_BOOL( pass_root, "enable", true );

        DIALOG_BUILD_LABELS( 2, "texture_stage_%d", textures_stages_labels )

        DIALOG_APPENDNODE_ITERATE_NODE_BEGIN( pass_root, i, textures_stages_labels, texture_stage )

            DIALOG_APPENDNODE_ENUM( texture_stage, "source", get_textures_list() )
            DIALOG_APPENDNODE_INTEGER( texture_stage, "order", 200 )

        DIALOG_APPENDNODE_ITERATE_NODE_END
                       
        DIALOG_FINALIZE
    }
    */
}

void BasicSceneMainFrame::on_specificbutton1_clicked( BasicSceneObjectPropertiesDialog* p_dialog )
{
    DIALOG_GETGRID

    if( DIALOG_TRANSFORM_EDITION_TITLE == DIALOG_TITLE )
    {
        DIALOG_CLEARGRID
        DIALOG_SETSPECIFIC0COUNTER( 0 )
    }

    else if( DIALOG_SPACEBOX_CREATION_TITLE == DIALOG_TITLE )
    {
        DIALOG_SPECIFIC1_LABEL( "shader params %d", param_label )

        DIALOG_APPENDROOT_NODE( param_label, param_root )
        DIALOG_APPENDNODE_ENUM( param_root, "pass", get_intermediatepasses_list() )
        DIALOG_APPENDNODE_INTEGER( param_root, "shader index", 0 )
        DIALOG_APPENDNODE_STRING( param_root, "param id", "" )
        DIALOG_APPENDNODE_INTEGER( param_root, "register", 0 )
        DIALOG_APPENDNODE_NODE( param_root, "values", param_values )
        DIALOG_APPENDNODE_FLOAT( param_values, "x", 0.0 )
        DIALOG_APPENDNODE_FLOAT( param_values, "y", 0.0 )
        DIALOG_APPENDNODE_FLOAT( param_values, "z", 0.0 )
        DIALOG_APPENDNODE_FLOAT( param_values, "w", 0.0 )
        DIALOG_FINALIZE

    }
}
