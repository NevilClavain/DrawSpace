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

#ifndef _SCENEGRAPH_H_
#define _SCENEGRAPH_H_

#include "pass.h"

namespace DrawSpace
{
class Scenegraph;

class ScenegraphNode : public Core::TransformNode
{
public:
	ScenegraphNode( const dsstring& p_name ) : TransformNode( p_name )
	{
	}
	virtual ~ScenegraphNode( void )
	{
	}

    virtual void OnRegister( Scenegraph* p_scenegraph ) = 0;
};


class Scenegraph : public Core::TransformQueue
{
protected:
	std::map<dsstring, Pass*>					m_passes;
	

public:
	Scenegraph( void );
	virtual ~Scenegraph( void );

	void RegisterPass( Pass* p_pass );
	bool RegisterNode( ScenegraphNode* p_node );	
	Pass* GetPass( const dsstring& p_passname );

};
}
#endif
