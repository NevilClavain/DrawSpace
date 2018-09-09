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

#include "mvtroot.h"
#include "file.h"
#include "fpstransformaspectimpl.h"
#include "freetransformaspectimpl.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface::Module;
using namespace DrawSpace::AspectImplementations;

MvtRoot::MvtRoot( void )
{
}

MvtRoot::~MvtRoot( void )
{
}

dsstring MvtRoot::GetModuleName( void )
{
    return "MvtMod";
}

dsstring MvtRoot::GetModuleDescr( void )
{
    return "Movements module";
}

void MvtRoot::Init( void )
{
}

DrawSpace::Interface::AspectImplementations::TransformAspectImpl* MvtRoot::InstanciateTransformAspectImpls(const dsstring& p_id)
{
    if( "fps" == p_id )
    {
        return new FPSTransformAspectImpl();
    }
    else if( "free" == p_id )
    {
        return new FreeTransformAspectImpl();
    }
    else if( "longlat" == p_id)
    {
        // todo
    }
    else if( "linear" == p_id)
    {
        // todo
    }
    else if ("head" == p_id)
    {
        // todo
    }
    else if ("spectator" == p_id)
    {
        //todo
    }

    return NULL;
}

void MvtRoot::TrashTransformAspectImpls(DrawSpace::Interface::AspectImplementations::TransformAspectImpl* p_impl)
{
    FPSTransformAspectImpl* fpst = dynamic_cast<FPSTransformAspectImpl*>( p_impl );
    if( fpst )
    {
        delete fpst;
    }
    else
    {
        FreeTransformAspectImpl* freet = dynamic_cast<FreeTransformAspectImpl*>(p_impl);
        if (freet)
        {
            delete freet;
        }
        else
        {
            // todo
        }
    }
}
