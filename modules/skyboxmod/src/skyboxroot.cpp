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

#include "skyboxroot.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface::Module;

SkyboxRoot::SkyboxRoot( void )
{
    DrawSpace::Core::Shader::EnableShadersDescrInFinalPath( true );
    DrawSpace::Core::Shader::SetRootPath( "test_data/shaders_bank" );

    DrawSpace::Core::Texture::SetRootPath( "test_data/textures_bank" );
}

SkyboxRoot::~SkyboxRoot( void )
{
}

dsstring SkyboxRoot::GetModuleName( void )
{
    return "SkyboxMod";
}

dsstring SkyboxRoot::GetModuleDescr( void )
{
    return "Skybox module";
}

void SkyboxRoot::ServicesInit( void )
{
    m_services["skybox"] = new SkyboxService( m_id );
}