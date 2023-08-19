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

#include "logoutputfile.h"

using namespace renderMe::core;

logger::OutputFile::OutputFile( const std::string& p_filename ) :
m_flush_period( 0 ),
m_period_count( 0 )
{
    m_file = std::make_unique<File>(p_filename, File::Mode::CREATENEWTEXT);
}

void logger::OutputFile::logIt( const std::string& p_trace )
{
    m_mutex.lock();

    m_file->puts( p_trace );

    if( m_period_count == m_flush_period )
    {
        m_file->flush();
        m_period_count = 0;       
    }
    else
    {
        m_period_count++;
    }

    m_mutex.unlock();
}

/*
void logger::OutputFile::flush( void )
{
    m_mutex.lock();
    m_file->Flush();
    m_mutex.unlock();
}
*/

void logger::OutputFile::setFlushPeriod( long p_period )
{
    m_mutex.lock();
    m_flush_period = p_period;
    m_mutex.unlock();
}

