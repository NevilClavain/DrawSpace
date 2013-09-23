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

#ifndef _CHUNK_H_
#define _CHUNK_H_

#include "renderingnode.h"
#include "transformnode.h"
#include "asset.h"

namespace DrawSpace
{

class Chunk : public Asset, public Core::TransformNode, public Core::RenderingNode
{
protected:
    Core::Meshe* m_meshe;
        
public:
    Chunk( const dsstring& p_name );
    ~Chunk( void );

    void SetMeshe( Core::Meshe* p_meshe );
    Core::Meshe* GetMeshe( void );

	void Serialize( Core::Factory& p_factory, Utils::Archive& p_archive  );
	void Unserialize( Core::Factory& p_factory, Utils::Archive& p_archive );

	bool LoadAssets( void );
    void OnDraw( void );
};
}

#endif