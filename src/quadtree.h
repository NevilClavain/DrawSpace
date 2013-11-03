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
    std::map<dsstring, BaseQuadtreeNode*>&          m_leafs;

public:

    BaseQuadtreeNode( std::map<dsstring, BaseQuadtreeNode*>& p_leafs, BaseQuadtreeNode* p_parent, int p_id );

    virtual ~BaseQuadtreeNode( void );

    virtual void SetParent( BaseQuadtreeNode* p_parent );
    virtual bool HasChildren( void );
    virtual BaseQuadtreeNode* GetChild( int p_id );

    virtual void Split( void ) = 0;
    virtual void Merge( void ) = 0;
};

template <typename Base>
class QuadtreeNode : public BaseQuadtreeNode
{
protected:    
    Base*       m_content;

    QuadtreeNode( std::map<dsstring, BaseQuadtreeNode*>& p_leafs, BaseQuadtreeNode* p_parent, int p_id ) : BaseQuadtreeNode( p_leafs, p_parent, p_id )
    {

    }

public:
    QuadtreeNode( std::map<dsstring, BaseQuadtreeNode*>& p_leafs, Base* p_content ) : BaseQuadtreeNode( p_leafs, NULL, RootNode ), m_content( p_content )
    {
        dsstring name;
        m_content->GetName( name );
        m_leafs[name] = this;
    }

    virtual ~QuadtreeNode( void )
    {
    }

    Base* GetContent( void )
    {
        return m_content;
    }

    virtual void Split( void )
    {
        if( m_splitted )
        {
            // deja splitte
            return;
        }

        m_children[NorthWestNode] = _DRAWSPACE_NEW_( QuadtreeNode<Base>, QuadtreeNode<Base>( m_leafs, this, NorthWestNode ) );
        m_children[NorthEastNode] = _DRAWSPACE_NEW_( QuadtreeNode<Base>, QuadtreeNode<Base>( m_leafs, this, NorthEastNode ) );
        m_children[SouthEastNode] = _DRAWSPACE_NEW_( QuadtreeNode<Base>, QuadtreeNode<Base>( m_leafs, this, SouthEastNode ) );
        m_children[SouthWestNode] = _DRAWSPACE_NEW_( QuadtreeNode<Base>, QuadtreeNode<Base>( m_leafs, this, SouthWestNode ) );

        m_splitted = true;

        // retirer celui-ci de la liste des leafs
        dsstring thisname;
        m_content->GetName( thisname );
        if( m_leafs.count( thisname ) > 0 )
        {
            m_leafs.erase( thisname );
        }


        // inscrire les fils dans la liste des leafs
        /*
        m_leafs[m_children[NorthWestNode]] = m_children[NorthWestNode];
        m_leafs[m_children[NorthEastNode]] = m_children[NorthEastNode];
        m_leafs[m_children[SouthEastNode]] = m_children[SouthEastNode];
        m_leafs[m_children[SouthWestNode]] = m_children[SouthWestNode];
        */

        m_content->Split();
    }

    virtual void Merge( void )
    {


    }
};
}
}

#endif
