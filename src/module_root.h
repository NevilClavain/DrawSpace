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



#ifndef _MODULE_ROOT_
#define _MODULE_ROOT_

#include "module_service.h"
#include "plugin.h"

#include "renderingaspectimpl.h"
#include "transformaspectimpl.h"
#include "serviceaspectimpl.h"

namespace DrawSpace
{
namespace Interface
{
namespace Module
{
class Root
{
protected:

    // table des differents services
    std::map<dsstring, DrawSpace::Interface::Module::Service*>                              m_services;


    dsstring                                                                                m_id;           //identifiant instance de module

public:

    virtual void                                                                UpdateRenderer( DrawSpace::Interface::Renderer* p_renderer );

    virtual void                                                                SetInstanceId( const dsstring& p_id ) { m_id = p_id; };

    virtual dsstring                                                            GetModuleName( void ) = 0;
    virtual dsstring                                                            GetModuleDescr( void ) = 0;
    
    virtual void                                                                Init( void ) = 0;

    virtual void                                                                DumpMemoryAllocs( void );

    virtual std::vector<dsstring>                                               GetServicesList( void );
    virtual Service*                                                            InstanciateService( const dsstring& p_id );

    virtual DrawSpace::Interface::AspectImplementations::RenderingAspectImpl*   InstanciateRenderingAspectImpls( const dsstring& p_id );
    virtual DrawSpace::Interface::AspectImplementations::TransformAspectImpl*   InstanciateTransformAspectImpls( const dsstring& p_id );
    virtual DrawSpace::Interface::AspectImplementations::ServiceAspectImpl*     InstanciateServiceAspectImpl( const dsstring& p_id );


};
}
}
}
#endif
