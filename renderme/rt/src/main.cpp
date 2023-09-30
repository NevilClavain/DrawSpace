
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

#include <Windows.h>

#include "app.h"
#include "exceptions.h"
#include "moduleload.h"
#include "module_root.h"

int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    try
    {
        const auto app{ renderMe::core::App::getInstance() };

        app->init(hInstance, "./rt_config/logrt.json", "./rt_config/windows_settings.json");

        if (strcmp(lpCmdLine, ""))
        {

            /*
            if (!DrawSpace::Utils::PILoad::LoadModule(lpCmdLine, "main_appmodule",
                &DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Module::Root>::GetInstance()->m_interface))
                */

            renderMe::interfaces::ModuleRoot* module_root;

            if( !renderMe::core::module::load(std::string(lpCmdLine), "main_appmodule", &module_root))
            {
                _EXCEPTION("cannot load " + std::string(lpCmdLine) + " module");
            }
        }
        else
        {
            _EXCEPTION("Usage : rt <module_name>");
        }

        app->loop();

    }
    catch (const std::exception& e)
    {
        const auto what{ e.what() };
        ::MessageBoxA(nullptr, what, "renderMe Exception", MB_OK | MB_ICONERROR);
    }
    return 0;
}
