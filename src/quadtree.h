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

#ifndef _QUADTREE_H_
#define _QUADTREE_H_

#include "drawspace_commons.h"
#include "memalloc.h"
#include "callback.h"

namespace DrawSpace
{
namespace Utils
{
class BaseQuadtreeNode
{
public:

	static const int    NorthWestNode   = 0;
	static const int    NorthEastNode   = 1;
	static const int    SouthEastNode   = 2;
	static const int    SouthWestNode   = 3;
	static const int    RootNode        = 4;

protected:
	int                                             m_id;
	BaseQuadtreeNode*                               m_children[4];
	BaseQuadtreeNode*                               m_parent;

	bool                                            m_splitted;	

public:

	BaseQuadtreeNode( BaseQuadtreeNode* p_parent, int p_id );

	virtual ~BaseQuadtreeNode( void );

	virtual void SetParent( BaseQuadtreeNode* p_parent );
    virtual BaseQuadtreeNode* GetParent( void );
	virtual bool HasChildren( void );
	virtual BaseQuadtreeNode* GetChild( int p_id );
    virtual int GetId( void );

	virtual void Split( void ) = 0;
	virtual void Merge( void ) = 0;
};

template <typename Base>
class QuadtreeNode : public BaseQuadtreeNode
{
public:

	typedef Core::BaseCallback<void, BaseQuadtreeNode*> InstanciationHandler;

protected:

	Base*                                               m_content;
	InstanciationHandler*                               m_insthandler;

	QuadtreeNode( InstanciationHandler* p_handler, BaseQuadtreeNode* p_parent, int p_id ) : BaseQuadtreeNode( p_parent, p_id ), m_insthandler( p_handler )
	{
        (*m_insthandler)( this );
		dsstring name;
		m_content->GetName( name );
	}

public:

	QuadtreeNode( InstanciationHandler* p_handler ) : BaseQuadtreeNode( NULL, RootNode ), m_insthandler( p_handler )
	{
		(*m_insthandler)( this );
		dsstring name;
		m_content->GetName( name );
	}

	virtual ~QuadtreeNode( void )
	{
	}

	Base* GetContent( void )
	{
		return m_content;
	}

	void SetContent( Base* p_content )
	{
		m_content = p_content;
	}

	virtual void Split( void )
	{
		if( m_splitted )
		{
			// deja splitte
			return;
		}

		m_children[NorthWestNode] = _DRAWSPACE_NEW_( QuadtreeNode<Base>, QuadtreeNode<Base>( m_insthandler, this, NorthWestNode ) );
		m_children[NorthEastNode] = _DRAWSPACE_NEW_( QuadtreeNode<Base>, QuadtreeNode<Base>( m_insthandler, this, NorthEastNode ) );
		m_children[SouthEastNode] = _DRAWSPACE_NEW_( QuadtreeNode<Base>, QuadtreeNode<Base>( m_insthandler, this, SouthEastNode ) );
		m_children[SouthWestNode] = _DRAWSPACE_NEW_( QuadtreeNode<Base>, QuadtreeNode<Base>( m_insthandler, this, SouthWestNode ) );

		m_splitted = true;
	}

	virtual void Merge( void )
	{


	}
};
}
}

#endif
