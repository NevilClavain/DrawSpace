

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

#include "entitygraph.h"
#include "entity.h"
#include "aspects.h"

using namespace renderMe;
using namespace renderMe::core;

static renderMe::core::logger::Sink localLogger("App", renderMe::core::logger::Configuration::getInstance());

#define TRAD_EVT_COORD_MOUSE( __pLParam__, __x__, __y__ ) \
    __x__ = (WORD)( __pLParam__ & 0x0000ffff ); \
    __y__ = (WORD)( ( __pLParam__ & 0xffff0000 ) >> 16 );



App::App()
{
    m_json_cb = [&, this](JSONEvent p_event, const std::string& p_id, int p_index, const std::string& p_value)
    {
        switch (p_event)
        {
            case renderMe::core::JSONEvent::PRIMITIVE:

                if ("fullscreen" == p_id)
                {                    
                    this->m_w_fullscreen = ("true" == p_value ? true : false);
                }
                else if ("width" == p_id)
                {
                    this->m_w_width = std::stoi(p_value);
                }
                else if ("height" == p_id)
                {
                    this->m_w_height = std::stoi(p_value);
                }
                break;
        }
    };

    m_module_events_cb = [&, this](interfaces::ModuleEvents p_event, int p_evt_value)
    {
        switch (p_event)
        {
            case interfaces::ModuleEvents::CLOSE_APP:
                quit(p_evt_value);
                break;

            case interfaces::ModuleEvents::MOUSE_DISPLAY_CHANGED:

                this->onModuleMouseVisible(p_evt_value);
                break;

            case interfaces::ModuleEvents::MOUSE_CIRCULARMODE_CHANGED:
                
                this->onModuleMouseCircularModeUpdate(p_evt_value);
                break;
        }
    };
}

void App::init(HINSTANCE p_hInstance, const std::string& p_logconfig_path, const std::string& p_rtconfig_path, renderMe::interfaces::ModuleRoot* p_root)
{
    m_module_root = p_root;
    m_module_root->registerSubscriber(m_module_events_cb);

    // load logging config
    {
        // set static to spare some space on stack // compiler message
        static renderMe::core::FileContent<char> logConfFileContent(p_logconfig_path);
        logConfFileContent.load();

        const auto dataSize{ logConfFileContent.getDataSize() };
        const std::string data(logConfFileContent.getData(), dataSize);

        // set static to spare some space on stack // compiler message
        static renderMe::core::Json jsonParser;
        jsonParser.registerSubscriber(logger::Configuration::getInstance()->getCallback());

        const auto logParseStatus{ jsonParser.parse(data) };

        if (logParseStatus < 0)
        {
            _EXCEPTION("Cannot parse logging configuration")
        }
    }


    // load RT window config
    {
        // set static to spare some space on stack // compiler message
        static renderMe::core::FileContent<char> rtConfFileContent(p_rtconfig_path);
        rtConfFileContent.load();

        const auto dataSize{ rtConfFileContent.getDataSize() };
        const std::string data(rtConfFileContent.getData(), dataSize);

        // set static to spare some space on stack // compiler message
        static renderMe::core::Json jsonParser;
        jsonParser.registerSubscriber(m_json_cb);

        const auto rtParseStatus{ jsonParser.parse(data) };

        if (rtParseStatus < 0)
        {
            _EXCEPTION("Cannot parse windows settings configuration")
        }
    }


	_RENDERME_DEBUG(localLogger, std::string("app config is : ") << m_w_width << std::string(" x ") << m_w_height << std::string(" fullscreen : ") << m_w_fullscreen);

    // set static to spare some space on stack // compiler message
	static WNDCLASSA wc;

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
        _EXCEPTION("RegisterClass FAIL")
    }
    else
    {
        // RegisterClass OK
        if (m_w_fullscreen)
        {
            // plein ecran
            RECT rect;
            GetWindowRect(GetDesktopWindow(), &rect);

            const long fsw{ rect.right - rect.left };
            const long fsh{ rect.bottom - rect.top };

            m_w_width = fsw;
            m_w_height = fsh;

            _RENDERME_DEBUG(localLogger, std::string("Fullscreen mode : CreateWindowExA ") << fsw << std::string(" x ") << fsh)
            m_hwnd = CreateWindowExA(WS_EX_TOPMOST, wc.lpszClassName, "", WS_POPUP, 0, 0, fsw, fsh, nullptr, nullptr, p_hInstance, nullptr);
        }
        else
        {
            // mode fenetre
            _RENDERME_DEBUG(localLogger, std::string("Windowed mode : CreateWindowA ") << m_w_width << std::string(" x ") << m_w_height)

            static const std::string wTitle{ "renderMe" };
            m_hwnd = CreateWindowA(wc.lpszClassName, (LPCSTR)wTitle.c_str(), WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZE, CW_USEDEFAULT, CW_USEDEFAULT, m_w_width, m_w_height, nullptr, nullptr, p_hInstance, nullptr);
        }

        if (!m_hwnd)
        {
            _RENDERME_FATAL(localLogger, "CreateWindow FAIL")
            _EXCEPTION("CreateWindowA FAIL")
        }
        else
        {
            ShowWindow(m_hwnd, SW_SHOWDEFAULT);
            UpdateWindow(m_hwnd);

            m_app_ready = true;

            /// add root node to module entity graph : window rendering target infos

            auto entitygraph{ m_module_root->entitygraph() };

            if (entitygraph)
            {
                auto& root_window_node{ entitygraph->makeRoot("appwindow") };

                const auto root_window_entity{ root_window_node.data() };
                root_window_entity->makeAspect(core::renderingAspect::id);

                auto& rendering_aspect{ root_window_entity->aspectAccess(core::renderingAspect::id) };

                rendering_aspect.addComponent<core::renderingAspect::renderingTarget>("renderingTarget", core::renderingAspect::renderingTarget::WINDOW_TARGET);
                rendering_aspect.addComponent<bool>("fullscreen", m_w_fullscreen);
                rendering_aspect.addComponent<int>("windowWidth", m_w_width);
                rendering_aspect.addComponent<int>("windowHeight", m_w_height);
                rendering_aspect.addComponent<HWND>("windowHWND", m_hwnd);

                //////////////////////////

                m_module_root->init();
            }
            else
            {
                _RENDERME_WARN(localLogger, "no entity graph in attached module")
            }
        }
    }
}

void App::processInputEvents(void)
{
    if (m_keypress)
    {
        onKeyPress(m_keycode);
    }

    if (m_keypulse)
    {
        onKeyPulse(m_keycode);
        m_keypulse = false;
    }

    if (m_mousemoving)
    {
        long x_m, y_m;

        TRAD_EVT_COORD_MOUSE(m_mousemoving_pos, x_m, y_m);

        if (m_mousemovingstart)
        {
            m_mousemoving_curr_x = x_m;
            m_mousemoving_curr_y = y_m;

            m_mousemoving_last_x = x_m;
            m_mousemoving_last_y = y_m;

            onMouseMove(m_mousemoving_curr_x, m_mousemoving_curr_y, 0, 0);


            m_mousemovingstart = false;
        }
        else
        {
            m_mousemoving_curr_x = x_m;
            m_mousemoving_curr_y = y_m;

            if (m_mouse_circularmode)
            {
                /*
                const long    width_resol{ m_renderer_characteristics.width_resol };
                const long    height_resol{ m_renderer_characteristics.height_resol };
                */
                const long    width_resol{ m_w_width };
                const long    height_resol{ m_w_height };
                // TEMPORAIRE EN ATTENDANT LE RENDERER


                if (x_m < width_resol / 4)
                {
                    x_m = 3 * width_resol / 4;

                    onMouseMove(x_m, y_m, 0, m_mousemoving_curr_y - m_mousemoving_last_y);

                    m_mousemoving_last_x = x_m;

                    POINT point;

                    point.x = x_m;
                    point.y = y_m;

                    ClientToScreen(m_hwnd, &point);

                    SetCursorPos(point.x, point.y);
                }
                else if (x_m > 3 * width_resol / 4)
                {
                    x_m = width_resol / 4;

                    onMouseMove(x_m, y_m, 0, m_mousemoving_curr_y - m_mousemoving_last_y);

                    m_mousemoving_last_x = x_m;

                    POINT point;

                    point.x = x_m;
                    point.y = y_m;

                    ClientToScreen(m_hwnd, &point);

                    SetCursorPos(point.x, point.y);
                }
                else if (y_m < height_resol / 4)
                {
                    y_m = 3 * height_resol / 4;

                    onMouseMove(x_m, y_m, m_mousemoving_curr_x - m_mousemoving_last_x, 0);

                    m_mousemoving_last_y = y_m;

                    POINT point;

                    point.x = x_m;
                    point.y = y_m;

                    ClientToScreen(m_hwnd, &point);

                    SetCursorPos(point.x, point.y);
                }
                else if (y_m > 3 * height_resol / 4)
                {
                    y_m = height_resol / 4;

                    onMouseMove(x_m, y_m, m_mousemoving_curr_x - m_mousemoving_last_x, 0);

                    m_mousemoving_last_y = y_m;

                    POINT point;

                    point.x = x_m;
                    point.y = y_m;

                    ClientToScreen(m_hwnd, &point);

                    SetCursorPos(point.x, point.y);
                }
                else
                {
                    onMouseMove(x_m, y_m, m_mousemoving_curr_x - m_mousemoving_last_x, m_mousemoving_curr_y - m_mousemoving_last_y);

                    m_mousemoving_last_x = m_mousemoving_curr_x;
                    m_mousemoving_last_y = m_mousemoving_curr_y;
                }
            }
            else
            {
                onMouseMove(x_m, y_m, m_mousemoving_curr_x - m_mousemoving_last_x, m_mousemoving_curr_y - m_mousemoving_last_y);

                m_mousemoving_last_x = m_mousemoving_curr_x;
                m_mousemoving_last_y = m_mousemoving_curr_y;
            }
        }

        m_mousemoving = false;
    }

    if (m_mousewheel)
    {
        onMouseWheel(m_mousewheel_delta);
        m_mousewheel = false;
    }

}

bool App::loopAppInit()
{
    // MODULE stuff init HERE 

    std::string win_title{ "renderMe Runtime - " };
    win_title += m_module_root->getModuleDescr();
    ::SetWindowText(m_hwnd, win_title.c_str());

    return true;
}

void App::loop(void)
{
    if (loopAppInit())
    {
        MSG	msg;
        while (1)
        {
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                if (WM_QUIT == msg.message)
                {
                    _RENDERME_DEBUG(localLogger, "WM_QUIT, calling OnClose()")
                    onClose();
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

                    processInputEvents();
                    onRenderFrame();
                }
            }
        }
    }
}


void App::onRenderFrame(void)
{
    if (m_module_root)
    {
        m_module_root->run();
    }
}




void App::onClose(void)
{
    _RENDERME_DEBUG(localLogger, std::string("shutdown..."));
}

void App::onKeyPress(long p_key)
{
    _RENDERME_TRACE(localLogger, std::string("onKeyPress :") << p_key);
    m_module_root->onKeyPress(p_key);
}

void App::onEndKeyPress(long p_key)
{
    _RENDERME_TRACE(localLogger, std::string("onEndKeyPress :") << p_key);
    m_module_root->onEndKeyPress(p_key);
}

void App::onKeyPulse(long p_key)
{
    _RENDERME_TRACE(localLogger, std::string("onKeyPulse :") << p_key);
    m_module_root->onKeyPulse(p_key);
}

void App::onChar(long p_char, long p_scan)
{
    _RENDERME_TRACE(localLogger, std::string("onChar :") << p_char << std::string(" ") << p_scan);
    m_module_root->onChar(p_char, p_scan);
}

void App::onMouseMove(long p_xm, long p_ym, long p_dx, long p_dy)
{
    m_module_root->onMouseMove(p_xm, p_ym, p_dx, p_dy);
}

void App::onMouseWheel(long p_distance)
{
    _RENDERME_TRACE(localLogger, std::string("onMouseWheel :") << p_distance);
    m_module_root->onMouseWheel(p_distance);
}

void App::onMouseLeftButtonDown(long p_xm, long p_ym)
{
    _RENDERME_TRACE(localLogger, std::string("onMouseLeftButtonDown :") << p_xm << std::string(" ") << p_ym);
    m_module_root->onMouseLeftButtonDown(p_xm, p_ym);
}

void App::onMouseLeftButtonUp(long p_xm, long p_ym)
{
    _RENDERME_TRACE(localLogger, std::string("onMouseLeftButtonUp :") << p_xm << std::string(" ") << p_ym);
    m_module_root->onMouseLeftButtonUp(p_xm, p_ym);
}

void App::onMouseRightButtonDown(long p_xm, long p_ym)
{
    _RENDERME_TRACE(localLogger, std::string("onMouseRightButtonDown :") << p_xm << std::string(" ") << p_ym);
    m_module_root->onMouseRightButtonDown(p_xm, p_ym);
}

void App::onMouseRightButtonUp(long p_xm, long p_ym)
{
    _RENDERME_TRACE(localLogger, std::string("onMouseRightButtonUp :") << p_xm << std::string(" ") << p_ym);
    m_module_root->onMouseRightButtonUp(p_xm, p_ym);
}

void App::onAppEvent(WPARAM p_wParam, LPARAM p_lParam)
{
    _RENDERME_TRACE(localLogger, std::string("onAppEvent :") << p_wParam << std::string(" ") << p_lParam);
    m_module_root->onAppEvent(p_wParam, p_lParam);
}


void App::onModuleMouseVisible(bool p_state)
{
    // ne pas appeler ::ShowCursor() quand c'est inutile (par ex. ShowCursor( true ) si le curseur est deja visible), car sinon
    // ca fout le bordel (gestion d'un compteur interne, dixit la doc windows !!!!)

    if (!p_state)
    {
        if (m_mousecursor_visible)
        {
            ::ShowCursor(false);
            m_mousecursor_visible = false;
        }
    }
    else
    {
        if (!m_mousecursor_visible)
        {
            ::ShowCursor(true);
            m_mousecursor_visible = true;
        }
    }
}

void App::onModuleMouseCircularModeUpdate(bool p_state)
{
    m_mouse_circularmode = p_state;

    /*
    // ne pas appeler ::ShowCursor() quand c'est inutile (par ex. ShowCursor( true ) si le curseur est deja visible), car sinon
    // ca fout le bordel (gestion d'un compteur interne, dixit la doc windows !!!!)

    if( p_state )
    {
        if( m_mousecursor_visible )
        {
            ::ShowCursor( false );
            m_mousecursor_visible = false;
        }
    }
    else
    {
        if( !m_mousecursor_visible )
        {
            ::ShowCursor( true );
            m_mousecursor_visible = true;
        }
    }
    */

}

void App::onModuleCloseapp(int p_code)
{
    quit(p_code);
}

void App::quit(int p_code)
{
    ::PostQuitMessage(p_code);
}


LRESULT CALLBACK App::winProc(HWND pHwnd, UINT pMsg, WPARAM pWParam, LPARAM pLParam)
{
    switch (pMsg)
    {
        case WM_LBUTTONUP:

            osInputEvtLButtonUp((long)pLParam);
            break;

        case WM_LBUTTONDOWN:

            osInputEvtLButtonDown((long)pLParam);
            break;

        case WM_RBUTTONUP:

            osInputEvtRButtonUp((long)pLParam);
            break;

        case WM_RBUTTONDOWN:

            osInputEvtRButtonDown((long)pLParam);
            break;

        case WM_MOUSEMOVE:

            osInputEvtMouseMove((long)pLParam, (long)pWParam);
            break;

        case WM_KEYDOWN:

            osInputEvtKeyDown((long)pWParam);
            break;

        case WM_KEYUP:

            osInputEvtKeyUp((long)pWParam);
            break;

        case WM_CHAR:

            osInputEvtChar((long)pWParam, (long)pLParam);
            break;

        case WM_MOUSEWHEEL:

            osInputEvtMouseWheel((long)pWParam);
            break;

        case WM_QUIT:
        case WM_DESTROY:

            _RENDERME_DEBUG(localLogger, "PostQuitMessage");
            PostQuitMessage(0);
            break;

        case WM_APP:

            osInputEvtApp(pWParam, pLParam);
            break;

        default:
            return(DefWindowProc(pHwnd, pMsg, pWParam, pLParam));
    }

    return 0;
}

void App::osInputEvtLButtonUp(long p_pos)
{
    App::getInstance()->m_mouselclick_pos = p_pos;

    long x_m, y_m;
    TRAD_EVT_COORD_MOUSE(App::getInstance()->m_mouselclick_pos, x_m, y_m);

    App::getInstance()->onMouseLeftButtonUp(x_m, y_m);
}

void App::osInputEvtLButtonDown(long p_pos)
{
    long x_m, y_m;
    TRAD_EVT_COORD_MOUSE(App::getInstance()->m_mouselclick_pos, x_m, y_m);

    App::getInstance()->onMouseLeftButtonDown(x_m, y_m);
}

void App::osInputEvtRButtonUp(long p_pos)
{
    App::getInstance()->m_mouserclick_pos = p_pos;

    long x_m, y_m;
    TRAD_EVT_COORD_MOUSE(App::getInstance()->m_mouserclick_pos, x_m, y_m);

    App::getInstance()->onMouseRightButtonUp(x_m, y_m);
}

void App::osInputEvtRButtonDown(long p_pos)
{
    long x_m, y_m;
    TRAD_EVT_COORD_MOUSE(App::getInstance()->m_mouserclick_pos, x_m, y_m);

    App::getInstance()->onMouseRightButtonDown(x_m, y_m);
}

void App::osInputEvtKeyDown(long p_key)
{
    App::getInstance()->m_keycode = p_key;
    App::getInstance()->m_keypress = true;
    App::getInstance()->m_keypulse = true;
}

void App::osInputEvtKeyUp(long p_key)
{
    App::getInstance()->m_keypress = false;
    App::getInstance()->onEndKeyPress(p_key);
}

void App::osInputEvtChar(long p_char, long p_scan)
{
    App::getInstance()->onChar(p_char, p_scan);
}

void App::osInputEvtMouseMove(long p_pos, long p_button)
{
    App::getInstance()->m_mousemoving = true;
    App::getInstance()->m_mousemoving_pos = p_pos;
}

void App::osInputEvtMouseWheel(long p_distance)
{
    App::getInstance()->m_mousewheel = true;
    App::getInstance()->m_mousewheel_delta = p_distance;
}

void App::osInputEvtApp(WPARAM p_wParam, LPARAM p_lParam)
{
    App::getInstance()->onAppEvent(p_wParam, p_lParam);
}
