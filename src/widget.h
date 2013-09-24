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

#include "drawspace_commons.h"
#include "image.h"
#include "text.h"
#include "callback.h"
#include "renderingnode.h"

namespace DrawSpace
{
namespace Gui
{
class Widget
{
protected:
    dsstring                m_name;

    Widget*                 m_parent;
    std::vector<Widget*>    m_children;

public:
    Widget( const dsstring& p_name );
    virtual ~Widget( void );

    void AddChild( Widget* p_widget );

    virtual void MouseMoveInput( long p_xm, long p_ym, long p_dx, long p_dy );
    virtual void MouseLeftButtonDownInput( long p_xm, long p_ym );
    virtual void MouseLeftButtonUpInput( long p_xm, long p_ym );
    virtual void MouseRightButtonDownInput( long p_xm, long p_ym );
    virtual void MouseRightButtonUpInput( long p_xm, long p_ym );

};
}
}