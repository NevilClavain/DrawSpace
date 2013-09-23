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

#ifndef _RENDERINGNODE_H_
#define _RENDERINGNODE_H_

#include "drawspace_commons.h"
#include "matrix.h"
#include "fx.h"
#include "texture.h"
#include "meshe.h"
#include "callback.h"

namespace DrawSpace
{
namespace Core
{
class RenderingNode
{
protected:
    Fx*										m_fx;
 	Texture*								m_textures[32]; // 32 textures stages max

    long									m_order;

	BaseCallback<void, RenderingNode*>*		m_handler;


public:
	RenderingNode( void );
	virtual ~RenderingNode( void );

    virtual void SetFx( Fx* p_fx );
 	virtual void SetTexture( Texture* p_texture, long p_stage );

	virtual Core::Fx* GetFx( void )
	{
		return m_fx;
	}

	virtual long GetTextureListSize( void );
	virtual Core::Texture* GetTexture( long p_index );    
    virtual void OnDraw( void );
	virtual void RegisterHandler( BaseCallback<void, RenderingNode*>* p_handler );

    friend class RenderingQueue;
};
}
}

#endif