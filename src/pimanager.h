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


#ifndef _PIMANAGER_H_
#define _PIMANAGER_H_

#include "drawspace_commons.h"

typedef enum
{
    PIM_OK,
    PIM_OK_PIALREADYLOADED,
    PIM_FAIL_PILOADING,
    PIM_FAIL_PIUNLOADING,
    PIM_FAIL_UNKNOWN,
    PIM_FAIL_ENTRYPOINTNOTFOUND,

} PluginManagerStatus;

#define PIFACTORYSYMBOLNAME "PIFactory"
#define PITRASHSYMBOLNAME   "PITrash"

namespace DrawSpace
{
namespace Utils
{
template <typename base>
class PlugInManager
{
public:
    typedef HMODULE Handle;

private:
    typedef struct
    {
        Handle          handle;
        dsstring        path;
        long            refcount;

    } PluginInfos;

    typedef base* (* Factory)( void );
    typedef void  (* Trash)( base* );

    typedef std::map<std::string, PluginInfos> LibList;

    PlugInManager( void )  { };
    ~PlugInManager( void ) { };

    static LibList* get_lib_list()
    {
        static LibList m_libs;
        return &m_libs;
    }

public:
    static PluginManagerStatus LoadPlugin( const char* p_path, Handle& p_handle );
    static PluginManagerStatus UnloadPlugin( const char* p_path );
    static PluginManagerStatus Instanciate( Handle p_handle, base** p_inst );
    static PluginManagerStatus TrashInstance( const char* p_path, base* p_inst );
};
}
}

#include "PIManager_impl.h"

#endif


