/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2019                     
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

#include "file.h"
#include "memalloc.h"
#include "exceptions.h"


using namespace DrawSpace::Utils;

File::FSMode File::m_fsMode = File::LOCALFILESYSTEM;
dsstring File::m_virtualFsArchiveName;

File::File( const dsstring& p_filename, Mode p_mode ) : 
m_fp( NULL ),
m_vfp( NULL ),
m_current_pos( 0 )
{
    if( LOCALFILESYSTEM == m_fsMode )
    {
        if( CREATENEW == p_mode )
        {
            m_fp = fopen( p_filename.c_str(), "wb" );
        }
        else if( OPENEXISTINGB == p_mode )
        {
            m_fp = fopen( p_filename.c_str(), "rb" );
        }	
        else if( CREATENEWTEXT == p_mode )
        {
            m_fp = fopen( p_filename.c_str(), "wt" );
        }
        else if( OPENEXISTINGTEXT == p_mode )
        {
            m_fp = fopen( p_filename.c_str(), "r" );
        }

        if( !m_fp )
        {
            _DSEXCEPTION( dsstring( "Failed to open file " ) + p_filename );
        }
    }
    else // VIRTUALFILESYSTEM
    {
        if( OPENEXISTINGB == p_mode || OPENEXISTINGTEXT == p_mode )
        {
            m_vfp = PHYSFS_openRead( p_filename.c_str() );

            if( !m_vfp )
            {
                _DSEXCEPTION( dsstring( "Failed to open virtual file " ) + p_filename );
            }
        }
        else
        {
            _DSEXCEPTION( "unsupported file access on VIRTUALFILESYSTEM mode" );
        }
    }
}

File::File(const dsstring& p_filename, const dsstring& p_mode) :
m_fp(NULL),
m_vfp(NULL),
m_current_pos(0)
{
    if (LOCALFILESYSTEM == m_fsMode)
    {
        m_fp = fopen(p_filename.c_str(), p_mode.c_str());

        if (!m_fp)
        {
            _DSEXCEPTION(dsstring("Failed to open file ") + p_filename);
        }
    }
    else // VIRTUALFILESYSTEM
    {
        if ("r" == p_mode || "rb" == p_mode)
        {
            m_vfp = PHYSFS_openRead(p_filename.c_str());

            if (!m_vfp)
            {
                _DSEXCEPTION(dsstring("Failed to open virtual file ") + p_filename);
            }
        }
        else
        {
            _DSEXCEPTION("unsupported file access on VIRTUALFILESYSTEM mode");
        }
    }

}

File::~File( void )
{
    if( LOCALFILESYSTEM == m_fsMode )
    {
        if( m_fp )
        {
            fclose( m_fp );
            m_fp = NULL;
        }
    }
    else // VIRTUALFILESYSTEM
    {
        if( m_vfp )
        {
            PHYSFS_close( m_vfp );
            m_vfp = NULL;
        }    
    }
}
void File::Puts( const dsstring& p_string )
{
    if( LOCALFILESYSTEM == m_fsMode )
    {
        if( m_fp )
        {
            fputs( p_string.c_str(), m_fp );
        }
    }
    else // VIRTUALFILESYSTEM
    {
        _DSEXCEPTION( "unsupported method on VIRTUALFILESYSTEM mode" );
    }
}

bool File::Gets( char* p_buff, int p_nbToRead )
{
    if( LOCALFILESYSTEM == m_fsMode )
    {
        if( m_fp )
        {
            char* s = fgets( p_buff, p_nbToRead, m_fp );
            if( s )
            {
                return true;
            }
        }
        else
        {
            _DSEXCEPTION( "File not open !" );
        }
    }
    else // VIRTUALFILESYSTEM
    {
        if( m_vfp )
        {
            char c;
            char* buff = p_buff;
            int real_size;
            bool cont = true;
            int cc = 0;

            while( cont && cc < p_nbToRead )
            {
                real_size = PHYSFS_read( m_vfp, &c, 1, 1 );
                if( real_size > 0 && c != '\n' && c!= '\r' )
                {
                    *buff = c;
                    buff++;
                    cc++;                    
                }
                else
                {
                    cont = false;
                }
            }

            *buff = 0;

            if( 0 == real_size )
            {
                return false;
            }           
            return true;
        }
        else
        {
            _DSEXCEPTION( "Virtual file not open !" );
        }
    }
    return false;
}

void File::Flush( void )
{
    if( LOCALFILESYSTEM == m_fsMode )
    {
        if( m_fp )
        {
            fflush( m_fp );
        }
        else
        {
            _DSEXCEPTION("File not open !");
        }
    }
    else // VIRTUALFILESYSTEM
    {
        if( m_vfp )
        {
            PHYSFS_flush( m_vfp );
        }
        else
        {
            _DSEXCEPTION("Virtual file not open !");
        }
    }
}

size_t File::FileSize( void ) const
{
    if( LOCALFILESYSTEM == m_fsMode )
    {
        if( m_fp )
        {
            return fileSize( m_fp );
        }
        else
        {
            _DSEXCEPTION( "File not open !" );
        }
    }
    else // VIRTUALFILESYSTEM
    {
        if( m_vfp )
        {
            return PHYSFS_fileLength( m_vfp );
        }
        else
        {
            _DSEXCEPTION( "Virtual file not open !" );
        }
    }
    return 0;
}

long File::fileSize( FILE *p_fp )
{
    long current_pos;
    current_pos = ftell( p_fp );
    fseek( p_fp, 0, SEEK_END );
    long size = ftell( p_fp );
    fseek( p_fp, current_pos, SEEK_SET );
    return size;
}

size_t File::Read(void* p_buffer, size_t p_size, size_t p_count)
{
    size_t status = 0;
    if (LOCALFILESYSTEM == m_fsMode)
    {
        if (m_fp)
        {
            status = ::fread(p_buffer, p_size, p_count, m_fp);
        }
        else
        {
            _DSEXCEPTION("File not open !");
        }
    }
    else // VIRTUALFILESYSTEM
    {
        if (m_vfp)
        {
            status = ::PHYSFS_read(m_vfp, p_buffer, p_size, p_count);
        }
        else
        {
            _DSEXCEPTION("Virtual file not open !");
        }
    }   
    return status;
}

size_t File::Write(const void* p_buffer, size_t p_size, size_t p_count)
{
    size_t status = 0;
    if (LOCALFILESYSTEM == m_fsMode)
    {
        if (m_fp)
        {
            status = ::fwrite(p_buffer, p_size, p_count, m_fp);
        }
        else
        {
            _DSEXCEPTION("File not open !");
        }
    }
    else // VIRTUALFILESYSTEM
    {
        if (m_vfp)
        {
            status = ::PHYSFS_write(m_vfp, p_buffer, p_size, p_count);
        }
        else
        {
            _DSEXCEPTION("Virtual file not open !");
        }
    }
    return status;
}

bool File::Seek(size_t p_offset, int p_origin)
{
    if (LOCALFILESYSTEM == m_fsMode)
    {
        if (m_fp)
        {
            // 0 -> call success
            return (0 == fseek(m_fp, (long)p_offset, (int)p_origin) ? true : false);
        }
        else
        {
            _DSEXCEPTION("File not open !");
        }
    }
    else // VIRTUALFILESYSTEM
    {
        if (m_vfp)
        {
            PHYSFS_uint64 final_offset = 0;
            long fsize = PHYSFS_fileLength(m_vfp);

            switch(p_origin)
            {
                case 0: //SEEK_SET
                    final_offset = p_offset;
                    break;

                case 1: //SEEK_CUR
                    final_offset = m_current_pos + (long)p_offset;
                    break;

                case 2: //SEEK_END
                    final_offset = fsize - 1 - p_offset;
                    break;
            }

            m_current_pos = final_offset;

            // 0 -> call fail
            return (0 == PHYSFS_seek(m_vfp, final_offset) ? false : true);
        }
        else
        {
            _DSEXCEPTION("Virtual file not open !");
        }
    }
}

size_t File::Tell() const
{
    if (LOCALFILESYSTEM == m_fsMode)
    {
        if (m_fp)
        {
            return ftell(m_fp);
        }
        else
        {
            _DSEXCEPTION("File not open !");
        }
    }
    else // VIRTUALFILESYSTEM
    {
        if (m_vfp)
        {
            return PHYSFS_tell(m_vfp);
        }
        else
        {
            _DSEXCEPTION("Virtual file not open !");
        }
    }
}

void* File::LoadAndAllocBinaryFile( const dsstring& p_file, long* p_size )
{
    if( LOCALFILESYSTEM == m_fsMode )
    {
        void* ptr = NULL;
        FILE* fp;
        fp = fopen( p_file.c_str(), "rb" );
        if( fp )
        {
            unsigned long fs = fileSize( fp );
            ptr = (void*)_DRAWSPACE_NEW_EXPLICIT_SIZE_WITH_COMMENT( unsigned char, unsigned char[fs], fs, p_file);
            if( ptr )
            {
                fread( (void *)ptr, fs, 1, fp );	
                if( p_size )
                {
                    *p_size = fs;
                }
            }
            fclose( fp );
        }
        return ptr;
    }
    else // VIRTUALFILESYSTEM
    {
        void* ptr = NULL;

        PHYSFS_file* vfile = PHYSFS_openRead( p_file.c_str() );
        if( NULL == vfile )
        {
            _DSEXCEPTION( dsstring( "cannot open file from virtual filesystem : " ) + p_file );
        }
        PHYSFS_sint64 file_size = PHYSFS_fileLength( vfile );

        unsigned char *vFileBuf;
        vFileBuf = _DRAWSPACE_NEW_EXPLICIT_SIZE_WITH_COMMENT( unsigned char, unsigned char[file_size], file_size, p_file );
        int length_read = PHYSFS_read( vfile, vFileBuf, 1, file_size );

        if( length_read != file_size )
        {
            _DSEXCEPTION( dsstring( "unexpected error while reading file from virtual filesystem : " ) + p_file );
        }

        ptr = vFileBuf;
        PHYSFS_close( vfile );

        *p_size = length_read;

        return ptr;
    }
}

void File::MountVirtualFS( const dsstring& p_virtualFsArchiveName )
{
    if( m_fsMode != VIRTUALFILESYSTEM )
    {
        PHYSFS_init( NULL );

        int status = PHYSFS_mount( p_virtualFsArchiveName.c_str(), NULL, 1 );
        if( !status )
        {
            _DSEXCEPTION( dsstring( "Cannot mount virtual filesystem archive : " ) + p_virtualFsArchiveName );
        }

        m_fsMode = VIRTUALFILESYSTEM;
    }
}

void File::UnmountVirtualFS( void )
{
    if( VIRTUALFILESYSTEM == m_fsMode )
    {
        PHYSFS_deinit();
        m_fsMode = LOCALFILESYSTEM;
    }
}
