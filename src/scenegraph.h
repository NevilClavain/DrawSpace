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

#ifndef _SCENEGRAPH_H_
#define _SCENEGRAPH_H_

#include "pass.h"

namespace DrawSpace
{
class Scenegraph : public Core::TransformQueue
{
protected:
    std::map<dsstring, Pass*>                   m_passes;
    //Core::TransformNode*                      m_camera;

    dsstring                                    m_current_camera;
    std::map<dsstring, Core::TransformNode*>    m_cameras_list;

    Utils::Matrix                               m_view;

public:
    Scenegraph( void );
    virtual ~Scenegraph( void );

    void RegisterPass( Pass* p_pass );
    bool RegisterNode( Core::TransformNode* p_node );	
    Pass* GetPass( const dsstring& p_passname );


    Core::TransformNode* GetCurrentCamera( void );
    bool SetCurrentCamera( const dsstring& p_nodename );

    void GetCurrentCameraView( Utils::Matrix& p_view );
    void GetCurrentCameraTranform( Utils::Matrix& p_mat );

    void ComputeTransformations( Utils::TimeManager& p_timemanager );

    std::map<dsstring, Core::TransformNode*>& GetCamerasList( void );

};
}
#endif
