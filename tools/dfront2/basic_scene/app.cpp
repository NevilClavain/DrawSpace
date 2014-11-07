/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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

IMPLEMENT_APP( DFrontApp )

DFrontApp::DFrontApp( void ) :
m_config( 800, 600, false ),
m_app_ready( false )
{
}

DFrontApp::~DFrontApp( void )
{


}

bool DFrontApp::OnInit( void )
{
    m_w_title = "Basic scene";

    DrawSpace::Initialize();

    bool parser_status = m_config.Run( "appconfig.txt", "    " );

    m_w_width = m_config.m_width;
    m_w_height = m_config.m_height;
    m_w_fullscreen = m_config.m_fullscreen;
    m_renderplugin = m_config.m_renderplugin;

    if( m_w_fullscreen )
    {
        wxMessageBox( wxT("Fullscreen mode not allowed. Exiting now"), wxT("DrawFront error"), wxICON_ERROR );
        return false;
    }

    RenderFrame::m_caption = m_w_title;
    RenderFrame::m_size = wxSize( m_w_width, m_w_height );
    m_frame = RenderFrame::GetInstance();

    m_hwnd = (HWND)m_frame->GetHWND();

	m_frame->Show();

    m_app_ready = true;

    if( false == DrawSpace::Utils::LoadMesheImportPlugin( "ac3dmeshe.dll", "ac3dmeshe_plugin" ) )
    {
        wxMessageBox( wxT("Unable to load ac3d plugin. Exiting now"), wxT("DrawFront error"), wxICON_ERROR );
        return false;
    }
    m_meshe_import = DrawSpace::Utils::InstanciateMesheImportFromPlugin( "ac3dmeshe_plugin" );
    m_frame->SetMesheImport( m_meshe_import );

    if( false == load_renderer_plugin( m_renderplugin ) )
    {
        wxMessageBox( wxT("Unable to load specified plugin. Exiting now"), wxT("DrawFront error"), wxICON_ERROR );
        return false;
    }

    if( false == init_renderer() )
    {
        wxMessageBox( wxT("Renderer init FAILURE. Exiting now"), wxT("DrawFront error"), wxICON_ERROR );
        return false;
    }

    if( false == parser_status )
    {
        wxMessageBox( wxT("Unable to load appconfig.txt\nBack to default values"), wxT("DrawFront warning"), wxICON_WARNING );
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
    PlugInManager<Renderer>::Handle pihandle;
    Renderer* renderer;
    PluginManagerStatus pistatus = PlugInManager<Renderer>::LoadPlugin( p_file.c_str(), pihandle );
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


bool DFrontApp::init_renderer( void )
{
    Renderer* renderer = SingletonPlugin<Renderer>::GetInstance()->m_interface;

    if( false == renderer->Init( m_hwnd, false, m_w_width, m_w_height ) )
    {
        return false;
    }
    renderer->SetViewport( true, 0, 0, 0, 0, 0.0f, 1.0f );

    m_frame->SetGlReady( true );
    return true;
}

DFrontApp::Config::Config( long p_width, long p_height, bool p_fullscreen ) :
m_width( p_width ),
m_height( p_height ),
m_fullscreen( p_fullscreen )
{
}

bool DFrontApp::Config::on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words )
{
    if( 2 == p_words.size() )
    {
        if( "width" == p_words[0] )
        {
            m_width = atoi( p_words[1].c_str() );
        }
        if( "height" == p_words[0] )
        {
            m_height = atoi( p_words[1].c_str() );
        }
        if( "fullscreen" == p_words[0] )
        {
            if( "true" == p_words[1] )
            {
                m_fullscreen = true;
            }
            else
            {
                m_fullscreen = false;
            }
        }
        if( "renderplugin" == p_words[0] )
        {
            m_renderplugin = p_words[1];
        }
    }
    return true;
}
