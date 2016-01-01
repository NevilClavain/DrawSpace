/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2016                              
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

#include "confframe.h"
#include "resources.h"

BEGIN_EVENT_TABLE( ConfFrame, wxFrame )
    EVT_IDLE( ConfFrame::OnIdle )
	EVT_CLOSE( ConfFrame::OnClose )
	EVT_CHECKBOX( wxID_CHECKBOX_FULLSCREEN, ConfFrame::OnCheckBoxFullscreen )
	EVT_BUTTON( wxID_BUTTON_SAVE, ConfFrame::OnButtonSave )
	EVT_COMBOBOX( wxID_COMBO_DX9ADAPTERS, ConfFrame::OnComboDx9Adapters )
	EVT_LISTBOX( wxID_LISTBOX_DX9ADAPTERMODES, ConfFrame::OnListBoxDx9Modes )
END_EVENT_TABLE()

ConfFrame::ConfFrame( const wxString& p_caption ) : 
wxFrame( NULL, wxID_ANY, p_caption, wxDefaultPosition, wxSize( 300, 400 ), wxMINIMIZE_BOX | wxCLOSE_BOX | wxSYSTEM_MENU | wxCAPTION ), 
m_fullscreen( false )
{
	///////////////////////////////////////////////////////////////////////////////////////////////
	m_notebook = new wxNotebook( this, wxID_NOTEBOOK, wxPoint( 0, 0 ), wxSize( 300, 340 ), wxNB_TOP | /*wxNB_LEFT |*/ wxNB_FIXEDWIDTH );
	m_rootsizer = new wxBoxSizer( wxVERTICAL );
	m_dx9tabsizer = new wxBoxSizer( wxVERTICAL );
	m_windowtabsizer = new wxBoxSizer( wxVERTICAL );
	m_windowtabsizer2 = new wxBoxSizer( wxHORIZONTAL );

	m_windowtab = new wxWindow( m_notebook, wxID_TAB_WINDOW );
	m_dx9tab = new wxWindow( m_notebook, wxID_TAB_DX9 );

	///////////////////////////////////////////////////////////////////////////////////////////////	

	
	////// Window tab //////////////////////////////////////////////////
	
	m_windowtab->SetSizer( m_windowtabsizer );
	m_notebook->AddPage( m_windowtab, _( "Window" ), true );

	//m_windowtabsizer->Add( m_windowtabsizer2 );

	wxString windowres_cb[7] = { _("512 x 380"), _("640 x 400"), _("640 x 480"), _("800 x 600"), _("1024 x 768"), _("1280 x 960"), _("1440 x 900") };
	m_combo_windowres = new wxComboBox( m_windowtab, wxID_COMBO_WINDOWRES, _("512 x 380"), wxPoint( 0, 30 ), wxSize( 170, 30 ), 7, windowres_cb, wxCB_READONLY );
	//m_windowtabsizer2->Add( m_combo_windowres, 0, wxALL, 0 );

	m_checkbox_fullscreen = new wxCheckBox( m_windowtab, wxID_CHECKBOX_FULLSCREEN,  _("Enable fullscreen"), wxPoint( 180, 30 ), wxSize( 170, 30 ) );
	//m_windowtabsizer2->Add( m_checkbox_fullscreen, 0, wxALL, 0 );

	m_statictext_renderplugin = new wxStaticText( m_windowtab, wxID_STATICTEXT_RENDERPLUGIN, _("Render plugin:"), wxPoint( 0, 80 ), wxSize( 90, 20 ) );
	//m_windowtabsizer2->Add( m_statictext_renderplugin, 0, wxALL, 0 );

	wxString plugin_cb[6] = { _("DirectX 9") };
	m_combo_renderplugin = new wxComboBox( m_windowtab, wxID_COMBO_RENDERPLUGIN, _("DirectX 9"), wxPoint( 100, 80 ), wxSize( 140, 20 ), 1, plugin_cb, wxCB_READONLY );
	//m_windowtabsizer->Add( m_combo_renderplugin, 0, wxALL, 0 );

	////// Dx9 tab //////////////////////////////////////////////////////

	m_dx9tab->SetSizer( m_dx9tabsizer );
	m_notebook->AddPage( m_dx9tab, _( "DirectX9" ), true );

	m_combo_dx9adapters = new wxComboBox( m_dx9tab, wxID_COMBO_DX9ADAPTERS, _(""), wxPoint( 0, 30 ), wxSize( 170, 30 ), 0, NULL, wxCB_READONLY );
	//m_dx9tabsizer->Add( m_combo_dx9adapters, 0, wxALL, 0 );

	m_statictext_adapterdescr = new wxStaticText( m_dx9tab, wxID_STATICTEXT_DX9ADAPTERDESCR, _(""), wxPoint( 0, 60 ), wxSize( 170, 30 ) );
	//m_dx9tabsizer->Add( m_statictext_adapterdescr, 0, wxALL, 0 );

	m_listbox_adaptermodes = new wxListBox( m_dx9tab, wxID_LISTBOX_DX9ADAPTERMODES, wxPoint( 0, 90 ), wxSize( 250, 100 ) );
	//m_dx9tabsizer->Add( m_listbox_adaptermodes, 0, wxALL, 0 );

	m_statictext_selectedmode = new wxStaticText( m_dx9tab, wxID_STATICTEXT_SELECTEDMODE, _(""), wxPoint( 0, 200 ), wxSize( 170, 30 ) );
	m_statictext_selectedmode->SetLabel( "Selected mode : none" );
	//m_dx9tabsizer->Add( m_statictext_selectedmode, 0, wxALL, 0 );

	m_listbox_adaptermodes->Disable();
	m_statictext_selectedmode->Disable();

	m_statictext_vertexproc = new wxStaticText( m_dx9tab, wxID_STATICTEXT_VERTEXPROC, _("Vertex processing:"), wxPoint( 0, 240 ), wxSize( 110, 30 ) );
	//m_dx9tabsizer->Add( m_statictext_vertexproc, 0, wxALL, 0 );

	wxString vertexproc_cb[2] = { _("Software"), _("Hardware") };
	m_combo_vertexproc = new wxComboBox( m_dx9tab, wxID_COMBO_VERTEXPROC, _("Software"), wxPoint( 110, 240 ), wxSize( 140, 30 ), 2, vertexproc_cb, wxCB_READONLY );
	m_combo_vertexproc->SetSelection( 1 );

	///////////////////////////////////////////////////////////////////////

	m_rootsizer->Add( m_notebook, 0, wxALL, 0 );
	m_button_save = new wxButton( this, wxID_BUTTON_SAVE, _( "Save" ), wxPoint( 0, 170 ), wxSize( 60, 25 ) ); 
	m_rootsizer->Add( m_button_save, 0, wxALL, 0 );

	SetSizer( m_rootsizer );

	m_notebook->SetSelection( 0 );

	m_lpd3d = Direct3DCreate9( D3D_SDK_VERSION );

	UINT nb_adapters = m_lpd3d->GetAdapterCount();

	for( UINT i = 0; i < nb_adapters; i++ )
	{
		adapter_infos infos;
		adapter_mode  mode;

		D3DADAPTER_IDENTIFIER9 current;
		m_lpd3d->GetAdapterIdentifier( i, 0, &current );
	
		infos.infos = current;

		//
		D3DDISPLAYMODE current_dm;

		UINT nb_modes;
		
		nb_modes = m_lpd3d->GetAdapterModeCount( i, D3DFMT_A1R5G5B5 );
		if( nb_modes > 0 )
		{
			for( UINT j = 0; j < nb_modes; j++ )
			{
				m_lpd3d->EnumAdapterModes( i, D3DFMT_A1R5G5B5, j, &current_dm );

				mode.format = D3DFMT_A1R5G5B5;
				mode.width = current_dm.Width;
				mode.height = current_dm.Height;
				mode.refresh_rate = current_dm.RefreshRate;

				char comment[256];
				sprintf( comment, "D3DFMT_A1R5G5B5 %d x %d, %d Hz", current_dm.Width, current_dm.Height, current_dm.RefreshRate );
				mode.comment = comment;

				infos.modes.push_back( mode );
			}
		}


		nb_modes = m_lpd3d->GetAdapterModeCount( i, D3DFMT_A2R10G10B10 );
		if( nb_modes > 0 )
		{
			for( UINT j = 0; j < nb_modes; j++ )
			{
				m_lpd3d->EnumAdapterModes( i, D3DFMT_A2R10G10B10, j, &current_dm );

				mode.format = D3DFMT_A2R10G10B10;
				mode.width = current_dm.Width;
				mode.height = current_dm.Height;
				mode.refresh_rate = current_dm.RefreshRate;

				char comment[256];
				sprintf( comment, "D3DFMT_A2R10G10B10 %d x %d, %d Hz", current_dm.Width, current_dm.Height, current_dm.RefreshRate );
				mode.comment = comment;

				infos.modes.push_back( mode );

			}
		}



		nb_modes = m_lpd3d->GetAdapterModeCount( i, D3DFMT_A8R8G8B8 );
		if( nb_modes > 0 )
		{
			for( UINT j = 0; j < nb_modes; j++ )
			{
				m_lpd3d->EnumAdapterModes( i, D3DFMT_A8R8G8B8, j, &current_dm );

				mode.format = D3DFMT_A8R8G8B8;
				mode.width = current_dm.Width;
				mode.height = current_dm.Height;
				mode.refresh_rate = current_dm.RefreshRate;

				char comment[256];
				sprintf( comment, "D3DFMT_A8R8G8B8 %d x %d, %d Hz", current_dm.Width, current_dm.Height, current_dm.RefreshRate );
				mode.comment = comment;

				infos.modes.push_back( mode );
			}
		}



		nb_modes = m_lpd3d->GetAdapterModeCount( i, D3DFMT_R5G6B5 );
		if( nb_modes > 0 )
		{
			for( UINT j = 0; j < nb_modes; j++ )
			{
				m_lpd3d->EnumAdapterModes( i, D3DFMT_R5G6B5, j, &current_dm );

				mode.format = D3DFMT_R5G6B5;
				mode.width = current_dm.Width;
				mode.height = current_dm.Height;
				mode.refresh_rate = current_dm.RefreshRate;

				char comment[256];
				sprintf( comment, "D3DFMT_R5G6B5 %d x %d, %d Hz", current_dm.Width, current_dm.Height, current_dm.RefreshRate );
				mode.comment = comment;


				infos.modes.push_back( mode );
			}
		}


		nb_modes = m_lpd3d->GetAdapterModeCount( i, D3DFMT_X1R5G5B5 );
		if( nb_modes > 0 )
		{
			for( UINT j = 0; j < nb_modes; j++ )
			{
				m_lpd3d->EnumAdapterModes( i, D3DFMT_X1R5G5B5, j, &current_dm );

				mode.format = D3DFMT_X1R5G5B5;
				mode.width = current_dm.Width;
				mode.height = current_dm.Height;
				mode.refresh_rate = current_dm.RefreshRate;
				
				char comment[256];
				sprintf( comment, "D3DFMT_X1R5G5B5 %d x %d, %d Hz", current_dm.Width, current_dm.Height, current_dm.RefreshRate );
				mode.comment = comment;


				infos.modes.push_back( mode );
			}
		}

		nb_modes = m_lpd3d->GetAdapterModeCount( i, D3DFMT_X8R8G8B8 );
		if( nb_modes > 0 )
		{
			for( UINT j = 0; j < nb_modes; j++ )
			{
				HRESULT res = m_lpd3d->EnumAdapterModes( i, D3DFMT_X8R8G8B8, j, &current_dm );

				mode.format = D3DFMT_X8R8G8B8;
				mode.width = current_dm.Width;
				mode.height = current_dm.Height;
				mode.refresh_rate = current_dm.RefreshRate;

				char comment[256];
				sprintf( comment, "D3DFMT_X8R8G8B8 %d x %d, %d Hz", current_dm.Width, current_dm.Height, current_dm.RefreshRate );
				mode.comment = comment;

				infos.modes.push_back( mode );
			}
		}

		m_adapters_infos.push_back( infos );

		m_combo_dx9adapters->Append( current.Driver );
	}

	m_combo_dx9adapters->Select( 0 );

	update_d3d9tab( 0 );
}

ConfFrame::~ConfFrame( void )
{
	D3D9_RELEASE( m_lpd3d );
}

void ConfFrame::OnIdle( wxIdleEvent& p_event )
{
    p_event.RequestMore( true ); 
}

void ConfFrame::OnClose( wxCloseEvent& p_event )
{
    Destroy();
}

void ConfFrame::OnCheckBoxFullscreen( wxCommandEvent& p_event )
{
	if( p_event.IsChecked() )
	{
		m_fullscreen = true;
		m_combo_windowres->Disable();
		m_listbox_adaptermodes->Enable();
		m_statictext_selectedmode->Enable();

		if( m_listbox_adaptermodes->GetSelection() == -1 )
		{
			m_button_save->Disable();
		}
	}
	else
	{
		m_fullscreen = false;
		m_combo_windowres->Enable();
		m_listbox_adaptermodes->Disable();
		m_statictext_selectedmode->Disable();

		m_button_save->Enable();
	}
}

void ConfFrame::OnComboDx9Adapters( wxCommandEvent& p_event )
{
	int index = m_combo_dx9adapters->GetCurrentSelection();
	update_d3d9tab( index );
}

void ConfFrame::update_d3d9tab( long p_adapter_index )
{
	adapter_infos infos = m_adapters_infos[p_adapter_index];

	m_statictext_adapterdescr->SetLabel( infos.infos.Description );

	m_listbox_adaptermodes->Clear();
	
	for( unsigned long i = 0; i < infos.modes.size(); i++ )
	{		
		m_listbox_adaptermodes->Append( infos.modes[i].comment );
	}

	m_statictext_selectedmode->SetLabel( std::string( "Selected mode : " ) );
}

void ConfFrame::OnListBoxDx9Modes( wxCommandEvent& p_event )
{
	int adapter_index = m_combo_dx9adapters->GetSelection();
	int mode_index = m_listbox_adaptermodes->GetSelection();

	adapter_infos ai = m_adapters_infos[adapter_index];
	adapter_mode am = ai.modes[mode_index];
	
	m_statictext_selectedmode->SetLabel( std::string( "Selected mode : " ) + am.comment );

	m_button_save->Enable();
}



void ConfFrame::OnButtonSave( wxCommandEvent& p_event )
{
	FILE* fp = fopen( "appconfig.txt", "w" );

	switch( m_combo_windowres->GetCurrentSelection() )
	{
		case 0:

			fprintf( fp, "width          512\n" );
			fprintf( fp, "height         380\n" );
			break;

		case 1:

			fprintf( fp, "width          640\n" );
			fprintf( fp, "height         400\n" );
			break;

		case 2:

			fprintf( fp, "width          640\n" );
			fprintf( fp, "height         480\n" );
			break;

		case 3:

			fprintf( fp, "width          800\n" );
			fprintf( fp, "height         600\n" );
			break;

		case 4:

			fprintf( fp, "width          1024\n" );
			fprintf( fp, "height         768\n" );
			break;

		case 5:

			fprintf( fp, "width          1280\n" );
			fprintf( fp, "height         960\n" );
			break;

		case 6:
			
			fprintf( fp, "width          1440\n" );
			fprintf( fp, "height         900\n" );
			break;
	}

	if( m_checkbox_fullscreen->IsChecked() )
	{
		fprintf( fp, "fullscreen     true\n" );

	}
	else
	{
		fprintf( fp, "fullscreen     false\n" );
	}

	switch( m_combo_renderplugin->GetCurrentSelection() )
	{
		case 0:
			fprintf( fp, "renderplugin   drawspaced3d9\n" );
			break;
	}

	fprintf( fp, "dx9adapterordinal      %d\n", m_combo_dx9adapters->GetSelection() );

	if( m_checkbox_fullscreen->IsChecked() )
	{
		int adapter_index = m_combo_dx9adapters->GetSelection();
		int mode_index = m_listbox_adaptermodes->GetSelection();
		adapter_infos ai = m_adapters_infos[adapter_index];
		adapter_mode am = ai.modes[mode_index];

		fprintf( fp, "dx9fullscreen          %d %d %d %d\n", am.width, am.height, am.refresh_rate, am.format );
	}

	if( m_combo_vertexproc->GetSelection() == 0 )
	{
		fprintf( fp, "dx9vertexproc     software\n" );
	}
	else
	{
		fprintf( fp, "dx9vertexproc     hardware\n" );
	}

	fclose( fp );
}
