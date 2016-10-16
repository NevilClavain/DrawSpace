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

#include "piload.h"
#include "pimanager.h"
#include "plugin.h"
#include "renderer.h"
#include "module_root.h"

using namespace DrawSpace;
using namespace DrawSpace::Utils;

bool PILoad::LoadRendererPlugin( const dsstring& p_file )
{
	dsstring complete_path = p_file;
#ifdef _DEBUG
	complete_path += ".dll";
#else
	complete_path += "_r.dll";
#endif

    DrawSpace::Utils::PlugInManager<DrawSpace::Interface::Renderer>::Handle pihandle = NULL;
    DrawSpace::Interface::Renderer* renderer = NULL;
	PluginManagerStatus pistatus = DrawSpace::Utils::PlugInManager<DrawSpace::Interface::Renderer>::LoadPlugin( complete_path.c_str(), pihandle );
    if( pistatus != PIM_OK )
    {
        return false;
    }

    if( DrawSpace::Utils::PlugInManager<DrawSpace::Interface::Renderer>::Instanciate( pihandle, &renderer ) != PIM_OK )
    {
        return false;
    }

    DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface = renderer;
    
    return true;
}

bool PILoad::LoadModule( const dsstring& p_file, DrawSpace::Interface::Module::Root** p_module_root )
{
	dsstring complete_path = p_file;
#ifdef _DEBUG
	complete_path += ".dll";
#else
	complete_path += "_r.dll";
#endif

    DrawSpace::Utils::PlugInManager<DrawSpace::Interface::Renderer>::Handle pihandle = NULL;

    DrawSpace::Interface::Module::Root* module_root = NULL;
    PluginManagerStatus pistatus = DrawSpace::Utils::PlugInManager<DrawSpace::Interface::Module::Root>::LoadPlugin( complete_path.c_str(), pihandle );
    if( pistatus != PIM_OK )
    {
        return false;
    }

    if( DrawSpace::Utils::PlugInManager<DrawSpace::Interface::Module::Root>::Instanciate( pihandle, &module_root ) != PIM_OK )
    {
        return false;
    }

    //DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Module::Root>::GetInstance()->m_interface = module_root;

    *p_module_root = module_root;

    module_root->UpdateRenderer( DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface );
    return true;
}