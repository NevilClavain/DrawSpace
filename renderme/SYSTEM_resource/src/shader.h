

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


namespace renderMe
{
    class Shader
    {
    public:
        Shader() = delete;
        Shader(const std::string& p_name);
        Shader(const Shader& p_other);

        Shader& operator=(const Shader& p_other)
        {
            m_name = p_other.m_name;
            m_content = p_other.m_content;
            m_contentMD5 = p_other.m_contentMD5;
            m_contentSize = p_other.m_contentSize;
            m_code = p_other.m_code;

            m_state_mutex.lock();
            p_other.m_state_mutex.lock();
            m_state = p_other.m_state;
            p_other.m_state_mutex.unlock();
            m_state_mutex.unlock();

            return *this;
        }

        enum class State
        {
            INIT,
            BLOBLOADING,
            BLOBLOADED,
            RENDERERLOADING,
            RENDERERLOADED,
        };

        std::string getName() const;
        std::string getContent() const;
        void setContent(const std::string& p_content);
        std::string getContentMD5() const;
        void setContentMD5(const std::string& p_contentMD5);
        size_t getContentSize() const;
        void setContentSize(size_t p_contentSize);
        State getState() const;
        void setState(State p_state);

        const core::Buffer<char>& getCode() const;
        void setCode(const core::Buffer<char>& p_code);

    private:

        std::string         m_name;
        std::string         m_content;
        std::string         m_contentMD5;
        size_t              m_contentSize{ 0 };

        core::Buffer<char>  m_code;

        mutable std::mutex	m_state_mutex;
        State               m_state{ State::INIT };
    };
}

