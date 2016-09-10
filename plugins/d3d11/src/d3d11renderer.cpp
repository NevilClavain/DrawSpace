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

#include "d3d11renderer.h"
#include "memalloc.h"
#include "misc_utils.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;


extern void TranslateD3DD11Error( HRESULT p_hRes, dsstring &p_str );

_DECLARE_DS_LOGGER( logger, "d3d11", NULL )

D3D11Renderer::D3D11Renderer( void ) :
m_lpd3dswapchain( NULL ),
m_lpd3ddevice( NULL ),
m_lpd3ddevcontext( NULL ),
m_screentarget( NULL ),
m_currentDevice( -1 )
{

}

D3D11Renderer::~D3D11Renderer( void )
{

}


void D3D11Renderer::GetDescr( dsstring& p_descr )
{
    p_descr = "Direct3D11";
}

void D3D11Renderer::GetDeviceDescr( DeviceDescr& p_ddescr )
{
    if( m_currentDevice != -1 )
    {
        p_ddescr = m_devices_descrs[m_currentDevice];
    }
}

void D3D11Renderer::DumpMemoryAllocs( void )
{

}

bool D3D11Renderer::Init( HWND p_hwnd, bool p_fullscreen, long p_w_width, long p_w_height, DrawSpace::Logger::Configuration* p_logconf )
{
    DECLARE_D3D11ASSERT_VARS

    p_logconf->RegisterSink( &logger );
    logger.SetConfiguration( p_logconf );

    p_logconf->RegisterSink( MemAlloc::GetLogSink() );
    MemAlloc::GetLogSink()->SetConfiguration( p_logconf );



    if( true == m_config.Run( "appconfig.txt", "    " ) )
    {
        _DSDEBUG( logger, "plugin configuration file -> OK" )
    }
    else
    {
        _DSWARN( logger, "Failed to parse plugin configuration file !! -> switching to default settings" )
        return false;
    }

    long adapter = m_config.m_adapter_ordinal;

    m_currentDevice = adapter;

    IDXGIFactory* factory = NULL; 
    CreateDXGIFactory(__uuidof(IDXGIFactory) ,(void**)&factory);

    // enum existing devices

    IDXGIAdapter* curr_adapter;

    for( UINT i = 0; factory->EnumAdapters(i, &curr_adapter) != DXGI_ERROR_NOT_FOUND; ++i )
    {
        //adapters.push_back(adapter);

        DXGI_ADAPTER_DESC   descr;
        curr_adapter->GetDesc( &descr );

        DeviceDescr currentDescr;

        std::wstring wdescr( descr.Description );


        currentDescr.description = DrawSpace::Utils::WString2String( descr.Description ); //"N/A"; //wcharToDsString( descr.Description );
        currentDescr.driver = "N/A";
        currentDescr.deviceName = "N/A";

        m_devices_descrs.push_back( currentDescr );
    } 


    //////////////////////////////

    m_hwnd = p_hwnd;

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd, sizeof( sd ) );
    sd.BufferCount = 1;

    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = p_hwnd;
    sd.SampleDesc.Count = 1; // de 1 a 4 en D3D11
    sd.SampleDesc.Quality = 0; // en low quality
    sd.Windowed = TRUE;

    hRes = D3D11CreateDeviceAndSwapChain( NULL, D3D_DRIVER_TYPE_HARDWARE,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  D3D11_SDK_VERSION,
                                  &sd,
                                  &m_lpd3dswapchain,
                                  &m_lpd3ddevice,
                                  NULL,
                                  &m_lpd3ddevcontext );

    D3D11_CHECK( D3D11CreateDeviceAndSwapChain )

    ///////////////////////////////////////////////////////////////////////

    ID3D11Texture2D* backBuffer;
    hRes = m_lpd3dswapchain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&backBuffer );
    D3D11_CHECK( GetBuffer )

    hRes = m_lpd3ddevice->CreateRenderTargetView( backBuffer, NULL, &m_screentarget );
    D3D11_CHECK( CreateRenderTargetView )

    backBuffer->Release();


	IFW1Factory* fW1Factory;
	hRes = FW1CreateFactory( FW1_VERSION, &fW1Factory );
	
	hRes = fW1Factory->CreateFontWrapper( m_lpd3ddevice, L"System", &m_fontWrapper );



    return true;
}

void D3D11Renderer::Release( void )
{

}

void D3D11Renderer::SetViewport( bool p_automatic, long p_vpx, long p_vpy, long p_vpwidth, long p_vpheight, float p_vpminz, float p_vpmaxz )
{
    if( p_automatic )
    {
        RECT wndrect;
        ::GetClientRect( m_hwnd, &wndrect );

        m_viewport.Width = wndrect.right - wndrect.left;
        m_viewport.Height = wndrect.bottom - wndrect.top;
        m_viewport.MinDepth = p_vpminz;
        m_viewport.MaxDepth = p_vpmaxz;
        m_viewport.TopLeftX = wndrect.left;
        m_viewport.TopLeftY = wndrect.top;

    }
    else
    {
        m_viewport.Width = p_vpwidth;
        m_viewport.Height = p_vpheight;
        m_viewport.MinDepth = p_vpminz;
        m_viewport.MaxDepth = p_vpmaxz;
        m_viewport.TopLeftX = p_vpx;
        m_viewport.TopLeftY = p_vpy; 
    }

    m_lpd3ddevcontext->RSSetViewports( 1, &m_viewport );
}

void D3D11Renderer::BeginScreen( void )
{
    m_lpd3ddevcontext->OMSetRenderTargets( 1, &m_screentarget, NULL );
}

void D3D11Renderer::EndScreen( void )
{
}

void D3D11Renderer::FlipScreen( void )
{
    m_lpd3dswapchain->Present( 0, 0 );
}


void D3D11Renderer::ClearScreen( unsigned char p_r, unsigned char p_g, unsigned char p_b, unsigned char p_a )
{
    FLOAT clearcolor[4];

    clearcolor[0] = p_r / 255.0;
    clearcolor[1] = p_g / 255.0;
    clearcolor[2] = p_b / 255.0;
    clearcolor[3] = p_a / 255.0;

    m_lpd3ddevcontext->ClearRenderTargetView( m_screentarget, clearcolor );
}

void D3D11Renderer::ClearDepth( dsreal p_value )
{

}

void D3D11Renderer::BeginTarget( DrawSpace::Core::Texture* p_texture )
{

}

void D3D11Renderer::EndTarget( DrawSpace::Core::Texture* p_texture )
{

}

bool D3D11Renderer::CreateMeshe( DrawSpace::Core::Meshe* p_meshe, void** p_data )
{
    return true;
}

void D3D11Renderer::RemoveMeshe( DrawSpace::Core::Meshe* p_meshe, void* p_data )
{

}

bool D3D11Renderer::SetMeshe( void* p_data )
{
    return true;
}

bool D3D11Renderer::UpdateMesheIndexes( DrawSpace::Core::Meshe* p_meshe, void* p_data )
{
    return true;
}

bool D3D11Renderer::UpdateMesheVertices( DrawSpace::Core::Meshe* p_meshe, void* p_data )
{
    return true;
}

bool D3D11Renderer::UpdateMesheVerticesFromImpostors( const DrawSpace::ImpostorsDisplayList& p_list, void* p_data )
{
    return true;
}

bool D3D11Renderer::CreateTexture( DrawSpace::Core::Texture* p_texture, void** p_data )
{
    return true;
}

void D3D11Renderer::DestroyTexture( void* p_data )
{

}

bool D3D11Renderer::SetTexture( void* p_data, int p_stage )
{
    return true;
}

bool D3D11Renderer::SetVertexTexture( void* p_data, int p_stage )
{
    return true;
}

bool D3D11Renderer::UnsetTexture( int p_stage )
{
    return true;
}

bool D3D11Renderer::UnsetVertexTexture( int p_stage )
{
    return true;
}

bool D3D11Renderer::AllocTextureContent( void* p_texturedata )
{
    return true;
}

void D3D11Renderer::ReleaseTextureContent( void* p_texturedata )
{

}

void* D3D11Renderer::GetTextureContentPtr( void* p_texturedata )
{
    return NULL;
}

bool D3D11Renderer::CopyTextureContent( void* p_texturedata )
{
    return true;
}

bool D3D11Renderer::UpdateTextureContent( void* p_texturedata )
{
    return true;
}

bool D3D11Renderer::CreateShaders( DrawSpace::Core::Fx* p_fx, void** p_data )
{
    return true;
}

bool D3D11Renderer::SetShaders( void* p_data )
{
    return true;
}

bool D3D11Renderer::CreateRenderStatesSet( DrawSpace::Core::Fx* p_fx, void** p_data )
{
    return true;
}

bool D3D11Renderer::ApplyRenderStatesIn( void* p_data )
{
    return true;
}

bool D3D11Renderer::ApplyRenderStatesOut( void* p_data )
{
    return true;
}

bool D3D11Renderer::SetFxShaderParams( int p_shader_index, long p_register, DrawSpace::Utils::Vector& p_vector )
{
    return true;
}

bool D3D11Renderer::SetFxShaderMatrix( int p_shader_index, long p_register, DrawSpace::Utils::Matrix& p_mat )
{
    return true;
}

bool D3D11Renderer::DrawMeshe( DrawSpace::Utils::Matrix p_world, DrawSpace::Utils::Matrix p_view, DrawSpace::Utils::Matrix p_proj )
{
    return true;
}

void D3D11Renderer::SetRenderState( DrawSpace::Core::RenderState* p_renderstate )
{

}

void D3D11Renderer::GetRenderCharacteristics( Characteristics& p_characteristics )
{

}

void D3D11Renderer::DrawText( long p_r, long p_g, long p_b, int p_posX, int p_posY, const char* p_format, ... )
{
    char buffer[512];
    _vsnprintf( buffer, 512, p_format, (va_list)( &p_format + 1 ) );

    unsigned long color32 = (((0xff)<<24)|(((p_b)&0xff)<<16)|(((p_g)&0xff)<<8)|((p_r)&0xff));

    m_fontWrapper->DrawString(
		m_lpd3ddevcontext,
        DrawSpace::Utils::String2WString( std::string( buffer ) ).c_str(),
		15.0f,// Font size
		p_posX,// X position
		p_posY,// Y position
		color32,// Text color, 0xAaBbGgRr
		0// Flags
	);
}

void D3D11Renderer::PointProjection( DrawSpace::Utils::Matrix p_view, DrawSpace::Utils::Matrix p_proj, DrawSpace::Utils::Vector& p_point, dsreal& p_outx, dsreal& p_outy, dsreal& p_outz )
{

}


bool D3D11Renderer::Config::on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words )
{
    if( 2 == p_words.size() )
    {
        if( "dx11adapterordinal" == p_words[0] )
        {
            m_adapter_ordinal = atoi( p_words[1].c_str() );
        }
    }
    if( 5 == p_words.size() )
    {
        if( "dx9fullscreen" == p_words[0] )
        {
            m_fullscreen_width = atoi( p_words[1].c_str() );
            m_fullscreen_height = atoi( p_words[2].c_str() );
            m_refreshrate = atoi( p_words[3].c_str() );
            m_fullscreen_format = (DXGI_FORMAT)atoi( p_words[4].c_str() );
        }
    }

    return true;
}

