/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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

#ifndef _TRANSFORMQUEUE_H_
#define _TRANSFORMQUEUE_H_

#include "drawspace_commons.h"
#include "transformnode.h"

namespace DrawSpace
{
namespace Core
{
class TransformQueue
{
protected:
    std::map<dsstring, TransformNode*>  m_nodes;

    bool add( TransformNode* p_node );

public:
    TransformQueue( void );
    ~TransformQueue( void );

    void ComputeTransformations( Utils::TimeManager& p_timemanager );    
    bool SetNodeLocalTransformation( const dsstring& p_nodename, const DrawSpace::Utils::Matrix& p_mat );
    bool GetNodeGlobalTransform( const dsstring& p_nodename, DrawSpace::Utils::Matrix& p_mat );

    std::map<dsstring, TransformNode*>& GetNodesList( void ); 

};
}
}

#endif
