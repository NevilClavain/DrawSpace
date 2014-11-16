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
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCloseButtonClicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnApplyButtonClicked( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		ObjectPropertiesDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 381,318 ), long style = wxDEFAULT_DIALOG_STYLE ); 
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
		wxListCtrl* m_mvts_listCtrl;
		wxButton* m_createmvt_button;
		wxComboBox* m_mvttype_comboBox;
		wxPanel* m_passesPanel;
		wxListCtrl* m_passes_listCtrl;
		wxListCtrl* m_shadersparams_listCtrl;
		wxPanel* m_resourcesPanel;
		wxListCtrl* m_assets_listCtrl;
		wxListCtrl* m_configs_listCtrl;
		wxPanel* m_scenePanel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnIdle( wxIdleEvent& event ) { event.Skip(); }
		virtual void OnCreateMvtButtonClicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnPassesListItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnPassesListItemSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnShadersListItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnAssetsListItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnConfigsListItemActivated( wxListEvent& event ) { event.Skip(); }
		
	
	public:
		
		MainFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("DFront Basic Scene"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1164,600 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		
		~MainFrame();
	
};

#endif //__PANEL_H__
