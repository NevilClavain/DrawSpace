/* -*-LIC_BEGIN-*- */
/*
*
* renderMe grafx framework
* Emmanuel Chaumont Copyright (c) 2013-2023
*
* This file is part of renderMe.
*
*    renderMe is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    renderMe is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with renderMe.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#pragma once

#pragma warning( disable : 4005 4838 26812 4996 )

// IMPORTANT : this .h is supposed to be included in d3dsystemimpl.cpp only, so no need of any forward declaration here

#include <d3d11.h>
#include <directxmath.h>
#include <dxgiformat.h>

#include <SpriteFont.h>
#include <SpriteBatch.h>



#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "singleton.h"
#include "entity.h"

#include "logsink.h"
#include "logconf.h"
#include "logging.h"

#include "tvector.h"

#include "filesystem.h"
#include "buffer.h"

#include "linemeshe.h"
#include "trianglemeshe.h"
#include "texture.h"
#include "renderstate.h"

#include "tvector.h"
#include "matrix.h"

static constexpr int nbTextureStages = renderMe::nbUVCoordsPerVertex;

#define DECLARE_D3D11ASSERT_VARS HRESULT hRes; \
                                 std::string d3dErrStr;

#define D3D11_CHECK( p_mName ) \
    if( hRes != S_OK ) \
    { \
        D3D11SystemImpl::translateD3DD11Error( hRes, d3dErrStr ); \
        std::string dstr = " "#p_mName" -> "; \
        dstr += d3dErrStr; \
        dstr += "\n"; \
        _RENDERME_ERROR( m_localLogger, dstr.c_str() ); \
        return false; \
    }

struct d3d11triangle
{
    unsigned long vertex1;
    unsigned long vertex2;
    unsigned long vertex3;
};

struct d3d11line
{
    unsigned long vertex1;
    unsigned long vertex2;
};

struct d3d11vertex
{
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT3 normale;
    DirectX::XMFLOAT4 t[nbTextureStages];
    DirectX::XMFLOAT3 tangent;
    DirectX::XMFLOAT3 binormale;
};

struct D3D10Include : public ID3D10Include
{
public:

    D3D10Include(const std::string& p_basepath, renderMe::core::logger::Sink& p_logger);
    ~D3D10Include() = default;

    HRESULT __stdcall Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes);
    HRESULT __stdcall Close(LPCVOID pData);

private:
    renderMe::core::FileContent<const char>*    m_fc{ nullptr };
    const std::string                           m_basepath;
    renderMe::core::logger::Sink&               m_logger;
};

class D3D11SystemImpl : public renderMe::property::Singleton<D3D11SystemImpl>
{
public:
    D3D11SystemImpl();
    ~D3D11SystemImpl() = default;

    renderMe::core::logger::Sink& logger();

    bool init(renderMe::core::Entity* p_mainWindow);

    void beginScreen();
    void clearTarget(const renderMe::core::maths::RGBAColor& p_clear_color);
    void flipScreen(void);

    void drawText(const std::string& p_font, const renderMe::core::maths::RGBAColor& p_clear_color, const renderMe::core::maths::IntCoords2D& p_pos, float p_rotation, const std::string& p_text);

    bool createShaderBytesOnFile(int p_shadertype,
                                    const std::string& p_includes_path,
                                    const renderMe::core::FileContent<const char>& srcFile,
                                    std::unique_ptr<char[]>& p_shaderBytes,
                                    size_t& p_shaderBytesLength);

    bool createVertexShader(const std::string& p_name, const renderMe::core::Buffer<char>& p_code);
    bool createPixelShader(const std::string& p_name, const renderMe::core::Buffer<char>& p_code);
          
    void setVertexShader(const std::string& p_name);
    void setPixelShader(const std::string& p_name);

    void forceCurrentVertexShader();
    void forceCurrentPixelShader();

    void destroyVertexShader(const std::string& p_name);
    void destroyPixelShader(const std::string& p_name);

    bool createLineMeshe(const renderMe::LineMeshe& p_lm);
    void setLineMeshe(const std::string& p_name);
    void destroyLineMeshe(const std::string& p_name);

    bool createTriangleMeshe(const renderMe::TriangleMeshe& p_tm);
    void setTriangleMeshe(const std::string& p_name);
    void destroyTriangleMeshe(const std::string& p_name);

    void forceCurrentMeshe();

    bool createTexture(renderMe::Texture& p_texture);

    void bindTextureStage(const std::string& p_name, size_t p_stage);
    void unbindTextureStage(size_t p_stage);

    void destroyTexture(const std::string& p_name);

    void prepareRenderState(const renderMe::rendering::RenderState& p_renderstate); // update struct
    bool setCacheRS(bool p_force = false); // apply

    void prepareBlendState(const renderMe::rendering::RenderState& p_renderstate); // update struct    
    bool setCacheBlendstate(bool p_force = false); // apply

    void setDepthStenciState(const renderMe::rendering::RenderState& p_renderstate);
    void forceCurrentDepthStenciState();    

    void setPSSamplers(const renderMe::rendering::RenderState& p_renderstate);
    void setVSSamplers(const renderMe::rendering::RenderState& p_renderstate);

    void forceCurrentPSSamplers();
    void forceCurrentVSSamplers();

    void forceTexturesBinding();

    void setTriangleListTopology();
    void setLineListTopology();

    void forceCurrentTopology();

    void drawLineMeshe(const renderMe::core::maths::Matrix& p_world, const renderMe::core::maths::Matrix& p_view, const renderMe::core::maths::Matrix& p_proj);
    void drawTriangleMeshe(const renderMe::core::maths::Matrix& p_world, const renderMe::core::maths::Matrix& p_view, const renderMe::core::maths::Matrix& p_proj);

    void setVertexshaderConstantsVec(int p_startreg, const renderMe::core::maths::Real4Vector& p_vec);
    void setPixelshaderConstantsVec(int p_startreg, const renderMe::core::maths::Real4Vector& p_vec);
    void setVertexshaderConstantsMat(int p_startreg, const renderMe::core::maths::Matrix& p_mat);
    void setPixelshaderConstantsMat(int p_startreg, const renderMe::core::maths::Matrix& p_mat);


    std::unordered_set<std::string> getShadersNames() const;

private:

    struct FontRenderingData
    {
        std::shared_ptr<DirectX::SpriteBatch> spriteBatch;
        std::shared_ptr<DirectX::SpriteFont>  spriteFont;
    };

    // render states
    struct RSCacheEntry
    {
        D3D11_RASTERIZER_DESC       rs_desc  { 0 };
        ID3D11RasterizerState*      rs_state { nullptr };
    };

    // blending states
    struct BSCacheEntry
    {
        D3D11_BLEND_DESC            bs_desc  { 0 };
        ID3D11BlendState*           bs_state { nullptr };
    };

    struct ShaderArg
    {
        DirectX::XMFLOAT4           vector[512];
        DirectX::XMFLOAT4X4         matrix[512];
    };

    struct VertexShadersData
    {
        ID3D11VertexShader*         vertex_shader   { nullptr };
        ID3D11InputLayout*          input_layout     { nullptr };
    };

    struct PixelShadersData
    {
        ID3D11PixelShader* pixel_shader     { nullptr };
    };

    struct MesheData
    {
        ID3D11Buffer* vertex_buffer         { nullptr };
        ID3D11Buffer* index_buffer          { nullptr };
        size_t        nb_vertices           { 0 };
        size_t        nb_primitives         { 0 };
    };

    struct TextureData
    {
        renderMe::Texture::Source   source;
        D3D11_TEXTURE2D_DESC        desc;

        // common
        ID3D11ShaderResourceView*   shaderResourceView      { nullptr };

        // specifique textures from files
        ID3D11Resource*             textureResource         { nullptr };

        // specifique textures render target

        ID3D11Texture2D*            targetTexture           { nullptr };
        ID3D11RenderTargetView*     rendertextureTargetView { nullptr };
        ID3D11Texture2D*            stencilDepthBuffer      { nullptr };
        ID3D11DepthStencilView*     stencilDepthView        { nullptr };
        D3D11_VIEWPORT              viewport; // viewport adapte au rendu dans cette texture

    };

    using RSCache =                 std::unordered_map<std::string, RSCacheEntry>;
    using BSCache =                 std::unordered_map<std::string, BSCacheEntry>;

    using VShaderList =             std::unordered_map<std::string, VertexShadersData>;
    using PShaderList =             std::unordered_map<std::string, PixelShadersData>;

    using MesheList =               std::unordered_map<std::string, MesheData>;
    using TextureList =             std::unordered_map<std::string, TextureData>;


    renderMe::core::logger::Sink                        m_localLogger;

    size_t                                              m_next_nbvertices{ 0 };
    size_t                                              m_next_nbtriangles{ 0 };
    size_t                                              m_next_nblines{ 0 };

    IDXGISwapChain*                                     m_lpd3dswapchain{ nullptr };
    ID3D11Device*                                       m_lpd3ddevice{ nullptr };
    ID3D11DeviceContext*                                m_lpd3ddevcontext{ nullptr };

    IDXGIAdapter*                                       m_dxgiAdapter{ nullptr };
    DXGI_ADAPTER_DESC                                   m_adapterDescription;

    ID3D11RenderTargetView*                             m_screentarget{ nullptr };
    ID3D11DepthStencilState*                            m_dsState_DepthTestDisabled{ nullptr };
    ID3D11DepthStencilState*                            m_dsState_DepthTestEnabled{ nullptr };

    ID3D11Texture2D*                                    m_pDepthStencil{ nullptr };
    ID3D11DepthStencilView*                             m_pDepthStencilView{ nullptr };

    D3D11_RASTERIZER_DESC                               m_currentRSDesc;
    D3D11_BLEND_DESC                                    m_currentBlendDesc;

    std::unordered_map<std::string, FontRenderingData>  m_fontWrappers;

    ID3D11SamplerState*                                 m_linearFilterSamplerState{ nullptr };
    ID3D11SamplerState*                                 m_pointFilterSamplerState{ nullptr };
    ID3D11SamplerState*                                 m_anisotropicFilterSamplerState{ nullptr };

    ID3D11SamplerState*                                 m_linearFilterSamplerState_uvwrap{ nullptr };
    ID3D11SamplerState*                                 m_pointFilterSamplerState_uvwrap{ nullptr };
    ID3D11SamplerState*                                 m_anisotropicFilterSamplerState_uvwrap{ nullptr };

    RSCache                                             m_rsCache;
    BSCache                                             m_bsCache;

    ID3D11Buffer*                                       m_vertexShaderArgsBuffer{ nullptr };
    ID3D11Buffer*                                       m_pixelShaderArgsBuffer{ nullptr };

    ShaderArg                                           m_vertexshader_args;
    ShaderArg                                           m_pixelshader_args;


    D3D11_VIEWPORT                                      m_mainScreenViewport;

    ID3D11DepthStencilView*                             m_currentView{ nullptr };
    ID3D11RenderTargetView*                             m_currentTarget{ nullptr };

    VShaderList                                         m_vshaders;
    PShaderList                                         m_pshaders;

    std::unordered_set<std::string>                     m_shaderNames;

    MesheList                                           m_lines;
    MesheList                                           m_triangles;

    TextureList                                         m_textures;


    ////////////////////////////////////////////////////////

    std::string                                         m_currentRenderStateMD5;
    std::string                                         m_currentBlendStateMD5;
    std::string                                         m_currentDepthStencilState;

    std::string                                         m_currentPSSampler;

    bool                                                m_PSExtendedSamplers;
    std::string                                         m_currentPSExtendedSamplers[nbTextureStages];
    int                                                 m_currentPSExtendedSamplersLength;

    std::string                                         m_currentVSSampler;

    bool                                                m_VSExtendedSamplers;
    std::string                                         m_currentVSExtendedSamplers[nbTextureStages];
    int                                                 m_currentVSExtendedSamplersLength;

    // current texture name for each stage
    std::string                                         m_currentTextures[nbTextureStages];

    // current primitive topology
    D3D11_PRIMITIVE_TOPOLOGY                            m_currentPrimitiveTopology{ D3D_PRIMITIVE_TOPOLOGY_UNDEFINED };

    // current shaders
    std::string                                         m_currentVs;
    std::string                                         m_currentPs;

    // current meshe (line or triangle)
    std::string                                         m_currentMeshe;


    ////////////////////////////////////////////////////////


    bool createDepthStencilBuffer(ID3D11Device* p_lpd3ddevice, int p_width, int p_height, DXGI_FORMAT p_format, ID3D11Texture2D** p_texture2D, ID3D11DepthStencilView** p_view);

    HRESULT compileShaderFromMem(void* p_data, int p_size, LPCTSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3D10Include* p_include, ID3DBlob** ppBlobOut, ID3DBlob** ppBlobErrOut);

    ///////////////////////////////////////////////////////////////////////////////

    static void translateD3DD11Error(HRESULT p_hRes, std::string& p_str);    
    static void fullscreenAutosetDesktopResolution(int& p_fullscreen_width, int& p_fullscreen_height, DXGI_FORMAT& p_fullscreen_format, 
                                                        int& p_fullscreen_refreshRate_num, int& p_fullscreen_refreshRate_den);
};

