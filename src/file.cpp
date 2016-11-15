/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2016                              
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
    
    }
}

void File::SaveArchive( Archive& p_arc )
{
    if( LOCALFILESYSTEM == m_fsMode )
    {
        if( m_fp )
        {
            fwrite( p_arc.m_data, p_arc.m_total_length, 1, m_fp );	
        }
    }
    else // VIRTUALFILESYSTEM
    {
        _DSEXCEPTION( "unsupported method on VIRTUALFILESYSTEM mode" );
    }
}

bool File::LoadArchive( Archive& p_arc )
{
    if( LOCALFILESYSTEM == m_fsMode )
    {
        if( m_fp )
        {
            unsigned long fsize = fileSize( m_fp ); //obtenir taille du fichier

            p_arc.SetArchiveTotalLength( fsize );
            fread( p_arc.GetCurrentPtr(), fsize, 1, m_fp );
            return true;
        }
        else
        {
            return false;
        }
    }
    else // VIRTUALFILESYSTEM
    {
        _DSEXCEPTION( "unsupported method on VIRTUALFILESYSTEM mode" );
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
    }
    else // VIRTUALFILESYSTEM
    {
    
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
    }
    else // VIRTUALFILESYSTEM
    {

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
