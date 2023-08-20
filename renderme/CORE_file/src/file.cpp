
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

#include "file.h"
#include "exceptions.h"

using namespace renderMe::core;

File::File( const std::string& p_filename, Mode p_mode )
{
    if(Mode::CREATENEW == p_mode )
    {
        m_fp = fopen( p_filename.c_str(), "wb" );
    }
    else if(Mode::OPENEXISTINGB == p_mode )
    {
        m_fp = fopen( p_filename.c_str(), "rb" );
    }	
    else if(Mode::CREATENEWTEXT == p_mode )
    {
        m_fp = fopen( p_filename.c_str(), "wt" );
    }
    else if(Mode::OPENEXISTINGTEXT == p_mode )
    {
        m_fp = fopen( p_filename.c_str(), "r" );
    }

    if( !m_fp )
    {
        _EXCEPTION( std::string( "Failed to open file " ) + p_filename );
    }

}

File::File(const std::string& p_filename, const std::string& p_mode)
{
    m_fp = fopen(p_filename.c_str(), p_mode.c_str());
    if (!m_fp)
    {
        _EXCEPTION(std::string("Failed to open file ") + p_filename);
    }
}

File::~File( void )
{
    if( m_fp )
    {
        fclose( m_fp );
        m_fp = nullptr;
    }
}
void File::puts( const std::string& p_string )
{
    if( m_fp )
    {
        fputs( p_string.c_str(), m_fp );
    }
}

bool File::gets( char* p_buff, int p_nbToRead )
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
        _EXCEPTION( "File not open !" );
    }
    return false;
}

void File::flush( void )
{
    if( m_fp )
    {
        fflush( m_fp );
    }
    else
    {
        _EXCEPTION("File not open !");
    }
}


size_t File::read(void* p_buffer, size_t p_size, size_t p_count)
{
    size_t status{ 0 };

    if (m_fp)
    {
        status = ::fread(p_buffer, p_size, p_count, m_fp);
    }
    else
    {
        _EXCEPTION("File not open !");
    }

    return status;
}

size_t File::write(const void* p_buffer, size_t p_size, size_t p_count)
{
    size_t status{ 0 };

    if (m_fp)
    {
        status = ::fwrite(p_buffer, p_size, p_count, m_fp);
    }
    else
    {
        _EXCEPTION("File not open !");
    }

    return status;
}

bool File::seek(size_t p_offset, int p_origin)
{
    bool status{ false };

    if (m_fp)
    {
        // 0 -> call success
        status = (0 == fseek(m_fp, (long)p_offset, (int)p_origin) ? true : false);
    }
    else
    {
        _EXCEPTION("File not open !");
    }

    return status;
}

size_t File::tell() const
{
    size_t result{ 0 };

    if (m_fp)
    {
        result = ftell(m_fp);
    }
    else
    {
        _EXCEPTION("File not open !");
    }
    return result;
}

