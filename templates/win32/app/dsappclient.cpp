
#include "dsappclient.h"

dsAppClient* dsAppClient::m_instance = NULL;


dsAppClient::dsAppClient( void )
{    
    _INIT_LOGGER( "app.conf" )  
}

dsAppClient::~dsAppClient( void )
{

}

void dsAppClient::OnRenderFrame( void )
{
}

bool dsAppClient::OnIdleAppInit( void )
{
    return true;
}

void dsAppClient::OnAppInit( void )
{

}

void dsAppClient::OnClose( void )
{

}

void dsAppClient::OnKeyPress( long p_key ) 
{

}

void dsAppClient::OnEndKeyPress( long p_key )
{

}

void dsAppClient::OnKeyPulse( long p_key )
{

}

void dsAppClient::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{

}

void dsAppClient::OnMouseLeftButtonDown( long p_xm, long p_ym )
{

}

void dsAppClient::OnMouseLeftButtonUp( long p_xm, long p_ym )
{

}

void dsAppClient::OnMouseRightButtonDown( long p_xm, long p_ym )
{

}

void dsAppClient::OnMouseRightButtonUp( long p_xm, long p_ym )
{

}

void dsAppClient::OnAppEvent( WPARAM p_wParam, LPARAM p_lParam )
{

}
