/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2021                     
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

#include "skyboxroot.h"
#include "file.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface::Module;

SkyboxRoot::SkyboxRoot( void )
{
}

SkyboxRoot::~SkyboxRoot( void )
{
}

dsstring SkyboxRoot::GetModuleName( void ) const
{
    return "SkyboxMod";
}

dsstring SkyboxRoot::GetModuleDescr( void ) const
{
    return "Skybox module";
}

void SkyboxRoot::Init( void )
{
    m_services["skybox"] = new SkyboxService( m_id );
}

void SkyboxRoot::Release(void)
{
    delete m_services["skybox"];
}

DrawSpace::Interface::AspectImplementations::RenderingAspectImpl* SkyboxRoot::InstanciateRenderingAspectImpls( const dsstring& p_id )
{    
    if( "skyboxRender" == p_id )
    {
        return new DrawSpace::AspectImplementations::SkyboxRenderingAspectImpl;
    }
    return NULL;
}

void SkyboxRoot::TrashRenderingAspectImpls( DrawSpace::Interface::AspectImplementations::RenderingAspectImpl* p_impl )
{
    DrawSpace::AspectImplementations::SkyboxRenderingAspectImpl* sb_impl = static_cast<DrawSpace::AspectImplementations::SkyboxRenderingAspectImpl*>( p_impl );
    delete sb_impl;
}
