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

#include "CEGUIWrapper.h"
#include "exceptions.h"

using namespace CEGUI;

CEGUIWrapper::CEGUIWrapper( void ) :
m_ready( false )
{
}


CEGUIWrapper::~CEGUIWrapper( void )
{
}

void CEGUIWrapper::SetReady( void )
{
    m_ready = true;
}

void CEGUIWrapper::RenderGUI( void )
{
    if( !m_ready )
    {
        _DSEXCEPTION( "GUI sub system is currently not initialized, cannot render" )
    }

    System::getSingleton().renderAllGUIContexts();
}

void CEGUIWrapper::SetResourcesRootDirectory( const dsstring& p_path )
{
    dsstring droot = p_path;

    DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>( CEGUI::System::getSingleton().getResourceProvider() );

    rp->setResourceGroupDirectory( "schemes", CEGUI::String( droot.c_str() ) + CEGUI::String( "/schemes/" ) );
    Scheme::setDefaultResourceGroup( "schemes" );  

    rp->setResourceGroupDirectory( "imagesets", CEGUI::String( droot.c_str() ) + CEGUI::String( "/imagesets/" ) );
    ImageManager::setImagesetDefaultResourceGroup( "imagesets" );

    rp->setResourceGroupDirectory( "fonts", CEGUI::String( droot.c_str() ) + CEGUI::String( "/fonts/" ) );
    Font::setDefaultResourceGroup( "fonts" );

    rp->setResourceGroupDirectory( "layouts", CEGUI::String( droot.c_str() ) + CEGUI::String( "/layouts/" ) );
    WindowManager::setDefaultResourceGroup( "layouts" );

    rp->setResourceGroupDirectory( "looknfeels", CEGUI::String( droot.c_str() ) + CEGUI::String( "/looknfeel/" ) );
    WidgetLookManager::setDefaultResourceGroup( "looknfeels" );

    rp->setResourceGroupDirectory("schemas", CEGUI::String( droot.c_str() ) + "/xml_schemas/");
    XMLParser* parser = CEGUI::System::getSingleton().getXMLParser();
    if( parser->isPropertyPresent( "SchemaDefaultResourceGroup" ) )
    {
        parser->setProperty( "SchemaDefaultResourceGroup", "schemas" );
    }    
}

void CEGUIWrapper::OnMouseMove( float p_xm, float p_ym, float p_dx, float p_dy )
{  
    CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();

    context.injectMouseMove( p_dx, p_dy );
    context.injectMousePosition( p_xm, p_ym );
}

void CEGUIWrapper::OnMouseLeftButtonDown( void )
{
}

void CEGUIWrapper::OnMouseLeftButtonUp( void )
{
}

void CEGUIWrapper::OnMouseRightButtonDown( void )
{
}

void CEGUIWrapper::OnMouseRightButtonUp( void )
{
}

void CEGUIWrapper::InitTest( void )
{
    WindowManager& wmgr = WindowManager::getSingleton();

    Window* myRoot = wmgr.createWindow( "DefaultWindow", "root" );
    System::getSingleton().getDefaultGUIContext().setRootWindow( myRoot );

    /*
    SchemeManager::getSingleton().createFromFile( "TaharezLook.scheme" );
    FrameWindow* fWnd = static_cast<FrameWindow*>( wmgr.createWindow( "TaharezLook/FrameWindow", "testWindow" ) );
    */

    SchemeManager::getSingleton().createFromFile( "AlfiskoSkin.scheme" );
    FrameWindow* fWnd = static_cast<FrameWindow*>( wmgr.createWindow( "AlfiskoSkin/FrameWindow", "testWindow" ) );

    myRoot->addChild( fWnd );
    fWnd->setPosition( UVector2( UDim( 0.01f, 0.0f ), UDim( 0.07f, 0.00f ) ) );
    fWnd->setText( "Hello World!" );

    CEGUI::Window* button0 = static_cast<FrameWindow*>( wmgr.createWindow( "AlfiskoSkin/Button", "testButton0" ) );

    fWnd->addChild( button0 );
    button0->setPosition( UVector2( UDim( 0.25f, 0.0f ), UDim( 0.75f, 0.0f ) ) );
    button0->setText( "Click me !" );
    
}