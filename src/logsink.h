/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2020                     
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



#ifndef _LOGSINK_H_
#define _LOGSINK_H_

#include "drawspace_commons.h"
#include "logoutput.h"

namespace DrawSpace
{
namespace Logger
{
class Configuration;

class Sink
{
public:

    typedef enum
    {
        LEVEL_FATAL,
        LEVEL_ERROR,
        LEVEL_WARN,
        LEVEL_DEBUG,
        LEVEL_TRACE,

    } Level;

protected:

    Level                   m_current_level;
    bool                    m_state;

    Output*                 m_output;

    dsstring                m_name;

    Logger::Configuration*  m_conf;

public:
    Sink( const dsstring& p_name, Configuration* p_conf );
    ~Sink( void );

    void SetCurrentLevel( Level p_level );
    void SetState( bool p_state );

    void LogIt( Level p_level, const dsstring& p_trace );

    void RegisterOutput( Output* p_output );

    void GetName( dsstring& p_name ) const;
    void SetConfiguration( Logger::Configuration* p_conf );
      
};
}
}


#endif
