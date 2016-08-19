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
public:
    
    virtual void                            GetLocalKeys( const dsstring p_instanceName, std::vector<DrawSpace::Module::KeySinkBase*>& p_keys ) = 0;
    virtual void                            GetGlobalKeys( std::vector<DrawSpace::Module::KeySinkBase*>& p_keys ) = 0;
    virtual void                            Init( DrawSpace::Interface::Renderer* p_renderer, DrawSpace::Logger::Configuration* p_logconf ) = 0;
    virtual void                            Run( void ) = 0;
    virtual void                            Release( void ) = 0;
    virtual DrawSpace::Core::BaseSceneNode* GetSceneNode( void ) = 0;

    virtual void                            OnKeyPress( long p_key ) = 0;
    virtual void                            OnEndKeyPress( long p_key ) = 0;
    virtual void                            OnKeyPulse( long p_key ) = 0;
    virtual void                            OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy ) = 0;
    virtual void                            OnMouseLeftButtonDown( long p_xm, long p_ym ) = 0;
    virtual void                            OnMouseLeftButtonUp( long p_xm, long p_ym ) = 0;
    virtual void                            OnMouseRightButtonDown( long p_xm, long p_ym ) = 0;
    virtual void                            OnMouseRightButtonUp( long p_xm, long p_ym ) = 0;
    virtual void                            OnAppEvent( WPARAM p_wParam, LPARAM p_lParam ) = 0;

};
}
}
}
#endif