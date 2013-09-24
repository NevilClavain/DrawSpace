/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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

#ifndef _AC3DMESHEIMPORT_H_
#define _AC3DMESHEIMPORT_H_

#include "mesheimport.h"
#include "parser.h"

namespace DrawSpace
{
namespace Utils
{
class AC3DMesheImport : public Interface::MesheImport, public Utils::Parser
{
protected:
    
    typedef enum
    {
        SEARCH_OBJECT_BEGIN,
        //SEARCH_OBJECT_LOC,
        SEARCH_VERT_BEGIN,
        VERT_INPUT,
        SEARCH_TRILIST_BEGIN,
        SEARCH_TRI_BEGIN,
        TRI_INPUT,
        DONE
    } State;

    Core::Meshe*                                m_meshe;
    State                                       m_state;
    long                                        m_object_index;
    long                                        m_object_count;
    Vector                                      m_object_loc;
    long                                        m_vertcount;
    long                                        m_numvert;
    long			                            m_tricount;
    long                                        m_numtri;
    long                                        m_trilinecount;
    Core::Triangle                              m_triangle;


    std::map<long, std::pair<float, float>>     m_vertices_uv_mem;

    virtual bool on_new_line( const dsstring& p_line, long p_line_num, std::vector<dsstring>& p_words );

public:
    AC3DMesheImport( void );
    virtual ~AC3DMesheImport( void );

    virtual bool LoadFromFile( const dsstring& p_filepath, long p_index, Core::Meshe* p_meshe );

};
}
}

#endif