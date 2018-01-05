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

#include "rootimpl.h"

#include "shader.h"
#include "texture.h"
#include "renderstate.h"
#include "AC3DMeshe.h"
#include "mainservice.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface::Module;

RootImpl::RootImpl( void )
{ 
    Shader::EnableShadersDescrInFinalPath( true );
    Shader::SetRootPath( "test_data/shaders_bank" );
    Texture::SetRootPath( "test_data/textures_bank" );
    RenderStatesSet::SetRootPath( "test_data/renderstates_bank" );

    AC3DMesheImport::SetRootPath( "test_data/meshes_bank" );

    //File::MountVirtualFS( "test_data.bank" );

}

RootImpl::~RootImpl( void )
{
}

void RootImpl::Init( void )
{
}

dsstring RootImpl::GetModuleName( void )
{
    return "ServiceExampleRoot";
}

dsstring RootImpl::GetModuleDescr( void )
{
    return "Service Example module";
}

DrawSpace::Interface::AspectImplementations::ServiceAspectImpl* RootImpl::InstanciateServiceAspectImpl( const dsstring& p_id )
{
    return _DRAWSPACE_NEW_( MainService, MainService );
}
