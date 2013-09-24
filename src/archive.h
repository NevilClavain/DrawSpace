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

#ifndef _ARCHIVE_H_
#define _ARCHIVE_H_

#include "drawspace_commons.h"

namespace DrawSpace
{
namespace Utils
{

class Archive
{
private:
    long            m_total_length;
    long            m_current_length;
    unsigned char*  m_data;
    unsigned char*  m_current;

public:	

    Archive( void );
    //constructeur par copie
    Archive( Archive &p_tocopy );
    ~Archive( void );

    void SetArchiveTotalLength( unsigned long p_len );
    long GetTotalLength( void );
    long GetCurrentLength( void );
    void Rewind( void );

    void operator <<( long p_dword );
    void operator <<( dsreal p_real );
    void operator <<( dsstring p_string );
    void operator <<( Archive& p_arc );
   
    void operator >>( long& p_dword );
    void operator >>( dsreal& p_real );
    void operator >>( dsstring& p_string );

    unsigned char operator[]( unsigned long p_index );

    unsigned char* GetCurrentPtr( void );

    friend class File;
};
}
}
#endif