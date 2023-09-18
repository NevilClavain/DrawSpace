

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

#include "app.h"

#include "filesystem.h"
#include "logsink.h"
#include "logconf.h"
#include "logging.h"


using namespace renderMe;
using namespace renderMe::core;

static renderMe::core::logger::Sink localLogger("App", renderMe::core::logger::Configuration::getInstance());

App::App()
{

}

LRESULT CALLBACK App::winProc(HWND pHwnd, UINT pMsg, WPARAM pWParam, LPARAM pLParam)
{
    return(DefWindowProcA(pHwnd, pMsg, pWParam, pLParam));
}

bool App::init(HINSTANCE p_hInstance, const std::string& p_logconfig_path)
{
	//renderMe::core::FileContent<char> logConfFileContent();
	renderMe::core::FileContent<char> logConfFileContent(p_logconfig_path);
	logConfFileContent.load();

	const auto dataSize{ logConfFileContent.getDataSize() };
	const std::string data(logConfFileContent.getData(), dataSize);

	renderMe::core::Json jsonParser;
	jsonParser.registerSubscriber(logger::Configuration::getInstance()->getCallback());

	const auto parseStatus{ jsonParser.parse(data) };

	_RENDERME_DEBUG(localLogger, std::string("app config is : ") << m_w_width << std::string(" x ") << m_w_height << std::string(" fullscreen : ") << m_w_fullscreen);

	bool status{ true };

	WNDCLASSA wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(DWORD);
	wc.hInstance = p_hInstance;
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = "renderMeAppWnd";

	wc.lpfnWndProc = (WNDPROC)winProc;

    if (!RegisterClassA(&wc))
    {
        _RENDERME_FATAL(localLogger, "RegisterClass FAIL")
        status = false;
    }
    else
    {
        // RegisterClass OK
        if (m_w_fullscreen)
        {
            // plein ecran
            RECT rect;
            GetWindowRect(GetDesktopWindow(), &rect);

            long fsw, fsh;
            fsw = (rect.right - rect.left);
            fsh = (rect.bottom - rect.top);

            m_w_width = fsw;
            m_w_height = fsh;

            _RENDERME_DEBUG(localLogger, std::string("Fullscreen mode : CreateWindowExA ") << fsw << std::string(" x ") << fsh)
            m_hwnd = CreateWindowExA(WS_EX_TOPMOST, wc.lpszClassName, "", WS_POPUP, 0, 0, fsw, fsh, NULL, NULL, p_hInstance, NULL);
        }
        else
        {
            // mode fenetre
            _RENDERME_DEBUG(localLogger, std::string("Windowed mode : CreateWindowA ") << m_w_width << std::string(" x ") << m_w_height)

            static const std::string wTitle{ "renderMe" };
            m_hwnd = CreateWindowA(wc.lpszClassName, (LPCSTR)wTitle.c_str(), WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZE, CW_USEDEFAULT, CW_USEDEFAULT, m_w_width, m_w_height, NULL, NULL, p_hInstance, NULL);

            const auto lastError{ GetLastError() };

            _asm nop
        }

        if (!m_hwnd)
        {
            status = false;
            _RENDERME_ERROR(localLogger, "CreateWindow FAIL")
        }

        ShowWindow(m_hwnd, SW_SHOWDEFAULT);
        UpdateWindow(m_hwnd);

        m_app_ready = true;
    }

    _RENDERME_DEBUG(localLogger, std::string("App init status = ") << status)

	return status;
}

bool App::loopAppInit()
{
    // MODULE stuff init HERE : TODO

    return true;
}

void App::loop(void)
{
    if (loopAppInit())
    {
        MSG	msg;
        while (1)
        {
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                if (WM_QUIT == msg.message)
                {
                    _RENDERME_DEBUG(localLogger, "WM_QUIT, calling OnClose()")
                    //OnClose();
                    break;
                }

                else
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
            else
            {
                if (m_app_ready)
                {
                    logger::Configuration::getInstance()->updateTick();

                    //process_input_events();
                    //OnRenderFrame();
                }
            }
        }
    }
}

bool App::initRenderer(void)
{
	return true;
}

void App::stopRenderer(void)
{

}
