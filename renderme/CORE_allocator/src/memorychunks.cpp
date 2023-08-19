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

using namespace renderMe::core;

void MemoryChunks::dumpContent(void)
{
	//TODO : log
}

void MemoryChunks::unregister(void* p_ptr)
{
    if (m_chunks.count(p_ptr) > 0)
    {
        //TODO : log

        m_totalSize -= m_chunks.at(p_ptr).size;
        m_chunks.erase(p_ptr);
    }
    else
    {
        // TODO : log a warning
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
}