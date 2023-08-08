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

#include "ds_types.h"
#include "memalloc.h"
#include "callback.h"

namespace DrawSpace
{
namespace Utils
{
class BaseQuadtreeNode
{
public:

	static constexpr int    NorthWestNode   = 0;
	static constexpr int    NorthEastNode   = 1;
	static constexpr int    SouthEastNode   = 2;
	static constexpr int    SouthWestNode   = 3;
	static constexpr int    RootNode        = 4;

protected:
	int                                             m_id;
	BaseQuadtreeNode*                               m_children[4];
	BaseQuadtreeNode*                               m_parent;
	bool                                            m_splitted;	

public:

	BaseQuadtreeNode( BaseQuadtreeNode* p_parent, int p_id );

	virtual ~BaseQuadtreeNode( void );

	virtual void SetParent( BaseQuadtreeNode* p_parent );
    virtual BaseQuadtreeNode* GetParent( void ) const;
	virtual bool HasChildren( void );
	virtual BaseQuadtreeNode* GetChild( int p_id ) const;
    virtual int GetId( void ) const;

	virtual void Split( void ) = 0;
	virtual void Merge( void ) = 0;
    virtual void RecursiveMerge( void ) = 0;
};

template <typename Base>
class QuadtreeNode : public BaseQuadtreeNode
{
public:

	typedef Core::BaseCallback<void, BaseQuadtreeNode*> InstanciationHandler;
    typedef Core::BaseCallback<void, BaseQuadtreeNode*> DeletionHandler;
    typedef Core::BaseCallback<void, BaseQuadtreeNode*> SplitHandler;
    typedef Core::BaseCallback<void, BaseQuadtreeNode*> MergeHandler;

protected:

	Base*                                               m_content;
	InstanciationHandler*                               m_insthandler;
    DeletionHandler*                                    m_delhandler;
    SplitHandler*                                       m_splithandler;
    MergeHandler*                                       m_mergehandler;
    long                                                m_depth_level;

	QuadtreeNode( InstanciationHandler* p_insthandler, DeletionHandler* p_delhandler, SplitHandler* p_splithandler, MergeHandler* p_mergehandler, BaseQuadtreeNode* p_parent, int p_id, long p_depth_level ) : BaseQuadtreeNode( p_parent, p_id ), 
    m_insthandler( p_insthandler ), 
    m_delhandler( p_delhandler ),
    m_splithandler( p_splithandler ),
    m_mergehandler( p_mergehandler ),
    m_depth_level( p_depth_level )
	{
        (*m_insthandler)( this );
	}

public:

	QuadtreeNode( InstanciationHandler* p_insthandler, DeletionHandler* p_delhandler, SplitHandler* p_splithandler, MergeHandler* p_mergehandler ) : BaseQuadtreeNode( NULL, RootNode ), 
    m_insthandler( p_insthandler ),
    m_delhandler( p_delhandler ),
    m_splithandler( p_splithandler ),
    m_mergehandler( p_mergehandler ),
    m_depth_level( 0 )
	{
		(*m_insthandler)( this );
	}

	virtual ~QuadtreeNode( void )
	{
        (*m_delhandler)( this );
	}

	Base* GetContent( void ) const
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

		m_children[NorthWestNode] = _DRAWSPACE_NEW_( QuadtreeNode<Base>, QuadtreeNode<Base>( m_insthandler, m_delhandler, m_splithandler, m_mergehandler, this, NorthWestNode, m_depth_level + 1 ) );
		m_children[NorthEastNode] = _DRAWSPACE_NEW_( QuadtreeNode<Base>, QuadtreeNode<Base>( m_insthandler, m_delhandler, m_splithandler, m_mergehandler, this, NorthEastNode, m_depth_level + 1 ) );
		m_children[SouthEastNode] = _DRAWSPACE_NEW_( QuadtreeNode<Base>, QuadtreeNode<Base>( m_insthandler, m_delhandler, m_splithandler, m_mergehandler, this, SouthEastNode, m_depth_level + 1 ) );
		m_children[SouthWestNode] = _DRAWSPACE_NEW_( QuadtreeNode<Base>, QuadtreeNode<Base>( m_insthandler, m_delhandler, m_splithandler, m_mergehandler, this, SouthWestNode, m_depth_level + 1 ) );

		m_splitted = true;

        (*m_splithandler)( this );
	}

	virtual void Merge( void )
	{
        if( !m_splitted )
        {
            // deja merge
            return;
        }

        // il ne doit pas y avoir de fils splitte, sinon annulation du split, on ne fait rien
        if( m_children[NorthWestNode]->HasChildren() ||
            m_children[NorthEastNode]->HasChildren() ||
            m_children[SouthEastNode]->HasChildren() ||
            m_children[SouthWestNode]->HasChildren() )
        {
            return;
        }

        (*m_mergehandler)( this );

        _DRAWSPACE_DELETE_( m_children[NorthWestNode] );
        _DRAWSPACE_DELETE_( m_children[NorthEastNode] );
        _DRAWSPACE_DELETE_( m_children[SouthEastNode] );
        _DRAWSPACE_DELETE_( m_children[SouthWestNode] );

        m_splitted = false;        
	}

    virtual long GetDepthLevel( void )
    {
        return m_depth_level;
    }

    virtual void RecursiveMerge( void )
    {
        if( !m_splitted )
        {
            // leaf reached
            return;
        }

        for( int i = 0; i < 4; i++ )
        {
            m_children[i]->RecursiveMerge();
        }

        (*m_mergehandler)( this );

        for( int i = 0; i < 4; i++ )
        {
            _DRAWSPACE_DELETE_( m_children[i] );
        }

        m_splitted = false; 
    }
};
}
}

