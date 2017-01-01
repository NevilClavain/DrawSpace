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

#include "DConfMainDialog.h"


DConfMainDialog::DConfMainDialog( wxWindow* parent, const wxString& title ) :
MainDialog( parent, wxID_ANY, title, wxDefaultPosition )
{
	m_lpd3d = Direct3DCreate9( D3D_SDK_VERSION );

	UINT nb_adapters = m_lpd3d->GetAdapterCount();

	for( UINT i = 0; i < nb_adapters; i++ )
	{
		adapter_infos infos;
		adapter_mode  mode;

		D3DADAPTER_IDENTIFIER9 current;
		m_lpd3d->GetAdapterIdentifier( i, 0, &current );

        infos.infos = current;


        D3DDISPLAYMODE current_dm;

		UINT nb_modes;
		
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
    }

    // remplissage formulaire � partir des donn�es r�colt�es

    m_propertyGrid->Append( new wxBoolProperty( "Fullscreen", wxPG_LABEL, false ) );

    wxArrayString window_dims;
    window_dims.Add( "800 x 600" );
    window_dims.Add( "1024 x 768" );
    window_dims.Add( "1280 x 960" );
    window_dims.Add( "1440 x 900" );
    window_dims.Add( "1920 x 1080" );
    m_propertyGrid->Append( new wxEnumProperty( "Window dims", wxPG_LABEL, window_dims ) );

    wxArrayString devices;
    for( size_t i = 0; i < m_adapters_infos.size(); i++ )
    {
        devices.Add( m_adapters_infos[i].infos.Description );
    }

    m_propertyGrid->Append( new wxEnumProperty( "Device", wxPG_LABEL, devices ) );

}


DConfMainDialog::~DConfMainDialog( void )
{
}

void DConfMainDialog::OnMainDialogClose( wxCloseEvent& event )
{
    Destroy();
}

#define DIALOG_PROPERTIES_VARS \
        wxStringProperty* string_prop; \
        wxFileProperty* file_prop; \
        wxFloatProperty* float_prop; \
        wxIntProperty* int_prop; \
        wxBoolProperty* bool_prop; \
        wxEnumProperty* enum_prop; \
        wxCharBuffer buffer; \
        wxAny value; \

#define DIALOG_GET_BOOL_PROPERTY( _name_, _var_name_ ) \
        bool _var_name_; \
        bool_prop = static_cast<wxBoolProperty*>( m_propertyGrid->GetProperty( _name_ ) ); \
        value = bool_prop->GetValue(); \
        value.GetAs<bool>( &_var_name_ );

#define DIALOG_GET_ENUM_PROPERTY( _name_, _var_name_ ) \
        wxString _var_name_; \
        enum_prop = static_cast<wxEnumProperty*>( m_propertyGrid->GetProperty( _name_ ) ); \
        _var_name_ = enum_prop->GetValueAsString();


#define DIALOG_GET_STRING_PROPERTY( _name_, _var_name_ ) \
        wxString _var_name_; \
        string_prop = static_cast<wxStringProperty*>( m_propertyGrid->GetProperty( _name_ ) ); \
        value = string_prop->GetValue(); \
        value.GetAs<wxString>( &_var_name_ );

#define DIALOG_WXSTRING_TO_DSSTRING( _src_var_name_, _dst_var_name_ ) \
        std::string _dst_var_name_; \
        buffer = _src_var_name_.ToAscii(); \
        _dst_var_name_ = buffer.data();



void DConfMainDialog::OnSaveButtonClick( wxCommandEvent& event )
{
    DIALOG_PROPERTIES_VARS

    DIALOG_GET_BOOL_PROPERTY( "Fullscreen", fullscreen );

    DIALOG_GET_ENUM_PROPERTY( "Window dims", wxsWindowsDims );
    DIALOG_WXSTRING_TO_DSSTRING( wxsWindowsDims, windowsDims );

    wxEnumProperty* dev_enum_prop = static_cast<wxEnumProperty*>( m_propertyGrid->GetProperty( "Device" ) );
    int device_ordinal = dev_enum_prop->GetChoiceSelection();


    DIALOG_GET_ENUM_PROPERTY( "Device", wxsDevice );
    DIALOG_WXSTRING_TO_DSSTRING( wxsDevice, device );

    FILE* fp = fopen( "appconfig.txt", "wt" );

    fprintf( fp, "renderplugin           drawspaced3d9\n" );
    fprintf( fp, "dx9vertexproc          hardware\n" );
    fprintf( fp, "dx9adapterordinal      %d\n", device_ordinal );
    

	if( fullscreen )
	{
        fprintf( fp, "fullscreen             true\n" );

	}
	else
	{
		fprintf( fp, "fullscreen             false\n" );
	}

    if( windowsDims == "800 x 600" )
    {
	    fprintf( fp, "width                  800\n" );
	    fprintf( fp, "height                 600\n" );
    }
    else if( windowsDims == "1024 x 768" )
    {
	    fprintf( fp, "width                  1024\n" );
	    fprintf( fp, "height                 768\n" );    
    }
    else if( windowsDims == "1280 x 960" )
    {
	    fprintf( fp, "width                  1280\n" );
	    fprintf( fp, "height                 960\n" );
    }
    else if( windowsDims == "1440 x 900" )
    {
	    fprintf( fp, "width                  1440\n" );
	    fprintf( fp, "height                 900\n" );    
    }
    else if( windowsDims == "1920 x 1080" )
    {
	    fprintf( fp, "width                  1920\n" );
	    fprintf( fp, "height                 1080\n" );    
    }

    wxEnumProperty* fsmode_enum_prop = static_cast<wxEnumProperty*>( m_propertyGrid->GetProperty( device + std::string( ".Display mode" ) ) );
    int device_fsmode_ordinal = fsmode_enum_prop->GetChoiceSelection();

    fflush( fp );

    fclose( fp );
}
