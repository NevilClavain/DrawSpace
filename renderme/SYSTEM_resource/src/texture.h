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
#include <mutex>

#include "buffer.h"

#include "matrix.h"

namespace renderMe
{
   
    class Texture
    {
    public:

        constexpr static size_t STAGE_0{ 0 };
        constexpr static size_t STAGE_1{ 1 };
        constexpr static size_t STAGE_2{ 2 };
        constexpr static size_t STAGE_3{ 3 };
        constexpr static size_t STAGE_4{ 4 };
        constexpr static size_t STAGE_5{ 5 };
        constexpr static size_t STAGE_6{ 6 };
        constexpr static size_t STAGE_7{ 7 };
        constexpr static size_t STAGE_8{ 8 };

        enum class Format
        {
            TEXTURE_RGB,
            TEXTURE_FLOAT,
            TEXTURE_FLOAT32,
            TEXTURE_FLOATVECTOR,
            TEXTURE_FLOATVECTOR32,
        };

        enum class State
        {
            INIT,
            BLOBLOADING,
            BLOBLOADED,
            RENDERERLOADING,
            RENDERERLOADED,
        };

        enum class Source
        {
            CONTENT_FROM_FILE,
            CONTENT_FROM_RENDERINGQUEUE
        };

        Texture() = delete;
        
        Texture::Texture(const std::string& p_filename);
        Texture(const std::string& p_name, Format p_format, size_t p_width, size_t p_height);

        Texture(const Texture& p_other);

        Texture& operator=(const Texture& p_other)
        {

            m_source = p_other.m_source;
            m_width = p_other.m_width;
            m_height = p_other.m_height;
            m_format = p_other.m_format;
            m_filecontent = p_other.m_filecontent;

            m_state_mutex.lock();
            p_other.m_state_mutex.lock();
            m_state = p_other.m_state;
            p_other.m_state_mutex.unlock();
            m_state_mutex.unlock();

            return *this;
        }

        Source getSource() const;
        size_t getWidth() const;
        size_t getHeight() const;
        Format getFormat() const;

    private:

        Source                  m_source;

        size_t                  m_width{ 0 };
        size_t                  m_height{ 0 };
        Format                  m_format;

        mutable std::mutex	    m_state_mutex;
        State                   m_state{ State::INIT };

        core::Buffer<char>      m_filecontent;

        // IF NEW MEMBERS HERE :
        // UPDATE COPY CTOR AND OPERATOR !!!!!!
    };
};