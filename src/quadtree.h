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
protected:
    int                 m_id;
    BaseQuadtreeNode*   m_children[4];
    BaseQuadtreeNode*   m_parent;

public:
    BaseQuadtreeNode( void );
    BaseQuadtreeNode( BaseQuadtreeNode* p_parent );

    virtual ~BaseQuadtreeNode( void );

    virtual void SetParent( BaseQuadtreeNode* p_parent );
    virtual void Split( void );
    virtual void Merge( void );
    bool HasChildren( void );

};

template <typename Base>
class QuadtreeNode : public BaseQuadtreeNode
{
protected:    
    Base*       m_content;

public:
    QuadtreeNode( void )
    {
    }

    QuadtreeNode( BaseQuadtreeNode* p_parent ) : BaseQuadtreeNode( p_parent )
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
