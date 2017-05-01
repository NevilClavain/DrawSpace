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


#ifndef _RENDERSTATESSET_H_
#define _RENDERSTATESSET_H_

#include "renderstate.h"

namespace DrawSpace
{
namespace Core
{

class RenderStatesSet
{
protected:

    static dsstring                 m_rootpath;
    dsstring                        m_path;

    std::vector<RenderState>        m_renderstates_in;
    std::vector<RenderState>        m_renderstates_out;

    dsstring                        compute_final_path( void );

public:

    RenderStatesSet( void );
    RenderStatesSet( const dsstring& p_filepath );
    ~RenderStatesSet( void );

    static void SetRootPath( const dsstring& p_path );

    void AddRenderStateIn( const RenderState& p_renderstate );
    void AddRenderStateOut( const RenderState& p_renderstate );

    void UpdateRenderStateIn( int p_index,const RenderState& p_renderstate );
    void UpdateRenderStateOut( int p_index, const RenderState& p_renderstate );

    bool LoadFromFile( void );

};

}
}
#endif