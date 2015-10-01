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

#include "ActionPropsDialog.h"
#include "ActionEditMvtDialog.h"
#include "ActionAddMatrix.h"
#include "ActionAddShaderParam.h"

#include "ActionMouseLoadScript.h"
#include "ActionKeyDownLoadScript.h"
#include "ActionKeyUpLoadScript.h"

#include "ActionKeyboardKeyboard.h"
#include "ActionMouseMouse.h"

#include "ActionScenenodeGraphCreationDialog.h"
#include "ActionScenenodeGraphCreationApply.h"

#include "ActionWorldCreationDialog.h"
#include "ActionWorldCreationApply.h"

#include "ActionWorldEditionDialog.h"
#include "ActionWorldEditionApply.h"

#include "ActionSpaceBoxCreationDialog.h"
#include "ActionSpaceBoxCreationSpecific0.h"
#include "ActionSpaceBoxCreationApply.h"

#include "ActionSpaceBoxEditionDialog.h"
#include "ActionSpaceBoxEditionApply.h"

#include "ActionSpaceBoxNodeUpdateShaderParam.h"

#include "ActionSpaceBoxLinkTo.h"

#include "ActionChunkCreationDialog.h"
#include "ActionChunkCreationSpecific0.h"
#include "ActionChunkCreationSpecific2.h"
#include "ActionChunkCreationApply.h"

#include "ActionChunkEditionDialog.h"
#include "ActionChunkEditionApply.h"

#include "ActionChunkNodeUpdateShaderParam.h"

#include "ActionChunkLinkTo.h"

#include "ActionCloudsCreationDialog.h"
#include "ActionCloudsCreationApply.h"

#include "ActionTransformCreationDialog.h"
#include "ActionTransformCreationApply.h"

#include "ActionTransformEditionDialog.h"
#include "ActionTransformEditionApply.h"
#include "ActionTransformEditionSpecific1.h"

#include "ActionTransformationTransformation.h"
#include "ActionTransformationLinkTo.h"

#include "ActionCameraPointCreationDialog.h"
#include "ActionCameraPointCreationApply.h"

#include "ActionCameraPointSelection.h"

#include "ActionCameraPointEditionDialog.h"
#include "ActionCameraPointEditionApply.h"

#include "ActionCameraPointCameraPoint.h"
#include "ActionCameraPointLinkTo.h"

#include "ActionFPSMvtCreationDialog.h"
#include "ActionFPSMvtCreationApply.h"

#include "ActionFPSMvtFPSMvt.h"
#include "ActionFPSMvtLinkTo.h"


#include "ActionLinearMvtCreationDialog.h"
#include "ActionLinearMvtCreationApply.h"

#include "ActionLinearMvtLinearMvt.h"
#include "ActionLinearMvtLinkTo.h"

#include "ActionLinearMvtEditionApply.h"


#include "ActionFreeMvtCreationDialog.h"
#include "ActionFreeMvtCreationApply.h"

#include "ActionFreeMvtFreeMvt.h"
#include "ActionFreeMvtLinkTo.h"

#include "ActionCircularMvtCreationDialog.h"
#include "ActionCircularMvtCreationApply.h"

#include "ActionCircularMvtEditionApply.h"

#include "ActionCircularMvtCircularMvt.h"
#include "ActionCircularMvtLinkTo.h"


#include "ActionLongLatCreationDialog.h"
#include "ActionLongLatCreationApply.h"
#include "ActionLongLatLinkTo.h"
#include "ActionLongLatLongLat.h"

#include "ActionLongLatMvtEditionApply.h"

#include "ActionInertBodyCreationDialog.h"
#include "ActionInertBodyCreationApply.h"

#include "ActionInertBodyEditionDialog.h"
#include "ActionInertBodyEditionApply.h"

#include "ActionInertBodyInertBody.h"
#include "ActionInertBodyLinkTo.h"

#include "ActionColliderCreationDialog.h"
#include "ActionColliderCreationApply.h"

#include "ActionColliderCollider.h"
#include "ActionColliderLinkTo.h"

#include "ActionTextureCtor.h"

#include "ActionMouseMoveScriptEditionDialog.h"
#include "ActionKeyupScriptEditionDialog.h"
#include "ActionKeydownScriptEditionDialog.h"

#include "ActionDrawSpaceDrawSpace.h"
#include "ActionGetSceneCameraName.h"
#include "ActionIsCurrentCamera.h"
#include "ActionSetSceneNodeGraphCurrentCamera.h"
#include "ActionSetWorldGravity.h"
#include "ActionGlobalPrint.h"
#include "ActionDisplayFramerate.h"
#include "ActionDisplayCurrentCamera.h"
#include "ActionCreateSceneNodeGraph.h"
#include "ActionCreateWorld.h"
#include "ActionAngleSpeedInc.h"
#include "ActionAngleSpeedDec.h"
#include "ActionTranslationSpeedInc.h"
#include "ActionTranslationSpeedDec.h"
#include "ActionUpdatePassShaderParam.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::Utils;

//#define RENDER_IDLE

BasicSceneMainFrame* BasicSceneMainFrame::m_instance = NULL;

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

BasicSceneMainFrame::BasicSceneMainFrame( wxWindow* parent ) : MainFrame( parent, wxID_ANY, "DrawFront", wxDefaultPosition, wxSize( 1264,600 ), wxCAPTION | wxCLOSE_BOX ),
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
    //PopupMenuEntry pme_editshaders = { CONTEXTMENU_EDIT_SHADERSPARAMS, "Edit shaders params..." };

    PopupMenuEntry pme_editworld = { CONTEXTMENU_EDIT_WORLD, "Edit world..." };
    PopupMenuEntry pme_editsb = { CONTEXTMENU_EDIT_SBNODE, "Edit spacebox..." };
    PopupMenuEntry pme_editchunk = { CONTEXTMENU_EDIT_CHUNKNODE, "Edit chunk..." };
    PopupMenuEntry pme_editinertbody = { CONTEXTMENU_EDIT_INERTBODY, "Edit inert body..." };

    

    PopupMenuEntry pme_showprops = { CONTEXTMENU_SHOW_PROPS, "Show properties..." };
    //PopupMenuEntry pme_editnodescript = { CONTEXTMENU_EDIT_NODESCRIPT, "Edit script..." };

    PopupMenuEntry pme_editmousemovescript = { CONTEXTMENU_EDIT_MOUSEMOVESCRIPT, "Edit mouse movement script..." };
    PopupMenuEntry pme_editkeydownscript = { CONTEXTMENU_EDIT_KEYDOWNSCRIPT, "Edit key down script..." };
    PopupMenuEntry pme_editkeyupscript = { CONTEXTMENU_EDIT_KEYUPSCRIPT, "Edit key up script..." };

    PopupMenuEntry pme_selectcamera = { CONTEXTMENU_SELECT_CAMERA, "Select camera..." };
    PopupMenuEntry pme_editcamera = { CONTEXTMENU_EDIT_CAMERA, "Edit camera..." };

    PopupMenuEntry pme_editmvt = { CONTEXTMENU_EDIT_MVT, "Edit movement..." };

    PopupMenuEntry pme_newscenenodegraph = { CONTEXTMENU_NEWSCENENODEGRAPH, "New scenenodegraph..." };
    PopupMenuEntry pme_newworld = { CONTEXTMENU_NEWWORLD, "New world..." };
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
    PopupMenuEntry pme_newclouds = { CONTEXTMENU_NEWCLOUDS, "New clouds..." };


    ///////////////////////////////////////////////////////////////////

    m_scenegraphs_masks[DRAWSPACE_MASK].push_back( pme_newscenenodegraph );
    m_scenegraphs_masks[DRAWSPACE_MASK].push_back( pme_newworld );

    ///////////////////////////////////////////////////////////////////

    m_scenegraphs_masks[KEYBOARD_MASK].push_back( pme_editkeydownscript );
    m_scenegraphs_masks[KEYBOARD_MASK].push_back( pme_editkeyupscript );
    m_scenegraphs_masks[MOUSE_MASK].push_back( pme_editmousemovescript );

    ///////////////////////////////////////////////////////////////////

    m_scenegraphs_masks[WORLD_MASK].push_back( pme_editworld );

    ///////////////////////////////////////////////////////////////////

    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_newspacebox );
    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_newchunk );
    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_newclouds );

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
    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_newheadmvt );
    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_newlonglatmvt );

    m_scenegraphs_masks[SCENEGRAPH_MASK].push_back( pme_newcamera );

    ///////////////////////////////////////////////////////////////////

    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newspacebox );
    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newchunk );
    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newclouds );
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
    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newheadmvt );
    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newlonglatmvt );

    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_newcamera );

    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_separator );

    m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_edittransformnode );

    //m_scenegraphs_masks[TRANSFO_MASK].push_back( pme_editnodescript );

    ///////////////////////////////////////////////////////////////////

    m_scenegraphs_masks[SPACEBOX_MASK].push_back( pme_showprops );
    m_scenegraphs_masks[SPACEBOX_MASK].push_back( pme_separator );
    m_scenegraphs_masks[SPACEBOX_MASK].push_back( pme_editsb );
    //m_scenegraphs_masks[SPACEBOX_MASK].push_back( pme_editnodescript );

    ///////////////////////////////////////////////////////////////////

    m_scenegraphs_masks[CAMERA_MASK].push_back( pme_showprops );
    m_scenegraphs_masks[CAMERA_MASK].push_back( pme_selectcamera );
    m_scenegraphs_masks[CAMERA_MASK].push_back( pme_separator );
    m_scenegraphs_masks[CAMERA_MASK].push_back( pme_editcamera );    
    //m_scenegraphs_masks[CAMERA_MASK].push_back( pme_editnodescript );


    ///////////////////////////////////////////////////////////////////

    m_scenegraphs_masks[FPSMOVEMENT_MASK].push_back( pme_newchunk );
    m_scenegraphs_masks[FPSMOVEMENT_MASK].push_back( pme_newclouds );
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
    //m_scenegraphs_masks[FPSMOVEMENT_MASK].push_back( pme_editnodescript );


    ///////////////////////////////////////////////////////////////////

    m_scenegraphs_masks[CHUNK_MASK].push_back( pme_newchunk );
    m_scenegraphs_masks[CHUNK_MASK].push_back( pme_newclouds );
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
    m_scenegraphs_masks[CHUNK_MASK].push_back( pme_editchunk );
    //m_scenegraphs_masks[CHUNK_MASK].push_back( pme_editnodescript );

    ///////////////////////////////////////////////////////////////////


    m_scenegraphs_masks[INERTBODY_MASK].push_back( pme_newchunk );
    m_scenegraphs_masks[INERTBODY_MASK].push_back( pme_newclouds );
    m_scenegraphs_masks[INERTBODY_MASK].push_back( pme_separator );
    m_scenegraphs_masks[INERTBODY_MASK].push_back( pme_newcollider );
    m_scenegraphs_masks[INERTBODY_MASK].push_back( pme_separator );
    m_scenegraphs_masks[INERTBODY_MASK].push_back( pme_newtransfo );
    m_scenegraphs_masks[INERTBODY_MASK].push_back( pme_separator );
    m_scenegraphs_masks[INERTBODY_MASK].push_back( pme_newlinearmvt );
    m_scenegraphs_masks[INERTBODY_MASK].push_back( pme_newcircularmvt );
    m_scenegraphs_masks[INERTBODY_MASK].push_back( pme_newfreemvt );
    m_scenegraphs_masks[INERTBODY_MASK].push_back( pme_newfpsmvt );
    m_scenegraphs_masks[INERTBODY_MASK].push_back( pme_newspectatormvt );
    m_scenegraphs_masks[INERTBODY_MASK].push_back( pme_newheadmvt );
    m_scenegraphs_masks[INERTBODY_MASK].push_back( pme_newlonglatmvt );
    m_scenegraphs_masks[INERTBODY_MASK].push_back( pme_newcamera );
    m_scenegraphs_masks[INERTBODY_MASK].push_back( pme_separator );
    m_scenegraphs_masks[INERTBODY_MASK].push_back( pme_showprops );
    m_scenegraphs_masks[INERTBODY_MASK].push_back( pme_separator );
    m_scenegraphs_masks[INERTBODY_MASK].push_back( pme_editinertbody );
    //m_scenegraphs_masks[INERTBODY_MASK].push_back( pme_editnodescript );

    ///////////////////////////////////////////////////////////////////

    m_scenegraphs_masks[COLLIDER_MASK].push_back( pme_newchunk );
    m_scenegraphs_masks[COLLIDER_MASK].push_back( pme_newclouds );
    m_scenegraphs_masks[COLLIDER_MASK].push_back( pme_separator );
    m_scenegraphs_masks[COLLIDER_MASK].push_back( pme_newcollider );
    m_scenegraphs_masks[COLLIDER_MASK].push_back( pme_separator );
    m_scenegraphs_masks[COLLIDER_MASK].push_back( pme_newtransfo );
    m_scenegraphs_masks[COLLIDER_MASK].push_back( pme_separator );
    m_scenegraphs_masks[COLLIDER_MASK].push_back( pme_newlinearmvt );
    m_scenegraphs_masks[COLLIDER_MASK].push_back( pme_newcircularmvt );
    m_scenegraphs_masks[COLLIDER_MASK].push_back( pme_newfreemvt );
    m_scenegraphs_masks[COLLIDER_MASK].push_back( pme_newfpsmvt );
    m_scenegraphs_masks[COLLIDER_MASK].push_back( pme_newspectatormvt );
    m_scenegraphs_masks[COLLIDER_MASK].push_back( pme_newheadmvt );
    m_scenegraphs_masks[COLLIDER_MASK].push_back( pme_newlonglatmvt );
    m_scenegraphs_masks[COLLIDER_MASK].push_back( pme_newcamera );
    m_scenegraphs_masks[COLLIDER_MASK].push_back( pme_separator );
    m_scenegraphs_masks[COLLIDER_MASK].push_back( pme_showprops );



    ///////////////////////////////////////////////////////////////////

    m_scenegraphs_masks[LINMOVEMENT_MASK].push_back( pme_newchunk );
    m_scenegraphs_masks[LINMOVEMENT_MASK].push_back( pme_newclouds );
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
    //m_scenegraphs_masks[LINMOVEMENT_MASK].push_back( pme_editnodescript );
    
    ///////////////////////////////////////////////////////////////////

    m_scenegraphs_masks[FREEMOVEMENT_MASK].push_back( pme_newchunk );
    m_scenegraphs_masks[FREEMOVEMENT_MASK].push_back( pme_newclouds );
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
    //m_scenegraphs_masks[FREEMOVEMENT_MASK].push_back( pme_editnodescript );

    ///////////////////////////////////////////////////////////////////

    m_scenegraphs_masks[CIRCMOVEMENT_MASK].push_back( pme_newchunk );
    m_scenegraphs_masks[CIRCMOVEMENT_MASK].push_back( pme_newclouds );
    m_scenegraphs_masks[CIRCMOVEMENT_MASK].push_back( pme_separator );
    m_scenegraphs_masks[CIRCMOVEMENT_MASK].push_back( pme_newcollider );
    m_scenegraphs_masks[CIRCMOVEMENT_MASK].push_back( pme_neworbit );
    m_scenegraphs_masks[CIRCMOVEMENT_MASK].push_back( pme_newplanet );
    m_scenegraphs_masks[CIRCMOVEMENT_MASK].push_back( pme_separator );
    m_scenegraphs_masks[CIRCMOVEMENT_MASK].push_back( pme_newtransfo );
    m_scenegraphs_masks[CIRCMOVEMENT_MASK].push_back( pme_separator );
    m_scenegraphs_masks[CIRCMOVEMENT_MASK].push_back( pme_newlinearmvt );
    m_scenegraphs_masks[CIRCMOVEMENT_MASK].push_back( pme_newcircularmvt );
    m_scenegraphs_masks[CIRCMOVEMENT_MASK].push_back( pme_newfreemvt );
    m_scenegraphs_masks[CIRCMOVEMENT_MASK].push_back( pme_newfpsmvt );
    m_scenegraphs_masks[CIRCMOVEMENT_MASK].push_back( pme_newspectatormvt );
    m_scenegraphs_masks[CIRCMOVEMENT_MASK].push_back( pme_newheadmvt );
    m_scenegraphs_masks[CIRCMOVEMENT_MASK].push_back( pme_newlonglatmvt );
    m_scenegraphs_masks[CIRCMOVEMENT_MASK].push_back( pme_newcamera );
    m_scenegraphs_masks[CIRCMOVEMENT_MASK].push_back( pme_separator );
    m_scenegraphs_masks[CIRCMOVEMENT_MASK].push_back( pme_showprops );
    m_scenegraphs_masks[CIRCMOVEMENT_MASK].push_back( pme_separator );
    m_scenegraphs_masks[CIRCMOVEMENT_MASK].push_back( pme_editmvt );
    //m_scenegraphs_masks[CIRCMOVEMENT_MASK].push_back( pme_editnodescript );

    ///////////////////////////////////////////////////////////////////

    m_scenegraphs_masks[LONGLATMOVEMENT_MASK].push_back( pme_newchunk );
    m_scenegraphs_masks[LONGLATMOVEMENT_MASK].push_back( pme_newclouds );
    m_scenegraphs_masks[LONGLATMOVEMENT_MASK].push_back( pme_separator );
    m_scenegraphs_masks[LONGLATMOVEMENT_MASK].push_back( pme_newcollider );
    m_scenegraphs_masks[LONGLATMOVEMENT_MASK].push_back( pme_neworbit );
    m_scenegraphs_masks[LONGLATMOVEMENT_MASK].push_back( pme_newplanet );
    m_scenegraphs_masks[LONGLATMOVEMENT_MASK].push_back( pme_separator );
    m_scenegraphs_masks[LONGLATMOVEMENT_MASK].push_back( pme_newtransfo );
    m_scenegraphs_masks[LONGLATMOVEMENT_MASK].push_back( pme_separator );
    m_scenegraphs_masks[LONGLATMOVEMENT_MASK].push_back( pme_newlinearmvt );
    m_scenegraphs_masks[LONGLATMOVEMENT_MASK].push_back( pme_newcircularmvt );
    m_scenegraphs_masks[LONGLATMOVEMENT_MASK].push_back( pme_newfreemvt );
    m_scenegraphs_masks[LONGLATMOVEMENT_MASK].push_back( pme_newfpsmvt );
    m_scenegraphs_masks[LONGLATMOVEMENT_MASK].push_back( pme_newspectatormvt );
    m_scenegraphs_masks[LONGLATMOVEMENT_MASK].push_back( pme_newheadmvt );
    m_scenegraphs_masks[LONGLATMOVEMENT_MASK].push_back( pme_newlonglatmvt );
    m_scenegraphs_masks[LONGLATMOVEMENT_MASK].push_back( pme_newcamera );
    m_scenegraphs_masks[LONGLATMOVEMENT_MASK].push_back( pme_separator );
    m_scenegraphs_masks[LONGLATMOVEMENT_MASK].push_back( pme_showprops );
    m_scenegraphs_masks[LONGLATMOVEMENT_MASK].push_back( pme_separator );
    m_scenegraphs_masks[LONGLATMOVEMENT_MASK].push_back( pme_editmvt );
    //m_scenegraphs_masks[LONGLATMOVEMENT_MASK].push_back( pme_editnodescript );

    ///////////////////////////////////////////////////////////////////

    m_actions[CONTEXTMENU_SHOW_PROPS] = new ActionPropsDialog();
    m_actions[CONTEXTMENU_EDIT_MVT] = new ActionEditMvtDialog();

    m_actions[CONTEXTMENU_NEWSCENENODEGRAPH] = new ActionScenenodeGraphCreationDialog();
    m_actiondialogs_apply[DIALOG_WORLD_CREATION_TITLE] = new ActionWorldCreationApply();

    m_actions[CONTEXTMENU_EDIT_WORLD] = new ActionWorldEditionDialog();
    m_actiondialogs_apply[DIALOG_WORLD_EDITION_TITLE] = new ActionWorldEditionApply();


    m_actions[CONTEXTMENU_NEWWORLD] = new ActionWorldCreationDialog();
    m_actiondialogs_apply[DIALOG_SCENEGRAPH_CREATION_TITLE] = new ActionScenenodeGraphCreationApply();

    m_actions[CONTEXTMENU_NEWSPACEBOX] = new ActionSpaceBoxCreationDialog();
    m_actiondialogs_specific0[DIALOG_SPACEBOX_CREATION_TITLE] = new ActionSpaceBoxCreationSpecific0();
    m_actiondialogs_specific1[DIALOG_SPACEBOX_CREATION_TITLE] = new ActionAddShaderParam();
    m_actiondialogs_apply[DIALOG_SPACEBOX_CREATION_TITLE] = new ActionSpaceBoxCreationApply();

    //m_actionscripts["SpaceboxNode:LoadScript"] = new ActionNodeLoadScript();

    m_actionscripts["SpaceboxNode:UpdateShaderParam"] = new ActionSpaceBoxNodeUpdateShaderParam();

    m_actionscripts["SpaceboxNode:LinkTo"] = new ActionSpaceBoxLinkTo();


    m_actions[CONTEXTMENU_EDIT_SBNODE] = new ActionSpaceBoxEditionDialog();
    m_actiondialogs_apply[DIALOG_SPACEBOX_EDITION_TITLE] = new ActionSpaceBoxEditionApply();


    m_actions[CONTEXTMENU_NEWCHUNK] = new ActionChunkCreationDialog();
    m_actiondialogs_specific0[DIALOG_CHUNK_CREATION_TITLE] = new ActionChunkCreationSpecific0();
    m_actiondialogs_specific1[DIALOG_CHUNK_CREATION_TITLE] = new ActionAddShaderParam();
    m_actiondialogs_specific2[DIALOG_CHUNK_CREATION_TITLE] = new ActionChunkCreationSpecific2();
    m_actiondialogs_apply[DIALOG_CHUNK_CREATION_TITLE] = new ActionChunkCreationApply();

    m_actions[CONTEXTMENU_EDIT_CHUNKNODE] = new ActionChunkEditionDialog();
    m_actiondialogs_apply[DIALOG_CHUNK_EDITION_TITLE] = new ActionChunkEditionApply();

    //m_actionscripts["ChunkNode:LoadScript"] = new ActionNodeLoadScript();

    m_actionscripts["ChunkNode:UpdateShaderParam"] = new ActionChunkNodeUpdateShaderParam();
    m_actionscripts["ChunkNode:LinkTo"] = new ActionChunkLinkTo();

    //////////////

    m_actions[CONTEXTMENU_NEWCLOUDS] = new ActionCloudsCreationDialog();
    
    m_actiondialogs_specific0[DIALOG_CLOUDS_CREATION_TITLE] = new ActionChunkCreationSpecific0();
    m_actiondialogs_specific1[DIALOG_CLOUDS_CREATION_TITLE] = new ActionAddShaderParam();    
    m_actiondialogs_apply[DIALOG_CLOUDS_CREATION_TITLE] = new ActionCloudsCreationApply();
    

    //////////////

    m_actions[CONTEXTMENU_NEWTRANSFO] = new ActionTransformCreationDialog();
    m_actiondialogs_apply[DIALOG_TRANSFORM_CREATION_TITLE] = new ActionTransformCreationApply();

    m_actions[CONTEXTMENU_EDIT_TRANSFORMNODE] = new ActionTransformEditionDialog();
    m_actiondialogs_apply[DIALOG_TRANSFORM_EDITION_TITLE] = new ActionTransformEditionApply();
    m_actiondialogs_specific0[DIALOG_TRANSFORM_EDITION_TITLE] = new ActionAddMatrix();
    m_actiondialogs_specific1[DIALOG_TRANSFORM_EDITION_TITLE] = new ActionTransformEditionSpecific1();

    //m_actionscripts["TransformationNode:LoadScript"] = new ActionNodeLoadScript();
    m_actionscripts["TransformationNode:TransformationNode"] = new ActionTransformationTransformation();
    m_actionscripts["TransformationNode:LinkTo"] = new ActionTransformationLinkTo();

    m_actions[CONTEXTMENU_NEWCAMERA] = new ActionCameraPointCreationDialog();
    m_actiondialogs_apply[DIALOG_CAMERA_CREATION_TITLE] = new ActionCameraPointCreationApply();

    m_actions[CONTEXTMENU_SELECT_CAMERA] = new ActionCameraPointSelection();

    m_actions[CONTEXTMENU_EDIT_CAMERA] = new ActionCameraPointEditionDialog();
    m_actiondialogs_apply[DIALOG_CAMERA_EDIT_TITLE] = new ActionCameraPointEditionApply();

    //m_actionscripts["CameraPointNode:LoadScript"] = new ActionNodeLoadScript();
    m_actionscripts["CameraPointNode:CameraPointNode"] = new ActionCameraPointCameraPoint();
    m_actionscripts["CameraPointNode:LinkTo"] = new ActionCameraPointLinkTo();



    m_actions[CONTEXTMENU_NEWFPSMVT] = new ActionFPSMvtCreationDialog();
    m_actiondialogs_apply[DIALOG_FPSMVT_CREATION_TITLE] = new ActionFPSMvtCreationApply();
    m_actionscripts["FpsMovementNode:FpsMovementNode"] = new ActionFPSMvtFPSMvt();
    m_actionscripts["FpsMovementNode:LinkTo"] = new ActionFPSMvtLinkTo();

    //m_actionscripts["FpsMovementNode:LoadScript"] = new ActionNodeLoadScript();

    m_actions[CONTEXTMENU_NEWLINEARMVT] = new ActionLinearMvtCreationDialog();
    m_actiondialogs_apply[DIALOG_LINMVT_CREATION_TITLE] = new ActionLinearMvtCreationApply();
    m_actionscripts["LinearMovementNode:LinearMovementNode"] = new ActionLinearMvtLinearMvt();
    m_actionscripts["LinearMovementNode:LinkTo"] = new ActionLinearMvtLinkTo();

    m_actiondialogs_apply[DIALOG_LINMVT_EDITION_TITLE] = new ActionLinearMvtEditionApply();

    //m_actionscripts["LinearMovementNode:LoadScript"] = new ActionNodeLoadScript();


    m_actions[CONTEXTMENU_NEWFREEMVT] = new ActionFreeMvtCreationDialog();
    m_actiondialogs_apply[DIALOG_FREEMVT_CREATION_TITLE] = new ActionFreeMvtCreationApply();
    m_actionscripts["FreeMovementNode:FreeMovementNode"] = new ActionFreeMvtFreeMvt();
    m_actionscripts["FreeMovementNode:LinkTo"] = new ActionFreeMvtLinkTo();

    //m_actionscripts["FreeMovementNode:LoadScript"] = new ActionNodeLoadScript();

    m_actions[CONTEXTMENU_NEWCIRCULARMVT] = new ActionCircularMvtCreationDialog();
    m_actiondialogs_apply[DIALOG_CIRCMVT_CREATION_TITLE] = new ActionCircularMvtCreationApply();
    m_actionscripts["CircularMovementNode:CircularMovementNode"] = new ActionCircularMvtCircularMvt();
    m_actionscripts["CircularMovementNode:LinkTo"] = new ActionCircularMvtLinkTo();

    m_actiondialogs_apply[DIALOG_CIRCMVT_EDITION_TITLE] = new ActionCircularMvtEditionApply();

    //m_actionscripts["CircularMovementNode:LoadScript"] = new ActionNodeLoadScript();



    m_actions[CONTEXTMENU_NEWLONGLATMVT] = new ActionLongLatCreationDialog();
    m_actiondialogs_apply[DIALOG_LONGLATMVT_CREATION_TITLE] = new ActionLongLatCreationApply();
    m_actionscripts["LongLatMovementNode:LinkTo"] = new ActionLongLatLinkTo();
    m_actionscripts["LongLatMovementNode:LongLatMovementNode"] = new ActionLongLatLongLat();


    m_actiondialogs_apply[DIALOG_LONGLATMVT_EDITION_TITLE] = new ActionLongLatMvtEditionApply();

    //m_actionscripts["LongLatMovementNode:LoadScript"] = new ActionNodeLoadScript();


    m_actions[CONTEXTMENU_NEWINERTBODY] = new ActionInertBodyCreationDialog();
    m_actiondialogs_specific0[DIALOG_INERTBODY_CREATION_TITLE] = new ActionAddMatrix();
    m_actiondialogs_apply[DIALOG_INERTBODY_CREATION_TITLE] = new ActionInertBodyCreationApply();

    m_actionscripts["InertBodyNode:InertBodyNode"] = new ActionInertBodyInertBody();
    m_actionscripts["InertBodyNode:LinkTo"] = new ActionInertBodyLinkTo();

    m_actions[CONTEXTMENU_EDIT_INERTBODY] = new ActionInertBodyEditionDialog();
    m_actiondialogs_apply[DIALOG_INERTBODY_EDITION_TITLE] = new ActionInertBodyEditionApply();

    m_actions[CONTEXTMENU_NEWCOLLIDER] = new ActionColliderCreationDialog();
    m_actiondialogs_apply[DIALOG_COLLIDER_CREATION_TITLE] = new ActionColliderCreationApply();

    m_actionscripts["ColliderNode:ColliderNode"] = new ActionColliderCollider();
    m_actionscripts["ColliderNode:LinkTo"] = new ActionColliderLinkTo();

    m_actions[CONTEXTMENU_EDIT_MOUSEMOVESCRIPT] = new ActionMouseMoveScriptEditionDialog();
    m_actionscripts["DrawSpace:LoadMouseScript"] = new ActionMouseLoadScript();

    m_actionscripts["Mouse:Mouse"] = new ActionMouseMouse();

    m_actions[CONTEXTMENU_EDIT_KEYDOWNSCRIPT] = new ActionKeydownScriptEditionDialog();
    m_actionscripts["DrawSpace:LoadKeyDownScript"] = new ActionKeyDownLoadScript();

    m_actions[CONTEXTMENU_EDIT_KEYUPSCRIPT] = new ActionKeyupScriptEditionDialog();
    m_actionscripts["DrawSpace:LoadKeyUpScript"] = new ActionKeyUpLoadScript();

    m_actionscripts["Keyboard:Keyboard"] = new ActionKeyboardKeyboard();


    m_actionscripts["Texture:Texture"] = new ActionTextureCtor();


    m_actionscripts["global:print"] = new ActionGlobalPrint();


    m_actionscripts["DrawSpace:DrawSpace"] = new ActionDrawSpaceDrawSpace();
    m_actionscripts["DrawSpace:GetSceneCameraName"] = new ActionGetSceneCameraName();
    m_actionscripts["DrawSpace:IsCurrentCamera"] = new ActionIsCurrentCamera();
    m_actionscripts["DrawSpace:SetSceneNodeGraphCurrentCamera"] = new ActionSetSceneNodeGraphCurrentCamera();
    m_actionscripts["DrawSpace:SetWorldGravity"] = new ActionSetWorldGravity();
    m_actionscripts["DrawSpace:DisplayFramerate"] = new ActionDisplayFramerate();
    m_actionscripts["DrawSpace:DisplayCurrentCamera"] = new ActionDisplayCurrentCamera();
    m_actionscripts["DrawSpace:CreateSceneNodeGraph"] = new ActionCreateSceneNodeGraph();
    m_actionscripts["DrawSpace:CreateWorld"] = new ActionCreateWorld();
    m_actionscripts["DrawSpace:AngleSpeedInc"] = new ActionAngleSpeedInc();
    m_actionscripts["DrawSpace:AngleSpeedDec"] = new ActionAngleSpeedDec();
    m_actionscripts["DrawSpace:TranslationSpeedInc"] = new ActionTranslationSpeedInc();
    m_actionscripts["DrawSpace:TranslationSpeedDec"] = new ActionTranslationSpeedDec();
    m_actionscripts["DrawSpace:UpdatePassShaderParam"] = new ActionUpdatePassShaderParam();
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    m_applybutton_clicked_cb = new DialogButtonCallback( this, &BasicSceneMainFrame::on_applybutton_clicked );
    m_specificbutton0_clicked_cb = new DialogButtonCallback( this, &BasicSceneMainFrame::on_specificbutton0_clicked );
    m_specificbutton1_clicked_cb = new DialogButtonCallback( this, &BasicSceneMainFrame::on_specificbutton1_clicked );
    m_specificbutton2_clicked_cb = new DialogButtonCallback( this, &BasicSceneMainFrame::on_specificbutton2_clicked );

    //m_nodeupdatebegin_cb = new NodeUpdateBeginCallBack( this, &BasicSceneMainFrame::on_nodeupdatebegin );


    ////////////////////////////////////

    m_mousemovescripteditionframe = new BasicSceneScriptEditFrame( this, "Mouse move", &m_mousemove_script, &m_mousemove_script_enabled );
    m_keyupscripteditionframe = new BasicSceneScriptEditFrame( this, "Key up", &m_keyup_script, &m_keyup_script_enabled );
    m_keydownscripteditionframe = new BasicSceneScriptEditFrame( this, "Key down", &m_keydown_script, &m_keydown_script_enabled );


    m_calendar = new Calendar( 0, &m_timer );

    m_calendar->Startup( 0 );

#ifndef RENDER_IDLE
    RenderTimer* timer = new RenderTimer(this);
    timer->Start();
    m_rendertimer = timer;
#endif
}

BasicSceneMainFrame* BasicSceneMainFrame::GetInstance( void )
{
    if( !m_instance )
    {
        m_instance = new BasicSceneMainFrame( NULL );
    }
    return m_instance;
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

    m_actionscripts[script_call_id]->Execute( p_propertypool );
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

        m_calendar->Run();

        /*
        m_timer.Update();
        if( m_timer.IsReady() )
        {

        }
        */
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
    wxBitmap bmp_world( "icon_world.bmp", wxBITMAP_TYPE_BMP );
    wxBitmap bmp_clouds( "icon_clouds.bmp", wxBITMAP_TYPE_BMP );


    
    pImageList->Add( bmp_camera );      // 0
    pImageList->Add( bmp_chunk );       // 1
    pImageList->Add( bmp_collider );    // 2
    pImageList->Add( bmp_inertbody );   // 3
    pImageList->Add( bmp_movement );    // 4
    pImageList->Add( bmp_orbit );       // 5
    pImageList->Add( bmp_orbiter );     // 6
    pImageList->Add( bmp_planet );      // 7
    pImageList->Add( bmp_rocket );      // 8
    pImageList->Add( bmp_spacebox );    // 9
    pImageList->Add( bmp_transfo );     // 10
    pImageList->Add( bmp_scenegraph );  // 11
    pImageList->Add( bmp_drawspace );   // 12
    pImageList->Add( bmp_keyboard );    // 13
    pImageList->Add( bmp_mouse );       // 14
    pImageList->Add( bmp_camera_sel );  // 15
    pImageList->Add( bmp_world );       // 16
    pImageList->Add( bmp_clouds );       // 17
    

    m_scenegraphs_treeCtrl->AssignImageList( pImageList );
    
    m_scenegraphs_root_item = m_scenegraphs_treeCtrl->AddRoot( "DrawSpace", DRAWSPACE_ICON_INDEX );
    m_keyboard_item = m_scenegraphs_treeCtrl->AppendItem( m_scenegraphs_root_item, "Keyboard", KEYBOARD_ICON_INDEX );
    m_mouse_item = m_scenegraphs_treeCtrl->AppendItem( m_scenegraphs_root_item, "Mouse", MOUSE_ICON_INDEX );


    ///////////////////////////////////////////////////////////////////////////////////////////////////

    m_menubuild_table[m_scenegraphs_root_item.GetID()] = DRAWSPACE_MASK;
    m_menubuild_table[m_keyboard_item.GetID()] = KEYBOARD_MASK;
    m_menubuild_table[m_mouse_item.GetID()] = MOUSE_MASK;
    
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

        Texture::RenderPurpose renderpurpose = props["renderpurpose"].GetPropValue<Texture::RenderPurpose>();

        
        DIALOG_DECLARE( DIALOG_TEXTURE_PROPS_TITLE )
        DIALOG_APPENDROOT_STRING( "assetname", assetname )
        DIALOG_APPENDROOT_STRING( "filepath", filepath )
        DIALOG_APPENDROOT_BOOL( "rendertarget", rendertarget )

        if( rendertarget )
        {
            DIALOG_APPENDROOT_NODE( "rendertarget dims", rendertarget_dimsnode )

            DIALOG_APPENDNODE_INTEGER( rendertarget_dimsnode, "width", rendetarget_w )
            DIALOG_APPENDNODE_INTEGER( rendertarget_dimsnode, "height", rendetarget_h )

            if( Texture::RENDERPURPOSE_COLOR == renderpurpose )
            {
                DIALOG_APPENDNODE_STRING( rendertarget_dimsnode, "render purpose", "color" );
            }
            else
            {
                DIALOG_APPENDNODE_STRING( rendertarget_dimsnode, "render purpose", "float" );
            }
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

            Texture::RenderPurpose renderpurpose = props["renderpurpose"].GetPropValue<Texture::RenderPurpose>();

            if( Texture::RENDERPURPOSE_COLOR == renderpurpose )
            {
                DIALOG_APPENDROOT_STRING( "render purpose", "color" );
            }
            else
            {
                DIALOG_APPENDROOT_STRING( "render purpose", "float" );
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
            ViewportQuad* vpq = ipass->GetViewportQuad();
            std::map<dsstring, RenderingNode::ShadersParams*> shaders_params_list;
            vpq->GetShadersParams( shaders_params_list );

            
            DIALOG_APPENDROOT_NODE( "viewport quad shader params", viewportquadshaderparams_root )
                           
            for( std::map<dsstring, RenderingNode::ShadersParams>::iterator it = viewportquad_shaderparams.begin(); it != viewportquad_shaderparams.end(); ++ it )
            {
               
                DIALOG_APPENDNODE_NODE( viewportquadshaderparams_root, it->first, shaderparam )

                DIALOG_APPENDNODE_INTEGER( shaderparam, "shader index", it->second.shader_index )
                DIALOG_APPENDNODE_INTEGER( shaderparam, "register", it->second.param_register )

                DIALOG_APPENDNODE_NODE( shaderparam, "values", shaderparamvalues )

                Vector params_value = shaders_params_list[it->first]->param_values;

                DIALOG_APPENDNODE_FLOAT( shaderparamvalues, "x", params_value[0] )
                DIALOG_APPENDNODE_FLOAT( shaderparamvalues, "y", params_value[1] )
                DIALOG_APPENDNODE_FLOAT( shaderparamvalues, "z", params_value[2] )
                DIALOG_APPENDNODE_FLOAT( shaderparamvalues, "w", params_value[3] )


                /*
                DIALOG_APPENDNODE_FLOAT( shaderparamvalues, "x", it->second.param_values[0] )
                DIALOG_APPENDNODE_FLOAT( shaderparamvalues, "y", it->second.param_values[1] )
                DIALOG_APPENDNODE_FLOAT( shaderparamvalues, "z", it->second.param_values[2] )
                DIALOG_APPENDNODE_FLOAT( shaderparamvalues, "w", it->second.param_values[3] )
                */
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

void BasicSceneMainFrame::AdaptPassesShaderParamsList( DrawSpace::Pass* p_pass )
{
    wxWidgetAdapter::GetInstance()->AdaptPassesShaderParamsList( p_pass, m_shadersparams_listCtrl );
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
    int id = p_evt.GetId();
    m_actions[id]->Execute();
}

void BasicSceneMainFrame::OnSceneNodeGraphsListRightClick( wxTreeEvent& p_event )
{	
 	wxMenu mnu;
 	
    wxTreeItemId item = p_event.GetItem();

    m_last_clicked_treeitem = item;
    build_popupmenu( m_menubuild_table[item.GetID()], mnu );
 	mnu.Connect( wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&BasicSceneMainFrame::OnPopupClick, NULL, this );
 	PopupMenu(&mnu);
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

wxArrayString BasicSceneMainFrame::get_world_list( void )
{
    wxArrayString availables_world_labels;

    for( std::map<void*, WorldEntry>::iterator it = m_worlds.begin(); it != m_worlds.end(); ++it )
    {
        availables_world_labels.Add( it->second.name );
    }
    return availables_world_labels;
}

wxArrayString BasicSceneMainFrame::get_scenenodes_list( SceneNodeGraph* p_sc )
{
    wxArrayString availables_nodes_names;

    std::vector<BaseSceneNode*> n_list = p_sc->GetAllNodesList();

    for( size_t i = 0; i < n_list.size(); i++ )
    {
        dsstring node_name;
        n_list[i]->GetSceneName( node_name );

        availables_nodes_names.Add( node_name.c_str() );
    }
    return availables_nodes_names;
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
    wxCharBuffer buffer;
    DIALOG_WXSTRING_TO_DSSTRING( DIALOG_TITLE, dialog_title )
    m_actiondialogs_apply[dialog_title]->Execute( p_dialog );
}

void BasicSceneMainFrame::on_specificbutton0_clicked( BasicSceneObjectPropertiesDialog* p_dialog )
{
    wxCharBuffer buffer;
    DIALOG_WXSTRING_TO_DSSTRING( DIALOG_TITLE, dialog_title )
    m_actiondialogs_specific0[dialog_title]->Execute( p_dialog );
}

void BasicSceneMainFrame::on_specificbutton1_clicked( BasicSceneObjectPropertiesDialog* p_dialog )
{
    wxCharBuffer buffer;
    DIALOG_WXSTRING_TO_DSSTRING( DIALOG_TITLE, dialog_title )
    m_actiondialogs_specific1[dialog_title]->Execute( p_dialog );
}

void BasicSceneMainFrame::on_specificbutton2_clicked( BasicSceneObjectPropertiesDialog* p_dialog )
{
    wxCharBuffer buffer;
    DIALOG_WXSTRING_TO_DSSTRING( DIALOG_TITLE, dialog_title )
    m_actiondialogs_specific2[dialog_title]->Execute( p_dialog );
}


wxTreeItemId BasicSceneMainFrame::searchTreeItemIdInNodes( void* p_id )
{
    return *( m_inv_treeitemid[p_id] );
}

wxTreeItemId BasicSceneMainFrame::AppendItem( const wxTreeItemId& p_parent_item, const wxString& p_text, int p_image )
{
    wxTreeItemId treeitemid = m_scenegraphs_treeCtrl->AppendItem( p_parent_item, p_text, p_image );
    BasicSceneMainFrame::GetInstance()->m_scenegraphs_treeCtrl->ExpandAllChildren( p_parent_item );
    return treeitemid;
}

void BasicSceneMainFrame::SetItemImage( const wxTreeItemId& p_item, int p_image )
{
    m_scenegraphs_treeCtrl->SetItemImage( p_item, p_image );
}