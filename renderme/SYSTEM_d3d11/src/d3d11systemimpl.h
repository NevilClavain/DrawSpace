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
#include <d3dx11.h>
#include <xnamath.h>
#include <dxgiformat.h>
#include <FW1FontWrapper.h>

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "singleton.h"
#include "entity.h"

#include "logsink.h"
#include "logconf.h"
#include "logging.h"

#include "tvector.h"

#include "filesystem.h"

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
    void clearTarget(const renderMe::core::RGBAColor& p_clear_color);
    void flipScreen(void);

    void drawText(const std::string& p_font, const renderMe::core::RGBAColor& p_clear_color, const renderMe::core::IntCoords2D& p_pos, float p_fontsize, const std::string& p_text);

    bool createShaderBytesOnFile(int p_shadertype,
                                    const std::string& p_includes_path,
                                    const renderMe::core::FileContent<const char>& srcFile,
                                    std::unique_ptr<char[]>& p_shaderBytes,
                                    size_t& p_shaderBytesLength);
                            

private:

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

    struct ShaderLegacyArg
    {
        XMFLOAT4                    vector[512];
        XMMATRIX                    matrix[512];
    };

    using RSCache =                 std::unordered_map<std::string, RSCacheEntry>;
    using BSCache =                 std::unordered_map<std::string, BSCacheEntry>;

    renderMe::core::logger::Sink                        m_localLogger;

    IDXGISwapChain*                                     m_lpd3dswapchain{ nullptr };
    ID3D11Device*                                       m_lpd3ddevice{ nullptr };
    ID3D11DeviceContext*                                m_lpd3ddevcontext{ nullptr };
    ID3D11RenderTargetView*                             m_screentarget{ nullptr };
    ID3D11DepthStencilState*                            m_dsState_DepthTestDisabled{ nullptr };
    ID3D11DepthStencilState*                            m_dsState_DepthTestEnabled{ nullptr };

    ID3D11Texture2D*                                    m_pDepthStencil{ nullptr };
    ID3D11DepthStencilView*                             m_pDepthStencilView{ nullptr };

    std::unordered_map<std::string, IFW1FontWrapper*>   m_fontWrappers;

    ID3D11SamplerState*                                 m_linearFilterSamplerState{ nullptr };
    ID3D11SamplerState*                                 m_pointFilterSamplerState{ nullptr };
    ID3D11SamplerState*                                 m_anisotropicFilterSamplerState{ nullptr };

    ID3D11SamplerState*                                 m_linearFilterSamplerState_uvwrap{ nullptr };
    ID3D11SamplerState*                                 m_pointFilterSamplerState_uvwrap{ nullptr };
    ID3D11SamplerState*                                 m_anisotropicFilterSamplerState_uvwrap{ nullptr };

    RSCache                                             m_rsCache;
    BSCache                                             m_bsCache;

    ID3D11Buffer*                                       m_vertexShaderLegacyargsBuffer{ nullptr };
    ID3D11Buffer*                                       m_pixelShaderLegacyargsBuffer{ nullptr };

    D3D11_VIEWPORT                                      m_mainScreenViewport;

    ID3D11DepthStencilView*                             m_currentView{ nullptr };
    ID3D11RenderTargetView*                             m_currentTarget{ nullptr };



    bool createDepthStencilBuffer(ID3D11Device* p_lpd3ddevice, int p_width, int p_height, DXGI_FORMAT p_format, ID3D11Texture2D** p_texture2D, ID3D11DepthStencilView** p_view);

    bool setCacheRS(const D3D11_RASTERIZER_DESC& p_currRS);
    bool setCacheBlendstate(const D3D11_BLEND_DESC& p_currBlendDesc);

    HRESULT compileShaderFromMem(void* p_data, int p_size, LPCTSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3D10Include* p_include, ID3DBlob** ppBlobOut, ID3DBlob** ppBlobErrOut);

    ///////////////////////////////////////////////////////////////////////////////

    static void translateD3DD11Error(HRESULT p_hRes, std::string& p_str);    
    static void fullscreenAutosetDesktopResolution(int& p_fullscreen_width, int& p_fullscreen_height, DXGI_FORMAT& p_fullscreen_format, 
                                                        int& p_fullscreen_refreshRate_num, int& p_fullscreen_refreshRate_den);
};

