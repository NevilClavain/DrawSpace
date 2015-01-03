/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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

#include "memalloc.h"
#include "tracedefs.h"
_DECLARE_DS_LOGGER( logger, "MemAlloc" )

using namespace DrawSpace;
using namespace DrawSpace::Utils;

MemAlloc* MemAlloc::m_instance = NULL;

MemAlloc::MemAlloc( void ) : m_totalsize( 0 )
{
}

MemAlloc::~MemAlloc( void )
{
}

MemAlloc* MemAlloc::GetInstance( void )
{
    if( !m_instance )
    {
        m_instance = new MemAlloc();
    }
    return m_instance;
}

void MemAlloc::DumpContent( void )
{
    _DSDEBUG( logger, "Allocations total size : " << (unsigned int)m_totalsize << " byte(s)" )

    long count = 1;
    for( std::map<void*, entry>::iterator it = m_blocs.begin(); it != m_blocs.end(); ++it )
    {
        _DSDEBUG( logger, "--> " << count << "/ ptr = " << (*it).first << " size = " << (*it).second.size << " object = " << (*it).second.item.c_str() << " in function : " << (*it).second.func.c_str()
            << " " << (*it).second.file.c_str() << ", line " << (*it).second.linenum )

        count++;
    }
}

void MemAlloc::register_bloc( void* p_ptr, size_t p_size, const std::string& p_item, const std::string& p_funcname, long p_line, const std::string& p_filename )
{
    entry e;
    e.file = p_filename;
    e.func = p_funcname;
    e.item = p_item;
    e.linenum = p_line;
    e.size = p_size;
    m_blocs[p_ptr] = e;

    m_totalsize += p_size;

    _DSTRACE( logger, "ptr = " << p_ptr << " size = " << p_size << " object = " << p_item.c_str() << " in function : " << p_funcname.c_str()
                    << " " << p_filename.c_str() << ", line " << p_line )
}

void MemAlloc::Unregister( void* p_ptr )
{
    if( m_blocs.count( p_ptr ) > 0 )
    {
        _DSTRACE( logger, "ptr = " << p_ptr << " size = " << m_blocs[p_ptr].size << " object = " << m_blocs[p_ptr].item.c_str() << " in function : " << m_blocs[p_ptr].func.c_str()
                    << " " << m_blocs[p_ptr].file.c_str() << ", line " << m_blocs[p_ptr].linenum )

        m_totalsize -= m_blocs[p_ptr].size;
        m_blocs.erase( p_ptr );
        
    }
    else
    {
        _DSWARN( logger, "no mem bloc " << p_ptr << "(already unallocated ?)" )
    }
}
