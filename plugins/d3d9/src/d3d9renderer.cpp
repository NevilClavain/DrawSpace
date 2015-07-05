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

#include "d3d9renderer.h"
#include <memalloc.h>

using namespace DrawSpace;
using namespace DrawSpace::Core;
extern void TranslateD3DD9Error( HRESULT p_hRes, dsstring &p_str );

_DECLARE_DS_LOGGER( logger, "d3d9", NULL )

D3D9Renderer::D3D9Renderer( void ) :
m_lpd3d( NULL ),
m_lpd3ddevice( NULL ),
m_vertexdeclaration( NULL )
{

}

D3D9Renderer::~D3D9Renderer( void )
{

}

void D3D9Renderer::GetDescr( dsstring& p_descr )
{
    p_descr = "Direct3D9";
}

void D3D9Renderer::DumpMemoryAllocs( void )
{
    DrawSpace::Utils::MemAlloc::GetInstance()->DumpContent();
}

D3DFORMAT D3D9Renderer::find_depthbuffer_format( int p_adapterordinal, D3DFORMAT p_displayformat )
{
    HRESULT hRes = m_lpd3d->CheckDeviceFormat( p_adapterordinal, D3DDEVTYPE_HAL, p_displayformat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D32 );
    if( D3D_OK == hRes )
    {
        return D3DFMT_D32;  
    }
    else
    {
        hRes = m_lpd3d->CheckDeviceFormat( p_adapterordinal, D3DDEVTYPE_HAL, p_displayformat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X8 );
        if( D3D_OK == hRes )
        {
            return D3DFMT_D24X8;
        }
        else
        {
            hRes = m_lpd3d->CheckDeviceFormat( p_adapterordinal, D3DDEVTYPE_HAL, p_displayformat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D16 );
            if( D3D_OK == hRes )
            {
                return D3DFMT_D16;
            }
            else
            {
                return D3DFMT_UNKNOWN;
            }
        }
    }
}

bool D3D9Renderer::Init( HWND p_hwnd, bool p_fullscreen, long p_w_width, long p_w_height, DrawSpace::Logger::Configuration* p_logconf )
{
    D3DPRESENT_PARAMETERS d3dpp;
    DECLARE_D3D9ASSERT_VARS
    D3DDISPLAYMODE d3ddm;

    p_logconf->RegisterSink( &logger );
    logger.SetConfiguration( p_logconf );

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

    _DSDEBUG( logger, "begin" )

    m_hwnd = p_hwnd;
    
    m_lpd3d = Direct3DCreate9( D3D_SDK_VERSION );
    if( NULL == m_lpd3d )
    {	
        _DSDEBUG( logger, "end : ko Direct3DCreate9 fail" )
        return false;
    }

    // check for shaders model 3.0 support

    D3DCAPS9 caps;

    hRes = m_lpd3d->GetDeviceCaps( adapter, D3DDEVTYPE_HAL, &caps );
    D3D9_CHECK( GetDeviceCaps )

    DWORD vs_version_maj = D3DSHADER_VERSION_MAJOR( caps.VertexShaderVersion );
    DWORD vs_version_min = D3DSHADER_VERSION_MINOR( caps.VertexShaderVersion );
    
    DWORD ps_version_maj = D3DSHADER_VERSION_MAJOR( caps.PixelShaderVersion );
    DWORD ps_version_min = D3DSHADER_VERSION_MINOR( caps.PixelShaderVersion );

	_DSDEBUG( logger, dsstring("vertex shaders version = ") << vs_version_maj << dsstring(".") << vs_version_min << dsstring(" pixel shaders version = ") << ps_version_maj << dsstring( "." ) << ps_version_min );

    if( vs_version_maj < 3 || ps_version_maj < 3 )
    {
		_DSFATAL( logger, dsstring( "Shader model 3.0 unsupported !" ) )
        return false;
    }
    else
    {
		_DSDEBUG( logger, dsstring( "Shader model 3.0 check OK" ) )
    }

    // search for best z-buffer format
    D3DFORMAT depth_format;
    if( p_fullscreen )
    {
        depth_format = find_depthbuffer_format( adapter, m_config.m_fullscreen_format );
    }
    else
    {
        D3DDISPLAYMODE desktop_displaymode;
        m_lpd3d->GetAdapterDisplayMode( adapter, &desktop_displaymode );
        depth_format = find_depthbuffer_format( adapter, desktop_displaymode.Format );
    }

    if( D3DFMT_UNKNOWN == depth_format )
    {
        _DSFATAL( logger, "No supported depth buffer found" )
        return false;
    }

	_DSDEBUG( logger, dsstring( "Selected depth buffer format = " ) << depth_format )
    m_depthbuffer_format = depth_format;

    //device creation

    ZeroMemory( &d3dpp, sizeof( d3dpp ) );

    if( p_fullscreen )
    {
        d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
        d3dpp.hDeviceWindow = m_hwnd;
        d3dpp.Windowed   = FALSE;
        d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        d3dpp.EnableAutoDepthStencil = TRUE;
        d3dpp.AutoDepthStencilFormat = depth_format;//D3DFMT_D24X8;// TODO LATER : config specifique plugin D3D9 //m_zbuffer_depth;;
        d3dpp.Flags = 0;		
        d3dpp.BackBufferCount = 0; // ca veut dire 1 !!!!

        d3dpp.BackBufferWidth = m_config.m_fullscreen_width;
        d3dpp.BackBufferHeight = m_config.m_fullscreen_height;
        d3dpp.BackBufferFormat = m_config.m_fullscreen_format;



        d3dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
        d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
        d3dpp.FullScreen_RefreshRateInHz = m_config.m_refreshrate;

        m_characteristics.fullscreen = true;
        m_characteristics.width_resol = d3dpp.BackBufferWidth;
        m_characteristics.height_resol = d3dpp.BackBufferHeight;

		_DSDEBUG( logger, dsstring( " -> FULLSCREEN " ) << d3dpp.BackBufferWidth << dsstring( " x " ) << d3dpp.BackBufferHeight << dsstring( " format " ) << d3dpp.BackBufferFormat )
    }
    else
    {
        hRes = m_lpd3d->GetAdapterDisplayMode( adapter, &d3ddm );
        D3D9_CHECK( GetAdapterDisplayMode )

        d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
        d3dpp.hDeviceWindow = m_hwnd;
        d3dpp.Windowed   = TRUE;
        d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        d3dpp.EnableAutoDepthStencil = TRUE;
        d3dpp.AutoDepthStencilFormat = depth_format; //D3DFMT_D24X8;// TODO LATER : config specifique plugin D3D9 //m_zbuffer_depth;

        d3dpp.BackBufferFormat = d3ddm.Format;
        d3dpp.BackBufferCount = 0; // ca veut dire 1 !!!!

        d3dpp.FullScreen_RefreshRateInHz = 0;
        d3dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
        d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

        m_characteristics.fullscreen = false;
        m_characteristics.width_resol = p_w_width;
        m_characteristics.height_resol = p_w_height;

		_DSDEBUG( logger, dsstring(" -> WINDOWED ") << p_w_width << dsstring( " x " ) << p_w_height )
    }

    DWORD behavior = m_config.m_vertex_processing;

    hRes = m_lpd3d->CreateDevice( adapter, D3DDEVTYPE_HAL, m_hwnd, behavior | D3DCREATE_FPU_PRESERVE, &d3dpp, &m_lpd3ddevice );
    D3D9_CHECK( CreateDevice )

    D3DVERTEXELEMENT9 vdecl[] = 
    {
        { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
        { 0, 24, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 40, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
        { 0, 56, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
        { 0, 72, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },
        { 0, 88, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4 },
        { 0, 104, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 5 },
        { 0, 120, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 6 },
        { 0, 136, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 7 },
        { 0, 152, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 8 },
        /*{ 0, 168, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 9 },*/
        D3DDECL_END()
    };

    hRes = m_lpd3ddevice->CreateVertexDeclaration( vdecl, &m_vertexdeclaration );
    D3D9_CHECK( CreateVertexDeclaration )

    m_lpd3ddevice->SetVertexDeclaration( m_vertexdeclaration );
    
    // no default renderstate configuration
    /*
    for( long i = 0; i < 8; i++ )
    {
        m_lpd3ddevice->SetSamplerState( i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
        m_lpd3ddevice->SetSamplerState( i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    }
    m_lpd3ddevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    m_lpd3ddevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
    */

    m_lpd3ddevice->SetRenderState( D3DRS_LIGHTING, FALSE );

    //projection set, to automatically fit with the screen resolution
    float v_width, v_height;
    if( p_fullscreen )
    {
        v_width = 1.0;
        v_height = v_width * m_config.m_fullscreen_height / m_config.m_fullscreen_width;
    }
    else
    {
        v_width = 1.0;
        v_height = v_width * p_w_height / p_w_width;
    }

	_DSDEBUG( logger, dsstring("projection : v_width = ") << v_width << dsstring( " v_height = " ) << v_height );

    //SetProjection( (float)v_width, (float)v_height, 1.0f, 100000000000.0f );

    m_characteristics.width_viewport = v_width;
    m_characteristics.height_viewport = v_height;


    hRes = D3DXCreateFontA( m_lpd3ddevice, 15, 10, 0, 0, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "System", &m_font );

    _DSDEBUG( logger, "end : ok" )
    return true;
}

void D3D9Renderer::Release( void )
{
    _DSDEBUG( logger, "begin" )

    D3D9_RELEASE( m_vertexdeclaration );

    D3D9_RELEASE( m_lpd3d );
    D3D9_RELEASE( m_lpd3ddevice );

    _DSDEBUG( logger, "end" )
}

void D3D9Renderer::SetViewport( bool p_automatic, long p_vpx, long p_vpy, long p_vpwidth, long p_vpheight, float p_vpminz, float p_vpmaxz )
{
    if( p_automatic )
    {
        RECT wndrect;
        ::GetClientRect( m_hwnd, &wndrect );
        m_viewport.X = wndrect.left;
        m_viewport.Y = wndrect.top;
        m_viewport.Width = wndrect.right - wndrect.left;
        m_viewport.Height = wndrect.bottom - wndrect.top;
        m_viewport.MinZ = p_vpminz;
        m_viewport.MaxZ = p_vpmaxz;
    }
    else
    {
        m_viewport.X = p_vpx;
        m_viewport.Y = p_vpy;
        m_viewport.Width = p_vpwidth;
        m_viewport.Height = p_vpheight;
        m_viewport.MinZ = p_vpminz;
        m_viewport.MaxZ = p_vpmaxz;
    }
    m_lpd3ddevice->SetViewport( &m_viewport );
}

/*
void D3D9Renderer::SetProjection( float p_vw, float p_vh, float p_zn, float p_zf )
{
    m_projection.Perspective( p_vw, p_vh, p_zn, p_zf );
}
*/


void D3D9Renderer::BeginScreen( void )
{
    m_lpd3ddevice->BeginScene();
}

void D3D9Renderer::EndScreen( void )
{
    m_lpd3ddevice->EndScene();
}

void D3D9Renderer::FlipScreen( void )
{
    m_lpd3ddevice->Present( NULL, NULL, NULL, NULL );
}

void D3D9Renderer::ClearScreen( unsigned char p_r, unsigned char p_g, unsigned char p_b )
{
    m_lpd3ddevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( p_r, p_g, p_b ), 1.0f, 0 );
}

void D3D9Renderer::ClearDepth( dsreal p_value )
{
    m_lpd3ddevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0, 0, 0 ), (float)p_value, 0 );
}

void D3D9Renderer::BeginTarget( DrawSpace::Core::Texture* p_texture )
{
    if( m_targettextures_base.count( p_texture ) > 0 )
    {
        m_targettextures_base[p_texture].render_to_surface->BeginScene( m_targettextures_base[p_texture].surface, NULL );
    }
}

void D3D9Renderer::EndTarget( DrawSpace::Core::Texture* p_texture )
{
    if( m_targettextures_base.count( p_texture ) > 0 )
    {
        m_targettextures_base[p_texture].render_to_surface->EndScene( 0 );
    }
}


bool D3D9Renderer::CreateMeshe( DrawSpace::Core::Meshe* p_meshe, void** p_data )
{
    DECLARE_D3D9ASSERT_VARS
    
    Core::Meshe* meshe = p_meshe;

    dsstring hash;
    p_meshe->GetMD5( hash );

    if( m_meshes_base.count( hash ) > 0 )
    {
        *p_data = (void *)m_meshes_base[hash];
        return true;
    }

    long nb_vertices = meshe->GetVertexListSize();
    long nb_triangles = meshe->GetTrianglesListSize();

    MesheData* meshe_data = _DRAWSPACE_NEW_( MesheData, MesheData );

    hRes = m_lpd3ddevice->CreateVertexBuffer( nb_vertices * sizeof( d3d9vertex ), 0, D3DFVF_XYZ | D3DFVF_NORMAL | 
                                                                                        D3DFVF_TEX0 | 
                                                                                        D3DFVF_TEX1 | 
                                                                                        D3DFVF_TEX2 |
                                                                                        D3DFVF_TEX3 |
                                                                                        D3DFVF_TEX4 |
                                                                                        D3DFVF_TEX5 |
                                                                                        D3DFVF_TEX6 |
                                                                                        D3DFVF_TEX7 |
                                                                                        D3DFVF_TEX8, D3DPOOL_DEFAULT, &meshe_data->vertex_buffer, NULL );
    D3D9_CHECK( CreateVertexBuffer );

    hRes = m_lpd3ddevice->CreateIndexBuffer( nb_triangles * sizeof( d3d9triangle ), 0, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &meshe_data->index_buffer, NULL );	
	D3D9_CHECK( CreateIndexBuffer )


    d3d9vertex* v;
    meshe_data->vertex_buffer->Lock( 0, 0, (VOID **)&v, 0 );

    for( long i = 0; i < nb_vertices; i++, v++ )
    {
        Core::Vertex vertex;
        meshe->GetVertex( i, vertex );

        v->x = (float)vertex.x;
        v->y = (float)vertex.y;
        v->z = (float)vertex.z;

        v->nx = (float)vertex.nx;
        v->ny = (float)vertex.ny;
        v->nz = (float)vertex.nz;

        v->tu0 = vertex.tu[0];
        v->tu1 = vertex.tu[1];
        v->tu2 = vertex.tu[2];
        v->tu3 = vertex.tu[3];
        v->tu4 = vertex.tu[4];
        v->tu5 = vertex.tu[5];
        v->tu6 = vertex.tu[6];
        v->tu7 = vertex.tu[7];
        v->tu8 = vertex.tu[8];

        v->tv0 = vertex.tv[0];
        v->tv1 = vertex.tv[1];
        v->tv2 = vertex.tv[2];
        v->tv3 = vertex.tv[3];
        v->tv4 = vertex.tv[4];
        v->tv5 = vertex.tv[5];
        v->tv6 = vertex.tv[6];
        v->tv7 = vertex.tv[7];
        v->tv8 = vertex.tv[8];
        
        v->tw0 = vertex.tw[0];
        v->tw1 = vertex.tw[1];
        v->tw2 = vertex.tw[2];
        v->tw3 = vertex.tw[3];
        v->tw4 = vertex.tw[4];
        v->tw5 = vertex.tw[5];
        v->tw6 = vertex.tw[6];
        v->tw7 = vertex.tw[7];
        v->tw8 = vertex.tw[8];
        
        v->ta0 = vertex.ta[0];
        v->ta1 = vertex.ta[1];
        v->ta2 = vertex.ta[2];
        v->ta3 = vertex.ta[3];
        v->ta4 = vertex.ta[4];
        v->ta5 = vertex.ta[5];
        v->ta6 = vertex.ta[6];
        v->ta7 = vertex.ta[7];
        v->ta8 = vertex.ta[8];
    }
    meshe_data->vertex_buffer->Unlock();

    d3d9triangle *t;
    meshe_data->index_buffer->Lock( 0, 0, (VOID **)&t, 0 );

    for( long i = 0; i < nb_triangles; i++, t++ )
    {
        Core::Triangle triangle;
        meshe->GetTriangles( i, triangle );

        t->vertex1 = triangle.vertex1;
        t->vertex2 = triangle.vertex2;
        t->vertex3 = triangle.vertex3;
    }   
    meshe_data->index_buffer->Unlock();

    *p_data = (void *)meshe_data;

    m_meshes_base[hash] = meshe_data;

    return true;
}

void D3D9Renderer::RemoveMeshe( DrawSpace::Core::Meshe* p_meshe, void* p_data )
{
    MesheData* meshe_data = (MesheData*)p_data;
   
    D3D9_RELEASE( meshe_data->vertex_buffer );
    D3D9_RELEASE( meshe_data->index_buffer );
    
    _DRAWSPACE_DELETE_( meshe_data );

    dsstring hash;
    p_meshe->GetMD5( hash );

    if( m_meshes_base.count( hash ) > 0 )
    {
        m_meshes_base.erase( hash );
    }
}


bool D3D9Renderer::SetMeshe( void* p_data )
{
    DECLARE_D3D9ASSERT_VARS

    MesheData* meshe_data = (MesheData*)p_data;

    // vb selections
    hRes = m_lpd3ddevice->SetStreamSource( 0, meshe_data->vertex_buffer, 0, sizeof( d3d9vertex ) );
	D3D9_CHECK( SetStreamSource );

    hRes = m_lpd3ddevice->SetIndices( meshe_data->index_buffer );
	D3D9_CHECK( SetIndices );

    return true;
}

bool D3D9Renderer::CreateTexture( DrawSpace::Core::Texture* p_texture, void** p_data )
{
    DECLARE_D3D9ASSERT_VARS

    dsstring path;
    p_texture->GetPath( path );
    LPDIRECT3DTEXTURE9	d3dt9;
    TextureInfos* texture_infos;

    if( m_textures_base.count( path ) > 0 )
    {
        *p_data = (void*)m_textures_base[path];

        long width = m_textures_base[path]->descr.Width;
        long height = m_textures_base[path]->descr.Height;
        long bpp;

        switch( m_textures_base[path]->descr.Format )
        {
            case D3DFMT_A8R8G8B8:
            case D3DFMT_X8R8G8B8:
                bpp = 4;
                break;

            case D3DFMT_R8G8B8:
                bpp = 3;
                break;

            default:
                bpp = -1;
                break;
        }

        p_texture->SetFormat( width, height, bpp );

        return true;
    }

    if( p_texture->IsRenderTarget() )
    {
        LPDIRECT3DSURFACE9		surface;
        LPD3DXRENDERTOSURFACE	render_to_surface;
        D3DSURFACE_DESC			desc;

        unsigned long rw, rh;
        p_texture->GetRenderTargetDims( rw, rh );

        D3DFORMAT format; 

        if( Texture::RENDERPURPOSE_COLOR == p_texture->GetRenderPurpose() )
        {
            format = D3DFMT_A8R8G8B8;
        }
        else
        {
            format = D3DFMT_R16F;
        }

        hRes = m_lpd3ddevice->CreateTexture( rw, rh, 1, D3DUSAGE_RENDERTARGET, format, D3DPOOL_DEFAULT, &d3dt9, NULL );
        D3D9_CHECK( CreateTexture )

        d3dt9->GetSurfaceLevel( 0, &surface );
        surface->GetDesc( &desc );

        hRes = D3DXCreateRenderToSurface( m_lpd3ddevice, desc.Width, desc.Height, desc.Format, TRUE, m_depthbuffer_format, &render_to_surface );
        D3D9_CHECK( D3DXCreateRenderToSurface )

        TargetTextureInfos targettexture_infos;

        targettexture_infos.render_to_surface = render_to_surface;
        targettexture_infos.surface = surface;

        m_targettextures_base[p_texture] = targettexture_infos;

        texture_infos = _DRAWSPACE_NEW_( TextureInfos, TextureInfos );
        texture_infos->texture = d3dt9;
        texture_infos->descr = desc;
        texture_infos->render_texture = true;
        texture_infos->bits = NULL;
        m_textures_base[path] = texture_infos;

        *p_data = (void*)texture_infos;
    }
    else
    {
        void* data = p_texture->GetData();
        size_t data_size = p_texture->GetDataSize();
        hRes = D3DXCreateTextureFromFileInMemory( m_lpd3ddevice, data, (UINT)data_size, &d3dt9 );
        D3D9_CHECK( D3DXCreateTextureFromFileInMemory );

        texture_infos = _DRAWSPACE_NEW_( TextureInfos, TextureInfos );
        texture_infos->texture = d3dt9;
        texture_infos->render_texture = false;
        texture_infos->bits = NULL;
        d3dt9->GetLevelDesc( 0, &texture_infos->descr );

        m_textures_base[path] = texture_infos;

        *p_data = (void*)texture_infos;
    }

    long width = texture_infos->descr.Width;
    long height = texture_infos->descr.Height;
    long bpp;

    switch( texture_infos->descr.Format )
    {
        case D3DFMT_A8R8G8B8:
        case D3DFMT_X8R8G8B8:
            bpp = 4;
            break;

        case D3DFMT_R8G8B8:
            bpp = 3;
            break;

        default:
            bpp = -1;
            break;
    }

    p_texture->SetFormat( width, height, bpp );

    return true;
}

bool D3D9Renderer::SetTexture( void* p_data, int p_stage )
{
    DECLARE_D3D9ASSERT_VARS

    TextureInfos* ti = (TextureInfos*)p_data;

    hRes = m_lpd3ddevice->SetTexture( p_stage, ti->texture );
    D3D9_CHECK( SetTexture );

    return true;
}

bool D3D9Renderer::UnsetTexture( int p_stage )
{
	DECLARE_D3D9ASSERT_VARS

	hRes = m_lpd3ddevice->SetTexture( p_stage, NULL );
	D3D9_CHECK( SetTexture );

	return true;
}

bool D3D9Renderer::SetVertexTexture( void* p_data, int p_stage )
{
    DECLARE_D3D9ASSERT_VARS

    TextureInfos* ti = (TextureInfos*)p_data;

    hRes = m_lpd3ddevice->SetTexture( p_stage + D3DVERTEXTEXTURESAMPLER0, ti->texture );
    D3D9_CHECK( SetTexture );

    return true;
}

bool D3D9Renderer::UnsetVertexTexture( int p_stage )
{
    DECLARE_D3D9ASSERT_VARS

    hRes = m_lpd3ddevice->SetTexture( p_stage + D3DVERTEXTEXTURESAMPLER0, NULL );
    D3D9_CHECK( SetTexture );

    return true;
}

bool D3D9Renderer::AllocTextureContent( void* p_texturedata )
{
    DECLARE_D3D9ASSERT_VARS

    TextureInfos* ti = (TextureInfos*)p_texturedata;

    if( ti->render_texture )
    {
        // LockRect ne fonctionne pas pour des textures creees avec D3DUSAGE_RENDERTARGET

        //LockRect cannot retrieve data from a texture resource created with D3DUSAGE_RENDERTARGET 
        //because such a texture must be assigned to D3DPOOL_DEFAULT memory and is therefore not lockable. 
        //In this case, use instead IDirect3DDevice9::GetRenderTargetData to copy texture data from device memory to system memory.

        return false;
    }

    // si pas deja alloue
    if( NULL == ti->bits )
    {
        D3DLOCKED_RECT dr;

        hRes = ti->texture->LockRect( 0, &dr, NULL, D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY );
        D3D9_CHECK( LockRect );

        int bpp;
        switch( ti->descr.Format )
        {
            case D3DFMT_A8R8G8B8:
            case D3DFMT_X8R8G8B8:
                bpp = 4;
                break;

            case D3DFMT_R8G8B8:
                bpp = 3;
                break;

            default:
                bpp = -1;
                break;
        }

        if( bpp != -1 )
        {
            long blocsize = bpp * ti->descr.Width * ti->descr.Height;

            ti->bits = _DRAWSPACE_NEW_EXPLICIT_SIZE_( unsigned char, unsigned char[blocsize], blocsize );
            memcpy( ti->bits, dr.pBits, blocsize );
        }
        
        hRes = ti->texture->UnlockRect( 0 );       
    }    
    return true;
}

void D3D9Renderer::RemoveTextureContent( void* p_texturedata )
{
    TextureInfos* ti = (TextureInfos*)p_texturedata;

    if( ti->bits )
    {
        _DRAWSPACE_DELETE_N_( ti->bits );
        ti->bits = NULL;
    }
}

void* D3D9Renderer::GetTextureContent( void* p_texturedata )
{
    TextureInfos* ti = (TextureInfos*)p_texturedata;

    return ti->bits;
}

bool D3D9Renderer::CreateFx( DrawSpace::Core::Fx* p_fx, void** p_data )
{
    DECLARE_D3D9ASSERT_VARS

    dsstring hash;
    p_fx->GetMD5( hash );

    if( m_fx_bases.count( hash ) > 0 )
    {
        *p_data = (void *)m_fx_bases[hash];
        return true;
    }

    FxData* fxdata = _DRAWSPACE_NEW_( FxData, FxData );

    fxdata->fx = p_fx;

    if( p_fx->GetShadersListSize() )
    {
        LPDIRECT3DVERTEXSHADER9		vs;
        LPDIRECT3DPIXELSHADER9		ps;

        /////////////////// Shaders loading

        Core::Shader* vertex_shader = p_fx->GetShader( 0 );
        Core::Shader* pixel_shader = p_fx->GetShader( 1 );

        LPD3DXBUFFER vbuff;
        LPD3DXBUFFER pbuff;
        LPD3DXBUFFER errors; 

        if( !vertex_shader->IsCompiled() )
        {
            if( NULL == vertex_shader->GetData() )
            {
                _DSFATAL( logger, "no data in vertex shader !" )
                return false;
            }

            hRes = D3DXCompileShader( (LPCSTR)vertex_shader->GetData(), (UINT)vertex_shader->GetDataSize(), NULL, NULL, "vs_main", "vs_3_0", 0, &vbuff, &errors, NULL );
            if( D3D_OK != hRes )
            {
                if( NULL != errors )
                {
					_DSFATAL( logger, dsstring( "D3DXCompileShader FAIL : " ) << (char *)errors->GetBufferPointer() )
                    _DSEXCEPTION( "D3DXCompileShader FAIL (vertex) : " << dsstring( (char *)errors->GetBufferPointer() ) )
                }
                return false;
            }

            hRes = m_lpd3ddevice->CreateVertexShader( (DWORD *)vbuff->GetBufferPointer(), &vs );
            D3D9_CHECK( CreateVertexShader );
        }
        else
        {
            hRes = m_lpd3ddevice->CreateVertexShader( (DWORD *)vertex_shader->GetData(), &vs );
            D3D9_CHECK( CreateVertexShader );		
        }

        fxdata->vertex_shader = vs;

        if( !pixel_shader->IsCompiled() )
        {
            if( NULL == pixel_shader->GetData() )
            {
                _DSFATAL( logger, "no data in pixel shader !" )
                return false;
            }

            hRes = D3DXCompileShader( (LPCSTR)pixel_shader->GetData(), (UINT)pixel_shader->GetDataSize(), NULL, NULL, "ps_main", "ps_3_0", 0, &pbuff, &errors, NULL );
            if( D3D_OK != hRes )
            {
                if( NULL != errors )
                {
					_DSFATAL( logger, dsstring( "D3DXCompileShader FAIL : " ) << (char *)errors->GetBufferPointer() )
                    _DSEXCEPTION( "D3DXCompileShader FAIL (pixel) : " << dsstring( (char *)errors->GetBufferPointer() ) )
                }
                return false;
            }

            hRes = m_lpd3ddevice->CreatePixelShader( (DWORD *)pbuff->GetBufferPointer(), &ps );
            D3D9_CHECK( CreatePixelShader );
        }
        else
        {
            hRes = m_lpd3ddevice->CreatePixelShader( (DWORD *)pixel_shader->GetData(), &ps );
            D3D9_CHECK( CreatePixelShader );
        }

        fxdata->pixel_shader = ps;
    }

    *p_data = (void*)fxdata;

    m_fx_bases[hash] = fxdata;

    return true;
}

bool D3D9Renderer::SetFx( void* p_data )
{
    DECLARE_D3D9ASSERT_VARS

	FxData* fx_data = (FxData*)p_data;
	DrawSpace::Core::Fx* fx = fx_data->fx;

    long nb_rs_in = fx->GetRenderStatesInListSize();
    for( long i = 0; i < nb_rs_in; i++ )
    {
        DrawSpace::Core::RenderState rs = fx->GetRenderStateIn( i );
        SetRenderState( &rs );
    }

    hRes = m_lpd3ddevice->SetVertexShader( fx_data->vertex_shader );
    D3D9_CHECK( SetVertexShader );

    hRes = m_lpd3ddevice->SetPixelShader( fx_data->pixel_shader );
    D3D9_CHECK( SetPixelShader );

    return true;
}

bool D3D9Renderer::UnsetFx( void* p_data )
{
	FxData* fx_data = (FxData*)p_data;
	DrawSpace::Core::Fx* fx = fx_data->fx;

    long nb_rs_out = fx->GetRenderStatesOutListSize();
    for( long i = 0; i < nb_rs_out; i++ )
    {
        DrawSpace::Core::RenderState rs = fx->GetRenderStateOut( i );
        SetRenderState( &rs );
    }

	return true;
}

bool D3D9Renderer::SetFxShaderParams( int p_shader_index, long p_register, DrawSpace::Utils::Vector& p_vector )
{
	switch( p_shader_index )
	{
		case 0:

			// vertex shader params application		
			set_vertexshader_constants( p_register, p_vector.GetArray(), 1 );
			break;


		case 1:

			// pixel shader params application
			set_pixelshader_constants( p_register, p_vector.GetArray(), 1 );
			break;

        default:
            return false;
	}

	return false;
}

bool D3D9Renderer::DrawMeshe( long p_nbvertices, long p_nbtriangles, DrawSpace::Utils::Matrix p_world, DrawSpace::Utils::Matrix p_view, DrawSpace::Utils::Matrix p_proj )
{
	DECLARE_D3D9ASSERT_VARS

    // setting transformation
    DrawSpace::Utils::Matrix final_view;
    DrawSpace::Utils::Matrix inv;
    DrawSpace::Utils::Matrix result;

    inv.Identity();
    inv( 2, 2 ) = -1.0;
    final_view = p_view * inv;

    Transformation chain;
    //chain.PushMatrix( m_projection );
    chain.PushMatrix( p_proj );
    chain.PushMatrix( final_view );
    chain.PushMatrix( p_world );
    chain.BuildResult();
    chain.GetResult( &result );
    result.Transpose();

    set_vertexshader_constants( 0, result.GetArray(), 4 );
    
    //////////////////////////////////////////////////////////////////////

    DrawSpace::Utils::Matrix world = p_world;
    DrawSpace::Utils::Matrix view = p_view;
    DrawSpace::Utils::Matrix worldview = world * view;
    worldview.Transpose();
    
    set_vertexshader_constants( 4, worldview.GetArray(), 4 );

	//////////////////////////////////////////////////////////////////////

	hRes = m_lpd3ddevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, p_nbvertices, 0, p_nbtriangles );

	return true;
}

void D3D9Renderer::SetRenderState( DrawSpace::Core::RenderState* p_renderstate )
{
    dsstring arg;
    p_renderstate->GetArg( arg );

    switch( p_renderstate->GetOperation() )
    {
        case DrawSpace::Core::RenderState::SETCULLING:
            
            if( "none" == arg )
            {
                m_lpd3ddevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
            }
            else if( "cw" == arg )
            {
                m_lpd3ddevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
            }
            else
            {
                m_lpd3ddevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
            }
            break;

        case DrawSpace::Core::RenderState::ENABLEZBUFFER:
            if( "true" == arg )
            {
                m_lpd3ddevice->SetRenderState( D3DRS_ZENABLE, TRUE );
            }
            else
            {
                m_lpd3ddevice->SetRenderState( D3DRS_ZENABLE, FALSE );
            }
            break;

        case DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE:
            {
                D3DTEXTUREFILTERTYPE ftype;

                if( "none" == arg )
                {
                    ftype = D3DTEXF_NONE;
                }
                else if( "point" == arg )
                {
                    ftype = D3DTEXF_POINT;
                }
                else if( "linear" == arg )
                {
                    ftype = D3DTEXF_LINEAR;
                }
                else if( "anisotropic" == arg )
                {
                    ftype = D3DTEXF_ANISOTROPIC;
                }

                for( long i = 0; i < 8; i++ )
                {
                    m_lpd3ddevice->SetSamplerState( i, D3DSAMP_MAGFILTER, ftype );
                    m_lpd3ddevice->SetSamplerState( i, D3DSAMP_MINFILTER, ftype );
                }

            }
            break;

        case DrawSpace::Core::RenderState::SETFILLMODE:
            {
                D3DFILLMODE fm;
                if( "point" == arg )
                {
                    fm = D3DFILL_POINT;
                }
                else if( "line" == arg )
                {
                    fm = D3DFILL_WIREFRAME ;
                }
                else if( "solid" == arg )
                {
                    fm = D3DFILL_SOLID;
                }
                m_lpd3ddevice->SetRenderState( D3DRS_FILLMODE, fm );
            }
            break;

        case DrawSpace::Core::RenderState::ALPHABLENDENABLE:
            {
                if( "true" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
                }
                else if( "false" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
                }
            }
            break;

        case DrawSpace::Core::RenderState::ALPHABLENDOP:
            {
                if( "add" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
                }
                else if( "sub" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_SUBTRACT );
                }
                else if( "revsub" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT );
                }
                else if( "min" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_MIN );
                }
                else if( "max" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_MAX );
                }
            }
            break;

        case DrawSpace::Core::RenderState::ALPHABLENDFUNC:
            {
                if( "never" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_NEVER );
                }
                else if( "less" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_LESS );
                }
                else if( "equal" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_EQUAL );
                }
                else if( "lessequal" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_LESSEQUAL );
                }
                else if( "greater" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER );
                }
                else if( "notequal" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_NOTEQUAL );
                }
                else if( "greaterequal" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
                }
                else if( "always" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_ALWAYS );
                }
            }
            break;

        case DrawSpace::Core::RenderState::ALPHABLENDDEST:
            {
                if( "zero" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
                }
                else if( "one" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
                }
                else if( "srccolor" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );
                }
                else if( "invsrccolor" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
                }
                else if( "srcalpha" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCALPHA );
                }
                else if( "invsrcalpha" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
                }
                else if( "destalpha" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_DESTALPHA );
                }
                else if( "invdestalpha" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVDESTALPHA );
                }
                else if( "destcolor" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR );
                }
                else if( "invdestcolor" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVDESTCOLOR );
                }
            }
            break;

        case DrawSpace::Core::RenderState::ALPHABLENDSRC:

                if( "zero" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
                }
                else if( "one" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
                }
                else if( "srccolor" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR );
                }
                else if( "invsrccolor" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_INVSRCCOLOR );
                }
                else if( "srcalpha" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
                }
                else if( "invsrcalpha" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_INVSRCALPHA );
                }
                else if( "destalpha" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTALPHA );
                }
                else if( "invdestalpha" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_INVDESTALPHA );
                }
                else if( "destcolor" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
                }
                else if( "invdestcolor" == arg )
                {
                    m_lpd3ddevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR );
                }

            break;

    }
}

void D3D9Renderer::GetRenderCharacteristics( Characteristics& p_characteristics )
{
    p_characteristics = m_characteristics;
    if( !m_characteristics.fullscreen )
    {
        // prendre en compte les bords fenetre

        RECT rect;
        GetClientRect( m_hwnd, &rect );

        p_characteristics.width_resol = rect.right;
        p_characteristics.height_resol = rect.bottom;
    }
}

void D3D9Renderer::set_vertexshader_constants( DWORD p_startreg, dsreal *p_ftab, DWORD p_v4fCount )
{
    float ftab[16];
    for( DWORD i = 0; i < p_v4fCount; i++ )
    {
        for( DWORD j = 0; j < 4; j++ )
        {
            ftab[(4 * i) + j] = (float)p_ftab[(4 * i) + j];
        }
    }		
    m_lpd3ddevice->SetVertexShaderConstantF( p_startreg, ftab, p_v4fCount );
}

void D3D9Renderer::set_pixelshader_constants( DWORD p_startreg, dsreal *p_ftab, DWORD p_v4fCount )
{
    float ftab[16];
    for( DWORD i = 0; i < p_v4fCount; i++ )
    {
        for( DWORD j = 0; j < 4; j++ )
        {
            ftab[(4 * i) + j] = (float)p_ftab[(4 * i) + j];
        }
    }	
    m_lpd3ddevice->SetPixelShaderConstantF( p_startreg, ftab, p_v4fCount );
}

bool D3D9Renderer::Config::on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words )
{
    if( 2 == p_words.size() )
    {
        if( "dx9adapterordinal" == p_words[0] )
        {
            m_adapter_ordinal = atoi( p_words[1].c_str() );
        }
        if( "dx9vertexproc" == p_words[0] )
        {
            if( "hardware" == p_words[1] )
            {
                m_vertex_processing = D3DCREATE_HARDWARE_VERTEXPROCESSING;
            }
            else
            {
                m_vertex_processing = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
            }
        }

    }
    if( 5 == p_words.size() )
    {
        if( "dx9fullscreen" == p_words[0] )
        {
            m_fullscreen_width = atoi( p_words[1].c_str() );
            m_fullscreen_height = atoi( p_words[2].c_str() );
            m_refreshrate = atoi( p_words[3].c_str() );
            m_fullscreen_format = (D3DFORMAT)atoi( p_words[4].c_str() );
        }
    }

    return true;
}

void D3D9Renderer::DrawText( long p_r, long p_g, long p_b, int p_posX, int p_posY, const char* p_format, ... )
{
    static char buffer[512];
    RECT rect;

    _vsnprintf( buffer, 512, p_format, (va_list)( &p_format + 1 ) );
	rect.left = p_posX;
	rect.top = p_posY;
	rect.right = rect.left + 1;
	rect.bottom = rect.top - 1;

    m_font->DrawTextA( NULL, buffer, -1, &rect, DT_LEFT | DT_NOCLIP, D3DCOLOR_XRGB( p_r, p_g, p_b ) );
}

void D3D9Renderer::PointProjection( DrawSpace::Utils::Matrix p_view, DrawSpace::Utils::Matrix p_proj, DrawSpace::Utils::Vector& p_point, dsreal& p_outx, dsreal& p_outy, dsreal& p_outz )
{
    DrawSpace::Utils::Matrix final_view;
    DrawSpace::Utils::Matrix inv;
    DrawSpace::Utils::Matrix proj;
    //DrawSpace::Utils::Matrix final;

    DrawSpace::Utils::Vector res;
    DrawSpace::Utils::Vector point = p_point;

    inv.Identity();
    inv( 2, 2 ) = -1.0;


    /*

    final_view = p_view * inv;
    final = final_view * p_proj;
    final.Transform( &point, &res );

    */

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

    //p_outz = res[2];
}
