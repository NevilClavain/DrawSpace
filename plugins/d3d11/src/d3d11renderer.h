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

class D3D10Include : public ID3D10Include
{
private:
	const dsstring m_basepath;

public:

	D3D10Include(const dsstring& p_basepath);

	HRESULT __stdcall Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes);
	HRESULT __stdcall Close(LPCVOID pData);
};

class D3D11Renderer : public DrawSpace::Interface::Renderer
{
protected:


	struct TextureInfos
    {
        dsstring                                                    path;
        DrawSpace::Core::Texture*                                   texture_instance;

        D3D11_TEXTURE2D_DESC                                        descr;
        D3D11_TEXTURE3D_DESC                                        descr3D;

        ID3D11Texture2D*                                            texture;
        ID3D11Texture2D*                                            texture_clone;

        ID3D11Texture3D*                                            texture3D;
        ID3D11Texture3D*                                            texture3D_clone;

        std::vector<ID3D11RenderTargetView*>                        rendertextureTargetViews;

        ID3D11ShaderResourceView*                                   textureShaderResourceView;

        // reserve au textures render target
        ID3D11Texture2D*                                            stencilDepthBuffer;
        ID3D11DepthStencilView*                                     stencilDepthView;
        D3D11_VIEWPORT                                              viewport; // viewport adapte au rendu dans cette texture

        void*                                                       bits;
        bool                                                        content_access; // si false, on ne peut acceder au contenu de cette texture

        dsstring                                                    hash; // hash des caracteristique class Texture associee

    };

	struct MesheData
    {
	    ID3D11Buffer*                                               vertex_buffer;
	    ID3D11Buffer*                                               index_buffer;
        int                                                         nb_vertices;
        int                                                         nb_triangles;
    };


	struct ShadersData
    {
        ID3D11VertexShader*                                         vertex_shader;
        ID3D11PixelShader*                                          pixel_shader;
        ID3D11InputLayout*                                          input_layout;
    };

	// compiled shaders bytescodes
	struct ShaderBytesData
	{
		bool														compilation_success;
		ID3DBlob*													blob;
	};

	struct ShaderLegacyArg
    {
        XMFLOAT4                                                    vector[512];
        XMMATRIX                                                    matrix[512];
    };

    // render states
	struct RSCacheEntry
    {
        D3D11_RASTERIZER_DESC   rs_desc;
        ID3D11RasterizerState*  rs_state;

    };

    // blending states
	struct BSCacheEntry
    {
        D3D11_BLEND_DESC        bs_desc;
        ID3D11BlendState*       bs_state;
    };

	static const int NbMaxVectorForShadersBuffers = 512;

    using RSCache = std::map<dsstring, RSCacheEntry>;
    using BSCache = std::map<dsstring, BSCacheEntry>;
    HWND                                                            m_hwnd;

    Characteristics                                                 m_characteristics;
    int                                                             m_next_nbvertices;
    int                                                             m_next_nbtriangles;

    IDXGISwapChain*                                                 m_lpd3dswapchain;
    ID3D11Device*                                                   m_lpd3ddevice;                     
    ID3D11DeviceContext*                                            m_lpd3ddevcontext;

    ID3D11DepthStencilView*                                         m_currentView;
    ID3D11RenderTargetView*                                         m_currentTarget;

    ID3D11SamplerState*                                             m_linearFilterSamplerState { nullptr };
    ID3D11SamplerState*                                             m_pointFilterSamplerState { nullptr };
    ID3D11SamplerState*                                             m_anisotropicFilterSamplerState { nullptr };

    ID3D11SamplerState*                                             m_linearFilterSamplerState_uvwrap { nullptr };
    ID3D11SamplerState*                                             m_pointFilterSamplerState_uvwrap { nullptr };
    ID3D11SamplerState*                                             m_anisotropicFilterSamplerState_uvwrap { nullptr };


    ID3D11Texture2D*                                                m_pDepthStencil; 
    ID3D11DepthStencilView*                                         m_pDepthStencilView;

    ID3D11DepthStencilState*                                        m_DSState_DepthTestDisabled;
    ID3D11DepthStencilState*                                        m_DSState_DepthTestEnabled;

    ID3D11RenderTargetView*                                         m_screentarget;

    D3D11_RASTERIZER_DESC                                           m_currentRSDesc;
    D3D11_BLEND_DESC                                                m_currentBlendDesc;

    RSCache                                                         m_rsCache;
    BSCache                                                         m_bsCache;

    D3D11_VIEWPORT                                                  m_mainScreenViewport;

    IFW1FontWrapper*                                                m_fontWrapper;

    std::map<dsstring, TextureInfos*>                               m_textures_base;
    std::map<dsstring, TextureInfos*>                               m_targettextures_base;

    std::map<dsstring, MesheData*>                                  m_meshes_base;
    std::map<dsstring, ShadersData*>                                m_shaders_base;
	
	std::set<ShaderBytesData*>										m_shadersbytes_base;

    std::map<dsstring, DrawSpace::Core::Fx*>                        m_fx_bases;

    ID3D11Buffer*                                                   m_vertexshader_legacyargs_buffer;
    ID3D11Buffer*                                                   m_pixelshader_legacyargs_buffer;

    ShaderLegacyArg                                                 m_vertexshader_legacyargs;
    ShaderLegacyArg                                                 m_pixelshader_legacyargs;

    CEGUIWrapper                                                    m_guisubsystem;

    dsstring                                                        m_driver_type;

    void set_vertexshader_constants_vec( DWORD p_startreg, const DrawSpace::Utils::Vector& p_vec );
    void set_pixelshader_constants_vec( DWORD p_startreg, const DrawSpace::Utils::Vector& p_vec );

    void set_vertexshader_constants_mat( DWORD p_startreg, const DrawSpace::Utils::Matrix& p_mat );
    void set_pixelshader_constants_mat( DWORD p_startreg, const DrawSpace::Utils::Matrix& p_mat );


    HRESULT D3D11Renderer::compile_shader_from_mem( void* p_data, int p_size, LPCTSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3D10Include* p_include, ID3DBlob** ppBlobOut, ID3DBlob** ppBlobErrOut );

    bool set_cache_rs( void );
    bool set_cache_blendstate( void );

    bool create_depth_stencil_buffer( int p_width, int p_height, DXGI_FORMAT p_format, ID3D11Texture2D** p_texture2D, ID3D11DepthStencilView** p_view );

    void fullscreen_autoset_desktop_resolution( int& p_fullscreen_width, int& p_fullscreen_height, DXGI_FORMAT& p_fullscreen_format, int& p_fullscreen_refreshRate_num, int& p_fullscreen_refreshRate_den );

    bool create2D_rendertarget(DrawSpace::Core::Texture* p_texture, DXGI_FORMAT p_format, TextureInfos* p_texture_infos);
    bool create3D_rendertarget(DrawSpace::Core::Texture* p_texture, DXGI_FORMAT p_format, TextureInfos* p_texture_infos);

public:
    D3D11Renderer( void );
    ~D3D11Renderer( void );

    void GetDescr( dsstring& p_descr );
    void GetShadersDescr( dsstring& p_descr );

    void DumpMemoryAllocs( void );

    bool Init( HWND p_hwnd, bool p_fullscreen, long p_w_width, long p_w_height, DrawSpace::Logger::Configuration* p_logconf );
    void Release( void );
    
    void BeginScreen( void );
    void EndScreen( void );
    void FlipScreen( void );

    void ClearScreen( unsigned char p_r, unsigned char p_g, unsigned char p_b, unsigned char p_a );
    void ClearDepth( dsreal p_value = 1.0 );

    void BeginTarget( void* p_data, int p_slice_index);
    void EndTarget( void* p_data );


    bool CreateMeshe( DrawSpace::Core::Meshe* p_meshe, void** p_data );
    void RemoveMeshe( DrawSpace::Core::Meshe* p_meshe, void* p_data );    
    bool SetMeshe( void* p_data );
    bool UpdateMesheIndexes( DrawSpace::Core::Meshe* p_meshe, void* p_data );
    bool UpdateMesheVertices( DrawSpace::Core::Meshe* p_meshe, void* p_data );

    bool CreateTexture( DrawSpace::Core::Texture* p_texture, void** p_data );
    void DestroyTexture( void* p_data );
    bool SetTexture( void* p_data, int p_stage );
    bool SetVertexTexture( void* p_data, int p_stage );
	bool UnsetTexture( int p_stage );
    bool UnsetVertexTexture( int p_stage );
    bool AllocTextureContent( void* p_texturedata );
    void ReleaseTextureContent( void* p_texturedata );
    void* GetTextureContentPtr( void* p_texturedata );
    bool CopyTextureContent( void* p_texturedata );
    bool UpdateTextureContent( void* p_texturedata );


    bool CreateShaders( DrawSpace::Core::Fx* p_fx, void** p_data );
    bool SetShaders( void* p_data );

	bool CreateShaderBytes(char* p_source, int p_source_length, int p_shadertype, const dsstring& p_path, const dsstring& p_includes_path, void** p_data);
	bool GetShaderCompilationStatus(void* p_data);
	void* GetShaderBytes(void* p_data);
	size_t GetShaderBytesLength(void* p_data);
	dsstring GetShaderCompilationError(void* p_data);
	void ReleaseShaderBytes(void* p_data);

    bool ApplyRenderStatesIn( void* p_data );
    bool ApplyRenderStatesOut( void* p_data );


    bool SetFxShaderParams( int p_shader_index, long p_register, DrawSpace::Utils::Vector& p_vector );
    bool SetFxShaderMatrix( int p_shader_index, long p_register, DrawSpace::Utils::Matrix& p_mat );

	bool SetShaderVectorBuffer(int p_shader_index, long p_register, const std::vector<DrawSpace::Utils::Vector>& p_vectors);

	bool DrawMeshe( DrawSpace::Utils::Matrix p_world, DrawSpace::Utils::Matrix p_view, DrawSpace::Utils::Matrix p_proj );

    void SetRenderState( DrawSpace::Core::RenderState* p_renderstate );

    void GetRenderCharacteristics( Characteristics& p_characteristics );

    void DrawText( long p_r, long p_g, long p_b, int p_posX, int p_posY, const char* p_format, ... );

    void PointProjection( DrawSpace::Utils::Matrix p_view, DrawSpace::Utils::Matrix p_proj, DrawSpace::Utils::Vector& p_point, dsreal& p_outx, dsreal& p_outy, dsreal& p_outz );

    bool GUI_InitSubSystem( void );
    void GUI_ReleaseSubSystem( void );
    void GUI_Render( void );
    void GUI_SetResourcesRootDirectory( const dsstring& p_path );

    void GUI_OnMouseMove( float p_xm, float p_ym, float p_dx, float p_dy );
    void GUI_OnMouseLeftButtonDown( void );
    void GUI_OnMouseLeftButtonUp( void );
    void GUI_OnMouseRightButtonDown( void );
    void GUI_OnMouseRightButtonUp( void );
    void GUI_OnKeyDown( long p_key );
    void GUI_OnKeyUp( long p_key );
    void GUI_OnChar( long p_key );
    void GUI_LoadLayout( const dsstring& p_layout_path, const dsstring& p_widgets_path );
    void GUI_UnloadAllLayouts( void );
    void GUI_LoadScheme( const dsstring& p_scheme_path );
    void GUI_UnloadAllSchemes( void );
    void GUI_SetLayout( const dsstring& p_layoutpath );
    void GUI_StoreWidget( const dsstring& p_layoutName, const dsstring& p_parentName, const dsstring& p_childName );
    void GUI_SetVisibleState( const dsstring& p_layoutName, const dsstring& p_widgetName, bool p_state );
    bool GUI_IsVisible( const dsstring& p_layoutName, const dsstring& p_widgetName );
    void GUI_SetWidgetText( const dsstring& p_layoutName, const dsstring& p_widgetName, const dsstring& p_text );
    void GUI_GetWidgetText( const dsstring& p_layoutName, const dsstring& p_widgetName, dsstring& p_outtext );
    void GUI_AddListboxTextItem( const dsstring& p_layoutName, const dsstring& p_widgetName, const dsstring& p_text, unsigned int p_colors, const dsstring& p_brushImage );
    void GUI_ClearListbox( const dsstring& p_layoutName, const dsstring& p_widgetName );
    bool GUI_GetListboxFirstSelectedItemIndex( const dsstring& p_layoutName, const dsstring& p_widgetName, int& p_index, dsstring& p_text );
    void GUI_AddComboboxTextItem( const dsstring& p_layoutName, const dsstring& p_widgetName, const dsstring& p_text, unsigned int p_colors, const dsstring& p_brushImage );
    void GUI_ClearCombobox( const dsstring& p_layoutName, const dsstring& p_widgetName );
    void GUI_SetComboBoxItemSelectionState( const dsstring& p_layoutName, const dsstring& p_widgetName, int p_index, bool p_state );
    int  GUI_GetComboBoxSelectionIndex( const dsstring& p_layoutName, const dsstring& p_widgetName );
    bool GUI_IsCheckBoxChecked( const dsstring& p_layoutName, const dsstring& p_widgetName );
    void GUI_SetCheckboxState( const dsstring& p_layoutName, const dsstring& p_widgetName, bool p_state );
    void GUI_SetMultiLineEditboxCaretIndex(const dsstring& p_layoutName, const dsstring& p_widgetName, int p_index);

    void GUI_RegisterPushButtonEventClickedHandler( DrawSpace::Core::BaseCallback2<void, const dsstring&, const dsstring&>* p_handler );
    void GUI_RegisterCheckboxEventStateChangedHandler( DrawSpace::Core::BaseCallback3<void, const dsstring&, const dsstring&, bool>* p_handler );
    void GUI_SetMouseCursorImage( const dsstring& p_image );
    void GUI_ShowMouseCursor( bool p_show );

    void GUI_CreateSprite( const dsstring& p_scheme_object, const dsstring& p_spriteName );
    void GUI_SetSpritePosition( const dsstring& p_spriteName, dsreal p_xpos, dsreal p_ypos );
    void GUI_SetSpriteImage( const dsstring& p_spriteName, const dsstring& p_image );
    void GUI_SetSpriteScale( const dsstring& p_spriteName, dsreal p_scale );
    void GUI_SetSpriteRotation( const dsstring& p_spriteName, const DrawSpace::Utils::Vector& p_axis, dsreal p_deg_angle );

    void GUI_InitTest( void ); // temporaire

};

#endif
