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
    IDXGIAdapter * adapter; 
    std::vector <IDXGIAdapter*> adapters; 
    IDXGIFactory* factory = NULL; 
    

    // Create a DXGIFactory object.
    CreateDXGIFactory(__uuidof(IDXGIFactory) ,(void**)&factory);


    for( UINT i = 0; factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i )
    {
        adapters.push_back(adapter);
    } 

    UINT nb_adapters = adapters.size();

    for( UINT i = 0; i < nb_adapters; i++ )
	{
		adapter_infos infos;
        adapter_mode  mode;

		DXGI_ADAPTER_DESC current;
		adapters[i]->GetDesc( &current );

        infos.infos = current;


        
        IDXGIOutput* output = NULL; 
        HRESULT hr;

        hr = adapters[i]->EnumOutputs( 0, &output );



        if( S_OK == hr )
        {

            DXGI_OUTPUT_DESC dod;

            output->GetDesc( &dod );


            UINT nb_modes = 0;
            DXGI_MODE_DESC* displayModes = NULL;
            DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

            // Get the number of elements
            hr = output->GetDisplayModeList( format, 0, &nb_modes, NULL );

            displayModes = new DXGI_MODE_DESC[nb_modes];
            hr = output->GetDisplayModeList( format, 0, &nb_modes, displayModes );

		    if( nb_modes > 0 )
		    {
			    for( UINT j = 0; j < nb_modes; j++ )
			    {

				    mode.format = DXGI_FORMAT_R8G8B8A8_UINT;
				    mode.width = displayModes[j].Width;
				    mode.height = displayModes[j].Height;
				    mode.refresh_rate = displayModes[j].RefreshRate.Numerator / displayModes[j].RefreshRate.Denominator;

				    char comment[256];
				    sprintf( comment, "DXGI_FORMAT_R8G8B8A8_UINT %d x %d, %d Hz", mode.width, mode.height, mode.refresh_rate );
				    mode.comment = comment;

				    infos.modes.push_back( mode );
			    }
		    }
        }

        m_adapters_infos.push_back( infos );
    }

    if( factory )
    {
        factory->Release();
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

    FILE* fp = fopen( "appconfig.txt", "w" );

    fprintf( fp, "renderplugin           drawspaced3d11\n" );
    fprintf( fp, "dx11adapterordinal      %d\n", device_ordinal );
    
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
