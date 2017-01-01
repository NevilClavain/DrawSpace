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

template <typename base>
PluginManagerStatus DrawSpace::Utils::PlugInManager<base>::LoadPlugin( const char* p_path, Handle& p_handle )
{
    typename LibList::iterator it = get_lib_list()->find( p_path );
    if( it == get_lib_list()->end() )
    {
        HMODULE hMod = LoadLibraryA( p_path );
        if( NULL == hMod )
        {
            return PIM_FAIL_PILOADING;
        }
        else
        {
            PluginInfos pii;
            pii.refcount = 1;
            pii.handle = hMod;
            pii.path   = p_path;
            LibList* ll = get_lib_list();
            (*ll)[p_path] = pii;
            p_handle   = hMod;
            return PIM_OK;
        }
    }
    else
    {
        // plugin already loaded
        p_handle = it->second.handle;
        it->second.refcount++;

        return PIM_OK_PIALREADYLOADED;
    }  
    return PIM_OK;
}

template <typename base>
PluginManagerStatus DrawSpace::Utils::PlugInManager<base>::UnloadPlugin( const char* p_path )
{
    typename LibList::iterator it = get_lib_list()->find( p_path );
    if( it == get_lib_list()->end() )
    {
        return PIM_FAIL_UNKNOWN;
    }

    else
    {
        it->second.refcount--;        
        if( 0 == it->second.refcount ) // si plus aucun hub ne fait reference a ce plugin
        {
            PluginInfos pii = it->second;
            get_lib_list()->erase( it );
            FreeLibrary( pii.handle );
        }
    }
    return PIM_OK;
}

template <typename base>
PluginManagerStatus DrawSpace::Utils::PlugInManager<base>::Instanciate( Handle p_handle, base** p_inst )
{
    // TODO  : verifier que le handle fourni est bien dans m_libs; sinon retour PIM_FAIL_UNKNOWN
    FARPROC proc = GetProcAddress( p_handle, PIFACTORYSYMBOLNAME );

    if( NULL == proc )
    {
        return PIM_FAIL_FACTORYFUNCNOTFOUND;
    }
    else
    {
        Factory factory = (Factory)proc;
        base* inst = (*factory)();
        *p_inst = inst;
        return PIM_OK;
    }
    return PIM_OK;
}


