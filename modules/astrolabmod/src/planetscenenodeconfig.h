/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2017
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

#ifndef _PLANETSCENENODECONFIG_H_
#define _PLANETSCENENODECONFIG_H_

#include "drawspace.h"

class PlanetSceneNodeConfig
{
public:

	//////////////////////////////////////////////////////////////////

	DrawSpace::Module::KeySource<dsstring>          m_planetName;
	DrawSpace::Module::KeySource<dsreal>            m_planetRay;
	DrawSpace::Module::KeySource<dsstring>			m_detailsVertexShader;
	DrawSpace::Module::KeySource<dsstring>			m_detailsPixelShader;

	//////////////////////////////////////////////////////////////////

	DrawSpace::Module::KeysLinkTable*               m_keylinksTable;

public:

	PlanetSceneNodeConfig(void) :
		m_planetName("planetName"),
		m_planetRay("planetRay"),
		m_detailsVertexShader("detailsVertexShader"),
		m_detailsPixelShader("detailsPixelShader")
	{
	}
};

#endif