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


#include "CEGUIWrapper.h"
#include "exceptions.h"
#include "memalloc.h"

using namespace CEGUI;
using namespace DrawSpace::Utils;

CEGUIWrapper::CEGUIWrapper( void ) :
m_ready( false ),
m_currentLayout( NULL )
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

    //status = context.injectMouseMove( p_dx, p_dy );
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

    Key::Scan s = (Key::Scan)p_key;
    context.injectKeyDown( s );
}

void CEGUIWrapper::OnKeyUp( long p_key )
{
    int delta = 0;
    if( VK_LEFT == p_key )
    {
        delta = -1;
    }
    else if( VK_RIGHT == p_key )
    {
        delta = 1;
    }


    if( delta )
    {
        CEGUI::Editbox* edbx = find_focused_editbox();
        if( edbx )
        {
            size_t idx = edbx->getCaretIndex();

            idx += delta;

            if( idx < 1 )
            {
                idx = 1;
            }
            else if( idx > edbx->getText().size() )
            {
                idx = edbx->getText().size();
            }

            edbx->setCaretIndex( idx );
        }
    }

    CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
    Key::Scan s = (Key::Scan)p_key;
    context.injectKeyUp( s );
}

void CEGUIWrapper::OnChar( long p_key )
{
    if( 0x08 == p_key ) // backspace
    {
        CEGUI::Editbox* edbx = find_focused_editbox();
        if( edbx )
        {
            CEGUI::String content = edbx->getText();
            if( content.size() > 0 )
            {                
                dsstring content2 = content.c_str();

                size_t idx = edbx->getCaretIndex();
                content2.erase( idx - 1, 1 );
                idx--;
                if( idx < 1 )
                {
                    idx= 1;
                }

                edbx->setText( content2.c_str() );
                edbx->setCaretIndex( idx );
            }
        }
    }

    CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
    context.injectChar( p_key  );
}

void CEGUIWrapper::LoadScheme( const dsstring& p_scheme_path )
{
    SchemeManager::getSingleton().createFromFile( p_scheme_path );
}

void CEGUIWrapper::LoadLayout( const dsstring& p_layout_path, const dsstring& p_widgets_path )
{
    WindowManager& wmgr = WindowManager::getSingleton();

    Window* wRoot = wmgr.loadLayoutFromFile( p_layout_path );

    m_layoutNamesTable[p_layout_path] = wRoot;
    
    dsstring rootName = wRoot->getName().c_str();
    m_layoutsTable[wRoot][rootName] = wRoot;

    ////
    
    JSONParser parser;
    int token_index = 1;

    //parser.ParseFromFile( "testskin/layouts/main_widgets.conf" );
    parser.ParseFromFile( p_widgets_path );

    if( JSMN_OBJECT == parser.GetTokenType( 0 ) )
    {
        recurs_register_widgets( parser, token_index, p_layout_path, "" );
    }
    else
    {
        _DSEXCEPTION( "JSON parse : unexpected type for token 0" );
    }  
}

void CEGUIWrapper::recurs_register_widgets( JSONParser& p_parser, int& p_token_index, const dsstring& p_layout_path, const dsstring& p_parent )
{
    int type0 = p_parser.GetTokenType( p_token_index );
    int size0 = p_parser.GetTokenSize( p_token_index );
    int type1 = p_parser.GetTokenType( p_token_index + 1 );
    int size1 = p_parser.GetTokenSize( p_token_index + 1 );

    if( JSMN_STRING == type0 )
    {
        dsstring widget_name;
        p_parser.GetTokenString( p_token_index, widget_name );

        p_token_index++;

        if( JSMN_OBJECT == type1 )
        {
            p_token_index++;

            if( "" != p_parent )
            {
                Store( p_layout_path, p_parent, widget_name );
            }

            if( size1 != 0 )
            {
                for( int i = 0; i < size1; i++ )
                {
                    recurs_register_widgets( p_parser, p_token_index, p_layout_path, widget_name );                
                }
            }
        }
        else
        {
            _DSEXCEPTION( "JSON parse : unexpected type for token (object expected)" );
        }
    }
    else
    {
        _DSEXCEPTION( "JSON parse : unexpected type for token (string expected)" );
    }
}

void CEGUIWrapper::SetLayout( const dsstring& p_layoutpath )
{
    if( m_layoutNamesTable.count( p_layoutpath ) )
    {
        m_currentLayoutName = p_layoutpath;
        m_currentLayout = m_layoutNamesTable[p_layoutpath];
        System::getSingleton().getDefaultGUIContext().setRootWindow( m_currentLayout );        
    }
    else
    {
        _DSEXCEPTION( "unregistered CEGUI layout" );
    }  
}

void CEGUIWrapper::Store( const dsstring& p_layoutName, const dsstring& p_parentName, const dsstring& p_childName )
{
    if( m_layoutNamesTable.count( p_layoutName ) > 0 )
    {
        CEGUI::Window* wRoot = m_layoutNamesTable[p_layoutName];

        WidgetsTable& wt = m_layoutsTable[wRoot];

        if( wt.count( p_parentName ) > 0 )
        {
            Window* parent = wt[p_parentName];
            //Window* child = parent->getChild( p_id );
            Window* child = parent->getChildRecursive( p_childName );

            if( !child )
            {
                _DSEXCEPTION( "Cannot locate windows : " + p_childName );
            }

            dsstring childName = child->getName().c_str();
            wt[childName] = child;

            CEGUI::Editbox* edbx = dynamic_cast<CEGUI::Editbox*>( child );
            if( edbx )
            {
                m_editBoxes.push_back( edbx );
                return;
            }

            CEGUI::ToggleButton* tbn = dynamic_cast<CEGUI::ToggleButton*>( child );
            if( tbn )
            {
                tbn->subscribeEvent( CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber( &CEGUIWrapper::on_ToggleButton_EventSelectStateChanged, this ) );
                return;
            }

            CEGUI::PushButton* btn = dynamic_cast<CEGUI::PushButton*>( child );            
            if( btn )
            {
                btn->subscribeEvent( CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber( &CEGUIWrapper::on_PushButton_EventClicked, this ) );
            }
        }
        else
        {
            _DSEXCEPTION( "unregistered CEGUI window ID" );
        }
    }
    else
    {
         _DSEXCEPTION( "unregistered CEGUI layout" );
    }
}

void CEGUIWrapper::SetVisibleState( const dsstring& p_layoutName, const dsstring& p_widgetName, bool p_state )
{
    if( m_layoutNamesTable.count( p_layoutName ) > 0 )
    {
        WidgetsTable& wt = m_layoutsTable[m_layoutNamesTable[p_layoutName]];

        if( wt.count( p_widgetName ) > 0 )
        {
            Window* widget = wt[p_widgetName];
            widget->setVisible( p_state );
        }
    }
}

bool CEGUIWrapper::IsVisible( const dsstring& p_layoutName, const dsstring& p_widgetName )
{
    if( m_layoutNamesTable.count( p_layoutName ) > 0 )
    {
        WidgetsTable& wt = m_layoutsTable[m_layoutNamesTable[p_layoutName]];

        if( wt.count( p_widgetName ) > 0 )
        {
            Window* widget = wt[p_widgetName];
            return widget->isVisible();
        }
    }
    return false;
}

void CEGUIWrapper::AddListboxTextItem( const dsstring& p_layoutName, const dsstring& p_widgetName, const dsstring& p_text, unsigned int p_colors, const dsstring& p_brushImage  )
{
    if( m_layoutNamesTable.count( p_layoutName ) > 0 )
    {
        WidgetsTable& wt = m_layoutsTable[m_layoutNamesTable[p_layoutName]];

        if( wt.count( p_widgetName ) > 0 )
        {
            Window* widget = wt[p_widgetName];
            
            CEGUI::Listbox* lsbx = dynamic_cast<CEGUI::Listbox*>( widget );
            if( !lsbx )
            {
                 _DSEXCEPTION( "Widget with corresponding ID is not a CEGUI::Listbox" );
            }

            CEGUI::ListboxTextItem* lbtext = new CEGUI::ListboxTextItem( p_text.c_str() );

            CEGUI::Colour tc;
            tc.setARGB( p_colors );

            lbtext->setSelectionBrushImage( p_brushImage );
            lbtext->setSelectionColours( tc );
            lbtext->setTextColours( tc );
            lbtext->setAutoDeleted( true ); //pas besoin de desallouer sur clear list ou suppression item :)

            lsbx->addItem( lbtext );
        }
        else
        {
            _DSEXCEPTION( "unregistered CEGUI window ID" );
        }
    }
    else
    {
         _DSEXCEPTION( "unregistered CEGUI layout" );
    }
}

void CEGUIWrapper::ClearListbox( const dsstring& p_layoutName, const dsstring& p_widgetName )
{
    if( m_layoutNamesTable.count( p_layoutName ) > 0 )
    {
        WidgetsTable& wt = m_layoutsTable[m_layoutNamesTable[p_layoutName]];

        if( wt.count( p_widgetName ) > 0 )
        {
            Window* widget = wt[p_widgetName];
            
            CEGUI::Listbox* lsbx = dynamic_cast<CEGUI::Listbox*>( widget );
            if( !lsbx )
            {
                 _DSEXCEPTION( "Widget with corresponding ID is not a CEGUI::Listbox" );
            }

            lsbx->resetList();
        }
        else
        {
            _DSEXCEPTION( "unregistered CEGUI window ID" );
        }
    }
    else
    {
         _DSEXCEPTION( "unregistered CEGUI layout" );
    }
}

bool CEGUIWrapper::GetListboxFirstSelectedItemIndex(  const dsstring& p_layoutName, const dsstring& p_widgetName, int& p_index, dsstring& p_text )
{
    if( m_layoutNamesTable.count( p_layoutName ) > 0 )
    {
        WidgetsTable& wt = m_layoutsTable[m_layoutNamesTable[p_layoutName]];

        if( wt.count( p_widgetName ) > 0 )
        {
            Window* widget = wt[p_widgetName];
            
            CEGUI::Listbox* lsbx = dynamic_cast<CEGUI::Listbox*>( widget );
            if( !lsbx )
            {
                 _DSEXCEPTION( "Widget with corresponding ID is not a CEGUI::Listbox" );
            }

            ListboxItem* lb_item = lsbx->getFirstSelectedItem();

            if( !lb_item )
            {
                return false;
            }

            ListboxTextItem* text = static_cast<ListboxTextItem*>( lb_item );

            p_text = text->getText().c_str();

            p_index = lsbx->getItemIndex( lb_item );

            return true;
        }
        else
        {
            _DSEXCEPTION( "unregistered CEGUI window ID" );
        }
    }
    else
    {
         _DSEXCEPTION( "unregistered CEGUI layout" );
    }
}


void CEGUIWrapper::AddComboboxTextItem( const dsstring& p_layoutName, const dsstring& p_widgetName, const dsstring& p_text, unsigned int p_colors, const dsstring& p_brushImage )
{
    if( m_layoutNamesTable.count( p_layoutName ) > 0 )
    {
        WidgetsTable& wt = m_layoutsTable[m_layoutNamesTable[p_layoutName]];

        if( wt.count( p_widgetName ) > 0 )
        {
            Window* widget = wt[p_widgetName];
            
            CEGUI::Combobox* cbbx = dynamic_cast<CEGUI::Combobox*>( widget );
            if( !cbbx )
            {
                 _DSEXCEPTION( "Widget with corresponding ID is not a CEGUI::Combobox" );
            }

            CEGUI::ListboxTextItem* lbtext = new CEGUI::ListboxTextItem( p_text.c_str() );

            CEGUI::Colour tc;
            tc.setARGB( p_colors );

            lbtext->setSelectionBrushImage( p_brushImage );
            lbtext->setSelectionColours( tc );
            lbtext->setTextColours( tc );
            lbtext->setAutoDeleted( true ); //pas besoin de desallouer sur clear list ou suppression item :)

            cbbx->addItem( lbtext );
        }
        else
        {
            _DSEXCEPTION( "unregistered CEGUI window ID" );
        }
    }
    else
    {
         _DSEXCEPTION( "unregistered CEGUI layout" );
    }
}

void CEGUIWrapper::ClearCombobox( const dsstring& p_layoutName, const dsstring& p_widgetName )
{
    if( m_layoutNamesTable.count( p_layoutName ) > 0 )
    {
        WidgetsTable& wt = m_layoutsTable[m_layoutNamesTable[p_layoutName]];

        if( wt.count( p_widgetName ) > 0 )
        {
            Window* widget = wt[p_widgetName];
            
            CEGUI::Combobox* cbbx = dynamic_cast<CEGUI::Combobox*>( widget );
            if( !cbbx )
            {
                 _DSEXCEPTION( "Widget with corresponding ID is not a CEGUI::Combobox" );
            }

            cbbx->resetList();
        }
        else
        {
            _DSEXCEPTION( "unregistered CEGUI window ID" );
        }
    }
    else
    {
         _DSEXCEPTION( "unregistered CEGUI layout" );
    }
}

void CEGUIWrapper::SetComboBoxItemSelectionState( const dsstring& p_layoutName, const dsstring& p_widgetName, int p_index, bool p_state )
{
    if( m_layoutNamesTable.count( p_layoutName ) > 0 )
    {
        WidgetsTable& wt = m_layoutsTable[m_layoutNamesTable[p_layoutName]];

        if( wt.count( p_widgetName ) > 0 )
        {
            Window* widget = wt[p_widgetName];
            
            CEGUI::Combobox* cbbx = dynamic_cast<CEGUI::Combobox*>( widget );
            if( !cbbx )
            {
                 _DSEXCEPTION( "Widget with corresponding ID is not a CEGUI::Combobox" );
            }
            cbbx->setItemSelectState( (size_t)p_index, p_state );
        }
        else
        {
            _DSEXCEPTION( "unregistered CEGUI window ID" );
        }
    }
    else
    {
         _DSEXCEPTION( "unregistered CEGUI layout" );
    }
}

int CEGUIWrapper::GetComboBoxSelectionIndex( const dsstring& p_layoutName, const dsstring& p_widgetName )
{
    if( m_layoutNamesTable.count( p_layoutName ) > 0 )
    {
        WidgetsTable& wt = m_layoutsTable[m_layoutNamesTable[p_layoutName]];

        if( wt.count( p_widgetName ) > 0 )
        {
            Window* widget = wt[p_widgetName];
            
            CEGUI::Combobox* cbbx = dynamic_cast<CEGUI::Combobox*>( widget );
            if( !cbbx )
            {
                 _DSEXCEPTION( "Widget with corresponding ID is not a CEGUI::Combobox" );
            }
            return cbbx->getItemIndex( cbbx->getSelectedItem() );
        }
        else
        {
            _DSEXCEPTION( "unregistered CEGUI window ID" );
        }
    }
    else
    {
         _DSEXCEPTION( "unregistered CEGUI layout" );
    }
}

void CEGUIWrapper::SetText( const dsstring& p_layoutName, const dsstring& p_widgetName, const dsstring& p_text )
{
    if( m_layoutNamesTable.count( p_layoutName ) > 0 )
    {
        WidgetsTable& wt = m_layoutsTable[m_layoutNamesTable[p_layoutName]];

        if( wt.count( p_widgetName ) > 0 )
        {
            Window* widget = wt[p_widgetName];
            widget->setText( p_text );
        }
        else
        {
            _DSEXCEPTION( "unregistered CEGUI window ID" );
        }
    }
    else
    {
         _DSEXCEPTION( "unregistered CEGUI layout" );
    }
}

void CEGUIWrapper::GetText( const dsstring& p_layoutName, const dsstring& p_widgetName, dsstring& p_outtext )
{
    if( m_layoutNamesTable.count( p_layoutName ) > 0 )
    {
        WidgetsTable& wt = m_layoutsTable[m_layoutNamesTable[p_layoutName]];

        if( wt.count( p_widgetName ) > 0 )
        {
            Window* widget = wt[p_widgetName];
            p_outtext = widget->getText().c_str();
        }
        else
        {
            _DSEXCEPTION( "unregistered CEGUI window ID" );
        }
    }
    else
    {
         _DSEXCEPTION( "unregistered CEGUI layout" );
    }
}

void CEGUIWrapper::RegisterPushButtonEventClickedHandler( DrawSpace::Core::BaseCallback2<void, const dsstring&, const dsstring&>* p_handler )
{
    m_pushbuttoneventclicked_handlers_list.push_back( p_handler );
}

void CEGUIWrapper::RegisterCheckboxEventStateChangedHandler( DrawSpace::Core::BaseCallback3<void, const dsstring&, const dsstring&, bool>* p_handler )
{
    m_checkboxeventstatechanged_handlers_list.push_back( p_handler );
}


bool CEGUIWrapper::IsCheckBoxChecked( const dsstring& p_layoutName, const dsstring& p_widgetName )
{
    if( m_layoutNamesTable.count( p_layoutName ) > 0 )
    {
        WidgetsTable& wt = m_layoutsTable[m_layoutNamesTable[p_layoutName]];

        if( wt.count( p_widgetName ) > 0 )
        {
            Window* widget = wt[p_widgetName];

            CEGUI::ToggleButton* tb = dynamic_cast<CEGUI::ToggleButton*>( widget );
            if( !tb )
            {
                 _DSEXCEPTION( "Widget with corresponding ID is not a CEGUI::ToggleButton" );
            }

            return tb->isSelected();
        }
        else
        {
            _DSEXCEPTION( "unregistered CEGUI window ID" );
        }
    }
    else
    {
         _DSEXCEPTION( "unregistered CEGUI layout" );
    }
}

void CEGUIWrapper::SetCheckboxState( const dsstring& p_layoutName, const dsstring& p_widgetName, bool p_state )
{
    if( m_layoutNamesTable.count( p_layoutName ) > 0 )
    {
        WidgetsTable& wt = m_layoutsTable[m_layoutNamesTable[p_layoutName]];

        if( wt.count( p_widgetName ) > 0 )
        {
            Window* widget = wt[p_widgetName];
            CEGUI::ToggleButton* tb = dynamic_cast<CEGUI::ToggleButton*>( widget );

            tb->setSelected( p_state );
        }
        else
        {
            _DSEXCEPTION( "unregistered CEGUI window ID" );
        }
    }
    else
    {
         _DSEXCEPTION( "unregistered CEGUI layout" );
    }
}

void CEGUIWrapper::InitTest( void )
{
    WindowManager& wmgr = WindowManager::getSingleton();

    SchemeManager::getSingleton().createFromFile( "AlfiskoSkin.scheme" );

    Window* myRoot = wmgr.loadLayoutFromFile( "main.layout" );
    System::getSingleton().getDefaultGUIContext().setRootWindow( myRoot );

    /*
    CEGUI::Window *myImageWindow = CEGUI::WindowManager::getSingleton().createWindow("xfskin/StaticImage", "progress" );
    myImageWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0.5,0),CEGUI::UDim(0.5,0)));
    */

    /*
    CEGUI::Window* button0 = static_cast<CEGUI::Window*>( wmgr.createWindow( "TaharezLook/Button", "testButton0" ) );
    button0->subscribeEvent( CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber( &CEGUIWrapper::on_PushButton_EventClicked, this ) );
    */

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
    const CEGUI::WindowEventArgs& we = static_cast<const CEGUI::MouseEventArgs&>( p_evt ); 
    CEGUI::String senderID = we.window->getName();

    for( size_t i = 0; i < m_pushbuttoneventclicked_handlers_list.size(); i++ )
    {
        dsstring widgetId( senderID.c_str() );
        (*m_pushbuttoneventclicked_handlers_list[i])( m_currentLayoutName, widgetId );
    }
    return true;
}

bool CEGUIWrapper::on_ToggleButton_EventSelectStateChanged(const CEGUI::EventArgs& p_evt )
{
    const CEGUI::WindowEventArgs& we = static_cast<const CEGUI::MouseEventArgs&>( p_evt ); 
    CEGUI::String senderID = we.window->getName();

    CEGUI::ToggleButton* tbn = static_cast<CEGUI::ToggleButton*>( we.window );

    for( size_t i = 0; i < m_checkboxeventstatechanged_handlers_list.size(); i++ )
    {
        dsstring widgetId( senderID.c_str() );
        (*m_checkboxeventstatechanged_handlers_list[i])( m_currentLayoutName, widgetId, tbn->isSelected() );
    }
    return true;
}

void CEGUIWrapper::SetMouseCursorImage( const dsstring& p_image )
{
    System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage( p_image );
}

void CEGUIWrapper::ShowMouseCursor( bool p_show )
{
    System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible( p_show );
}

CEGUI::Editbox* CEGUIWrapper::find_focused_editbox( void )
{
    for( size_t i = 0; i < m_editBoxes.size(); i++ )
    {
        if( m_editBoxes[i]->hasInputFocus() )
        {
            return m_editBoxes[i];
        }
    }

    return NULL;
}

void CEGUIWrapper::CreateSprite( const dsstring& p_scheme_object, const dsstring& p_spriteName )
{
    CEGUI::Window* myImageWindow = CEGUI::WindowManager::getSingleton().createWindow( p_scheme_object, p_spriteName );
    
    CEGUI::Window* wRoot = m_layoutNamesTable["planetsetup.layout"];
    wRoot->addChild( myImageWindow );

    m_spritesTable[p_spriteName] = myImageWindow;
}

void CEGUIWrapper::SetSpritePosition( const dsstring& p_spriteName, dsreal p_xpos, dsreal p_ypos )
{
    if( 0 == m_spritesTable.count( p_spriteName ) )
    {
        _DSEXCEPTION( "unregistered CEGUI sprite ID" );
    }

    m_spritesTable[p_spriteName]->setPosition( CEGUI::UVector2( CEGUI::UDim( p_xpos, 0 ),CEGUI::UDim( p_ypos,0 ) ) );
}

void CEGUIWrapper::SetSpriteImage( const dsstring& p_spriteName, const dsstring& p_image )
{
    if( 0 == m_spritesTable.count( p_spriteName ) )
    {
        _DSEXCEPTION( "unregistered CEGUI sprite ID" );
    }

    m_spritesTable[p_spriteName]->setProperty( "Image", p_image );
}

void CEGUIWrapper::SetSpriteScale( const dsstring& p_spriteName, dsreal p_scale )
{
    if( 0 == m_spritesTable.count( p_spriteName ) )
    {
        _DSEXCEPTION( "unregistered CEGUI sprite ID" );
    }

    CEGUI::USize us = m_spritesTable[p_spriteName]->getSize();

    us = us * p_scale;
    m_spritesTable[p_spriteName]->setSize( us );
}

void CEGUIWrapper::SetSpriteRotation( const dsstring& p_spriteName, const DrawSpace::Utils::Vector& p_axis, dsreal p_deg_angle )
{
    if( 0 == m_spritesTable.count( p_spriteName ) )
    {
        _DSEXCEPTION( "unregistered CEGUI sprite ID" );
    }

    m_spritesTable[p_spriteName]->setRotation( CEGUI::Quaternion::axisAngleDegrees( CEGUI::Vector3f( p_axis[0], p_axis[1], p_axis[2] ), p_deg_angle ) );
}
