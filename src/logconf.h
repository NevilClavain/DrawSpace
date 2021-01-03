/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2021                     
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

#ifndef _LOGCONF_H_
#define _LOGCONF_H_

#include "drawspace_commons.h"
#include "logoutput.h"
#include "logsink.h"
#include "parser.h"

namespace DrawSpace
{
namespace Logger
{
class Configuration : public DrawSpace::Utils::Parser
{
protected:

    typedef struct
    {
        Sink*       sink;
        bool        state;
        Sink::Level level;
        Output*     output;

    } SinkEntry;

    static Configuration*           m_instance;

    std::map<dsstring, Output*>     m_outputs;
    std::map<dsstring, SinkEntry>   m_sinks;

    LARGE_INTEGER                   m_base_tick;
    LARGE_INTEGER                   m_last_tick;
    LARGE_INTEGER                   m_freq;

    Configuration( void );
    virtual bool on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words );

public:
    ~Configuration( void );

    static Configuration* GetInstance( void );
    static void RemoveInstance( void );

    void RegisterSink( Sink* p_sink );
    void UpdateTick( void );
    LONGLONG GetLastTick( void );
   
};
}
}


#endif
