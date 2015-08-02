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

#ifndef _SHADERSCONTROLLER_H_
#define _SHADERSCONTROLLER_H_

#include "renderingnode.h"

namespace DrawSpace
{
namespace Core
{
class ShadersController
{
protected:



    static ShadersController*                                           m_instance;
    //std::map<dsstring, DrawSpace::Core::RenderingNode*> m_shader_nodes;

    std::map<dsstring, std::vector<DrawSpace::Core::RenderingNode*> >   m_shader_nodes;

    ShadersController( void );

public:

    virtual ~ShadersController( void );

    static ShadersController* GetInstance( void )
    {
        if( NULL == m_instance )
        {
            m_instance = new ShadersController();
        }

        return m_instance;
    }
    
    void RegisterRenderingNode( DrawSpace::Core::RenderingNode* p_rnode );
    bool Update( const dsstring& p_id, const DrawSpace::Utils::Vector& p_value );

    void GetNodes( std::map<dsstring, std::vector<DrawSpace::Core::RenderingNode*> >& p_list );
    
};
}
}

#endif
