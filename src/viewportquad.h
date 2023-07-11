/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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

#pragma once

#include "renderingnode.h"
#include "primitives.h"

namespace DrawSpace
{
class ViewportQuad : public Core::RenderingNode
{
protected:
    dsreal                                  m_width;
    dsreal                                  m_height;

    void*                                   m_renderer_meshe_data;

    DrawSpace::Utils::Matrix                m_projection;

    dsreal                                  m_z_offset;

public:
    ViewportQuad( dsreal p_width, dsreal p_height, dsreal p_z_offset = 0.0 );
    virtual ~ViewportQuad( void );
    void OnDraw( void );
};
}


