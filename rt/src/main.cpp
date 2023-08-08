/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
*
* This file is part of DrawSpace.
*
*    DrawSpace is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    DrawSpace is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with DrawSpace.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#include "dsappclient.h"
#include "piload.h"

int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    try
    {
        dsAppClient* app = dsAppClient::GetInstance();
        app->SetCmdLine( lpCmdLine );

        if( app->InitApp( hInstance ) == false )
        {
            MessageBox( NULL, "InitApp FAILURE" , "DrawSpace", MB_OK | MB_ICONSTOP );
            return 0;
        }

        // get plugin name
        dsstring plugin;
        app->GetRenderPluginName( plugin );
        if( "" == plugin )
        {
            MessageBoxA( NULL, "No plugin specified" , "DrawSpace", MB_OK | MB_ICONSTOP );
            return 0;
        }

        if( !DrawSpace::Utils::PILoad::LoadRendererPlugin( plugin ) )
        {
            dsstring msg{ dsstring("Cannot load specified plugin : ") + plugin };
            MessageBoxA( NULL, msg.c_str(), "DrawSpace", MB_OK | MB_ICONSTOP );
            return 0;
        }


        if( app->InitRenderer() == false )
        {
            MessageBoxA( NULL, "InitRenderer FAILURE" , "DrawSpace", MB_OK | MB_ICONSTOP );
            return 0;
        }

        ///////////////////////////////////////////////////////////

        if( strcmp( lpCmdLine, "" ) )
        {
            if( !DrawSpace::Utils::PILoad::LoadModule( lpCmdLine, "main_appmodule",                                                         
                                                        &DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Module::Root>::GetInstance()->m_interface ) )
            {
                _DSEXCEPTION( "cannot load " + dsstring( lpCmdLine ) + " module" );
            }
        }
        else
        {
            _DSEXCEPTION( "Usage : rt <module_name>" );
        }

        ///////////////////////////////////////////////////////////

        app->IdleApp();
        app->StopRenderer();
        
    }
    catch( dsexception& p_exception )
    {
        const char* what = p_exception.what();        
        MessageBoxA( NULL, what , "DrawSpace Exception", MB_OK | MB_ICONERROR );
    }

    return 0;
}
