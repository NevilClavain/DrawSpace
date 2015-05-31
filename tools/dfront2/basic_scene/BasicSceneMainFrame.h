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

#ifndef __BasicSceneMainFrame__
#define __BasicSceneMainFrame__

#include <wx/timer.h>
#include "panel.h"
#include "drawspace.h"
#include "scripting.h"
#include "BasicSceneObjectPropertiesDialog.h"
#include "BasicSceneScriptEditFrame.h"
#include "action.h"
#include "actiondialog.h"
#include "actionscript.h"

#pragma warning( disable : 4101 )

#define DFRONT_ICON_DIM                     22

#define CAMERA_ICON_INDEX                   0
#define CHUNK_ICON_INDEX                    1
#define COLLIDER_ICON_INDEX                 2
#define INERTBODY_ICON_INDEX                3
#define MOVEMENT_ICON_INDEX                 4
#define ORBIT_ICON_INDEX                    5
#define ORBITER_ICON_INDEX                  6
#define PLANET_ICON_INDEX                   7
#define ROCKET_ICON_INDEX                   8
#define SPACEBOX_ICON_INDEX                 9
#define TRANSFO_ICON_INDEX                  10
#define SCENEGRAPH_ICON_INDEX               11
#define DRAWSPACE_ICON_INDEX                12
#define KEYBOARD_ICON_INDEX                 13
#define MOUSE_ICON_INDEX                    14
#define CAMERASEL_ICON_INDEX                15
#define WORLD_ICON_INDEX                    16


#define CAMERA_MASK                         1
#define CHUNK_MASK                          2
#define COLLIDER_MASK                       3
#define INERTBODY_MASK                      4
//#define MOVEMENT_MASK                       5
#define ORBIT_MASK                          6
#define ORBITER_MASK                        7
#define PLANET_MASK                         8
#define ROCKET_MASK                         9
#define SPACEBOX_MASK                       10
#define TRANSFO_MASK                        11
#define SCENEGRAPH_MASK                     12
#define DRAWSPACE_MASK                      13
#define KEYBOARD_MASK                       14
#define MOUSE_MASK                          15
#define WORLD_MASK                          16


#define FPSMOVEMENT_MASK                    500
#define LINMOVEMENT_MASK                    501
#define FREEMOVEMENT_MASK                   502
#define CIRCMOVEMENT_MASK                   503
#define LONGLATMOVEMENT_MASK                504


#define CONTEXTMENU_NEWSCENENODEGRAPH       2000
#define CONTEXTMENU_NEWWORLD                2001

#define CONTEXTMENU_NEWSPACEBOX             2010
#define CONTEXTMENU_NEWCHUNK                2011
#define CONTEXTMENU_NEWINERTBODY            2012
#define CONTEXTMENU_NEWCOLLIDER             2013
#define CONTEXTMENU_NEWROCKET               2014
#define CONTEXTMENU_NEWORBIT                2015
#define CONTEXTMENU_NEWORBITER              2016
#define CONTEXTMENU_NEWPLANET               2017
#define CONTEXTMENU_NEWTRANSFO              2018
#define CONTEXTMENU_NEWCAMERA               2019
#define CONTEXTMENU_NEWLINEARMVT            2020
#define CONTEXTMENU_NEWCIRCULARMVT          2021
#define CONTEXTMENU_NEWFPSMVT               2022
#define CONTEXTMENU_NEWFREEMVT              2023
#define CONTEXTMENU_NEWHEADMVT              2024
#define CONTEXTMENU_NEWLONGLATMVT           2025
#define CONTEXTMENU_NEWSPECTATORMVT         2026
#define CONTEXTMENU_SEPARATOR               2080
#define CONTEXTMENU_EDIT_TRANSFORMNODE      2081
//#define CONTEXTMENU_EDIT_SHADERSPARAMS      2082
#define CONTEXTMENU_SHOW_PROPS              2083
#define CONTEXTMENU_EDIT_NODESCRIPT         2084

#define CONTEXTMENU_EDIT_MOUSEMOVESCRIPT    2085
#define CONTEXTMENU_EDIT_KEYDOWNSCRIPT      2086
#define CONTEXTMENU_EDIT_KEYUPSCRIPT        2087

#define CONTEXTMENU_SELECT_CAMERA           2088
#define CONTEXTMENU_EDIT_CAMERA             2089

#define CONTEXTMENU_EDIT_MVT                2090

#define CONTEXTMENU_EDIT_CHUNKNODE          2091
#define CONTEXTMENU_EDIT_SBNODE             2092
#define CONTEXTMENU_EDIT_WORLD              2093


//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define DIALOG_TEXTURE_PROPS_TITLE          "Texture properties"
#define DIALOG_SHADER_PROPS_TITLE           "Shader properties"
#define DIALOG_FONT_PROPS_TITLE             "Font properties"
#define DIALOG_MESHE_PROPS_TITLE            "Meshe properties"
#define DIALOG_FX_PROPS_TITLE               "Fx properties"
#define DIALOG_IPASS_PROPS_TITLE            "Intermediate Pass properties"
#define DIALOG_FPASS_PROPS_TITLE            "Final Pass properties"
#define DIALOG_TRANSFORM_CREATION_TITLE     "Transformation node creation"
#define DIALOG_TRANSFORM_EDITION_TITLE      "Transformation node edition"
#define DIALOG_SPACEBOX_CREATION_TITLE      "Spacebox node creation"
#define DIALOG_SPACEBOX_PROPS_TITLE         "Spacebox node properties"
#define DIALOG_SPACEBOX_EDITION_TITLE       "Spacebox node edition"
#define DIALOG_SCENEGRAPH_CREATION_TITLE    "Scenegraph creation"
#define DIALOG_WORLD_CREATION_TITLE         "World creation"
#define DIALOG_WORLD_EDITION_TITLE          "World edition"
#define DIALOG_CAMERA_CREATION_TITLE        "Camera node creation"
#define DIALOG_CAMERA_PROPS_TITLE           "Camera node properties"
#define DIALOG_CAMERA_EDIT_TITLE            "Camera node edition"
#define DIALOG_FPSMVT_CREATION_TITLE        "FPS mvt node creation"
#define DIALOG_FPSMVT_PROPS_TITLE           "FPS mvt node properties"
#define DIALOG_CHUNK_CREATION_TITLE         "Chunk node creation"
#define DIALOG_CHUNK_PROPS_TITLE            "Chunk node properties"
#define DIALOG_CHUNK_EDITION_TITLE          "Chunk node edition"
#define DIALOG_LINMVT_CREATION_TITLE        "Linear mvt node creation"
#define DIALOG_LINMVT_EDITION_TITLE         "Linear mvt node edition"
#define DIALOG_LINMVT_PROPS_TITLE           "Linear mvt node properties"
#define DIALOG_FREEMVT_CREATION_TITLE       "Free mvt node creation"
#define DIALOG_FREEMVT_PROPS_TITLE          "Free mvt node properties"
#define DIALOG_CIRCMVT_CREATION_TITLE       "Circular mvt node creation"
#define DIALOG_CIRCMVT_EDITION_TITLE        "Circular mvt node edition"
#define DIALOG_CIRCMVT_PROPS_TITLE          "Circular mvt node properties"
#define DIALOG_LONGLATMVT_CREATION_TITLE    "Longlat mvt node creation"
#define DIALOG_LONGLATMVT_EDITION_TITLE     "Longlat mvt node edition"
#define DIALOG_LONGLATMVT_PROPS_TITLE       "Longlat mvt node properties"
#define DIALOG_INERTBODY_CREATION_TITLE     "InertBody node creation"


//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define DIALOG_DECLARE( _title_ ) \
    BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( this, _title_, m_last_clicked_treeitem ); \
    wxPropertyGrid* propertygrid = dialog->GetPropertyGrid(); \
    dialog->RegisterApplyButtonHandler( m_applybutton_clicked_cb ); \
    dialog->RegisterSpecificButton0Handler( m_specificbutton0_clicked_cb ); \
    dialog->RegisterSpecificButton1Handler( m_specificbutton1_clicked_cb ); \


#define DIALOG_ACTION_DECLARE( _title_ ) \
    BasicSceneObjectPropertiesDialog* dialog = new BasicSceneObjectPropertiesDialog( BasicSceneMainFrame::GetInstance(), _title_, BasicSceneMainFrame::GetInstance()->m_last_clicked_treeitem ); \
    wxPropertyGrid* propertygrid = dialog->GetPropertyGrid(); \
    dialog->RegisterApplyButtonHandler( BasicSceneMainFrame::GetInstance()->m_applybutton_clicked_cb ); \
    dialog->RegisterSpecificButton0Handler( BasicSceneMainFrame::GetInstance()->m_specificbutton0_clicked_cb ); \
    dialog->RegisterSpecificButton1Handler( BasicSceneMainFrame::GetInstance()->m_specificbutton1_clicked_cb ); \



#define DIALOG_GETGRID wxPropertyGrid* propertygrid = p_dialog->GetPropertyGrid();

#define DIALOG_SHOW dialog->Show(); propertygrid->ResetColumnSizes(); propertygrid->CollapseAll();

#define DIALOG_FINALIZE propertygrid->ResetColumnSizes(); propertygrid->CollapseAll();


#define DIALOG_APPENDROOT_STRING( _label_, _value_ )    propertygrid->Append( new wxStringProperty( _label_, wxPG_LABEL, _value_ ) );
#define DIALOG_APPENDROOT_INTEGER( _label_, _value_ )   propertygrid->Append( new wxIntProperty( _label_, wxPG_LABEL, _value_ ) );
#define DIALOG_APPENDROOT_FLOAT( _label_, _value_ )     propertygrid->Append( new wxFloatProperty( _label_, wxPG_LABEL, _value_ ) );
#define DIALOG_APPENDROOT_ENUM( _label_, _value_ )      propertygrid->Append( new wxEnumProperty( _label_, wxPG_LABEL, _value_ ) );
#define DIALOG_APPENDROOT_BOOL( _label_, _value_ )      propertygrid->Append( new wxBoolProperty( _label_, wxPG_LABEL, _value_ ) );

#define DIALOG_APPENDROOT_NODE( _label_, _var_name_ )   wxPGProperty* _var_name_ = propertygrid->Append( new wxStringProperty( _label_, wxPG_LABEL, "<composed>" ) );


#define DIALOG_APPENDNODE_NODE( _parent_, _label_, _var_name_ ) wxPGProperty* _var_name_ = propertygrid->AppendIn( _parent_, new wxStringProperty( _label_, wxPG_LABEL, "<composed>" ) );

#define DIALOG_APPENDNODE_STRING( _parent_, _label_, _value_ )      propertygrid->AppendIn( _parent_, new wxStringProperty( _label_, wxPG_LABEL, _value_ ) );
#define DIALOG_APPENDNODE_INTEGER( _parent_, _label_, _value_ )     propertygrid->AppendIn( _parent_, new wxIntProperty( _label_, wxPG_LABEL, _value_ ) );
#define DIALOG_APPENDNODE_FLOAT( _parent_, _label_, _value_ )       propertygrid->AppendIn( _parent_, new wxFloatProperty( _label_, wxPG_LABEL, _value_ ) );
#define DIALOG_APPENDNODE_ENUM( _parent_, _label_, _value_ )        propertygrid->AppendIn( _parent_, new wxEnumProperty( _label_, wxPG_LABEL, _value_ ) );
#define DIALOG_APPENDNODE_ENUM_PRESELECTED( _parent_, _label_, _value_, _array_ids_, _index_ )        propertygrid->AppendIn( _parent_, new wxEnumProperty( _label_, wxPG_LABEL, _value_, _array_ids_, _index_ ) );

#define DIALOG_APPENDNODE_BOOL( _parent_, _label_, _value_ )        propertygrid->AppendIn( _parent_, new wxBoolProperty( _label_, wxPG_LABEL, _value_ ) );


#define DIALOG_APPENDNODE_ITERATE( _parent_, _value_, _func_, _labels_ ) \
    for( size_t i = 0; i < _labels_.size(); i++ ) \
    { \
        wxString current = _labels_[i]; \
        _func_( _parent_, current, _value_ )\
    } \

#define DIALOG_APPENDNODE_ITERATE_NODE_BEGIN( _parent_, _counter_, _list_, _var_name_ ) \
    for( size_t _counter_ = 0; _counter_ < _list_.size(); _counter_++ ) \
    { \
        DIALOG_APPENDNODE_NODE( _parent_, _list_[_counter_], _var_name_ ) \


#define DIALOG_APPENDNODE_ITERATE_NODE_END \
    } \


#define DIALOG_APPENDROOT_ITERATE_NODE_BEGIN( _counter_, _list_, _var_name_ ) \
    for( size_t _counter_ = 0; _counter_ < _list_.size(); _counter_++ ) \
    { \
        DIALOG_APPENDROOT_NODE( _list_[_counter_], _var_name_ ) \


#define DIALOG_APPENDROOT_ITERATE_NODE_END \
    } \


#define DIALOG_BUILD_LABELS( _count_, _format_, _var_name_ ) \
    wxArrayString _var_name_; \
    for( size_t i = 0; i < (size_t)_count_; i++ ) \
    { \
        char comment[128]; \
        sprintf( comment, _format_, i ); \
        wxString curr = comment; \
        _var_name_.Add( curr ); \
    } \

#define DIALOG_APPLY                                            dialog->EnableApplyButton();
#define DIALOG_SPECIFIC0( _text_ )                              dialog->EnableSpecificButton0( _text_ );
#define DIALOG_SPECIFIC1( _text_ )                              dialog->EnableSpecificButton1( _text_ );

#define DIALOG_TITLE                                            p_dialog->GetTitle()

#define DIALOG_SPECIFIC0_LABEL( _format_, _var_name_ ) \
    char comment[128]; \
    sprintf( comment, _format_, p_dialog->GetSpecific0Counter() ); \
    wxString _var_name_ = comment; \

#define DIALOG_SPECIFIC1_LABEL( _format_, _var_name_ ) \
    char comment[128]; \
    sprintf( comment, _format_, p_dialog->GetSpecific1Counter() ); \
    wxString _var_name_ = comment; \


#define DIALOG_PROPERTIES_VARS \
        wxStringProperty* string_prop; \
        wxFloatProperty* float_prop; \
        wxIntProperty* int_prop; \
        wxBoolProperty* bool_prop; \
        wxEnumProperty* enum_prop; \
        wxCharBuffer buffer; \
        wxAny value; \

#define DIALOG_CHECK_PROPERTY( _name_ ) propertygrid->GetProperty( _name_ )

#define DIALOG_GET_STRING_PROPERTY( _name_, _var_name_ ) \
        wxString _var_name_; \
        string_prop = static_cast<wxStringProperty*>( propertygrid->GetProperty( _name_ ) ); \
        value = string_prop->GetValue(); \
        value.GetAs<wxString>( &_var_name_ ); \

#define DIALOG_GET_INT_PROPERTY( _name_, _var_name_ ) \
        int _var_name_; \
        int_prop = static_cast<wxIntProperty*>( propertygrid->GetProperty( _name_ ) ); \
        value = int_prop->GetValue(); \
        value.GetAs<int>( &_var_name_ ); \

#define DIALOG_GET_BOOL_PROPERTY( _name_, _var_name_ ) \
        bool _var_name_; \
        bool_prop = static_cast<wxBoolProperty*>( propertygrid->GetProperty( _name_ ) ); \
        value = bool_prop->GetValue(); \
        value.GetAs<bool>( &_var_name_ );
        
#define DIALOG_GET_FLOAT_PROPERTY( _name_, _var_name_ ) \
        dsreal _var_name_;\
        float_prop = static_cast<wxFloatProperty*>( propertygrid->GetProperty( _name_ ) ); \
        value = float_prop->GetValue(); \
        value.GetAs<dsreal>( &_var_name_ );
        
#define DIALOG_GET_ENUM_PROPERTY( _name_, _var_name_ ) \
        wxString _var_name_; \
        enum_prop = static_cast<wxEnumProperty*>( propertygrid->GetProperty( _name_ ) ); \
        _var_name_ = enum_prop->GetValueAsString(); \

#define DIALOG_WXSTRING_TO_DSSTRING( _src_var_name_, _dst_var_name_ ) \
        dsstring _dst_var_name_; \
        buffer = _src_var_name_.ToAscii(); \
        _dst_var_name_ = buffer.data(); \


#define DIALOG_EXPLORE_NODES_BEGIN( _root_, _format_, _counter_, _curr_label_ ) \
        long _counter_ = 0;\
        while( 1 ) \
        { \
            dsstring _curr_label_; \
            char comment[128]; \
            sprintf( comment, _format_, _counter_ ); \
            dsstring final; \
            if( _root_ != "" ) \
            { \
                final = _root_ + dsstring( "." ) + comment;\
            } \
            else \
            { \
                final = comment; \
            } \
            if( NULL == DIALOG_CHECK_PROPERTY( final ) )\
            { \
                break;\
            } \
            _curr_label_ = final; \
  

#define DIALOG_EXPLORE_NODES_END( _counter_ ) \
            _counter_++;\
        } \

#define DIALOG_INCREMENT_STRING( _src_, _delta_ ) \
        dsstring( _src_ + dsstring( "." ) + _delta_ ) \

#define DIALOG_CLOSE p_dialog->Close();


#define DIALOG_CLEARGRID p_dialog->GetPropertyGrid()->Clear();

#define DIALOG_SETSPECIFIC0COUNTER( _value_ )    p_dialog->SetSpecific0Counter( _value_ );
#define DIALOG_SETSPECIFIC1COUNTER( _value_ )    p_dialog->SetSpecific1Counter( _value_ );

//////////////////////////////////////////////////////////////////////////////////////////////////////////

class BasicSceneMainFrame;
class RenderTimer : public wxTimer
{
    BasicSceneMainFrame* pane;
public:
    RenderTimer( BasicSceneMainFrame* p_pane );
    void Notify( void );
    void Start( void );
    void Stop( void );
};


class BasicSceneMainFrame : public MainFrame
{
public:

    typedef struct
    {
        int         id;
        dsstring    text;

    } PopupMenuEntry;
    
    typedef struct
    {
        dsstring                            name;
        DrawSpace::Core::SceneNodeGraph*    scenenodegraph;        
        wxTreeItemId                        treeitemid;
        bool                                current_camera_set;
        wxTreeItemId                        current_camera;

    } SceneNodeGraphEntry;

    typedef struct
    {
        dsstring                            name;
        DrawSpace::Dynamics::World*         world;
        wxTreeItemId                        treeitemid;

    } WorldEntry;

    template <typename Base>
    class SceneNodeEntry
    {
    public:
        SceneNodeEntry( void )
        {
            script_enabled = true;
        }

        dsstring                                                        name;
        DrawSpace::Core::SceneNode<Base>*                               scene_node;
        wxTreeItemId                                                    treeitemid;

        dsstring                                                        script;
        bool                                                            script_enabled;
    };


    typedef DrawSpace::Core::CallBack<BasicSceneMainFrame, void, const dsstring&>                   TimerCallback;
    typedef DrawSpace::Core::CallBack<BasicSceneMainFrame, void, const dsstring&>                   ScriptingErrorCallback;

    typedef DrawSpace::Core::CallBack<BasicSceneMainFrame, void, DrawSpace::Core::PropertyPool&>    ScriptingCallsCallback;


    typedef DrawSpace::Core::CallBack<BasicSceneMainFrame, void, BasicSceneObjectPropertiesDialog*> DialogButtonCallback;

    typedef DrawSpace::Core::CallBack<BasicSceneMainFrame, void, DrawSpace::Core::BaseSceneNode*>   NodeUpdateBeginCallBack;


    static BasicSceneMainFrame*                                                             m_instance;

    long                                                                                    m_w_width;
    long                                                                                    m_w_height;

    bool                                                                                    m_glready;
    DrawSpace::Utils::TimeManager                                                           m_timer;
    
    RenderTimer*                                                                            m_rendertimer;
    //DrawSpace::Scenegraph                                                                   m_scenegraph;
    


    //////////////////////////////////////////////////////////////////////////////////

    std::map<void*, SceneNodeGraphEntry>                                                    m_scenenodegraphs;
    std::map<void*, WorldEntry>                                                             m_worlds;

    std::map<void*, DrawSpace::Core::BaseSceneNode*>                                        m_tree_nodes;
    std::map<DrawSpace::Core::BaseSceneNode*, void*>                                        m_inv_tree_nodes;

    std::map<void*, wxTreeItemId*>                                                          m_inv_treeitemid;
   
    std::map<void*, SceneNodeEntry<DrawSpace::Core::Transformation>>                        m_transformation_nodes;
    std::map<void*, SceneNodeEntry<DrawSpace::Spacebox>>                                    m_spacebox_nodes;
    std::map<void*, SceneNodeEntry<DrawSpace::Chunk>>                                       m_chunk_nodes;
    std::map<void*, SceneNodeEntry<DrawSpace::Dynamics::CameraPoint>>                       m_camera_nodes;
    std::map<void*, SceneNodeEntry<DrawSpace::Core::FPSMovement>>                           m_fps_nodes;
    std::map<void*, SceneNodeEntry<DrawSpace::Core::LinearMovement>>                        m_lin_nodes;
    std::map<void*, SceneNodeEntry<DrawSpace::Core::CircularMovement>>                      m_circ_nodes;
    std::map<void*, SceneNodeEntry<DrawSpace::Core::FreeMovement>>                          m_free_nodes;
    std::map<void*, SceneNodeEntry<DrawSpace::Core::LongLatMovement>>                       m_ll_nodes;

    std::map<void*, DrawSpace::Utils::SpaceboxDescriptor>                                   m_spacebox_descriptors;
    std::map<void*, DrawSpace::Utils::ChunkDescriptor>                                      m_chunk_descriptors;
    


    std::map<int, Action*>                                                                  m_actions;

    std::map<dsstring, ActionDialog*>                                                       m_actiondialogs_apply;
    std::map<dsstring, ActionDialog*>                                                       m_actiondialogs_specific0;
    std::map<dsstring, ActionDialog*>                                                       m_actiondialogs_specific1;

    std::map<dsstring, ActionScript*>                                                       m_actionscripts;

    std::map<void*, BasicSceneScriptEditFrame*>                                             m_script_edit_frames;

    std::map<void*, int>                                                                    m_menubuild_table;
  
    //////////////////////////////////////////////////////////////////////////////////

    std::vector<DrawSpace::Core::Configurable*>             m_ordered_configs;
    long                                                    m_scenegraphlistctrl_currentindex;
    long                                                    m_mvtslistctrl_currentindex;
    long                                                    m_cameraslistctrl_currentindex;
    long                                                    m_regslistctrl_currentindex;

    

    

    wxCoord                                                 m_last_xmouse;
    wxCoord                                                 m_last_ymouse;

    bool                                                    m_delta_mouse_init;

    DrawSpace::Dynamics::CameraPoint*                       m_current_camera;

    Scripting*                                              m_scripting;


    ScriptingErrorCallback*                                 m_scripting_error_cb;
    ScriptingCallsCallback*                                 m_scripting_calls_cb;

    bool                                                    m_display_framerate;
    bool                                                    m_display_currentcamera;
    SceneNodeGraphEntry                                     m_scenegraph_currentcameradisplay;

    wxFont                                                  m_console_font;

    wxTreeItemId                                            m_scenegraphs_root_item;
    wxTreeItemId                                            m_keyboard_item;
    wxTreeItemId                                            m_mouse_item;


    std::map<int, std::vector<PopupMenuEntry>>              m_scenegraphs_masks;

    wxTreeItemId                                            m_last_clicked_treeitem;


    DialogButtonCallback*                                   m_applybutton_clicked_cb;
    DialogButtonCallback*                                   m_specificbutton0_clicked_cb;
    DialogButtonCallback*                                   m_specificbutton1_clicked_cb;


    NodeUpdateBeginCallBack*                                m_nodeupdatebegin_cb;


    dsstring                                                m_keydown_script;
    bool                                                    m_keydown_script_enabled;

    int*                                                    m_keydown_code;

    dsstring                                                m_keyup_script;
    bool                                                    m_keyup_script_enabled;

    int*                                                    m_keyup_code;

    dsstring                                                m_mousemove_script;
    bool                                                    m_mousemove_script_enabled;

    DrawSpace::Utils::MouseMovementsDescriptor*             m_mousemove_descr;

    BasicSceneScriptEditFrame*                              m_mousemovescripteditionframe;

    BasicSceneScriptEditFrame*                              m_keydownscripteditionframe;
    BasicSceneScriptEditFrame*                              m_keyupscripteditionframe;


    void on_scripting_error( const dsstring& p_error );
    void on_scripting_calls( DrawSpace::Core::PropertyPool& p_propertypool );

    



    void build_popupmenu( int p_level, wxMenu& p_menu );

    wxArrayString get_passes_list( void );
    wxArrayString get_finalpasses_list( void );
    wxArrayString get_intermediatepasses_list( void );
    wxArrayString get_fx_list( void );
    wxArrayString get_textures_list( void );
    wxArrayString get_shaders_list( void );
    wxArrayString get_meshes_list( void );
    wxArrayString get_fonts_list( void );
    wxArrayString get_scenenodes_list( DrawSpace::Core::SceneNodeGraph* p_sc );
    wxArrayString insert_void_choice( const wxArrayString& p_array );



    void build_passes_infos_dialog( DrawSpace::Core::Configurable* p_config );

    void* find_scenenodegraph_id( wxTreeItemId p_item );

    void render( void );

    void on_applybutton_clicked( BasicSceneObjectPropertiesDialog* p_dialog );
    void on_specificbutton0_clicked( BasicSceneObjectPropertiesDialog* p_dialog );
    void on_specificbutton1_clicked( BasicSceneObjectPropertiesDialog* p_dialog );


    void on_nodeupdatebegin( DrawSpace::Core::BaseSceneNode* p_node );

    virtual void OnPaint( wxPaintEvent& p_event );
    virtual void OnClose( wxCloseEvent& p_event );
    virtual void OnIdle( wxIdleEvent& p_event );
	virtual void OnMouseMotion( wxMouseEvent& p_event );
    virtual void OnAssetsListItemActivated( wxListEvent& p_event );
    virtual void OnConfigsListItemActivated( wxListEvent& p_event );
    virtual void OnPassesListItemActivated( wxListEvent& p_event );
    virtual void OnPassesListItemSelected( wxListEvent& p_event );
    virtual void OnShadersListItemActivated( wxListEvent& p_event );

    
    virtual void OnMouseLeftDown( wxMouseEvent& event );
	virtual void OnMouseLeftUp( wxMouseEvent& event );
	virtual void OnMouseRightDown( wxMouseEvent& event );
	virtual void OnMouseRightUp( wxMouseEvent& event );

    
   
    virtual void OnPopupClick(wxCommandEvent& p_evt);
    virtual void OnSceneNodeGraphsListRightClick( wxTreeEvent& p_event );

    virtual void OnConsoleInSendButtonClicked( wxCommandEvent& p_event );
    virtual void OnConsoleInClearButtonClicked( wxCommandEvent& p_event );
    virtual void OnConsoleInLoadButtonClicked( wxCommandEvent& p_event );


    wxTreeItemId searchTreeItemIdInNodes( void* p_id );

    BasicSceneMainFrame( wxWindow* parent );

public:

	static BasicSceneMainFrame* GetInstance( void );

    void SetGLReady( void );
    void Update( void );
    wxNotebook* GetNoteBook( void );

    void SetWindowDims( long p_w_width, long p_w_height );


    void ExecStartupScript( const dsstring& p_scriptfilepath );
    void PrintOutputConsole( const dsstring& p_text );

    void OnKeyDown( wxKeyEvent& p_event );
	void OnKeyUp( wxKeyEvent& p_event );

    wxTreeItemId AppendItem( const wxTreeItemId& p_parent_item, const wxString& p_text, int p_image );

    void SetItemImage( const wxTreeItemId& p_item, int p_image );

    friend class RenderTimer;
};

#endif // __BasicSceneMainFrame__
