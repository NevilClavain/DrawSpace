/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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

#include "app.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface;

DrawSpace::Core::SingletonPlugin<Scripting>* DrawSpace::Core::SingletonPlugin<Scripting>::m_instance = NULL;

IMPLEMENT_APP( DFrontApp )

DFrontApp::DFrontApp( void ) :
m_app_ready( false )
{
}

DFrontApp::~DFrontApp( void )
{


}

bool DFrontApp::OnInit( void )
{
    if( !wxApp::OnInit() )
    {
        return false;
    }

    m_w_title = "Basic scene";

    DrawSpace::Initialize();


    /*
    if( false == load_scripting_plugin( "luascripting" ) )
    {
        wxMessageBox( "Unable to load scripting plugin. Exiting now", "DrawFront error", wxICON_ERROR );
        return false;
    }
    */


    m_mainframe = new BasicSceneMainFrame( NULL );
    m_mainframe->Show();
   
    m_hwnd = (HWND)m_mainframe->GetHWND();

    HWND hwndpanel = m_mainframe->GetNoteBook()->GetHWND();

    RECT rect, rectpanel;
    GetClientRect( m_hwnd, &rect );
    GetClientRect( hwndpanel, &rectpanel );

    m_w_width = rect.right - rectpanel.right;
    m_w_height = rect.bottom;
    m_mainframe->SetWindowDims( m_w_width, m_w_height );

    m_w_fullscreen = false;
    m_renderplugin = "drawspaced3d9";

    m_app_ready = true;
   
    if( false == load_renderer_plugin( m_renderplugin ) )
    {
        wxMessageBox( "Unable to load specified rendering plugin. Exiting now", "DrawFront error", wxICON_ERROR );
        return false;
    }
    m_mainframe->PrintOutputConsole( "Rendering plugin: ready (" + m_renderplugin + ")" );

    if( false == init_renderer() )
    {
        wxMessageBox( "Renderer init FAILURE. Exiting now", "DrawFront error", wxICON_ERROR );
        return false;
    }

    m_mainframe->PrintOutputConsole( "Rendering initialization: done" );

    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );

    try
    {

        bool status = Factory::GetInstance()->ExecuteFromTextFile( m_resource_filepath );
        if( status )
        {
            m_mainframe->PrintOutputConsole( "Resources file execution: OK (" + m_resource_filepath + ")" );
            m_mainframe->Update();
            m_mainframe->ExecStartupScript( m_script_filepath );
            m_mainframe->PrintOutputConsole( "Script file execution: OK (" + m_script_filepath + ")" );
        }
        else
        {
            dsstring last_error;

            Factory::GetInstance()->GetLastError( last_error );
            wxMessageBox( last_error.c_str(), "DrawFront parsing error", wxICON_ERROR );
            return false;
        }
    }
    catch( dsexception& p_exception )
    {
        const char* what = p_exception.what();
        wxMessageBox( what, "DrawSpace exception", wxICON_ERROR );
        return false;
    }

	return true;
}

int DFrontApp::OnExit( void )
{
    SingletonPlugin<Renderer>::GetInstance()->m_interface->Release();
    return 0;
}

bool DFrontApp::load_renderer_plugin( const dsstring& p_file )
{
	dsstring complete_path = p_file;
#ifdef _DEBUG
	complete_path += ".dll";
#else
	complete_path += "_r.dll";
#endif

    PlugInManager<Renderer>::Handle pihandle;
    Renderer* renderer;
	PluginManagerStatus pistatus = PlugInManager<Renderer>::LoadPlugin(complete_path.c_str(), pihandle);
    if( pistatus != PIM_OK )
    {
        return false;
    }

    if( PlugInManager<Renderer>::Instanciate( pihandle, &renderer ) != PIM_OK )
    {
        return false;
    }

    SingletonPlugin<Renderer>::GetInstance()->m_interface = renderer;
    
    return true;
}

/*
bool DFrontApp::load_scripting_plugin( const dsstring& p_file )
{
	dsstring complete_path = p_file;
#ifdef _DEBUG
	complete_path += ".dll";
#else
	complete_path += "_r.dll";
#endif

    PlugInManager<Scripting>::Handle pihandle;
    Scripting* scripting;
	PluginManagerStatus pistatus = PlugInManager<Scripting>::LoadPlugin( complete_path.c_str(), pihandle );
    if( pistatus != PIM_OK )
    {
        return false;
    }

    if( PlugInManager<Scripting>::Instanciate( pihandle, &scripting ) != PIM_OK )
    {
        return false;
    }

    SingletonPlugin<Scripting>::GetInstance()->m_interface = scripting;

    return true;
}
*/

bool DFrontApp::init_renderer( void )
{
    Renderer* renderer = SingletonPlugin<Renderer>::GetInstance()->m_interface;

    if( false == renderer->Init( m_hwnd, false, m_w_width, m_w_height, DrawSpace::Logger::Configuration::GetInstance() ) )
    {
        return false;
    }
    renderer->SetViewport( false, 0, 0, m_w_width, m_w_height, 0.0f, 1.0f );

    //m_frame->SetGlReady( true );
    m_mainframe->SetGLReady();
    return true;
}

void DFrontApp::OnInitCmdLine( wxCmdLineParser& p_parser )
{
    p_parser.SetDesc( cmdLineDesc );
}

bool DFrontApp::OnCmdLineParsed( wxCmdLineParser& p_parser )
{
    wxString path;

    p_parser.Found( "r", &path );

    wxCharBuffer buffer = path.ToAscii();
    m_resource_filepath = buffer.data();

    p_parser.Found( "s", &path );

    buffer = path.ToAscii();
    m_script_filepath = buffer.data();

    return true;
}

bool DFrontApp::OnExceptionInMainLoop( void )
{
    try
    {
        throw;
    }
    catch( dsexception& p_exception )
    {
        const char* what = p_exception.what();
        wxMessageBox( what, "DrawSpace exception", wxICON_ERROR );
    }
    catch( ... )
    {

    }

    return true;
}
