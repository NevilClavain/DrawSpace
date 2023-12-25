
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

void D3D11SystemImpl::translateD3DD11Error(HRESULT p_hRes, std::string& p_str)
{
	static const std::unordered_map<HRESULT, std::string> translate =
	{
		{ S_OK, "S_OK"},
		{ S_FALSE, "S_FALSE"},
		{ E_NOTIMPL, "E_NOTIMPL"},
		{ E_OUTOFMEMORY, "E_OUTOFMEMORY"},
		{ E_INVALIDARG, "E_INVALIDARG"},
		{ E_FAIL, "E_FAIL"},
		//{ D3DERR_WASSTILLDRAWING, "D3DERR_WASSTILLDRAWING"},
		{ DXGI_ERROR_WAS_STILL_DRAWING, "DXGI_ERROR_WAS_STILL_DRAWING"},
		//{ D3DERR_INVALIDCALL, "D3DERR_INVALIDCALL"},
		{ DXGI_ERROR_INVALID_CALL, "DXGI_ERROR_INVALID_CALL"},
		{ D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD, "D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD"},
		{ D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS, "D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS"},
		{ D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS, "D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS"},
		{ D3D11_ERROR_FILE_NOT_FOUND, "D3D11_ERROR_FILE_NOT_FOUND"},
	};
	p_str = translate.at(p_hRes);
}

void D3D11SystemImpl::fullscreenAutosetDesktopResolution(int& p_fullscreen_width, int& p_fullscreen_height, DXGI_FORMAT& p_fullscreen_format, int& p_fullscreen_refreshRate_num, int& p_fullscreen_refreshRate_den)
{
	bool found{ false };
	// get user windows desktop resolution
	RECT desktop_rect;
	GetWindowRect(GetDesktopWindow(), &desktop_rect);

	p_fullscreen_width = desktop_rect.right - desktop_rect.left;
	p_fullscreen_height = desktop_rect.bottom - desktop_rect.top;
	p_fullscreen_format = DXGI_FORMAT_R8G8B8A8_UNORM;
	p_fullscreen_refreshRate_num = 60;
	p_fullscreen_refreshRate_den = 1;
}