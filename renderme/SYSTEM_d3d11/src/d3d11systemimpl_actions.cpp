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

void D3D11SystemImpl::setTriangleListTopology()
{
	if (D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST != m_currentPrimitiveTopology)
	{
		m_lpd3ddevcontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_currentPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}	
}

void D3D11SystemImpl::setLineListTopology()
{
	if (D3D11_PRIMITIVE_TOPOLOGY_LINELIST != m_currentPrimitiveTopology)
	{
		m_lpd3ddevcontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		m_currentPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	}
}

void D3D11SystemImpl::forceCurrentTopology()
{
	m_lpd3ddevcontext->IASetPrimitiveTopology(m_currentPrimitiveTopology);
}