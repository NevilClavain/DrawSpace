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

#ifndef _D3D11RENDERER_H_
#define _D3D11RENDERER_H_

#pragma warning( disable : 4005 )

#include <d3d11.h>
#include <d3dx11.h>
#include <xnamath.h>

#include <memalloc.h>
#include <tracedefs.h>
#include <renderer.h>
#include <parser.h>
#include <fx.h>
#include <vector.h>
#include <matrix.h>
#include <transformation.h>
#include <FW1FontWrapper.h>

#include "CEGUIWrapper.h"
#include <CEGUI\RendererModules\Direct3D11\Renderer.h>

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

#define D3D11_RELEASE( p_Obj ) { if( p_Obj ) { p_Obj->Release(); p_Obj = NULL; } }

class D3D11Renderer : public DrawSpace::Interface::Renderer
{
protected:


    typedef struct
    {
        dsstring                                                    path;
        DrawSpace::Core::Texture*                                   texture_instance;

        D3D11_TEXTURE2D_DESC                                        descr;

        ID3D11Texture2D*                                            texture;
        ID3D11Texture2D*                                            texture_clone;
        ID3D11RenderTargetView*                                     rendertextureTargetView;
        ID3D11ShaderResourceView*                                   textureShaderResourceView;

        // reserve au textures render target
        ID3D11Texture2D*                                            stencilDepthBuffer;
        ID3D11DepthStencilView*                                     stencilDepthView;

        void*                                                       bits;
        bool                                                        content_access; // si false, on ne peut acceder au contenu de cette texture

    } TextureInfos;

    typedef struct
    {
	    ID3D11Buffer*                                               vertex_buffer;
	    ID3D11Buffer*                                               index_buffer;
        int                                                         nb_vertices;
        int                                                         nb_triangles;

    } MesheData;


    typedef struct
    {
        ID3D11VertexShader*                                         vertex_shader;
        ID3D11PixelShader*                                          pixel_shader;

    } ShadersData;
    

    typedef struct
    {
        XMFLOAT4                                                    vector[512];
        XMMATRIX                                                    matrix[512];
    
    } ShaderLegacyArg;

    // render states
    typedef struct
    {
        D3D11_RASTERIZER_DESC   rs_desc;
        ID3D11RasterizerState*  rs_state;

    } RSCacheEntry;

    // blending states
    typedef struct
    {
        D3D11_BLEND_DESC        bs_desc;
        ID3D11BlendState*       bs_state;

    } BSCacheEntry;

    typedef std::map<dsstring, RSCacheEntry>                        RSCache;
    typedef std::map<dsstring, BSCacheEntry>                        BSCache;

    HWND                                                            m_hwnd;

    Characteristics                                                 m_characteristics;
    int                                                             m_next_nbvertices;
    int                                                             m_next_nbtriangles;

    IDXGISwapChain*                                                 m_lpd3dswapchain;
    ID3D11Device*                                                   m_lpd3ddevice;                     
    ID3D11DeviceContext*                                            m_lpd3ddevcontext;

    ID3D11DepthStencilView*                                         m_currentView;
    ID3D11RenderTargetView*                                         m_currentTarget;

    ID3D11SamplerState*                                             m_linearFilterSamplerState;
    ID3D11SamplerState*                                             m_pointFilterSamplerState;
    ID3D11SamplerState*                                             m_anisotropicFilterSamplerState;

    ID3D11Texture2D*                                                m_pDepthStencil; 
    ID3D11DepthStencilView*                                         m_pDepthStencilView;

    ID3D11DepthStencilState*                                        m_DSState_DepthTestDisabled;
    ID3D11DepthStencilState*                                        m_DSState_DepthTestEnabled;

    ID3D11InputLayout*                                              m_inputLayout;

    ID3D11RenderTargetView*                                         m_screentarget;

    D3D11_RASTERIZER_DESC                                           m_currentRSDesc;
    D3D11_BLEND_DESC                                                m_currentBlendDesc;

    RSCache                                                         m_rsCache;
    BSCache                                                         m_bsCache;

    D3D11_VIEWPORT                                                  m_viewport;

    IFW1FontWrapper*                                                m_fontWrapper;

    std::map<dsstring, TextureInfos*>                               m_textures_base;
    std::map<DrawSpace::Core::Texture*, TextureInfos*>              m_targettextures_base;
    std::map<dsstring, MesheData*>                                  m_meshes_base;
    std::map<dsstring, ShadersData*>                                m_shaders_bases;
    std::map<dsstring, DrawSpace::Core::Fx*>                        m_fx_bases;

    ID3D11Buffer*                                                   m_vertexshader_legacyargs_buffer;
    ID3D11Buffer*                                                   m_pixelshader_legacyargs_buffer;

    ShaderLegacyArg                                                 m_vertexshader_legacyargs;
    ShaderLegacyArg                                                 m_pixelshader_legacyargs;

    CEGUIWrapper                                                    m_guisubsystem;


    void set_vertexshader_constants_vec( DWORD p_startreg, const DrawSpace::Utils::Vector& p_vec );
    void set_pixelshader_constants_vec( DWORD p_startreg, const DrawSpace::Utils::Vector& p_vec );

    void set_vertexshader_constants_mat( DWORD p_startreg, const DrawSpace::Utils::Matrix& p_mat );
    void set_pixelshader_constants_mat( DWORD p_startreg, const DrawSpace::Utils::Matrix& p_mat );


    HRESULT D3D11Renderer::compile_shader_from_file( void* p_data, int p_size, LPCTSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut, ID3DBlob** ppBlobErrOut );

    bool set_cache_rs( void );
    bool set_cache_blendstate( void );

    bool create_depth_stencil_buffer( int p_width, int p_height, DXGI_FORMAT p_format, ID3D11Texture2D** p_texture2D, ID3D11DepthStencilView** p_view );

    void fullscreen_autoset_desktop_resolution( int& p_fullscreen_width, int& p_fullscreen_height, DXGI_FORMAT& p_fullscreen_format, int& p_fullscreen_refreshRate_num, int& p_fullscreen_refreshRate_den );

public:
    D3D11Renderer( void );
    ~D3D11Renderer( void );

    virtual void GetDescr( dsstring& p_descr );
    virtual void GetShadersDescr( dsstring& p_descr );

    virtual void DumpMemoryAllocs( void );

    virtual bool Init( HWND p_hwnd, bool p_fullscreen, long p_w_width, long p_w_height, DrawSpace::Logger::Configuration* p_logconf );
    virtual void Release( void );
    
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

    virtual bool GUI_InitSubSystem( void );
    virtual void GUI_Render( void );
    virtual void GUI_SetResourcesRootDirectory( const dsstring& p_path );

    virtual void GUI_OnMouseMove( float p_xm, float p_ym, float p_dx, float p_dy );
    virtual void GUI_OnMouseLeftButtonDown( void );
    virtual void GUI_OnMouseLeftButtonUp( void );
    virtual void GUI_OnMouseRightButtonDown( void );
    virtual void GUI_OnMouseRightButtonUp( void );
    virtual void GUI_OnKeyDown( long p_key );
    virtual void GUI_OnKeyUp( long p_key );
    virtual void GUI_OnChar( long p_key );
    virtual void GUI_LoadLayout( const dsstring& p_layout_path );
    virtual void GUI_LoadScheme( const dsstring& p_scheme_path );
    virtual void GUI_SetLayout( const dsstring& p_layoutpath );
    virtual void GUI_StoreWidget( int p_parent_id, int p_id );
    virtual void GUI_SetWidgetText( int p_id, const dsstring& p_text );
    virtual void GUI_GetWidgetText( int p_id, dsstring& p_outtext );
    virtual void GUI_SubscribeWidgetPushButtonEventClicked( int p_id );
    virtual void GUI_RegisterPushButtonEventClickedHandler( DrawSpace::Core::BaseCallback<void, dsstring>* p_handler );
    virtual void GUI_SetMouseCursorImage( const dsstring& p_image );
    virtual void GUI_ShowMouseCursor( bool p_show );



    virtual void GUI_InitTest( void ); // temporaire

};

#endif
