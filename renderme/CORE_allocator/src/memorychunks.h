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

#pragma once

#include <string>
#include <unordered_map>

#include "singleton.h"

namespace renderMe
{
    namespace core
    {
        class MemoryChunks : public property::Singleton<MemoryChunks>
        {
        public:

            MemoryChunks(void) = default;
            ~MemoryChunks() = default;

            void dumpContent(void);

            template <typename base>
            base* registerChunk(base* p_ptr, size_t p_size, const std::string& p_item, const std::string& p_funcname, long p_line, const std::string& p_filename, const std::string& p_comment = "")
            {
                base* t = p_ptr;
                register_bloc(t, p_size, p_item, p_funcname, p_line, p_filename, p_comment);

                return t;
            };

            void    unregisterChunk(void* p_ptr);
            size_t  getTotalSize(void) const;


        private:

            struct chunk
            {
                size_t          size;
                std::string     item;
                std::string     func;
                long            linenum;
                std::string     file;
                std::string     comment;
            };

            std::unordered_map<void*, chunk>  m_chunks;
            size_t                  m_totalSize{ 0 };
           
            void register_bloc(void* p_ptr, size_t p_size, const std::string& p_item, const std::string& p_funcname, long p_line, const std::string& p_filename, const std::string& p_comment);

        };
    }
}
