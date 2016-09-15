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

#ifndef _D3D11RENDERER_H_
#define _D3D11RENDERER_H_

#pragma warning( disable : 4005 )

#include <d3d11.h>
#include <d3dx11.h>

#include <memalloc.h>
#include <tracedefs.h>
#include <renderer.h>
#include <parser.h>
#include <fx.h>
#include <vector.h>
#include <matrix.h>
#include <transformation.h>
#include <FW1FontWrapper.h>

#include "d3d11vertex.h"
#include "d3d11triangle.h"

#define DECLARE_D3D11ASSERT_VARS HRESULT hRes; \
                                 dsstring d3dErrStr;

#define D3D11_CHECK( p_mName ) \
    if( hRes != S_OK ) \
    { \
        TranslateD3DD11Error( hRes, d3dErrStr ); \
        dsstring dstr = " "#p_mName" -> "; \
        dstr += d3dErrStr; \
        dstr += "\n"; \
        _DSERROR( logger, dstr.c_str() ); \
        return false; \
    }

class D3D11Renderer : public DrawSpace::Interface::Renderer
{
protected:

    class Config : public DrawSpace::Utils::Parser
    {
    protected:
        bool on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words );

    public:
        int                         m_adapter_ordinal;
        int                         m_fullscreen_width;
        int                         m_fullscreen_height;
        int                         m_refreshrate;
        DXGI_FORMAT                 m_fullscreen_format;
        DWORD                       m_vertex_processing;
    };

    typedef struct
    {
        dsstring                    path;
        DrawSpace::Core::Texture*   texture_instance;

        D3D11_TEXTURE2D_DESC        descr;

        ID3D11Texture2D*            texture;
        
        ID3D11RenderTargetView*     rendertextureTargetView;
        ID3D11ShaderResourceView*   textureShaderResourceView;

    } TextureInfos;

    typedef struct
    {
	    ID3D11Buffer*               vertex_buffer;
	    ID3D11Buffer*               index_buffer;
        int                         nb_vertices;
        int                         nb_triangles;

    } MesheData;


    typedef struct
    {
        ID3D11VertexShader*         vertex_shader;
        ID3D11PixelShader*          pixel_shader;

    } ShadersData;
    

    Config                              m_config;


    HWND                                m_hwnd;

    Characteristics                     m_characteristics;

    IDXGISwapChain*                     m_lpd3dswapchain;
    ID3D11Device*                       m_lpd3ddevice;                     
    ID3D11DeviceContext*                m_lpd3ddevcontext;   

    ID3D11RenderTargetView*             m_screentarget;

    D3D11_VIEWPORT                      m_viewport;

    IFW1FontWrapper*                    m_fontWrapper;

    std::vector<DeviceDescr>            m_devices_descrs;
    int                                 m_currentDevice;

    std::map<dsstring, TextureInfos*>   m_textures_base;
    std::map<dsstring, MesheData*>      m_meshes_base;
    std::map<dsstring, ShadersData*>    m_shaders_bases;


    HRESULT D3D11Renderer::compile_shader_from_file( void* p_data, int p_size, LPCTSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut );


public:
    D3D11Renderer( void );
    ~D3D11Renderer( void );

    virtual void GetDescr( dsstring& p_descr );
    virtual void GetDeviceDescr( DeviceDescr& p_ddescr );
    virtual void DumpMemoryAllocs( void );

    virtual bool Init( HWND p_hwnd, bool p_fullscreen, long p_w_width, long p_w_height, DrawSpace::Logger::Configuration* p_logconf );
    virtual void Release( void );

    virtual void SetViewport( bool p_automatic, long p_vpx, long p_vpy, long p_vpwidth, long p_vpheight, float p_vpminz, float p_vpmaxz );
    
    virtual void BeginScreen( void );
    virtual void EndScreen( void );
    virtual void FlipScreen( void );

    virtual void ClearScreen( unsigned char p_r, unsigned char p_g, unsigned char p_b, unsigned char p_a );
    virtual void ClearDepth( dsreal p_value = 1.0 );

    virtual void BeginTarget( DrawSpace::Core::Texture* p_texture );
    virtual void EndTarget( DrawSpace::Core::Texture* p_texture );

    virtual bool CreateMeshe( DrawSpace::Core::Meshe* p_meshe, void** p_data );
    virtual void RemoveMeshe( DrawSpace::Core::Meshe* p_meshe, void* p_data );    
    virtual bool SetMeshe( void* p_data );
    virtual bool UpdateMesheIndexes( DrawSpace::Core::Meshe* p_meshe, void* p_data );
    virtual bool UpdateMesheVertices( DrawSpace::Core::Meshe* p_meshe, void* p_data );
    virtual bool UpdateMesheVerticesFromImpostors( const DrawSpace::ImpostorsDisplayList& p_list, void* p_data );

    virtual bool CreateTexture( DrawSpace::Core::Texture* p_texture, void** p_data );
    virtual void DestroyTexture( void* p_data );
    virtual bool SetTexture( void* p_data, int p_stage );
    virtual bool SetVertexTexture( void* p_data, int p_stage );
	virtual bool UnsetTexture( int p_stage );
    virtual bool UnsetVertexTexture( int p_stage );
    virtual bool AllocTextureContent( void* p_texturedata );
    virtual void ReleaseTextureContent( void* p_texturedata );
    virtual void* GetTextureContentPtr( void* p_texturedata );
    virtual bool CopyTextureContent( void* p_texturedata );
    virtual bool UpdateTextureContent( void* p_texturedata );


    virtual bool CreateShaders( DrawSpace::Core::Fx* p_fx, void** p_data );
    virtual bool SetShaders( void* p_data );

    virtual bool CreateRenderStatesSet( DrawSpace::Core::Fx* p_fx, void** p_data );
    virtual bool ApplyRenderStatesIn( void* p_data );
    virtual bool ApplyRenderStatesOut( void* p_data );


    virtual bool SetFxShaderParams( int p_shader_index, long p_register, DrawSpace::Utils::Vector& p_vector );
    virtual bool SetFxShaderMatrix( int p_shader_index, long p_register, DrawSpace::Utils::Matrix& p_mat );

	virtual bool DrawMeshe( DrawSpace::Utils::Matrix p_world, DrawSpace::Utils::Matrix p_view, DrawSpace::Utils::Matrix p_proj );

    virtual void SetRenderState( DrawSpace::Core::RenderState* p_renderstate );

    virtual void GetRenderCharacteristics( Characteristics& p_characteristics );

    virtual void DrawText( long p_r, long p_g, long p_b, int p_posX, int p_posY, const char* p_format, ... );

    virtual void PointProjection( DrawSpace::Utils::Matrix p_view, DrawSpace::Utils::Matrix p_proj, DrawSpace::Utils::Vector& p_point, dsreal& p_outx, dsreal& p_outy, dsreal& p_outz );
};

#endif
