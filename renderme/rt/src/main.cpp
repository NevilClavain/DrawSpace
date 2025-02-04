
/* -*-LIC_BEGIN-*- */
/*
*
* MaGE rendering framework
* Emmanuel Chaumont Copyright (c) 2013-2025
*
* This file is part of MaGE.
*
*    MaGE is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    MaGE is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with MaGE.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#include <Windows.h>

#include "app.h"
#include "exceptions.h"
#include "moduleload.h"
#include "module_root.h"

int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    try
    {
        if (strcmp(lpCmdLine, ""))
        {
            mage::interfaces::ModuleRoot* module_root{ nullptr };

            if (!mage::core::module::load(std::string(lpCmdLine), "main_appmodule", &module_root))
            {
                _EXCEPTION("cannot load " + std::string(lpCmdLine) + " module");
            }

            const auto app{ mage::core::App::getInstance() };
            app->init(hInstance, "./rt_config/logrt.json", "./rt_config/windows_settings.json", module_root);
            app->loop();

        }

        else
        {
            _EXCEPTION("Usage : rt <module_name>");
        }


    }
    catch (const std::exception& e)
    {
        const auto what{ e.what() };
        ::MessageBoxA(nullptr, what, "mage Exception", MB_OK | MB_ICONERROR);
    }
    return 0;
}
