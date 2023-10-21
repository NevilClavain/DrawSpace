
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


#include "d3d11system.h"

#include "logsink.h"
#include "logconf.h"
#include "logging.h"


#include "aspects.h"
#include "entity.h"
#include "entitygraph.h"

using namespace renderMe::core;
using namespace renderMe::system;

static renderMe::core::logger::Sink localLogger("D3D11", renderMe::core::logger::Configuration::getInstance());


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

D3D11::D3D11(Entitygraph& p_entitygraph) : System(p_entitygraph)
{
}


void D3D11::run()
{
	for (auto it = m_entitygraph.preBegin(); it != m_entitygraph.preEnd(); ++it)
	{
		const auto current_entity{ it->data() };
		const auto currId{ current_entity->getId() };

		if (current_entity->hasAspect(core::renderingAspect::id))
		{
			const auto& rendering_aspect{ current_entity->aspectAccess(core::renderingAspect::id) };

			auto rendering_target_comp{ rendering_aspect.getComponent<core::renderingAspect::renderingTarget>("renderingTarget") };
			if (rendering_target_comp)
			{
				auto& rendering_target{ rendering_target_comp->getPurpose() };

				if (core::renderingAspect::renderingTarget::WINDOW_TARGET == rendering_target)
				{					
					if (!m_initialized)
					{						
						initD3D(current_entity);
						m_initialized = true;
					}
				}
			}
		}
	}
}

void D3D11::initD3D(core::Entity* p_mainWindow)
{
	_RENDERME_DEBUG(localLogger, std::string("initD3D"))

	IDXGIFactory* factory { nullptr };
	CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);

	DXGI_SWAP_CHAIN_DESC swap_chain;
	ZeroMemory(&swap_chain, sizeof(swap_chain));

	//get main windows infos
	const auto& rendering_aspect { p_mainWindow->aspectAccess(core::renderingAspect::id) };

	const auto fullscreen{ rendering_aspect.getComponent<bool>("fullscreen")->getPurpose() };
	if (fullscreen)
	{
		int         fullscreen_width;
		int         fullscreen_height;
		DXGI_FORMAT fullscreen_format;
		int         fullscreen_refresh_rate_num;
		int         fullscreen_refresh_rate_den;

		fullscreen_autoset_desktop_resolution(fullscreen_width, fullscreen_height, fullscreen_format, fullscreen_refresh_rate_num, fullscreen_refresh_rate_den);



		_RENDERME_TRACE(localLogger, std::string("full screen resol : ") + std::to_string(fullscreen_width) + "x" + std::to_string(fullscreen_height))
	}
	else
	{
		const auto windowwidth { rendering_aspect.getComponent<int>("windowWidth")->getPurpose() };
		const auto windowheight { rendering_aspect.getComponent<int>("windowHeight")->getPurpose() };

		
	}



}



