/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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

/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
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

#include "dsapp.h"
#include "tracedefs.h"

// sur laptop, forcer l'utilisation du GPU haute performance, au lieu des chipset graphiques integr�s
extern "C" {
	// This is the quickest and easiest way to enable using the nVidia GPU on a Windows laptop with a dedicated nVidia GPU and Optimus tech.
	// enable optimus!
    __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;

	// AMD have one too!!!
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}


_DECLARE_DS_LOGGER( logger, "App", DrawSpace::Logger::Configuration::GetInstance() )

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Interface;

#define TRAD_EVT_COORD_MOUSE( __pLParam__, __x__, __y__ ) \
    __x__ = (WORD)( __pLParam__ & 0x0000ffff ); \
    __y__ = (WORD)( ( __pLParam__ & 0xffff0000 ) >> 16 );

App* App::m_base_instance = NULL;

DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>* 	    DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::m_instance        = NULL;
DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Module::Root>* 	DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Module::Root>::m_instance    = NULL;



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK DrawSpaceAppWinProc( HWND pHwnd, UINT pMsg, WPARAM pWParam, LPARAM pLParam )
{
    switch( pMsg )
    {
        case WM_LBUTTONUP:

            App::OSInputEvtLButtonUp( (long)pLParam );
            break;

        case WM_LBUTTONDOWN:

            App::OSInputEvtLButtonDown( (long)pLParam );
            break;

        case WM_RBUTTONUP:
            
            App::OSInputEvtRButtonUp( (long)pLParam );
            break;

        case WM_RBUTTONDOWN:
            
            App::OSInputEvtRButtonDown( (long)pLParam );
            break;

        case WM_MOUSEMOVE:

            App::OSInputEvtMouseMove( (long)pLParam, (long)pWParam );
            break;

        case WM_KEYDOWN:

            App::OSInputEvtKeyDown( (long)pWParam );
            break;

        case WM_KEYUP:

            App::OSInputEvtKeyUp( (long)pWParam );
            break;

        case WM_CHAR:

            App::OSInputEvtChar( (long)pWParam, (long)pLParam);
            break;

        case WM_MOUSEWHEEL:

            App::OSInputEvtMouseWheel( (long)pWParam );
            break;

        case WM_QUIT:
        case WM_DESTROY:

            _DSDEBUG( logger, "PostQuitMessage" )
            PostQuitMessage( 0 );
            break;


        case WM_APP:

            App::OSInputEvtApp( pWParam, pLParam );
            break;

        default:
            return( DefWindowProc( pHwnd, pMsg, pWParam, pLParam ) );
    }
    
    return 0;
}


App::App( void ) :
m_config( 800, 600, false, false ),
m_w_width( 800 ),
m_w_height( 600 ),
m_w_fullscreen( false ),
m_w_style( WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZE ),
m_w_title( "DrawSpace Sample" ),
m_app_ready( false ),
m_keypress( false ),
m_keypulse( false ),
m_mousemoving( true ),
m_mousewheel( false ),
m_mousewheel_delta( 0 ),
m_mousemovingstart( true ),
m_mousemoving_curr_x( 0 ),
m_mousemoving_curr_y( 0 ),
m_mousemoving_last_x( 0 ),
m_mousemoving_last_y( 0 ),
m_mouse_circularmode( false ),
m_cmdline( "" ),
m_gl_ready( false )
{
}

App::~App( void )
{


}

void App::process_input_events( void )
{
    if( m_keypress )
    {
        OnKeyPress( m_keycode );
    }

    if( m_keypulse )
    {
        OnKeyPulse( m_keycode );
        m_keypulse = false;
    }

    if( m_mousemoving )
    {
        long x_m, y_m;

        TRAD_EVT_COORD_MOUSE( m_mousemoving_pos, x_m, y_m );

        if( m_mousemovingstart )
        {
            m_mousemoving_curr_x = x_m;
            m_mousemoving_curr_y = y_m;

            m_mousemoving_last_x = x_m;
            m_mousemoving_last_y = y_m;

            OnMouseMove( m_mousemoving_curr_x, m_mousemoving_curr_y, 0, 0 );


            m_mousemovingstart = false;
        }
        else
        {
            m_mousemoving_curr_x = x_m;
            m_mousemoving_curr_y = y_m;

            if( m_mouse_circularmode )
            {
                if( x_m < m_renderer_characteristics.width_resol / 4 )
                {
                    x_m = 3 * m_renderer_characteristics.width_resol / 4;

                    OnMouseMove( x_m, y_m, 0, m_mousemoving_curr_y - m_mousemoving_last_y );

                    m_mousemoving_last_x = x_m;

                    POINT point;

                    point.x = x_m;
                    point.y = y_m;

                    ClientToScreen( m_hwnd, &point );

                    SetCursorPos( point.x, point.y );
                }
                else if( x_m > 3 * m_renderer_characteristics.width_resol / 4 )
                {
                    x_m = m_renderer_characteristics.width_resol / 4;

                    OnMouseMove( x_m, y_m, 0, m_mousemoving_curr_y - m_mousemoving_last_y );

                    m_mousemoving_last_x = x_m;

                    POINT point;

                    point.x = x_m;
                    point.y = y_m;

                    ClientToScreen( m_hwnd, &point );

                    SetCursorPos( point.x, point.y );
                }
                else if( y_m < m_renderer_characteristics.height_resol / 4 )
                {
                    y_m = 3 * m_renderer_characteristics.height_resol / 4;

                    OnMouseMove( x_m, y_m, m_mousemoving_curr_x - m_mousemoving_last_x, 0 );

                    m_mousemoving_last_y = y_m;

                    POINT point;

                    point.x = x_m;
                    point.y = y_m;

                    ClientToScreen( m_hwnd, &point );

                    SetCursorPos( point.x, point.y );
                }
                else if( y_m > 3 * m_renderer_characteristics.height_resol / 4 )
                {
                    y_m = m_renderer_characteristics.height_resol / 4;

                    OnMouseMove( x_m, y_m, m_mousemoving_curr_x - m_mousemoving_last_x, 0 );

                    m_mousemoving_last_y = y_m;

                    POINT point;

                    point.x = x_m;
                    point.y = y_m;

                    ClientToScreen( m_hwnd, &point );

                    SetCursorPos( point.x, point.y );
                }
                else
                {
                    OnMouseMove( x_m, y_m, m_mousemoving_curr_x - m_mousemoving_last_x, m_mousemoving_curr_y - m_mousemoving_last_y );

                    m_mousemoving_last_x = m_mousemoving_curr_x;
                    m_mousemoving_last_y = m_mousemoving_curr_y;
                }
            }
            else
            {
                OnMouseMove( x_m, y_m, m_mousemoving_curr_x - m_mousemoving_last_x, m_mousemoving_curr_y - m_mousemoving_last_y );

                m_mousemoving_last_x = m_mousemoving_curr_x;
                m_mousemoving_last_y = m_mousemoving_curr_y;
            }
        }

        m_mousemoving = false;
    }

    if( m_mousewheel )
    {
        OnMouseWheel( m_mousewheel_delta );
        m_mousewheel = false;
    }
}

bool App::InitApp( HINSTANCE p_hInstance )
{
    m_config.ParseFromFile( "appgraphics.conf" );

    // transferer resultats du parsing
    m_w_width = m_config.m_width;
    m_w_height = m_config.m_height;
    m_w_fullscreen = m_config.m_fullscreen;
    m_renderplugin = m_config.m_renderplugin;

	_DSDEBUG(logger, dsstring( "configuration file reading success, new app config is now ") << m_w_width << dsstring(" x ") << m_w_height << dsstring( " fullscreen : " ) << m_w_fullscreen 
                    << dsstring( " renderplugin : " ) << m_renderplugin )

    OnAppInit();

	_DSDEBUG(logger, dsstring( "begin" ) )

    bool status = true;

    WNDCLASSA wc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( DWORD );
    wc.hInstance = p_hInstance;
    wc.hIcon = NULL;
    wc.hCursor = NULL;
    wc.hbrBackground = (HBRUSH)GetStockObject( BLACK_BRUSH );
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "DrawSpaceAppWnd";

    wc.lpfnWndProc = (WNDPROC)DrawSpaceAppWinProc;

    if( !RegisterClassA( &wc ) )
    {	
        _DSFATAL( logger, "RegisterClass FAIL" )
        status = false;
    }
    else
    {
        // RegisterClass OK
        if( m_w_fullscreen )
        {            
            // plein ecran
            RECT rect;
            GetWindowRect( GetDesktopWindow(), &rect );

            long fsw, fsh;
            fsw = (rect.right - rect.left);
            fsh = (rect.bottom - rect.top );

            m_w_width = fsw;
            m_w_height = fsh;

			_DSDEBUG(logger, dsstring("Fullscreen mode : CreateWindowExA ") << fsw << dsstring( " x " ) << fsh)
            m_hwnd = CreateWindowExA( WS_EX_TOPMOST, wc.lpszClassName, "", WS_POPUP, 0, 0, fsw, fsh, NULL, NULL, p_hInstance, NULL );
        }
        else
        {
            // mode fenetre
			_DSDEBUG(logger, dsstring("Windowed mode : CreateWindowA ") << m_w_width << dsstring( " x " ) << m_w_height )
            m_hwnd = CreateWindowA( wc.lpszClassName, (LPCSTR)m_w_title.c_str(), m_w_style, CW_USEDEFAULT, CW_USEDEFAULT, m_w_width, m_w_height, NULL, NULL, p_hInstance, NULL );
        }

        if( !m_hwnd )
        {			
            status = false;
            _DSERROR( logger, "CreateWindow FAIL" )
        }

        ShowWindow( m_hwnd, SW_SHOWDEFAULT );
        UpdateWindow( m_hwnd );

        m_app_ready = true;
    }

	_DSDEBUG(logger, dsstring( "status = " ) << status )
    return status;
}

void App::IdleApp( void )
{
    MSG	msg;

    if( false == OnIdleAppInit() )
    {
        return;
    }

    while( 1 )
    {	
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) 
        {
            if( WM_QUIT == msg.message ) 
            {
                _DSDEBUG( logger, "WM_QUIT, calling OnClose()" )
                OnClose();                
                break;
            }
            
            else
            {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }
        }
        else
        {
            if( m_app_ready )
            {
                DrawSpace::Logger::Configuration::GetInstance()->UpdateTick();
                process_input_events();
                OnRenderFrame();		
            }
        }
    }	
}

bool App::InitRenderer( void )
{
    _DSDEBUG( logger, "begin" )
    DrawSpace::Interface::Renderer* renderer = SingletonPlugin<Renderer>::GetInstance()->m_interface;

    if( false == renderer->Init( m_hwnd, m_w_fullscreen, m_w_width, m_w_height, DrawSpace::Logger::Configuration::GetInstance() ) )
    {
        _DSDEBUG( logger, "end FAIL" )
        return false;
    }

    m_gl_ready = true;

    // get renderer characteristics
    renderer->GetRenderCharacteristics( m_renderer_characteristics );

    _DSDEBUG( logger, "end OK" )
    return true;
}

void App::StopRenderer( void )
{
    SingletonPlugin<Renderer>::GetInstance()->m_interface->Release();
}

void App::GetRenderPluginName( dsstring& p_plugin )
{
    p_plugin = m_renderplugin;
}



//////////////////////////OS inputs////////////////////////////////////////////

void App::OSInputEvtLButtonDown( long p_pos )
{
    m_base_instance->m_mouselclick_pos = p_pos;
    
    long x_m, y_m;
    TRAD_EVT_COORD_MOUSE( m_base_instance->m_mouselclick_pos, x_m, y_m );

    m_base_instance->OnMouseLeftButtonDown( x_m, y_m );

}

void App::OSInputEvtLButtonUp( long p_pos )
{
    long x_m, y_m;
    TRAD_EVT_COORD_MOUSE( m_base_instance->m_mouselclick_pos, x_m, y_m );
    
    m_base_instance->OnMouseLeftButtonUp( x_m, y_m );

}

void App::OSInputEvtRButtonDown( long p_pos )
{
    m_base_instance->m_mouserclick_pos = p_pos;
    
    long x_m, y_m;
    TRAD_EVT_COORD_MOUSE( m_base_instance->m_mouserclick_pos, x_m, y_m );

    m_base_instance->OnMouseRightButtonDown( x_m, y_m );

}

void App::OSInputEvtRButtonUp( long p_pos )
{
    long x_m, y_m;
    TRAD_EVT_COORD_MOUSE( m_base_instance->m_mouserclick_pos, x_m, y_m );

    m_base_instance->OnMouseRightButtonUp( x_m, y_m );

}

void App::OSInputEvtKeyDown( long p_key )
{
    m_base_instance->m_keycode = p_key;
    m_base_instance->m_keypress = true;
    m_base_instance->m_keypulse = true;
}

void App::OSInputEvtKeyUp( long p_key )
{
    m_base_instance->m_keypress = false;
    m_base_instance->OnEndKeyPress( p_key );
}

void App::OSInputEvtChar( long p_char, long p_scan )
{
    m_base_instance->OnChar( p_char, p_scan );
}


void App::OSInputEvtMouseMove( long p_pos, long p_button )
{
    m_base_instance->m_mousemoving = true;
    m_base_instance->m_mousemoving_pos = p_pos;
}


void App::OSInputEvtMouseWheel( long p_distance )
{
    m_base_instance->m_mousewheel = true;
    m_base_instance->m_mousewheel_delta = p_distance;
}

void App::OSInputEvtApp( WPARAM p_wParam, LPARAM p_lParam )
{
    m_base_instance->OnAppEvent( p_wParam, p_lParam );
}

App::Config::Config( long p_width, long p_height, bool p_fullscreen, bool p_multithread ) :
m_width( p_width ),
m_height( p_height ),
m_fullscreen( p_fullscreen )
{
}

void App::Config::ParseFromFile( const dsstring& p_filepath )
{
    Utils::JSONParser::ParseFromFile( p_filepath );

    // check tokens

	int type0, size0;
	dsstring token_text0;

	int type1, size1;
	dsstring token_text1;

    if( JSMN_OBJECT == GetTokenType( 0 ) )
    {
        for( int i = 0; i < GetTokenSize( 0 ); i++ )
        {
            int tkindex = ( 2 * i ) + 1;

	        type0 = GetTokenType( tkindex );
	        size0 = GetTokenSize( tkindex );
	        GetTokenString( tkindex, token_text0 );

	        type1 = GetTokenType( tkindex + 1);
	        size1 = GetTokenSize( tkindex + 1 );
	        GetTokenString( tkindex + 1, token_text1 );

            
            if( "renderplugin" == token_text0 )
            {
                if( JSMN_STRING != type1 )
                {
                    _DSEXCEPTION( "string type expected for renderplugin field" );
                }

                m_renderplugin = token_text1;
            }
            else if( "fullscreen" == token_text0 )
            {
                if( JSMN_PRIMITIVE != type1 )
                {
                    _DSEXCEPTION( "string type expected for fullscreen field" );
                }

                if( "true" == token_text1 )
                {
                    m_fullscreen = true;
                }
                else
                {
                    m_fullscreen = false;
                }
            }
            else if( "width" == token_text0 )
            {
                if( JSMN_PRIMITIVE != type1 )
                {
                    _DSEXCEPTION( "string type expected for width field" );
                }

                m_width = atoi( token_text1.c_str() );
            
            }
            else if( "height" == token_text0 )
            {
                if( JSMN_PRIMITIVE != type1 )
                {
                    _DSEXCEPTION( "string type expected for height field" );
                }
            
                m_height = atoi( token_text1.c_str() );
            }            
        }
    }
    else
    {
        _DSEXCEPTION( "JSON parse : unexpected type for token 0" );
    }
}

void App::Quit( int p_code )
{
    PostQuitMessage( p_code );
}
