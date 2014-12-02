/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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

class BasicSceneMainFrame : public MainFrame
{
public:

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
        DrawSpace::Utils::Vector translation;

        DrawSpace::Utils::Vector rotation;
        dsreal                   angle;

        DrawSpace::Utils::Vector scale;

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
        
        DrawSpace::Utils::Transformation                matrix_stack;
        DrawSpace::Core::Movement*                      movement;
        DrawSpace::Dynamics::Body*                      body;

        // descriptions source de transformation

        // cas matrix_stack
        std::vector<TransformationMatrixDescriptor>     matrix_stack_descr;

        // cas movement

        // cas body

    } MetadataScenegraphEntry;


    typedef enum
    {
        MOVEMENTCONTROLSOURCE_KEYBMOUSE,
        MOVEMENTCONTROLSOURCE_AUTOMATIC

    } MovementControlSource;

    typedef enum
    {
        THETAPHICONTROL_POSITION,
        THETAPHICONTROL_SPEED,

    } ThetaPhiControlMode;

    typedef struct
    {
        DrawSpace::Core::Movement*  movement;
        MovementControlSource       movement_control_source;

        dsreal                      speed;
        ThetaPhiControlMode         thetaphi_control_mode;

        dsreal                      theta;
        dsreal                      phi;

        dsreal                      yaw_speed;
        dsreal                      pitch_speed;
        dsreal                      roll_speed;


    } MovementEntry;


protected:
    bool                                                    m_glready;
    DrawSpace::Utils::TimeManager                           m_timer;
    //std::map<dsstring, DrawSpace::Core::Movement*>          m_movements;
    std::map<dsstring, MovementEntry>                       m_movements;

    DrawSpace::Scenegraph                                   m_scenegraph;
    std::map<dsstring, MetadataScenegraphEntry>             m_metada_scenegraph;

    std::vector<DrawSpace::Core::Configurable*>             m_ordered_configs;
    long                                                    m_scenegraphlistctrl_currentindex;


    MovementEntry*                                          m_mousekeyb_output;

    

    virtual void OnClose( wxCloseEvent& event );
    virtual void OnIdle( wxIdleEvent& event );
    virtual void OnAssetsListItemActivated( wxListEvent& p_event );
    virtual void OnConfigsListItemActivated( wxListEvent& p_event );
    virtual void OnPassesListItemActivated( wxListEvent& p_event );
    virtual void OnPassesListItemSelected( wxListEvent& p_event );
    virtual void OnShadersListItemActivated( wxListEvent& p_event );
    virtual void OnCreateMvtButtonClicked( wxCommandEvent& p_event );
    virtual void OnMvtsListItemActivated( wxListEvent& p_event );
    virtual void OnCreateCameraButtonClicked( wxCommandEvent& p_event );
    virtual void OnCamerasListItemActivated( wxListEvent& p_event );
    virtual void OnScenegraphItemActivated( wxListEvent& p_event );
    virtual void OnScenegraphListItemSelected( wxListEvent& p_event );
    virtual void OnSetCameraButtonClicked( wxCommandEvent& p_event );
    virtual void OnCreateDrawableButtonClicked( wxCommandEvent& p_event );
    virtual void OnTransfTypeButtonClicked( wxCommandEvent& p_event );
	virtual void OnTransfoEditButtonClicked( wxCommandEvent& p_event );
    virtual void OnScenegraphListItemDeselected( wxListEvent& p_event );
    virtual void OnScenegraphListDeleteItem( wxListEvent& p_event );
    virtual void OnScenegraphListDeleteAllItems( wxListEvent& p_event );
	virtual void OnControlButtonClicked( wxCommandEvent& p_event );
	virtual void OnMouseKeyboardOutputButtonClicked( wxCommandEvent& p_event );



public:
	BasicSceneMainFrame( wxWindow* parent );

    void SetGLReady( void );
    void Update( void );
    wxNotebook* GetNoteBook( void );
	
};

#endif // __BasicSceneMainFrame__
