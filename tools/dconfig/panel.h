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

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class MainDialog
///////////////////////////////////////////////////////////////////////////////
class MainDialog : public wxDialog 
{
	private:
	
	protected:
		wxPropertyGrid* m_propertyGrid;
		wxButton* m_buttonSave;
		
		// Virtual event handlers, overide them in your derived class
		virtual void MainDialogOnClose( wxCloseEvent& event ) { event.Skip(); }
		
	
	public:
		
		MainDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("..."), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 442,471 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~MainDialog();
	
};

#endif //__PANEL_H__
