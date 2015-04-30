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
#include "buildobjects.h"
#include "luascripting.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

//#define RENDER_IDLE

RenderTimer::RenderTimer( BasicSceneMainFrame* p_pane ) : pane( p_pane )
{
}
void RenderTimer::Notify( void )
{
    pane->render();
}
void RenderTimer::Start( void )
{
    wxTimer::Start( 5 );
}

void RenderTimer::Stop( void )
{
    wxTimer::Stop();
}

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
m_console_font( 8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false ),
m_keyup_code( NULL ),
m_keydown_code( NULL ),
m_mousemove_descr( NULL ),
m_delta_mouse_init( true )
{

   
    m_consoleinput_textCtrl->SetFont( m_console_font );
    m_consoleoutput_textCtrl->SetFont( m_console_font );

    m_scripting = new LuaScripting();



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
    PopupMenuEntry pme_editshaders = { CONTEXTMENU_EDIT_SHADERSPARAMS, "Edit shaders params..." };
    PopupMenuEntry pme_showprops = { CONTEXTMENU_SHOW_PROPS, "Show properties..." };
    PopupMenuEntry pme_editnodescript = { CONTEXTMENU_EDIT_NODESCRIPT, "Edit script..." };

    PopupMenuEntry pme_editmousemovescript = { CONTEXTMENU_EDIT_MOUSEMOVESCRIPT, "Edit mouse movement script..." };
    PopupMenuEntry pme_editkeydownscript = { CONTEXTMENU_EDIT_KEYDOWNSCRIPT, "Edit key down script..." };
    PopupMenuEntry pme_editkeyupscript = { CONTEXTMENU_EDIT_KEYUPSCRIPT, "Edit key up script..." };

    PopupMenuEntry pme_selectcamera = { CONTEXTMENU_SELECT_CAMERA, "Select camera..." };
    PopupMenuEntry pme_editcamera = { CONTEXTMENU_EDIT_CAMERA, "Edit camera..." };

    PopupMenuEntry pme_editmvt = { CONTEXTMENU_EDIT_MVT, "Edit movement..." };

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

    m_scenegraphs_masks[KEYBOARD_MASK].push_back( pme_editkeydownscript );
    m_scenegraphs_masks[KEYBOARD_MASK].push_back( pme_editkeyupscript );
    m_scenegraphs_masks[MOUSE_MASK].push_back( pme_editmousemovescript );

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

    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_newcamera );

    ///////////////////////////////////////////////////////////////////

    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newspacebox );
    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newchunk );
    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_separator );
    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newcollider );
    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_neworbit );
    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newplanet );

    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_separator );

    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newtransfo );

    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_separator );

    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newlinearmvt );
    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newcircularmvt );
    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newfreemvt );
    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newfpsmvt );
    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newspectatormvt );    
    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newlonglatmvt );
    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newcamera );

    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_separator );

    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_edittransformnode );

    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_editnodescript );

    ///////////////////////////////////////////////////////////////////

    m_scenegraphs_masks[SPACEBOX_MASK].push_back( pme_showprops );
    m_scenegraphs_masks[SPACEBOX_MASK].push_back( pme_separator );
    m_scenegraphs_masks[SPACEBOX_MASK].push_back( pme_editshaders );
    m_scenegraphs_masks[SPACEBOX_MASK].push_back( pme_editnodescript );

    ///////////////////////////////////////////////////////////////////

    m_scenegraphs_masks[CAMERA_MASK].push_back( pme_showprops );
    m_scenegraphs_masks[CAMERA_MASK].push_back( pme_selectcamera );
    m_scenegraphs_masks[CAMERA_MASK].push_back( pme_separator );
    m_scenegraphs_masks[CAMERA_MASK].push_back( pme_editcamera );    
    m_scenegraphs_masks[CAMERA_MASK].push_back( pme_editnodescript );


    ///////////////////////////////////////////////////////////////////

    m_scenegraphs_masks[FPSMOVEMENT_MASK].push_back( pme_newchunk );
    m_scenegraphs_masks[FPSMOVEMENT_MASK].push_back( pme_separator );
    m_scenegraphs_masks[FPSMOVEMENT_MASK].push_back( pme_newcollider );
    m_scenegraphs_masks[FPSMOVEMENT_MASK].push_back( pme_neworbit );
    m_scenegraphs_masks[FPSMOVEMENT_MASK].push_back( pme_newplanet );
    m_scenegraphs_masks[FPSMOVEMENT_MASK].push_back( pme_separator );
    m_scenegraphs_masks[FPSMOVEMENT_MASK].push_back( pme_newtransfo );
    m_scenegraphs_masks[FPSMOVEMENT_MASK].push_back( pme_separator );
    m_scenegraphs_masks[FPSMOVEMENT_MASK].push_back( pme_newlinearmvt );
    m_scenegraphs_masks[FPSMOVEMENT_MASK].push_back( pme_newcircularmvt );
    m_scenegraphs_masks[FPSMOVEMENT_MASK].push_back( pme_newfreemvt );
    m_scenegraphs_masks[FPSMOVEMENT_MASK].push_back( pme_newfpsmvt );
    m_scenegraphs_masks[FPSMOVEMENT_MASK].push_back( pme_newspectatormvt );
    m_scenegraphs_masks[FPSMOVEMENT_MASK].push_back( pme_newheadmvt );
    m_scenegraphs_masks[FPSMOVEMENT_MASK].push_back( pme_newlonglatmvt );
    m_scenegraphs_masks[FPSMOVEMENT_MASK].push_back( pme_newcamera );
    m_scenegraphs_masks[FPSMOVEMENT_MASK].push_back( pme_separator );
    m_scenegraphs_masks[FPSMOVEMENT_MASK].push_back( pme_showprops );
    m_scenegraphs_masks[FPSMOVEMENT_MASK].push_back( pme_separator );
    m_scenegraphs_masks[FPSMOVEMENT_MASK].push_back( pme_editnodescript );


    ///////////////////////////////////////////////////////////////////

    m_scenegraphs_masks[CHUNK_MASK].push_back( pme_newchunk );
    m_scenegraphs_masks[CHUNK_MASK].push_back( pme_separator );
    m_scenegraphs_masks[CHUNK_MASK].push_back( pme_newcollider );
    m_scenegraphs_masks[CHUNK_MASK].push_back( pme_separator );
    m_scenegraphs_masks[CHUNK_MASK].push_back( pme_newtransfo );
    m_scenegraphs_masks[CHUNK_MASK].push_back( pme_separator );
    m_scenegraphs_masks[CHUNK_MASK].push_back( pme_newlinearmvt );
    m_scenegraphs_masks[CHUNK_MASK].push_back( pme_newcircularmvt );
    m_scenegraphs_masks[CHUNK_MASK].push_back( pme_newfreemvt );
    m_scenegraphs_masks[CHUNK_MASK].push_back( pme_newfpsmvt );
    m_scenegraphs_masks[CHUNK_MASK].push_back( pme_newspectatormvt );
    m_scenegraphs_masks[CHUNK_MASK].push_back( pme_newheadmvt );
    m_scenegraphs_masks[CHUNK_MASK].push_back( pme_newlonglatmvt );
    m_scenegraphs_masks[CHUNK_MASK].push_back( pme_newcamera );
    m_scenegraphs_masks[CHUNK_MASK].push_back( pme_separator );
    m_scenegraphs_masks[CHUNK_MASK].push_back( pme_showprops );
    m_scenegraphs_masks[CHUNK_MASK].push_back( pme_separator );
    m_scenegraphs_masks[CHUNK_MASK].push_back( pme_editshaders );
    m_scenegraphs_masks[CHUNK_MASK].push_back( pme_editnodescript );

    ///////////////////////////////////////////////////////////////////

    m_scenegraphs_masks[LINMOVEMENT_MASK].push_back( pme_newchunk );
    m_scenegraphs_masks[LINMOVEMENT_MASK].push_back( pme_separator );
    m_scenegraphs_masks[LINMOVEMENT_MASK].push_back( pme_newcollider );
    m_scenegraphs_masks[LINMOVEMENT_MASK].push_back( pme_neworbit );
    m_scenegraphs_masks[LINMOVEMENT_MASK].push_back( pme_newplanet );
    m_scenegraphs_masks[LINMOVEMENT_MASK].push_back( pme_separator );
    m_scenegraphs_masks[LINMOVEMENT_MASK].push_back( pme_newtransfo );
    m_scenegraphs_masks[LINMOVEMENT_MASK].push_back( pme_separator );
    m_scenegraphs_masks[LINMOVEMENT_MASK].push_back( pme_newlinearmvt );
    m_scenegraphs_masks[LINMOVEMENT_MASK].push_back( pme_newcircularmvt );
    m_scenegraphs_masks[LINMOVEMENT_MASK].push_back( pme_newfreemvt );
    m_scenegraphs_masks[LINMOVEMENT_MASK].push_back( pme_newfpsmvt );
    m_scenegraphs_masks[LINMOVEMENT_MASK].push_back( pme_newspectatormvt );
    m_scenegraphs_masks[LINMOVEMENT_MASK].push_back( pme_newheadmvt );
    m_scenegraphs_masks[LINMOVEMENT_MASK].push_back( pme_newlonglatmvt );
    m_scenegraphs_masks[LINMOVEMENT_MASK].push_back( pme_newcamera );
    m_scenegraphs_masks[LINMOVEMENT_MASK].push_back( pme_separator );
    m_scenegraphs_masks[LINMOVEMENT_MASK].push_back( pme_showprops );
    m_scenegraphs_masks[LINMOVEMENT_MASK].push_back( pme_separator );
    m_scenegraphs_masks[LINMOVEMENT_MASK].push_back( pme_editmvt );
    m_scenegraphs_masks[LINMOVEMENT_MASK].push_back( pme_editnodescript );
    
    ///////////////////////////////////////////////////////////////////

    m_scenegraphs_masks[FREEMOVEMENT_MASK].push_back( pme_newchunk );
    m_scenegraphs_masks[FREEMOVEMENT_MASK].push_back( pme_separator );
    m_scenegraphs_masks[FREEMOVEMENT_MASK].push_back( pme_newcollider );
    m_scenegraphs_masks[FREEMOVEMENT_MASK].push_back( pme_neworbit );
    m_scenegraphs_masks[FREEMOVEMENT_MASK].push_back( pme_newplanet );
    m_scenegraphs_masks[FREEMOVEMENT_MASK].push_back( pme_separator );
    m_scenegraphs_masks[FREEMOVEMENT_MASK].push_back( pme_newtransfo );
    m_scenegraphs_masks[FREEMOVEMENT_MASK].push_back( pme_separator );
    m_scenegraphs_masks[FREEMOVEMENT_MASK].push_back( pme_newlinearmvt );
    m_scenegraphs_masks[FREEMOVEMENT_MASK].push_back( pme_newcircularmvt );
    m_scenegraphs_masks[FREEMOVEMENT_MASK].push_back( pme_newfreemvt );
    m_scenegraphs_masks[FREEMOVEMENT_MASK].push_back( pme_newfpsmvt );
    m_scenegraphs_masks[FREEMOVEMENT_MASK].push_back( pme_newspectatormvt );
    m_scenegraphs_masks[FREEMOVEMENT_MASK].push_back( pme_newheadmvt );
    m_scenegraphs_masks[FREEMOVEMENT_MASK].push_back( pme_newlonglatmvt );
    m_scenegraphs_masks[FREEMOVEMENT_MASK].push_back( pme_newcamera );
    m_scenegraphs_masks[FREEMOVEMENT_MASK].push_back( pme_separator );
    m_scenegraphs_masks[FREEMOVEMENT_MASK].push_back( pme_showprops );
    m_scenegraphs_masks[FREEMOVEMENT_MASK].push_back( pme_separator );
    m_scenegraphs_masks[FREEMOVEMENT_MASK].push_back( pme_editmvt );
    m_scenegraphs_masks[FREEMOVEMENT_MASK].push_back( pme_editnodescript );



    m_applybutton_clicked_cb = new DialogButtonCallback( this, &BasicSceneMainFrame::on_applybutton_clicked );
    m_specificbutton0_clicked_cb = new DialogButtonCallback( this, &BasicSceneMainFrame::on_specificbutton0_clicked );
    m_specificbutton1_clicked_cb = new DialogButtonCallback( this, &BasicSceneMainFrame::on_specificbutton1_clicked );

    m_nodeupdatebegin_cb = new NodeUpdateBeginCallBack( this, &BasicSceneMainFrame::on_nodeupdatebegin );

#ifndef RENDER_IDLE
    RenderTimer* timer = new RenderTimer(this);
    timer->Start();
    m_rendertimer = timer;
#endif
}

void BasicSceneMainFrame::on_scripting_error( const dsstring& p_error )
{
    PrintOutputConsole( p_error );
    //wxMessageBox( "Scripting syntax error", "Script error", wxICON_ERROR );
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
    else if( "DrawSpace:DrawSpace" == script_call_id )
    {
        TimeManager** timer = p_propertypool.GetPropValue<TimeManager**>( "timer_ref" );
        *timer = &m_timer;
    }
    else if( "DrawSpace:DisplayFramerate" == script_call_id )
    {
        bool state = p_propertypool.GetPropValue<bool>( "state" );
        m_display_framerate = state;
    }
    else if( "DrawSpace:DisplayCurrentCamera" == script_call_id )
    {
        bool state = p_propertypool.GetPropValue<bool>( "state" );
        dsstring scenegraphname = p_propertypool.GetPropValue<dsstring>( "scenegraphname" );

        bool found = false;
        for( std::map<void*, SceneNodeGraphEntry>::iterator it = m_scenenodegraphs.begin(); it != m_scenenodegraphs.end(); ++it )
        {
            if( it->second.name == scenegraphname )
            {
                m_display_currentcamera = state;
                m_scenegraph_currentcameradisplay = it->second;
                found = true;
                break;
            }
        }
        if( !found )
        {
            wxMessageBox( "DrawSpace:DisplayCurrentCamera, unknown scenegraph name : " + scenegraphname, "Script error", wxICON_ERROR );
        }
    }
    else if( "DrawSpace:CreateSceneNodeGraph" == script_call_id )
    {
        dsstring alias = p_propertypool.GetPropValue<dsstring>( "name" );

        SceneNodeGraphEntry entry;

        entry.name = alias;
        entry.scenenodegraph = new SceneNodeGraph();
        entry.treeitemid = m_scenegraphs_treeCtrl->AppendItem( m_scenegraphs_root_item, alias.c_str(), SCENEGRAPH_ICON_INDEX );
        entry.current_camera_set = false;
        m_scenenodegraphs[entry.treeitemid.GetID()] = entry;

        m_scenegraphs_treeCtrl->ExpandAllChildren( m_scenegraphs_root_item );
    }
    else if( "DrawSpace:AngleSpeedInc" == script_call_id )
    {
        dsreal speed = p_propertypool.GetPropValue<dsreal>( "speed" );
        dsreal* realvar = p_propertypool.GetPropValue<dsreal*>( "realvar" );

        m_timer.AngleSpeedInc( realvar, speed );

    }
    else if( "DrawSpace:AngleSpeedDec" == script_call_id )
    {
        dsreal speed = p_propertypool.GetPropValue<dsreal>( "speed" );
        dsreal* realvar = p_propertypool.GetPropValue<dsreal*>( "realvar" );
        m_timer.AngleSpeedDec( realvar, speed );

    }
    else if( "DrawSpace:TranslationSpeedInc" == script_call_id )
    {
        dsreal speed = p_propertypool.GetPropValue<dsreal>( "speed" );
        dsreal* realvar = p_propertypool.GetPropValue<dsreal*>( "realvar" );
        m_timer.TranslationSpeedInc( realvar, speed );
    }
    else if( "DrawSpace:TranslationSpeedDec" == script_call_id )
    {
        dsreal speed = p_propertypool.GetPropValue<dsreal>( "speed" );
        dsreal* realvar = p_propertypool.GetPropValue<dsreal*>( "realvar" );
        m_timer.TranslationSpeedDec( realvar, speed );
    }

    else if( "TransformationNode:TransformationNode" == script_call_id )
    {
        dsstring scene_name = p_propertypool.GetPropValue<dsstring>( "scene_name" );
        SceneNode<Transformation>** node_ptr = p_propertypool.GetPropValue<SceneNode<Transformation>**>( "existing_node" );

        for( std::map<void*, SceneNodeEntry<DrawSpace::Core::Transformation>>::iterator it = m_transformation_nodes.begin(); it != m_transformation_nodes.end(); ++it )
        {
            if( it->second.name == scene_name )
            {
                // node exists
                *node_ptr = it->second.scene_node;
                break;
            }
        }
    }
    else if( "CameraPointNode:CameraPointNode" == script_call_id )
    {
        dsstring scene_name = p_propertypool.GetPropValue<dsstring>( "scene_name" );
        SceneNode<CameraPoint>** node_ptr = p_propertypool.GetPropValue<SceneNode<CameraPoint>**>( "existing_node" );

        for( std::map<void*, SceneNodeEntry<DrawSpace::Dynamics::CameraPoint>>::iterator it = m_camera_nodes.begin(); it != m_camera_nodes.end(); ++it )
        {
            if( it->second.name == scene_name )
            {
                // node exists
                *node_ptr = it->second.scene_node;
                break;
            }
        }
    }
    else if( "TransformationNode:LinkTo" == script_call_id )
    {
        dsstring scene_name = p_propertypool.GetPropValue<dsstring>( "scene_name" );
        dsstring scenegraph_name = p_propertypool.GetPropValue<dsstring>( "scenegraph_name" );
        dsstring parent_name = p_propertypool.GetPropValue<dsstring>( "parent_name" );
        BaseSceneNode* node = p_propertypool.GetPropValue<BaseSceneNode*>( "node" );
        int nbmat = p_propertypool.GetPropValue<int>( "nbmat" );
       
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
        Transformation* tf = new Transformation( nbmat );
        transfo_node->SetContent( tf );

        transfo_node->RegisterUpdateBeginEvtHandler( m_nodeupdatebegin_cb );
                

        scenenodegraph_entry.scenenodegraph->RegisterNode( node );

        if( parent )
        {            
            transfo_node->LinkTo( parent );
            parent_tree_item = searchTreeItemIdInNodes( parent_id );
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

        BasicSceneMainFrame::SceneNodeEntry<Transformation> t_entry;

        t_entry.name = scene_name;
        t_entry.scene_node = transfo_node;
        t_entry.treeitemid = treeitemid;


        m_transformation_nodes[t_entry.treeitemid.GetID()] = t_entry;
        m_tree_nodes[t_entry.treeitemid.GetID()] = transfo_node;
        m_inv_tree_nodes[transfo_node] = t_entry.treeitemid.GetID();
    }



    else if( "SpaceboxNode:LinkTo" == script_call_id )
    {
        dsstring scene_name = p_propertypool.GetPropValue<dsstring>( "scene_name" );
        dsstring scenegraph_name = p_propertypool.GetPropValue<dsstring>( "scenegraph_name" );
        dsstring parent_name = p_propertypool.GetPropValue<dsstring>( "parent_name" );
        BaseSceneNode* node = p_propertypool.GetPropValue<BaseSceneNode*>( "node" );
        DrawSpace::Utils::SpaceboxDescriptor sb_descr = p_propertypool.GetPropValue<DrawSpace::Utils::SpaceboxDescriptor>( "descriptor" );

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
            wxMessageBox( "Spacebox node, unknown scenegraph name : " + scenegraph_name, "Script error", wxICON_ERROR );
            return;           
        }

        else if( !parent_found )
        {
            wxMessageBox( "Spacebox node, unknown parent name : " + parent_name, "Script error", wxICON_ERROR );
            return;
        }

        SceneNode<Spacebox>* sb_node = static_cast<SceneNode<Spacebox>*>( node );

        sb_node->RegisterUpdateBeginEvtHandler( m_nodeupdatebegin_cb );
        
        //sb_node->SetContent( new Spacebox );
        dsstring sb_error;
        Spacebox* sb = BuildSpaceBox( sb_descr, sb_error );
        if( NULL == sb )
        {
            wxMessageBox( "Spacebox node creation error : " + sb_error, "Script error", wxICON_ERROR );
            return;
        }
        else
        {
            sb_node->SetContent( sb );

            scenenodegraph_entry.scenenodegraph->RegisterNode( node );

            if( parent )
            {            
                sb_node->LinkTo( parent );
                parent_tree_item = searchTreeItemIdInNodes( parent_id );
            }
            else
            {
                scenenodegraph_entry.scenenodegraph->AddNode( node );
                parent_tree_item = scenenodegraph_entry.treeitemid;
            }

        
            // GUI : add item in the tree
            wxTreeItemId treeitemid = m_scenegraphs_treeCtrl->AppendItem( parent_tree_item, scene_name.c_str(), SPACEBOX_ICON_INDEX );
            m_scenegraphs_treeCtrl->ExpandAllChildren( parent_tree_item );
        
            // record the new spacebox node and associated metadata

            BasicSceneMainFrame::SceneNodeEntry<Spacebox> t_entry;

            t_entry.name = scene_name;
            t_entry.scene_node = sb_node;
            t_entry.treeitemid = treeitemid;

            m_spacebox_nodes[t_entry.treeitemid.GetID()] = t_entry;
            m_tree_nodes[t_entry.treeitemid.GetID()] = sb_node;
            m_inv_tree_nodes[sb_node] = t_entry.treeitemid.GetID();

            // update passes output queues
            for( std::map<dsstring, SpaceboxPassDescriptor>::iterator it = sb_descr.passes_slots.begin(); it != sb_descr.passes_slots.end(); ++it )
            {
                Pass* current_pass = dynamic_cast<Pass*>( ConfigsBase::GetInstance()->GetConfigurableInstance( it->first ) );
                current_pass->GetRenderingQueue()->UpdateOutputQueue();
            }

            // store spacebox description

            m_spacebox_descriptors[t_entry.treeitemid.GetID()] = sb_descr;
        }
    }
    else if( "CameraPointNode:LinkTo" == script_call_id )
    {
        dsstring scene_name = p_propertypool.GetPropValue<dsstring>( "scene_name" );
        dsstring scenegraph_name = p_propertypool.GetPropValue<dsstring>( "scenegraph_name" );
        dsstring parent_name = p_propertypool.GetPropValue<dsstring>( "parent_name" );
        BaseSceneNode* node = p_propertypool.GetPropValue<BaseSceneNode*>( "node" );

        wxTreeItemId parent_tree_item;
        void* parent_id = NULL;


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
            wxMessageBox( "CameraPoint node, unknown scenegraph name : " + scenegraph_name, "Script error", wxICON_ERROR );
            return;           
        }

        else if( !parent_found )
        {
            wxMessageBox( "CameraPoint node, unknown parent name : " + parent_name, "Script error", wxICON_ERROR );
            return;
        }

        SceneNode<CameraPoint>* cam_node = static_cast<SceneNode<CameraPoint>*>( node );
        cam_node->RegisterUpdateBeginEvtHandler( m_nodeupdatebegin_cb );
        
        cam_node->SetContent( new CameraPoint() );

        scenenodegraph_entry.scenenodegraph->RegisterNode( node );

        if( parent )
        {            
            cam_node->LinkTo( parent );
            parent_tree_item = searchTreeItemIdInNodes( parent_id );
        }
        else
        {
            scenenodegraph_entry.scenenodegraph->AddNode( node );
            parent_tree_item = scenenodegraph_entry.treeitemid;
        }

        // GUI : add item in the tree
        wxTreeItemId treeitemid = m_scenegraphs_treeCtrl->AppendItem( parent_tree_item, scene_name.c_str(), CAMERA_ICON_INDEX );
        m_scenegraphs_treeCtrl->ExpandAllChildren( parent_tree_item );

        // record the new transformation node and associated metadata

        BasicSceneMainFrame::SceneNodeEntry<CameraPoint> c_entry;

        c_entry.name = scene_name;
        c_entry.scene_node = cam_node;
        c_entry.treeitemid = treeitemid;

        m_camera_nodes[c_entry.treeitemid.GetID()] = c_entry;
        m_tree_nodes[c_entry.treeitemid.GetID()] = cam_node;
        m_inv_tree_nodes[cam_node] = c_entry.treeitemid.GetID();
    }
    else if( "ChunkNode:LinkTo" == script_call_id )
    {
        dsstring scene_name = p_propertypool.GetPropValue<dsstring>( "scene_name" );
        dsstring scenegraph_name = p_propertypool.GetPropValue<dsstring>( "scenegraph_name" );
        dsstring parent_name = p_propertypool.GetPropValue<dsstring>( "parent_name" );
        BaseSceneNode* node = p_propertypool.GetPropValue<BaseSceneNode*>( "node" );
        DrawSpace::Utils::ChunkDescriptor chunk_descr = p_propertypool.GetPropValue<DrawSpace::Utils::ChunkDescriptor>( "descriptor" );

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
            wxMessageBox( "Chunk node, unknown scenegraph name : " + scenegraph_name, "Script error", wxICON_ERROR );
            return;           
        }

        else if( !parent_found )
        {
            wxMessageBox( "Chunk node, unknown parent name : " + parent_name, "Script error", wxICON_ERROR );
            return;
        }

        SceneNode<Chunk>* chunk_node = static_cast<SceneNode<Chunk>*>( node );

        chunk_node->RegisterUpdateBeginEvtHandler( m_nodeupdatebegin_cb );

        dsstring chunk_error;
        Chunk* chunk = BuildChunk( chunk_descr, chunk_error );
        if( NULL == chunk )
        {
            wxMessageBox( "Chunk node creation error : " + chunk_error, "Script error", wxICON_ERROR );
            return;
        }
        else
        {
            chunk_node->SetContent( chunk );

            scenenodegraph_entry.scenenodegraph->RegisterNode( node );

            if( parent )
            {            
                chunk_node->LinkTo( parent );
                parent_tree_item = searchTreeItemIdInNodes( parent_id );
            }
            else
            {
                scenenodegraph_entry.scenenodegraph->AddNode( node );
                parent_tree_item = scenenodegraph_entry.treeitemid;
            }

            // GUI : add item in the tree
            wxTreeItemId treeitemid = m_scenegraphs_treeCtrl->AppendItem( parent_tree_item, scene_name.c_str(), CHUNK_ICON_INDEX );
            m_scenegraphs_treeCtrl->ExpandAllChildren( parent_tree_item );

            // record the new chunk node and associated metadata

            BasicSceneMainFrame::SceneNodeEntry<Chunk> c_entry;

            c_entry.name = scene_name;
            c_entry.scene_node = chunk_node;
            c_entry.treeitemid = treeitemid;

            m_chunk_nodes[c_entry.treeitemid.GetID()] = c_entry;
            m_tree_nodes[c_entry.treeitemid.GetID()] = chunk_node;
            m_inv_tree_nodes[chunk_node] = c_entry.treeitemid.GetID();

            // update passes output queues
            for( std::map<dsstring, ChunkPassDescriptor>::iterator it = chunk_descr.passes_slots.begin(); it != chunk_descr.passes_slots.end(); ++it )
            {
                Pass* current_pass = dynamic_cast<Pass*>( ConfigsBase::GetInstance()->GetConfigurableInstance( it->first ) );
                current_pass->GetRenderingQueue()->UpdateOutputQueue();
            }

            // store chunk description

            m_chunk_descriptors[c_entry.treeitemid.GetID()] = chunk_descr;
            
        }
    }
    else if( "DrawSpace:SetSceneNodeGraphCurrentCamera" == script_call_id )
    {
        dsstring scenegraphname = p_propertypool.GetPropValue<dsstring>( "scenegraphname" );
        dsstring cameraname = p_propertypool.GetPropValue<dsstring>( "cameraname" );

        bool sg_found = false;
        SceneNodeGraphEntry s_entry;
        
        for( std::map<void*, SceneNodeGraphEntry>::iterator it = m_scenenodegraphs.begin(); it != m_scenenodegraphs.end(); ++it )
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

        bool cam_found = false;
        SceneNodeEntry<DrawSpace::Dynamics::CameraPoint> c_entry;

        for( std::map<void*, SceneNodeEntry<DrawSpace::Dynamics::CameraPoint>>::iterator it = m_camera_nodes.begin(); it != m_camera_nodes.end(); ++it )
        {
            if( it->second.name == cameraname )
            {
                c_entry = it->second;
                cam_found = true;
                break;
            }
        }
        if( !cam_found )
        {
            wxMessageBox( "DrawSpace:SetSceneNodeGraphCurrentCamera : unknown camera name", "Script error", wxICON_ERROR );
            return;
        }
        
        void* idsg;
        for( std::map<void*, SceneNodeGraphEntry>::iterator it = m_scenenodegraphs.begin(); it != m_scenenodegraphs.end(); ++it )
        {
            if( it->second.name == scenegraphname )
            {
                if( it->second.current_camera_set )
                {
                    m_scenegraphs_treeCtrl->SetItemImage( it->second.current_camera, CAMERA_ICON_INDEX );
                }
                idsg = it->first;
                break;
            }
        }

        void* idcam = m_inv_tree_nodes[c_entry.scene_node];

        m_scenegraphs_treeCtrl->SetItemImage( c_entry.treeitemid, CAMERASEL_ICON_INDEX );
        m_scenenodegraphs[idsg].current_camera_set = true;
        m_scenenodegraphs[idsg].current_camera = c_entry.treeitemid;

        m_scenenodegraphs[idsg].scenenodegraph->SetCurrentCamera( c_entry.name );

    }
    else if( "DrawSpace:IsCurrentCamera" == script_call_id )
    {
        dsstring scenegraphname = p_propertypool.GetPropValue<dsstring>( "scenegraphname" );
        bool* result = p_propertypool.GetPropValue<bool*>( "result" );
        BaseSceneNode* camera_node = p_propertypool.GetPropValue<BaseSceneNode*>( "camera_node" );

        bool sg_found = false;
        SceneNodeGraphEntry s_entry;
        
        for( std::map<void*, SceneNodeGraphEntry>::iterator it = m_scenenodegraphs.begin(); it != m_scenenodegraphs.end(); ++it )
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

        if( s_entry.scenenodegraph->GetCurrentCamera() == camera_node )
        {
            *result = true;
        }
        else
        {
            *result = false;
        }
    }
    else if( "DrawSpace:GetSceneCameraName" == script_call_id )
    {
        dsstring scenegraphname = p_propertypool.GetPropValue<dsstring>( "scenegraphname" );
        dsstring* result = p_propertypool.GetPropValue<dsstring*>( "result" );

        bool sg_found = false;
        SceneNodeGraphEntry s_entry;
        
        for( std::map<void*, SceneNodeGraphEntry>::iterator it = m_scenenodegraphs.begin(); it != m_scenenodegraphs.end(); ++it )
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
    else if( "SpaceboxNode:UpdateShaderParam" == script_call_id )
    {
        dsstring scene_name = p_propertypool.GetPropValue<dsstring>( "scene_name" );
        dsstring pass_name = p_propertypool.GetPropValue<dsstring>( "pass_name" );
        dsstring id = p_propertypool.GetPropValue<dsstring>( "id" );
        Vector value = p_propertypool.GetPropValue<Vector>( "value" );

        for( std::map<void*, SceneNodeEntry<DrawSpace::Spacebox>>::iterator it = m_spacebox_nodes.begin(); it != m_spacebox_nodes.end(); ++it )
        {
            if( it->second.name == scene_name )
            {
                DrawSpace::Core::SceneNode<Spacebox>* sb = it->second.scene_node;
                DrawSpace::Utils::SpaceboxDescriptor sb_descr = m_spacebox_descriptors[it->first];

                if( sb_descr.passes_slots.count( pass_name ) > 0 )
                {
                    Pass* current_pass = dynamic_cast<Pass*>( ConfigsBase::GetInstance()->GetConfigurableInstance( pass_name ) );

                    int id_index = -1;

                    for( size_t i = 0; i < sb_descr.passes_slots[pass_name].shader_params.size(); i++ )
                    {
                        if( sb_descr.passes_slots[pass_name].shader_params[i].id == id )
                        {
                            id_index = i;
                            break;
                        }
                    }

                    if( id_index == -1 )
                    {
                        wxMessageBox( "SpaceboxNode:UpdateShaderParam : unknown shader param id for this node", "Script error", wxICON_ERROR );
                        return;
                    }

                    for( int j = 0; j < 6; j++ )
                    {
                        sb->GetContent()->GetNodeFromPass( current_pass, j )->SetShaderRealVector( id, value );
                    }

                    // update descriptor
                    sb_descr.passes_slots[pass_name].shader_params[id_index].value = value;

                    m_spacebox_descriptors[it->first] = sb_descr;

                    return;
                }
                else
                {
                    wxMessageBox( "SpaceboxNode:UpdateShaderParam : unknown pass name for this node", "Script error", wxICON_ERROR );
                    return;
                }
            }
        }
    }
    else if( "ChunkNode:UpdateShaderParam" == script_call_id )
    {
        dsstring scene_name = p_propertypool.GetPropValue<dsstring>( "scene_name" );
        dsstring pass_name = p_propertypool.GetPropValue<dsstring>( "pass_name" );
        dsstring id = p_propertypool.GetPropValue<dsstring>( "id" );
        Vector value = p_propertypool.GetPropValue<Vector>( "value" );


        for( std::map<void*, SceneNodeEntry<DrawSpace::Chunk>>::iterator it = m_chunk_nodes.begin(); it != m_chunk_nodes.end(); ++it )
        {
            if( it->second.name == scene_name )
            {
                DrawSpace::Core::SceneNode<Chunk>* chunk = it->second.scene_node;
                DrawSpace::Utils::ChunkDescriptor chunk_descr = m_chunk_descriptors[it->first];

                if( chunk_descr.passes_slots.count( pass_name ) > 0 )
                {
                    Pass* current_pass = dynamic_cast<Pass*>( ConfigsBase::GetInstance()->GetConfigurableInstance( pass_name ) );

                    int id_index = -1;

                    for( size_t i = 0; i < chunk_descr.passes_slots[pass_name].shader_params.size(); i++ )
                    {
                        if( chunk_descr.passes_slots[pass_name].shader_params[i].id == id )
                        {
                            id_index = i;
                            break;
                        }
                    }

                    if( id_index == -1 )
                    {
                        wxMessageBox( "ChunkNode:UpdateShaderParam : unknown shader param id for this node", "Script error", wxICON_ERROR );
                        return;
                    }

                    chunk->GetContent()->GetNodeFromPass( current_pass )->SetShaderRealVector( id, value );

                    // update descriptor
                    chunk_descr.passes_slots[pass_name].shader_params[id_index].value = value;

                    m_chunk_descriptors[it->first] = chunk_descr;

                    return;
                }
                else
                {
                    wxMessageBox( "ChunkNode:UpdateShaderParam : unknown pass name for this node", "Script error", wxICON_ERROR );
                    return;
                }
            }
        }

    }
    else if( "Keyboard:Keyboard" == script_call_id )
    {
        m_keyup_code = p_propertypool.GetPropValue<int*>( "keyupcode" );
        m_keydown_code = p_propertypool.GetPropValue<int*>( "keydowncode" );
    }
    else if( "Mouse:Mouse" == script_call_id )
    {
        m_mousemove_descr = p_propertypool.GetPropValue<DrawSpace::Utils::MouseMovementsDescriptor*>( "descriptor" );
    }
    else if( "DrawSpace:LoadKeyUpScript" == script_call_id )
    {
        dsstring filepath = p_propertypool.GetPropValue<dsstring>( "filepath" );

        long size;
        void* data = File::LoadAndAllocBinaryFile( filepath, &size );
        if( data )
        {
            char* script_text = new char[size + 1];
            memcpy( script_text, data, size );
            script_text[size] = 0;
            m_keyup_script = script_text;
        }
        else
        {
            wxMessageBox( "DrawSpace:LoadKeyUpScript : file not found", "Script error", wxICON_ERROR );
        }
    }
    else if( "DrawSpace:LoadKeyDownScript" == script_call_id )
    {
        dsstring filepath = p_propertypool.GetPropValue<dsstring>( "filepath" );

        long size;
        void* data = File::LoadAndAllocBinaryFile( filepath, &size );
        if( data )
        {
            char* script_text = new char[size + 1];
            memcpy( script_text, data, size );
            script_text[size] = 0;
            m_keydown_script = script_text;
        }
        else
        {
            wxMessageBox( "DrawSpace:LoadKeyDownScript : file not found", "Script error", wxICON_ERROR );
        }
    }
    else if( "DrawSpace:LoadMouseScript" == script_call_id )
    {
        dsstring filepath = p_propertypool.GetPropValue<dsstring>( "filepath" );

        long size;
        void* data = File::LoadAndAllocBinaryFile( filepath, &size );
        if( data )
        {
            char* script_text = new char[size + 1];
            memcpy( script_text, data, size );
            script_text[size] = 0;
            m_mousemove_script = script_text;
        }
        else
        {
            wxMessageBox( "DrawSpace:LoadMouseScript : file not found", "Script error", wxICON_ERROR );
        }
    }
    else if( "TransformationNode:LoadScript" == script_call_id || 
                "CameraPointNode:LoadScript" == script_call_id || 
                "SpaceboxNode:LoadScript" == script_call_id || 
                "FpsMovementNode:LoadScript" == script_call_id ||
                "LinearMovementNode:LoadScript" == script_call_id ||
                "FreeMovementNode:LoadScript" == script_call_id ||
                "ChunkNode:LoadScript" == script_call_id )
    {
        dsstring filepath = p_propertypool.GetPropValue<dsstring>( "filepath" );
        BaseSceneNode* node = p_propertypool.GetPropValue<BaseSceneNode*>( "node" );
        void* id = m_inv_tree_nodes[node];

        long size;
        void* data = File::LoadAndAllocBinaryFile( filepath, &size );
        if( data )
        {
            char* script_text = new char[size + 1];
            memcpy( script_text, data, size );
            script_text[size] = 0;

            if( "TransformationNode:LoadScript" == script_call_id )
            {
                m_transformation_nodes[id].script = script_text;
            }
            else if( "CameraPointNode:LoadScript" == script_call_id )
            {
                m_camera_nodes[id].script = script_text;
            }
            else if( "SpaceboxNode:LoadScript" == script_call_id )
            {
                m_spacebox_nodes[id].script = script_text;
            }
            else if( "FpsMovementNode:LoadScript" == script_call_id )
            {
                m_fps_nodes[id].script = script_text;
            }
            else if( "ChunkNode:LoadScript" == script_call_id )
            {
                m_chunk_nodes[id].script = script_text;
            }
            else if( "LinearMovementNode:LoadScript" == script_call_id )
            {
                m_lin_nodes[id].script = script_text;
            }
            else if( "FreeMovementNode:LoadScript" == script_call_id )
            {
                m_free_nodes[id].script = script_text;
            }

        }
        else
        {
            wxMessageBox( "LoadScript : file not found", "Script error", wxICON_ERROR );
        }        
    }
   


    else if( "FpsMovementNode:FpsMovementNode" == script_call_id )
    {
        dsstring scene_name = p_propertypool.GetPropValue<dsstring>( "scene_name" );
        SceneNode<FPSMovement>** node_ptr = p_propertypool.GetPropValue<SceneNode<FPSMovement>**>( "existing_node" );

        for( std::map<void*, SceneNodeEntry<DrawSpace::Core::FPSMovement>>::iterator it = m_fps_nodes.begin(); it != m_fps_nodes.end(); ++it )
        {
            if( it->second.name == scene_name )
            {
                // node exists
                *node_ptr = it->second.scene_node;
                break;
            }
        }
    }
    else if( "FpsMovementNode:LinkTo" == script_call_id )
    {
        dsstring scene_name = p_propertypool.GetPropValue<dsstring>( "scene_name" );
        dsstring scenegraph_name = p_propertypool.GetPropValue<dsstring>( "scenegraph_name" );
        dsstring parent_name = p_propertypool.GetPropValue<dsstring>( "parent_name" );
        BaseSceneNode* node = p_propertypool.GetPropValue<BaseSceneNode*>( "node" );

        Vector init_pos = p_propertypool.GetPropValue<Vector>( "init_pos" );
        dsreal init_theta = p_propertypool.GetPropValue<dsreal>( "init_theta" );
        dsreal init_phi = p_propertypool.GetPropValue<dsreal>( "init_phi" );
        bool y_mvt = p_propertypool.GetPropValue<bool>( "y_mvt" );

        wxTreeItemId parent_tree_item;
        void* parent_id = NULL;


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
            wxMessageBox( "FpsMovement node, unknown scenegraph name : " + scenegraph_name, "Script error", wxICON_ERROR );
            return;           
        }

        else if( !parent_found )
        {
            wxMessageBox( "FpsMovement node, unknown parent name : " + parent_name, "Script error", wxICON_ERROR );
            return;
        }

        SceneNode<FPSMovement>* fps_node = static_cast<SceneNode<FPSMovement>*>( node );
        fps_node->RegisterUpdateBeginEvtHandler( m_nodeupdatebegin_cb );
        
        fps_node->SetContent( new FPSMovement( y_mvt ) );
        fps_node->GetContent()->Init( init_pos, Maths::DegToRad( init_theta ), Maths::DegToRad( init_phi ) );

        scenenodegraph_entry.scenenodegraph->RegisterNode( node );

        if( parent )
        {            
            fps_node->LinkTo( parent );
            parent_tree_item = searchTreeItemIdInNodes( parent_id );
        }
        else
        {
            scenenodegraph_entry.scenenodegraph->AddNode( node );
            parent_tree_item = scenenodegraph_entry.treeitemid;
        }

        // GUI : add item in the tree
        wxTreeItemId treeitemid = m_scenegraphs_treeCtrl->AppendItem( parent_tree_item, scene_name.c_str(), MOVEMENT_ICON_INDEX );
        m_scenegraphs_treeCtrl->ExpandAllChildren( parent_tree_item );

        // record the new node and associated metadata

        BasicSceneMainFrame::SceneNodeEntry<FPSMovement> f_entry;

        f_entry.name = scene_name;
        f_entry.scene_node = fps_node;
        f_entry.treeitemid = treeitemid;

        m_fps_nodes[f_entry.treeitemid.GetID()] = f_entry;
        m_tree_nodes[f_entry.treeitemid.GetID()] = fps_node;
        m_inv_tree_nodes[fps_node] = f_entry.treeitemid.GetID();

    }

    else if( "LinearMovementNode:LinearMovementNode" == script_call_id )
    {
        dsstring scene_name = p_propertypool.GetPropValue<dsstring>( "scene_name" );
        SceneNode<LinearMovement>** node_ptr = p_propertypool.GetPropValue<SceneNode<LinearMovement>**>( "existing_node" );

        for( std::map<void*, SceneNodeEntry<DrawSpace::Core::LinearMovement>>::iterator it = m_lin_nodes.begin(); it != m_lin_nodes.end(); ++it )
        {
            if( it->second.name == scene_name )
            {
                // node exists
                *node_ptr = it->second.scene_node;
                break;
            }
        }
    }

    else if( "LinearMovementNode:LinkTo" == script_call_id )
    {
        dsstring scene_name = p_propertypool.GetPropValue<dsstring>( "scene_name" );
        dsstring scenegraph_name = p_propertypool.GetPropValue<dsstring>( "scenegraph_name" );
        dsstring parent_name = p_propertypool.GetPropValue<dsstring>( "parent_name" );
        BaseSceneNode* node = p_propertypool.GetPropValue<BaseSceneNode*>( "node" );

        Vector init_pos = p_propertypool.GetPropValue<Vector>( "init_pos" );
        Vector dir = p_propertypool.GetPropValue<Vector>( "dir" );
        dsreal init_theta = p_propertypool.GetPropValue<dsreal>( "init_theta" );
        dsreal init_phi = p_propertypool.GetPropValue<dsreal>( "init_phi" );

        wxTreeItemId parent_tree_item;
        void* parent_id = NULL;


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
            wxMessageBox( "LinearMovement node, unknown scenegraph name : " + scenegraph_name, "Script error", wxICON_ERROR );
            return;           
        }

        else if( !parent_found )
        {
            wxMessageBox( "LinearMovement node, unknown parent name : " + parent_name, "Script error", wxICON_ERROR );
            return;
        }

        SceneNode<LinearMovement>* lin_node = static_cast<SceneNode<LinearMovement>*>( node );
        lin_node->RegisterUpdateBeginEvtHandler( m_nodeupdatebegin_cb );

        lin_node->SetContent( new LinearMovement() );
        lin_node->GetContent()->Init( init_pos, dir, init_theta, init_phi );

        scenenodegraph_entry.scenenodegraph->RegisterNode( node );

        if( parent )
        {            
            lin_node->LinkTo( parent );
            parent_tree_item = searchTreeItemIdInNodes( parent_id );
        }
        else
        {
            scenenodegraph_entry.scenenodegraph->AddNode( node );
            parent_tree_item = scenenodegraph_entry.treeitemid;
        }

        // GUI : add item in the tree
        wxTreeItemId treeitemid = m_scenegraphs_treeCtrl->AppendItem( parent_tree_item, scene_name.c_str(), MOVEMENT_ICON_INDEX );
        m_scenegraphs_treeCtrl->ExpandAllChildren( parent_tree_item );

        // record the new node and associated metadata

        BasicSceneMainFrame::SceneNodeEntry<LinearMovement> l_entry;

        l_entry.name = scene_name;
        l_entry.scene_node = lin_node;
        l_entry.treeitemid = treeitemid;

        m_lin_nodes[l_entry.treeitemid.GetID()] = l_entry;
        m_tree_nodes[l_entry.treeitemid.GetID()] = lin_node;
        m_inv_tree_nodes[lin_node] = l_entry.treeitemid.GetID();
    }

    else if( "FreeMovementNode:FreeMovementNode" == script_call_id )
    {
        dsstring scene_name = p_propertypool.GetPropValue<dsstring>( "scene_name" );
        SceneNode<FreeMovement>** node_ptr = p_propertypool.GetPropValue<SceneNode<FreeMovement>**>( "existing_node" );

        for( std::map<void*, SceneNodeEntry<DrawSpace::Core::FreeMovement>>::iterator it = m_free_nodes.begin(); it != m_free_nodes.end(); ++it )
        {
            if( it->second.name == scene_name )
            {
                // node exists
                *node_ptr = it->second.scene_node;
                break;
            }
        }        
    }
    else if( "FreeMovementNode:LinkTo" == script_call_id )
    {
        dsstring scene_name = p_propertypool.GetPropValue<dsstring>( "scene_name" );
        dsstring scenegraph_name = p_propertypool.GetPropValue<dsstring>( "scenegraph_name" );
        dsstring parent_name = p_propertypool.GetPropValue<dsstring>( "parent_name" );
        BaseSceneNode* node = p_propertypool.GetPropValue<BaseSceneNode*>( "node" );

        Vector init_pos = p_propertypool.GetPropValue<Vector>( "init_pos" );

        wxTreeItemId parent_tree_item;
        void* parent_id = NULL;


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
            wxMessageBox( "FreeMovement node, unknown scenegraph name : " + scenegraph_name, "Script error", wxICON_ERROR );
            return;           
        }

        else if( !parent_found )
        {
            wxMessageBox( "FreeMovement node, unknown parent name : " + parent_name, "Script error", wxICON_ERROR );
            return;
        }

        SceneNode<FreeMovement>* free_node = static_cast<SceneNode<FreeMovement>*>( node );
        free_node->RegisterUpdateBeginEvtHandler( m_nodeupdatebegin_cb );

        free_node->SetContent( new FreeMovement() );
        free_node->GetContent()->Init( init_pos );

        scenenodegraph_entry.scenenodegraph->RegisterNode( node );

        if( parent )
        {            
            free_node->LinkTo( parent );
            parent_tree_item = searchTreeItemIdInNodes( parent_id );
        }
        else
        {
            scenenodegraph_entry.scenenodegraph->AddNode( node );
            parent_tree_item = scenenodegraph_entry.treeitemid;
        }

        // GUI : add item in the tree
        wxTreeItemId treeitemid = m_scenegraphs_treeCtrl->AppendItem( parent_tree_item, scene_name.c_str(), MOVEMENT_ICON_INDEX );
        m_scenegraphs_treeCtrl->ExpandAllChildren( parent_tree_item );

        // record the new node and associated metadata

        BasicSceneMainFrame::SceneNodeEntry<FreeMovement> f_entry;

        f_entry.name = scene_name;
        f_entry.scene_node = free_node;
        f_entry.treeitemid = treeitemid;

        m_free_nodes[f_entry.treeitemid.GetID()] = f_entry;
        m_tree_nodes[f_entry.treeitemid.GetID()] = free_node;
        m_inv_tree_nodes[free_node] = f_entry.treeitemid.GetID();

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

void BasicSceneMainFrame::OnClose( wxCloseEvent& p_event )
{
#ifndef RENDER_IDLE
    m_rendertimer->Stop();
    delete m_rendertimer;
#endif
    Destroy();
}


void BasicSceneMainFrame::OnKeyDown( wxKeyEvent& p_event )
{
    int keycode = p_event.GetKeyCode();

    if( m_keydown_script_enabled )
    {
        if( m_keydown_code )
        {
            *m_keydown_code = keycode;
        }

        bool status = m_scripting->ExecChunk( m_keydown_script.c_str() );
        if( !status )
        {
            m_keydown_script_enabled = false;
        }
    }
}
void BasicSceneMainFrame::OnKeyUp( wxKeyEvent& p_event )
{
    int keycode = p_event.GetKeyCode();

    if( m_keyup_script_enabled )
    {
        if( m_keyup_code )
        {
            *m_keyup_code = keycode;
        }

        bool status = m_scripting->ExecChunk( m_keyup_script.c_str() );
        if( !status )
        {
            m_keyup_script_enabled = false;
        }
    }
}

void BasicSceneMainFrame::OnMouseLeftDown( wxMouseEvent& event )
{
    if( m_mousemove_script_enabled )
    {
        if( m_mousemove_descr )
        {
            m_mousemove_descr->leftbutton_down = true;
        }

        bool status = m_scripting->ExecChunk( m_mousemove_script.c_str() );
        if( !status )
        {
            m_mousemove_script_enabled = false;
        }
    }
}

void BasicSceneMainFrame::OnMouseLeftUp( wxMouseEvent& event )
{
    if( m_mousemove_script_enabled )
    {
        if( m_mousemove_descr )
        {
            m_mousemove_descr->leftbutton_down = false;
        }

        bool status = m_scripting->ExecChunk( m_mousemove_script.c_str() );
        if( !status )
        {
            m_mousemove_script_enabled = false;
        }
    }
}

void BasicSceneMainFrame::OnMouseRightDown( wxMouseEvent& event )
{
    if( m_mousemove_script_enabled )
    {
        if( m_mousemove_descr )
        {
            m_mousemove_descr->rightbutton_down = true;
        }

        bool status = m_scripting->ExecChunk( m_mousemove_script.c_str() );
        if( !status )
        {
            m_mousemove_script_enabled = false;
        }
    }
}

void BasicSceneMainFrame::OnMouseRightUp( wxMouseEvent& event )
{
    if( m_mousemove_script_enabled )
    {
        if( m_mousemove_descr )
        {
            m_mousemove_descr->rightbutton_down = false;
        }

        bool status = m_scripting->ExecChunk( m_mousemove_script.c_str() );
        if( !status )
        {
            m_mousemove_script_enabled = false;
        }
    }
}


void BasicSceneMainFrame::OnMouseMotion( wxMouseEvent& p_event )
{    
    if( !p_event.Dragging() )
    {
        m_delta_mouse_init = true;
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

    /*
    int delta_x = curr_xmouse - m_last_xmouse;
    int delta_y = curr_ymouse - m_last_ymouse;
    */

    int delta_x, delta_y;

    if( m_delta_mouse_init )
    {
        delta_x = delta_y = 0;
        m_delta_mouse_init = false;
    }
    else
    {
        delta_x = curr_xmouse - m_last_xmouse;
        delta_y = curr_ymouse - m_last_ymouse;
    }


    m_last_xmouse = curr_xmouse;
    m_last_ymouse = curr_ymouse;


    if( m_mousemove_script_enabled )
    {
        if( m_mousemove_descr )
        {
            m_mousemove_descr->xmouse = m_last_xmouse;
            m_mousemove_descr->ymouse = m_last_ymouse;

            m_mousemove_descr->delta_xmouse = delta_x;
            m_mousemove_descr->delta_ymouse = delta_y;
        }

        bool status = m_scripting->ExecChunk( m_mousemove_script.c_str() );
        if( !status )
        {
            m_mousemove_script_enabled = false;
        }
    }
}

void BasicSceneMainFrame::OnPaint( wxPaintEvent& p_event )
{
    render();
}

void BasicSceneMainFrame::render( void )
{
    if( m_glready )
    {           
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
            /*
            dsstring camera_name;
            m_scenegraph.GetCurrentCameraName( camera_name );
            if( "" == camera_name )
            {
                camera_name = "...";
            }
            renderer->DrawText( 255, 0, 0, 10, 40, "current camera : %s", camera_name.c_str() );
            */
            
            dsstring camera_name;
            m_scenegraph_currentcameradisplay.scenenodegraph->GetCurrentCameraName( camera_name );

            if( "" == camera_name )
            {
                camera_name = "...";
            }
            renderer->DrawText( 255, 0, 0, 10, 40, "scene %s current camera : %s", m_scenegraph_currentcameradisplay.name.c_str(), camera_name.c_str() );
        }
        


        renderer->FlipScreen();

        m_timer.Update();
        if( m_timer.IsReady() )
        {

        }
    }
}

void BasicSceneMainFrame::OnIdle( wxIdleEvent& p_event )
{
#ifdef RENDER_IDLE
    render();
    //wxWidget framework specific !
    p_event.RequestMore( true );
#endif
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
    //wxWidgetAdapter::GetInstance()->AdaptCameraListComboBox( &m_scenegraph, m_cameraslist_comboBox );

    

    ConfigsBase::GetInstance()->GetOrderedConfigsInstancesList( m_ordered_configs );
    
    for( size_t i = 0; i < m_ordered_configs.size(); i++ )
    {
        Pass* pass = dynamic_cast<Pass*>( m_ordered_configs[i] );
        if( pass )
        {
            //m_scenegraph.RegisterPass( pass );
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
    wxBitmap bmp_keyboard( "icon_keyboard.bmp", wxBITMAP_TYPE_BMP );
    wxBitmap bmp_mouse( "icon_mouse.bmp", wxBITMAP_TYPE_BMP );
    wxBitmap bmp_camera_sel( "icon_camera_sel.bmp", wxBITMAP_TYPE_BMP );


    
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
    pImageList->Add( bmp_keyboard );
    pImageList->Add( bmp_mouse );
    pImageList->Add( bmp_camera_sel );
    

    m_scenegraphs_treeCtrl->AssignImageList( pImageList );
    
    m_scenegraphs_root_item = m_scenegraphs_treeCtrl->AddRoot( "DrawSpace", DRAWSPACE_ICON_INDEX );
    m_keyboard_item = m_scenegraphs_treeCtrl->AppendItem( m_scenegraphs_root_item, "Keyboard", KEYBOARD_ICON_INDEX );
    m_mouse_item = m_scenegraphs_treeCtrl->AppendItem( m_scenegraphs_root_item, "Mouse", MOUSE_ICON_INDEX );
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
        dsstring mode = props["mode"].GetPropValue<dsstring>();
        dsstring texturefilepath = props["filespath"].GetPropValue<dsstring>( "texturefilepath" );
        dsstring metricsfilepath = props["filespath"].GetPropValue<dsstring>( "metricsfilepath" );

        DIALOG_DECLARE( DIALOG_FONT_PROPS_TITLE )

        DIALOG_APPENDROOT_STRING( "assetname", assetname )
        DIALOG_APPENDROOT_STRING( "mode", mode )
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
        dsstring mode = props["mode"].GetPropValue<dsstring>();

        DIALOG_DECLARE( DIALOG_MESHE_PROPS_TITLE )

        DIALOG_APPENDROOT_STRING( "assetname", assetname )
        DIALOG_APPENDROOT_STRING( "filepath", filepath )
        DIALOG_APPENDROOT_INTEGER( "index", index )
        DIALOG_APPENDROOT_STRING( "mode", mode )

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

        ///////////////////////////////////////////////////

        std::map<dsstring, DrawSpace::Core::Configurable*> config_list;
        ConfigsBase::GetInstance()->GetConfigsInstancesList( config_list );
        for( std::map<dsstring, DrawSpace::Core::Configurable*>::iterator it = config_list.begin(); it != config_list.end(); ++it )
        {
            if( it->second == config )
            {
                DIALOG_APPENDROOT_STRING( "configname", it->first )
            }
        }

        

        ///////////////////////////////////////////////////

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
        DIALOG_APPENDROOT_STRING( "configname", passname )

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

    BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, "Shader parameters modification", m_last_clicked_treeitem );

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
                    m_transformation_nodes[id].scene_node->GetContent()->GetMatrixChain( mat_chain );


                    DIALOG_APPENDROOT_STRING( "scene name", m_transformation_nodes[id].name );

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

        case CONTEXTMENU_NEWCHUNK:
            {
                DIALOG_DECLARE( DIALOG_CHUNK_CREATION_TITLE )

                DIALOG_APPENDROOT_STRING( "scene name", "" )

                DIALOG_APPENDROOT_ENUM( "meshe",  get_meshes_list() )

                DIALOG_APPLY
                DIALOG_SPECIFIC0( "New pass slot" )
                DIALOG_SPECIFIC1( "New shaders param slot" )

                DIALOG_SHOW
            }
            break;

        case CONTEXTMENU_EDIT_SHADERSPARAMS:
            {               
                void* id = m_last_clicked_treeitem.GetID();
                if( m_spacebox_descriptors.count( id ) > 0 )
                {
                    DrawSpace::Utils::SpaceboxDescriptor sb_descr = m_spacebox_descriptors[id];

                    DIALOG_DECLARE( DIALOG_SPACEBOX_EDITION_TITLE )

                    
                    DIALOG_APPENDROOT_STRING( "scene name", sb_descr.scene_name )

                    for( std::map<dsstring, SpaceboxPassDescriptor>::iterator it = sb_descr.passes_slots.begin(); it != sb_descr.passes_slots.end(); ++it )
                    {
                        SpaceboxPassDescriptor pass_descr = it->second;                                                

                        DIALOG_BUILD_LABELS( pass_descr.shader_params.size(), "shader parameter %d", params_list )

                        DIALOG_APPENDROOT_ITERATE_NODE_BEGIN( i, params_list, param_root )

                            DIALOG_APPENDNODE_STRING( param_root, "pass name", it->first )

                            DIALOG_APPENDNODE_STRING( param_root, "param id", pass_descr.shader_params[i].id );

                            DIALOG_APPENDNODE_NODE( param_root, "values", shader_param_values_root )

                            DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "x", pass_descr.shader_params[i].value[0] )
                            DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "y", pass_descr.shader_params[i].value[1] )
                            DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "z", pass_descr.shader_params[i].value[2] )
                            DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "w", pass_descr.shader_params[i].value[3] )                           

                        DIALOG_APPENDROOT_ITERATE_NODE_END
                    }

                    DIALOG_APPLY

                    DIALOG_SHOW
                }
                else if( m_chunk_descriptors.count( id ) > 0 )
                {
                    DrawSpace::Utils::ChunkDescriptor chunk_descr = m_chunk_descriptors[id];

                    DIALOG_DECLARE( DIALOG_CHUNK_EDITION_TITLE )
                   
                    DIALOG_APPENDROOT_STRING( "scene name", chunk_descr.scene_name )

                    for( std::map<dsstring, ChunkPassDescriptor>::iterator it = chunk_descr.passes_slots.begin(); it != chunk_descr.passes_slots.end(); ++it )
                    {
                        ChunkPassDescriptor pass_descr = it->second;                                                

                        DIALOG_BUILD_LABELS( pass_descr.shader_params.size(), "shader parameter %d", params_list )

                        DIALOG_APPENDROOT_ITERATE_NODE_BEGIN( i, params_list, param_root )

                            DIALOG_APPENDNODE_STRING( param_root, "pass name", it->first )

                            DIALOG_APPENDNODE_STRING( param_root, "param id", pass_descr.shader_params[i].id );

                            DIALOG_APPENDNODE_NODE( param_root, "values", shader_param_values_root )

                            DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "x", pass_descr.shader_params[i].value[0] )
                            DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "y", pass_descr.shader_params[i].value[1] )
                            DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "z", pass_descr.shader_params[i].value[2] )
                            DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "w", pass_descr.shader_params[i].value[3] )                           

                        DIALOG_APPENDROOT_ITERATE_NODE_END
                    }

                    DIALOG_APPLY

                    DIALOG_SHOW
                }

            }
            break;

        case CONTEXTMENU_SHOW_PROPS:
            {               
                void* id = m_last_clicked_treeitem.GetID();
                if( m_spacebox_descriptors.count( id ) > 0 )
                {
                    DrawSpace::Utils::SpaceboxDescriptor sb_descr = m_spacebox_descriptors[id];

                    DIALOG_DECLARE( DIALOG_SPACEBOX_PROPS_TITLE )

                    DIALOG_APPENDROOT_STRING( "scene name", sb_descr.scene_name )

                    for( std::map<dsstring, SpaceboxPassDescriptor>::iterator it = sb_descr.passes_slots.begin(); it != sb_descr.passes_slots.end(); ++it )
                    {
                        SpaceboxPassDescriptor pass_descr = it->second;                        
                        //DIALOG_APPENDROOT_NODE( pass_labels[label_index], pass_root )

                        DIALOG_APPENDROOT_NODE( it->first, pass_root )

                        DIALOG_APPENDNODE_STRING( pass_root, "fx name", pass_descr.fx_name )
                        DIALOG_APPENDNODE_INTEGER( pass_root, "rendering order", pass_descr.rendering_order )

                        DIALOG_BUILD_LABELS( RenderingNode::NbMaxTextures, "stage %d", texture_stages )


                        {
                            DIALOG_APPENDNODE_NODE( pass_root, "front textures", front_root )
                            for( size_t i = 0; i < texture_stages.size(); i++ )
                            {
                                if( pass_descr.textures[Spacebox::FrontQuad][i] != "" )
                                {
                                    DIALOG_APPENDNODE_STRING( front_root, texture_stages[i], pass_descr.textures[Spacebox::FrontQuad][i] )
                                }
                            }
                        }

                        {
                            DIALOG_APPENDNODE_NODE( pass_root, "rear textures", rear_root )
                            for( size_t i = 0; i < texture_stages.size(); i++ )
                            {
                                if( pass_descr.textures[Spacebox::RearQuad][i] != "" )
                                {
                                    DIALOG_APPENDNODE_STRING( rear_root, texture_stages[i], pass_descr.textures[Spacebox::RearQuad][i] )
                                }
                            }
                        }

                        {
                            DIALOG_APPENDNODE_NODE( pass_root, "left textures", left_root )
                            for( size_t i = 0; i < texture_stages.size(); i++ )
                            {
                                if( pass_descr.textures[Spacebox::LeftQuad][i] != "" )
                                {
                                    DIALOG_APPENDNODE_STRING( left_root, texture_stages[i], pass_descr.textures[Spacebox::LeftQuad][i] )
                                }
                            }
                        }

                        {
                            DIALOG_APPENDNODE_NODE( pass_root, "right textures", right_root )
                            for( size_t i = 0; i < texture_stages.size(); i++ )
                            {
                                if( pass_descr.textures[Spacebox::RightQuad][i] != "" )
                                {
                                    DIALOG_APPENDNODE_STRING( right_root, texture_stages[i], pass_descr.textures[Spacebox::RightQuad][i] )
                                }
                            }
                        }

                        {
                            DIALOG_APPENDNODE_NODE( pass_root, "top textures", top_root )
                            for( size_t i = 0; i < texture_stages.size(); i++ )
                            {
                                if( pass_descr.textures[Spacebox::TopQuad][i] != "" )
                                {
                                    DIALOG_APPENDNODE_STRING( top_root, texture_stages[i], pass_descr.textures[Spacebox::TopQuad][i] )
                                }
                            }
                        }

                        {
                            DIALOG_APPENDNODE_NODE( pass_root, "bottom textures", bottom_root )
                            for( size_t i = 0; i < texture_stages.size(); i++ )
                            {
                                if( pass_descr.textures[Spacebox::BottomQuad][i] != "" )
                                {
                                    DIALOG_APPENDNODE_STRING( bottom_root, texture_stages[i], pass_descr.textures[Spacebox::BottomQuad][i] )
                                }
                            }
                        }


                        for( size_t i = 0; i < pass_descr.shader_params.size(); i++ )
                        {
                            DIALOG_APPENDNODE_NODE( pass_root, pass_descr.shader_params[i].id, shader_param_root )

                            DIALOG_APPENDNODE_INTEGER( shader_param_root, "shader index", pass_descr.shader_params[i].shader_index )
                            DIALOG_APPENDNODE_INTEGER( shader_param_root, "shader register", pass_descr.shader_params[i].shader_register )

                            DIALOG_APPENDNODE_NODE( shader_param_root, "values", shader_param_values_root )

                            DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "x", pass_descr.shader_params[i].value[0] )
                            DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "y", pass_descr.shader_params[i].value[1] )
                            DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "z", pass_descr.shader_params[i].value[2] )
                            DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "w", pass_descr.shader_params[i].value[3] )

                        }
                    }
                 
                    DIALOG_SHOW
                }
                else if( m_chunk_descriptors.count( id ) > 0 )
                {
                    DrawSpace::Utils::ChunkDescriptor chunk_descr = m_chunk_descriptors[id];

                    DIALOG_DECLARE( DIALOG_CHUNK_PROPS_TITLE )

                    DIALOG_APPENDROOT_STRING( "scene name", chunk_descr.scene_name )

                    for( std::map<dsstring, ChunkPassDescriptor>::iterator it = chunk_descr.passes_slots.begin(); it != chunk_descr.passes_slots.end(); ++it )
                    {
                        ChunkPassDescriptor pass_descr = it->second;                        

                        DIALOG_APPENDROOT_NODE( it->first, pass_root )

                        DIALOG_APPENDNODE_STRING( pass_root, "fx name", pass_descr.fx_name )
                        DIALOG_APPENDNODE_INTEGER( pass_root, "rendering order", pass_descr.rendering_order )

                        DIALOG_BUILD_LABELS( RenderingNode::NbMaxTextures, "stage %d", texture_stages )

                        DIALOG_APPENDNODE_NODE( pass_root, "textures", textures_root )

                        for( size_t i = 0; i < texture_stages.size(); i++ )
                        {
                            if( pass_descr.textures[i] != "" )
                            {
                                DIALOG_APPENDNODE_STRING( textures_root, texture_stages[i], pass_descr.textures[i] )
                            }
                        }

                        for( size_t i = 0; i < pass_descr.shader_params.size(); i++ )
                        {
                            DIALOG_APPENDNODE_NODE( pass_root, pass_descr.shader_params[i].id, shader_param_root )

                            DIALOG_APPENDNODE_INTEGER( shader_param_root, "shader index", pass_descr.shader_params[i].shader_index )
                            DIALOG_APPENDNODE_INTEGER( shader_param_root, "shader register", pass_descr.shader_params[i].shader_register )

                            DIALOG_APPENDNODE_NODE( shader_param_root, "values", shader_param_values_root )

                            DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "x", pass_descr.shader_params[i].value[0] )
                            DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "y", pass_descr.shader_params[i].value[1] )
                            DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "z", pass_descr.shader_params[i].value[2] )
                            DIALOG_APPENDNODE_FLOAT( shader_param_values_root, "w", pass_descr.shader_params[i].value[3] )

                        }
                    }

                    DIALOG_SHOW
                }
                else if( m_camera_nodes.count( id ) > 0 )
                {
                    CameraPoint* camera = m_camera_nodes[id].scene_node->GetContent();

                    CameraPoint::Infos infos;
                    camera->GetInfos( infos );

                    DIALOG_DECLARE( DIALOG_CAMERA_PROPS_TITLE )

                    DIALOG_APPENDROOT_STRING( "scene name", m_camera_nodes[id].name );

                    DIALOG_APPENDROOT_BOOL( "relative to an orbiter", ( infos.relative_orbiter != NULL ? true : false ) );
                    if( infos.relative_orbiter != NULL )
                    {
                        DIALOG_APPENDROOT_FLOAT( "altitud", infos.altitud );
                    }

                    DIALOG_APPENDROOT_BOOL( "locked on node", infos.locked_on_node );
                    if( infos.locked_on_node )
                    {
                        DIALOG_APPENDROOT_STRING( "locked on", infos.locked_node_alias );
                    }
                    
                    DIALOG_SHOW
                }
                else if( m_fps_nodes.count( id ) > 0 )
                {
                    FPSMovement* fps = m_fps_nodes[id].scene_node->GetContent();

                    Vector pos;
                    dsreal curr_yaw, curr_pitch;
                    fps->GetCurrentPos( pos );
                    curr_yaw = fps->GetCurrentYaw();
                    curr_pitch = fps->GetCurrentPitch();
                   
                    DIALOG_DECLARE( DIALOG_FPSMVT_PROPS_TITLE )
                    DIALOG_APPENDROOT_STRING( "scene name", m_fps_nodes[id].name );
                    DIALOG_APPENDROOT_STRING( "movement type ", "fps" );
                    DIALOG_APPENDROOT_FLOAT( "current yaw", Maths::RadToDeg( curr_yaw ) );
                    DIALOG_APPENDROOT_FLOAT( "current pitch", Maths::RadToDeg( curr_pitch ) );
                    DIALOG_APPENDROOT_NODE( "current position", curr_pos_root );
                    DIALOG_APPENDNODE_FLOAT( curr_pos_root, "x", pos[0] );
                    DIALOG_APPENDNODE_FLOAT( curr_pos_root, "y", pos[1] );
                    DIALOG_APPENDNODE_FLOAT( curr_pos_root, "z", pos[2] );

                    DIALOG_SHOW
                }

                else if( m_lin_nodes.count( id ) > 0 )
                {
                    LinearMovement* lin = m_lin_nodes[id].scene_node->GetContent();

                    Vector init_pos;
                    Vector curr_pos;
                    Vector dir;

                    dsreal curr_theta, curr_phi, curr_speed;

                    lin->GetCurrentPos( curr_pos );
                    lin->GetInitPos( init_pos );
                    lin->GetDirection( dir );

                    curr_theta = lin->GetCurrentTheta();
                    curr_phi = lin->GetCurrentPhi();
                    curr_speed = lin->GetCurrentSpeed();

                    DIALOG_DECLARE( DIALOG_LINMVT_PROPS_TITLE )
                    DIALOG_APPENDROOT_STRING( "scene name", m_lin_nodes[id].name );
                    DIALOG_APPENDROOT_STRING( "movement type ", "linear" );

                    DIALOG_APPENDROOT_FLOAT( "current theta", curr_theta );
                    DIALOG_APPENDROOT_FLOAT( "current phi", curr_phi );
                    DIALOG_APPENDROOT_FLOAT( "current speed", curr_speed );

                    DIALOG_APPENDROOT_NODE( "initial position", init_pos_root );
                    DIALOG_APPENDNODE_FLOAT( init_pos_root, "x", init_pos[0] );
                    DIALOG_APPENDNODE_FLOAT( init_pos_root, "y", init_pos[1] );
                    DIALOG_APPENDNODE_FLOAT( init_pos_root, "z", init_pos[2] );

                    DIALOG_APPENDROOT_NODE( "current position", curr_pos_root );
                    DIALOG_APPENDNODE_FLOAT( curr_pos_root, "x", curr_pos[0] );
                    DIALOG_APPENDNODE_FLOAT( curr_pos_root, "y", curr_pos[1] );
                    DIALOG_APPENDNODE_FLOAT( curr_pos_root, "z", curr_pos[2] );

                    DIALOG_APPENDROOT_NODE( "direction", dir_root );
                    DIALOG_APPENDNODE_FLOAT( dir_root, "x", dir[0] );
                    DIALOG_APPENDNODE_FLOAT( dir_root, "y", dir[1] );
                    DIALOG_APPENDNODE_FLOAT( dir_root, "z", dir[2] );

                    DIALOG_SHOW                    
                }
            }
            break;

        case CONTEXTMENU_EDIT_NODESCRIPT:
            {

                dsstring title;
                dsstring* script_text;
                bool * script_state;
                void* id = m_last_clicked_treeitem.GetID();

                if( m_transformation_nodes.count( id ) > 0 )
                {
                    title = "Transformation node: ";
                    title += m_transformation_nodes[id].name;

                    script_text = &m_transformation_nodes[id].script;
                    script_state = &m_transformation_nodes[id].script_enabled;
                }
                else if( m_spacebox_nodes.count( id ) > 0 )
                {
                    title = "Spacebox node: ";
                    title += m_spacebox_nodes[id].name;

                    script_text = &m_spacebox_nodes[id].script;
                    script_state = &m_spacebox_nodes[id].script_enabled;
                }
                else if( m_camera_nodes.count( id ) > 0 )
                {
                    title = "CameraPoint node: ";
                    title += m_camera_nodes[id].name;

                    script_text = &m_camera_nodes[id].script;
                    script_state = &m_camera_nodes[id].script_enabled;
                }
                else if( m_fps_nodes.count( id ) > 0 )
                {
                    title = "FPS movement node: ";
                    title += m_fps_nodes[id].name;

                    script_text = &m_fps_nodes[id].script;
                    script_state = &m_fps_nodes[id].script_enabled;
                }
                else if( m_chunk_nodes.count( id ) > 0 )
                {
                    title = "Chunk node: ";
                    title += m_chunk_nodes[id].name;

                    script_text = &m_chunk_nodes[id].script;
                    script_state = &m_chunk_nodes[id].script_enabled;
                }
                else if( m_lin_nodes.count( id ) > 0 )
                {
                    title = "Linear movement node: ";
                    title += m_lin_nodes[id].name;

                    script_text = &m_lin_nodes[id].script;
                    script_state = &m_lin_nodes[id].script_enabled;
                }
                else if( m_free_nodes.count( id ) > 0 )
                {
                    title = "Free movement node: ";
                    title += m_free_nodes[id].name;

                    script_text = &m_free_nodes[id].script;
                    script_state = &m_free_nodes[id].script_enabled;
                }

                BasicSceneScriptEditFrame* frame = new BasicSceneScriptEditFrame( this, title, script_text, script_state );
                frame->Show();
            }
            break;

        case CONTEXTMENU_EDIT_MOUSEMOVESCRIPT:
            {
                BasicSceneScriptEditFrame* frame = new BasicSceneScriptEditFrame( this, "Mouse move", &m_mousemove_script, &m_mousemove_script_enabled );
                frame->Show();
            }
            break;


        case CONTEXTMENU_EDIT_KEYDOWNSCRIPT:
            {
                BasicSceneScriptEditFrame* frame = new BasicSceneScriptEditFrame( this, "Key down", &m_keydown_script, &m_keydown_script_enabled );
                frame->Show();
            }
            break;

        case CONTEXTMENU_EDIT_KEYUPSCRIPT:
            {
                BasicSceneScriptEditFrame* frame = new BasicSceneScriptEditFrame( this, "Key up", &m_keyup_script, &m_keyup_script_enabled );
                frame->Show();
            }
            break;

        case CONTEXTMENU_NEWCAMERA:
            {
                DIALOG_DECLARE( DIALOG_CAMERA_CREATION_TITLE )

                DIALOG_APPENDROOT_STRING( "scene name", "" )
                DIALOG_APPLY
                DIALOG_SHOW
            }
            break;

        case CONTEXTMENU_SELECT_CAMERA:
            {
                void* id = find_scenenodegraph_id( m_last_clicked_treeitem );

                if( m_scenenodegraphs[id].current_camera_set )
                {
                    m_scenegraphs_treeCtrl->SetItemImage( m_scenenodegraphs[id].current_camera, CAMERA_ICON_INDEX );
                }

                m_scenegraphs_treeCtrl->SetItemImage( m_last_clicked_treeitem, CAMERASEL_ICON_INDEX );
                m_scenenodegraphs[id].current_camera_set = true;
                m_scenenodegraphs[id].current_camera = m_last_clicked_treeitem;

                m_scenenodegraphs[id].scenenodegraph->SetCurrentCamera( m_camera_nodes[m_last_clicked_treeitem.GetID()].name );

            }
            break;

        case CONTEXTMENU_EDIT_CAMERA:
            {
                void* id = m_last_clicked_treeitem.GetID();

                SceneNodeEntry<DrawSpace::Dynamics::CameraPoint> camera_node = m_camera_nodes[id];

                DIALOG_DECLARE( DIALOG_CAMERA_EDIT_TITLE )

                DIALOG_APPENDROOT_STRING( "scene name", camera_node.name );
                DIALOG_APPENDROOT_FLOAT( "znear", camera_node.scene_node->GetContent()->GetZNear() );
                DIALOG_APPLY
                DIALOG_SHOW
            }
            break;

        case CONTEXTMENU_NEWFPSMVT:
            {
                DIALOG_DECLARE( DIALOG_FPSMVT_CREATION_TITLE )

                DIALOG_APPENDROOT_STRING( "scene name", "" )
                DIALOG_APPENDROOT_NODE( "initial position", init_pos_root )
                DIALOG_APPENDNODE_FLOAT( init_pos_root, "x", 0.0 );
                DIALOG_APPENDNODE_FLOAT( init_pos_root, "y", 0.0 );
                DIALOG_APPENDNODE_FLOAT( init_pos_root, "z", 0.0 );

                DIALOG_APPENDROOT_FLOAT( "initial theta", 0.0 );
                DIALOG_APPENDROOT_FLOAT( "initial phi", 0.0 );
                DIALOG_APPENDROOT_BOOL( "y movement", 0.0 );

                DIALOG_APPLY
                DIALOG_SHOW
            }
            break;

        case CONTEXTMENU_NEWLINEARMVT:
            {
                DIALOG_DECLARE( DIALOG_LINMVT_CREATION_TITLE )

                DIALOG_APPENDROOT_STRING( "scene name", "" )
                DIALOG_APPENDROOT_NODE( "initial position", init_pos_root )
                DIALOG_APPENDNODE_FLOAT( init_pos_root, "x", 0.0 );
                DIALOG_APPENDNODE_FLOAT( init_pos_root, "y", 0.0 );
                DIALOG_APPENDNODE_FLOAT( init_pos_root, "z", 0.0 );

                DIALOG_APPENDROOT_NODE( "direction", dir_root )
                DIALOG_APPENDNODE_FLOAT( dir_root, "x", 0.0 );
                DIALOG_APPENDNODE_FLOAT( dir_root, "y", 0.0 );
                DIALOG_APPENDNODE_FLOAT( dir_root, "z", 0.0 );

                DIALOG_APPENDROOT_FLOAT( "initial theta", 0.0 );
                DIALOG_APPENDROOT_FLOAT( "initial phi", 0.0 );

                DIALOG_APPLY
                DIALOG_SHOW
            }
            break;

        case CONTEXTMENU_NEWFREEMVT:
            {
                DIALOG_DECLARE( DIALOG_FREEMVT_CREATION_TITLE )

                DIALOG_APPENDROOT_STRING( "scene name", "" )
                DIALOG_APPENDROOT_NODE( "initial position", init_pos_root )
                DIALOG_APPENDNODE_FLOAT( init_pos_root, "x", 0.0 );
                DIALOG_APPENDNODE_FLOAT( init_pos_root, "y", 0.0 );
                DIALOG_APPENDNODE_FLOAT( init_pos_root, "z", 0.0 );

                DIALOG_APPLY
                DIALOG_SHOW

            }
            break;

        case CONTEXTMENU_EDIT_MVT:
            {
                void* id = m_last_clicked_treeitem.GetID();
                if( m_lin_nodes.count( id ) > 0 )
                {
                    SceneNodeEntry<LinearMovement> lin_node = m_lin_nodes[id];                    
                    LinearMovement* linmvt = lin_node.scene_node->GetContent();

                    dsreal curr_theta = linmvt->GetCurrentTheta();
                    dsreal curr_phi = linmvt->GetCurrentPhi();
                    dsreal curr_speed = linmvt->GetCurrentSpeed();

                    DIALOG_DECLARE( DIALOG_LINMVT_EDITION_TITLE )

                    DIALOG_APPENDROOT_STRING( "scene name", lin_node.name );
                    DIALOG_APPENDROOT_FLOAT( "current speed", curr_speed );
                    DIALOG_APPENDROOT_FLOAT( "current theta", curr_theta );
                    DIALOG_APPENDROOT_FLOAT( "current phi", curr_phi );

                    DIALOG_APPLY
                    DIALOG_SHOW                    
                }
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
    else if( item.GetID() == m_keyboard_item.GetID() )
    {
        build_popupmenu( KEYBOARD_MASK, mnu );
    }
    else if( item.GetID() == m_mouse_item.GetID() )
    {
        build_popupmenu( MOUSE_MASK, mnu );
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
        else if( m_spacebox_nodes.count( item.GetID() ) > 0 )
        {
            build_popupmenu( SPACEBOX_MASK, mnu );
        }
        else if( m_camera_nodes.count( item.GetID() ) > 0 )
        {
            build_popupmenu( CAMERA_MASK, mnu );
        }
        else if( m_fps_nodes.count( item.GetID() ) > 0 )
        {
            build_popupmenu( FPSMOVEMENT_MASK, mnu );
        }
        else if( m_chunk_nodes.count( item.GetID() ) > 0 )
        {
            build_popupmenu( CHUNK_MASK, mnu );
        }
        else if( m_lin_nodes.count( item.GetID() ) > 0 )
        {
            build_popupmenu( LINMOVEMENT_MASK, mnu );
        }
        else if( m_free_nodes.count( item.GetID() ) > 0 )
        {
            build_popupmenu( FREEMOVEMENT_MASK, mnu );
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
    wxFileDialog openFileDialog( this, "Open lua script", "", "", "lua files (*.lua)|*.lua", wxFD_OPEN | wxFD_FILE_MUST_EXIST );
    if( wxID_CANCEL == openFileDialog.ShowModal() )
    {
        return;
    }
    wxString path = openFileDialog.GetPath();
    m_consoleinput_textCtrl->LoadFile( path );
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

void* BasicSceneMainFrame::find_scenenodegraph_id( wxTreeItemId p_item )
{
    // now we must found the scenenodegraph we belong to make the RegisterNode() call
    wxTreeItemId current = p_item;

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

    return id;
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
            entry.current_camera_set = false;

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

        transfo_node->RegisterUpdateBeginEvtHandler( m_nodeupdatebegin_cb );

        /////////////////////////////////////////////////////////////////////////////////

        // now we must found the scenenodegraph we belong to make the RegisterNode() call
        void* id = find_scenenodegraph_id( p_dialog->GetTreeItem() );

        BasicSceneMainFrame::SceneNodeGraphEntry entry;

        entry = m_scenenodegraphs[id];
        entry.scenenodegraph->RegisterNode( transfo_node );

        ///////////////////////////////////////////////////////////////////////////////////////////////

        // link to the scenegraph hierarchy

        wxTreeItemId current;
        current = p_dialog->GetTreeItem(); //m_last_clicked_treeitem;
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

        wxTreeItemId treeitemid = m_scenegraphs_treeCtrl->AppendItem( /*m_last_clicked_treeitem*/ p_dialog->GetTreeItem(), alias2, TRANSFO_ICON_INDEX );
        m_scenegraphs_treeCtrl->ExpandAllChildren( /*m_last_clicked_treeitem*/ p_dialog->GetTreeItem() );


        /////////////////////////////////////////////////////////////////////////////////

        // record the new transformation node and associated metadata

        BasicSceneMainFrame::SceneNodeEntry<Transformation> t_entry;

        t_entry.name = alias;
        t_entry.scene_node = transfo_node;
        t_entry.treeitemid = treeitemid;


        m_transformation_nodes[t_entry.treeitemid.GetID()] = t_entry;

        m_tree_nodes[t_entry.treeitemid.GetID()] = transfo_node;
        m_inv_tree_nodes[transfo_node] = t_entry.treeitemid.GetID();

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


        DIALOG_EXPLORE_NODES_END( i )

        if( ok )
        {
            Transformation* tdet = m_transformation_nodes[/*m_last_clicked_treeitem*/p_dialog->GetTreeItem().GetID()].scene_node->GetContent();
            //(*tdet) = new_chain;

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

    else if( DIALOG_SPACEBOX_CREATION_TITLE == DIALOG_TITLE )
    {
        DrawSpace::Utils::SpaceboxDescriptor descr;
        
        DIALOG_GET_STRING_PROPERTY( "scene name", alias2 )

        DIALOG_WXSTRING_TO_DSSTRING( alias2, alias )

        if( "" == alias )
        {
            wxMessageBox( "'scene name' attribute cannot be void", "DrawFront error", wxICON_ERROR );
            return;
        }

        descr.scene_name = alias;

        {

            DIALOG_EXPLORE_NODES_BEGIN( "", "pass %d", i, pass_slot )

                DrawSpace::Utils::SpaceboxPassDescriptor pass_descr;

                DIALOG_GET_ENUM_PROPERTY( DIALOG_INCREMENT_STRING( pass_slot, "pass" ), pass_name )
                DIALOG_WXSTRING_TO_DSSTRING( pass_name, pass_name2 )

                DIALOG_GET_ENUM_PROPERTY( DIALOG_INCREMENT_STRING( pass_slot, "fx" ), fx_name )
                DIALOG_GET_INT_PROPERTY( DIALOG_INCREMENT_STRING( pass_slot, "rendering order" ), rendering_order )

                dsstring texture_root_name;

                {
                    texture_root_name = DIALOG_INCREMENT_STRING( pass_slot, "textures.front" );
                    DIALOG_EXPLORE_NODES_BEGIN( texture_root_name, "stage %d", j, texture_stage )
                        DIALOG_GET_ENUM_PROPERTY( texture_stage, texture_name )
                        DIALOG_WXSTRING_TO_DSSTRING( texture_name, texture_name2 )

                        if( texture_name2 != "..." )
                        {
                            pass_descr.textures[Spacebox::FrontQuad][j] = texture_name2;
                        }
                        else
                        {
                            pass_descr.textures[Spacebox::FrontQuad][j] = "";
                        }
                    DIALOG_EXPLORE_NODES_END( j )
                }

                {
                    texture_root_name = DIALOG_INCREMENT_STRING( pass_slot, "textures.rear" );
                    DIALOG_EXPLORE_NODES_BEGIN( texture_root_name, "stage %d", j, texture_stage )
                        DIALOG_GET_ENUM_PROPERTY( texture_stage, texture_name )
                        DIALOG_WXSTRING_TO_DSSTRING( texture_name, texture_name2 )

                        if( texture_name2 != "..." )
                        {
                            pass_descr.textures[Spacebox::RearQuad][j] = texture_name2;
                        }
                        else
                        {
                            pass_descr.textures[Spacebox::RearQuad][j] = "";
                        }
                    DIALOG_EXPLORE_NODES_END( j )
                }

                {
                    texture_root_name = DIALOG_INCREMENT_STRING( pass_slot, "textures.left" );
                    DIALOG_EXPLORE_NODES_BEGIN( texture_root_name, "stage %d", j, texture_stage )
                        DIALOG_GET_ENUM_PROPERTY( texture_stage, texture_name )
                        DIALOG_WXSTRING_TO_DSSTRING( texture_name, texture_name2 )

                        if( texture_name2 != "..." )
                        {
                            pass_descr.textures[Spacebox::LeftQuad][j] = texture_name2;
                        }
                        else
                        {
                            pass_descr.textures[Spacebox::LeftQuad][j] = "";
                        }
                    DIALOG_EXPLORE_NODES_END( j )
                }

                {
                    texture_root_name = DIALOG_INCREMENT_STRING( pass_slot, "textures.right" );
                    DIALOG_EXPLORE_NODES_BEGIN( texture_root_name, "stage %d", j, texture_stage )
                        DIALOG_GET_ENUM_PROPERTY( texture_stage, texture_name )
                        DIALOG_WXSTRING_TO_DSSTRING( texture_name, texture_name2 )

                        if( texture_name2 != "..." )
                        {
                            pass_descr.textures[Spacebox::RightQuad][j] = texture_name2;
                        }
                        else
                        {
                            pass_descr.textures[Spacebox::RightQuad][j] = "";
                        }
                    DIALOG_EXPLORE_NODES_END( j )
                }

                {
                    texture_root_name = DIALOG_INCREMENT_STRING( pass_slot, "textures.top" );
                    DIALOG_EXPLORE_NODES_BEGIN( texture_root_name, "stage %d", j, texture_stage )
                        DIALOG_GET_ENUM_PROPERTY( texture_stage, texture_name )
                        DIALOG_WXSTRING_TO_DSSTRING( texture_name, texture_name2 )

                        if( texture_name2 != "..." )
                        {
                            pass_descr.textures[Spacebox::TopQuad][j] = texture_name2;
                        }
                        else
                        {
                            pass_descr.textures[Spacebox::TopQuad][j] = "";
                        }
                    DIALOG_EXPLORE_NODES_END( j )
                }

                {
                    texture_root_name = DIALOG_INCREMENT_STRING( pass_slot, "textures.bottom" );
                    DIALOG_EXPLORE_NODES_BEGIN( texture_root_name, "stage %d", j, texture_stage )
                        DIALOG_GET_ENUM_PROPERTY( texture_stage, texture_name )
                        DIALOG_WXSTRING_TO_DSSTRING( texture_name, texture_name2 )

                        if( texture_name2 != "..." )
                        {
                            pass_descr.textures[Spacebox::BottomQuad][j] = texture_name2;
                        }
                        else
                        {
                            pass_descr.textures[Spacebox::BottomQuad][j] = "";
                        }
                    DIALOG_EXPLORE_NODES_END( j )
                }

                pass_descr.fx_name = fx_name;
                pass_descr.rendering_order = rendering_order;          
                descr.passes_slots[pass_name2] = pass_descr;

            DIALOG_EXPLORE_NODES_END( i )

        }

        {
            DIALOG_EXPLORE_NODES_BEGIN( "", "shader params %d", i, sp_slot )

                DIALOG_GET_ENUM_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "pass" ), pass_name )
                DIALOG_WXSTRING_TO_DSSTRING( pass_name, pass_name2 )

                DIALOG_GET_INT_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "shader index" ), shader_index )
                DIALOG_GET_STRING_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "param id" ), param_id )
                DIALOG_GET_INT_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "register" ), shader_register )

                DIALOG_GET_FLOAT_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "values.x" ), val_x )
                DIALOG_GET_FLOAT_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "values.y" ), val_y )
                DIALOG_GET_FLOAT_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "values.z" ), val_z )
                DIALOG_GET_FLOAT_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "values.w" ), val_w )

                if( descr.passes_slots.count( pass_name2 ) > 0 )
                {
                    PassShaderParam psp;

                    psp.id = param_id;
                    psp.shader_index = shader_index;
                    psp.shader_register = shader_register;
                    psp.value[0] = val_x;
                    psp.value[1] = val_y;
                    psp.value[2] = val_z;
                    psp.value[3] = val_w;

                    descr.passes_slots[pass_name2].shader_params.push_back( psp );
                }


            DIALOG_EXPLORE_NODES_END( i )

        }

        dsstring sb_error;
        Spacebox* sb = BuildSpaceBox( descr, sb_error );

        if( sb )
        {
            SceneNode<Spacebox>* sb_node = new SceneNode<Spacebox>( alias );
            sb_node->SetContent( sb );
            sb_node->RegisterUpdateBeginEvtHandler( m_nodeupdatebegin_cb );
            
            // now we must found the scenenodegraph we belong to make the RegisterNode() call
            void* id = find_scenenodegraph_id(  p_dialog->GetTreeItem() );

            BasicSceneMainFrame::SceneNodeGraphEntry entry;

            entry = m_scenenodegraphs[id];
            entry.scenenodegraph->RegisterNode( sb_node );

            // link to the scenegraph hierarchy

            wxTreeItemId current;
            current = p_dialog->GetTreeItem(); //m_last_clicked_treeitem;
            id = current.GetID();

            if( m_scenenodegraphs.count( id ) > 0 )
            {
                // parent is a scenegraph : use SceneNodeGraph::Add() method
                entry.scenenodegraph->AddNode( sb_node );
            }
            else
            {
                BaseSceneNode* parent_node = m_tree_nodes[id];
                sb_node->LinkTo( parent_node );
            }

            // GUI : add item in the tree

            wxTreeItemId treeitemid = m_scenegraphs_treeCtrl->AppendItem( /*m_last_clicked_treeitem*/ p_dialog->GetTreeItem(), alias2, SPACEBOX_ICON_INDEX );
            m_scenegraphs_treeCtrl->ExpandAllChildren( /*m_last_clicked_treeitem*/ p_dialog->GetTreeItem() );

            // record the new spacebox node and associated metadata

            BasicSceneMainFrame::SceneNodeEntry<Spacebox> t_entry;

            t_entry.name = alias;
            t_entry.scene_node = sb_node;
            t_entry.treeitemid = treeitemid;


            m_spacebox_nodes[t_entry.treeitemid.GetID()] = t_entry;

            m_tree_nodes[t_entry.treeitemid.GetID()] = sb_node;
            m_inv_tree_nodes[sb_node] = t_entry.treeitemid.GetID();

            // update passes output queues
            for( std::map<dsstring, SpaceboxPassDescriptor>::iterator it = descr.passes_slots.begin(); it != descr.passes_slots.end(); ++it )
            {
                Pass* current_pass = dynamic_cast<Pass*>( ConfigsBase::GetInstance()->GetConfigurableInstance( it->first ) );
                current_pass->GetRenderingQueue()->UpdateOutputQueue();
            }

            // store spacebox description

            m_spacebox_descriptors[t_entry.treeitemid.GetID()] = descr;
            
            DIALOG_CLOSE
        }
        else
        {
            wxMessageBox( wxString( "spacebox creation failure : " ) + wxString( sb_error.c_str() ) , "DrawFront error", wxICON_ERROR );
        }

    }

    else if( DIALOG_CHUNK_CREATION_TITLE == DIALOG_TITLE )
    {
        DrawSpace::Utils::ChunkDescriptor descr;
        
        DIALOG_GET_STRING_PROPERTY( "scene name", alias2 )
        DIALOG_WXSTRING_TO_DSSTRING( alias2, alias )

        if( "" == alias )
        {
            wxMessageBox( "'scene name' attribute cannot be void", "DrawFront error", wxICON_ERROR );
            return;
        }

        descr.scene_name = alias;

        DIALOG_GET_ENUM_PROPERTY( "meshe", meshe_name )
        DIALOG_WXSTRING_TO_DSSTRING( meshe_name, meshe_name2 )

        descr.meshe = meshe_name2;

        {
            DIALOG_EXPLORE_NODES_BEGIN( "", "pass %d", i, pass_slot )

                DrawSpace::Utils::ChunkPassDescriptor pass_descr;

                DIALOG_GET_ENUM_PROPERTY( DIALOG_INCREMENT_STRING( pass_slot, "pass" ), pass_name )
                DIALOG_WXSTRING_TO_DSSTRING( pass_name, pass_name2 )

                DIALOG_GET_ENUM_PROPERTY( DIALOG_INCREMENT_STRING( pass_slot, "fx" ), fx_name )
                DIALOG_GET_INT_PROPERTY( DIALOG_INCREMENT_STRING( pass_slot, "rendering order" ), rendering_order )

                dsstring texture_root_name;
                texture_root_name = DIALOG_INCREMENT_STRING( pass_slot, "textures" );

                DIALOG_EXPLORE_NODES_BEGIN( texture_root_name, "stage %d", j, texture_stage )
                    DIALOG_GET_ENUM_PROPERTY( texture_stage, texture_name )
                    DIALOG_WXSTRING_TO_DSSTRING( texture_name, texture_name2 )

                    if( texture_name2 != "..." )
                    {
                        pass_descr.textures[j] = texture_name2;
                    }
                    else
                    {
                        pass_descr.textures[j] = "";
                    }
                DIALOG_EXPLORE_NODES_END( j )

                pass_descr.fx_name = fx_name;
                pass_descr.rendering_order = rendering_order;          
                descr.passes_slots[pass_name2] = pass_descr;

            DIALOG_EXPLORE_NODES_END( i )
        }
        
        {
            DIALOG_EXPLORE_NODES_BEGIN( "", "shader params %d", i, sp_slot )

                DIALOG_GET_ENUM_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "pass" ), pass_name )
                DIALOG_WXSTRING_TO_DSSTRING( pass_name, pass_name2 )

                DIALOG_GET_INT_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "shader index" ), shader_index )
                DIALOG_GET_STRING_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "param id" ), param_id )
                DIALOG_GET_INT_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "register" ), shader_register )

                DIALOG_GET_FLOAT_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "values.x" ), val_x )
                DIALOG_GET_FLOAT_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "values.y" ), val_y )
                DIALOG_GET_FLOAT_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "values.z" ), val_z )
                DIALOG_GET_FLOAT_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "values.w" ), val_w )

                if( descr.passes_slots.count( pass_name2 ) > 0 )
                {
                    PassShaderParam psp;

                    psp.id = param_id;
                    psp.shader_index = shader_index;
                    psp.shader_register = shader_register;
                    psp.value[0] = val_x;
                    psp.value[1] = val_y;
                    psp.value[2] = val_z;
                    psp.value[3] = val_w;

                    descr.passes_slots[pass_name2].shader_params.push_back( psp );
                }

            DIALOG_EXPLORE_NODES_END( i )
        }

        dsstring chunk_error;
        Chunk* chunk = BuildChunk( descr, chunk_error );

        if( chunk )
        {
            SceneNode<Chunk>* chunk_node = new SceneNode<Chunk>( alias );
            chunk_node->SetContent( chunk );
            chunk_node->RegisterUpdateBeginEvtHandler( m_nodeupdatebegin_cb );

            // now we must found the scenenodegraph we belong to make the RegisterNode() call
            void* id = find_scenenodegraph_id(  p_dialog->GetTreeItem() );

            BasicSceneMainFrame::SceneNodeGraphEntry entry;

            entry = m_scenenodegraphs[id];
            entry.scenenodegraph->RegisterNode( chunk_node );

            // link to the scenegraph hierarchy

            wxTreeItemId current;
            current = p_dialog->GetTreeItem(); //m_last_clicked_treeitem;
            id = current.GetID();

            if( m_scenenodegraphs.count( id ) > 0 )
            {
                // parent is a scenegraph : use SceneNodeGraph::Add() method
                entry.scenenodegraph->AddNode( chunk_node );
            }
            else
            {
                BaseSceneNode* parent_node = m_tree_nodes[id];
                chunk_node->LinkTo( parent_node );
            }

            // GUI : add item in the tree

            wxTreeItemId treeitemid = m_scenegraphs_treeCtrl->AppendItem( /*m_last_clicked_treeitem*/ p_dialog->GetTreeItem(), alias2, CHUNK_ICON_INDEX );
            m_scenegraphs_treeCtrl->ExpandAllChildren( /*m_last_clicked_treeitem*/ p_dialog->GetTreeItem() );

            // record the new chunk node and associated metadata

            BasicSceneMainFrame::SceneNodeEntry<Chunk> c_entry;

            c_entry.name = alias;
            c_entry.scene_node = chunk_node;
            c_entry.treeitemid = treeitemid;

            m_chunk_nodes[c_entry.treeitemid.GetID()] = c_entry;

            m_tree_nodes[c_entry.treeitemid.GetID()] = chunk_node;
            m_inv_tree_nodes[chunk_node] = c_entry.treeitemid.GetID();

            // update passes output queues
            for( std::map<dsstring, ChunkPassDescriptor>::iterator it = descr.passes_slots.begin(); it != descr.passes_slots.end(); ++it )
            {
                Pass* current_pass = dynamic_cast<Pass*>( ConfigsBase::GetInstance()->GetConfigurableInstance( it->first ) );
                current_pass->GetRenderingQueue()->UpdateOutputQueue();
            }

            // store chunk description

            m_chunk_descriptors[c_entry.treeitemid.GetID()] = descr;
            
            DIALOG_CLOSE

        }
        else
        {
            wxMessageBox( wxString( "chunk creation failure : " ) + wxString( chunk_error.c_str() ) , "DrawFront error", wxICON_ERROR );
        }
        
    }

    else if( DIALOG_SPACEBOX_EDITION_TITLE == DIALOG_TITLE )
    {

        DrawSpace::Utils::SpaceboxDescriptor sb_descr = m_spacebox_descriptors[/*m_last_clicked_treeitem*/ p_dialog->GetTreeItem().GetID()];
        SceneNodeEntry<DrawSpace::Spacebox> sne = m_spacebox_nodes[/*m_last_clicked_treeitem*/ p_dialog->GetTreeItem().GetID()];

        DIALOG_EXPLORE_NODES_BEGIN( "", "shader parameter %d", i, sp_slot )

            DIALOG_GET_STRING_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "pass name" ), pass_name )
            DIALOG_GET_STRING_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "param id" ), param_id )
            
            DIALOG_GET_FLOAT_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "values.x" ), val_x )
            DIALOG_GET_FLOAT_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "values.y" ), val_y )
            DIALOG_GET_FLOAT_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "values.z" ), val_z )
            DIALOG_GET_FLOAT_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "values.w" ), val_w )

           

            DIALOG_WXSTRING_TO_DSSTRING( pass_name, pass_name2 )
            DIALOG_WXSTRING_TO_DSSTRING( param_id, param_id2 )

            Pass* current_pass = dynamic_cast<Pass*>( ConfigsBase::GetInstance()->GetConfigurableInstance( pass_name2 ) );

            for( int j = 0; j < 6; j++ )
            {
                sne.scene_node->GetContent()->GetNodeFromPass( current_pass, j )->SetShaderRealVector( param_id2, Vector( val_x, val_y, val_z, val_w ) );
            }

            // update descriptor
            sb_descr.passes_slots[pass_name2].shader_params[i].value = Vector( val_x, val_y, val_z, val_w );

            m_spacebox_descriptors[/*m_last_clicked_treeitem*/ p_dialog->GetTreeItem().GetID()] = sb_descr;

        DIALOG_EXPLORE_NODES_END( i )
        
        DIALOG_CLOSE
    }

    else if( DIALOG_CHUNK_EDITION_TITLE == DIALOG_TITLE )
    {

        DrawSpace::Utils::ChunkDescriptor chunk_descr = m_chunk_descriptors[/*m_last_clicked_treeitem*/ p_dialog->GetTreeItem().GetID()];
        SceneNodeEntry<DrawSpace::Chunk> cne = m_chunk_nodes[/*m_last_clicked_treeitem*/ p_dialog->GetTreeItem().GetID()];

        DIALOG_EXPLORE_NODES_BEGIN( "", "shader parameter %d", i, sp_slot )

            DIALOG_GET_STRING_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "pass name" ), pass_name )
            DIALOG_GET_STRING_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "param id" ), param_id )
            
            DIALOG_GET_FLOAT_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "values.x" ), val_x )
            DIALOG_GET_FLOAT_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "values.y" ), val_y )
            DIALOG_GET_FLOAT_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "values.z" ), val_z )
            DIALOG_GET_FLOAT_PROPERTY( DIALOG_INCREMENT_STRING( sp_slot, "values.w" ), val_w )

           

            DIALOG_WXSTRING_TO_DSSTRING( pass_name, pass_name2 )
            DIALOG_WXSTRING_TO_DSSTRING( param_id, param_id2 )

            Pass* current_pass = dynamic_cast<Pass*>( ConfigsBase::GetInstance()->GetConfigurableInstance( pass_name2 ) );

            cne.scene_node->GetContent()->GetNodeFromPass( current_pass )->SetShaderRealVector( param_id2, Vector( val_x, val_y, val_z, val_w ) );

            // update descriptor
            chunk_descr.passes_slots[pass_name2].shader_params[i].value = Vector( val_x, val_y, val_z, val_w );

            m_chunk_descriptors[/*m_last_clicked_treeitem*/ p_dialog->GetTreeItem().GetID()] = chunk_descr;

        DIALOG_EXPLORE_NODES_END( i )
        
        DIALOG_CLOSE
    }


    else if( DIALOG_CAMERA_CREATION_TITLE == DIALOG_TITLE )
    {
        DIALOG_GET_STRING_PROPERTY( "scene name", alias2 )

        DIALOG_WXSTRING_TO_DSSTRING( alias2, alias )

        if( "" == alias )
        {
            wxMessageBox( "'scene name' attribute cannot be void", "DrawFront error", wxICON_ERROR );
            return;
        }

        /////////////////////////////////////////////////////////////////////////////////

        // create the camera node

        SceneNode<CameraPoint>* camera_node;
        camera_node = new SceneNode<CameraPoint>( alias );
        camera_node->SetContent( new CameraPoint );

        camera_node->RegisterUpdateBeginEvtHandler( m_nodeupdatebegin_cb );

        /////////////////////////////////////////////////////////////////////////////////

        // now we must found the scenenodegraph we belong to make the RegisterNode() call
        void* id = find_scenenodegraph_id(  p_dialog->GetTreeItem() );

        BasicSceneMainFrame::SceneNodeGraphEntry entry;

        entry = m_scenenodegraphs[id];
        entry.scenenodegraph->RegisterNode( camera_node );

        ///////////////////////////////////////////////////////////////////////////////////////////////

        // link to the scenegraph hierarchy

        wxTreeItemId current;
        current = p_dialog->GetTreeItem(); //m_last_clicked_treeitem;
        id = current.GetID();

        if( m_scenenodegraphs.count( id ) > 0 )
        {
            // parent is a scenegraph : use SceneNodeGraph::Add() method
            entry.scenenodegraph->AddNode( camera_node );
        }
        else
        {
            BaseSceneNode* parent_node = m_tree_nodes[id];
            camera_node->LinkTo( parent_node );
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////

        // GUI : add item in the tree

        wxTreeItemId treeitemid = m_scenegraphs_treeCtrl->AppendItem( /*m_last_clicked_treeitem*/ p_dialog->GetTreeItem(), alias2, CAMERA_ICON_INDEX );
        m_scenegraphs_treeCtrl->ExpandAllChildren( /*m_last_clicked_treeitem*/ p_dialog->GetTreeItem() );
       
        /////////////////////////////////////////////////////////////////////////////////

        // record the new transformation node and associated metadata

        BasicSceneMainFrame::SceneNodeEntry<CameraPoint> c_entry;

        c_entry.name = alias;
        c_entry.scene_node = camera_node;
        c_entry.treeitemid = treeitemid;


        m_camera_nodes[c_entry.treeitemid.GetID()] = c_entry;

        m_tree_nodes[c_entry.treeitemid.GetID()] = camera_node;
        m_inv_tree_nodes[camera_node] = c_entry.treeitemid.GetID();

        DIALOG_CLOSE
    }

    else if( DIALOG_CAMERA_EDIT_TITLE == DIALOG_TITLE )
    {
        DIALOG_GET_FLOAT_PROPERTY( "znear", znear );
        SceneNodeEntry<DrawSpace::Dynamics::CameraPoint> camera_node = m_camera_nodes[/*m_last_clicked_treeitem*/ p_dialog->GetTreeItem().GetID()];
        camera_node.scene_node->GetContent()->UpdateProjectionZNear( znear );
        //DIALOG_CLOSE
    }
    else if( DIALOG_FPSMVT_CREATION_TITLE == DIALOG_TITLE )
    {
        DIALOG_GET_STRING_PROPERTY( "scene name", alias2 )

        DIALOG_WXSTRING_TO_DSSTRING( alias2, alias )

        if( "" == alias )
        {
            wxMessageBox( "'scene name' attribute cannot be void", "DrawFront error", wxICON_ERROR );
            return;
        }

        /////////////////////////////////////////////////////////////////////////////////


        DIALOG_GET_FLOAT_PROPERTY( "initial theta", init_theta );
        DIALOG_GET_FLOAT_PROPERTY( "initial phi", init_phi );

        DIALOG_GET_FLOAT_PROPERTY( "initial position.x", x );
        DIALOG_GET_FLOAT_PROPERTY( "initial position.y", y );
        DIALOG_GET_FLOAT_PROPERTY( "initial position.z", z );
        DIALOG_GET_BOOL_PROPERTY( "y movement", ymvt );

        /////////////////////////////////////////////////////////////////////////////////

        // create the fps mvt node

        SceneNode<FPSMovement>* fps_node;
        fps_node = new SceneNode<FPSMovement>( alias );
        fps_node->SetContent( new FPSMovement( ymvt ) );

        fps_node->RegisterUpdateBeginEvtHandler( m_nodeupdatebegin_cb );

        fps_node->GetContent()->Init( Vector( x, y, z, 1 ), Maths::DegToRad( init_theta ), Maths::DegToRad( init_phi ) );

        /////////////////////////////////////////////////////////////////////////////////

        // now we must found the scenenodegraph we belong to make the RegisterNode() call
        void* id = find_scenenodegraph_id(  p_dialog->GetTreeItem() );

        BasicSceneMainFrame::SceneNodeGraphEntry entry;

        entry = m_scenenodegraphs[id];
        entry.scenenodegraph->RegisterNode( fps_node );

        ///////////////////////////////////////////////////////////////////////////////////////////////

        // link to the scenegraph hierarchy

        wxTreeItemId current;
        current = p_dialog->GetTreeItem(); //m_last_clicked_treeitem;
        id = current.GetID();

        if( m_scenenodegraphs.count( id ) > 0 )
        {
            // parent is a scenegraph : use SceneNodeGraph::Add() method
            entry.scenenodegraph->AddNode( fps_node );
        }
        else
        {
            BaseSceneNode* parent_node = m_tree_nodes[id];
            fps_node->LinkTo( parent_node );
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////

        // GUI : add item in the tree

        wxTreeItemId treeitemid = m_scenegraphs_treeCtrl->AppendItem( /*m_last_clicked_treeitem*/ p_dialog->GetTreeItem(), alias2, MOVEMENT_ICON_INDEX );
        m_scenegraphs_treeCtrl->ExpandAllChildren( /*m_last_clicked_treeitem*/ p_dialog->GetTreeItem() );
       
        /////////////////////////////////////////////////////////////////////////////////

        // record the new node and associated metadata

        BasicSceneMainFrame::SceneNodeEntry<FPSMovement> f_entry;

        f_entry.name = alias;
        f_entry.scene_node = fps_node;
        f_entry.treeitemid = treeitemid;


        m_fps_nodes[f_entry.treeitemid.GetID()] = f_entry;

        m_tree_nodes[f_entry.treeitemid.GetID()] = fps_node;
        m_inv_tree_nodes[fps_node] = f_entry.treeitemid.GetID();

        DIALOG_CLOSE

    }

    else if( DIALOG_LINMVT_CREATION_TITLE == DIALOG_TITLE )
    {
        DIALOG_GET_STRING_PROPERTY( "scene name", alias2 )

        DIALOG_WXSTRING_TO_DSSTRING( alias2, alias )

        if( "" == alias )
        {
            wxMessageBox( "'scene name' attribute cannot be void", "DrawFront error", wxICON_ERROR );
            return;
        }

        /////////////////////////////////////////////////////////////////////////////////

        DIALOG_GET_FLOAT_PROPERTY( "initial theta", init_theta );
        DIALOG_GET_FLOAT_PROPERTY( "initial phi", init_phi );

        DIALOG_GET_FLOAT_PROPERTY( "initial position.x", x );
        DIALOG_GET_FLOAT_PROPERTY( "initial position.y", y );
        DIALOG_GET_FLOAT_PROPERTY( "initial position.z", z );

        DIALOG_GET_FLOAT_PROPERTY( "direction.x", xdir );
        DIALOG_GET_FLOAT_PROPERTY( "direction.y", ydir );
        DIALOG_GET_FLOAT_PROPERTY( "direction.z", zdir );

        /////////////////////////////////////////////////////////////////////////////////

        // create the linear mvt node

        SceneNode<LinearMovement>* lin_node;
        lin_node = new SceneNode<LinearMovement>( alias );
        lin_node->SetContent( new LinearMovement() );

        lin_node->RegisterUpdateBeginEvtHandler( m_nodeupdatebegin_cb );
        lin_node->GetContent()->Init( Vector( x, y, z, 1 ), Vector( xdir, ydir, zdir, 1 ), init_theta, init_phi );

        /////////////////////////////////////////////////////////////////////////////////

        // now we must found the scenenodegraph we belong to make the RegisterNode() call
        void* id = find_scenenodegraph_id(  p_dialog->GetTreeItem() );

        BasicSceneMainFrame::SceneNodeGraphEntry entry;

        entry = m_scenenodegraphs[id];
        entry.scenenodegraph->RegisterNode( lin_node );

        /////////////////////////////////////////////////////////////////////////////////

        // link to the scenegraph hierarchy

        wxTreeItemId current;
        current = p_dialog->GetTreeItem();
        id = current.GetID();

        if( m_scenenodegraphs.count( id ) > 0 )
        {
            // parent is a scenegraph : use SceneNodeGraph::Add() method
            entry.scenenodegraph->AddNode( lin_node );
        }
        else
        {
            BaseSceneNode* parent_node = m_tree_nodes[id];
            lin_node->LinkTo( parent_node );
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////

        // GUI : add item in the tree

        wxTreeItemId treeitemid = m_scenegraphs_treeCtrl->AppendItem( p_dialog->GetTreeItem(), alias2, MOVEMENT_ICON_INDEX );
        m_scenegraphs_treeCtrl->ExpandAllChildren( p_dialog->GetTreeItem() );
       
        /////////////////////////////////////////////////////////////////////////////////

        // record the new node and associated metadata

        BasicSceneMainFrame::SceneNodeEntry<LinearMovement> l_entry;

        l_entry.name = alias;
        l_entry.scene_node = lin_node;
        l_entry.treeitemid = treeitemid;

        m_lin_nodes[l_entry.treeitemid.GetID()] = l_entry;

        m_tree_nodes[l_entry.treeitemid.GetID()] = lin_node;
        m_inv_tree_nodes[lin_node] = l_entry.treeitemid.GetID();

        DIALOG_CLOSE
    }
    else if( DIALOG_LINMVT_EDITION_TITLE == DIALOG_TITLE )
    {
        DIALOG_GET_FLOAT_PROPERTY( "current theta", curr_theta );
        DIALOG_GET_FLOAT_PROPERTY( "current phi", curr_phi );
        DIALOG_GET_FLOAT_PROPERTY( "current speed", curr_speed );
        SceneNodeEntry<DrawSpace::Core::LinearMovement> lin_node = m_lin_nodes[p_dialog->GetTreeItem().GetID()];
        lin_node.scene_node->GetContent()->SetSpeed( curr_speed );
        lin_node.scene_node->GetContent()->SetTheta( curr_theta );
        lin_node.scene_node->GetContent()->SetPhi( curr_phi );
    }

    else if( DIALOG_FREEMVT_CREATION_TITLE == DIALOG_TITLE )
    {
        DIALOG_GET_STRING_PROPERTY( "scene name", alias2 )

        DIALOG_WXSTRING_TO_DSSTRING( alias2, alias )

        if( "" == alias )
        {
            wxMessageBox( "'scene name' attribute cannot be void", "DrawFront error", wxICON_ERROR );
            return;
        }

        /////////////////////////////////////////////////////////////////////////////////

        DIALOG_GET_FLOAT_PROPERTY( "initial position.x", x );
        DIALOG_GET_FLOAT_PROPERTY( "initial position.y", y );
        DIALOG_GET_FLOAT_PROPERTY( "initial position.z", z );


        /////////////////////////////////////////////////////////////////////////////////

        // create the free mvt node

        SceneNode<FreeMovement>* free_node;
        free_node = new SceneNode<FreeMovement>( alias );
        free_node->SetContent( new FreeMovement() );

        free_node->RegisterUpdateBeginEvtHandler( m_nodeupdatebegin_cb );
        free_node->GetContent()->Init( Vector( x, y, z, 1 ) );

        /////////////////////////////////////////////////////////////////////////////////

        // now we must found the scenenodegraph we belong to make the RegisterNode() call
        void* id = find_scenenodegraph_id(  p_dialog->GetTreeItem() );

        BasicSceneMainFrame::SceneNodeGraphEntry entry;

        entry = m_scenenodegraphs[id];
        entry.scenenodegraph->RegisterNode( free_node );

        /////////////////////////////////////////////////////////////////////////////////

        // link to the scenegraph hierarchy

        wxTreeItemId current;
        current = p_dialog->GetTreeItem();
        id = current.GetID();

        if( m_scenenodegraphs.count( id ) > 0 )
        {
            // parent is a scenegraph : use SceneNodeGraph::Add() method
            entry.scenenodegraph->AddNode( free_node );
        }
        else
        {
            BaseSceneNode* parent_node = m_tree_nodes[id];
            free_node->LinkTo( parent_node );
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////

        // GUI : add item in the tree

        wxTreeItemId treeitemid = m_scenegraphs_treeCtrl->AppendItem( p_dialog->GetTreeItem(), alias2, MOVEMENT_ICON_INDEX );
        m_scenegraphs_treeCtrl->ExpandAllChildren( p_dialog->GetTreeItem() );
       
        /////////////////////////////////////////////////////////////////////////////////

        // record the new node and associated metadata

        BasicSceneMainFrame::SceneNodeEntry<FreeMovement> f_entry;

        f_entry.name = alias;
        f_entry.scene_node = free_node;
        f_entry.treeitemid = treeitemid;

        m_free_nodes[f_entry.treeitemid.GetID()] = f_entry;

        m_tree_nodes[f_entry.treeitemid.GetID()] = free_node;
        m_inv_tree_nodes[free_node] = f_entry.treeitemid.GetID();

        DIALOG_CLOSE

    }
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

        DIALOG_APPENDNODE_NODE( textures_label, "top", top_label )
        DIALOG_BUILD_LABELS( RenderingNode::NbMaxTextures, "stage %d", top_textures_stages_labels )
        DIALOG_APPENDNODE_ITERATE( top_label, insert_void_choice( get_textures_list() ), DIALOG_APPENDNODE_ENUM, top_textures_stages_labels )

        DIALOG_APPENDNODE_NODE( textures_label, "bottom", bottom_label )
        DIALOG_BUILD_LABELS( RenderingNode::NbMaxTextures, "stage %d", bottom_textures_stages_labels )
        DIALOG_APPENDNODE_ITERATE( bottom_label, insert_void_choice( get_textures_list() ), DIALOG_APPENDNODE_ENUM, bottom_textures_stages_labels )


        DIALOG_FINALIZE
    }

    else if( DIALOG_CHUNK_CREATION_TITLE == DIALOG_TITLE )
    {
        DIALOG_SPECIFIC0_LABEL( "pass %d", pass_label )

        DIALOG_APPENDROOT_NODE( pass_label, pass_root )

        DIALOG_APPENDNODE_ENUM( pass_root, "pass", get_intermediatepasses_list() )
        DIALOG_APPENDNODE_ENUM( pass_root, "fx", get_fx_list() )
        DIALOG_APPENDNODE_INTEGER( pass_root, "rendering order", 10000 )
      
        DIALOG_APPENDNODE_NODE( pass_root, "textures", textures_label )

        DIALOG_BUILD_LABELS( RenderingNode::NbMaxTextures, "stage %d", textures_stages_labels )
        DIALOG_APPENDNODE_ITERATE( textures_label, insert_void_choice( get_textures_list() ), DIALOG_APPENDNODE_ENUM, textures_stages_labels )

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
}

void BasicSceneMainFrame::on_specificbutton1_clicked( BasicSceneObjectPropertiesDialog* p_dialog )
{
    DIALOG_GETGRID

    if( DIALOG_TRANSFORM_EDITION_TITLE == DIALOG_TITLE )
    {
        DIALOG_CLEARGRID
        DIALOG_SETSPECIFIC0COUNTER( 0 )
    }

    else if( DIALOG_SPACEBOX_CREATION_TITLE == DIALOG_TITLE || DIALOG_CHUNK_CREATION_TITLE == DIALOG_TITLE )
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

void BasicSceneMainFrame::on_nodeupdatebegin( DrawSpace::Core::BaseSceneNode* p_node )
{
    if( m_inv_tree_nodes.count( p_node ) > 0 )
    {
        void* id = m_inv_tree_nodes[p_node];

        dsstring script;
        bool* script_enabled;

        if( m_transformation_nodes.count( id ) > 0 )
        {
            script = m_transformation_nodes[id].script;
            script_enabled = &m_transformation_nodes[id].script_enabled;
        }
        else if( m_spacebox_nodes.count( id ) > 0 )
        {
            script = m_spacebox_nodes[id].script;
            script_enabled = &m_spacebox_nodes[id].script_enabled;
        }
        else if( m_camera_nodes.count( id ) > 0 )
        {
            script = m_camera_nodes[id].script;
            script_enabled = &m_camera_nodes[id].script_enabled;
        }
        else if( m_fps_nodes.count( id ) > 0 )
        {
            script = m_fps_nodes[id].script;
            script_enabled = &m_fps_nodes[id].script_enabled;
        }
        else if( m_chunk_nodes.count( id ) > 0 )
        {
            script = m_chunk_nodes[id].script;
            script_enabled = &m_chunk_nodes[id].script_enabled;
        }
        else if( m_lin_nodes.count( id ) > 0 )
        {
            script = m_lin_nodes[id].script;
            script_enabled = &m_lin_nodes[id].script_enabled;
        }
        else if( m_free_nodes.count( id ) > 0 )
        {
            script = m_free_nodes[id].script;
            script_enabled = &m_free_nodes[id].script_enabled;
        }

        if( *script_enabled )
        {
            bool status = m_scripting->ExecChunk( script.c_str() );
            if( !status )
            {
                *script_enabled = false;
            }
        }
    }
}

wxTreeItemId BasicSceneMainFrame::searchTreeItemIdInNodes( void* p_id )
{
    if( m_transformation_nodes.count( p_id ) > 0 )
    {
        return m_transformation_nodes[p_id].treeitemid;
    }
    if( m_spacebox_nodes.count( p_id ) > 0 )
    {
        return m_spacebox_nodes[p_id].treeitemid;
    }
    if( m_camera_nodes.count( p_id ) > 0 )
    {
        return m_camera_nodes[p_id].treeitemid;
    }
    if( m_fps_nodes.count( p_id ) > 0 )
    {
        return m_fps_nodes[p_id].treeitemid;
    }
    if( m_chunk_nodes.count( p_id ) > 0 )
    {
        return m_chunk_nodes[p_id].treeitemid;
    }
    if( m_lin_nodes.count( p_id ) > 0 )
    {
        return m_lin_nodes[p_id].treeitemid;
    }
    if( m_free_nodes.count( p_id ) > 0 )
    {
        return m_free_nodes[p_id].treeitemid;
    }

}