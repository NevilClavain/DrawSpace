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

#ifndef _SCENENODE_H_
#define _SCENENODE_H_

#include "drawspace_commons.h"
#include "matrix.h"
#include "vsphere.h"
#include "lod.h"
#include "timemanager.h"

namespace DrawSpace
{
namespace Core
{
class BaseSceneNode
{
protected:
    
    dsstring                    m_scenename;
    std::vector<BaseSceneNode*> m_children;
    DrawSpace::Utils::Matrix    m_finaltransform;

    
public:
    virtual void GetSceneName( dsstring& p_scenename )
    {
        p_scenename = m_scenename;
    }

    virtual void GetFinalTransform( DrawSpace::Utils::Matrix& p_mat )
    {
        p_mat = m_finaltransform;
    }

    virtual void ComputeTransformation( void ) = 0;

    virtual void AddChild( BaseSceneNode* p_node )
    {
        m_children.push_back( p_node );
    }
    
    friend class SceneNodeGraph;
    
};

template <typename Base>
class SceneNode : public BaseSceneNode
{
protected:

    
    BaseSceneNode*              m_parent;    
    DrawSpace::Utils::Matrix    m_globaltransform;
    Base*                       m_content;

    bool                        m_detachable;

    virtual void ComputeTransformation( void )
    {
        if( !m_content )
        {
            return;
        }

        Matrix base_mat;
        m_content->GetBaseTransform( base_mat );
       
        if( m_parent )
        {
            Matrix parent_final_transform;
            m_parent->GetFinalTransform( parent_final_transform );

            m_finaltransform = base_mat * parent_final_transform;
        }
        else
        {
            m_finaltransform = base_mat;
        }
        m_content->SetFinalTransform( m_finaltransform );

        // Update() children
        for( size_t i = 0; i < m_children.size(); i++ )
        {
            m_children[i]->ComputeTransformation();
        }
    }

public:

    SceneNode( void ) :
    m_content( NULL ),
    m_detachable( false )    
    {
    }

    virtual ~SceneNode( void )
    {
    }

    virtual void SetContent( Base* p_content )
	{
		m_content = p_content;
	}

    virtual void LinkTo( BaseSceneNode* p_node )
    {
        p_node->AddChild( this );
        m_parent = p_node;
    }

    virtual Base* GetContent( void )
    {
        return m_content;
    }

    friend class SceneNodeGraph;
};

}
}
#endif