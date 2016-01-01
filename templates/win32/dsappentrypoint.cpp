/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2016                              
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

#include <dsappclient.h>

bool LoadRendererPlugin( const dsstring& p_file )
{
	dsstring complete_path = p_file;
#ifdef _DEBUG
	complete_path += ".dll";
#else
	complete_path += "_r.dll";
#endif

    DrawSpace::Utils::PlugInManager<DrawSpace::Interface::Renderer>::Handle pihandle;
    DrawSpace::Interface::Renderer* renderer;
	PluginManagerStatus pistatus = DrawSpace::Utils::PlugInManager<DrawSpace::Interface::Renderer>::LoadPlugin( complete_path.c_str(), pihandle );
    if( pistatus != PIM_OK )
    {
        return false;
    }

    if( DrawSpace::Utils::PlugInManager<DrawSpace::Interface::Renderer>::Instanciate( pihandle, &renderer ) != PIM_OK )
    {
        return false;
    }

    DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface = renderer;
    
    return true;
}

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

        if( !LoadRendererPlugin( plugin ) )
        {
            MessageBoxA( NULL, "Cannot load specified plugin" , "DrawSpace", MB_OK | MB_ICONSTOP );
            return 0;
        }


        if( app->InitRenderer() == false )
        {
            MessageBoxA( NULL, "InitRenderer FAILURE" , "DrawSpace", MB_OK | MB_ICONSTOP );
            return 0;
        }

        app->IdleApp();
        app->StopRenderer();
        DrawSpace::Logger::Configuration::RemoveInstance();
    }
    catch( dsexception& p_exception )
    {
        const char* what = p_exception.what();        
        MessageBoxA( NULL, what , "DrawSpace Exception", MB_OK | MB_ICONERROR );
    }

    return 0;
}
