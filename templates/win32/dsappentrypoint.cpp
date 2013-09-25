//////////////////////////////////////////////////////////////////
// DrawSpace app entry point
// ECH 06/2013
// Copyright © 2013-2014
//
// 
//
//////////////////////////////////////////////////////////////////

#include <dsappclient.h>

bool LoadRendererPlugin( const dsstring& p_file )
{
    DrawSpace::Utils::PlugInManager<DrawSpace::Interface::Renderer>::Handle pihandle;
	DrawSpace::Interface::Renderer* renderer;
	PluginManagerStatus pistatus = DrawSpace::Utils::PlugInManager<DrawSpace::Interface::Renderer>::LoadPlugin( p_file.c_str(), pihandle );
	if( pistatus != PIM_OK )
	{
		return false;
	}

	if( DrawSpace::Utils::PlugInManager<DrawSpace::Interface::Renderer>::Instanciate( pihandle, &renderer ) != PIM_OK )
	{
		return false;
	}

    DrawSpace::Core::Plugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface = renderer;
	
	return true;
}

int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
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

    return 0;
}