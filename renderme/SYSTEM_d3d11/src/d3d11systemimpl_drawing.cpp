/* -*-LIC_BEGIN-*- */
/*
*
* MaGE rendering framework
* Emmanuel Chaumont Copyright (c) 2013-2025
*
* This file is part of MaGE.
*
*    MaGE is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    MaGE is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with MaGE.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#include "d3d11systemimpl.h"
#include "matrixchain.h"


using namespace mage::core::maths;
using namespace mage::transform;

void D3D11SystemImpl::drawLineMeshe(const mage::core::maths::Matrix& p_world, const mage::core::maths::Matrix& p_view, const mage::core::maths::Matrix& p_proj)
{   
    // setting transformation    
    Matrix inv;    
    inv.identity();
    inv(2, 2) = -1.0;
    const auto final_view{ p_view * inv };
    
    MatrixChain chain;
    chain.pushMatrix(p_proj);
    chain.pushMatrix(final_view);
    chain.pushMatrix(p_world);
    chain.buildResult();
    auto result{ chain.getResultTransform() };
    result.transpose();

    setVertexshaderConstantsMat(0, result);
    setPixelshaderConstantsMat(0, result);

    //////////////////////////////////////////////////////////////////////
    
    Matrix worldview{ p_world * p_view };
    worldview.transpose();

    setVertexshaderConstantsMat(4, worldview);
    setPixelshaderConstantsMat(4, worldview);

    //////////////////////////////////////////////////////////////////////

    auto world{ p_world };
    world.transpose();

    auto view{ p_view };
    view.transpose();

    setVertexshaderConstantsMat(8, world);
    setVertexshaderConstantsMat(12, view);

    setPixelshaderConstantsMat(8, world);
    setPixelshaderConstantsMat(12, view);

    //////////////////////////////////////////////////////////////////////

    auto cam{ view };
    cam.inverse();
    cam.transpose();

    setVertexshaderConstantsMat(16, cam);
    setPixelshaderConstantsMat(16, cam);

    auto proj { p_proj };

    proj.transpose();
    setVertexshaderConstantsMat(20, proj);
    setPixelshaderConstantsMat(20, proj);

    // update des shaders legacy constants buffers...

    m_lpd3ddevcontext->UpdateSubresource(m_vertexShaderArgsBuffer, 0, nullptr, &m_vertexshader_args, 0, 0);
    m_lpd3ddevcontext->UpdateSubresource(m_pixelShaderArgsBuffer, 0, nullptr, &m_pixelshader_args, 0, 0);

    ///////////////

    m_lpd3ddevcontext->VSSetConstantBuffers(0, 1, &m_vertexShaderArgsBuffer);
    m_lpd3ddevcontext->PSSetConstantBuffers(0, 1, &m_pixelShaderArgsBuffer);
    m_lpd3ddevcontext->DrawIndexed(m_next_nblines * 2, 0, 0);
}

void D3D11SystemImpl::drawTriangleMeshe(const mage::core::maths::Matrix& p_world, const mage::core::maths::Matrix& p_view, const mage::core::maths::Matrix& p_proj)
{
    // setting transformation    
    Matrix inv;
    inv.identity();
    inv(2, 2) = -1.0;
    const auto final_view{ p_view * inv };

    MatrixChain chain;
    chain.pushMatrix(p_proj);
    chain.pushMatrix(final_view);
    chain.pushMatrix(p_world);
    chain.buildResult();
    auto result{ chain.getResultTransform() };
    result.transpose();

    setVertexshaderConstantsMat(0, result);
    setPixelshaderConstantsMat(100, result);

    //////////////////////////////////////////////////////////////////////

    Matrix worldview{ p_world * p_view };
    worldview.transpose();

    setVertexshaderConstantsMat(4, worldview);
    setPixelshaderConstantsMat(104, worldview);

    //////////////////////////////////////////////////////////////////////

    auto world{ p_world };
    world.transpose();

    auto view{ p_view };
    auto cam{ p_view };

    view.transpose();

    setVertexshaderConstantsMat(8, world);
    setVertexshaderConstantsMat(12, view);

    setPixelshaderConstantsMat(108, world);
    setPixelshaderConstantsMat(112, view);

    //////////////////////////////////////////////////////////////////////

    //auto cam{ view };
    cam.inverse();
    cam.transpose();

    setVertexshaderConstantsMat(16, cam);
    setPixelshaderConstantsMat(116, cam);

    auto proj{ p_proj };

    proj.transpose();
    setVertexshaderConstantsMat(20, proj);
    setPixelshaderConstantsMat(120, proj);

    // update des shaders legacy constants buffers...

    m_lpd3ddevcontext->UpdateSubresource(m_vertexShaderArgsBuffer, 0, nullptr, &m_vertexshader_args, 0, 0);
    m_lpd3ddevcontext->UpdateSubresource(m_pixelShaderArgsBuffer, 0, nullptr, &m_pixelshader_args, 0, 0);

    ///////////////

    m_lpd3ddevcontext->VSSetConstantBuffers(0, 1, &m_vertexShaderArgsBuffer);
    m_lpd3ddevcontext->PSSetConstantBuffers(0, 1, &m_pixelShaderArgsBuffer);

    m_lpd3ddevcontext->DrawIndexed(m_next_nbtriangles * 3, 0, 0);
}

void D3D11SystemImpl::beginScreen()
{
    m_currentTarget = m_screentarget;
    m_currentView = m_pDepthStencilView;

    m_lpd3ddevcontext->OMSetRenderTargets(1, &m_currentTarget, m_currentView);
    m_lpd3ddevcontext->RSSetViewports(1, &m_mainScreenViewport);
}

void D3D11SystemImpl::beginTarget(const std::string& p_targetName)
{
    if (m_textures.count(p_targetName))
    {
        const auto ti{ m_textures.at(p_targetName) };

        m_currentTarget = ti.rendertextureTargetView;
        m_currentView = ti.stencilDepthView;

        m_lpd3ddevcontext->OMSetRenderTargets(1, &m_currentTarget, m_currentView);
        m_lpd3ddevcontext->RSSetViewports(1, &ti.viewport);
    }
    else
    {
        _EXCEPTION_("Target texture not found : " + p_targetName);
    }
}

void D3D11SystemImpl::clearTarget(const mage::core::maths::RGBAColor& p_clear_color)
{
    FLOAT clearcolor[4];

    clearcolor[0] = p_clear_color.r() / 255.0f;
    clearcolor[1] = p_clear_color.g() / 255.0f;
    clearcolor[2] = p_clear_color.b() / 255.0f;
    clearcolor[3] = p_clear_color.a() / 255.0f;

    if (m_currentTarget) 
    {
        m_lpd3ddevcontext->ClearRenderTargetView(m_currentTarget, clearcolor);
    }    
}

void D3D11SystemImpl::clearTargetDepth()
{
    if (m_currentView)
    {
        m_lpd3ddevcontext->ClearDepthStencilView(m_currentView, D3D11_CLEAR_DEPTH, 1.0, 0);
    }    
}

void D3D11SystemImpl::flipScreen(void)
{
    m_lpd3dswapchain->Present(0, 0);
}

void D3D11SystemImpl::drawText(const std::string& p_font, const mage::core::maths::RGBAColor& p_clear_color, const mage::core::maths::IntCoords2D& p_pos, float p_rotation, const std::string& p_text)
{
    const auto fontData{ m_fontWrappers.at(p_font) };

    const auto spriteBatch{ fontData.spriteBatch.get() };
    const auto spriteFont{ fontData.spriteFont.get() };

    const DirectX::XMFLOAT2 pos{ (float)p_pos.x(), (float)p_pos.y() };

    const DirectX::FXMVECTOR color{ p_clear_color.r() / 255.0f, p_clear_color.g() / 255.0f, p_clear_color.b() / 255.0f, p_clear_color.a() / 255.0f };
    
    spriteBatch->Begin();
    spriteFont->DrawString(spriteBatch, p_text.c_str(), pos, color, p_rotation);
    spriteBatch->End();
}