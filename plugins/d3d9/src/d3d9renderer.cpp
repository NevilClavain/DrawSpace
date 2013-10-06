/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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
#include "memalloc.h"

using namespace DrawSpace;
extern void TranslateD3DD9Error( HRESULT p_hRes, dsstring &p_str );

_DECLARE_DS_LOGGER( logger, "d3d9" )

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

bool D3D9Renderer::Init( HWND p_hwnd, bool p_fullscreen, long p_w_width, long p_w_height )
{
    D3DPRESENT_PARAMETERS d3dpp;
    DECLARE_D3D9ASSERT_VARS
    D3DDISPLAYMODE d3ddm;

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
    ::GetClientRect( m_hwnd, &m_wndrect );

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

    _DSDEBUG( logger, "vertex shaders version = " << vs_version_maj << "." << vs_version_min << " pixel shaders version = " << ps_version_maj << "." << ps_version_min );

    if( vs_version_maj < 3 || ps_version_maj < 3 )
    {
        _DSFATAL( logger, "Shader model 3.0 unsupported !" )
        return false;
    }
    else
    {
        _DSDEBUG( logger, "Shader model 3.0 check OK" )
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

    _DSDEBUG( logger, "Selected depth buffer format = " << depth_format )
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

        _DSDEBUG( logger, " -> FULLSCREEN " << d3dpp.BackBufferWidth << " x " << d3dpp.BackBufferHeight << " format " << d3dpp.BackBufferFormat )
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

        _DSDEBUG( logger, " -> WINDOWED " << p_w_width << " x " << p_w_height )
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
        { 0, 168, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 9 },
        D3DDECL_END()
    };

    hRes = m_lpd3ddevice->CreateVertexDeclaration( vdecl, &m_vertexdeclaration );
    D3D9_CHECK( CreateVertexDeclaration )
    
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

    _DSDEBUG( logger, "projection : v_width = " << v_width << " v_height = " << v_height );

    SetProjection( v_width, v_height, 1.0, 1000.0 );

    m_characteristics.width_viewport = v_width;
    m_characteristics.height_viewport = v_height;

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

void D3D9Renderer::SetViewport( bool p_windowed, long p_vpx, long p_vpy, long p_vpwidth, long p_vpheight, float p_vpminz, float p_vpmaxz )
{
    if( p_windowed )
    {
        m_viewport.X = m_wndrect.left;
        m_viewport.Y = m_wndrect.top;
        m_viewport.Width = m_wndrect.right - m_wndrect.left;
        m_viewport.Height = m_wndrect.bottom - m_wndrect.top;
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

void D3D9Renderer::SetProjection( float p_vw, float p_vh, float p_zn, float p_zf )
{
    m_projection.Perspective( p_vw, p_vh, p_zn, p_zf );
}

bool D3D9Renderer::CreateRenderingNode( DrawSpace::Core::RenderingNode* p_node )
{
    DECLARE_D3D9ASSERT_VARS

    NodeInfos					node_infos;
    LPDIRECT3DVERTEXSHADER9		vs;
    LPDIRECT3DPIXELSHADER9		ps;
    
    Core::Fx* fx = p_node->GetFx();

    if( NULL != fx && 2 == fx->GetShadersListSize() )
    {
        /////////////////// Shaders loading

        Core::Shader* vertex_shader = fx->GetShader( 0 );
        Core::Shader* pixel_shader = fx->GetShader( 1 );

        LPD3DXBUFFER vbuff;
        LPD3DXBUFFER pbuff;
        LPD3DXBUFFER errors; 

        if( !vertex_shader->IsCompiled() )
        {
            if( vertex_shader->GetData() == NULL )
            {
                _DSFATAL( logger, "no data in vertex shader !" )
                return false;
            }

            hRes = D3DXCompileShader( (LPCSTR)vertex_shader->GetData(), vertex_shader->GetDataSize(), NULL, NULL, "vs_main", "vs_3_0", 0, &vbuff, &errors, NULL );
            if( D3D_OK != hRes )
            {
                if( NULL != errors )
                {
                    _DSFATAL( logger, "D3DXCompileShader FAIL : " << (char *)errors->GetBufferPointer() )
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

        vertex_shader->SetRenderReady();

        if( !pixel_shader->IsCompiled() )
        {
            if( pixel_shader->GetData() == NULL )
            {
                _DSFATAL( logger, "no data in pixel shader !" )
                return false;
            }

            hRes = D3DXCompileShader( (LPCSTR)pixel_shader->GetData(), pixel_shader->GetDataSize(), NULL, NULL, "ps_main", "ps_3_0", 0, &pbuff, &errors, NULL );
            if( D3D_OK != hRes )
            {
                if( NULL != errors )
                {
                    _DSFATAL( logger, "D3DXCompileShader FAIL : " << (char *)errors->GetBufferPointer() )
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

        pixel_shader->SetRenderReady();

        node_infos.vertex_shader = vs;
        node_infos.pixel_shader  = ps;

        /////////////////// Textures loading

        node_infos.nb_stages = 0;
        DrawSpace::Core::Texture* current_texture;
        long nb_textures = p_node->GetTextureListSize();

        for( long i = 0; i < nb_textures; i++ )
        {
            LPDIRECT3DTEXTURE9	d3dt9;
            // fait office d'id
            dsstring path;
            current_texture = p_node->GetTexture( i );
            if( NULL == current_texture )
            {
                continue;
            }
            current_texture->GetPath( path );

            if( 0 == m_textures_base.count( path ) )
            {
                if( current_texture->IsRenderTarget() )
                {
                    LPDIRECT3DSURFACE9		surface;
                    LPD3DXRENDERTOSURFACE	render_to_surface;
                    D3DSURFACE_DESC			desc;

                    unsigned long rw, rh;
                    current_texture->GetRenderTargetDims( rw, rh );

                    hRes = m_lpd3ddevice->CreateTexture( rw, rh, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &d3dt9, NULL );
                    D3D9_CHECK( CreateTexture )

                    d3dt9->GetSurfaceLevel( 0, &surface );
                    surface->GetDesc( &desc );

                    hRes = D3DXCreateRenderToSurface( m_lpd3ddevice, desc.Width, desc.Height, desc.Format, TRUE, m_depthbuffer_format, &render_to_surface );
                    D3D9_CHECK( D3DXCreateRenderToSurface )

                    if( i < 8 )
                    {
                        node_infos.textures[i] = d3dt9;						
                        node_infos.nb_stages++;

                        TargetTextureInfos targettexture_infos;

                        targettexture_infos.render_to_surface = render_to_surface;
                        targettexture_infos.surface = surface;

                        m_targettextures_base[current_texture] = targettexture_infos;

                        TextureInfos texture_infos;
                        texture_infos.texture = d3dt9;
                        texture_infos.descr = desc;

                        m_textures_base[path] = texture_infos;

                    }
                    else
                    {
                        _DSWARN( logger, "Max D3D9 texture stage reached" ) 
                    }
                }
                else
                {
                    _DSDEBUG( logger, "texture " << path.c_str() << " is new, loading it..." )
                    
                    void* data = current_texture->GetData();
                    long data_size = current_texture->GetDataSize();
                    hRes = D3DXCreateTextureFromFileInMemory( m_lpd3ddevice, data, data_size, &d3dt9 );
                    D3D9_CHECK( D3DXCreateTextureFromFileInMemory );
                    if( i < 8 )
                    {
                        node_infos.textures[i] = d3dt9;
                        node_infos.nb_stages++;

                        TextureInfos texture_infos;
                        texture_infos.texture = d3dt9;
                        d3dt9->GetLevelDesc( 0, &texture_infos.descr );

                        m_textures_base[path] = texture_infos;
                    }
                    else
                    {
                        _DSWARN( logger, "Max D3D9 texture stage reached" ) 
                    }
                }			
            }
            else
            {
                _DSDEBUG( logger, "texture " << path.c_str() << " exists..." )
                node_infos.textures[i] = m_textures_base[path].texture;
                node_infos.nb_stages++;
            }
            current_texture->SetRenderReady();
        }
    }
    else
    {
        if( NULL == fx )
        {
            _DSWARN( logger, "no associated fx" )
        }

        /*
        if( 2 != fx->GetShadersListSize() )
        {
            _DSWARN( logger, "fx does not have 2 shaders" )
        }
        */
    }
    /////////////////// final step : registering infos
    m_nodes[p_node] = node_infos;

    return true;
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


bool D3D9Renderer::BeginNodeRender( DrawSpace::Core::RenderingNode* p_node, long p_textures_set_index )
{
    DECLARE_D3D9ASSERT_VARS

    NodeInfos node_infos;

    if( 0 == m_nodes.count( p_node ) )
    {
        return false;
    }
    node_infos = m_nodes[p_node];

    // setting textures
    for( long i = 0; i < node_infos.nb_stages; i++ )
    {
        hRes = m_lpd3ddevice->SetTexture( i, node_infos.textures[i] );
        D3D9_CHECK( SetTexture );
    }

    // apply fx
    DrawSpace::Core::Fx* fx = p_node->GetFx();

    long nb_rs_in = fx->GetRenderStatesInListSize();
    for( long i = 0; i < nb_rs_in; i++ )
    {
        DrawSpace::Core::RenderState rs = fx->GetRenderStateIn( i );
        SetRenderState( &rs );
    }

    DrawSpace::Core::Shader* vertex_shader;
    DrawSpace::Core::Shader* pixel_shader;

    vertex_shader = fx->GetShader( 0 );
    pixel_shader = fx->GetShader( 1 );

    // vertex shader params application
    std::map<long, Utils::Vector> vertexshader_table;
    vertex_shader->GetParamsTable( vertexshader_table );
    for( std::map<long, Utils::Vector>::iterator it = vertexshader_table.begin(); it != vertexshader_table.end(); ++it )
    {
        set_vertexshader_constants( (*it).first, (*it).second.GetArray(), 1 );
    }

    hRes = m_lpd3ddevice->SetVertexShader( node_infos.vertex_shader );
    D3D9_CHECK( SetVertexShader );
    m_lpd3ddevice->SetVertexDeclaration( m_vertexdeclaration );

    // pixel shader params application
    std::map<long, Utils::Vector> pixelshader_table;
    pixel_shader->GetParamsTable( pixelshader_table );
    for( std::map<long, Utils::Vector>::iterator it = pixelshader_table.begin(); it != pixelshader_table.end(); ++it )
    {
        set_pixelshader_constants( (*it).first, (*it).second.GetArray(), 1 );
    }

    hRes = m_lpd3ddevice->SetPixelShader( node_infos.pixel_shader );
    D3D9_CHECK( SetPixelShader );

    return true;
}

bool D3D9Renderer::EndNodeRender( DrawSpace::Core::RenderingNode* p_node )
{
    m_lpd3ddevice->SetVertexShader( NULL );
    m_lpd3ddevice->SetPixelShader( NULL );

    m_lpd3ddevice->SetTexture( 0, NULL );
    m_lpd3ddevice->SetTexture( 1, NULL );
    m_lpd3ddevice->SetTexture( 2, NULL );
    m_lpd3ddevice->SetTexture( 3, NULL );
    m_lpd3ddevice->SetTexture( 4, NULL );
    m_lpd3ddevice->SetTexture( 5, NULL );
    m_lpd3ddevice->SetTexture( 6, NULL );
    m_lpd3ddevice->SetTexture( 7, NULL );

    // unapply fx
    DrawSpace::Core::Fx* fx = p_node->GetFx();
    long nb_rs_out = fx->GetRenderStatesOutListSize();
    for( long i = 0; i < nb_rs_out; i++ )
    {
        DrawSpace::Core::RenderState rs = fx->GetRenderStateOut( i );
        SetRenderState( &rs );
    }
    return true;
}

long D3D9Renderer::AddMesheToNode( DrawSpace::Core::Meshe* p_meshe, DrawSpace::Core::RenderingNode* p_node )
{
    if( 0 == m_nodes.count( p_node ) )
    {
        return -1;
    }
    
    Core::Meshe* meshe = p_meshe;
    long nb_vertices = meshe->GetVertexListSize();
    long nb_triangles = meshe->GetTrianglesListSize();

    MesheData meshe_data;
    meshe_data.vertices = _DRAWSPACE_NEW_EXPLICIT_SIZE_( d3d9vertex, d3d9vertex[nb_vertices], sizeof( d3d9vertex ) * nb_vertices );
    meshe_data.triangles = _DRAWSPACE_NEW_EXPLICIT_SIZE_( d3d9triangle, d3d9triangle[nb_triangles], sizeof( d3d9triangle ) * nb_triangles );


    meshe_data.nb_vertices = nb_vertices;
    meshe_data.nb_triangles = nb_triangles;

    for( long i = 0; i < nb_vertices; i++ )
    {
        Core::Vertex vertex;
        meshe->GetVertex( i, vertex );

        meshe_data.vertices[i].x = (float)vertex.x;
        meshe_data.vertices[i].y = (float)vertex.y;
        meshe_data.vertices[i].z = (float)vertex.z;

        meshe_data.vertices[i].nx = (float)vertex.nx;
        meshe_data.vertices[i].ny = (float)vertex.ny;
        meshe_data.vertices[i].nz = (float)vertex.nz;

        meshe_data.vertices[i].tu0 = vertex.tu[0];
        meshe_data.vertices[i].tu1 = vertex.tu[1];
        meshe_data.vertices[i].tu2 = vertex.tu[2];
        meshe_data.vertices[i].tu3 = vertex.tu[3];
        meshe_data.vertices[i].tu4 = vertex.tu[4];
        meshe_data.vertices[i].tu5 = vertex.tu[5];
        meshe_data.vertices[i].tu6 = vertex.tu[6];
        meshe_data.vertices[i].tu7 = vertex.tu[7];
        meshe_data.vertices[i].tu8 = vertex.tu[8];
        meshe_data.vertices[i].tu9 = vertex.tu[9];

        meshe_data.vertices[i].tv0 = vertex.tv[0];
        meshe_data.vertices[i].tv1 = vertex.tv[1];
        meshe_data.vertices[i].tv2 = vertex.tv[2];
        meshe_data.vertices[i].tv3 = vertex.tv[3];
        meshe_data.vertices[i].tv4 = vertex.tv[4];
        meshe_data.vertices[i].tv5 = vertex.tv[5];
        meshe_data.vertices[i].tv6 = vertex.tv[6];
        meshe_data.vertices[i].tv7 = vertex.tv[7];
        meshe_data.vertices[i].tv8 = vertex.tv[8];
        meshe_data.vertices[i].tv9 = vertex.tv[9];

        meshe_data.vertices[i].tw0 = vertex.tw[0];
        meshe_data.vertices[i].tw1 = vertex.tw[1];
        meshe_data.vertices[i].tw2 = vertex.tw[2];
        meshe_data.vertices[i].tw3 = vertex.tw[3];
        meshe_data.vertices[i].tw4 = vertex.tw[4];
        meshe_data.vertices[i].tw5 = vertex.tw[5];
        meshe_data.vertices[i].tw6 = vertex.tw[6];
        meshe_data.vertices[i].tw7 = vertex.tw[7];
        meshe_data.vertices[i].tw8 = vertex.tw[8];
        meshe_data.vertices[i].tw9 = vertex.tw[9];

        meshe_data.vertices[i].ta0 = vertex.ta[0];
        meshe_data.vertices[i].ta1 = vertex.ta[1];
        meshe_data.vertices[i].ta2 = vertex.ta[2];
        meshe_data.vertices[i].ta3 = vertex.ta[3];
        meshe_data.vertices[i].ta4 = vertex.ta[4];
        meshe_data.vertices[i].ta5 = vertex.ta[5];
        meshe_data.vertices[i].ta6 = vertex.ta[6];
        meshe_data.vertices[i].ta7 = vertex.ta[7];
        meshe_data.vertices[i].ta8 = vertex.ta[8];
        meshe_data.vertices[i].ta9 = vertex.ta[9];
    }

    for( long i = 0; i < nb_triangles; i++ )
    {
        Core::Triangle triangle;
        meshe->GetTriangles( i, triangle );

        meshe_data.triangles[i].vertex1 = triangle.vertex1;
        meshe_data.triangles[i].vertex2 = triangle.vertex2;
        meshe_data.triangles[i].vertex3 = triangle.vertex3;
    }
    meshe->SetRenderReady();
    //node_infos.meshes.push_back( meshe_data );
    //return (long)node_infos.meshes.size();

    m_nodes[p_node].meshes.push_back( meshe_data );
    return (long)m_nodes[p_node].meshes.size();
}

bool D3D9Renderer::RenderNodeMeshe( DrawSpace::Utils::Matrix p_world, DrawSpace::Utils::Matrix p_view, DrawSpace::Core::RenderingNode* p_node, long p_index )
{
    DECLARE_D3D9ASSERT_VARS

    NodeInfos node_infos;
    if( 0 == m_nodes.count( p_node ) )
    {
        return false;
    }

    node_infos = m_nodes[p_node];

    // setting transformation
    DrawSpace::Utils::Matrix final_view;
    DrawSpace::Utils::Matrix inv;
    DrawSpace::Utils::Matrix result;

    inv.Identity();
    inv( 2, 2 ) = -1.0;
    final_view = p_view * inv;

    DrawSpace::Utils::Transformation chain;
    chain.PushMatrix( m_projection );
    chain.PushMatrix( final_view );
    chain.PushMatrix( p_world );
    chain.BuildResult();
    chain.GetResult( &result );
    result.Transpose();

    set_vertexshader_constants( 0, result.GetArray(), 4 );
    
    //////////////////////////////////////////////////////////////////////
    DrawSpace::Utils::Matrix world = p_world;
    DrawSpace::Utils::Matrix view = p_view;
    world.Transpose();
    view.Transpose();

    set_vertexshader_constants( 4, world.GetArray(), 4 );
    set_vertexshader_constants( 8, view.GetArray(), 4 );

    //////////////////////////////////////////////////////////////////////

    MesheData meshe_data = node_infos.meshes[p_index];
    hRes = m_lpd3ddevice->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, meshe_data.nb_vertices, meshe_data.nb_triangles, meshe_data.triangles, D3DFMT_INDEX32, meshe_data.vertices, sizeof( d3d9vertex ) );
    D3D9_CHECK( DrawIndexedPrimitiveUP );
    return true;
}

void D3D9Renderer::SetRenderState( DrawSpace::Core::RenderState* p_renderstate )
{
    dsstring arg;
    p_renderstate->GetArg( arg );

    switch( p_renderstate->GetOperation() )
    {
        case DrawSpace::Core::RenderState::SETCLEARMASK:
            break;

        case DrawSpace::Core::RenderState::SETCLEARCOLOR:
            break;

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
            break;

        case DrawSpace::Core::RenderState::ALPHABLENDOP:
            break;

        case DrawSpace::Core::RenderState::ALPHABLENDFUNC:
            break;

        case DrawSpace::Core::RenderState::ALPHABLENDDEST:
            break;

        case DrawSpace::Core::RenderState::ALPHABLENDSRC:
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
