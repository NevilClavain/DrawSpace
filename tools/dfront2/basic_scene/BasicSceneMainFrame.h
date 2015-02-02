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


#include "panel.h"
#include "drawspace.h"
#include "scripting.h"

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


#define CONTEXTMENU_NEWSCENENODEGRAPH       2000

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


class BasicSceneMainFrame : public MainFrame
{
public:

    typedef struct
    {
        int         id;
        dsstring    text;

    } PopupMenuEntry;

    typedef enum
    {
        TRANSFORMATIONSOURCE_MATRIXSTACK,
        TRANSFORMATIONSOURCE_MOVEMENT,
        TRANSFORMATIONSOURCE_BODY

    } TransformationSourceType;

    typedef enum
    {
        TRANSFORMATIONMATRIX_IDENTITY,
        TRANSFORMATIONMATRIX_SCALE,
        TRANSFORMATIONMATRIX_TRANSLATION,
        TRANSFORMATIONMATRIX_ROTATION

    } TransformationMatrixOperation;

    typedef struct
    {
        dsstring    var_alias;
        dsreal      value;          // si var_alias = ""

    } TransformationMatrixValueLinkage;

    typedef struct
    {
        //DrawSpace::Utils::Vector                translation;
        TransformationMatrixValueLinkage        translation_vals_link[3];

        //DrawSpace::Utils::Vector                rotation;
        TransformationMatrixValueLinkage        rotation_vals_link[3];

        //dsreal                                  angle;
        TransformationMatrixValueLinkage        angle_val_link;

        //DrawSpace::Utils::Vector                scale;
        TransformationMatrixValueLinkage        scale_vals_link[3];

    } TransformationMatrixArg;

    typedef struct
    {
        TransformationMatrixOperation   ope;
        TransformationMatrixArg         arg;

    } TransformationMatrixDescriptor;


    typedef struct
    {
        DrawSpace::Core::TransformNode*                 node;
        TransformationSourceType                        transformation_source_type;

        // les sources de transfo autorisees pour ce node
        bool                                            propose_matrixstack;
        bool                                            propose_movement;
        bool                                            propose_body;
        
        //DrawSpace::Utils::Transformation                matrix_stack;
        std::vector<TransformationMatrixDescriptor>     matrix_stack_descr;
        DrawSpace::Core::Movement*                      movement;
        DrawSpace::Dynamics::Body*                      body;


    } MetadataScenegraphEntry;



    typedef struct
    {
        dsstring                            name;
        DrawSpace::Core::SceneNodeGraph*    scenenodegraph;        
        wxTreeItemId                        treeitemid;

    } SceneNodeGraphEntry;


    typedef enum
    {
        MOVEMENTCONTROLSOURCE_KEYBMOUSE,
        MOVEMENTCONTROLSOURCE_REGISTER

    } MovementControlSource;


    typedef struct
    {
        DrawSpace::Core::Movement*  movement;

        MovementControlSource       speed_control_source;
        dsstring                    speed_control_register;

        MovementControlSource       yaw_control_source;
        dsstring                    yaw_control_register;

        MovementControlSource       pitch_control_source;
        dsstring                    pitch_control_register;

        MovementControlSource       roll_control_source;
        dsstring                    roll_control_register;

        dsreal                      theta_pos_mouse;
        dsreal                      phi_pos_mouse;

    } MovementEntry;


    typedef enum
    {
        REGISTER_CONSTANT,
        REGISTER_VARIABLE

    } RegisterMode;

    typedef enum
    {
        REGISTER_VARIABLE_TRANSLATION_SIMPLE,
        REGISTER_VARIABLE_TRANSLATION_ROUNDTRIP,
        REGISTER_VARIABLE_ANGULAR_SIMPLE,
        REGISTER_VARIABLE_ANGULAR_ROUNDTRIP,

    } RegisterVariableMode;


    typedef struct
    {
        long                    id;
        RegisterMode            mode;
        dsreal                  const_value;

        RegisterVariableMode    variable_mode;
        dsreal                  variable_initial_value;
        dsreal                  variable_speed;
        dsreal                  variable_range_sup;
        dsreal                  variable_range_inf;

        bool                    state;

        dsreal                  current_value;

       
        bool                    variable_roundtrip_back;
        bool                    discontinuity;

    } RegisterEntry;


protected:

    typedef DrawSpace::Core::CallBack<BasicSceneMainFrame, void, const dsstring&>                   TimerCallback;
    typedef DrawSpace::Core::CallBack<BasicSceneMainFrame, void, const dsstring&>                   ScriptingErrorCallback;

    typedef DrawSpace::Core::CallBack<BasicSceneMainFrame, void, DrawSpace::Core::PropertyPool&>    ScriptingCallsCallback;


    void on_timer( const dsstring& p_timername );
    void on_scripting_error( const dsstring& p_error );
    void on_scripting_calls( DrawSpace::Core::PropertyPool& p_propertypool );

    void compute_scenegraph_transforms( void );
    void compute_regs( void );
    void compute_movements( void );

    bool set_var_alias( const dsstring& p_source, dsstring& p_dest );

    void build_popupmenu( int p_level, wxMenu& p_menu );


    long                                                    m_w_width;
    long                                                    m_w_height;

    bool                                                    m_glready;
    DrawSpace::Utils::TimeManager                           m_timer;
    std::map<dsstring, MovementEntry>                       m_movements;

    DrawSpace::Scenegraph                                   m_scenegraph;
    std::map<dsstring, MetadataScenegraphEntry>             m_metada_scenegraph;

    std::map<void*, SceneNodeGraphEntry>                    m_scenenodegraphs;






    std::vector<DrawSpace::Core::Configurable*>             m_ordered_configs;
    long                                                    m_scenegraphlistctrl_currentindex;
    long                                                    m_mvtslistctrl_currentindex;
    long                                                    m_cameraslistctrl_currentindex;
    long                                                    m_regslistctrl_currentindex;

    std::map<dsstring, RegisterEntry>                       m_registers;

    TimerCallback*                                          m_timercb;

    wxCoord                                                 m_last_xmouse;
    wxCoord                                                 m_last_ymouse;

    DrawSpace::Dynamics::CameraPoint*                       m_current_camera;

    Scripting*                                              m_scripting;


    ScriptingErrorCallback*                                 m_scripting_error_cb;
    ScriptingCallsCallback*                                 m_scripting_calls_cb;

    bool                                                    m_display_framerate;
    bool                                                    m_display_currentcamera;

    wxFont                                                  m_console_font;

    wxTreeItemId                                            m_scenegraphs_root_item;


    std::map<int, std::vector<PopupMenuEntry>>              m_scenegraphs_masks;



    virtual void OnClose( wxCloseEvent& p_event );
    virtual void OnIdle( wxIdleEvent& p_event );
	virtual void OnKeyDown( wxKeyEvent& p_event );
	virtual void OnKeyUp( wxKeyEvent& p_event );
	virtual void OnMouseMotion( wxMouseEvent& p_event );
    virtual void OnAssetsListItemActivated( wxListEvent& p_event );
    virtual void OnConfigsListItemActivated( wxListEvent& p_event );
    virtual void OnPassesListItemActivated( wxListEvent& p_event );
    virtual void OnPassesListItemSelected( wxListEvent& p_event );
    virtual void OnShadersListItemActivated( wxListEvent& p_event );
    virtual void OnCreateMvtButtonClicked( wxCommandEvent& p_event );
    virtual void OnMvtsListItemActivated( wxListEvent& p_event );
    virtual void OnMvtsListDeleteAllItems( wxListEvent& p_event );
    virtual void OnMvtsListItemSelected( wxListEvent& p_event );
    virtual void OnCreateCameraButtonClicked( wxCommandEvent& p_event );
    virtual void OnCamerasListItemActivated( wxListEvent& p_event );
    virtual void OnCamerasListDeleteAllItems( wxListEvent& p_event );
    virtual void OnCamerasListItemSelected( wxListEvent& p_event );
    virtual void OnScenegraphItemActivated( wxListEvent& p_event );
    virtual void OnScenegraphListItemSelected( wxListEvent& p_event );
    virtual void OnCreateDrawableButtonClicked( wxCommandEvent& p_event );
    virtual void OnTransfTypeButtonClicked( wxCommandEvent& p_event );
	virtual void OnTransfoEditButtonClicked( wxCommandEvent& p_event );
    virtual void OnScenegraphListItemDeselected( wxListEvent& p_event );
    virtual void OnScenegraphListDeleteItem( wxListEvent& p_event );
    virtual void OnScenegraphListDeleteAllItems( wxListEvent& p_event );

    virtual void OnPopupClick(wxCommandEvent& p_evt);
    virtual void OnSceneNodeGraphsListRightClick( wxTreeEvent& p_event );

	virtual void OnControlButtonClicked( wxCommandEvent& p_event );
    virtual void OnMouseKeyboardOutputCombobox( wxCommandEvent& p_event );
    virtual void OnCreateRegButtonClicked( wxCommandEvent& p_event );
    virtual void OnCameraEditButtonClicked( wxCommandEvent& p_event );
    virtual void OnRegistersListItemActivated( wxListEvent& p_event );
	virtual void OnRegsListDeleteAllItems( wxListEvent& p_event );
	virtual void OnRegsListItemSelected( wxListEvent& p_event );
	virtual void OnModRegButtonClicked( wxCommandEvent& p_event );
    virtual void OnResetRegButtonClicked( wxCommandEvent& p_event );
    virtual void OnRegOnButtonClicked( wxCommandEvent& p_event );
    virtual void OnRegOffButtonClicked( wxCommandEvent& p_event );
    virtual void OnConsoleInSendButtonClicked( wxCommandEvent& p_event );
    virtual void OnConsoleInClearButtonClicked( wxCommandEvent& p_event );
    virtual void OnConsoleInLoadButtonClicked( wxCommandEvent& p_event );


public:
	BasicSceneMainFrame( wxWindow* parent );

    void SetGLReady( void );
    void Update( void );
    wxNotebook* GetNoteBook( void );

    void SetWindowDims( long p_w_width, long p_w_height );


    void ExecStartupScript( const dsstring& p_scriptfilepath );
    void PrintOutputConsole( const dsstring& p_text );

    bool RegisterMovement( const dsstring& p_name, const MovementEntry& p_movement );
};

#endif // __BasicSceneMainFrame__
