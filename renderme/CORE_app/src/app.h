
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

namespace renderMe
{
    namespace core
    {
        class App : public property::Singleton<App>
        {
        public:

            App();
            ~App() = default;

            bool init(HINSTANCE p_hInstance, const std::string& p_logconfig_path);
            void idleApp(void);
            bool initRenderer(void);
            void stopRenderer(void);

        private:

            HWND                                    m_hwnd          { nullptr };
            long                                    m_w_width       { 800 };
            long                                    m_w_height      { 600 };
            bool                                    m_w_fullscreen  { false };

        };

    } //core

} //renderMe