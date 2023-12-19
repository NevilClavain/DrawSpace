/* -*-LIC_BEGIN-*- */
/*
*
* renderMe grafx framework
* Emmanuel Chaumont Copyright (c) 2013-2023
*
* This file is part of renderMe.
*
*    renderMe is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    renderMe is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with renderMe.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#include "memorychunks.h"

#include "logsink.h"
#include "logconf.h"
#include "logging.h"

using namespace renderMe::core;

static logger::Sink memAllocLogger("MemAlloc", logger::Configuration::getInstance());

void MemoryChunks::dumpContent(void)
{
    _RENDERME_DEBUG(memAllocLogger, std::string("Allocations total size : ") + std::to_string(m_totalSize) + std::string (" byte(s)"))

    long count{ 1 };

    for(auto e : m_chunks)
    {
        _RENDERME_DEBUG(memAllocLogger, 
                        std::string("--> ") << count 
                                        
                        << std::string("/ ptr = ") << e.first 
                        << std::string(" size = ") << e.second.size 
            
                        << std::string(" object = ") << e.second.item.c_str() 
                        << std::string(" in function : ") << e.second.func.c_str() << std::string(" ") << e.second.file.c_str() 
                                        
                        << std::string(", line ") << e.second.linenum 
                        << std::string(" comment = [") << e.second.comment 
            
                        << std::string("]"))    
        count++;
    }

}

void MemoryChunks::unregisterChunk(void* p_ptr)
{
    if (m_chunks.count(p_ptr) > 0)
    {
        _RENDERME_TRACE(memAllocLogger, 
                        std::string("ptr = ") << p_ptr << 
                        std::string(" size = ") << m_chunks[p_ptr].size <<
                        std::string(" object = ") << m_chunks[p_ptr].item.c_str() << 
                        std::string(" in function : ") << m_chunks[p_ptr].func.c_str() << std::string(" ") << m_chunks[p_ptr].file.c_str() << 
                        std::string(", line ") << m_chunks[p_ptr].linenum)

        m_totalSize -= m_chunks.at(p_ptr).size;
        m_chunks.erase(p_ptr);
    }
    else
    {
        _RENDERME_WARN(memAllocLogger, std::string("no mem bloc ") << p_ptr << std::string("(already unallocated ?)"))
    }
}

size_t MemoryChunks::getTotalSize(void) const
{
	return m_totalSize;
}

void MemoryChunks::register_bloc(void* p_ptr, size_t p_size, const std::string& p_item, const std::string& p_funcname, long p_line, const std::string& p_filename, const std::string& p_comment)
{
    const chunk c{ p_size, p_item, p_funcname, p_line, p_filename, p_comment };
    m_chunks[p_ptr] = c;
    m_totalSize += p_size;

    _RENDERME_TRACE(memAllocLogger, std::string("ptr = ") << p_ptr << std::string(" size = ") << p_size << std::string(" object = ") << p_item.c_str() << std::string(" in function : ") << p_funcname.c_str()
        << std::string(" ") << p_filename.c_str() << std::string(", line ") << p_line)
}