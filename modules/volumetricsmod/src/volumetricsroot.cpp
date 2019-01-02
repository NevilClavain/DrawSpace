/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2019                     
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

#include "volumetricsroot.h"
#include "file.h"
#include "nebulaerenderingaspectimpl.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface::Module;

VolumetricsRoot::VolumetricsRoot( void )
{
}

VolumetricsRoot::~VolumetricsRoot( void )
{
}

dsstring VolumetricsRoot::GetModuleName( void ) const
{
    return "VolumetricsMod";
}

dsstring VolumetricsRoot::GetModuleDescr( void ) const
{
    return "Volumetrics rendering module";
}

void VolumetricsRoot::Init( void )
{
}

DrawSpace::Interface::AspectImplementations::RenderingAspectImpl* VolumetricsRoot::InstanciateRenderingAspectImpls( const dsstring& p_id )
{
    if( "nebulaeRender" == p_id )
    {
        return new DrawSpace::AspectImplementations::NebulaeRenderingAspectImpl;
    }
    return NULL;
}

void VolumetricsRoot::TrashRenderingAspectImpls( DrawSpace::Interface::AspectImplementations::RenderingAspectImpl* p_impl )
{
    DrawSpace::AspectImplementations::NebulaeRenderingAspectImpl* nb_impl = static_cast<DrawSpace::AspectImplementations::NebulaeRenderingAspectImpl*>(p_impl);
    delete nb_impl;
}
