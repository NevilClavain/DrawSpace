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
    bool status;
    CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();

    status = context.injectMouseMove( p_dx, p_dy );
    status = context.injectMousePosition( p_xm, p_ym );
}

void CEGUIWrapper::OnMouseLeftButtonDown( void )
{
    CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
    context.injectMouseButtonDown( CEGUI::LeftButton );
}

void CEGUIWrapper::OnMouseLeftButtonUp( void )
{
    CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
    context.injectMouseButtonUp( CEGUI::LeftButton );
}

void CEGUIWrapper::OnMouseRightButtonDown( void )
{
    CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
    context.injectMouseButtonDown( CEGUI::RightButton );
}

void CEGUIWrapper::OnMouseRightButtonUp( void )
{
    CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
    context.injectMouseButtonUp( CEGUI::RightButton );
}

void CEGUIWrapper::OnKeyDown( long p_key )
{
    CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();    
    //context.injectKeyDown( CEGUI::Key::Scan::A );
}

void CEGUIWrapper::OnKeyUp( long p_key )
{
    CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
    //context.injectKeyUp( CEGUI::Key::Scan::A );
}

void CEGUIWrapper::OnChar( long p_key )
{
    CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
    context.injectChar( p_key );
}

void CEGUIWrapper::LoadLayoutFromFile( const dsstring& p_layout_path, const dsstring& p_scheme_path )
{
    WindowManager& wmgr = WindowManager::getSingleton();

    SchemeManager::getSingleton().createFromFile( p_scheme_path );

    Window* wRoot = wmgr.loadLayoutFromFile( p_layout_path );
    System::getSingleton().getDefaultGUIContext().setRootWindow( wRoot );

    // id 0 reservé à la fenetre root
    m_ceguiWindowTable[0] = wRoot;

    /*
    Window* myRoot = System::getSingleton().getDefaultGUIContext().getRootWindow();

    CEGUI::DefaultWindow* main_win = static_cast<CEGUI::DefaultWindow*>( myRoot->getChild( 1 ) );
    CEGUI::DefaultWindow* static_text = static_cast<CEGUI::DefaultWindow*>( main_win->getChild( 2 ) );
    static_text->setText( "prout !" );
    */
}

void CEGUIWrapper::Store( int p_parent_id, int p_id )
{
    if( m_ceguiWindowTable.count( p_parent_id ) > 0 )
    {
        Window* parent = m_ceguiWindowTable[p_parent_id];
        Window* child = parent->getChild( p_id );
        m_ceguiWindowTable[p_id] = child;
    }
    else
    {
        _DSEXCEPTION( "unregistered CEGUI window ID" );
    }
}

void CEGUIWrapper::SetText( int p_id, const dsstring& p_text )
{
    if( m_ceguiWindowTable.count( p_id ) > 0 )
    {
        m_ceguiWindowTable[p_id]->setText( p_text );
    }
    else
    {
        _DSEXCEPTION( "unregistered CEGUI window ID" );
    }
}

void CEGUIWrapper::SubscribePushButtonEventClicked( int p_id )
{
    if( m_ceguiWindowTable.count( p_id ) > 0 )
    {
        m_ceguiWindowTable[p_id]->subscribeEvent( CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber( &CEGUIWrapper::on_PushButton_EventClicked, this ) );
    }
    else
    {
        _DSEXCEPTION( "unregistered CEGUI window ID" );
    }
}

void CEGUIWrapper::InitTest( void )
{
    WindowManager& wmgr = WindowManager::getSingleton();

    SchemeManager::getSingleton().createFromFile( "AlfiskoSkin.scheme" );

    Window* myRoot = wmgr.loadLayoutFromFile( "main.layout" );
    System::getSingleton().getDefaultGUIContext().setRootWindow( myRoot );


    CEGUI::Window* button0 = static_cast<CEGUI::Window*>( wmgr.createWindow( "TaharezLook/Button", "testButton0" ) );
    button0->subscribeEvent( CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber( &CEGUIWrapper::on_PushButton_EventClicked, this ) );


    /*
    Window* myRoot = wmgr.createWindow( "DefaultWindow", "root" );
    System::getSingleton().getDefaultGUIContext().setRootWindow( myRoot );

    SchemeManager::getSingleton().createFromFile( "TaharezLook.scheme" );
    FrameWindow* fWnd = static_cast<FrameWindow*>( wmgr.createWindow( "TaharezLook/FrameWindow", "testWindow" ) );

    //////////////////////////////////////////

    myRoot->addChild( fWnd );
    fWnd->setPosition( UVector2( UDim( 0.01f, 0.0f ), UDim( 0.07f, 0.00f ) ) );
    fWnd->setText( "Hello World!" );

    //////////////////////////////////////////

    CEGUI::PushButton* button0 = static_cast<CEGUI::PushButton*>( wmgr.createWindow( "TaharezLook/Button", "testButton0" ) );

    fWnd->addChild( button0 );
    button0->setPosition( UVector2( UDim( 0.25f, 0.0f ), UDim( 0.5f, 0.0f ) ) );
    button0->setText( "Click me !" );

    button0->subscribeEvent( CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber( &CEGUIWrapper::on_button_click, this ) );
    
    //////////////////////////////////////////

    CEGUI::Editbox* ebx = static_cast<CEGUI::Editbox*>( wmgr.createWindow( "TaharezLook/Editbox", "testEbx0" ) );

    fWnd->addChild( ebx );

    ebx->setText( "" );
    */
}

bool CEGUIWrapper::on_PushButton_EventClicked(const CEGUI::EventArgs& p_evt )
{
    const CEGUI::MouseEventArgs& we = static_cast<const CEGUI::MouseEventArgs&>( p_evt ); 
    CEGUI::String senderID = we.window->getName();

    if( senderID == "Trigger" )
    {
        _asm nop;
    }

    if( senderID == "Quit" )
    {
        _asm nop;
    }


    return true;
}