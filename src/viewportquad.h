/***************************************************************************
*                                                                          *
* DrawSpace Rendering engine                                               *
* Emmanuel Chaumont Copyright (c) 2013-2014                                *
*                                                                          *
* This file is part of DrawSpace.                                          *
*                                                                          *
*    DrawSpace is free software: you can redistribute it and/or modify     *
*    it under the terms of the GNU General Public License as published by  *
*    the Free Software Foundation, either version 3 of the License, or     *
*    (at your option) any later version.                                   *
*                                                                          *
*    DrawSpace is distributed in the hope that it will be useful,          *
*    but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*    GNU General Public License for more details.                          *
*                                                                          *
*    You should have received a copy of the GNU General Public License     *
*    along with DrawSpace.  If not, see <http://www.gnu.org/licenses/>.    *
*                                                                          *
***************************************************************************/

#ifndef _VIEWPORTQUAD_H_
#define _VIEWPORTQUAD_H_

#include "renderingnode.h"
#include "transformnode.h"
#include "vertex.h"
#include "triangle.h"

namespace DrawSpace
{
class ViewportQuad : public Core::TransformNode, public Core::RenderingNode
{
protected:
	dsreal				m_width;
	dsreal				m_height;
	Core::Meshe*		m_meshe;

public:
	ViewportQuad( const dsstring& p_name, dsreal p_width, dsreal p_height );
	virtual ~ViewportQuad( void );

	void OnDraw( void );

	bool LoadAssets( void );
};
}
#endif