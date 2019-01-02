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

#include "logoutputfile.h"

using namespace DrawSpace;
using namespace DrawSpace::Utils;

Logger::OutputFile::OutputFile( const dsstring& p_filename ) :
m_flush_period( 0 ),
m_period_count( 0 )
{
    m_file = new File( p_filename, File::CREATENEWTEXT );
}

Logger::OutputFile::~OutputFile( void )
{
    delete m_file;
}

void Logger::OutputFile::LogIt( const dsstring& p_trace )
{
    m_file->Puts( p_trace );

    if( m_period_count == m_flush_period )
    {
        Flush();
        m_period_count = 0;
        return;
    }
    m_period_count++;
}

void Logger::OutputFile::Flush( void )
{
    m_file->Flush();
}

void Logger::OutputFile::SetFlushPeriod( long p_period )
{
    m_flush_period = p_period;
}

