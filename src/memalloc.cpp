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



#include "memalloc.h"

_DECLARE_DS_LOGGER( logger, "MemAlloc", DrawSpace::Logger::Configuration::GetInstance() )

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
    _DSDEBUG( logger, dsstring( "Allocations total size : " ) << (unsigned int)m_totalsize << dsstring( " byte(s)" ) )

    long count = 1;
    for( auto& it = m_blocs.begin(); it != m_blocs.end(); ++it )
    {
        _DSDEBUG( logger, dsstring( "--> " ) << count << dsstring( "/ ptr = " ) << (*it).first << dsstring( " size = " ) << (*it).second.size << dsstring( " object = " ) << (*it).second.item.c_str() << dsstring( " in function : " ) << (*it).second.func.c_str()
            << dsstring( " " ) << (*it).second.file.c_str() << dsstring( ", line " ) << (*it).second.linenum )
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

	_DSTRACE(logger, dsstring("ptr = ") << p_ptr << dsstring(" size = ") << p_size << dsstring(" object = ") << p_item.c_str() << dsstring( " in function : " ) << p_funcname.c_str()
		<< dsstring(" ") << p_filename.c_str() << dsstring( ", line " ) << p_line)
}

void MemAlloc::Unregister( void* p_ptr )
{
    if( m_blocs.count( p_ptr ) > 0 )
    {
		_DSTRACE(logger, dsstring("ptr = ") << p_ptr << dsstring(" size = ") << m_blocs[p_ptr].size << dsstring(" object = ") << m_blocs[p_ptr].item.c_str() << dsstring( " in function : " ) << m_blocs[p_ptr].func.c_str()
			<< dsstring(" ") << m_blocs[p_ptr].file.c_str() << dsstring( ", line " ) << m_blocs[p_ptr].linenum)

        m_totalsize -= m_blocs[p_ptr].size;
        m_blocs.erase( p_ptr );
        
    }
    else
    {
		_DSWARN(logger, dsstring("no mem bloc ") << p_ptr << dsstring( "(already unallocated ?)" ) )
    }
}

DrawSpace::Logger::Sink* MemAlloc::GetLogSink( void )
{
    return &logger;
}

size_t MemAlloc::GetTotalSize( void ) const
{
    return m_totalsize;
}