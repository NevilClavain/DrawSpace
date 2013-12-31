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

#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "renderingnode.h"
#include "transformnode.h"
#include "vertex.h"
#include "triangle.h"
#include "renderer.h"
#include "plugin.h"


namespace DrawSpace
{
class Image : public Core::RenderingNode
{
protected:
    dsreal                                  m_width;
    dsreal                                  m_height;

    long                                    m_virtual_x, m_virtual_y;
    dsreal                                  m_x, m_y;
    dsreal                                  m_scale_x, m_scale_y;
    Interface::Renderer::Characteristics    m_rc;

    void*                                   m_renderer_meshe_data;

public:

    Image( dsreal p_width, dsreal p_height );
    Image( dsreal p_width, dsreal p_height, Utils::Vector& p_uv1, Utils::Vector& p_uv2, Utils::Vector& p_uv3, Utils::Vector& p_uv4 );
    Image( long p_virtual_width, long p_virtual_height );
    Image( long p_virtual_width, long p_virtual_height, Utils::Vector& p_uv1, Utils::Vector& p_uv2, Utils::Vector& p_uv3, Utils::Vector& p_uv4 );
    virtual ~Image( void );
    
    void OnDraw( void );

    void SetTranslation( dsreal p_x, dsreal p_y );
    void SetVirtualTranslation( long p_x ,long p_y );
    void GetVirtualTranslation( long& p_x, long& p_y );
    void SetScale( dsreal p_sx, dsreal p_sy );

    void** GetRenderMesheData( void );
};
}

#endif