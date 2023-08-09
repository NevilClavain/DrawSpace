/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
*
* This file is part of DrawSpace.
*
*    DrawSpace is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    DrawSpace is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with DrawSpace.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#pragma once

#include "ds_types.h"
#include "Physfs.h"

namespace DrawSpace
{
    class File
    {
    public:
        enum class FSMode
        {
            LOCALFILESYSTEM,
            VIRTUALFILESYSTEM,
        };


        enum class Mode
        {
            CREATENEW,
            OPENEXISTINGB,
            CREATENEWTEXT,
            OPENEXISTINGTEXT,
        };

        File( const dsstring& p_filename, Mode p_mode );
        File( const dsstring& p_filename, const dsstring& p_mode );
        ~File( void );

        static void mountVirtualFS( const dsstring& p_virtualFsArchiveName );
        static void unmountVirtualFS( void );

        size_t fileSize( void ) const;

        void puts( const dsstring& p_string );
        bool gets( char* p_buff, int p_nbToRead );

        void flush( void );
        size_t read(void* p_buffer, size_t p_size, size_t p_count);
        size_t write(const void* p_buffer, size_t p_size, size_t p_count);
        bool seek(size_t p_offset, int p_origin);
        size_t tell() const;

        static void* loadAndAllocBinaryFile( const dsstring& p_file, long* p_size );

    private:

        FILE*               m_fp{ nullptr };
        PHYSFS_file*        m_vfp{ nullptr };

        int                 m_current_pos{ 0 }; // used for VIRTUALFILESYSTEM only

        static FSMode       m_fsMode;
        static dsstring     m_virtualFsArchiveName;

        static long	        fileSize(FILE* p_fp);
    };
}

