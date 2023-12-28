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

bool D3D11SystemImpl::createDepthStencilBuffer(ID3D11Device* p_lpd3ddevice, int p_width, int p_height, DXGI_FORMAT p_format, ID3D11Texture2D** p_texture2D, ID3D11DepthStencilView** p_view)
{
	DECLARE_D3D11ASSERT_VARS

	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = p_width;
	descDepth.Height = p_height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hRes = p_lpd3ddevice->CreateTexture2D(&descDepth, NULL, p_texture2D);

	D3D11_CHECK(CreateTexture2D)

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hRes = p_lpd3ddevice->CreateDepthStencilView(*p_texture2D, &descDSV, p_view);

	D3D11_CHECK(CreateDepthStencilView)

	return true;
}

bool D3D11SystemImpl::setCacheRS(const D3D11_RASTERIZER_DESC& p_currRS)
{
	bool status{ true };
	DECLARE_D3D11ASSERT_VARS

	const auto currRS{ p_currRS };
	MD5 md5;

	const std::string rsdesc_key{ md5.digestMemory((BYTE*)&currRS, sizeof(D3D11_RASTERIZER_DESC)) };

	if (m_rsCache.count(rsdesc_key) > 0)
	{
		m_lpd3ddevcontext->RSSetState(m_rsCache.at(rsdesc_key).rs_state);
	}
	else
	{
		ID3D11RasterizerState* rs{ nullptr };
		hRes = m_lpd3ddevice->CreateRasterizerState(&currRS, &rs);
		D3D11_CHECK(CreateRasterizerState)
		m_lpd3ddevcontext->RSSetState(rs);

		// create new entry in cache
		const RSCacheEntry cache_e { currRS, rs };
		m_rsCache[rsdesc_key] = cache_e; // store in cache
	}
	return status;
}

bool D3D11SystemImpl::setCacheBlendstate(const D3D11_BLEND_DESC& p_currBlendDesc)
{
	bool status{ true };
	DECLARE_D3D11ASSERT_VARS

	const auto currBlendDesc{ p_currBlendDesc };
	MD5 md5;

	FLOAT bvals[4] { 0.0, 0.0, 0.0, 0.0 };

	const std::string bsdesc_key{ md5.digestMemory((BYTE*)&currBlendDesc, sizeof(D3D11_BLEND_DESC)) };

	if (m_bsCache.count(bsdesc_key) > 0)
	{
		m_lpd3ddevcontext->OMSetBlendState(m_bsCache.at(bsdesc_key).bs_state, bvals, 0xffffffff);
	}
	else
	{
		ID3D11BlendState* bs{ nullptr };
		hRes = m_lpd3ddevice->CreateBlendState(&currBlendDesc, &bs);
		D3D11_CHECK(CreateBlendState)
		m_lpd3ddevcontext->OMSetBlendState(bs, bvals, 0xffffffff);

		// create new entry in cache
		const BSCacheEntry cache_e { currBlendDesc, bs };
		m_bsCache[bsdesc_key] = cache_e; // store in cache
	}

	return status;
}

void D3D11SystemImpl::beginScreen()
{
	m_currentTarget = m_screentarget;
	m_currentView = m_pDepthStencilView;

	m_lpd3ddevcontext->OMSetRenderTargets(1, &m_currentTarget, m_currentView);
	m_lpd3ddevcontext->RSSetViewports(1, &m_mainScreenViewport);
}

void D3D11SystemImpl::clearTarget(const renderMe::core::RGBAColor& p_clear_color)
{
	FLOAT clearcolor[4];

	clearcolor[0] = p_clear_color.r() / 255.0f;
	clearcolor[1] = p_clear_color.g() / 255.0f;
	clearcolor[2] = p_clear_color.b() / 255.0;
	clearcolor[3] = p_clear_color.a() / 255.0;

	m_lpd3ddevcontext->ClearRenderTargetView(m_currentTarget, clearcolor);
}

void D3D11SystemImpl::flipScreen(void)
{
	m_lpd3dswapchain->Present(0, 0);
}

void D3D11SystemImpl::drawText(const std::string& p_font, const renderMe::core::RGBAColor& p_clear_color, const renderMe::core::IntCoords2D& p_pos, float p_rotation, const std::string& p_text)
{
	const unsigned long color32{ (
									(((unsigned long)(p_clear_color.a())) << 24) |
									(((unsigned long)(p_clear_color.b()) & 0xff) << 16) | 
									(((unsigned long)(p_clear_color.g()) & 0xff) << 8) |
									((unsigned long)(p_clear_color.r()) & 0xff)
								) };

	const auto fontData{ m_fontWrappers.at(p_font) };

	const auto spriteBatch{ fontData.spriteBatch.get() };
	const auto spriteFont{ fontData.spriteFont.get() };

	const DirectX::XMFLOAT2 pos{ (float)p_pos.x(), (float)p_pos.y() };

	const DirectX::FXMVECTOR color{ p_clear_color.r(), p_clear_color.g(), p_clear_color.b(), p_clear_color.a() };

	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch, p_text.c_str(), pos, color, p_rotation);
	spriteBatch->End();
}