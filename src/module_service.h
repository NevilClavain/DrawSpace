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

#ifndef _MODULE_SERVICE_
#define _MODULE_SERVICE_

#include "scenenode.h"
#include "renderer.h"
#include "module_keys.h"

namespace DrawSpace
{
namespace Interface
{
namespace Module
{
class Service
{
protected:
    DrawSpace::Module::KeysLinkTable        m_keysLinkTable;

    void                                    connect_keys( Service* p_subservice )
    {
        std::vector<DrawSpace::Module::KeySinkBase*> keys;
        p_subservice->GetKeys( keys );
        m_keysLinkTable.RegisterModuleKeysVector( keys );
    }

public:
    virtual void                            GetKeys( std::vector<DrawSpace::Module::KeySinkBase*>& p_keys ) = 0;

    virtual void                            Init( DrawSpace::Logger::Configuration* p_logconf, 
                                                    DrawSpace::Core::BaseCallback<void, bool>* p_mousecircularmode_cb, 
                                                    DrawSpace::Core::BaseCallback<void, bool>* p_mousevisible_cb, 
                                                    DrawSpace::Core::BaseCallback<void, int>* p_closeapp_cb ) = 0;

    virtual void                            Run( void ) = 0;
    virtual void                            Release( void ) = 0;

    virtual DrawSpace::Core::BaseSceneNode* InstanciateSceneNode( const dsstring& p_sceneNodeName ) = 0;
    virtual void                            RegisterScenegraphCallbacks( DrawSpace::Core::SceneNodeGraph& p_scenegraph ) = 0;
    virtual void                            ReleaseSceneNode( const dsstring& p_sceneNodeName ) = 0;


    virtual void                            OnKeyPress( long p_key ) {};
    virtual void                            OnEndKeyPress( long p_key ) {};
    virtual void                            OnKeyPulse( long p_key ) {};
    virtual void                            OnChar( long p_char, long p_scan ) {};
    virtual void                            OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy ) {};
    virtual void                            OnMouseWheel( long p_delta ) {};
    virtual void                            OnMouseLeftButtonDown( long p_xm, long p_ym ) {};
    virtual void                            OnMouseLeftButtonUp( long p_xm, long p_ym ) {};
    virtual void                            OnMouseRightButtonDown( long p_xm, long p_ym ) {};
    virtual void                            OnMouseRightButtonUp( long p_xm, long p_ym ) {};
    virtual void                            OnAppEvent( WPARAM p_wParam, LPARAM p_lParam ) {};

};
}
}
}
#endif