
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

#include <Windows.h>
#include <string>
#include "singleton.h"
#include "json.h"
#include "module_root.h"

namespace renderMe
{
    namespace core
    {
        class App : public property::Singleton<App>
        {
        public:

            App();
            ~App() = default;

            void init(HINSTANCE p_hInstance, const std::string& p_logconfig_path, const std::string& p_rtconfig_path, renderMe::interfaces::ModuleRoot* p_root);
            void loop(void);

        private:

            HWND                                    m_hwnd                  { nullptr };
            long                                    m_w_width               { 1024 };
            long                                    m_w_height              { 768 };
            bool                                    m_w_fullscreen          { false };

            bool                                    m_app_ready             { false };
            bool                                    m_keypress              { false };
            bool                                    m_keypulse              { false };
            long                                    m_keycode;

            bool                                    m_mousemoving           { true };
            bool                                    m_mousemovingstart      { true };
            bool                                    m_mousewheel            { false };
            long                                    m_mousemoving_pos;
            long                                    m_mousewheel_delta;

            long                                    m_mousemoving_last_x    { 0 };
            long                                    m_mousemoving_last_y    { 0 };
            long                                    m_mousemoving_curr_x    { 0 };
            long                                    m_mousemoving_curr_y    { 0 };
            long                                    m_mouse_circularmode    { false };

            bool                                    m_mousecursor_visible   { true };

            long                                    m_mouselclick_pos       { 0 };
            long                                    m_mouserclick_pos       { 0 };

            Json<>::Callback	                    m_json_cb;

            enum class JSONParsingMode
            {
                ON_ROOT,
                ON_FONTS,
            };

            JSONParsingMode                         m_json_parsing_mode{ JSONParsingMode::ON_ROOT };

            interfaces::ModuleRoot::Callback	    m_module_events_cb;

            interfaces::ModuleRoot*                 m_module_root           { nullptr };

            std::vector<std::string>                m_fonts;


            void    processInputEvents(void);

            bool    loopAppInit();

            void    onRenderFrame(void);


            void    onClose(void);

            void    onKeyPress(long p_key);
            void    onEndKeyPress(long p_key);

            void    onKeyPulse(long p_key);

            void    onChar(long p_char, long p_scan);

            void    onMouseMove(long p_xm, long p_ym, long p_dx, long p_dy);
            void    onMouseWheel(long p_distance);

            void    onMouseLeftButtonDown(long p_xm, long p_ym);
            void    onMouseLeftButtonUp(long p_xm, long p_ym);

            void    onMouseRightButtonDown(long p_xm, long p_ym);
            void    onMouseRightButtonUp(long p_xm, long p_ym);

            void    onAppEvent(WPARAM p_wParam, LPARAM p_lParam);


            // event from loaded module
            void    onModuleMouseVisible(bool p_state);
            void    onModuleMouseCircularModeUpdate(bool p_state);
            void    onModuleCloseapp(int p_code);

            void    quit(int p_code);

            static LRESULT CALLBACK winProc(HWND pHwnd, UINT pMsg, WPARAM pWParam, LPARAM pLParam);

            static void osInputEvtLButtonUp(long p_pos);
            static void	osInputEvtLButtonDown(long p_pos);
            static void	osInputEvtRButtonUp(long p_pos);
            static void	osInputEvtRButtonDown(long p_pos);
            static void	osInputEvtKeyDown(long p_key);
            static void	osInputEvtKeyUp(long p_key);
            static void osInputEvtChar(long p_char, long p_scan);
            static void	osInputEvtMouseMove(long p_pos, long p_button);
            static void osInputEvtMouseWheel(long p_distance);
            static void	osInputEvtApp(WPARAM p_wParam, LPARAM p_lParam);

        };

    } //core

} //renderMe