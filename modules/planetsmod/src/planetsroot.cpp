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

#include "planetsroot.h"
#include "file.h"
#include "planetsrenderingaspectimpl.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface::Module;

PlanetsRoot::PlanetsRoot( void )
{
}

PlanetsRoot::~PlanetsRoot( void )
{
}

dsstring PlanetsRoot::GetModuleName( void ) const
{
    return "PlanetsMod";
}

dsstring PlanetsRoot::GetModuleDescr( void ) const
{
    return "Planets rendering module";
}

void PlanetsRoot::Init( void )
{
}

DrawSpace::Interface::AspectImplementations::RenderingAspectImpl* PlanetsRoot::InstanciateRenderingAspectImpls( const dsstring& p_id )
{
    if ("planetsRender" == p_id)
    {
        DrawSpace::AspectImplementations::PlanetsRenderingAspectImpl* impl = new DrawSpace::AspectImplementations::PlanetsRenderingAspectImpl;
        impl->SetHub( m_hub );
        return impl;
    }

    return NULL;
}

void PlanetsRoot::TrashRenderingAspectImpls( DrawSpace::Interface::AspectImplementations::RenderingAspectImpl* p_impl )
{
    DrawSpace::AspectImplementations::PlanetsRenderingAspectImpl* pl_impl = static_cast<DrawSpace::AspectImplementations::PlanetsRenderingAspectImpl*>(p_impl);
    delete pl_impl;
}
