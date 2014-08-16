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

#ifndef _DEFAULTSCENE_H_
#define _DEFAULTSCENE_H_

#include "drawspace.h"

class Scene
{
protected:
    bool                                m_showfps;
    dsstring                            m_name;

    DrawSpace::Utils::TimeManager       m_timer;

public:
    Scene( lua_State* p_L );
    virtual ~Scene( void );

    int Lua_ShowFps( lua_State* p_L );

    
    virtual void Draw( void );

    void GetName( dsstring& p_name );
    void SetName( const dsstring& p_name );

    static const char className[];
    static const DrawSpace::Luna<Scene>::RegType Register[];

};

#endif