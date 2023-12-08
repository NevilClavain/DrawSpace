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
#include <memory>

namespace renderMe
{
    namespace core
    {
        template<typename T>
        class Buffer
        {
        public:

            Buffer() = default;

            Buffer(const Buffer& p_other)
            {
                if (p_other.isEmpty())
                {
                    return;
                }

                m_data.release();
                m_data = std::make_unique<T[]>(p_other.m_dataSize);
                memcpy((void*)m_data.get(), p_other.m_data.get(), p_other.m_dataSize * sizeof(T));
                m_dataSize = p_other.m_dataSize;
            }            
            Buffer(Buffer&&) = delete;
            
            Buffer& operator=(const Buffer& p_other)
            {
                if (p_other.isEmpty())
                {
                    return *this;
                }
                m_data.release();
                m_data = std::make_unique<T[]>(p_other.m_dataSize);
                memcpy((void*)m_data.get(), p_other.m_data.get(), p_other.m_dataSize * sizeof(T));
                m_dataSize = p_other.m_dataSize;

                return *this;
            }

            ~Buffer() = default;

            T* getData(void) const
            {
                return m_data.get();
            }

            size_t getDataSize(void) const
            {
                return m_dataSize;
            }

            bool isEmpty() const
            {
                return (m_data.get()==nullptr);
            }

            void fill(T* p_buffer, size_t p_bufferSize)
            {
                m_data.release();
                m_data = std::make_unique<T[]>(p_bufferSize);
                memcpy((void*)m_data.get(), p_buffer, p_bufferSize * sizeof(T));
                m_dataSize = p_bufferSize;
            }


        private:
            std::unique_ptr<T[]>    m_data;
            size_t                  m_dataSize { 0 };
        };
    }
}
