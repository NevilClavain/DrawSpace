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
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/listctrl.h>
#include <wx/panel.h>
#include <wx/treectrl.h>
#include <wx/notebook.h>
#include <wx/frame.h>
#include <wx/checkbox.h>

///////////////////////////////////////////////////////////////////////////


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
		wxButton* m_specific_button_2;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCloseButtonClicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnApplyButtonClicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSpecificButton0Clicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSpecificButton1Clicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSpecificButton2Clicked( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		ObjectPropertiesDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 563,318 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~ObjectPropertiesDialog();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class MainFrame
///////////////////////////////////////////////////////////////////////////////
class MainFrame : public wxFrame 
{
	private:
	
	protected:
		wxTextCtrl* m_consoleinput_textCtrl;
		wxButton* m_consoleinsend_button;
		wxButton* m_consoleinclear_button;
		wxButton* m_consoleinload_button;
		wxNotebook* m_notebook2;
		wxPanel* m_resourcesPanel;
		wxListCtrl* m_assets_listCtrl;
		wxListCtrl* m_configs_listCtrl;
		wxPanel* m_passesPanel;
		wxListCtrl* m_passes_listCtrl;
		wxListCtrl* m_shadersparams_listCtrl;
		wxPanel* m_scenePanel;
		wxTreeCtrl* m_scenegraphs_treeCtrl;
		wxTextCtrl* m_consoleoutput_textCtrl;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnIdle( wxIdleEvent& event ) { event.Skip(); }
		virtual void OnMouseLeftDown( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnMouseLeftUp( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnMouseMotion( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnPaint( wxPaintEvent& event ) { event.Skip(); }
		virtual void OnMouseRightDown( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnMouseRightUp( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnConsoleInSendButtonClicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnConsoleInClearButtonClicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnConsoleInLoadButtonClicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAssetsListItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnConfigsListItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnPassesListItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnPassesListItemSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnShadersListItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnSceneNodeGraphsListRightClick( wxTreeEvent& event ) { event.Skip(); }
		
	
	public:
		
		MainFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("DrawFront"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1300,600 ), long style = wxCAPTION|wxCLOSE_BOX|wxTAB_TRAVERSAL );
		
		~MainFrame();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class ScriptEditFrame
///////////////////////////////////////////////////////////////////////////////
class ScriptEditFrame : public wxFrame 
{
	private:
	
	protected:
		wxTextCtrl* m_script_textCtrl;
		wxButton* m_close_button;
		wxButton* m_apply_button;
		wxButton* m_load_button;
		wxButton* m_clear_button;
		wxCheckBox* m_activation_checkBox;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCloseButtonClicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnApplyButtonClicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnLoadButtonClicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClearButtonClicked( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		ScriptEditFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxCAPTION|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxRESIZE_BORDER|wxSYSTEM_MENU|wxTAB_TRAVERSAL );
		
		~ScriptEditFrame();
	
};

#endif //__PANEL_H__
