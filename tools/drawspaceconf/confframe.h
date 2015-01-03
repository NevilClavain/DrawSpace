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

#ifndef _DXCONFFRAME_H_
#define _DXCONFFRAME_H_

#include <wx/wx.h>
#include <wx/notebook.h>
#include <d3dx9.h>
#include <vector>

#define D3D9_RELEASE( p_Obj ) { if( p_Obj ) { p_Obj->Release(); p_Obj = NULL; } }

class ConfFrame : public wxFrame
{
protected:
	DECLARE_EVENT_TABLE()

	wxBoxSizer*								m_rootsizer;
	wxBoxSizer*								m_dx9tabsizer;
	wxBoxSizer*								m_windowtabsizer;
	wxBoxSizer*								m_windowtabsizer2;

	wxButton*								m_button_save;

	wxNotebook*								m_notebook;
	wxWindow*								m_windowtab;
	wxWindow*								m_dx9tab;

	wxComboBox*								m_combo_windowres;
	wxCheckBox*								m_checkbox_fullscreen;
	wxComboBox*								m_combo_renderplugin;
	wxStaticText*							m_statictext_renderplugin;

	wxComboBox*								m_combo_dx9adapters;
	wxStaticText*							m_statictext_adapterdescr;
	wxListBox*								m_listbox_adaptermodes;
	wxStaticText*							m_statictext_selectedmode;

	wxStaticText*							m_statictext_vertexproc;
	wxComboBox*								m_combo_vertexproc;


	LPDIRECT3D9								m_lpd3d;

	typedef struct
	{
		UINT width; 
		UINT height;
		UINT refresh_rate;
		D3DFORMAT format;
		std::string comment;

	} adapter_mode;

	typedef struct
	{
		D3DADAPTER_IDENTIFIER9		infos;
		std::vector<adapter_mode>	modes;
		std::vector<D3DFORMAT>		m_zbufferformats;

	} adapter_infos;

	bool									m_fullscreen;


	std::vector<adapter_infos>				m_adapters_infos;

	void update_d3d9tab( long p_adapter_index );

	void search_available_zbuffersformats( void );

public:

	ConfFrame( const wxString& p_caption );
	virtual ~ConfFrame( void );

	void OnIdle( wxIdleEvent& p_event );
	void OnClose( wxCloseEvent& p_event );
	void OnCheckBoxFullscreen( wxCommandEvent& p_event );
	void OnButtonSave( wxCommandEvent& p_event );
	void OnComboDx9Adapters( wxCommandEvent& p_event );
	void OnListBoxDx9Modes( wxCommandEvent& p_event );
};


#endif
