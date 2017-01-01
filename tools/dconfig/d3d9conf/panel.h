/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
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
		virtual void OnMainDialogClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnSaveButtonClick( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		MainDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("..."), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 793,303 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~MainDialog();
	
};

#endif //__PANEL_H__
