
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

#include <windows.h>
#include <string>

#include "eventsource.h"

namespace renderMe
{
    namespace core
    {
        class Entitygraph;
    }

    namespace interfaces
    {
        enum class ModuleEvents
        {
            MOUSE_CIRCULARMODE_CHANGED,
            MOUSE_DISPLAY_CHANGED,
            CLOSE_APP
        };

        class ModuleRoot : public property::EventSource<ModuleEvents, int>
        {
        public:
            virtual std::string             getModuleName() const = 0;
            virtual std::string             getModuleDescr() const = 0;

            virtual core::Entitygraph*      entitygraph() = 0;

            virtual void                    onKeyPress(long p_key) = 0;
            virtual void                    onEndKeyPress(long p_key) = 0;
            virtual void                    onKeyPulse(long p_key) = 0;
            virtual void                    onChar(long p_char, long p_scan) = 0;
            virtual void                    onMouseMove(long p_xm, long p_ym, long p_dx, long p_dy) = 0;
            virtual void                    onMouseWheel(long p_delta) = 0;
            virtual void                    onMouseLeftButtonDown(long p_xm, long p_ym) = 0;
            virtual void                    onMouseLeftButtonUp(long p_xm, long p_ym) = 0;
            virtual void                    onMouseRightButtonDown(long p_xm, long p_ym) = 0;
            virtual void                    onMouseRightButtonUp(long p_xm, long p_ym) = 0;
            virtual void                    onAppEvent(WPARAM p_wParam, LPARAM p_lParam) = 0;

        };
    }
}