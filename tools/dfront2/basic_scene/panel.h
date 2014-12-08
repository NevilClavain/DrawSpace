///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun  5 2014)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __PANEL_H__
#define __PANEL_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/listctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/statbox.h>
#include <wx/combobox.h>
#include <wx/stattext.h>
#include <wx/panel.h>
#include <wx/notebook.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ObjectsListDialog
///////////////////////////////////////////////////////////////////////////////
class ObjectsListDialog : public wxDialog 
{
	private:
	
	protected:
		wxListCtrl* m_listCtrl;
		wxButton* m_button4;
	
	public:
		
		ObjectsListDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 537,388 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~ObjectsListDialog();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class ObjectPropertiesDialog
///////////////////////////////////////////////////////////////////////////////
class ObjectPropertiesDialog : public wxDialog 
{
	private:
	
	protected:
		wxPropertyGrid* m_propertyGrid;
		wxButton* m_close_button;
		wxButton* m_apply_button;
		wxButton* m_specific_button_0;
		wxButton* m_specific_button_1;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCloseButtonClicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnApplyButtonClicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSpecificButton0Clicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSpecificButton1Clicked( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		ObjectPropertiesDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 476,318 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~ObjectPropertiesDialog();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class MainFrame
///////////////////////////////////////////////////////////////////////////////
class MainFrame : public wxFrame 
{
	private:
	
	protected:
		wxNotebook* m_notebook2;
		wxPanel* m_camerasPanel;
		wxListCtrl* m_cameras_listCtrl;
		wxButton* m_cameraedit_button;
		wxButton* m_createcamera_button;
		wxListCtrl* m_mvts_listCtrl;
		wxButton* m_control_button;
		wxButton* m_createmvt_button;
		wxComboBox* m_mvttype_comboBox;
		wxStaticText* m_staticText1;
		wxComboBox* m_mousekeyboardoutput_comboBox;
		wxPanel* m_passesPanel;
		wxListCtrl* m_passes_listCtrl;
		wxListCtrl* m_shadersparams_listCtrl;
		wxPanel* m_resourcesPanel;
		wxListCtrl* m_assets_listCtrl;
		wxListCtrl* m_configs_listCtrl;
		wxPanel* m_scenePanel;
		wxListCtrl* m_scenegraph_listCtrl;
		wxButton* m_transftype_button;
		wxButton* m_transfoedit_button;
		wxButton* m_setcamera_button;
		wxComboBox* m_cameraslist_comboBox;
		wxButton* m_create_drawable_button;
		wxComboBox* m_drawabletypes_comboBox;
		wxPanel* m_registersPanel;
		wxListCtrl* m_registers_listCtrl;
		wxButton* m_modreg_button;
		wxButton* m_resetreg_button;
		wxButton* m_regon_button;
		wxButton* m_regoff_button;
		wxButton* m_createreg_button;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnIdle( wxIdleEvent& event ) { event.Skip(); }
		virtual void OnKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnKeyUp( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnMouseMotion( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnCamerasListDeleteAllItems( wxListEvent& event ) { event.Skip(); }
		virtual void OnCamerasListItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnCamerasListItemSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnCameraEditButtonClicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCreateCameraButtonClicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnMvtsListDeleteAllItems( wxListEvent& event ) { event.Skip(); }
		virtual void OnMvtsListItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnMvtsListItemSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnControlButtonClicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCreateMvtButtonClicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnMouseKeyboardOutputCombobox( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnPassesListItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnPassesListItemSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnShadersListItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnAssetsListItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnConfigsListItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnScenegraphListDeleteAllItems( wxListEvent& event ) { event.Skip(); }
		virtual void OnScenegraphListDeleteItem( wxListEvent& event ) { event.Skip(); }
		virtual void OnScenegraphItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnScenegraphListItemDeselected( wxListEvent& event ) { event.Skip(); }
		virtual void OnScenegraphListItemSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnTransfTypeButtonClicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTransfoEditButtonClicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSetCameraButtonClicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCreateDrawableButtonClicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRegsListDeleteAllItems( wxListEvent& event ) { event.Skip(); }
		virtual void OnRegistersListItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnRegsListItemSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnModRegButtonClicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnResetRegButtonClicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRegOnButtonClicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRegOffButtonClicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCreateRegButtonClicked( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		MainFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("DFront Basic Scene"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1164,600 ), long style = wxCAPTION|wxCLOSE_BOX|wxTAB_TRAVERSAL );
		
		~MainFrame();
	
};

#endif //__PANEL_H__
