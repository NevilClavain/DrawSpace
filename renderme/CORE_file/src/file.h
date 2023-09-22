
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

#pragma warning( disable : 4996 4244 )

namespace renderMe
{
    namespace core
    {
        class File
        {
        public:

            enum class Mode
            {
                CREATENEW,
                OPENEXISTINGB,
                CREATENEWTEXT,
                OPENEXISTINGTEXT,
            };

            File(const std::string& p_filename, Mode p_mode);
            File(const std::string& p_filename, const std::string& p_mode);

            File() = delete;
            File(const File&) = delete;
            File(File&&) = delete;
            File& operator=(const File& t) = delete;

            ~File(void);

            void puts(const std::string& p_string);
            bool gets(char* p_buff, int p_nbToRead);

            void flush(void);
            size_t read(void* p_buffer, size_t p_size, size_t p_count);
            size_t write(const void* p_buffer, size_t p_size, size_t p_count);
            bool seek(size_t p_offset, int p_origin);
            size_t tell() const;

        private:

            FILE*           m_fp{ nullptr };
        };
    }
}

