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
#include "textures_service.h"




//fwd decl
class D3D11SystemImpl;

namespace renderMe
{
    //fwd decl
    class ResourceSystem;
    class D3D11System;

    namespace rendering
    {
        struct Queue;
    }

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

        enum class ContentAccessMode
        {
            NO_CONTENT_ACCESS,
            CONTENT_ACCESS
            /*
            CONTENT_ACCESS_READ, 
            CONTENT_ACCESS_WRITE,
            CONTENT_ACCESS_READWRITE
            */
        };

        Texture() = default;
        Texture(Format p_format, size_t p_width, size_t p_height, ContentAccessMode p_content_access_mode = ContentAccessMode::NO_CONTENT_ACCESS);

        Texture(const Texture& p_other);

        Texture& operator=(const Texture& p_other)
        {
            m_source = p_other.m_source;
            m_source_id = p_other.m_source_id;
            m_resource_uid = p_other.m_resource_uid;
            m_width = p_other.m_width;
            m_height = p_other.m_height;
            m_format = p_other.m_format;
            m_file_content = p_other.m_file_content;
            m_content_access_mode = p_other.m_content_access_mode;

            m_state_mutex.lock();
            p_other.m_state_mutex.lock();
            m_state = p_other.m_state;
            p_other.m_state_mutex.unlock();
            m_state_mutex.unlock();

            return *this;
        }

        Source                              getSource() const;

        int                                 getWidth() const;
        int                                 getHeight() const;
        Format                              getFormat() const;

        State                               getState() const;

        ContentAccessMode                   getContentAccessMode() const;

        const core::Buffer<unsigned char>&  getFileContent() const;

        std::string                         getResourceUID() const;

        std::string                         getSourceID() const;
        
        template<typename T>
        void                                getTextureContent(renderMe::core::Buffer<T>& p_destbuffer);

    private:

        std::string                         m_resource_uid;       // texture content source unique identifier

        Source                              m_source            { Source::CONTENT_FROM_FILE };

        std::string                         m_source_id;       // texture content source unique identifier


        core::Buffer<unsigned char>         m_file_content;

        int                                 m_width             { 0 };
        int                                 m_height            { 0 };
        Format                              m_format            { Format::TEXTURE_RGB };

        ContentAccessMode                   m_content_access_mode{ ContentAccessMode::NO_CONTENT_ACCESS };

        mutable std::mutex	                m_state_mutex;
        State                               m_state             { State::INIT };
       
        // IF NEW MEMBERS HERE :
        // UPDATE COPY CTOR AND OPERATOR !!!!!!

        void setState(Texture::State p_state);
        void compute_resource_uid();
        
        friend class renderMe::ResourceSystem;
        friend class renderMe::D3D11System;
        friend class D3D11SystemImpl;

        friend struct renderMe::rendering::Queue;
    };

    template<typename T>
    void Texture::getTextureContent(renderMe::core::Buffer<T>& p_destbuffer)
    {
        void* buffer;
        size_t bufferSize;

        core::services::TextureContentCopyService::getInstance()->readTextureContent(m_resource_uid, &buffer, &bufferSize);
        if (p_destbuffer.isEmpty())
        {
            p_destbuffer.fill(static_cast<T*>(buffer), m_width * m_height);
        }
        else
        {
            p_destbuffer.update(static_cast<T*>(buffer));
        }
    }
};