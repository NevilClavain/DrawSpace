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

#ifndef _TRANSFORMATION_H_
#define _TRANSFORMATION_H_

#include "drawspace_commons.h"
#include "Matrix.h"
#include "scenenodegraph.h"

namespace DrawSpace
{
namespace Core
{

class Transformation
{
protected:

	std::vector<DrawSpace::Utils::Matrix>	m_matrix_chain;
    DrawSpace::Utils::Matrix	            m_result;

public:
	Transformation( void );
    Transformation( int p_nbmat );

	~Transformation( void );

	void    PushMatrix( const DrawSpace::Utils::Matrix& p_matrix );
	void    BuildResult( void );
	void	GetResult( DrawSpace::Utils::Matrix* p_res );
	void	ClearAll( void );
    void    GetMatrixChain( std::vector<DrawSpace::Utils::Matrix>& p_list );
    void    UpdateMatrix( size_t p_index, const DrawSpace::Utils::Matrix& p_mat );

    void    Update( DrawSpace::Utils::TimeManager& p_timemanager );
    void    Update2( DrawSpace::Utils::TimeManager& p_timemanager ) {};

    void    GetBaseTransform( DrawSpace::Utils::Matrix& p_mat );
    void    SetFinalTransform( const DrawSpace::Utils::Matrix& p_mat ) { };
    
    void    OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node ) { };

};
}
}

#endif
