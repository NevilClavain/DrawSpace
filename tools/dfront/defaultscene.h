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

#include "scene.h"
#include "drawspace.h"

class DefaultScene : public IScene
{
protected:

    DrawSpace::Utils::TimeManager       m_timer;

public:
    DefaultScene( void );
    virtual ~DefaultScene( void );
    
    virtual void Draw( void );
    virtual bool IsBrowsable( void );
    virtual bool IsLuaScriptRecipient( void );
};

#endif