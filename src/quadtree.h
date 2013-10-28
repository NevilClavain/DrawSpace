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

namespace DrawSpace
{
namespace Utils
{
class BaseQuadtreeNode
{
public:

    static const int    NorthWestNode = 0;
    static const int    NorthEastNode = 1;
    static const int    SouthEastNode = 2;
    static const int    SouthWestNode = 3;
    static const int    RootNode = 4;

protected:
    int                                             m_id;
    BaseQuadtreeNode*                               m_children[4];
    BaseQuadtreeNode*                               m_parent;

    bool                                            m_splitted;
    std::map<BaseQuadtreeNode*, BaseQuadtreeNode*>& m_leafs;

    BaseQuadtreeNode( std::map<BaseQuadtreeNode*, BaseQuadtreeNode*>& p_leafs, BaseQuadtreeNode* p_parent, int p_id );
public:

    BaseQuadtreeNode( std::map<BaseQuadtreeNode*, BaseQuadtreeNode*>& p_leafs );    
    virtual ~BaseQuadtreeNode( void );

    virtual void SetParent( BaseQuadtreeNode* p_parent );
    virtual void Split( void );
    virtual void Merge( void );
    bool HasChildren( void );
    BaseQuadtreeNode* GetChild( int p_id );


};

template <typename Base>
class QuadtreeNode : public BaseQuadtreeNode
{
protected:    
    Base*       m_content;

public:
    QuadtreeNode( std::map<BaseQuadtreeNode*, BaseQuadtreeNode*>& p_leafs ) : BaseQuadtreeNode( p_leafs )
    {
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
};
}
}

#endif
