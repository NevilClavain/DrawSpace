/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2017                        
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

#include "file.h"
#include "memalloc.h"
#include "exceptions.h"


using namespace DrawSpace::Utils;

File::FSMode File::m_fsMode = File::LOCALFILESYSTEM;
dsstring File::m_virtualFsArchiveName;

File::File( const dsstring& p_filename, Mode p_mode ) : 
m_fp( NULL ),
m_vfp( NULL )
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
            

            /*
            char* buff = p_buff;
            int real_size = PHYSFS_read( m_vfp, buff, 1, p_nbToRead );

            if( 0 == real_size )
            {
                return false;
            }

            // emuler le comportement du fgets : il faut donc tronquer quand une fin de ligne est rencontree
            for( int i = 0; i < real_size; i++ )
            {
                if( '\n' == buff[i] || '\r' == buff[i] )
                {
                    buff[i] = 0x00;
                    return true;
                }
            }
            */
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
    }
    else // VIRTUALFILESYSTEM
    {
        _DSEXCEPTION( "unsupported method on VIRTUALFILESYSTEM mode" );
    }
}

long File::FileSize( void )
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
            ptr = (void*)_DRAWSPACE_NEW_EXPLICIT_SIZE_( unsigned char, unsigned char[fs], fs );
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
        vFileBuf = _DRAWSPACE_NEW_EXPLICIT_SIZE_( unsigned char, unsigned char[file_size], file_size );
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
