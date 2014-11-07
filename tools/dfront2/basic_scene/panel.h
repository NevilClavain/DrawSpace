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
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/radiobut.h>
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/listbox.h>
#include <wx/notebook.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class Panel
///////////////////////////////////////////////////////////////////////////////
class Panel : public wxDialog 
{
	private:
	
	protected:
		wxNotebook* m_notebook1;
		wxPanel* m_camerasPanel;
		wxListCtrl* m_listCtrl1;
		wxButton* m_deleteCameraButton;
		wxStaticText* m_staticText1;
		wxTextCtrl* m_textCtrl1;
		wxStaticText* m_staticText2;
		wxComboBox* m_comboBox2;
		wxStaticText* m_staticText3;
		wxComboBox* m_comboBox3;
		wxStaticText* m_staticText4;
		wxComboBox* m_comboBox4;
		wxStaticText* m_staticText5;
		wxRadioButton* m_radioBtn6;
		wxRadioButton* m_radioBtn7;
		wxComboBox* m_comboBox6;
		wxButton* m_button6;
		wxPanel* m_passesPanel;
		wxListBox* m_listBox2;
		wxPanel* m_resourcesPanel;
		wxPanel* m_scenePanel;
	
	public:
		
		Panel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Basic_scene commands"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 565,644 ), long style = wxCAPTION ); 
		~Panel();
	
};

#endif //__PANEL_H__
