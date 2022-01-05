/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2022
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

/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
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

#include "cdlodplanetroot.h"
#include "file.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface::Module;

CDLODPlanetRoot::CDLODPlanetRoot( void )
{
    Shader::EnableShadersDescrInFinalPath( true );
    Shader::SetRootPath( "cdlodplanet_data/shaders_bank" );
	Texture::SetRootPath( "cdlodplanet_data/textures_bank" );
    RenderStatesSet::SetRootPath( "test_data/renderstates_bank" );
}

CDLODPlanetRoot::~CDLODPlanetRoot( void )
{
}

dsstring CDLODPlanetRoot::GetModuleName( void ) const
{
    return "CDLODPlanetMod";
}

dsstring CDLODPlanetRoot::GetModuleDescr( void ) const
{
    return "CDLOD/Procedural planet module";
}

void CDLODPlanetRoot::Init( void )
{
    m_services["cdlodplanet"] = new CDLODPlanetService( m_id );
}

void CDLODPlanetRoot::Release(void)
{
    delete m_services["cdlodplanet"];
}
