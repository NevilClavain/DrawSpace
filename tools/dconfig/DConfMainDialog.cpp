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

    // remplissage formulaire à partir des données récoltées

    m_propertyGrid->Append( new wxBoolProperty( "Fullscreen", wxPG_LABEL, false ) );

    wxArrayString window_dims;
    window_dims.Add( "800 x 600" );
    window_dims.Add( "1024 x 768" );
    window_dims.Add( "1280 x 960" );
    window_dims.Add( "1440 x 900" );
    m_propertyGrid->Append( new wxEnumProperty( "Window dims", wxPG_LABEL, window_dims ) );

    wxArrayString devices;
    for( size_t i = 0; i < m_adapters_infos.size(); i++ )
    {
        devices.Add( m_adapters_infos[i].infos.Description );       
    }

    m_propertyGrid->Append( new wxEnumProperty( "Device", wxPG_LABEL, devices ) );

    for( size_t i = 0; i < m_adapters_infos.size(); i++ )
    {
        wxPGProperty* devices_modes_root = m_propertyGrid->Append( new wxStringProperty( m_adapters_infos[i].infos.Description + wxString( " properties" ), wxPG_LABEL, "<composed>" ) );

        wxArrayString device_modes;

        for( size_t j = 0; j < m_adapters_infos[i].modes.size(); j++ )
        {
            device_modes.Add( m_adapters_infos[i].modes[j].comment );
        }

        m_propertyGrid->AppendIn( devices_modes_root, new wxEnumProperty( "Display mode", wxPG_LABEL, device_modes ) );
    }   

    

    

    /*
    wxArrayString renderers;


    renderers.Add( "Direct3D9" );
    // :-P
    //renderers.Add( "Direct3D11" );
    //renderers.Add( "Direct3D12" );
    //renderers.Add( "OpenGL4" );
    //renderers.Add( "Vulkan" );

    m_propertyGrid->Append( new wxEnumProperty( "Select renderer", wxPG_LABEL, renderers ) );
    */



}


DConfMainDialog::~DConfMainDialog( void )
{
}

void DConfMainDialog::OnMainDialogClose( wxCloseEvent& event )
{
    Destroy();
}

void DConfMainDialog::OnSaveButtonClick( wxCommandEvent& event )
{

}