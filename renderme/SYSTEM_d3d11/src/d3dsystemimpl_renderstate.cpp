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

#include "d3d11systemimpl.h"
#include "md5.h"

void D3D11SystemImpl::setDepthStenciState(const renderMe::rendering::RenderState& p_renderstate)
{
    const auto arg{ p_renderstate.getArg() };
    if (renderMe::rendering::RenderState::Operation::ENABLEZBUFFER == p_renderstate.getOperation())
    {
        if (m_currentDepthStencilState != arg)
        {
            if ("true" == arg)
            {
                m_lpd3ddevcontext->OMSetDepthStencilState(m_dsState_DepthTestEnabled, 1);
            }
            else
            {
                m_lpd3ddevcontext->OMSetDepthStencilState(m_dsState_DepthTestDisabled, 1);
            }
            m_currentDepthStencilState = arg;
        }
    }
}

void D3D11SystemImpl::setPSSamplers(const renderMe::rendering::RenderState& p_renderstate)
{
    static const std::map<std::string, ID3D11SamplerState*> translate_samplerstate =
    {
        { "none",               m_pointFilterSamplerState              },
        { "point",              m_pointFilterSamplerState              },
        { "linear",             m_linearFilterSamplerState             },
        { "anisotropic",        m_anisotropicFilterSamplerState        },
        { "point_uvwrap",       m_pointFilterSamplerState_uvwrap       },
        { "linear_uvwrap",      m_linearFilterSamplerState_uvwrap      },
        { "anisotropic_uvwrap", m_anisotropicFilterSamplerState_uvwrap }
    };

    const auto arg{ p_renderstate.getArg() };
    if (renderMe::rendering::RenderState::Operation::SETTEXTUREFILTERTYPE == p_renderstate.getOperation())
    {
        if ("extended" == arg)
        {
            ID3D11SamplerState* ss_array[] = { nullptr };

            const auto args_list{ p_renderstate.getExtendedArgs() };
            auto index{ 0 };
            for (const auto& e : args_list)
            {
                if (m_currentPSExtendedSamplers[index] != e)
                {
                    ss_array[0] = translate_samplerstate.at(e);
                    m_lpd3ddevcontext->PSSetSamplers(index++, 1, ss_array);

                    m_currentPSExtendedSamplers[index] = e;
                }
            }
        }
        else
        {
            if (m_currentPSSampler != arg)
            {
                ID3D11SamplerState* ss_array[] = { translate_samplerstate.at(arg) };
                for (auto i = 0; i < nbTextureStages; i++)
                {
                    m_lpd3ddevcontext->PSSetSamplers(i, 1, ss_array);
                }
                m_currentPSSampler = arg;
            }
        }
    }
}

void D3D11SystemImpl::setVSSamplers(const renderMe::rendering::RenderState& p_renderstate)
{
    static const std::map<std::string, ID3D11SamplerState*> translate_samplerstate =
    {
        { "none",               m_pointFilterSamplerState              },
        { "point",              m_pointFilterSamplerState              },
        { "linear",             m_linearFilterSamplerState             },
        { "anisotropic",        m_anisotropicFilterSamplerState        },
        { "point_uvwrap",       m_pointFilterSamplerState_uvwrap       },
        { "linear_uvwrap",      m_linearFilterSamplerState_uvwrap      },
        { "anisotropic_uvwrap", m_anisotropicFilterSamplerState_uvwrap }
    };

    const auto arg{ p_renderstate.getArg() };
    if (renderMe::rendering::RenderState::Operation::SETVERTEXTEXTUREFILTERTYPE == p_renderstate.getOperation())
    {
        if ("extended" == arg)
        {
            ID3D11SamplerState* ss_array[] = { nullptr };

            const auto args_list{ p_renderstate.getExtendedArgs() };
            auto index{ 0 };
            for (const auto& e : args_list)
            {
                if (m_currentVSExtendedSamplers[index] != e)
                {
                    ss_array[0] = translate_samplerstate.at(e);
                    m_lpd3ddevcontext->VSSetSamplers(index++, 1, ss_array);

                    m_currentVSExtendedSamplers[index] = e;
                }
            }
        }
        else
        {
            if (m_currentPSSampler != arg)
            {
                ID3D11SamplerState* ss_array[] = { translate_samplerstate.at(arg) };
                for (auto i = 0; i < nbTextureStages; i++)
                {
                    m_lpd3ddevcontext->VSSetSamplers(i, 1, ss_array);
                }
                m_currentVSSampler = arg;
            }
        }
    }
}

void D3D11SystemImpl::prepareBlendState(const renderMe::rendering::RenderState& p_renderstate)
{
    const auto arg{ p_renderstate.getArg() };

    switch (p_renderstate.getOperation())
    {
        case renderMe::rendering::RenderState::Operation::ALPHABLENDENABLE:
        {
            if ("true" == arg)
            {
                m_currentBlendDesc.RenderTarget[0].BlendEnable = TRUE;
            }
            else if ("false" == arg)
            {
                m_currentBlendDesc.RenderTarget[0].BlendEnable = FALSE;
            }
        }
        break;

        case renderMe::rendering::RenderState::Operation::ALPHABLENDOP:
        {
            if ("add" == arg)
            {
                m_currentBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
                m_currentBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            }
            else if ("sub" == arg)
            {
                m_currentBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_SUBTRACT;
                m_currentBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_SUBTRACT;
            }
            else if ("revsub" == arg)
            {
                m_currentBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
                m_currentBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_REV_SUBTRACT;
            }
            else if ("min" == arg)
            {
                m_currentBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MIN;
                m_currentBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MIN;
            }
            else if ("max" == arg)
            {
                m_currentBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MAX;
                m_currentBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
            }
        }
        break;

        case renderMe::rendering::RenderState::Operation::ALPHABLENDFUNC:
        {
            if (arg != "always")
            {
                _EXCEPTION("unsupported alpha blending func for D3D11")
            }
        }
        break;

        case renderMe::rendering::RenderState::Operation::ALPHABLENDDEST:
        {
            if ("zero" == arg)
            {
                m_currentBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
                m_currentBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
            }
            else if ("one" == arg)
            {
                m_currentBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
                m_currentBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
            }
            else if ("srccolor" == arg)
            {
                m_currentBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
                m_currentBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_SRC_COLOR;
            }
            else if ("invsrccolor" == arg)
            {
                m_currentBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;
                m_currentBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_COLOR;
            }
            else if ("srcalpha" == arg)
            {
                m_currentBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_ALPHA;
                m_currentBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_SRC_ALPHA;
            }
            else if ("invsrcalpha" == arg)
            {
                m_currentBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
                m_currentBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
            }
            else if ("destalpha" == arg)
            {
                m_currentBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_DEST_ALPHA;
                m_currentBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
            }
            else if ("invdestalpha" == arg)
            {
                m_currentBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_DEST_ALPHA;
                m_currentBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
            }
            else if ("destcolor" == arg)
            {
                m_currentBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_DEST_COLOR;
                m_currentBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_COLOR;
            }
            else if ("invdestcolor" == arg)
            {
                m_currentBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_DEST_COLOR;
                m_currentBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_DEST_COLOR;
            }
        }
        break;

        case renderMe::rendering::RenderState::Operation::ALPHABLENDSRC:
        {
            if ("zero" == arg)
            {
                m_currentBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
                m_currentBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
            }
            else if ("one" == arg)
            {
                m_currentBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
                m_currentBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
            }
            else if ("srccolor" == arg)
            {
                m_currentBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;
                m_currentBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_COLOR;
            }
            else if ("invsrccolor" == arg)
            {
                m_currentBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_INV_SRC_COLOR;
                m_currentBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_COLOR;
            }
            else if ("srcalpha" == arg)
            {
                m_currentBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
                m_currentBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
            }
            else if ("invsrcalpha" == arg)
            {
                m_currentBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_INV_SRC_ALPHA;
                m_currentBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
            }
            else if ("destalpha" == arg)
            {
                m_currentBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_DEST_ALPHA;
                m_currentBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;
            }
            else if ("invdestalpha" == arg)
            {
                m_currentBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_INV_DEST_ALPHA;
                m_currentBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
            }
            else if ("destcolor" == arg)
            {
                m_currentBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_DEST_COLOR;
                m_currentBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_DEST_COLOR;
            }
            else if ("invdestcolor" == arg)
            {
                m_currentBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_INV_DEST_COLOR;
                m_currentBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_COLOR;
            }
        }
        break;
    }
}

void D3D11SystemImpl::prepareRenderState(const renderMe::rendering::RenderState& p_renderstate)
{
    const auto arg{ p_renderstate.getArg() };
   
    switch (p_renderstate.getOperation())
    {
        case renderMe::rendering::RenderState::Operation::SETCULLING:
        {           
            if ("none" == arg)
            {
                m_currentRSDesc.CullMode = D3D11_CULL_NONE;
                m_currentRSDesc.FrontCounterClockwise = FALSE;
            }
            else if ("cw" == arg)
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
        }
        break;

        case renderMe::rendering::RenderState::Operation::SETFILLMODE:
        {
            if ("line" == arg)
            {
                m_currentRSDesc.FillMode = D3D11_FILL_WIREFRAME;
            }
            else if ("solid" == arg)
            {
                m_currentRSDesc.FillMode = D3D11_FILL_SOLID;
            }
        }
        break;
    }
}


bool D3D11SystemImpl::setCacheRS()
{
    bool status{ true };
    DECLARE_D3D11ASSERT_VARS

    const auto currRS{ m_currentRSDesc };
    MD5 md5;

    const std::string rsdesc_key{ md5.digestMemory((BYTE*)&currRS, sizeof(D3D11_RASTERIZER_DESC)) };

    if (m_rsCache.count(rsdesc_key) > 0)
    {
        if (rsdesc_key != m_currentRenderStateMD5)
        {
            // not already applied
            m_lpd3ddevcontext->RSSetState(m_rsCache.at(rsdesc_key).rs_state);
            m_currentRenderStateMD5 = rsdesc_key;
        }
    }
    else
    {
        ID3D11RasterizerState* rs{ nullptr };
        hRes = m_lpd3ddevice->CreateRasterizerState(&currRS, &rs);
        D3D11_CHECK(CreateRasterizerState)
        m_lpd3ddevcontext->RSSetState(rs);

        // create new entry in cache
        const RSCacheEntry cache_e{ currRS, rs };
        m_rsCache[rsdesc_key] = cache_e; // store in cache

        m_currentRenderStateMD5 = rsdesc_key;
    }
    return status;
}

bool D3D11SystemImpl::setCacheBlendstate()
{
    bool status{ true };
    DECLARE_D3D11ASSERT_VARS

    const auto currBlendDesc{ m_currentBlendDesc };
    MD5 md5;

    FLOAT bvals[4]{ 0.0, 0.0, 0.0, 0.0 };

    const std::string bsdesc_key{ md5.digestMemory((BYTE*)&currBlendDesc, sizeof(D3D11_BLEND_DESC)) };

    if (m_bsCache.count(bsdesc_key) > 0)
    {
        if (bsdesc_key != m_currentBlendStateMD5)
        {
            // not already applied
            m_lpd3ddevcontext->OMSetBlendState(m_bsCache.at(bsdesc_key).bs_state, bvals, 0xffffffff);
            m_currentBlendStateMD5 = bsdesc_key;
        }
    }
    else
    {
        ID3D11BlendState* bs{ nullptr };
        hRes = m_lpd3ddevice->CreateBlendState(&currBlendDesc, &bs);
        D3D11_CHECK(CreateBlendState)
            m_lpd3ddevcontext->OMSetBlendState(bs, bvals, 0xffffffff);

        // create new entry in cache
        const BSCacheEntry cache_e{ currBlendDesc, bs };
        m_bsCache[bsdesc_key] = cache_e; // store in cache

        m_currentBlendStateMD5 = bsdesc_key;
    }
    return status;
}