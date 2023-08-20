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
#include <functional>
#include "exceptions.h"

#pragma warning( disable : 4996 )

namespace renderMe
{
    namespace core
    {
        namespace fileSystem
        {
            bool exists(const std::string& p_path);
            bool isDirectory(const std::string& p_path);
            void createDirectory(const std::string& p_path);
            long fileSize(FILE* p_fp);
        }

        template<typename T>
        class fileContent
        {
        public:

            fileContent() = delete;

            fileContent(const std::string& p_path) :
            m_path(p_path)
            {
            }

            ~fileContent() = default;

            void load(void)
            {
                static_assert(sizeof(T) == 1, "unexpected type");

                const auto fp{ ::fopen(m_path.c_str(), "rb") };
                if (fp)
                {
                    const auto fs{ fileSystem::fileSize(fp)};
                 
                    m_data.release();
                    m_data = std::make_unique<T[]>(fs * sizeof(T));
                    
                    auto dstPtr{ m_data.get() };

                    ::fread((void*)dstPtr, fs, 1, fp);
                    ::fclose(fp);

                    m_dataSize = fs;
                }
                else
                {
                    _EXCEPTION("Cannot open " + m_path);
                }
            }

            T* getData(void) const
            {
                return m_data.get();
            }

            size_t getDataSize(void) const
            {
                return m_dataSize;
            }

        private:

            std::string             m_path;
            std::unique_ptr<T[]>    m_data;
            size_t                  m_dataSize{ 0 };

        };
    }
}
