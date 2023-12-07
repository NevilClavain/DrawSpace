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
#include <memory>
#include <utility> 
#include <functional>
#include <filesystem>
#include "exceptions.h"
#include "buffer.h"

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
            std::pair<std::string, std::string> splitFilename(const std::string& p_filename);
            std::pair<std::string, std::string> splitPath(const std::string& p_path);
        }

        template<typename T>
        class FileContent
        {
        public:

            FileContent() = delete;
            FileContent(const FileContent&) = delete;
            FileContent(FileContent&&) = delete;
            FileContent& operator=(const FileContent& t) = delete;

            FileContent(const std::string& p_path) :
            m_path(p_path)
            {
            }

            ~FileContent() = default;

            void load(void)
            {
                const auto fp{ ::fopen(m_path.c_str(), "rb") };
                if (fp)
                {
                    const auto fs{ fileSystem::fileSize(fp)};
                 
                    m_data.release();
                    m_data = std::make_unique<T[]>(fs * sizeof(T));
                    
                    auto dstPtr{ m_data.get() };

                    ::fread((void*)dstPtr, sizeof(T), fs, fp);
                    ::fclose(fp);

                    m_dataSize = fs;
                }
                else
                {
                    _EXCEPTION("Cannot open " + m_path);
                }
            }

            void save(T* p_buffer, size_t p_buffersize) const
            {
                const auto fp{ ::fopen(m_path.c_str(), "wb") };
                if (fp)
                {
                    ::fwrite(p_buffer, p_buffersize, sizeof(T), fp);
                    ::fclose(fp);
                }
                else
                {
                    _EXCEPTION("Cannot create " + m_path);
                }
            }

            void save(const Buffer<T>& p_buffer) const
            {
                const auto fp{ ::fopen(m_path.c_str(), "wb") };
                if (fp)
                {
                    if (!p_buffer.isEmpty())
                    {
                        ::fwrite(p_buffer.getData(), p_buffer.getDataSize(), sizeof(T), fp);
                    }
                    ::fclose(fp);
                }
                else
                {
                    _EXCEPTION("Cannot create " + m_path);
                }
            }

            void save() const
            {
                const auto fp{ ::fopen(m_path.c_str(), "wb") };
                if (fp)
                {
                    if (!isEmpty())
                    {
                        ::fwrite(m_data.get(), m_dataSize, sizeof(T), fp);
                    }
                    ::fclose(fp);
                }
                else
                {
                    _EXCEPTION("Cannot create " + m_path);
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

            std::string getPath() const
            {
                return m_path;
            }

            bool isEmpty() const
            {
                return (m_data.get() == nullptr);
            }

            void cloneDataTo(Buffer<T>& p_buffer)
            {
                p_buffer.fill(m_data.get(), m_dataSize);
            }

            void cloneDataFrom(const Buffer<T>& p_buffer)
            {
                // TODO
            }


        private:

            std::string             m_path;
            std::unique_ptr<T[]>    m_data;
            size_t                  m_dataSize{ 0 };

        };
    }
}
