/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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
/* -*-LIC_END-*- */

#include "d3d11renderer.h"
#include "memalloc.h"
#include "file.h"
#include "misc_utils.h"
#include "exceptions.h"
#include <md5.h>
#include <d3dcompiler.h>

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;


extern void TranslateD3DD11Error( HRESULT p_hRes, dsstring &p_str );

static DrawSpace::Logger::Sink logger("d3d11");



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

D3D10Include::D3D10Include(const dsstring& p_basepath) : 
m_basepath(p_basepath)
{
}

HRESULT __stdcall D3D10Include::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
{
	dsstring final_path { m_basepath + pFileName };

	long size;
	void* data;

	// load it
	data = Utils::File::LoadAndAllocBinaryFile(final_path, &size);
	if (!data)
	{
		return S_FALSE;
	}

	*pBytes = size;
	*ppData = data;

	return S_OK;
}

HRESULT __stdcall D3D10Include::Close(LPCVOID pData)
{
	void* data{ (void*)pData };

	_DRAWSPACE_DELETE_N_(data);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

D3D11Renderer::D3D11Renderer( void ) :
m_lpd3dswapchain( NULL ),
m_lpd3ddevice( NULL ),
m_lpd3ddevcontext( NULL ),
m_screentarget( NULL ),
//m_inputLayout( NULL ),
m_pDepthStencil( NULL ),
m_pDepthStencilView( NULL ),
m_currentTarget( NULL ),
m_currentView( NULL )
{

}

D3D11Renderer::~D3D11Renderer( void )
{

}


void D3D11Renderer::GetDescr( dsstring& p_descr )
{
    p_descr = "Direct3D11 - " + m_driver_type;
}

void D3D11Renderer::GetShadersDescr( dsstring& p_descr )
{
    p_descr = "hlsl_4_0";
}

void D3D11Renderer::DumpMemoryAllocs( void )
{

}

void D3D11Renderer::fullscreen_autoset_desktop_resolution( int& p_fullscreen_width, int& p_fullscreen_height, DXGI_FORMAT& p_fullscreen_format, int& p_fullscreen_refreshRate_num, int& p_fullscreen_refreshRate_den )
{
    bool found = false;
    // get user windows desktop resolution
    RECT desktop_rect;
    GetWindowRect( GetDesktopWindow(), &desktop_rect );

    p_fullscreen_width = desktop_rect.right - desktop_rect.left;
    p_fullscreen_height = desktop_rect.bottom - desktop_rect.top;
    p_fullscreen_format = DXGI_FORMAT_R8G8B8A8_UNORM;
    p_fullscreen_refreshRate_num = 60;
    p_fullscreen_refreshRate_den = 1;

}


bool D3D11Renderer::Init( HWND p_hwnd, bool p_fullscreen, long p_w_width, long p_w_height, DrawSpace::Logger::Configuration* p_logconf )
{
    DECLARE_D3D11ASSERT_VARS

    _DSDEBUG( logger, "begin" )

    p_logconf->registerSink( &logger );
    logger.SetConfiguration( p_logconf );

    p_logconf->registerSink( MemAlloc::GetLogSink() );
    MemAlloc::GetLogSink()->SetConfiguration( p_logconf );


    _DSDEBUG( logger, "D3D11 init begin" )

    IDXGIFactory* factory = NULL; 
    CreateDXGIFactory(__uuidof(IDXGIFactory) ,(void**)&factory);


    int         fullscreen_width;
    int         fullscreen_height;
    DXGI_FORMAT fullscreen_format;
    int         fullscreen_refresh_rate_num;
    int         fullscreen_refresh_rate_den;

    if( p_fullscreen )
    {
        // basculer automatiquement sur un mode correspondant a la resol actuelle du bureau windows
        fullscreen_autoset_desktop_resolution( fullscreen_width, fullscreen_height, fullscreen_format, fullscreen_refresh_rate_num, fullscreen_refresh_rate_den );
    }
    //////////////////////////////

    m_hwnd = p_hwnd;

    RECT rect;
    GetClientRect( m_hwnd, &rect );

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif


    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd, sizeof( sd ) );

    if( p_fullscreen )
    {
        m_characteristics.fullscreen = true;

        m_characteristics.width_resol = fullscreen_width;
        m_characteristics.height_resol = fullscreen_height;

        sd.BufferCount = 1;
        sd.BufferDesc.Width = fullscreen_width;
        sd.BufferDesc.Height = fullscreen_height;
        sd.BufferDesc.Format = fullscreen_format;
        sd.BufferDesc.RefreshRate.Numerator = fullscreen_refresh_rate_num;
        sd.BufferDesc.RefreshRate.Denominator = fullscreen_refresh_rate_den;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = p_hwnd;
        sd.SampleDesc.Count = 1; // de 1 a 4 en D3D11
        sd.SampleDesc.Quality = 0; // en low quality        
        sd.Windowed = FALSE;    
    }
    else
    {
        
        m_characteristics.fullscreen = false;

        m_characteristics.width_resol = rect.right - rect.left;
        m_characteristics.height_resol = rect.bottom - rect.top;

        sd.BufferCount = 1;
        sd.BufferDesc.Width = m_characteristics.width_resol;
        sd.BufferDesc.Height = m_characteristics.height_resol;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = p_hwnd;
        sd.SampleDesc.Count = 1; // de 1 a 4 en D3D11
        sd.SampleDesc.Quality = 0; // en low quality
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        sd.Windowed = TRUE;

    }

    static const std::vector<std::pair<D3D_DRIVER_TYPE,dsstring>> driver_type =
    {
        { D3D_DRIVER_TYPE_HARDWARE, "HARDWARE initialized"},
        { D3D_DRIVER_TYPE_WARP, "WARP initialized"},
        { D3D_DRIVER_TYPE_REFERENCE, "REF initialized"},
        { D3D_DRIVER_TYPE_SOFTWARE, "SOFTWARE initialized"}
    };

    HRESULT r;
    for( auto& e : driver_type)
    {
        r = D3D11CreateDeviceAndSwapChain(NULL, e.first,
            NULL,
            createDeviceFlags,
            NULL,
            0,
            D3D11_SDK_VERSION,
            &sd,
            &m_lpd3dswapchain,
            &m_lpd3ddevice,
            NULL,
            &m_lpd3ddevcontext);

        if( r == S_OK )
        {
            m_driver_type = e.second;
            break;
        }
    }
    hRes = r;
    D3D11_CHECK( D3D11CreateDeviceAndSwapChain )

    if( p_fullscreen )
    {
        m_lpd3dswapchain->SetFullscreenState( TRUE, NULL );
    }


    ///////////////////////////////////////////////////////////////////////

    ID3D11Texture2D* backBuffer;
    hRes = m_lpd3dswapchain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&backBuffer );
    D3D11_CHECK( GetBuffer )

    hRes = m_lpd3ddevice->CreateRenderTargetView( backBuffer, NULL, &m_screentarget );
    D3D11_CHECK( CreateRenderTargetView )

    backBuffer->Release();


    ////////////////////////////////////////////////////////////////////////

    D3D11_DEPTH_STENCIL_DESC dsDesc;
    ZeroMemory( &dsDesc, sizeof( dsDesc ) );

    // Depth test parameters
    dsDesc.DepthEnable = FALSE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

    // Stencil test parameters
    dsDesc.StencilEnable = FALSE;
    dsDesc.StencilReadMask = 0xFF;
    dsDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    hRes = m_lpd3ddevice->CreateDepthStencilState( &dsDesc, &m_DSState_DepthTestDisabled );
    D3D11_CHECK( CreateDepthStencilState )

    dsDesc.DepthEnable = TRUE;

    hRes = m_lpd3ddevice->CreateDepthStencilState( &dsDesc, &m_DSState_DepthTestEnabled );
    D3D11_CHECK( CreateDepthStencilState )

    // dans D3D9 plugin, zbuffer est activé par défaut (cf doc)
    // donc idem ici
    m_lpd3ddevcontext->OMSetDepthStencilState( m_DSState_DepthTestEnabled, 1 );

    
    if( !create_depth_stencil_buffer( m_characteristics.width_resol, m_characteristics.height_resol, DXGI_FORMAT_D24_UNORM_S8_UINT, 
                                        &m_pDepthStencil, &m_pDepthStencilView ) )
    {
        return false;
    }
    

    ////////////////////////////////////////////////////////////////////////

    //projection set, to automatically fit with the screen resolution
    float v_width, v_height;
    if( p_fullscreen )
    {
        v_width = 1.0;
        v_height = v_width * fullscreen_height / fullscreen_width;
    }
    else
    {
        v_width = 1.0;
        v_height = v_width * p_w_height / p_w_width;
    }

	_DSDEBUG( logger, dsstring("projection : v_width = ") << v_width << dsstring( " v_height = " ) << v_height );

    m_characteristics.width_viewport = v_width;
    m_characteristics.height_viewport = v_height;

    //////////////////////////////////////////////////////////////////////////
    
	IFW1Factory* fW1Factory;
	hRes = FW1CreateFactory( FW1_VERSION, &fW1Factory );
	
	hRes = fW1Factory->CreateFontWrapper( m_lpd3ddevice, L"Courier New", &m_fontWrapper );
    
    //////////////////////////////////////////////////////////////////////////

    m_lpd3ddevcontext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
   

    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory( &sampDesc, sizeof(sampDesc) );

    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;


    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    hRes = m_lpd3ddevice->CreateSamplerState( &sampDesc, &m_pointFilterSamplerState );
    D3D11_CHECK( CreateSamplerState )

    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    hRes = m_lpd3ddevice->CreateSamplerState(&sampDesc, &m_pointFilterSamplerState_uvwrap);
    D3D11_CHECK(CreateSamplerState)


    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    hRes = m_lpd3ddevice->CreateSamplerState( &sampDesc, &m_linearFilterSamplerState );
    D3D11_CHECK( CreateSamplerState )

    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    hRes = m_lpd3ddevice->CreateSamplerState(&sampDesc, &m_linearFilterSamplerState_uvwrap);
    D3D11_CHECK(CreateSamplerState)


    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    hRes = m_lpd3ddevice->CreateSamplerState( &sampDesc, &m_anisotropicFilterSamplerState );
    D3D11_CHECK( CreateSamplerState )

    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    hRes = m_lpd3ddevice->CreateSamplerState(&sampDesc, &m_anisotropicFilterSamplerState_uvwrap);
    D3D11_CHECK(CreateSamplerState)


    ID3D11SamplerState* ss_array[] = { m_pointFilterSamplerState };

    // a mettre en option dans l'interface renderer ?
    for( long i = 0; i < 8; i++ )
    {
        m_lpd3ddevcontext->VSSetSamplers( i, 1, ss_array );
        m_lpd3ddevcontext->PSSetSamplers( i, 1, ss_array );
    }


    ////////////////////////////////////////////////////////////////////////////

    // default renderstate description
    D3D11_RASTERIZER_DESC rsDesc;

    rsDesc.FillMode = D3D11_FILL_SOLID;

    // dans d3d9, le cull mode par defaut est ccw (cf doc)
    // donc idem ici

    // cull ccw
    rsDesc.CullMode = D3D11_CULL_BACK;   
    rsDesc.FrontCounterClockwise = FALSE;
    // cull ccw

    /*
    // cull cw
    rsDesc.CullMode = D3D11_CULL_FRONT;   
    rsDesc.FrontCounterClockwise = FALSE;
    // cull cw
    */

    rsDesc.DepthBias = 0;
    rsDesc.SlopeScaledDepthBias = 0.0f;
    rsDesc.DepthBiasClamp = 0.0f;
    rsDesc.DepthClipEnable = TRUE;
    rsDesc.ScissorEnable = FALSE;
    rsDesc.MultisampleEnable = FALSE;
    rsDesc.AntialiasedLineEnable = FALSE;

    m_currentRSDesc = rsDesc;

    // apply this default renderstate
    set_cache_rs();

    ////////////////////////////////////////////////////////////////////////////

    // default blend state description

    D3D11_BLEND_DESC blDesc;
    ZeroMemory( &blDesc, sizeof( blDesc ) );

    blDesc.AlphaToCoverageEnable = false;
    blDesc.IndependentBlendEnable = false;
    blDesc.RenderTarget[0].BlendEnable = false;
    blDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    blDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
    blDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    m_currentBlendDesc = blDesc;

    // apply this default blend state
    set_cache_blendstate();

    /////////////////////////////////////////////////////////////////////////////

    D3D11_BUFFER_DESC bd;
    ZeroMemory( &bd, sizeof( bd ) );

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( ShaderLegacyArg );
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;

    hRes = m_lpd3ddevice->CreateBuffer( &bd, NULL, &m_vertexshader_legacyargs_buffer );
    D3D11_CHECK( CreateBuffer )
    
    hRes = m_lpd3ddevice->CreateBuffer( &bd, NULL, &m_pixelshader_legacyargs_buffer );
    D3D11_CHECK( CreateBuffer )


    // set viewport....
    if( p_fullscreen )
    {
        m_mainScreenViewport.Width = fullscreen_width;
        m_mainScreenViewport.Height = fullscreen_height;
        m_mainScreenViewport.MinDepth = 0.0;
        m_mainScreenViewport.MaxDepth = 1.0;
        m_mainScreenViewport.TopLeftX = 0.0;
        m_mainScreenViewport.TopLeftY = 0.0;
    }
    else
    {
        m_mainScreenViewport.Width = rect.right - rect.left;
        m_mainScreenViewport.Height = rect.bottom - rect.top;
        m_mainScreenViewport.MinDepth = 0.0;
        m_mainScreenViewport.MaxDepth = 1.0;
        m_mainScreenViewport.TopLeftX = rect.left;
        m_mainScreenViewport.TopLeftY = rect.top;        
    }

    m_lpd3ddevcontext->RSSetViewports( 1, &m_mainScreenViewport );

    // renderer characteristics dump
    _DSDEBUG( logger, dsstring( "characteristics.width_resol = " ) << (int)m_characteristics.width_resol );
    _DSDEBUG( logger, dsstring( "characteristics.height_resol = " ) << (int)m_characteristics.height_resol );
    _DSDEBUG( logger, dsstring( "characteristics.width_viewport = " ) << (float)m_characteristics.width_viewport );
    _DSDEBUG( logger, dsstring( "characteristics.height_viewport = " ) << (float)m_characteristics.height_viewport );
    _DSDEBUG( logger, dsstring( "characteristics.fullscreen = " ) << m_characteristics.fullscreen );

    _DSDEBUG( logger, "end : ok" )

    return true;
}

bool D3D11Renderer::create_depth_stencil_buffer( int p_width, int p_height, DXGI_FORMAT p_format, ID3D11Texture2D** p_texture2D, ID3D11DepthStencilView** p_view )
{
    DECLARE_D3D11ASSERT_VARS

    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory( &descDepth, sizeof( descDepth ) );
    descDepth.Width = p_width;
    descDepth.Height = p_height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hRes = m_lpd3ddevice->CreateTexture2D( &descDepth, NULL, p_texture2D );

    D3D11_CHECK( CreateTexture2D )

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory( &descDSV, sizeof(descDSV) );
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hRes = m_lpd3ddevice->CreateDepthStencilView( *p_texture2D, &descDSV, p_view );

    D3D11_CHECK( CreateDepthStencilView )

    return true;
}

void D3D11Renderer::Release( void )
{
    _DSDEBUG( logger, "begin" )

    // nettoyer toutes les resources textures...

    for( auto it = m_textures_base.begin(); it != m_textures_base.end(); ++it )
    {
        if( it->second->texture )
        {
            it->second->texture->Release();
        }

        for (auto& e : it->second->rendertextureTargetViews)
        {
            e->Release();
        }
        it->second->rendertextureTargetViews.clear();

        if( it->second->stencilDepthBuffer )
        {
            it->second->stencilDepthBuffer->Release();
        }

        if( it->second->stencilDepthView )
        {
            it->second->stencilDepthView->Release();
        }

        if( it->second->textureShaderResourceView )
        {
            it->second->textureShaderResourceView->Release();
        }

        if( it->second->bits )
        {
            _DRAWSPACE_DELETE_N_( it->second->bits );
            it->second->bits = NULL;
        }

        _DRAWSPACE_DELETE_( it->second );
    }
    m_textures_base.clear();

    // nettoyer toutes les resources shaders...
    for( auto it = m_shaders_base.begin(); it != m_shaders_base.end(); ++it )
    {
        it->second->vertex_shader->Release();
        it->second->pixel_shader->Release();
        it->second->input_layout->Release();

        _DRAWSPACE_DELETE_( it->second );
    }
    m_shaders_base.clear();

    // nettoyer toutes les resources meshes...
    for( auto it = m_meshes_base.begin(); it != m_meshes_base.end(); ++it )
    {
        it->second->index_buffer->Release();
        it->second->vertex_buffer->Release();

        _DRAWSPACE_DELETE_( it->second );
    }
    m_meshes_base.clear();


	for (auto it = m_shadersbytes_base.begin(); it != m_shadersbytes_base.end(); ++it)
	{
		ShaderBytesData* e = *it;

		if (e->blob)
		{
			e->blob->Release();
		}
		_DRAWSPACE_DELETE_(e);
	}
	m_shadersbytes_base.clear();



    D3D11_RELEASE( m_pixelshader_legacyargs_buffer );
    D3D11_RELEASE( m_vertexshader_legacyargs_buffer );

    for( auto it = m_bsCache.begin(); it != m_bsCache.end(); ++it )
    {
        it->second.bs_state->Release();
    }
    m_bsCache.clear();

    for( auto it = m_rsCache.begin(); it != m_rsCache.end(); ++it )
    {
        it->second.rs_state->Release();
    }
    m_rsCache.clear();
    
    D3D11_RELEASE( m_anisotropicFilterSamplerState );
    D3D11_RELEASE( m_linearFilterSamplerState );
    D3D11_RELEASE( m_pointFilterSamplerState );

    D3D11_RELEASE(m_anisotropicFilterSamplerState_uvwrap);
    D3D11_RELEASE(m_linearFilterSamplerState_uvwrap);
    D3D11_RELEASE(m_pointFilterSamplerState_uvwrap);


    m_fontWrapper->Release();
    D3D11_RELEASE( m_pDepthStencilView );
    D3D11_RELEASE( m_pDepthStencil );
    D3D11_RELEASE( m_pDepthStencil );
    D3D11_RELEASE( m_DSState_DepthTestEnabled );
    D3D11_RELEASE( m_DSState_DepthTestDisabled );
    D3D11_RELEASE( m_screentarget );
    D3D11_RELEASE( m_lpd3dswapchain );
    D3D11_RELEASE( m_lpd3ddevcontext );
    D3D11_RELEASE( m_lpd3ddevice );

    _DSDEBUG( logger, "end" )
}

void D3D11Renderer::BeginScreen( void )
{
    m_currentTarget = m_screentarget;
    m_currentView = m_pDepthStencilView;
    m_lpd3ddevcontext->OMSetRenderTargets( 1, &m_currentTarget, m_currentView );

    m_lpd3ddevcontext->RSSetViewports( 1, &m_mainScreenViewport );
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

    m_lpd3ddevcontext->ClearRenderTargetView( m_currentTarget, clearcolor );
}

void D3D11Renderer::ClearDepth( dsreal p_value )
{
    m_lpd3ddevcontext->ClearDepthStencilView( m_currentView, D3D11_CLEAR_DEPTH, p_value, 0 );
}

void D3D11Renderer::BeginTarget( void* p_data, int p_slice_index)
{
    TextureInfos* ti = (TextureInfos*)p_data;
    dsstring hash = ti->hash;

    if( m_targettextures_base.count( hash ) > 0 )
    {
        if (!ti->texture3D && p_slice_index > 0)
        {
            _DSEXCEPTION("Unexpected slice number - not a 3D texture !")
        }
        else if ((size_t)p_slice_index >= ti->rendertextureTargetViews.size())
        {
            _DSEXCEPTION("Unexpected slice number!")
        }

        TextureInfos* ti { m_targettextures_base.at(hash) };

        m_currentTarget = ti->rendertextureTargetViews[p_slice_index];
       
        m_currentView = ti->stencilDepthView;
        m_lpd3ddevcontext->OMSetRenderTargets( 1, &m_currentTarget, m_currentView );
        m_lpd3ddevcontext->RSSetViewports( 1, &ti->viewport );
    }
    else
    {
        // pas trouvé de texture cible à setter 

        // quick'n dirty pour eviter que ce rendu ne vienne écraser le résultat de la rendertarget actuelle (positionnée par le dernier 
        // appel à OMSetRenderTargets() )

        // a noter que ce pb n'existait pas en D3D9 (API structurée différement)

        BeginScreen();
    }
}

void D3D11Renderer::EndTarget( void* p_data )
{
    // rien a faire ici en D3D11...
}



bool D3D11Renderer::CreateMeshe( DrawSpace::Core::Meshe* p_meshe, void** p_data )
{
    DECLARE_D3D11ASSERT_VARS

    D3D11_SUBRESOURCE_DATA id = {0};
    D3D11_BUFFER_DESC vertexBufferDesc = {0};
    D3D11_BUFFER_DESC indexBufferDesc = {0};
    
    const auto meshe{ p_meshe };

    dsstring hash;
    meshe->GetMD5( hash );

    if( m_meshes_base.count( hash ) > 0 )
    {
        *p_data = (void *)m_meshes_base[hash];
        meshe->SetRenderData( (void *)m_meshes_base[hash] );
        return true;
    }

    long nb_vertices = meshe->GetVertexListSize();
    long nb_triangles = meshe->GetTrianglesListSize();

    MesheData* meshe_data = _DRAWSPACE_NEW_( MesheData, MesheData );

    // vertex buffer creation
    d3d11vertex* v = new d3d11vertex[nb_vertices];
    for( long i = 0; i < nb_vertices; i++ )
    {
        Core::Vertex vertex;
        meshe->GetVertex( i, vertex );
     

        v[i].pos.x = (float)vertex.x;
        v[i].pos.y = (float)vertex.y;
        v[i].pos.z = (float)vertex.z;

        v[i].normale.x = (float)vertex.nx;
        v[i].normale.y = (float)vertex.ny;
        v[i].normale.z = (float)vertex.nz;

        v[i].tangent.x = (float)vertex.tx;
        v[i].tangent.y = (float)vertex.ty;
        v[i].tangent.z = (float)vertex.tz;

        v[i].binormale.x = (float)vertex.bx;
        v[i].binormale.y = (float)vertex.by;
        v[i].binormale.z = (float)vertex.bz;

        for( size_t j = 0; j < 9; j++ )
        {
            v[i].t[j].x = vertex.tu[j];
            v[i].t[j].y = vertex.tv[j];
            v[i].t[j].z = vertex.tw[j];
            v[i].t[j].w = vertex.ta[j];
        }
    }

	id.pSysMem = v;
	id.SysMemPitch = 0;//sizeof( d3d11vertex );
	id.SysMemSlicePitch = 0;

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = nb_vertices * sizeof( d3d11vertex );
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0; 

    hRes = m_lpd3ddevice->CreateBuffer( &vertexBufferDesc, &id, &meshe_data->vertex_buffer );
    D3D11_CHECK( CreateBuffer )

    delete[] v;

    // index buffer creation
    
    d3d11triangle *t = new d3d11triangle[nb_triangles];

    for( long i = 0; i < nb_triangles; i++ )
    {
        Core::TrianglePrimitive<unsigned int> triangle;
        meshe->GetTriangles( i, triangle );

        t[i].vertex1 = triangle.at(0);
        t[i].vertex2 = triangle.at(1);
        t[i].vertex3 = triangle.at(2);
    }

	id.pSysMem = t;
    
	id.SysMemPitch = 0;//sizeof( d3d11triangle );
	id.SysMemSlicePitch = 0;

	indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	indexBufferDesc.ByteWidth = nb_triangles * sizeof( d3d11triangle );
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	indexBufferDesc.MiscFlags = 0; 

    hRes = m_lpd3ddevice->CreateBuffer( &indexBufferDesc, &id, &meshe_data->index_buffer );
    D3D11_CHECK( CreateBuffer )

    delete[] t;


    meshe_data->nb_vertices = nb_vertices;
    meshe_data->nb_primitives = nb_triangles;

    *p_data = (void *)meshe_data;

    meshe->SetRenderData( (void *)meshe_data );

    m_meshes_base[hash] = meshe_data;

    return true;
}

void D3D11Renderer::RemoveMeshe( DrawSpace::Core::Meshe* p_meshe, void* p_data )
{
    auto meshe_data{ static_cast<MesheData*>(p_data) };
   
    meshe_data->index_buffer->Release();
    meshe_data->vertex_buffer->Release();

    _DRAWSPACE_DELETE_( meshe_data );

    dsstring hash;
    p_meshe->GetMD5( hash );

    if( m_meshes_base.count( hash ) > 0 )
    {
        m_meshes_base.erase( hash );
    }
}

bool D3D11Renderer::SetMeshe( void* p_data )
{
    const auto meshe_data{ static_cast<MesheData*>(p_data) };

    const UINT stride{ sizeof(d3d11vertex) };
    const UINT offset = 0;

    m_lpd3ddevcontext->IASetVertexBuffers( 0, 1, &meshe_data->vertex_buffer, &stride, &offset ); 
    m_lpd3ddevcontext->IASetIndexBuffer( meshe_data->index_buffer, DXGI_FORMAT_R32_UINT, 0 );

    m_next_nbvertices = meshe_data->nb_vertices;
    m_next_nbtriangles = meshe_data->nb_primitives;

    return true;
}

bool D3D11Renderer::UpdateMesheIndexes( DrawSpace::Core::Meshe* p_meshe, void* p_data )
{
    DECLARE_D3D11ASSERT_VARS
    D3D11_MAPPED_SUBRESOURCE mappedResource;

    MesheData* meshe_data = (MesheData*)p_data;

    hRes = m_lpd3ddevcontext->Map( meshe_data->index_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    D3D11_CHECK( Map )

    d3d11triangle* t = (d3d11triangle*)mappedResource.pData;

    long nb_triangles = p_meshe->GetTrianglesListSize();

    for( long i = 0; i < nb_triangles; i++ )
    {
        Core::TrianglePrimitive<unsigned int> triangle;
        p_meshe->GetTriangles( i, triangle );

        t[i].vertex1 = triangle.at(0);
        t[i].vertex2 = triangle.at(1);
        t[i].vertex3 = triangle.at(2);
    }

    m_lpd3ddevcontext->Unmap( meshe_data->vertex_buffer, 0 );
    return true;
}

bool D3D11Renderer::UpdateMesheVertices( DrawSpace::Core::Meshe* p_meshe, void* p_data )
{
    DECLARE_D3D11ASSERT_VARS
    D3D11_MAPPED_SUBRESOURCE mappedResource;

    MesheData* meshe_data = (MesheData*)p_data;

    hRes = m_lpd3ddevcontext->Map( meshe_data->vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
    D3D11_CHECK( Map )

    d3d11vertex* v = (d3d11vertex*)mappedResource.pData;

    long nb_vertices = p_meshe->GetVertexListSize();

    for( long i = 0; i < nb_vertices; i++ )
    {
        Core::Vertex vertex;
        p_meshe->GetVertex( i, vertex );

        v[i].pos.x = (float)vertex.x;
        v[i].pos.y = (float)vertex.y;
        v[i].pos.z = (float)vertex.z;

        v[i].normale.x = (float)vertex.nx;
        v[i].normale.y = (float)vertex.ny;
        v[i].normale.z = (float)vertex.nz;

        v[i].tangent.x = (float)vertex.tx;
        v[i].tangent.y = (float)vertex.ty;
        v[i].tangent.z = (float)vertex.tz;

        v[i].binormale.x = (float)vertex.bx;
        v[i].binormale.y = (float)vertex.by;
        v[i].binormale.z = (float)vertex.bz;

        for( size_t j = 0; j < 9; j++ )
        {
            v[i].t[j].x = vertex.tu[j];
            v[i].t[j].y = vertex.tv[j];
            v[i].t[j].z = vertex.tw[j];
            v[i].t[j].w = vertex.ta[j];
        }
    }

    m_lpd3ddevcontext->Unmap( meshe_data->vertex_buffer, 0 );
    return true;
}

bool D3D11Renderer::CreateLineMeshe(DrawSpace::Core::LineMeshe* p_meshe, void** p_data)
{
    DECLARE_D3D11ASSERT_VARS

    const auto meshe{ p_meshe };
    const auto hash{ meshe->GetMD5() };

    if (m_meshes_base.count(hash) > 0)
    {
        *p_data = (void*)m_meshes_base[hash];
        meshe->SetRenderData((void*)m_meshes_base[hash]);
        return true;
    }

    const auto nb_vertices{ meshe->GetVertexListSize() };
    const auto nb_lines{ meshe->GetLinesListSize() };

    const auto meshe_data{ _DRAWSPACE_NEW_(MesheData, MesheData) };

    // vertex buffer creation
    const auto v{ new d3d11vertex[nb_vertices] };

    for (long i = 0; i < nb_vertices; i++)
    {
        Core::Vertex vertex;
        meshe->GetVertex(i, vertex);

        v[i].pos.x = (float)vertex.x;
        v[i].pos.y = (float)vertex.y;
        v[i].pos.z = (float)vertex.z;

        for (size_t j = 0; j < 9; j++)
        {
            v[i].t[j].x = vertex.tu[j];
            v[i].t[j].y = vertex.tv[j];
            v[i].t[j].z = vertex.tw[j];
            v[i].t[j].w = vertex.ta[j];
        }
    }

    D3D11_SUBRESOURCE_DATA id = { 0 };

    id.pSysMem = v;
    id.SysMemPitch = 0;//sizeof( d3d11vertex );
    id.SysMemSlicePitch = 0;

    D3D11_BUFFER_DESC vertexBufferDesc = { 0 };

    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = nb_vertices * sizeof(d3d11vertex);
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufferDesc.MiscFlags = 0;

    hRes = m_lpd3ddevice->CreateBuffer(&vertexBufferDesc, &id, &meshe_data->vertex_buffer);
    D3D11_CHECK(CreateBuffer)

    delete[] v;

    // index buffer creation

    const auto t{ new d3d11line[nb_lines] };

    for (long i = 0; i < nb_lines; i++)
    {
        Core::LinePrimitive<unsigned int> line;
        meshe->GetLine(i, line);

        t[i].vertex1 = line.at(0);
        t[i].vertex2 = line.at(1);
    }

    id.pSysMem = t;

    id.SysMemPitch = 0;
    id.SysMemSlicePitch = 0;

    D3D11_BUFFER_DESC indexBufferDesc = { 0 };

    indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    indexBufferDesc.ByteWidth = nb_lines * sizeof(d3d11line);
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    indexBufferDesc.MiscFlags = 0;

    hRes = m_lpd3ddevice->CreateBuffer(&indexBufferDesc, &id, &meshe_data->index_buffer);
    D3D11_CHECK(CreateBuffer)

    delete[] t;

    meshe_data->nb_vertices = nb_vertices;
    meshe_data->nb_primitives = nb_lines;
    *p_data = (void*)meshe_data;

    meshe->SetRenderData((void*)meshe_data);
    m_meshes_base[hash] = meshe_data;

    return true;
}

void D3D11Renderer::RemoveLineMeshe(DrawSpace::Core::LineMeshe* p_meshe, void* p_data)
{
    auto meshe_data{ static_cast<MesheData*>(p_data) };

    meshe_data->index_buffer->Release();
    meshe_data->vertex_buffer->Release();

    _DRAWSPACE_DELETE_(meshe_data);

    const auto hash{ p_meshe->GetMD5() };
    if (m_meshes_base.count(hash) > 0)
    {
        m_meshes_base.erase(hash);
    }
}

bool D3D11Renderer::SetLineMeshe(void* p_data)
{
    const auto meshe_data{ static_cast<MesheData*>(p_data) };

    const UINT stride{ sizeof(d3d11vertex) };
    const UINT offset = 0;

    m_lpd3ddevcontext->IASetVertexBuffers(0, 1, &meshe_data->vertex_buffer, &stride, &offset);
    m_lpd3ddevcontext->IASetIndexBuffer(meshe_data->index_buffer, DXGI_FORMAT_R32_UINT, 0);

    m_next_nbvertices = meshe_data->nb_vertices;
    m_next_nblines = meshe_data->nb_primitives;

    return true;
}



bool D3D11Renderer::create2D_rendertarget(DrawSpace::Core::Texture* p_texture, DXGI_FORMAT p_format, TextureInfos* p_texture_infos)
{
    DECLARE_D3D11ASSERT_VARS

    ID3D11Texture2D* d3dt11{ nullptr };
    ID3D11Texture2D* d3dt11_clone{ nullptr };

    unsigned long rw, rh;
    p_texture->GetRenderTargetDims(rw, rh);

    D3D11_TEXTURE2D_DESC textureDesc;

    // Initialize the render target texture description.
    ZeroMemory(&textureDesc, sizeof(textureDesc));

    // Setup the render target texture description.

    textureDesc.Width = rw;
    textureDesc.Height = rh;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = p_format;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;

    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    // Create the render target texture.
    hRes = m_lpd3ddevice->CreateTexture2D(&textureDesc, NULL, &d3dt11);
    D3D11_CHECK(CreateTexture2D)

    // creation du render target view
    D3D11_RENDER_TARGET_VIEW_DESC       renderTargetViewDesc;
    D3D11_SHADER_RESOURCE_VIEW_DESC     shaderResourceViewDesc;

    ID3D11RenderTargetView* rendertextureTargetView = NULL;
    ID3D11ShaderResourceView* rendertextureResourceView = NULL;

    ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));

    renderTargetViewDesc.Format = p_format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;

    hRes = m_lpd3ddevice->CreateRenderTargetView(d3dt11, &renderTargetViewDesc, &rendertextureTargetView);
    D3D11_CHECK(CreateRenderTargetView)


    // creation du shader resource view associé
    ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
    shaderResourceViewDesc.Format = p_format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    hRes = m_lpd3ddevice->CreateShaderResourceView(d3dt11, &shaderResourceViewDesc, &rendertextureResourceView);
    D3D11_CHECK(CreateShaderResourceView)

    /////////////////////// creation texture "clone", pour lire le contenu d'une render target

    if (Texture::RENDERTARGET_CPU == p_texture->GetRenderTarget())
    {
        textureDesc.Usage = D3D11_USAGE_STAGING;
        textureDesc.BindFlags = 0;
        textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

        hRes = m_lpd3ddevice->CreateTexture2D(&textureDesc, NULL, &d3dt11_clone);
        D3D11_CHECK(CreateTexture2D)
    }
    ////////////////////////////////////////////////////////////////////////

    D3D11_TEXTURE2D_DESC descr;
    d3dt11->GetDesc(&descr);

    dsstring path;
    p_texture->GetPath(path);
    
    p_texture_infos->content_access = (Texture::RENDERTARGET_CPU == p_texture->GetRenderTarget() ? true : false);
    p_texture_infos->bits = NULL;
    p_texture_infos->path = path;
    p_texture_infos->texture_instance = p_texture;
    p_texture_infos->texture = d3dt11;
    p_texture_infos->texture_clone = d3dt11_clone;
    p_texture_infos->texture3D = NULL;
    p_texture_infos->texture3D_clone = NULL;
    p_texture_infos->descr = descr;
    p_texture_infos->textureShaderResourceView = rendertextureResourceView;
    p_texture_infos->rendertextureTargetViews.push_back(rendertextureTargetView);

    // creation d'un stencil-depth buffer associe
    create_depth_stencil_buffer(rw, rh, DXGI_FORMAT_D24_UNORM_S8_UINT, &p_texture_infos->stencilDepthBuffer, &p_texture_infos->stencilDepthView);


    p_texture_infos->viewport.Width = descr.Width;
    p_texture_infos->viewport.Height = descr.Height;

    p_texture_infos->viewport.MinDepth = 0.0;
    p_texture_infos->viewport.MaxDepth = 1.0;
    p_texture_infos->viewport.TopLeftX = 0.0;
    p_texture_infos->viewport.TopLeftY = 0.0;

    return true;
}

bool D3D11Renderer::create3D_rendertarget(DrawSpace::Core::Texture* p_texture, DXGI_FORMAT p_format, TextureInfos* p_texture_infos)
{
    DECLARE_D3D11ASSERT_VARS

    ID3D11Texture3D* d3dt11_3D { nullptr };
    ID3D11Texture3D* d3dt11_3D_clone { nullptr };

    unsigned long rw, rh, rd;
    p_texture->GetRenderTargetDims(rw, rh);
    rd = p_texture->GetDepth();

    D3D11_TEXTURE3D_DESC textureDesc;

    // Initialize the render target texture description.
    ZeroMemory(&textureDesc, sizeof(textureDesc));


    // Setup the render target texture description.

    textureDesc.Width = rw;
    textureDesc.Height = rh;
    textureDesc.Depth = rd;
    textureDesc.MipLevels = 1;
    textureDesc.Format = p_format;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    // Create the render target texture.
    hRes = m_lpd3ddevice->CreateTexture3D(&textureDesc, NULL, &d3dt11_3D);
    D3D11_CHECK(CreateTexture3D)

    for (size_t i = 0; i < rd; i++)
    {
        // creation du render target view
        D3D11_RENDER_TARGET_VIEW_DESC       renderTargetViewDesc;

        ID3D11RenderTargetView* rendertextureTargetView = NULL;

        ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));

        renderTargetViewDesc.Format = p_format;
        renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
        renderTargetViewDesc.Texture3D.MipSlice = 0;
        renderTargetViewDesc.Texture3D.WSize = 1;
        renderTargetViewDesc.Texture3D.FirstWSlice = i;

        hRes = m_lpd3ddevice->CreateRenderTargetView(d3dt11_3D, &renderTargetViewDesc, &rendertextureTargetView);
        D3D11_CHECK(CreateRenderTargetView)

        p_texture_infos->rendertextureTargetViews.push_back(rendertextureTargetView);
    }   

    D3D11_SHADER_RESOURCE_VIEW_DESC     shaderResourceViewDesc;
    ID3D11ShaderResourceView*           rendertextureResourceView = NULL;
    // creation du shader resource view associé
    ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
    shaderResourceViewDesc.Format = p_format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
    shaderResourceViewDesc.Texture3D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture3D.MipLevels = 1;

    hRes = m_lpd3ddevice->CreateShaderResourceView(d3dt11_3D, &shaderResourceViewDesc, &rendertextureResourceView);
    D3D11_CHECK(CreateShaderResourceView)

    /////////////////////// creation texture "clone", pour lire le contenu d'une render target

    if (Texture::RENDERTARGET_CPU == p_texture->GetRenderTarget())
    {
        textureDesc.Usage = D3D11_USAGE_STAGING;
        textureDesc.BindFlags = 0;
        textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

        hRes = m_lpd3ddevice->CreateTexture3D(&textureDesc, NULL, &d3dt11_3D_clone);
        D3D11_CHECK(CreateTexture3D)
    }
    ////////////////////////////////////////////////////////////////////////

    D3D11_TEXTURE3D_DESC descr;
    d3dt11_3D->GetDesc(&descr);

    dsstring path;
    p_texture->GetPath(path);

    p_texture_infos->content_access = (Texture::RENDERTARGET_CPU == p_texture->GetRenderTarget() ? true : false);
    p_texture_infos->bits = NULL;
    p_texture_infos->path = path;
    p_texture_infos->texture_instance = p_texture;
    p_texture_infos->texture3D = d3dt11_3D;
    p_texture_infos->texture3D_clone = d3dt11_3D_clone;
    p_texture_infos->texture = NULL;
    p_texture_infos->texture_clone = NULL;
    p_texture_infos->descr3D = descr;
    p_texture_infos->textureShaderResourceView = rendertextureResourceView;
    
    p_texture_infos->stencilDepthView = NULL;

    p_texture_infos->viewport.Width = descr.Width;
    p_texture_infos->viewport.Height = descr.Height;

    p_texture_infos->viewport.MinDepth = 0.0;
    p_texture_infos->viewport.MaxDepth = 1.0;// rd;
    p_texture_infos->viewport.TopLeftX = 0.0;
    p_texture_infos->viewport.TopLeftY = 0.0;

    return true;
}

bool D3D11Renderer::CreateTexture( DrawSpace::Core::Texture* p_texture, void** p_data )
{
    DECLARE_D3D11ASSERT_VARS

    TextureInfos*           texture_infos;

    dsstring hash;
    p_texture->GetMD5( hash );

    if( m_textures_base.count( hash ) > 0 )
    {
        *p_data = (void*)m_textures_base[hash];
    
        long width = m_textures_base[hash]->descr.Width;
        long height = m_textures_base[hash]->descr.Height;

        long bpp;

        switch( m_textures_base[hash]->descr.Format )
        {
            case DXGI_FORMAT_R8G8B8A8_TYPELESS:
            case DXGI_FORMAT_R8G8B8A8_UNORM:
            case DXGI_FORMAT_B8G8R8A8_UNORM:
            case DXGI_FORMAT_R8G8B8A8_UINT:
            case DXGI_FORMAT_R8G8B8A8_SNORM:
            case DXGI_FORMAT_R8G8B8A8_SINT:
                bpp = 4;
                break;

            case DXGI_FORMAT_R16_FLOAT:
                bpp = 2;
                break;

            case DXGI_FORMAT_R32_FLOAT:
                bpp = 4;
                break;

            case DXGI_FORMAT_R32G32B32A32_FLOAT:
                bpp = 16;
                break;

            default:
                bpp = -1;
                break;
        }
               
        p_texture->SetFormat( width, height, bpp );
        p_texture->SetRenderData( (void*)m_textures_base[hash] );
        
        return true;
    }

    bool setformat_call = true; // si true, appel SetFormat() sur la texture

    if( p_texture->IsRenderTarget() )
    {
        DXGI_FORMAT format;

        int bpp = 0;
        switch (p_texture->GetRenderPurpose())
        {
            case Texture::RENDERPURPOSE_COLOR:

                format = DXGI_FORMAT_B8G8R8A8_UNORM;
                bpp = 4;
                break;

            case Texture::RENDERPURPOSE_FLOAT:

                format = DXGI_FORMAT_R16_FLOAT;
                bpp = 2;
                break;

            case Texture::RENDERPURPOSE_FLOAT32:

                format = DXGI_FORMAT_R32_FLOAT;
                bpp = 4;
                break;

            case Texture::RENDERPURPOSE_FLOATVECTOR:

                format = DXGI_FORMAT_R16G16B16A16_FLOAT;
                bpp = 8;
                break;

            case Texture::RENDERPURPOSE_FLOATVECTOR32:

                format = DXGI_FORMAT_R32G32B32A32_FLOAT;
                bpp = 16;
                break;
        }

        texture_infos = _DRAWSPACE_NEW_(TextureInfos, TextureInfos);

        bool status = p_texture->Is3DTexture() ? create3D_rendertarget(p_texture, format, texture_infos) : create2D_rendertarget(p_texture, format, texture_infos);
        if (status)
        {
            texture_infos->hash = hash;
            m_targettextures_base[hash] = texture_infos;
            m_textures_base[hash] = texture_infos;

            *p_data = (void*)texture_infos;
        }
        else
        {
            return false;
        }

    }
    else
    {
        ID3D11Texture2D* d3dt11{ nullptr };
        ID3D11Texture2D* d3dt11_clone{ nullptr };

        if( p_texture->GetPurpose() == Texture::PURPOSE_COLORFROMFILE )
        {
            ID3D11ShaderResourceView*           textureResourceView;


            void* data = p_texture->GetData();
            size_t data_size = p_texture->GetDataSize();

            hRes = D3DX11CreateShaderResourceViewFromMemory( m_lpd3ddevice, data, data_size, NULL, NULL, &textureResourceView, NULL );
            D3D11_CHECK( D3DX11CreateShaderResourceViewFromMemory )



            dsstring path;
            p_texture->GetPath( path );

            texture_infos = _DRAWSPACE_NEW_( TextureInfos, TextureInfos );
            texture_infos->content_access = false;
            texture_infos->bits = NULL;
            texture_infos->path = path;
            texture_infos->texture_instance = p_texture;
            texture_infos->texture = NULL;
            texture_infos->texture_clone = NULL;
            texture_infos->texture3D = NULL;
            texture_infos->texture3D_clone = NULL;            
            texture_infos->textureShaderResourceView = textureResourceView;
            texture_infos->stencilDepthBuffer = NULL;
            texture_infos->stencilDepthView = NULL;

            texture_infos->hash = hash;
            m_textures_base[hash] = texture_infos;

            *p_data = (void*)texture_infos;
        
            setformat_call = false; // ici on ne dispose pas des infos description texture
        }
        else
        {
            DXGI_FORMAT format;

            switch( p_texture->GetPurpose() )
            {
                case Texture::PURPOSE_COLOR:
                    {
                        //format = DXGI_FORMAT_R8G8B8A8_UNORM;
                        format = DXGI_FORMAT_B8G8R8A8_UNORM;
                    }
                    break;

                case Texture::PURPOSE_FLOAT32:
                    {
                        format = DXGI_FORMAT_R32_FLOAT;
                    }
                    break;

                case Texture::PURPOSE_FLOAT32VECTOR:
                    {
                        format = DXGI_FORMAT_R32G32B32A32_FLOAT;
                    }
                    break;
            }

            long w, h, bpp;
            p_texture->GetFormat( w, h, bpp );

            dsstring path;
            p_texture->GetPath( path );


            D3D11_TEXTURE2D_DESC desc;
            desc.Width = w;
            desc.Height = h;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = format;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Usage = D3D11_USAGE_DYNAMIC;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            desc.MiscFlags = 0;

            hRes = m_lpd3ddevice->CreateTexture2D( &desc, NULL, &d3dt11 );

            D3D11_CHECK( CreateTexture2D )

            texture_infos = _DRAWSPACE_NEW_( TextureInfos, TextureInfos );
            texture_infos->content_access = true;
            texture_infos->bits = NULL;
            texture_infos->path = path;
            texture_infos->texture_instance = p_texture;
            texture_infos->texture = d3dt11;
            texture_infos->texture_clone = NULL;
            texture_infos->texture3D = NULL;
            texture_infos->texture3D_clone = NULL;            
            texture_infos->stencilDepthBuffer = NULL;
            texture_infos->stencilDepthView = NULL;


            D3D11_TEXTURE2D_DESC descr;
            d3dt11->GetDesc( &descr );

            texture_infos->hash = hash;
            texture_infos->descr = descr;

            D3D11_SHADER_RESOURCE_VIEW_DESC     shaderResourceViewDesc;
            ID3D11ShaderResourceView*           textureResourceView;

            // creation du shader resource view associé 
	        shaderResourceViewDesc.Format = format;
	        shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	        shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	        shaderResourceViewDesc.Texture2D.MipLevels = 1;

	        hRes = m_lpd3ddevice->CreateShaderResourceView( d3dt11, &shaderResourceViewDesc, &textureResourceView );
            D3D11_CHECK( CreateShaderResourceView )

            texture_infos->textureShaderResourceView = textureResourceView;

            m_textures_base[hash] = texture_infos;

            *p_data = (void*)texture_infos;
                   
            // inutile d'appeler SetFormat() sur la texture
            setformat_call = false;
        }

    }


    if( setformat_call )
    {
        long width = texture_infos->descr.Width;
        long height = texture_infos->descr.Height;
        long bpp;

        switch( texture_infos->descr.Format )
        {
            case DXGI_FORMAT_R8G8B8A8_TYPELESS:
            case DXGI_FORMAT_R8G8B8A8_UNORM:
            case DXGI_FORMAT_B8G8R8A8_UNORM:
            case DXGI_FORMAT_R8G8B8A8_UINT:
            case DXGI_FORMAT_R8G8B8A8_SNORM:
            case DXGI_FORMAT_R8G8B8A8_SINT:
                bpp = 4;
                break;

            case DXGI_FORMAT_R16_FLOAT:
                bpp = 2;
                break;

            case DXGI_FORMAT_R32_FLOAT:
                bpp = 4;
                break;

            case DXGI_FORMAT_R32G32B32A32_FLOAT:
                bpp = 16;
                break;

            case DXGI_FORMAT_R16G16B16A16_FLOAT:
                bpp = 8;
                break;

            default:
                bpp = -1;
                break;
        }

        p_texture->SetFormat( width, height, bpp );
    }

    p_texture->SetRenderData( (void*)texture_infos );

    return true;
}

void D3D11Renderer::DestroyTexture( void* p_data )
{
    TextureInfos* ti = (TextureInfos*)p_data;

    if( ti->texture )
    {
        ti->texture->Release();
    }

    if( ti->texture_clone )
    {
        ti->texture_clone->Release();
    }

    if (ti->texture3D)
    {
        ti->texture3D->Release();
    }

    if (ti->texture3D_clone)
    {
        ti->texture3D_clone->Release();
    }

    for (auto& e : ti->rendertextureTargetViews)
    {
        e->Release();
    }
    ti->rendertextureTargetViews.clear();

    if( ti->stencilDepthBuffer )
    {
        ti->stencilDepthBuffer->Release();
    }
    if( ti->stencilDepthView )
    {
        ti->stencilDepthView->Release();
    }
    if( ti->textureShaderResourceView )
    {
        ti->textureShaderResourceView->Release();
    }


    if( ti->bits )
    {
        _DRAWSPACE_DELETE_N_( ti->bits );
        ti->bits = NULL;
    }

    if( m_textures_base.count( ti->hash ) > 0 )
    {
        m_textures_base.erase( ti->hash );
    }


    ti->texture_instance->SetRenderData( NULL );

    if( m_targettextures_base.count( ti->hash ) > 0 )
    {
        m_targettextures_base.erase( ti->hash );
    }


    _DRAWSPACE_DELETE_( ti );
}

bool D3D11Renderer::SetTexture( void* p_data, int p_stage )
{
    TextureInfos* ti = (TextureInfos*)p_data;
    m_lpd3ddevcontext->PSSetShaderResources( p_stage, 1, &ti->textureShaderResourceView );

    return true;
}

bool D3D11Renderer::SetVertexTexture( void* p_data, int p_stage )
{
    TextureInfos* ti = (TextureInfos*)p_data;
    m_lpd3ddevcontext->VSSetShaderResources( p_stage, 1, &ti->textureShaderResourceView );

    return true;
}

bool D3D11Renderer::UnsetTexture( int p_stage )
{
    ID3D11ShaderResourceView* nullSRV[1] = {nullptr};
    m_lpd3ddevcontext->PSSetShaderResources( p_stage, 1, nullSRV );
    return true;
}

bool D3D11Renderer::UnsetVertexTexture( int p_stage )
{
    return true;
}

bool D3D11Renderer::AllocTextureContent( void* p_texturedata )
{
    TextureInfos* ti = (TextureInfos*)p_texturedata;

    if( !ti->content_access )
    {
        _DSEXCEPTION( "trying to alloc content for a non accessible texture content" )
    }

    // si pas deja alloue
    if( NULL == ti->bits )
    {
        int bpp;
        switch( ti->descr.Format )
        {
            case DXGI_FORMAT_R8G8B8A8_TYPELESS:
            case DXGI_FORMAT_R8G8B8A8_UNORM:
            case DXGI_FORMAT_B8G8R8A8_UNORM:
            case DXGI_FORMAT_R8G8B8A8_UINT:
            case DXGI_FORMAT_R8G8B8A8_SNORM:
            case DXGI_FORMAT_R8G8B8A8_SINT:
                bpp = 4;
                break;

            case DXGI_FORMAT_R16_FLOAT:
                bpp = 2;
                break;

            case DXGI_FORMAT_R32_FLOAT:
                bpp = 4;
                break;

            case DXGI_FORMAT_R32G32B32A32_FLOAT:
                bpp = 16;
                break;

            case DXGI_FORMAT_R16G16B16A16_FLOAT:
                bpp = 8;
                break;

            default:
                bpp = -1;
                break;        
        }

        if( bpp != -1 )
        {
            long blocsize = bpp * ti->descr.Width * ti->descr.Height;
            ti->bits = _DRAWSPACE_NEW_EXPLICIT_SIZE_( unsigned char, unsigned char[blocsize], blocsize );
        }
    }
    return true;
}

void D3D11Renderer::ReleaseTextureContent( void* p_texturedata )
{
    TextureInfos* ti = (TextureInfos*)p_texturedata;

    if( ti->content_access && ti->bits )
    {
        _DRAWSPACE_DELETE_N_( ti->bits );
        ti->bits = NULL;
    }
}

void* D3D11Renderer::GetTextureContentPtr( void* p_texturedata )
{
    TextureInfos* ti = (TextureInfos*)p_texturedata;
    if( !ti->content_access )
    {
        _DSEXCEPTION( "trying to access content for a non accessible texture content" )
    }

    return ti->bits;
}

bool D3D11Renderer::CopyTextureContent( void* p_texturedata )
{
    DECLARE_D3D11ASSERT_VARS

    TextureInfos* ti = (TextureInfos*)p_texturedata;
    if( !ti->content_access )
    {
        _DSEXCEPTION( "trying to access content for a non accessible texture content" )
    }

    // copy GPU to GPU ...
    m_lpd3ddevcontext->CopyResource( ti->texture_clone, ti->texture );

    if( NULL == ti->bits )
    {
        return false;
    }

    long blocsize;
    int bpp;
    switch( ti->descr.Format )
    {
            case DXGI_FORMAT_R8G8B8A8_TYPELESS:
            case DXGI_FORMAT_R8G8B8A8_UNORM:
            case DXGI_FORMAT_B8G8R8A8_UNORM:
            case DXGI_FORMAT_R8G8B8A8_UINT:
            case DXGI_FORMAT_R8G8B8A8_SNORM:
            case DXGI_FORMAT_R8G8B8A8_SINT:
                bpp = 4;
                break;

            case DXGI_FORMAT_R16_FLOAT:
                bpp = 2;
                break;

            case DXGI_FORMAT_R32_FLOAT:
                bpp = 4;
                break;

            case DXGI_FORMAT_R32G32B32A32_FLOAT:
                bpp = 16;
                break;

            case DXGI_FORMAT_R16G16B16A16_FLOAT:
                bpp = 8;
                break;

            default:
                bpp = -1;
                break;
    }

    if( bpp != -1 )
    {
        blocsize = bpp * ti->descr.Width * ti->descr.Height;            
    }
    else
    {
        return false;
    }

    D3D11_MAPPED_SUBRESOURCE mappedResource;

    hRes = m_lpd3ddevcontext->Map( ti->texture_clone, 0, D3D11_MAP_READ, 0, &mappedResource );
    D3D11_CHECK( Map )

    memcpy( ti->bits, mappedResource.pData, blocsize );

    m_lpd3ddevcontext->Unmap( ti->texture, 0 );

    return true;
}

bool D3D11Renderer::UpdateTextureContent( void* p_texturedata )
{
    DECLARE_D3D11ASSERT_VARS

    TextureInfos* ti = (TextureInfos*)p_texturedata;
    if( !ti->content_access )
    {
        _DSEXCEPTION( "trying to access content for a non accessible texture content" )
    }

    if( NULL == ti->bits )
    {
        return false;
    }

    long blocsize;
    int bpp;
    switch( ti->descr.Format )
    {
            case DXGI_FORMAT_R8G8B8A8_TYPELESS:
            case DXGI_FORMAT_R8G8B8A8_UNORM:
            case DXGI_FORMAT_B8G8R8A8_UNORM:
            case DXGI_FORMAT_R8G8B8A8_UINT:
            case DXGI_FORMAT_R8G8B8A8_SNORM:
            case DXGI_FORMAT_R8G8B8A8_SINT:
                bpp = 4;
                break;

            case DXGI_FORMAT_R16_FLOAT:
                bpp = 2;
                break;

            case DXGI_FORMAT_R32_FLOAT:
                bpp = 4;
                break;

            case DXGI_FORMAT_R32G32B32A32_FLOAT:
                bpp = 16;
                break;

            default:
                bpp = -1;
                break;
    }

    if( bpp != -1 )
    {
        blocsize = bpp * ti->descr.Width * ti->descr.Height;            
    }
    else
    {
        return false;
    }

    if( ti->rendertextureTargetViews.size() > 0 )
    {
        // vouloir updater une render texture = non sens
        return false;
    }
    else
    { 
        D3D11_MAPPED_SUBRESOURCE mappedResource;

        hRes = m_lpd3ddevcontext->Map( ti->texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
        D3D11_CHECK( Map )

        memcpy( mappedResource.pData, ti->bits, blocsize );
        m_lpd3ddevcontext->Unmap( ti->texture, 0 );
    }

    return true;
}

bool D3D11Renderer::CreateShaders( DrawSpace::Core::Fx* p_fx, void** p_data )
{
    DECLARE_D3D11ASSERT_VARS

	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMALE", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 5, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 6, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 7, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 8, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "BINORMALE", 0, DXGI_FORMAT_R32G32B32_FLOAT,   0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

    dsstring hash;
    p_fx->GetShadersMD5( hash );

    if( m_shaders_base.count( hash ) > 0 )
    {
        *p_data = (void *)m_shaders_base[hash];
        return true;
    }

    ShadersData* sdata = _DRAWSPACE_NEW_( ShadersData, ShadersData );

    if( p_fx->GetShadersListSize() )
    {
        ID3D11VertexShader*         vs;
        ID3D11PixelShader*          ps;
        ID3D11InputLayout*          input_layout;

        /////////////////// Shaders loading

        Core::Shader* vertex_shader = p_fx->GetShader( 0 );
        Core::Shader* pixel_shader = p_fx->GetShader( 1 );

        dsstring vshader_path, pshader_path;

        vertex_shader->GetPath( vshader_path );
        pixel_shader->GetPath( pshader_path );

        if( !vertex_shader->IsCompiled() )
        {
			_DSEXCEPTION("vertex shader shall be compiled !!")
			return false;
        }
        else
        {
			void* shader_bc{ vertex_shader->GetData() };
			size_t shader_bc_length{ vertex_shader->GetDataSize() };

            hRes = m_lpd3ddevice->CreateVertexShader(shader_bc, shader_bc_length, NULL, &vs );
            D3D11_CHECK( CreateVertexShader );

            hRes = m_lpd3ddevice->CreateInputLayout( layout, ARRAYSIZE( layout ), vertex_shader->GetData(), vertex_shader->GetDataSize(), &input_layout );
            D3D11_CHECK( CreateInputLayout )
        }

        sdata->input_layout = input_layout;

        sdata->vertex_shader = vs; 

        if( !pixel_shader->IsCompiled() )
        {
			_DSEXCEPTION("pixel shader shall be compiled !!")
			return false;
        }
        else
        {
			void* shader_bc{ pixel_shader->GetData() };
			size_t shader_bc_length{ pixel_shader->GetDataSize() };

            hRes = m_lpd3ddevice->CreatePixelShader( shader_bc, shader_bc_length, NULL, &ps );
            D3D11_CHECK( CreatePixelShader );       
        }

        sdata->pixel_shader = ps;
    }

    *p_data = (void*)sdata;

    m_shaders_base[hash] = sdata;

    return true;

}

bool D3D11Renderer::SetShaders( void* p_data )
{
	ShadersData* sdata = (ShadersData*)p_data;

    m_lpd3ddevcontext->IASetInputLayout( sdata->input_layout );
    m_lpd3ddevcontext->VSSetShader( sdata->vertex_shader, NULL, 0 );
    m_lpd3ddevcontext->PSSetShader( sdata->pixel_shader, NULL, 0 );

    return true;
}



bool D3D11Renderer::CreateShaderBytes(char* p_source, int p_source_length, int p_shadertype, const dsstring& p_path, const dsstring& p_includes_path, void** p_data)
{
	DECLARE_D3D11ASSERT_VARS

	bool status{ false };

	ShaderBytesData* sdata = _DRAWSPACE_NEW_(ShaderBytesData, ShaderBytesData);
	sdata->blob = nullptr;

	ID3DBlob* pBlob { nullptr };
	ID3DBlob* pErrBlob{ nullptr };

	D3D10Include include_mgmt(p_includes_path);

	hRes = compile_shader_from_mem(p_source, p_source_length, p_path.c_str(), (p_shadertype == 0 ? "vs_main" : "ps_main"), (p_shadertype == 0 ? "vs_4_0" : "ps_4_0"), &include_mgmt, &pBlob, &pErrBlob);

	if (S_OK != hRes)
	{
		if (pErrBlob != NULL)
		{
			sdata->blob = pErrBlob;
		}
		sdata->compilation_success = false;
	}
	else
	{
		sdata->blob = pBlob;
		sdata->compilation_success = true;
		status = true;
	}

	*p_data = (void*)sdata;

	m_shadersbytes_base.insert(sdata);

	return status;
}

bool D3D11Renderer::GetShaderCompilationStatus(void* p_data)
{
	ShaderBytesData* sdata = (ShaderBytesData*)p_data;
	return sdata->compilation_success;
}

void* D3D11Renderer::GetShaderBytes(void* p_data)
{
	ShaderBytesData* sdata = (ShaderBytesData*)p_data;

	if (sdata->blob)
	{
		return sdata->blob->GetBufferPointer();
	}
	return NULL;
}

size_t D3D11Renderer::GetShaderBytesLength(void* p_data)
{
	ShaderBytesData* sdata = (ShaderBytesData*)p_data;
	if (sdata->blob)
	{
		return sdata->blob->GetBufferSize();
	}
	return 0;
}

dsstring D3D11Renderer::GetShaderCompilationError(void* p_data)
{
	ShaderBytesData* sdata = (ShaderBytesData*)p_data;
	dsstring error_text;

	if (sdata->blob)
	{
		error_text = dsstring((char*)sdata->blob->GetBufferPointer());
	}

	return error_text;
}

void D3D11Renderer::ReleaseShaderBytes(void* p_data)
{
	ShaderBytesData* sdata = (ShaderBytesData*)p_data;

	m_shadersbytes_base.erase(sdata);

	if (sdata->blob)
	{
		sdata->blob->Release();
	}
	_DRAWSPACE_DELETE_(sdata);	
}

void D3D11Renderer::EnableTrianglesPrimitives(void)
{
    DECLARE_D3D11ASSERT_VARS
    m_lpd3ddevcontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void D3D11Renderer::EnableLinesPrimitives(void)
{
    DECLARE_D3D11ASSERT_VARS
    m_lpd3ddevcontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}


bool D3D11Renderer::ApplyRenderStatesIn( void* p_data )
{
	DrawSpace::Core::Fx* fx = (DrawSpace::Core::Fx*)p_data;

    long nb_rs_in = fx->GetRenderStatesInListSize();
    for( long i = 0; i < nb_rs_in; i++ )
    {
        DrawSpace::Core::RenderState rs = fx->GetRenderStateIn( i );
        SetRenderState( &rs );
    }
    return true;
}

bool D3D11Renderer::ApplyRenderStatesOut( void* p_data )
{
	DrawSpace::Core::Fx* fx = (DrawSpace::Core::Fx*)p_data;

    long nb_rs_out = fx->GetRenderStatesOutListSize();
    for( long i = 0; i < nb_rs_out; i++ )
    {
        DrawSpace::Core::RenderState rs = fx->GetRenderStateOut( i );
        SetRenderState( &rs );
    }
    return true;
}

bool D3D11Renderer::SetFxShaderParams( int p_shader_index, long p_register, DrawSpace::Utils::Vector& p_vector )
{
	switch( p_shader_index )
	{
		case 0:

			// vertex shader params application		
			set_vertexshader_constants_vec( p_register, p_vector );
			break;

		case 1:

			// pixel shader params application
			set_pixelshader_constants_vec( p_register, p_vector );
			break;

        default:
            return false;
	}

	return false;
}

bool D3D11Renderer::SetFxShaderMatrix( int p_shader_index, long p_register, DrawSpace::Utils::Matrix& p_mat )
{
	switch( p_shader_index )
	{
		case 0:

			// vertex shader params application		
			set_vertexshader_constants_mat( p_register, p_mat );
			break;

		case 1:

			// pixel shader params application
			set_pixelshader_constants_mat( p_register, p_mat );
			break;

        default:
            return false;
	}

	return false;
}

bool D3D11Renderer::SetShaderVectorBuffer(int p_shader_index, long p_register, const std::vector<DrawSpace::Utils::Vector>& p_vectors)
{
	if (p_vectors.size() > NbMaxVectorForShadersBuffers)
	{
		_DSEXCEPTION("Too many vectors");
	}

	long index = p_register;

	switch (p_shader_index)
	{
		case 0:
			for (auto e : p_vectors)
			{
				m_vertexshader_legacyargs.vector[index].x = e[0];
				m_vertexshader_legacyargs.vector[index].y = e[1];
				m_vertexshader_legacyargs.vector[index].z = e[2];
				m_vertexshader_legacyargs.vector[index].w = e[3];
				index++;
			}
			break;

		case 1:
			for (auto e : p_vectors)
			{
				m_pixelshader_legacyargs.vector[index].x = e[0];
				m_pixelshader_legacyargs.vector[index].y = e[1];
				m_pixelshader_legacyargs.vector[index].z = e[2];
				m_pixelshader_legacyargs.vector[index].w = e[3];
				index++;
			}
			break;

		default:
			return false;
	}
	return true;
}

bool D3D11Renderer::DrawMeshe( DrawSpace::Utils::Matrix p_world, DrawSpace::Utils::Matrix p_view, DrawSpace::Utils::Matrix p_proj )
{

    // setting transformation
    DrawSpace::Utils::Matrix final_view;
    DrawSpace::Utils::Matrix inv;
    DrawSpace::Utils::Matrix result;

    inv.Identity();
    inv( 2, 2 ) = -1.0;
    final_view = p_view * inv;

    Transformation chain;    
    chain.PushMatrix( p_proj );
    chain.PushMatrix( final_view );
    chain.PushMatrix( p_world );
    chain.BuildResult();
    chain.GetResult( &result );
    result.Transpose();

    set_vertexshader_constants_mat( 0, result );
    set_pixelshader_constants_mat( 100, result );
    
    //////////////////////////////////////////////////////////////////////

    DrawSpace::Utils::Matrix proj = p_proj;
    DrawSpace::Utils::Matrix world = p_world;
    DrawSpace::Utils::Matrix view = p_view;
    DrawSpace::Utils::Matrix cam = p_view;
    DrawSpace::Utils::Matrix worldview = world * view;
    worldview.Transpose();
    
    set_vertexshader_constants_mat( 4, worldview );
    set_pixelshader_constants_mat( 104, worldview );

	//////////////////////////////////////////////////////////////////////
    
    world.Transpose();
    view.Transpose();
    cam.Inverse();
    cam.Transpose();

    set_vertexshader_constants_mat( 8, world );
    set_vertexshader_constants_mat( 12, view );

    set_pixelshader_constants_mat( 108, world );
    set_pixelshader_constants_mat( 112, view );

	//////////////////////////////////////////////////////////////////////

    set_vertexshader_constants_mat( 16, cam );
    set_pixelshader_constants_mat( 116, cam );


    proj.Transpose();
    set_vertexshader_constants_mat( 20, proj );
    set_pixelshader_constants_mat( 120, proj );

    // update des shaders legacy constants buffers...

    m_lpd3ddevcontext->UpdateSubresource( m_vertexshader_legacyargs_buffer, 0, NULL, &m_vertexshader_legacyargs, 0, 0 );
    m_lpd3ddevcontext->UpdateSubresource( m_pixelshader_legacyargs_buffer, 0, NULL, &m_pixelshader_legacyargs, 0, 0 );

    ///////////////

    m_lpd3ddevcontext->VSSetConstantBuffers( 0, 1, &m_vertexshader_legacyargs_buffer );
    m_lpd3ddevcontext->PSSetConstantBuffers( 0, 1, &m_pixelshader_legacyargs_buffer );
    m_lpd3ddevcontext->DrawIndexed( m_next_nbtriangles * 3, 0, 0 );

    return true;
}

bool D3D11Renderer::DrawLineMeshe(DrawSpace::Utils::Matrix p_world, DrawSpace::Utils::Matrix p_view, DrawSpace::Utils::Matrix p_proj)
{
    // setting transformation
    DrawSpace::Utils::Matrix final_view;
    DrawSpace::Utils::Matrix inv;
    DrawSpace::Utils::Matrix result;

    inv.Identity();
    inv(2, 2) = -1.0;
    final_view = p_view * inv;

    Transformation chain;
    chain.PushMatrix(p_proj);
    chain.PushMatrix(final_view);
    chain.PushMatrix(p_world);
    chain.BuildResult();
    chain.GetResult(&result);
    result.Transpose();

    set_vertexshader_constants_mat(0, result);
    set_pixelshader_constants_mat(100, result);

    //////////////////////////////////////////////////////////////////////

    DrawSpace::Utils::Matrix proj = p_proj;
    DrawSpace::Utils::Matrix world = p_world;
    DrawSpace::Utils::Matrix view = p_view;
    DrawSpace::Utils::Matrix cam = p_view;
    DrawSpace::Utils::Matrix worldview = world * view;
    worldview.Transpose();

    set_vertexshader_constants_mat(4, worldview);
    set_pixelshader_constants_mat(104, worldview);

    //////////////////////////////////////////////////////////////////////

    world.Transpose();
    view.Transpose();
    cam.Inverse();
    cam.Transpose();

    set_vertexshader_constants_mat(8, world);
    set_vertexshader_constants_mat(12, view);

    set_pixelshader_constants_mat(108, world);
    set_pixelshader_constants_mat(112, view);

    //////////////////////////////////////////////////////////////////////

    set_vertexshader_constants_mat(16, cam);
    set_pixelshader_constants_mat(116, cam);


    proj.Transpose();
    set_vertexshader_constants_mat(20, proj);
    set_pixelshader_constants_mat(120, proj);

    // update des shaders legacy constants buffers...

    m_lpd3ddevcontext->UpdateSubresource(m_vertexshader_legacyargs_buffer, 0, NULL, &m_vertexshader_legacyargs, 0, 0);
    m_lpd3ddevcontext->UpdateSubresource(m_pixelshader_legacyargs_buffer, 0, NULL, &m_pixelshader_legacyargs, 0, 0);

    ///////////////

    m_lpd3ddevcontext->VSSetConstantBuffers(0, 1, &m_vertexshader_legacyargs_buffer);
    m_lpd3ddevcontext->PSSetConstantBuffers(0, 1, &m_pixelshader_legacyargs_buffer);
    m_lpd3ddevcontext->DrawIndexed(m_next_nblines * 2, 0, 0);

    return true;
}

bool D3D11Renderer::set_cache_rs( void )
{
    bool status = true;
    DECLARE_D3D11ASSERT_VARS

    D3D11_RASTERIZER_DESC currRS = m_currentRSDesc;
    MD5 md5;

    dsstring rsdesc_key = md5.digestMemory( (BYTE*)&currRS, sizeof( D3D11_RASTERIZER_DESC ) );
    
    if( m_rsCache.count( rsdesc_key ) > 0 )
    {
        m_lpd3ddevcontext->RSSetState( m_rsCache[rsdesc_key].rs_state );
    }
    else
    {
        ID3D11RasterizerState* rs;
        hRes = m_lpd3ddevice->CreateRasterizerState( &currRS, &rs );
        D3D11_CHECK( CreateRasterizerState )
        m_lpd3ddevcontext->RSSetState( rs );

        // create new entry in cache
        RSCacheEntry cache_e = { currRS, rs };
        m_rsCache[rsdesc_key] = cache_e; // store in cache
    }

    return status;
}

bool D3D11Renderer::set_cache_blendstate( void )
{
    bool status = true;
    DECLARE_D3D11ASSERT_VARS

    D3D11_BLEND_DESC currBlendDesc = m_currentBlendDesc;
    MD5 md5;

    FLOAT bvals[4] = { 0.0, 0.0, 0.0, 0.0 };

    dsstring bsdesc_key = md5.digestMemory( (BYTE*)&currBlendDesc, sizeof( D3D11_BLEND_DESC ) );

    if( m_bsCache.count( bsdesc_key ) > 0 )
    {
        m_lpd3ddevcontext->OMSetBlendState( m_bsCache[bsdesc_key].bs_state, bvals, 0xffffffff );
    }
    else
    {
        ID3D11BlendState* bs;
        hRes = m_lpd3ddevice->CreateBlendState( &currBlendDesc, &bs );
        D3D11_CHECK( CreateBlendState )
        m_lpd3ddevcontext->OMSetBlendState( bs, bvals, 0xffffffff );

        // create new entry in cache
        BSCacheEntry cache_e = { currBlendDesc, bs };
        m_bsCache[bsdesc_key] = cache_e; // store in cache
    }

    return status;
}

void D3D11Renderer::SetRenderState( DrawSpace::Core::RenderState* p_renderstate )
{    
    const auto arg{ p_renderstate->GetArg() };

    static const std::map<dsstring, ID3D11SamplerState*> translate_samplerstate =
    {
        { "none",               m_pointFilterSamplerState              },
        { "point",              m_pointFilterSamplerState              },
        { "linear",             m_linearFilterSamplerState             },
        { "anisotropic",        m_anisotropicFilterSamplerState        },
        { "point_uvwrap",       m_pointFilterSamplerState_uvwrap       },
        { "linear_uvwrap",      m_linearFilterSamplerState_uvwrap      },
        { "anisotropic_uvwrap", m_anisotropicFilterSamplerState_uvwrap }
    };

    switch( p_renderstate->GetOperation() )
    {
        case DrawSpace::Core::RenderState::SETCULLING:
            
            if( "none" == arg )
            {
                m_currentRSDesc.CullMode = D3D11_CULL_NONE;
                m_currentRSDesc.FrontCounterClockwise = FALSE;
            }
            else if( "cw" == arg )
            {
                // cull cw
                m_currentRSDesc.CullMode = D3D11_CULL_FRONT;   
                m_currentRSDesc.FrontCounterClockwise = FALSE;
                // cull cw
            }
            else
            {
                // cull ccw
                m_currentRSDesc.CullMode = D3D11_CULL_BACK;   
                m_currentRSDesc.FrontCounterClockwise = FALSE;
                // cull ccw
            }

            set_cache_rs();
            break;

        case DrawSpace::Core::RenderState::ENABLEZBUFFER:
            if( "true" == arg )
            {
                m_lpd3ddevcontext->OMSetDepthStencilState( m_DSState_DepthTestEnabled, 1 );
            }
            else
            {
                m_lpd3ddevcontext->OMSetDepthStencilState( m_DSState_DepthTestDisabled, 1 );
            }
            break;

        case DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE:
            {
                /*if ("special" == arg)
                {
                    ID3D11SamplerState* ss_array[] = { nullptr };

                    ss_array[0] = m_linearFilterSamplerState;
                    m_lpd3ddevcontext->PSSetSamplers(0, 1, ss_array);

                    ss_array[0] = m_linearFilterSamplerState;
                    m_lpd3ddevcontext->PSSetSamplers(1, 1, ss_array);

                    ss_array[0] = m_linearFilterSamplerState_uvwrap;
                    m_lpd3ddevcontext->PSSetSamplers(2, 1, ss_array);

                    ss_array[0] = m_linearFilterSamplerState_uvwrap;
                    m_lpd3ddevcontext->PSSetSamplers(3, 1, ss_array);

                    ss_array[0] = m_linearFilterSamplerState_uvwrap;
                    m_lpd3ddevcontext->PSSetSamplers(4, 1, ss_array);

                    ss_array[0] = m_linearFilterSamplerState_uvwrap;
                    m_lpd3ddevcontext->PSSetSamplers(5, 1, ss_array);

                    ss_array[0] = m_linearFilterSamplerState_uvwrap;
                    m_lpd3ddevcontext->PSSetSamplers(6, 1, ss_array);

                    ss_array[0] = m_linearFilterSamplerState;
                    m_lpd3ddevcontext->PSSetSamplers(7, 1, ss_array);
                }
                else */if ("extended" == arg)
                {
                    ID3D11SamplerState* ss_array[] = { nullptr };

                    const auto args_list{ p_renderstate->GetExtendedArgs() };
                    auto index{ 0 };
                    for (const auto& e : args_list)
                    {
                        ss_array[0] = translate_samplerstate.at(e);
                        m_lpd3ddevcontext->PSSetSamplers(index++, 1, ss_array);
                    }
                }
                else
                {
                    ID3D11SamplerState* ss_array[] = { translate_samplerstate.at(arg) };
                    for (auto i = 0; i < 8; i++)
                    {
                        m_lpd3ddevcontext->PSSetSamplers(i, 1, ss_array);
                    }
                }               
            }
            break;

        case DrawSpace::Core::RenderState::SETVERTEXTEXTUREFILTERTYPE:
            {
                /*if ("special" == arg)
                {
                    ID3D11SamplerState* ss_array[] = { nullptr };

                    ss_array[0] = m_linearFilterSamplerState;
                    m_lpd3ddevcontext->VSSetSamplers(0, 1, ss_array);

                    ss_array[0] = m_linearFilterSamplerState;
                    m_lpd3ddevcontext->VSSetSamplers(1, 1, ss_array);

                    ss_array[0] = m_linearFilterSamplerState_uvwrap;
                    m_lpd3ddevcontext->VSSetSamplers(2, 1, ss_array);

                    ss_array[0] = m_linearFilterSamplerState_uvwrap;
                    m_lpd3ddevcontext->VSSetSamplers(3, 1, ss_array);

                    ss_array[0] = m_linearFilterSamplerState_uvwrap;
                    m_lpd3ddevcontext->VSSetSamplers(4, 1, ss_array);

                    ss_array[0] = m_linearFilterSamplerState_uvwrap;
                    m_lpd3ddevcontext->VSSetSamplers(5, 1, ss_array);

                    ss_array[0] = m_linearFilterSamplerState_uvwrap;
                    m_lpd3ddevcontext->VSSetSamplers(6, 1, ss_array);

                    ss_array[0] = m_linearFilterSamplerState;
                    m_lpd3ddevcontext->VSSetSamplers(7, 1, ss_array);
                }
                else */if ("extended" == arg)
                {
                    ID3D11SamplerState* ss_array[] = { nullptr };

                    const auto args_list{ p_renderstate->GetExtendedArgs() };
                    auto index{ 0 };
                    for (const auto& e : args_list)
                    {
                        ss_array[0] = translate_samplerstate.at(e);
                        m_lpd3ddevcontext->VSSetSamplers(index++, 1, ss_array);
                    }
                }
                else
                {
                    ID3D11SamplerState* ss_array[] = { translate_samplerstate.at(arg) };
                    for (auto i = 0; i < 8; i++)
                    {
                        m_lpd3ddevcontext->VSSetSamplers(i, 1, ss_array);
                    }
                }
            }
            break;

        case DrawSpace::Core::RenderState::SETFILLMODE:
            {
                if( "line" == arg )
                {
                    m_currentRSDesc.FillMode = D3D11_FILL_WIREFRAME;
                }
                else if( "solid" == arg )
                {
                    m_currentRSDesc.FillMode = D3D11_FILL_SOLID;
                }
   
                set_cache_rs();
            }
            break;

        case DrawSpace::Core::RenderState::ALPHABLENDENABLE:
            {
                if( "true" == arg )
                {
                    m_currentBlendDesc.RenderTarget[0].BlendEnable = TRUE;
                }
                else if( "false" == arg )
                {
                    m_currentBlendDesc.RenderTarget[0].BlendEnable = FALSE;
                }

                set_cache_blendstate();
            }
            break;

        case DrawSpace::Core::RenderState::ALPHABLENDOP:
            {
                if( "add" == arg )
                {
                    m_currentBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
                    m_currentBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
                }
                else if( "sub" == arg )
                {
                    m_currentBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_SUBTRACT;
                    m_currentBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_SUBTRACT;
                }
                else if( "revsub" == arg )
                {
                    m_currentBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
                    m_currentBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_REV_SUBTRACT;
                }
                else if( "min" == arg )
                {
                    m_currentBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MIN;
                    m_currentBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MIN;
                }
                else if( "max" == arg )
                {
                    m_currentBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MAX;
                    m_currentBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
                }

                set_cache_blendstate();
            }
            break;

        case DrawSpace::Core::RenderState::ALPHABLENDFUNC:
            {
                if( arg != "always" )
                {
                    _DSEXCEPTION( "unsupported alpha blending func for D3D11" )
                }
                set_cache_blendstate();
            }
            break;

        case DrawSpace::Core::RenderState::ALPHABLENDDEST:
            {
                if( "zero" == arg )
                {
                    m_currentBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
                    m_currentBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
                }
                else if( "one" == arg )
                {
                    m_currentBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
                    m_currentBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
                }
                else if( "srccolor" == arg )
                {
                    m_currentBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
                    m_currentBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_SRC_COLOR;
                }
                else if( "invsrccolor" == arg )
                {
                    m_currentBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;
                    m_currentBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_COLOR;
                }
                else if( "srcalpha" == arg )
                {
                    m_currentBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_ALPHA;
                    m_currentBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_SRC_ALPHA;
                }
                else if( "invsrcalpha" == arg )
                {
                    m_currentBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
                    m_currentBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
                }
                else if( "destalpha" == arg )
                {
                    m_currentBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_DEST_ALPHA;
                    m_currentBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;   
                }
                else if( "invdestalpha" == arg )
                {
                    m_currentBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_DEST_ALPHA;
                    m_currentBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;    
                }
                else if( "destcolor" == arg )
                {
                    m_currentBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_DEST_COLOR;
                    m_currentBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_COLOR; 
                }
                else if( "invdestcolor" == arg )
                {
                    m_currentBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_DEST_COLOR;
                    m_currentBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_DEST_COLOR;
                }
                set_cache_blendstate();
            }
            break;

        case DrawSpace::Core::RenderState::ALPHABLENDSRC:
            {
                if( "zero" == arg )
                {
                    m_currentBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
                    m_currentBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
                }
                else if( "one" == arg )
                {
                    m_currentBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
                    m_currentBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
                }
                else if( "srccolor" == arg )
                {
                    m_currentBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;
                    m_currentBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_COLOR;
                }
                else if( "invsrccolor" == arg )
                {
                    m_currentBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_INV_SRC_COLOR;
                    m_currentBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_COLOR;
                }
                else if( "srcalpha" == arg )
                {
                    m_currentBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
                    m_currentBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
                }
                else if( "invsrcalpha" == arg )
                {
                    m_currentBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_INV_SRC_ALPHA;
                    m_currentBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
                }
                else if( "destalpha" == arg )
                {
                    m_currentBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_DEST_ALPHA;
                    m_currentBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;   
                }
                else if( "invdestalpha" == arg )
                {
                    m_currentBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_INV_DEST_ALPHA;
                    m_currentBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;    
                }
                else if( "destcolor" == arg )
                {
                    m_currentBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_DEST_COLOR;
                    m_currentBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_DEST_COLOR; 
                }
                else if( "invdestcolor" == arg )
                {
                    m_currentBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_INV_DEST_COLOR;
                    m_currentBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_COLOR;
                }
                set_cache_blendstate();
            }

            break;
    }
}

void D3D11Renderer::set_vertexshader_constants_vec( DWORD p_startreg, const DrawSpace::Utils::Vector& p_vec )
{
    m_vertexshader_legacyargs.vector[p_startreg].x = p_vec[0];
    m_vertexshader_legacyargs.vector[p_startreg].y = p_vec[1];
    m_vertexshader_legacyargs.vector[p_startreg].z = p_vec[2];
    m_vertexshader_legacyargs.vector[p_startreg].w = p_vec[3];
}

void D3D11Renderer::set_pixelshader_constants_vec( DWORD p_startreg, const DrawSpace::Utils::Vector& p_vec )
{
    m_pixelshader_legacyargs.vector[p_startreg].x = p_vec[0];
    m_pixelshader_legacyargs.vector[p_startreg].y = p_vec[1];
    m_pixelshader_legacyargs.vector[p_startreg].z = p_vec[2];
    m_pixelshader_legacyargs.vector[p_startreg].w = p_vec[3]; 
}

void D3D11Renderer::set_vertexshader_constants_mat( DWORD p_startreg, const DrawSpace::Utils::Matrix& p_mat )
{
    for( int i = 0; i < 4; i++ )
    {
        for( int j = 0; j < 4; j++ )
        {
            m_vertexshader_legacyargs.matrix[p_startreg]( i, j ) = p_mat( i, j );
        }
    }
}

void D3D11Renderer::set_pixelshader_constants_mat( DWORD p_startreg, const DrawSpace::Utils::Matrix& p_mat )
{
    for( int i = 0; i < 4; i++ )
    {
        for( int j = 0; j < 4; j++ )
        {
            m_pixelshader_legacyargs.matrix[p_startreg]( i, j ) = p_mat( i, j );
        }
    }
}

void D3D11Renderer::GetRenderCharacteristics( Characteristics& p_characteristics )
{
    p_characteristics = m_characteristics;
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
		FW1_NOGEOMETRYSHADER | FW1_RESTORESTATE// Flags
        
	);
}

void D3D11Renderer::PointProjection( DrawSpace::Utils::Matrix p_view, DrawSpace::Utils::Matrix p_proj, DrawSpace::Utils::Vector& p_point, dsreal& p_outx, dsreal& p_outy, dsreal& p_outz )
{
    DrawSpace::Utils::Matrix final_view;
    DrawSpace::Utils::Matrix inv;
    DrawSpace::Utils::Matrix proj;

    DrawSpace::Utils::Vector res;
    DrawSpace::Utils::Vector point = p_point;

    inv.Identity();
    inv( 2, 2 ) = -1.0;

    final_view = p_view * inv;

    DrawSpace::Utils::Vector point2;
    final_view.Transform( &point, &point2 );

    p_outz = point2[2];

    if( point2[2] < 1.0 )
    {
        point2[2] = 1.0;
    }

    p_proj.Transform( &point2, &res );

    p_outx = 0.5 * m_characteristics.width_viewport * ( res[0] / ( res[2] + 1.0 ) );
    p_outy = 0.5 * m_characteristics.height_viewport * ( res[1] / ( res[2] + 1.0 ) );
}

HRESULT D3D11Renderer::compile_shader_from_mem( void* p_data, int p_size, LPCTSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3D10Include* p_include, ID3DBlob** ppBlobOut, ID3DBlob** ppBlobErrOut )
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.

#ifdef _DEBUG
    dwShaderFlags |= D3DCOMPILE_DEBUG;
    //dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;  // decommmenter ce flag si besoin de debugger les shaders avec Graphics diagnostics tool
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DX11CompileFromMemory( (LPCTSTR)p_data, p_size, szFileName, NULL, p_include, szEntryPoint, szShaderModel, dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL );
    if( FAILED(hr) )
    {
        if( pErrorBlob != NULL )
        {
            *ppBlobErrOut = pErrorBlob;
        }

        return hr;
    }
    return S_OK;
}

bool D3D11Renderer::GUI_InitSubSystem( void )
{
    if( !m_lpd3ddevice || !m_lpd3ddevcontext )
    {
        _DSEXCEPTION( "Cannot initialize GUI sub system cause D3D11 is currently not initialized" )
    }

    CEGUI::Direct3D11Renderer::bootstrapSystem( m_lpd3ddevice, m_lpd3ddevcontext );

    m_guisubsystem.SetReady( true );
    return true;
}

void D3D11Renderer::GUI_ReleaseSubSystem( void )
{
    if( m_lpd3ddevice )
    {
        CEGUI::Direct3D11Renderer::destroySystem();
    }
    m_guisubsystem.SetReady( false );
}

void D3D11Renderer::GUI_Render( void )
{
    // save current RS state
    ID3D11RasterizerState* curr_rs;
    m_lpd3ddevcontext->RSGetState( &curr_rs );

    // save current blend state
    ID3D11BlendState* curr_bs;
    FLOAT bvals[4];
    UINT mask;
    m_lpd3ddevcontext->OMGetBlendState( &curr_bs, bvals, &mask );


    m_guisubsystem.RenderGUI();

    // restore my input layout, 'cause CEGUI puts its own...
    // m_lpd3ddevcontext->IASetInputLayout( m_inputLayout );

    // then restore current RS state
    m_lpd3ddevcontext->RSSetState( curr_rs );
    
    // and restore current blend state
    m_lpd3ddevcontext->OMSetBlendState( curr_bs, bvals, mask );
}

void D3D11Renderer::GUI_SetResourcesRootDirectory( const dsstring& p_path )
{
    m_guisubsystem.SetResourcesRootDirectory( p_path );
}

void D3D11Renderer::GUI_OnMouseMove( float p_xm, float p_ym, float p_dx, float p_dy )
{
    m_guisubsystem.OnMouseMove( p_xm, p_ym, p_dx, p_dy );
}

void D3D11Renderer::GUI_OnMouseLeftButtonDown( void )
{
    m_guisubsystem.OnMouseLeftButtonDown();
}

void D3D11Renderer::GUI_OnMouseLeftButtonUp( void )
{
    m_guisubsystem.OnMouseLeftButtonUp();
}

void D3D11Renderer::GUI_OnMouseRightButtonDown( void )
{
    m_guisubsystem.OnMouseRightButtonDown();
}

void D3D11Renderer::GUI_OnMouseRightButtonUp( void )
{
    m_guisubsystem.OnMouseRightButtonUp();
}

void D3D11Renderer::GUI_InitTest( void )
{
    m_guisubsystem.InitTest();
}

void D3D11Renderer::GUI_OnKeyDown( long p_key )
{
    m_guisubsystem.OnKeyDown( p_key );
}

void D3D11Renderer::GUI_OnKeyUp( long p_key )
{
    m_guisubsystem.OnKeyUp( p_key );
}

void D3D11Renderer::GUI_OnChar( long p_key )
{
    m_guisubsystem.OnChar( p_key );
}

void D3D11Renderer::GUI_LoadLayout( const dsstring& p_layout_path, const dsstring& p_widgets_path )
{
    m_guisubsystem.LoadLayout( p_layout_path, p_widgets_path );
}

void D3D11Renderer::GUI_UnloadAllLayouts( void )
{
    m_guisubsystem.UnloadAllLayouts();
}

void D3D11Renderer::GUI_LoadScheme( const dsstring& p_scheme_path )
{
    m_guisubsystem.LoadScheme( p_scheme_path );
}

void D3D11Renderer::GUI_UnloadAllSchemes( void )
{
    m_guisubsystem.UnloadAllSchemes();
}

void D3D11Renderer::GUI_StoreWidget( const dsstring& p_layoutName, const dsstring& p_parentName, const dsstring& p_childName )
{
    m_guisubsystem.Store( p_layoutName, p_parentName, p_childName );
}

void D3D11Renderer::GUI_SetVisibleState( const dsstring& p_layoutName, const dsstring& p_widgetName, bool p_state )
{
    m_guisubsystem.SetVisibleState( p_layoutName, p_widgetName, p_state );
}

bool D3D11Renderer::GUI_IsVisible( const dsstring& p_layoutName, const dsstring& p_widgetName )
{
    return m_guisubsystem.IsVisible( p_layoutName, p_widgetName );
}

void D3D11Renderer::GUI_SetWidgetText( const dsstring& p_layoutName, const dsstring& p_widgetName, const dsstring& p_text )
{
    m_guisubsystem.SetText( p_layoutName, p_widgetName, p_text );
}

void D3D11Renderer::GUI_GetWidgetText( const dsstring& p_layoutName, const dsstring& p_widgetName, dsstring& p_outtext )
{
    m_guisubsystem.GetText( p_layoutName, p_widgetName, p_outtext );
}

void D3D11Renderer::GUI_AddListboxTextItem( const dsstring& p_layoutName, const dsstring& p_widgetName, const dsstring& p_text, unsigned int p_colors, const dsstring& p_brushImage )
{
    m_guisubsystem.AddListboxTextItem( p_layoutName, p_widgetName, p_text, p_colors, p_brushImage );
}

void D3D11Renderer::GUI_ClearListbox( const dsstring& p_layoutName, const dsstring& p_widgetName )
{
    m_guisubsystem.ClearListbox( p_layoutName, p_widgetName );
}

bool D3D11Renderer::GUI_GetListboxFirstSelectedItemIndex( const dsstring& p_layoutName, const dsstring& p_widgetName, int& p_index, dsstring& p_text )
{
    return m_guisubsystem.GetListboxFirstSelectedItemIndex( p_layoutName, p_widgetName, p_index, p_text );
}


void D3D11Renderer::GUI_AddComboboxTextItem( const dsstring& p_layoutName, const dsstring& p_widgetName, const dsstring& p_text, unsigned int p_colors, const dsstring& p_brushImage )
{
    m_guisubsystem.AddComboboxTextItem( p_layoutName, p_widgetName, p_text, p_colors, p_brushImage );
}

void D3D11Renderer::GUI_ClearCombobox( const dsstring& p_layoutName, const dsstring& p_widgetName )
{
    m_guisubsystem.ClearCombobox( p_layoutName, p_widgetName );
}

void D3D11Renderer::GUI_SetComboBoxItemSelectionState( const dsstring& p_layoutName, const dsstring& p_widgetName, int p_index, bool p_state )
{
    m_guisubsystem.SetComboBoxItemSelectionState( p_layoutName, p_widgetName, p_index, p_state );
}

int D3D11Renderer::GUI_GetComboBoxSelectionIndex( const dsstring& p_layoutName, const dsstring& p_widgetName )
{
    return m_guisubsystem.GetComboBoxSelectionIndex( p_layoutName, p_widgetName );
}

bool D3D11Renderer::GUI_IsCheckBoxChecked( const dsstring& p_layoutName, const dsstring& p_widgetName )
{
    return m_guisubsystem.IsCheckBoxChecked( p_layoutName, p_widgetName );
}

void D3D11Renderer::GUI_SetCheckboxState( const dsstring& p_layoutName, const dsstring& p_widgetName, bool p_state )
{
    m_guisubsystem.SetCheckboxState( p_layoutName, p_widgetName, p_state );
}

void D3D11Renderer::GUI_SetMultiLineEditboxCaretIndex(const dsstring& p_layoutName, const dsstring& p_widgetName, int p_index)
{
    m_guisubsystem.SetMultiLineEditboxCaretIndex(p_layoutName, p_widgetName, p_index);
}

void D3D11Renderer::GUI_RegisterPushButtonEventClickedHandler( DrawSpace::Core::BaseCallback2<void, const dsstring&, const dsstring&>* p_handler )
{
    m_guisubsystem.RegisterPushButtonEventClickedHandler( p_handler );
}

void D3D11Renderer::GUI_RegisterCheckboxEventStateChangedHandler( DrawSpace::Core::BaseCallback3<void, const dsstring&, const dsstring&, bool>* p_handler )
{
    m_guisubsystem.RegisterCheckboxEventStateChangedHandler( p_handler );
}

void D3D11Renderer::GUI_SetLayout( const dsstring& p_layoutpath )
{
    m_guisubsystem.SetLayout( p_layoutpath );
}

void D3D11Renderer::GUI_SetMouseCursorImage( const dsstring& p_image )
{
    m_guisubsystem.SetMouseCursorImage( p_image );
}

void D3D11Renderer::GUI_ShowMouseCursor( bool p_show )
{
    m_guisubsystem.ShowMouseCursor( p_show );
}

void D3D11Renderer::GUI_CreateSprite( const dsstring& p_scheme_object, const dsstring& p_spriteName )
{
    m_guisubsystem.CreateSprite( p_scheme_object, p_spriteName );
}

void D3D11Renderer::GUI_SetSpritePosition( const dsstring& p_spriteName, dsreal p_xpos, dsreal p_ypos )
{
    m_guisubsystem.SetSpritePosition( p_spriteName, p_xpos, p_ypos );
}

void D3D11Renderer::GUI_SetSpriteImage( const dsstring& p_spriteName, const dsstring& p_image )
{
    m_guisubsystem.SetSpriteImage( p_spriteName, p_image );
}

void D3D11Renderer::GUI_SetSpriteScale( const dsstring& p_spriteName, dsreal p_scale )
{
    m_guisubsystem.SetSpriteScale( p_spriteName, p_scale );
}

void D3D11Renderer::GUI_SetSpriteRotation( const dsstring& p_spriteName, const DrawSpace::Utils::Vector& p_axis, dsreal p_deg_angle )
{
    m_guisubsystem.SetSpriteRotation( p_spriteName, p_axis, p_deg_angle );
}
