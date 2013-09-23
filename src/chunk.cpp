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

#include "chunk.h"
#include "renderer.h"
#include "plugin.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Interface;

Chunk::Chunk( const dsstring& p_name ) : TransformNode( p_name )
{
    m_meshe = _DRAWSPACE_NEW_( Meshe, Meshe );
}

Chunk::~Chunk( void )
{
	_DRAWSPACE_DELETE_( m_meshe );
}


void Chunk::SetMeshe( Core::Meshe* p_meshe )
{
    m_meshe = p_meshe;
}

Core::Meshe* Chunk::GetMeshe( void )
{
	return m_meshe;
}

bool Chunk::LoadAssets( void )
{
	Renderer* renderer = Plugin<Renderer>::GetInstance()->m_interface;

	if( false == renderer->CreateRenderingNode( this ) )
	{
		return false;
	}
	if( -1 == renderer->AddMesheToNode( m_meshe, this ) )
	{
		return false;
	}
	return true;
}

void Chunk::OnDraw( void )
{
    Renderer* renderer = Plugin<Renderer>::GetInstance()->m_interface;
    renderer->RenderNodeMeshe( m_globaltransformation, m_view, this, 0 );
}

void Chunk::Serialize( Core::Factory& p_factory, Utils::Archive& p_archive  )
{

}

void Chunk::Unserialize( Core::Factory& p_factory, Utils::Archive& p_archive )
{

}

