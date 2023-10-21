
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

#pragma warning( disable : 4005 4838 )

#include <d3d11.h>
#include <d3dx11.h>
#include <xnamath.h>
#include <dxgiformat.h>

#include "init.h"

#include "logsink.h"
#include "logconf.h"
#include "logging.h"

#include "aspects.h"
#include "entity.h"


using namespace renderMe::core;

static renderMe::core::logger::Sink localLogger("D3D11Init", renderMe::core::logger::Configuration::getInstance());

static void fullscreen_autoset_desktop_resolution(int& p_fullscreen_width, int& p_fullscreen_height, DXGI_FORMAT& p_fullscreen_format, int& p_fullscreen_refreshRate_num, int& p_fullscreen_refreshRate_den)
{
	bool found = false;
	// get user windows desktop resolution
	RECT desktop_rect;
	GetWindowRect(GetDesktopWindow(), &desktop_rect);

	p_fullscreen_width = desktop_rect.right - desktop_rect.left;
	p_fullscreen_height = desktop_rect.bottom - desktop_rect.top;
	p_fullscreen_format = DXGI_FORMAT_R8G8B8A8_UNORM;
	p_fullscreen_refreshRate_num = 60;
	p_fullscreen_refreshRate_den = 1;

}

void renderMe::system::d3dInit(Entity* p_mainWindow)
{
	_RENDERME_DEBUG(localLogger, std::string("initD3D"))

		IDXGIFactory* factory {
		nullptr
	};
	CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);

	DXGI_SWAP_CHAIN_DESC swap_chain;
	ZeroMemory(&swap_chain, sizeof(swap_chain));

	//get main windows infos
	auto& rendering_aspect{ p_mainWindow->aspectAccess(renderingAspect::id) };

	int characteristics_width_resol{ 0 };
	int characteristics_height_resol{ 0 };
	float characteristics_v_width, characteristics_v_height;

	const auto windowHWND{ rendering_aspect.getComponent<HWND>("windowHWND")->getPurpose() };

	const auto fullscreen{ rendering_aspect.getComponent<bool>("fullscreen")->getPurpose() };
	if (fullscreen)
	{
		int         fullscreen_width;
		int         fullscreen_height;
		DXGI_FORMAT fullscreen_format;
		int         fullscreen_refresh_rate_num;
		int         fullscreen_refresh_rate_den;

		fullscreen_autoset_desktop_resolution(fullscreen_width, fullscreen_height, fullscreen_format, fullscreen_refresh_rate_num, fullscreen_refresh_rate_den);

		characteristics_width_resol = fullscreen_width;
		characteristics_height_resol = fullscreen_height;

		characteristics_v_width = 1.0;
		characteristics_v_height = characteristics_v_width * fullscreen_height / fullscreen_width;

		_RENDERME_TRACE(localLogger, std::string("full screen resol : ") + std::to_string(fullscreen_width) + "x" + std::to_string(fullscreen_height))

		swap_chain.BufferDesc.Format = fullscreen_format;
		swap_chain.BufferDesc.RefreshRate.Numerator = fullscreen_refresh_rate_num;
		swap_chain.BufferDesc.RefreshRate.Denominator = fullscreen_refresh_rate_den;

		swap_chain.Windowed = FALSE;
	}
	else
	{
		RECT rect;
		GetClientRect(windowHWND, &rect);

		characteristics_width_resol = rect.right - rect.left;
		characteristics_height_resol = rect.bottom - rect.top;
		
		characteristics_v_width = 1.0;
		characteristics_v_height = characteristics_v_width * characteristics_height_resol / characteristics_width_resol;

		swap_chain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swap_chain.BufferDesc.RefreshRate.Numerator = 60;
		swap_chain.BufferDesc.RefreshRate.Denominator = 1;	

		swap_chain.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		swap_chain.Windowed = TRUE;
	}


	_RENDERME_TRACE(localLogger, std::string("renderer characteristics : width_resol = ") + std::to_string(characteristics_width_resol) +
									std::string(" height_resol = ") + std::to_string(characteristics_height_resol) +
									std::string(" v_width = ") + std::to_string(characteristics_v_width) +
									std::string(" v_height = ") + std::to_string(characteristics_v_height))


	// complete main window entity with renderer characteristics
	rendering_aspect.addComponent<int>("widthResol", characteristics_width_resol);
	rendering_aspect.addComponent<int>("heightResol", characteristics_width_resol);
	rendering_aspect.addComponent<float>("viewportWidth", characteristics_v_width);
	rendering_aspect.addComponent<float>("viewporHeight", characteristics_v_height);


	swap_chain.BufferDesc.Width = characteristics_width_resol;
	swap_chain.BufferDesc.Height = characteristics_height_resol;
	swap_chain.BufferCount = 1;
	swap_chain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain.OutputWindow = windowHWND;
	swap_chain.SampleDesc.Count = 1; // de 1 a 4 en D3D11
	swap_chain.SampleDesc.Quality = 0; // en low quality

}