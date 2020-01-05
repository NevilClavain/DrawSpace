/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2020                     
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

#include "d3d9renderer.h"
#include <memalloc.h>

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

extern void TranslateD3DD9Error( HRESULT p_hRes, dsstring &p_str );

_DECLARE_DS_LOGGER( logger, "d3d9", NULL )

D3D9Renderer::D3D9Renderer( void ) :
m_lpd3d( NULL ),
m_lpd3ddevice( NULL ),
m_vertexdeclaration( NULL ),
m_next_nbvertices( 0 ),
m_next_nbtriangles( 0 )
{

}

D3D9Renderer::~D3D9Renderer( void )
{

}

void D3D9Renderer::GetDescr( dsstring& p_descr )
{
    p_descr = "Direct3D9 " + m_driver_type;
}

void D3D9Renderer::GetShadersDescr( dsstring& p_descr )
{
    p_descr = "hlsl_3_0";
}

void D3D9Renderer::DumpMemoryAllocs( void )
{
    DrawSpace::Utils::MemAlloc::GetInstance()->DumpContent();
}

D3DFORMAT D3D9Renderer::find_depthbuffer_format( int p_adapterordinal, D3DFORMAT p_displayformat )
{
    HRESULT hRes = m_lpd3d->CheckDeviceFormat( p_adapterordinal, m_dev_type, p_displayformat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D32 );
    if( D3D_OK == hRes )
    {
        return D3DFMT_D32;  
    }
    else
    {
        hRes = m_lpd3d->CheckDeviceFormat( p_adapterordinal, m_dev_type, p_displayformat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X8 );
        if( D3D_OK == hRes )
        {
            return D3DFMT_D24X8;
        }
        else
        {
            hRes = m_lpd3d->CheckDeviceFormat( p_adapterordinal, m_dev_type, p_displayformat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D16 );
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

void D3D9Renderer::fullscreen_autoset_desktop_resolution( int& p_fullscreen_width, int& p_fullscreen_height, D3DFORMAT& p_fullscreen_format, UINT& p_fullscreen_refreshRate )
{
    bool found = false;
    // get user windows desktop resolution
    RECT desktop_rect;
    GetWindowRect( GetDesktopWindow(), &desktop_rect );

    p_fullscreen_width = desktop_rect.right - desktop_rect.left;
    p_fullscreen_height = desktop_rect.bottom - desktop_rect.top;
    p_fullscreen_format = D3DFMT_X8R8G8B8;
    p_fullscreen_refreshRate = 20;
}

bool D3D9Renderer::Init( HWND p_hwnd, bool p_fullscreen, long p_w_width, long p_w_height, DrawSpace::Logger::Configuration* p_logconf )
{
    D3DPRESENT_PARAMETERS d3dpp;
    DECLARE_D3D9ASSERT_VARS
    D3DDISPLAYMODE d3ddm;

    p_logconf->RegisterSink( &logger );
    logger.SetConfiguration( p_logconf );

    p_logconf->RegisterSink( MemAlloc::GetLogSink() );
    MemAlloc::GetLogSink()->SetConfiguration( p_logconf );

    
    long adapter = 0;

    _DSDEBUG( logger, "begin" )

    m_hwnd = p_hwnd;

    RECT rect;
    GetClientRect( m_hwnd, &rect );
    
    m_lpd3d = Direct3DCreate9( D3D_SDK_VERSION );
    if( NULL == m_lpd3d )
    {	
        _DSDEBUG( logger, "end : ko Direct3DCreate9 fail" )
        return false;
    }


    int fullscreen_width;
    int fullscreen_height;
    D3DFORMAT fullscreen_format;
    UINT fullscreen_refresh_rate;

    if( p_fullscreen )
    {
        // basculer automatiquement sur un mode correspondant a la resol actuelle du bureau windows
        fullscreen_autoset_desktop_resolution( fullscreen_width, fullscreen_height, fullscreen_format, fullscreen_refresh_rate );
    }


    // check for shaders model 3.0 support

    D3DCAPS9 caps;

    static std::vector<std::tuple<D3DDEVTYPE, int, dsstring>> driver_type =
    {
        { D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING, "HAL initialized"},        
        { D3DDEVTYPE_REF, D3DCREATE_SOFTWARE_VERTEXPROCESSING, "REF initialized"},
        { D3DDEVTYPE_SW, D3DCREATE_SOFTWARE_VERTEXPROCESSING, "SW initialized"}        
    };

    HRESULT r;
    for (auto& e : driver_type)
    {
        D3DDEVTYPE dev_type;
        int vproc;
        dsstring type;
        std::tie(dev_type, vproc, type) = e;

        r = m_lpd3d->GetDeviceCaps(adapter, dev_type, &caps);
        if (r == S_OK)
        {
            m_driver_type = type;
            m_dev_type = dev_type;
            m_vproc = vproc;
            break;
        }
    }
    hRes = r;
    D3D9_CHECK(GetDeviceCaps)

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
        depth_format = find_depthbuffer_format( adapter, fullscreen_format );
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

        
        d3dpp.BackBufferWidth = fullscreen_width;
        d3dpp.BackBufferHeight = fullscreen_height;
        d3dpp.BackBufferFormat = fullscreen_format;
        


        d3dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
        d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
        d3dpp.FullScreen_RefreshRateInHz = fullscreen_refresh_rate;

        m_characteristics.fullscreen = true;

        m_characteristics.width_resol = fullscreen_width;
        m_characteristics.height_resol = fullscreen_height;
        
        ///// ??????????????

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

        m_characteristics.width_resol = rect.right - rect.left;
        m_characteristics.height_resol = rect.bottom - rect.top;


		_DSDEBUG( logger, dsstring(" -> WINDOWED ") << p_w_width << dsstring( " x " ) << p_w_height )
    }
    
    hRes = m_lpd3d->CreateDevice(adapter, m_dev_type, m_hwnd, m_vproc | D3DCREATE_FPU_PRESERVE, &d3dpp, &m_lpd3ddevice);
    D3D9_CHECK(CreateDevice)


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
        { 0, 168, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
        { 0, 180, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
        
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


    
    // a mettre en option dans l'interface renderer
    for( long i = 0; i < 8; i++ )
    {
        m_lpd3ddevice->SetSamplerState( i, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
        m_lpd3ddevice->SetSamplerState( i, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
    }
    


    m_lpd3ddevice->SetRenderState( D3DRS_LIGHTING, FALSE );

    //projection set, to automatically fit with the screen resolution
    float v_width, v_height;
    if( p_fullscreen )
    {
        v_width = 1.0;
        v_height = v_width * (float)fullscreen_height / (float)fullscreen_width;

        _DSDEBUG( logger, " fullscreen for perpective : v_width = " << v_width << " v_height " << v_height )
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


    hRes = D3DXCreateFontA( m_lpd3ddevice, 15, 10, 0, 0, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Courier New", &m_font );


    // set viewport....

    // set viewport....
    if( p_fullscreen )
    {
        m_viewport.X = 0.0;
        m_viewport.Y = 0.0;
        m_viewport.Width = fullscreen_width;
        m_viewport.Height = fullscreen_height;
        m_viewport.MinZ = 0.0;
        m_viewport.MaxZ = 1.0;
    }
    else
    {
        m_viewport.X = rect.left;
        m_viewport.Y = rect.top;
        m_viewport.Width = rect.right - rect.left;
        m_viewport.Height = rect.bottom - rect.top;
        m_viewport.MinZ = 0.0;
        m_viewport.MaxZ = 1.0;
    }


    m_lpd3ddevice->SetViewport( &m_viewport );

    // renderer characteristics dump
    _DSDEBUG( logger, dsstring( "characteristics.width_resol = " ) << (int)m_characteristics.width_resol );
    _DSDEBUG( logger, dsstring( "characteristics.height_resol = " ) << (int)m_characteristics.height_resol );
    _DSDEBUG( logger, dsstring( "characteristics.width_viewport = " ) << (float)m_characteristics.width_viewport );
    _DSDEBUG( logger, dsstring( "characteristics.height_viewport = " ) << (float)m_characteristics.height_viewport );
    _DSDEBUG( logger, dsstring( "characteristics.fullscreen = " ) << m_characteristics.fullscreen );

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

void D3D9Renderer::ClearScreen( unsigned char p_r, unsigned char p_g, unsigned char p_b, unsigned char p_a )
{
    m_lpd3ddevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_RGBA( p_r, p_g, p_b, p_a ), 1.0f, 0 );
}

void D3D9Renderer::ClearDepth( dsreal p_value )
{
    m_lpd3ddevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0, 0, 0 ), (float)p_value, 0 );
}

//void D3D9Renderer::BeginTarget( DrawSpace::Core::Texture* p_texture )
void D3D9Renderer::BeginTarget( void* p_data )
{
    /*
    if( m_targettextures_base.count( p_texture ) > 0 )
    {
        m_targettextures_base[p_texture].render_to_surface->BeginScene( m_targettextures_base[p_texture].surface, NULL );
    }
    */

    TextureInfos* ti = (TextureInfos*)p_data;
    dsstring hash = ti->hash;

    if( m_targettextures_base.count( hash ) > 0 )
    {
        m_targettextures_base[hash].render_to_surface->BeginScene( m_targettextures_base[hash].surface, NULL );
    }

}

//void D3D9Renderer::EndTarget( DrawSpace::Core::Texture* p_texture )
void D3D9Renderer::EndTarget( void* p_data )
{
    /*
    if( m_targettextures_base.count( p_texture ) > 0 )
    {
        m_targettextures_base[p_texture].render_to_surface->EndScene( 0 );
    }
    */

    TextureInfos* ti = (TextureInfos*)p_data;
    dsstring hash = ti->hash;

    if( m_targettextures_base.count( hash ) > 0 )
    {
        m_targettextures_base[hash].render_to_surface->EndScene( 0 );
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
        p_meshe->SetRenderData( (void *)m_meshes_base[hash] );
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
                                                                                        D3DFVF_TEX8 |
                                                                                        D3DFVF_NORMAL |
                                                                                        D3DFVF_NORMAL, D3DPOOL_DEFAULT, &meshe_data->vertex_buffer, NULL );
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

        v->tx = (float)vertex.tx;
        v->ty = (float)vertex.ty;
        v->tz = (float)vertex.tz;

        v->bx = (float)vertex.bx;
        v->by = (float)vertex.by;
        v->bz = (float)vertex.bz;


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

    meshe_data->nb_vertices = nb_vertices;
    meshe_data->nb_triangles = nb_triangles;

    *p_data = (void *)meshe_data;

    meshe->SetRenderData( (void *)meshe_data );

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

    m_next_nbvertices = meshe_data->nb_vertices;
    m_next_nbtriangles = meshe_data->nb_triangles;

    return true;
}

bool D3D9Renderer::UpdateMesheIndexes( DrawSpace::Core::Meshe* p_meshe, void* p_data )
{
    MesheData* meshe_data = (MesheData*)p_data;

    d3d9triangle* t;
    meshe_data->index_buffer->Lock( 0, 0, (VOID **)&t, 0 );

    long nb_triangles = p_meshe->GetTrianglesListSize();

    for( long i = 0; i < nb_triangles; i++, t++ )
    {
        Core::Triangle triangle;
        p_meshe->GetTriangles( i, triangle );

        t->vertex1 = triangle.vertex1;
        t->vertex2 = triangle.vertex2;
        t->vertex3 = triangle.vertex3;
    }
    meshe_data->index_buffer->Unlock();

    return true;
}

bool D3D9Renderer::UpdateMesheVertices( DrawSpace::Core::Meshe* p_meshe, void* p_data )
{
    MesheData* meshe_data = (MesheData*)p_data;

    d3d9vertex* v;
    meshe_data->vertex_buffer->Lock( 0, 0, (VOID **)&v, 0 );

    long nb_vertices = p_meshe->GetVertexListSize();

    for( long i = 0; i < nb_vertices; i++, v++ )
    {
        Core::Vertex vertex;
        p_meshe->GetVertex( i, vertex );

        v->x = (float)vertex.x;
        v->y = (float)vertex.y;
        v->z = (float)vertex.z;

        v->nx = (float)vertex.nx;
        v->ny = (float)vertex.ny;
        v->nz = (float)vertex.nz;

        v->tx = (float)vertex.tx;
        v->ty = (float)vertex.ty;
        v->tz = (float)vertex.tz;

        v->bx = (float)vertex.bx;
        v->by = (float)vertex.by;
        v->bz = (float)vertex.bz;

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

    return true;
}

bool D3D9Renderer::UpdateMesheVerticesFromImpostors( const DrawSpace::ImpostorsDisplayList& p_list, void* p_data )
{
    MesheData* meshe_data = (MesheData*)p_data;

    d3d9vertex* v;
    meshe_data->vertex_buffer->Lock( 0, 0, (VOID **)&v, 0 );

    for( size_t i = 0; i < p_list.size(); i++ )
    {
   
        // vertex x,y,z set by impostors shaders
        v->x = 0.0;
        v->y = 0.0;
        v->z = 0.0;

        

        v->tu0 = p_list[i].u1;
        v->tv0 = p_list[i].v1;
        v->nx = 1.0;
        v->tu6 = p_list[i].spherical_ray;
        v->tv6 = p_list[i].spherical_longitud;
        v->tw6 = p_list[i].spherical_latitud;
        v->tu7 = p_list[i].localpos[0];
        v->tv7 = p_list[i].localpos[1];
        v->tw7 = p_list[i].localpos[2];
        v->tu8 = p_list[i].width_scale;
        v->tv8 = p_list[i].height_scale;

        v++;

        // vertex x,y,z set by impostors shaders
        v->x = 0.0;
        v->y = 0.0;
        v->z = 0.0;

        v->tu0 = p_list[i].u2;
        v->tv0 = p_list[i].v2;
        v->nx = 2.0;
        v->tu6 = p_list[i].spherical_ray;
        v->tv6 = p_list[i].spherical_longitud;
        v->tw6 = p_list[i].spherical_latitud;
        v->tu7 = p_list[i].localpos[0];
        v->tv7 = p_list[i].localpos[1];
        v->tw7 = p_list[i].localpos[2];
        v->tu8 = p_list[i].width_scale;
        v->tv8 = p_list[i].height_scale;

        v++;

        // vertex x,y,z set by impostors shaders
        v->x = 0.0;
        v->y = 0.0;
        v->z = 0.0;

        v->tu0 = p_list[i].u3;
        v->tv0 = p_list[i].v3;
        v->nx = 3.0;
        v->tu6 = p_list[i].spherical_ray;
        v->tv6 = p_list[i].spherical_longitud;
        v->tw6 = p_list[i].spherical_latitud;
        v->tu7 = p_list[i].localpos[0];
        v->tv7 = p_list[i].localpos[1];
        v->tw7 = p_list[i].localpos[2];
        v->tu8 = p_list[i].width_scale;
        v->tv8 = p_list[i].height_scale;

        v++;
        
        // vertex x,y,z set by impostors shaders
        v->x = 0.0;
        v->y = 0.0;
        v->z = 0.0;

        v->tu0 = p_list[i].u4;
        v->tv0 = p_list[i].v4;
        v->nx = 4.0;
        v->tu6 = p_list[i].spherical_ray;
        v->tv6 = p_list[i].spherical_longitud;
        v->tw6 = p_list[i].spherical_latitud;
        v->tu7 = p_list[i].localpos[0];
        v->tv7 = p_list[i].localpos[1];
        v->tw7 = p_list[i].localpos[2];
        v->tu8 = p_list[i].width_scale;
        v->tv8 = p_list[i].height_scale;

        v++;
    }
    meshe_data->vertex_buffer->Unlock();

    return true;
}

bool D3D9Renderer::CreateTexture( DrawSpace::Core::Texture* p_texture, void** p_data )
{
    DECLARE_D3D9ASSERT_VARS

    dsstring hash;
    p_texture->GetMD5( hash );
    LPDIRECT3DTEXTURE9	d3dt9;
    TextureInfos* texture_infos;
   
    if( m_textures_base.count( hash ) > 0 )
    {
        *p_data = (void*)m_textures_base[hash];

        long width = m_textures_base[hash]->descr.Width;
        long height = m_textures_base[hash]->descr.Height;
        long bpp;

        switch( m_textures_base[hash]->descr.Format )
        {
            case D3DFMT_A8R8G8B8:
            case D3DFMT_X8R8G8B8:
                bpp = 4;
                break;

            case D3DFMT_R8G8B8:
                bpp = 3;
                break;

            case D3DFMT_R16F:
                bpp = 2;
                break;

            case D3DFMT_R32F:
                bpp = 4;
                break;

            case D3DFMT_A32B32G32R32F:
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
        LPDIRECT3DSURFACE9		surface;
        LPD3DXRENDERTOSURFACE	render_to_surface;
        D3DSURFACE_DESC			desc;

        unsigned long rw, rh;
        p_texture->GetRenderTargetDims( rw, rh );

        D3DFORMAT format; 

        switch( p_texture->GetRenderPurpose() )
        {
            case Texture::RENDERPURPOSE_COLOR:

                format = D3DFMT_A8R8G8B8;
                break;

            case Texture::RENDERPURPOSE_FLOAT:

                format = D3DFMT_R16F;
                break;

            case Texture::RENDERPURPOSE_FLOAT32:

                format = D3DFMT_R32F;
                break;

            case Texture::RENDERPURPOSE_FLOATVECTOR:

                format = D3DFMT_A16B16G16R16F;
                break;
        
        }

        hRes = m_lpd3ddevice->CreateTexture( rw, rh, 1, D3DUSAGE_RENDERTARGET, format, D3DPOOL_DEFAULT, &d3dt9, NULL );
        D3D9_CHECK( CreateTexture )

        d3dt9->GetSurfaceLevel( 0, &surface );
        surface->GetDesc( &desc );

        dsstring path;
        p_texture->GetPath( path );

        hRes = D3DXCreateRenderToSurface( m_lpd3ddevice, desc.Width, desc.Height, desc.Format, TRUE, m_depthbuffer_format, &render_to_surface );
        D3D9_CHECK( D3DXCreateRenderToSurface )

        TargetTextureInfos targettexture_infos;

        targettexture_infos.render_to_surface = render_to_surface;
        targettexture_infos.surface = surface;

        m_targettextures_base[hash] = targettexture_infos;

        texture_infos = _DRAWSPACE_NEW_( TextureInfos, TextureInfos );
        texture_infos->path = path;
        texture_infos->texture_instance = p_texture;
        texture_infos->texture = d3dt9;
        texture_infos->descr = desc;
        texture_infos->render_texture = true;
        texture_infos->bits = NULL;
        texture_infos->hash = hash;

        m_textures_base[hash] = texture_infos;

        hRes = m_lpd3ddevice->CreateTexture( rw, rh, 1, /*D3DUSAGE_DYNAMIC*/ 0, format, D3DPOOL_SYSTEMMEM, &d3dt9, NULL );
        D3D9_CHECK( CreateTexture )
        texture_infos->texture2 = d3dt9;

        *p_data = (void*)texture_infos;
    }
    else
    {
        dsstring path;
        p_texture->GetPath( path );

        switch( p_texture->GetPurpose() )
        {
            case Texture::PURPOSE_COLORFROMFILE:
                {
                    void* data = p_texture->GetData();
                    size_t data_size = p_texture->GetDataSize();
                    hRes = D3DXCreateTextureFromFileInMemory( m_lpd3ddevice, data, (UINT)data_size, &d3dt9 );
                    D3D9_CHECK( D3DXCreateTextureFromFileInMemory );

                    texture_infos = _DRAWSPACE_NEW_( TextureInfos, TextureInfos );
                    texture_infos->path = path;
                    texture_infos->texture_instance = p_texture;
                    texture_infos->texture = d3dt9;
                    texture_infos->texture2 = NULL;
                    texture_infos->render_texture = false;
                    texture_infos->bits = NULL;
                    d3dt9->GetLevelDesc( 0, &texture_infos->descr );

                    texture_infos->hash = hash;
                    m_textures_base[hash] = texture_infos;

                    *p_data = (void*)texture_infos;
                }
                break;

            case Texture::PURPOSE_COLOR:
                {
                    long w, h, bpp;
                    p_texture->GetFormat( w, h, bpp );

                    hRes = D3DXCreateTexture( m_lpd3ddevice, w, h, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &d3dt9 );
                    D3D9_CHECK( D3DXCreateTexture );

                    texture_infos = _DRAWSPACE_NEW_( TextureInfos, TextureInfos );
                    texture_infos->path = path;
                    texture_infos->texture_instance = p_texture;
                    texture_infos->texture = d3dt9;
                    texture_infos->texture2 = NULL;
                    texture_infos->render_texture = false;
                    texture_infos->bits = NULL;
                    d3dt9->GetLevelDesc( 0, &texture_infos->descr );
                    texture_infos->hash = hash;
                    m_textures_base[hash] = texture_infos;

                    
                    *p_data = (void*)texture_infos;
                   
                    // inutile d'appeler SetFormat() sur la texture
                    setformat_call = false;
                }
                break;

            case Texture::PURPOSE_FLOAT32:
                {
                    long w, h, bpp;
                    p_texture->GetFormat( w, h, bpp );

                    hRes = D3DXCreateTexture( m_lpd3ddevice, w, h, 1, D3DUSAGE_DYNAMIC, D3DFMT_R32F, D3DPOOL_DEFAULT, &d3dt9 );                   
                    D3D9_CHECK( D3DXCreateTexture );

                    texture_infos = _DRAWSPACE_NEW_( TextureInfos, TextureInfos );
                    texture_infos->path = path;
                    texture_infos->texture_instance = p_texture;
                    texture_infos->texture = d3dt9;
                    texture_infos->texture2 = NULL;
                    texture_infos->render_texture = false;
                    texture_infos->bits = NULL;
                    d3dt9->GetLevelDesc( 0, &texture_infos->descr );
                    texture_infos->hash = hash;
                    m_textures_base[hash] = texture_infos;
                    
                    *p_data = (void*)texture_infos;

                    // inutile d'appeler SetFormat() sur la texture
                    setformat_call = false;
                }
                break;

            case Texture::PURPOSE_FLOAT32VECTOR:
                {
                    long w, h, bpp;
                    p_texture->GetFormat( w, h, bpp );

                    hRes = D3DXCreateTexture( m_lpd3ddevice, w, h, 1, D3DUSAGE_DYNAMIC, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT, &d3dt9 );                   
                    D3D9_CHECK( D3DXCreateTexture );

                    texture_infos = _DRAWSPACE_NEW_( TextureInfos, TextureInfos );
                    texture_infos->path = path;
                    texture_infos->texture_instance = p_texture;
                    texture_infos->texture = d3dt9;
                    texture_infos->texture2 = NULL;
                    texture_infos->render_texture = false;
                    texture_infos->bits = NULL;
                    d3dt9->GetLevelDesc( 0, &texture_infos->descr );
                    texture_infos->hash = hash;
                    m_textures_base[hash] = texture_infos;
                    
                    *p_data = (void*)texture_infos;

                    // inutile d'appeler SetFormat() sur la texture
                    setformat_call = false;
                }
                break;
        }
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

        case D3DFMT_R16F:
            bpp = 2;
            break;

        case D3DFMT_R32F:
            bpp = 4;
            break;

        case D3DFMT_A32B32G32R32F:
            bpp = 16;
            break;

        case D3DFMT_A16B16G16R16F:

            bpp = 8;
            break;

        default:
            bpp = -1;
            break;
    }

    if( setformat_call )
    {
        p_texture->SetFormat( width, height, bpp );
    }
    p_texture->SetRenderData( (void*)texture_infos );

    return true;
}

void D3D9Renderer::DestroyTexture( void* p_data )
{
    TextureInfos* ti = (TextureInfos*)p_data;
    ti->texture->Release();

    if( ti->render_texture )
    {
        ti->texture2->Release();

        if( m_targettextures_base.count( ti->hash ) > 0 )
        {
            TargetTextureInfos tti = m_targettextures_base[ti->hash];
            tti.render_to_surface->Release();
            tti.surface->Release();
            
            m_targettextures_base.erase( ti->hash );
        }
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

    _DRAWSPACE_DELETE_( ti );
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
    //DECLARE_D3D9ASSERT_VARS

    TextureInfos* ti = (TextureInfos*)p_texturedata;

    /*
    if( ti->render_texture )
    {
        // LockRect ne fonctionne pas pour des textures creees avec D3DUSAGE_RENDERTARGET

        //LockRect cannot retrieve data from a texture resource created with D3DUSAGE_RENDERTARGET 
        //because such a texture must be assigned to D3DPOOL_DEFAULT memory and is therefore not lockable. 
        //In this case, use instead IDirect3DDevice9::GetRenderTargetData to copy texture data from device memory to system memory.

        return false;
    }
    */

    // si pas deja alloue
    if( NULL == ti->bits )
    {
        //D3DLOCKED_RECT dr;

        //hRes = ti->texture->LockRect( 0, &dr, NULL, D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY );
        //D3D9_CHECK( LockRect );

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

            case D3DFMT_R16F:
                bpp = 2;
                break;

            case D3DFMT_R32F:
                bpp = 4;
                break;

            case D3DFMT_A32B32G32R32F:
                bpp = 16;
                break;

            default:
                bpp = -1;
                break;
        }

        if( bpp != -1 )
        {
            long blocsize = bpp * ti->descr.Width * ti->descr.Height;

            ti->bits = _DRAWSPACE_NEW_EXPLICIT_SIZE_( unsigned char, unsigned char[blocsize], blocsize );
            //memcpy( ti->bits, dr.pBits, blocsize );
        }
        
        //hRes = ti->texture->UnlockRect( 0 );
    }    
    return true;
}

void D3D9Renderer::ReleaseTextureContent( void* p_texturedata )
{
    TextureInfos* ti = (TextureInfos*)p_texturedata;

    if( ti->bits )
    {
        _DRAWSPACE_DELETE_N_( ti->bits );
        ti->bits = NULL;
    }
}

void* D3D9Renderer::GetTextureContentPtr( void* p_texturedata )
{
    TextureInfos* ti = (TextureInfos*)p_texturedata;
    return ti->bits;
}

bool D3D9Renderer::CopyTextureContent( void* p_texturedata )
{
    DECLARE_D3D9ASSERT_VARS
    TextureInfos* ti = (TextureInfos*)p_texturedata;

    if( NULL == ti->bits )
    {
        return false;
    }

    long blocsize;
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

        case D3DFMT_R16F:
            bpp = 2;
            break;

        case D3DFMT_R32F:
            bpp = 4;
            break;

        case D3DFMT_A32B32G32R32F:
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

    D3DLOCKED_RECT dr;

    if( ti->render_texture )
    {
        // LockRect ne fonctionne pas pour des textures creees avec D3DUSAGE_RENDERTARGET

        //LockRect cannot retrieve data from a texture resource created with D3DUSAGE_RENDERTARGET 
        //because such a texture must be assigned to D3DPOOL_DEFAULT memory and is therefore not lockable. 
        //In this case, use instead IDirect3DDevice9::GetRenderTargetData to copy texture data from device memory to system memory.

        LPDIRECT3DSURFACE9 rendersurface, destsurface;

        hRes = ti->texture->GetSurfaceLevel( 0, &rendersurface );
        D3D9_CHECK( GetSurfaceLevel );

        hRes = ti->texture2->GetSurfaceLevel( 0, &destsurface );
        D3D9_CHECK( GetSurfaceLevel );

        hRes = m_lpd3ddevice->GetRenderTargetData( rendersurface, destsurface );
        D3D9_CHECK( GetRenderTargetData );

        hRes = ti->texture2->LockRect( 0, &dr, NULL, D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY );
        D3D9_CHECK( LockRect );
        memcpy( ti->bits, dr.pBits, blocsize );
        hRes = ti->texture2->UnlockRect( 0 );    
    }
    else
    {        
        hRes = ti->texture->LockRect( 0, &dr, NULL, D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY );
        D3D9_CHECK( LockRect );

        memcpy( ti->bits, dr.pBits, blocsize );

        hRes = ti->texture->UnlockRect( 0 );    
    }
    return true;
}

bool D3D9Renderer::UpdateTextureContent( void* p_texturedata )
{
    DECLARE_D3D9ASSERT_VARS
    TextureInfos* ti = (TextureInfos*)p_texturedata;

    if( NULL == ti->bits )
    {
        return false;
    }


    long blocsize;
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

        case D3DFMT_R16F:
            bpp = 2;
            break;

        case D3DFMT_R32F:
            bpp = 4;
            break;

        case D3DFMT_A32B32G32R32F:
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

    D3DLOCKED_RECT dr;

    if( ti->render_texture )
    {
        // vouloir updater une render texture = non sens
        return false;
    }
    else
    {        
        hRes = ti->texture->LockRect( 0, &dr, NULL, D3DLOCK_NOSYSLOCK );
        D3D9_CHECK( LockRect );
        
        memcpy( dr.pBits, ti->bits, blocsize );

        hRes = ti->texture->UnlockRect( 0 );    
    }
    return true;
}


bool D3D9Renderer::CreateShaders( DrawSpace::Core::Fx* p_fx, void** p_data )
{
    DECLARE_D3D9ASSERT_VARS

    dsstring hash;
    p_fx->GetShadersMD5( hash );

    if( m_shaders_bases.count( hash ) > 0 )
    {
        *p_data = (void *)m_shaders_bases[hash];
        return true;
    }

    ShadersData* sdata = _DRAWSPACE_NEW_( ShadersData, ShadersData );

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

        sdata->vertex_shader = vs;

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

        sdata->pixel_shader = ps;
    }

    *p_data = (void*)sdata;

    m_shaders_bases[hash] = sdata;

    return true;
}

bool D3D9Renderer::SetShaders( void* p_data )
{
    DECLARE_D3D9ASSERT_VARS

	ShadersData* sdata = (ShadersData*)p_data;

    hRes = m_lpd3ddevice->SetVertexShader( sdata->vertex_shader );
    D3D9_CHECK( SetVertexShader );

    hRes = m_lpd3ddevice->SetPixelShader( sdata->pixel_shader );
    D3D9_CHECK( SetPixelShader );

    return true;
}

bool D3D9Renderer::ApplyRenderStatesIn( void* p_data )
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

bool D3D9Renderer::ApplyRenderStatesOut( void* p_data )
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

bool D3D9Renderer::SetFxShaderMatrix( int p_shader_index, long p_register, DrawSpace::Utils::Matrix& p_mat )
{
	switch( p_shader_index )
	{
		case 0:

			// vertex shader params application		
			set_vertexshader_constants( p_register, p_mat.GetArray(), 4 );
			break;

		case 1:

			// pixel shader params application
			set_pixelshader_constants( p_register, p_mat.GetArray(), 4 );
			break;

        default:
            return false;
	}

	return false;
}

bool D3D9Renderer::SetShaderVectorBuffer(int p_shader_index, long p_register, const std::vector<DrawSpace::Utils::Vector>& p_vectors)
{
	DECLARE_D3D9ASSERT_VARS

	if (p_vectors.size() > NbMaxVectorForShadersBuffers)
	{
		_DSEXCEPTION("Too many vectors");
	}

	long index = 0;
	for (auto e : p_vectors)
	{
		m_shaders_array[index++] = e[0];
		m_shaders_array[index++] = e[1];
		m_shaders_array[index++] = e[2];
		m_shaders_array[index++] = e[3];
	}

	switch (p_shader_index)
	{
		case 0:	
			hRes = m_lpd3ddevice->SetVertexShaderConstantF(p_register, m_shaders_array, p_vectors.size());
			D3D9_CHECK(SetVertexShaderConstantF);
			break;

		case 1:
			hRes = m_lpd3ddevice->SetPixelShaderConstantF(p_register, m_shaders_array, p_vectors.size());
			D3D9_CHECK(SetPixelShaderConstantF);
			break;

		default:
			return false;
	}
	return true;
}

bool D3D9Renderer::DrawMeshe( DrawSpace::Utils::Matrix p_world, DrawSpace::Utils::Matrix p_view, DrawSpace::Utils::Matrix p_proj )
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
    chain.PushMatrix( p_proj );
    chain.PushMatrix( final_view );
    chain.PushMatrix( p_world );
    chain.BuildResult();
    chain.GetResult( &result );
    result.Transpose();

    set_vertexshader_constants( 0, result.GetArray(), 4 );
    set_pixelshader_constants( 100, result.GetArray(), 4 );
    
    //////////////////////////////////////////////////////////////////////

    DrawSpace::Utils::Matrix proj = p_proj;
    DrawSpace::Utils::Matrix world = p_world;
    DrawSpace::Utils::Matrix view = p_view;
    DrawSpace::Utils::Matrix cam = p_view;
    DrawSpace::Utils::Matrix worldview = world * view;
    worldview.Transpose();
    
    set_vertexshader_constants( 4, worldview.GetArray(), 4 );
    set_pixelshader_constants( 104, worldview.GetArray(), 4 );

	//////////////////////////////////////////////////////////////////////
    
    world.Transpose();
    view.Transpose();
    cam.Inverse();
    cam.Transpose();

    set_vertexshader_constants( 8, world.GetArray(), 4 );
    set_vertexshader_constants( 12, view.GetArray(), 4 );

    set_pixelshader_constants( 108, world.GetArray(), 4 );
    set_pixelshader_constants( 112, view.GetArray(), 4 );

	//////////////////////////////////////////////////////////////////////

    set_vertexshader_constants( 16, cam.GetArray(), 4 );
    set_pixelshader_constants( 116, cam.GetArray(), 4 );


    proj.Transpose();
    set_vertexshader_constants( 20, proj.GetArray(), 4 );
    set_pixelshader_constants( 120, proj.GetArray(), 4 );


	hRes = m_lpd3ddevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_next_nbvertices, 0, m_next_nbtriangles );

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
                    //ftype = D3DTEXF_NONE;
                    ftype = D3DTEXF_POINT;
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


        case DrawSpace::Core::RenderState::SETVERTEXTEXTUREFILTERTYPE:
            {
                D3DTEXTUREFILTERTYPE ftype;

                if( "none" == arg )
                {
                    //ftype = D3DTEXF_NONE;
                    ftype = D3DTEXF_POINT;
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
                    m_lpd3ddevice->SetSamplerState( i + D3DVERTEXTEXTURESAMPLER0, D3DSAMP_MAGFILTER, ftype );
                    m_lpd3ddevice->SetSamplerState( i + D3DVERTEXTEXTURESAMPLER0, D3DSAMP_MINFILTER, ftype );
                }
            }
            break;

        case DrawSpace::Core::RenderState::SETFILLMODE:
            {
                D3DFILLMODE fm;
                if( "line" == arg )
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

bool D3D9Renderer::GUI_InitSubSystem( void )
{
    if( !m_lpd3ddevice )
    {
        _DSEXCEPTION( "Cannot initialize GUI sub system cause D3D9 is currently not initialized" )
    }

    CEGUI::Direct3D9Renderer::bootstrapSystem( m_lpd3ddevice );

    m_guisubsystem.SetReady( true );
    return true;
}

void D3D9Renderer::GUI_ReleaseSubSystem( void )
{
    if( m_lpd3ddevice )
    {
        CEGUI::Direct3D9Renderer::destroySystem();
    }
    m_guisubsystem.SetReady( false );
}

void D3D9Renderer::GUI_Render( void )
{
    // save some render states
    DWORD alphablendenable_state;
    m_lpd3ddevice->GetRenderState( D3DRS_ALPHABLENDENABLE, &alphablendenable_state );

    DWORD cull_state;
    m_lpd3ddevice->GetRenderState( D3DRS_CULLMODE, &cull_state );

    DWORD fill_state;
    m_lpd3ddevice->GetRenderState( D3DRS_FILLMODE, &fill_state );
    
    m_guisubsystem.RenderGUI();

    // restore previous render states...
    m_lpd3ddevice->SetVertexDeclaration( m_vertexdeclaration );
    m_lpd3ddevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );
    m_lpd3ddevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
    
    m_lpd3ddevice->SetRenderState( D3DRS_ALPHABLENDENABLE, alphablendenable_state );
    m_lpd3ddevice->SetRenderState( D3DRS_CULLMODE, cull_state );
    m_lpd3ddevice->SetRenderState( D3DRS_FILLMODE, fill_state );    
}

void D3D9Renderer::GUI_SetResourcesRootDirectory( const dsstring& p_path )
{
    m_guisubsystem.SetResourcesRootDirectory( p_path );
}

void D3D9Renderer::GUI_OnMouseMove( float p_xm, float p_ym, float p_dx, float p_dy )
{
    m_guisubsystem.OnMouseMove( p_xm, p_ym, p_dx, p_dy );
}

void D3D9Renderer::GUI_OnMouseLeftButtonDown( void )
{
    m_guisubsystem.OnMouseLeftButtonDown();
}

void D3D9Renderer::GUI_OnMouseLeftButtonUp( void )
{
    m_guisubsystem.OnMouseLeftButtonUp();
}

void D3D9Renderer::GUI_OnMouseRightButtonDown( void )
{
    m_guisubsystem.OnMouseRightButtonDown();
}

void D3D9Renderer::GUI_OnMouseRightButtonUp( void )
{
    m_guisubsystem.OnMouseRightButtonUp();
}

void D3D9Renderer::GUI_InitTest( void )
{
    m_guisubsystem.InitTest();
}

void D3D9Renderer::GUI_OnKeyDown( long p_key )
{
    m_guisubsystem.OnKeyDown( p_key );
}

void D3D9Renderer::GUI_OnKeyUp( long p_key )
{
    m_guisubsystem.OnKeyUp( p_key );
}

void D3D9Renderer::GUI_OnChar( long p_key )
{
    m_guisubsystem.OnChar( p_key );
}

void D3D9Renderer::GUI_LoadLayout( const dsstring& p_layout_path, const dsstring& p_widgets_path )
{
    m_guisubsystem.LoadLayout( p_layout_path, p_widgets_path );
}

void D3D9Renderer::GUI_UnloadAllLayouts( void )
{
    m_guisubsystem.UnloadAllLayouts();
}

void D3D9Renderer::GUI_LoadScheme( const dsstring& p_scheme_path )
{
    m_guisubsystem.LoadScheme( p_scheme_path );
}

void D3D9Renderer::GUI_UnloadAllSchemes( void )
{
    m_guisubsystem.UnloadAllSchemes();
}

void D3D9Renderer::GUI_StoreWidget( const dsstring& p_layoutName, const dsstring& p_parentName, const dsstring& p_childName )
{
    m_guisubsystem.Store( p_layoutName, p_parentName, p_childName );
}

void D3D9Renderer::GUI_SetVisibleState( const dsstring& p_layoutName, const dsstring& p_widgetName, bool p_state )
{
    m_guisubsystem.SetVisibleState( p_layoutName, p_widgetName, p_state );
}

bool D3D9Renderer::GUI_IsVisible( const dsstring& p_layoutName, const dsstring& p_widgetName )
{
    return m_guisubsystem.IsVisible( p_layoutName, p_widgetName );
}

void D3D9Renderer::GUI_SetWidgetText( const dsstring& p_layoutName, const dsstring& p_widgetName, const dsstring& p_text )
{
    m_guisubsystem.SetText( p_layoutName, p_widgetName, p_text );
}

void D3D9Renderer::GUI_GetWidgetText( const dsstring& p_layoutName, const dsstring& p_widgetName, dsstring& p_outtext )
{
    m_guisubsystem.GetText( p_layoutName, p_widgetName, p_outtext );
}

void D3D9Renderer::GUI_AddListboxTextItem( const dsstring& p_layoutName, const dsstring& p_widgetName, const dsstring& p_text, unsigned int p_colors, const dsstring& p_brushImage )
{
    m_guisubsystem.AddListboxTextItem( p_layoutName, p_widgetName, p_text, p_colors, p_brushImage );
}

void D3D9Renderer::GUI_ClearListbox( const dsstring& p_layoutName, const dsstring& p_widgetName )
{
    m_guisubsystem.ClearListbox( p_layoutName, p_widgetName );
}

bool D3D9Renderer::GUI_GetListboxFirstSelectedItemIndex( const dsstring& p_layoutName, const dsstring& p_widgetName, int& p_index, dsstring& p_text )
{
    return m_guisubsystem.GetListboxFirstSelectedItemIndex( p_layoutName, p_widgetName, p_index, p_text );
}

void D3D9Renderer::GUI_AddComboboxTextItem( const dsstring& p_layoutName, const dsstring& p_widgetName, const dsstring& p_text, unsigned int p_colors, const dsstring& p_brushImage )
{
    m_guisubsystem.AddComboboxTextItem( p_layoutName, p_widgetName, p_text, p_colors, p_brushImage );
}

void D3D9Renderer::GUI_ClearCombobox( const dsstring& p_layoutName, const dsstring& p_widgetName )
{
    m_guisubsystem.ClearCombobox( p_layoutName, p_widgetName );
}

void D3D9Renderer::GUI_SetComboBoxItemSelectionState( const dsstring& p_layoutName, const dsstring& p_widgetName, int p_index, bool p_state )
{
    m_guisubsystem.SetComboBoxItemSelectionState( p_layoutName, p_widgetName, p_index, p_state );
}

int D3D9Renderer::GUI_GetComboBoxSelectionIndex( const dsstring& p_layoutName, const dsstring& p_widgetName )
{
    return m_guisubsystem.GetComboBoxSelectionIndex( p_layoutName, p_widgetName );
}

bool D3D9Renderer::GUI_IsCheckBoxChecked( const dsstring& p_layoutName, const dsstring& p_widgetName )
{
    return m_guisubsystem.IsCheckBoxChecked( p_layoutName, p_widgetName );
}

void D3D9Renderer::GUI_SetCheckboxState( const dsstring& p_layoutName, const dsstring& p_widgetName, bool p_state )
{
    m_guisubsystem.SetCheckboxState( p_layoutName, p_widgetName, p_state );
}

void D3D9Renderer::GUI_RegisterPushButtonEventClickedHandler( DrawSpace::Core::BaseCallback2<void, const dsstring&, const dsstring&>* p_handler )
{
    m_guisubsystem.RegisterPushButtonEventClickedHandler( p_handler );
}

void D3D9Renderer::GUI_RegisterCheckboxEventStateChangedHandler( DrawSpace::Core::BaseCallback3<void, const dsstring&, const dsstring&, bool>* p_handler )
{
    m_guisubsystem.RegisterCheckboxEventStateChangedHandler( p_handler );
}

void D3D9Renderer::GUI_SetLayout( const dsstring& p_layoutpath )
{
    m_guisubsystem.SetLayout( p_layoutpath );
}

void D3D9Renderer::GUI_SetMouseCursorImage( const dsstring& p_image )
{
    m_guisubsystem.SetMouseCursorImage( p_image );
}

void D3D9Renderer::GUI_ShowMouseCursor( bool p_show )
{
    m_guisubsystem.ShowMouseCursor( p_show );
}

void D3D9Renderer::GUI_CreateSprite( const dsstring& p_scheme_object, const dsstring& p_spriteName )
{
    m_guisubsystem.CreateSprite( p_scheme_object, p_spriteName );
}

void D3D9Renderer::GUI_SetSpritePosition( const dsstring& p_spriteName, dsreal p_xpos, dsreal p_ypos )
{
    m_guisubsystem.SetSpritePosition( p_spriteName, p_xpos, p_ypos );
}

void D3D9Renderer::GUI_SetSpriteImage( const dsstring& p_spriteName, const dsstring& p_image )
{
    m_guisubsystem.SetSpriteImage( p_spriteName, p_image );
}

void D3D9Renderer::GUI_SetSpriteScale( const dsstring& p_spriteName, dsreal p_scale )
{
    m_guisubsystem.SetSpriteScale( p_spriteName, p_scale );
}

void D3D9Renderer::GUI_SetSpriteRotation( const dsstring& p_spriteName, const DrawSpace::Utils::Vector& p_axis, dsreal p_deg_angle )
{
    m_guisubsystem.SetSpriteRotation( p_spriteName, p_axis, p_deg_angle );
}
