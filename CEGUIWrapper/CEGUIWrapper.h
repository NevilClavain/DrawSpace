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

#include <CEGUI\CEGUI.h>
#include "drawspace_commons.h"

#ifndef _CEGUIWRAPPER_H_
#define _CEGUIWRAPPER_H_

class CEGUIWrapper
{
protected:

    bool            m_ready;
public:
    CEGUIWrapper( void );
    ~CEGUIWrapper( void );

    void SetReady( void );
    void RenderGUI( void );

    void SetResourcesRootDirectory( const dsstring& p_path );

    void OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy );
    void OnMouseLeftButtonDown( long p_xm, long p_ym );
    void OnMouseLeftButtonUp( long p_xm, long p_ym );
    void OnMouseRightButtonDown( long p_xm, long p_ym );
    void OnMouseRightButtonUp( long p_xm, long p_ym );


    // temporaire, pour tests divers :-D
    void InitTest( void );
};

#endif
